/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "calendarcontroller.h"
#include <QDebug>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <KLocalizedString>
#include <KConfig>
#include <KConfigGroup>

class CalendarController::Private
{
public:
    Private() : config {"kongressrc"}  {};
    KConfig config;
};

CalendarController::CalendarController(QObject *parent)
    : QObject {parent}, m_storages {QMap<QString, KCalendarCore::FileStorage::Ptr> {}}, m_calendars {QMap<QString, KCalendarCore::MemoryCalendar::Ptr>{}}, d {new Private}
{
    loadSavedConferences();
}

QString CalendarController::calendars() const
{
    return d->config.group("general").readEntry("calendars", QString {});
}

void CalendarController::removeCalendarFromConfig(const QString &calendarId)
{
    d->config.reparseConfiguration();
    auto calendarsList = d->config.group("general").readEntry("calendars", QString {}).split(";");
    if (calendarsList.contains(calendarId)) {
        qDebug() << "Removing calendar " << calendarId;
        calendarsList.removeAll(calendarId);

        d->config.deleteGroup(calendarId);
        d->config.group("general").writeEntry("calendars", calendarsList.join(";"));
        d->config.sync();

        Q_EMIT calendarsChanged();
    }
}

QString CalendarController::calendarFile(const QString &calendarId)
{
    if (d->config.hasGroup(calendarId) && d->config.group(calendarId).hasKey("file")) {
        return  d->config.group(calendarId).readEntry("file");
    }
    d->config.group(calendarId).writeEntry("file", filenameToPath(calendarId));
    d->config.sync();

    return filenameToPath(calendarId);
}

QString CalendarController::filenameToPath(const QString &calendarId)
{
    auto basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir baseFolder {basePath};
    baseFolder.mkpath(QStringLiteral("."));

    return basePath + "/kongress_" + calendarId + ".ics";
}

KCalendarCore::MemoryCalendar::Ptr CalendarController::createLocalCalendar(const QString &calendarId, const QByteArray &timeZoneId)
{
    auto m_fullpath = calendarFile(calendarId);
    auto tzId = QTimeZone::availableTimeZoneIds().contains(timeZoneId) ? timeZoneId : QTimeZone::systemTimeZoneId();

    KCalendarCore::MemoryCalendar::Ptr calendar {new KCalendarCore::MemoryCalendar {tzId}};
    qDebug() << "Memory calendar " << calendarId << "(local) with timezone " << tzId << " has been created";
    KCalendarCore::FileStorage::Ptr storage {new KCalendarCore::FileStorage {calendar}};
    storage->setFileName(m_fullpath);
    QFile calendarFile {m_fullpath};

    if (!calendarFile.exists()) {
        qDebug() << "Creating file" << storage->save();
    }

    if (storage->load()) {
        m_storages[calendarId] = storage;
        m_calendars[calendarId] = calendar;
    }

    Q_EMIT calendarsChanged();

    return calendar;
}

void CalendarController::deleteCalendar(const QString &calendarId)
{
    qDebug() << "Deleting calendar " << calendarId;

    auto fileName = m_storages[calendarId]->fileName();

    QFile calendarFile {fileName};

    if (calendarFile.exists()) {
        calendarFile.remove();
    }

    m_storages.remove(calendarId);
    m_calendars.remove(calendarId);

    removeCalendarFromConfig(calendarId);

    Q_EMIT calendarsChanged();
}

bool CalendarController::save(const QString &calendarId)
{
    if (m_storages[calendarId]->save()) {
        Q_EMIT calendarsChanged();

        return true;
    }

    return false;
}

void CalendarController::createCalendarFromUrl(const QString &calendarId, const QUrl &url, const QByteArray &timeZoneId)
{
    if (calendarId.isEmpty() || url.isEmpty()) {
        return;
    }
    qDebug() << "createCalendarFromUrl: calendarId " << calendarId << ", url" << url;
    Q_EMIT downlading(calendarId, true);

    auto filePath = calendarFile(calendarId);

    QNetworkRequest request {url};
    auto nm = new QNetworkAccessManager {this};
    nm->get(request);

    connect(nm, &QNetworkAccessManager::finished, [this, nm, filePath, calendarId, timeZoneId](QNetworkReply * reply) {
        if (reply->error() == QNetworkReply::NoError) {
            if (saveToDisk(filePath, reply->readAll())) {
                downloadFinished(calendarId, timeZoneId, filePath);
            }
        }
        reply->deleteLater();
        nm->deleteLater();
        Q_EMIT downlading(calendarId, false);
    });

}

void CalendarController::downloadFinished(const QString &calendarId, const QByteArray &timeZoneId, const QString &filePath)
{
    auto availableTimezones = QTimeZone::availableTimeZoneIds();
    auto tz = availableTimezones.contains(timeZoneId) ? timeZoneId : QTimeZone::systemTimeZoneId();
    addTzIdToConfig(calendarId, tz);

    KCalendarCore::MemoryCalendar::Ptr calendar {new KCalendarCore::MemoryCalendar {tz}};
    qDebug() << "Memory calendar " << calendarId << " (online) with timezone " << tz << " has been created";
    KCalendarCore::FileStorage::Ptr storage {new KCalendarCore::FileStorage {calendar}};
    storage->setFileName(filePath);

    if (storage->load()) {
        m_storages[calendarId] = storage;
        m_calendars[calendarId] = calendar;
    }

    addConferenceToConfig(calendarId);

    Q_EMIT calendarsChanged();
    Q_EMIT calendarDownloaded(calendarId);
}

bool CalendarController::saveToDisk(const QString &filename, const QByteArray &data)
{
    QFile file {filename};
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << (QString {"Could not open %1 for writing: %2"}).arg(qPrintable(filename), qPrintable(file.errorString()));
        return false;
    }
    file.write(data);
    file.close();

    return true;
}

KCalendarCore::MemoryCalendar::Ptr CalendarController::memoryCalendar(const QString &calendarId) const
{
    if (m_calendars.contains(calendarId)) {
        return m_calendars[calendarId];
    }

    return nullptr;
}

void CalendarController::addConferenceToConfig(const QString &calendarId)
{

    const QStringList calendarLists {"conferenceCalendars", "favoritesCalendars"};

    for (const auto &calendarList : calendarLists) {
        const auto calendarName = (calendarList == "conferenceCalendars") ? calendarId : (QString {"%1_%2"}).arg("favorites", calendarId);

        if (d->config.group("general").readEntry(calendarList, QString {}).isEmpty()) {
            d->config.group("general").writeEntry(calendarList, calendarName);
            d->config.sync();

            return;
        }

        auto calendarsList = d->config.group("general").readEntry(calendarList, QString {}).split(";");
        if (!calendarsList.contains(calendarName)) {
            calendarsList.append(calendarName);
            d->config.group("general").writeEntry(calendarList, calendarsList.join(";"));
            d->config.sync();
        }
    }
}

void CalendarController::loadSavedConferences()
{
    auto onlineCalendarIds = d->config.group("general").readEntry("conferenceCalendars", QString {});
    if (onlineCalendarIds.isEmpty()) {
        return;
    }

    auto calendarsList = onlineCalendarIds.split(";");

    for (const auto &calendarId : calendarsList) {
        auto filePath = d->config.group(calendarId).readEntry("file", QString {});
        QFile calendarFile {filePath};

        if (!calendarFile.exists()) {
            continue;
        }

        auto tz = tzIdFromConfig(calendarId);
        KCalendarCore::MemoryCalendar::Ptr calendar {new KCalendarCore::MemoryCalendar {tz}};
        KCalendarCore::FileStorage::Ptr storage {new KCalendarCore::FileStorage {calendar}};
        storage->setFileName(filePath);

        if (storage->load()) {
            m_storages[calendarId] = storage;
            m_calendars[calendarId] = calendar;

            qDebug() << "Calendar " << calendarId << " loaded succesfully";
        }
    }
}

void CalendarController::addTzIdToConfig(const QString &calendarId, const QByteArray &timeZoneId)
{
    if (!(d->config.hasGroup(calendarId))) {
        return;
    }

    d->config.group(calendarId).writeEntry("timeZoneId", timeZoneId);
    d->config.sync();
}

QByteArray CalendarController::tzIdFromConfig(const QString &calendarId) const
{
    if (!(d->config.hasGroup(calendarId))) {
        return QByteArray {};
    }

    if (d->config.hasGroup(calendarId) && !(d->config.group(calendarId).hasKey("timeZoneId"))) {
        return QByteArray {};
    }

    return d->config.group(calendarId).readEntry("timeZoneId").toUtf8();
}
