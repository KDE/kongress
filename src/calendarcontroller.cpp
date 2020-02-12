/*
 * Copyright (C) 2019-2020 Dimitris Kardarakos
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "calendarcontroller.h"

#include <KLocalizedString>
#include <KConfig>
#include <KConfigGroup>
#include <QDebug>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

class CalendarController::Private
{
public:
    Private()
        : config("kongressrc")
    {};
    KConfig config;
};

CalendarController::CalendarController(QObject* parent)
    : QObject(parent), m_storages(QMap<QString, FileStorage::Ptr>()), m_calendars(QMap<QString, MemoryCalendar::Ptr>()), m_downloadManager(new DonwloadManager), d(new Private)
{
    loadSavedConferences();
    connect(&(m_downloadManager->networkManager), SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}

CalendarController::~CalendarController()
{
    delete d;
}

QString CalendarController::calendars() const
{
   return d->config.group("general").readEntry("calendars", QString());
}

QVariantMap CalendarController::canAddCalendar(const QString& calendar)
{
    QVariantMap result;
    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    QRegExp invalidChars("[\\;\\\\/<>:\\?\\*|\"\']");
    if(calendar.contains(invalidChars))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(i18n("Calendar name contains invalid characters"));
        return result;
    }

    if(d->config.group("general").readEntry("calendars", QString()).isEmpty())
    {
        return result;
    }

    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");

    if(calendarsList.contains(calendar))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(i18n("Calendar already exists"));
        return result;
    }

    return result;
}

QVariantMap CalendarController::addCalendar(const QString & calendar)
{
    QVariantMap result;
    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    QVariantMap canAddResult = canAddCalendar(calendar);

    if(!(canAddResult["success"].toBool()))
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(canAddResult["reason"].toString());
        return result;
    }

    if(d->config.group("general").readEntry("calendars", QString()).isEmpty())
    {
        d->config.group("general").writeEntry("calendars", calendar);
        d->config.sync();

        return result;
    }

    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");
    calendarsList.append(calendar);
    d->config.group("general").writeEntry("calendars", calendarsList.join(";"));
    d->config.sync();

    Q_EMIT calendarsChanged();

    return result;
}

void CalendarController::removeCalendarFromConfig(const QString& calendarId)
{
    d->config.reparseConfiguration();
    QStringList calendarsList = d->config.group("general").readEntry("calendars", QString()).split(";");
    if(calendarsList.contains(calendarId))
    {
        qDebug() << "Removing calendar " << calendarId;
        calendarsList.removeAll(calendarId);

        d->config.deleteGroup(calendarId);
        d->config.group("general").writeEntry("calendars", calendarsList.join(";"));
        d->config.sync();

        Q_EMIT calendarsChanged();
    }
}

QString CalendarController::calendarFile(const QString& calendarId)
{
    if(d->config.hasGroup(calendarId) && d->config.group(calendarId).hasKey("file"))
    {
        return  d->config.group(calendarId).readEntry("file");
    }
    d->config.group(calendarId).writeEntry("file", filenameToPath(calendarId));
    d->config.sync();

    return filenameToPath(calendarId);
}

QString CalendarController::filenameToPath(const QString& calendarId)
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir baseFolder(basePath);
    baseFolder.mkpath(QStringLiteral("."));

    return basePath + "/kongress_" + calendarId + ".ics";
}

MemoryCalendar::Ptr CalendarController::createLocalCalendar(const QString& calendarId, const QByteArray& timeZoneId)
{
    auto m_fullpath = calendarFile(calendarId);
    auto tzId = QTimeZone::availableTimeZoneIds().contains(timeZoneId) ? timeZoneId : QTimeZone::systemTimeZoneId();

    MemoryCalendar::Ptr calendar(new MemoryCalendar(tzId));
    FileStorage::Ptr storage(new FileStorage(calendar));
    storage->setFileName(m_fullpath);
    QFile calendarFile(m_fullpath);

    if(!calendarFile.exists())
    {
        qDebug() << "Creating file" << storage->save();
    }

    if(storage->load())
    {
        m_storages[calendarId] = storage;
        m_calendars[calendarId] = calendar;
    }

    Q_EMIT calendarsChanged();

    return calendar;
}

QVariantMap CalendarController::importCalendar(const QString& calendarId, const QString& sourcePath)
{
    QVariantMap result;
    result["success"] = QVariant(false);

    MemoryCalendar::Ptr calendar(new MemoryCalendar(QTimeZone::systemTimeZoneId()));
    FileStorage::Ptr storage(new FileStorage(calendar));

    QVariantMap canCreateCheck = canCreateFile(calendarId);

    if(!(canCreateCheck["success"].toBool()))
    {
        result["reason"] = QVariant(canCreateCheck["reason"].toString());

        return result;
    }

    storage->setFileName(sourcePath);

    if(!(storage->load()))
    {
        result["reason"] = QVariant(QString(i18n("The calendar file is not valid")));

        return result;
    }

    storage->setFileName(canCreateCheck["targetPath"].toString());

    if(!(storage->save()))
    {
        result["reason"] = QVariant(QString(i18n("The calendar file cannot be saved")));

        return result;
    }

    m_storages[calendarId] = storage;
    m_calendars[calendarId] = calendar;

    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    return result;
}

void CalendarController::deleteCalendar(const QString& calendarId)
{
    qDebug() << "Deleting calendar " << calendarId;

    auto fileName = m_storages[calendarId]->fileName();

    QFile calendarFile(fileName);

    if(calendarFile.exists())
    {
        calendarFile.remove();
    }

    m_storages.remove(calendarId);
    m_calendars.remove(calendarId);

    removeCalendarFromConfig(calendarId);

    Q_EMIT calendarsChanged();
}

bool CalendarController::save(const QString& calendarId)
{
    if(m_storages[calendarId]->save())
    {
        Q_EMIT calendarsChanged();

        return true;
    }

    return false;
}

QVariantMap CalendarController::canCreateFile(const QString& calendarId)
{
    QVariantMap result;
    result["success"] = QVariant(true);
    result["reason"] = QVariant(QString());

    QString targetPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/kongress_" + calendarId + ".ics" ;
    QFile calendarFile(targetPath);

    if(calendarFile.exists())
    {
        result["success"] = QVariant(false);
        result["reason"] = QVariant(QString(i18n("A calendar with the same name already exists")));

        return result;
    }

    result["targetPath"] = QVariant(QString(targetPath));

    return result;
}

void CalendarController::createCalendarFromUrl(const QString& calendarId, const QUrl& url, const QByteArray& timeZoneId)
{
    QNetworkRequest request(url);
    m_downloadManager->calendarId = calendarId;
    m_downloadManager->calendarTzId = timeZoneId;
    (m_downloadManager->networkManager).get(request);
}

void CalendarController::downloadFinished(QNetworkReply *reply)
{
    auto url = reply->url();

    if(reply->error())
    {
        qDebug() << "Download failed";
    }
    else
    {
        auto filePath = calendarFile(m_downloadManager->calendarId);

        if(saveToDisk(filePath, reply))
        {
            qDebug() << QString("Download of %1 succeeded (saved to %2)").arg(url.toEncoded().constData()).arg(qPrintable(filePath));

            auto availableTimezones = QTimeZone::availableTimeZoneIds();
            auto tz = availableTimezones.contains(m_downloadManager->calendarTzId) ? m_downloadManager->calendarTzId : QTimeZone::systemTimeZoneId();
            addTzIdToConfig(m_downloadManager->calendarId, tz);

            MemoryCalendar::Ptr calendar(new MemoryCalendar(tz));
            FileStorage::Ptr storage(new FileStorage(calendar));
            storage->setFileName(filePath);

            if(storage->load())
            {
                m_storages[m_downloadManager->calendarId] = storage;
                m_calendars[m_downloadManager->calendarId] = calendar;
            }

            addConferenceToConfig(m_downloadManager->calendarId);

            Q_EMIT calendarsChanged();
            Q_EMIT calendarDownloaded(m_downloadManager->calendarId);
        }
    }

    reply->deleteLater();
}

bool CalendarController::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << QString("Could not open %1 for writing: %2").arg(qPrintable(filename)).arg(qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

MemoryCalendar::Ptr CalendarController::memoryCalendar(const QString& calendarId) const
{
    if(m_calendars.contains(calendarId))
    {
        return m_calendars[calendarId];
    }

    return nullptr;
}

void CalendarController::addConferenceToConfig(const QString& calendarId)
{
    if(d->config.group("general").readEntry("conferenceCalendars", QString()).isEmpty())
    {
        d->config.group("general").writeEntry("conferenceCalendars", calendarId);
        d->config.sync();

        return;
    }

    QStringList calendarsList = d->config.group("general").readEntry("conferenceCalendars", QString()).split(";");
    if(!calendarsList.contains(calendarId))
    {
        calendarsList.append(calendarId);
        d->config.group("general").writeEntry("conferenceCalendars", calendarsList.join(";"));
        d->config.sync();
    }
}

void CalendarController::loadSavedConferences()
{
    auto onlineCalendarIds = d->config.group("general").readEntry("conferenceCalendars", QString());
    if(onlineCalendarIds.isEmpty())
    {
        return;
    }

    QStringList calendarsList = onlineCalendarIds.split(";");

    for(const auto& calendarId : calendarsList)
    {
       auto filePath = d->config.group(calendarId).readEntry("file", QString());
       QFile calendarFile(filePath);

       if(!calendarFile.exists())
       {
           continue;
       }

       auto tz = tzIdFromConfig(calendarId);
       MemoryCalendar::Ptr calendar(new MemoryCalendar(tz));
       FileStorage::Ptr storage(new FileStorage(calendar));
       storage->setFileName(filePath);

       if(storage->load())
       {
           m_storages[calendarId] = storage;
           m_calendars[calendarId] = calendar;

           qDebug() << "Calendar " << calendarId << " loaded succesfully";
       }
    }
}

void CalendarController::addTzIdToConfig(const QString& calendarId, const QByteArray& timeZoneId)
{
    if(!(d->config.hasGroup(calendarId)))
    {
        return;
    }

    d->config.group(calendarId).writeEntry("timeZoneId", timeZoneId);
    d->config.sync();
}

QByteArray CalendarController::tzIdFromConfig(const QString& calendarId) const
{
    if(!(d->config.hasGroup(calendarId)))
    {
        return QByteArray();
    }

    if(d->config.hasGroup(calendarId) && !(d->config.group(calendarId).hasKey("timeZoneId")))
    {
        return QByteArray();
    }

    return d->config.group(calendarId).readEntry("timeZoneId").toUtf8();
}
