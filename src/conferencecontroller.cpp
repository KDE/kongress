/*
 * Copyright (C) 2020 Dimitris Kardarakos
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

#include "conferencecontroller.h"
#include "settingscontroller.h"
#include "conference.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KIO/StoredTransferJob>

class ConferenceController::Private
{
public:
    Private() : config {"kongressrc"}
    {};
    KConfig config;
};

ConferenceController::ConferenceController(QObject *parent) : QObject {parent}, m_active_conference {nullptr}, m_conferences_file {new QFile {}}, d {new Private}
{
    if (!defaultConferenceId().isEmpty()) {
        loadConferences();
    }

    connect(this, &ConferenceController::conferencesLoaded, [this]() {
        activateDefaultConference();
    });
}

QObject *ConferenceController::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new ConferenceController;
}

QVector<Conference *> ConferenceController::conferences() const
{
    return m_conferences;
}

void ConferenceController::loadConferences()
{
    if (!m_conferences.isEmpty()) {
        qDeleteAll(m_conferences.begin(), m_conferences.end());
        m_conferences.clear();
    }

    Q_EMIT downlading(true);

    const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    QLatin1String fileName {"/conference-data.json"};
    m_conferences_file = new QFile {dir + fileName};

    const QUrl conferencesUrl {QStringLiteral("https://autoconfig.kde.org/kongress") + fileName};
    auto *fetchJob = KIO::storedGet(conferencesUrl, KIO::Reload, KIO::HideProgressInfo);

    connect(fetchJob, &KIO::StoredTransferJob::result, this, [this, fetchJob]() {

        if (fetchJob->error() == 0) {
            if (m_conferences_file == nullptr || !m_conferences_file->open(QIODevice::WriteOnly)) {
                qDebug() << "Cannot open conferences file" << m_conferences_file->errorString();
            }
            m_conferences_file->write(fetchJob->data());
            m_conferences_file->close();
        }
        Q_EMIT downlading(false);
        loadConferencesFromFile(*m_conferences_file);
    });
}

void ConferenceController::loadConferencesFromFile(QFile &jsonFile)
{
    if (!jsonFile.exists()) {
        return;
    }

    QString data;

    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        data = jsonFile.readAll();
        jsonFile.close();
    }

    auto jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    QVariantList jsonVarList;

    if (!jsonDoc.isEmpty() && jsonDoc.isArray()) {
        auto jsonArray = jsonDoc.array();
        jsonVarList = jsonArray.toVariantList();
    }

    for (const QVariant &jsonVar : qAsConst(jsonVarList)) {
        loadConference(jsonVar.toJsonObject());
    }

    Q_EMIT conferencesLoaded();
}

void ConferenceController::loadConference(const QJsonObject &jsonObj)
{
    auto conference = new Conference {this};

    conference->setId(jsonObj["id"].toString());
    conference->setName(jsonObj["name"].toString());
    conference->setDescription(jsonObj["description"].toString());
    conference->setIcalUrl(jsonObj["icalUrl"].toString());
    auto jsonDays = jsonObj["days"].toVariant();
    conference->setDays(jsonDays.toStringList());
    conference->setVenueImageUrl(jsonObj["venueImageUrl"].toString());
    conference->setVenueLatitude(jsonObj["venueLatitude"].toString());
    conference->setVenueLongitude(jsonObj["venueLongitude"].toString());
    conference->setVenueOsmUrl(jsonObj["venueOsmUrl"].toString());
    conference->setTimeZoneId(jsonObj["timeZoneId"].toString());

    m_conferences << conference;
}

Conference *ConferenceController::activeConference() const
{
    return m_active_conference;
}

void ConferenceController::activateConference(const QString &conferenceId)
{
    if (conferenceId.isEmpty()) {
        return;
    }

    for (const auto cf : qAsConst(m_conferences)) {
        if (cf->id() == conferenceId) {

            m_active_conference = cf;
            qDebug() << "activateConference: conference " << conferenceId << " activated";

            setDefaultConferenceId(conferenceId);

            Q_EMIT activeConferenceChanged();

            return;
        }
    }
}

void ConferenceController::activateDefaultConference()
{
    activateConference(defaultConferenceId());
}

QString ConferenceController::defaultConferenceId() const
{
    auto confId = d->config.group("general").readEntry("defaultConferenceId", QString {});
    d->config.sync();

    return confId;
}

void ConferenceController::setDefaultConferenceId(const QString &confId)
{
    d->config.group("general").writeEntry("defaultConferenceId", confId);
    d->config.sync();

    Q_EMIT defaultConferenceIdChanged();
}

void ConferenceController::clearActiveConference()
{
    m_active_conference = nullptr;
    setDefaultConferenceId(QString {});
    Q_EMIT activeConferenceChanged();
}
