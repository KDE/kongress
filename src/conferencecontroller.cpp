/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "conferencecontroller.h"
#include "settingscontroller.h"
#include "conference.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <KSharedConfig>
#include <KConfigGroup>

using namespace Qt::Literals::StringLiterals;

class ConferenceController::Private
{
public:
    Private() : config {u"kongressrc"_s}
    {};
    KConfig config;
    QNetworkAccessManager *nam = nullptr;
};

ConferenceController::ConferenceController(QObject *parent) : QObject {parent}, m_active_conference {nullptr}, m_conferences_file {new QFile {}}, d {new Private}
{
    connect(this, &ConferenceController::conferencesLoaded, [this]() {
        activateDefaultConference();
    });
}

void ConferenceController::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    d->nam = nam;
    if (!defaultConferenceId().isEmpty()) {
        loadConferences();
    }
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
    QNetworkRequest request {conferencesUrl};
    auto reply = d->nam->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            if (m_conferences_file == nullptr || !m_conferences_file->open(QIODevice::WriteOnly)) {
                qDebug() << "Cannot open conferences file" << m_conferences_file->errorString();
            } else {
                m_conferences_file->write(reply->readAll());
                m_conferences_file->close();
            }
        }
        reply->deleteLater();
        Q_EMIT downlading(false);
        loadConferencesFromFile(*m_conferences_file);
    });
}

void ConferenceController::loadConferencesFromFile(QFile &jsonFile)
{
    if (!jsonFile.exists()) {
        return;
    }

    QByteArray data;

    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        data = jsonFile.readAll();
        jsonFile.close();
    }

    auto jsonDoc = QJsonDocument::fromJson(data);
    QVariantList jsonVarList;

    if (!jsonDoc.isEmpty() && jsonDoc.isArray()) {
        auto jsonArray = jsonDoc.array();
        jsonVarList = jsonArray.toVariantList();
    }

    for (const QVariant &jsonVar : std::as_const(jsonVarList)) {
        loadConference(jsonVar.toJsonObject());
    }

    Q_EMIT conferencesLoaded();
}

void ConferenceController::loadConference(const QJsonObject &jsonObj)
{
    auto conferenceId = jsonObj["id"_L1].toString();

    for (const auto cf : std::as_const(m_conferences)) {
        if (cf->id() == conferenceId) {
            qDebug() << "Conference already loaded";
            return;
        }
    }

    auto conference = new Conference {this};
    conference->setId(conferenceId);
    conference->setName(jsonObj["name"_L1].toString());
    conference->setDescription(jsonObj["description"_L1].toString());
    conference->setIcalUrl(jsonObj["icalUrl"_L1].toString());
    auto jsonDays = jsonObj["days"_L1].toVariant();
    conference->setDays(jsonDays.toStringList());
    conference->setVenueImageUrl(jsonObj["venueImageUrl"_L1].toString());
    conference->setVenueLatitude(jsonObj["venueLatitude"_L1].toString());
    conference->setVenueLongitude(jsonObj["venueLongitude"_L1].toString());
    conference->setVenueOsmUrl(jsonObj["venueOsmUrl"_L1].toString());
    conference->setTimeZoneId(jsonObj["timeZoneId"_L1].toString());

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

    for (const auto cf : std::as_const(m_conferences)) {
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
    auto confId = d->config.group(u"general"_s).readEntry("defaultConferenceId", QString {});
    d->config.sync();

    return confId;
}

void ConferenceController::setDefaultConferenceId(const QString &confId)
{
    d->config.group(u"general"_s).writeEntry("defaultConferenceId", confId);
    d->config.sync();

    Q_EMIT defaultConferenceIdChanged();
}

void ConferenceController::clearActiveConference()
{
    m_active_conference = nullptr;
    setDefaultConferenceId(QString {});
    Q_EMIT activeConferenceChanged();
}

#include "moc_conferencecontroller.cpp"
