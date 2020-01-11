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

ConferenceController::ConferenceController(QObject* parent) : QObject(parent)
{
    loadConferences();
}

QVector<Conference*> ConferenceController::conferences() const
{
    return m_conferences;
}

Conference* ConferenceController::conference(const QString& conferenceId) const
{
    for(auto confr : m_conferences)
    {
        if(confr->id() == conferenceId) return confr;
    }

    return new Conference();
}

void ConferenceController::writeConference(const Conference *const conference)
{
    //TODO: Implement
    qDebug() << "Saving conference" << conference->id() << " to configuration file";
}

void ConferenceController::loadConferences()
{
    auto config =  KSharedConfig::openConfig();
    auto loadPredefined = config->group("general").readEntry("loadPredefined", QString());
    QFile preconfiguredFile("://ConferenceData.json");

    if(loadPredefined.isEmpty())
    {
        config->group("general").writeEntry("loadPredefined","yes");
        config->sync();
        loadConferencesFromFile(preconfiguredFile);
    }
    else if(loadPredefined == "yes")
    {
        loadConferencesFromFile(preconfiguredFile);
    }

    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir baseFolder(basePath);
    baseFolder.mkpath(QStringLiteral("."));
    QFile userDataFile(QString("%1%2%3").arg(basePath).arg(QString("/")).arg(QString("ConferenceUserData.json")));

    loadConferencesFromFile(userDataFile);
    conferencesChanged();
}

void ConferenceController::loadConferencesFromFile(QFile& jsonFile)
{
    if(!jsonFile.exists())
    {
        return;
    }

    QString data;

    if(jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        data = jsonFile.readAll();
        jsonFile.close();
    }

    auto jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    QVariantList jsonVarList;

    if(!jsonDoc.isEmpty() && jsonDoc.isArray())
    {
        auto jsonArray = jsonDoc.array();
        jsonVarList = jsonArray.toVariantList();
    }

    for(auto jsonVar : jsonVarList)
    {
        loadConference(jsonVar.toJsonObject());
    }
}

void ConferenceController::loadConference(const QJsonObject& jsonObj)
{
    qDebug() << "Loading conference " << jsonObj["id"].toString();

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

    m_conferences << conference;
}
