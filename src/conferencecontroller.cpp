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
    Private()
        : config("kongressrc")
    {};
    KConfig config;
};

ConferenceController::ConferenceController(QObject *parent) : QObject(parent), m_activeConferenceInfo { new Conference() }, mPredefinedConferencesFile { new QString() }, d { new Private() }
{
    loadConferences();
    loadDefaultConference(defaultConferenceId());
}

QVector<Conference *> ConferenceController::conferences() const
{
    return m_conferences;
}

void ConferenceController::writeConference(const Conference *const conference)
{
    //TODO: Implement
    qDebug() << "Saving conference" << conference->id() << " to configuration file";
}

void ConferenceController::loadConferences()
{
    if (!m_conferences.isEmpty()) {
        qDeleteAll(m_conferences.begin(), m_conferences.end());
        m_conferences.clear();
    }

    const QString dir { QStandardPaths::writableLocation(QStandardPaths::CacheLocation) };
    QDir().mkpath(dir);

    auto fileName = QLatin1String("/conference-data.json");
    *mPredefinedConferencesFile = dir + fileName;

    const QUrl conferencesUrl(QStringLiteral("https://autoconfig.kde.org/kongress") + fileName);
    auto *fetchJob = KIO::storedGet(conferencesUrl, KIO::Reload, KIO::HideProgressInfo);

    connect(fetchJob, &KIO::StoredTransferJob::result, this, [this, fetchJob]() {

        if (fetchJob->error() == 0) {
            QFile f { *mPredefinedConferencesFile };
            if (!f.open(QIODevice::WriteOnly)) {
                qDebug() << "Cannot open" << *mPredefinedConferencesFile << f.errorString();
            }
            f.write(fetchJob->data());
            f.close();
            loadConferencesFromFile(f);
        }
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

    for (auto jsonVar : jsonVarList) {
        loadConference(jsonVar.toJsonObject());
    }

    conferencesChanged();
}

void ConferenceController::loadConference(const QJsonObject &jsonObj)
{
    auto conference = new Conference();

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

QString ConferenceController::defaultConferenceId() const
{
    auto confId = d->config.group("general").readEntry("defaultConferenceId", QString());
    d->config.sync();

    return confId;
}

void ConferenceController::setDefaultConferenceId(const QString &confId)
{
    d->config.group("general").writeEntry("defaultConferenceId", confId);
    d->config.sync();

    Q_EMIT defaultConferenceIdChanged();

    loadDefaultConference(confId);
}

Conference *ConferenceController::activeConferenceInfo() const
{
    return m_activeConferenceInfo;
}

void ConferenceController::loadDefaultConference(const QString &conferenceId)
{
    if (conferenceId.isEmpty()) {
        return;
    }

    for (auto cf : m_conferences) {
        if (cf->id() == conferenceId) {
            m_activeConferenceInfo->setId(cf->id());
            m_activeConferenceInfo->setName(cf->name());
            m_activeConferenceInfo->setDescription(cf->description());
            m_activeConferenceInfo->setIcalUrl(cf->icalUrl());
            m_activeConferenceInfo->setDays(cf->days());
            m_activeConferenceInfo->setVenueImageUrl(cf->venueImageUrl());
            m_activeConferenceInfo->setVenueLatitude(cf->venueLatitude());
            m_activeConferenceInfo->setVenueLongitude(cf->venueLongitude());
            m_activeConferenceInfo->setVenueOsmUrl(cf->venueOsmUrl());
            m_activeConferenceInfo->setTimeZoneId(cf->timeZoneId());
        }
    }

    Q_EMIT activeConferenceInfoChanged();

}
