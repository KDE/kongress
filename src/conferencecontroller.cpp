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

    const QUrl conferencesUrl {QStringLiteral("https://diogenis.mooo.com") + fileName};

    QNetworkRequest request {conferencesUrl};
    auto nm = new QNetworkAccessManager {this};
    nm->get(request);

    connect(nm, &QNetworkAccessManager::finished, [this, nm](QNetworkReply * reply) {
        if (reply->error() == QNetworkReply::NoError) {
            if (m_conferences_file == nullptr || !m_conferences_file->open(QIODevice::WriteOnly)) {
                qDebug() << "Cannot open conferences file" << m_conferences_file->errorString();
            } else {
                m_conferences_file->write(reply->readAll());
                m_conferences_file->close();
            }
        }
        reply->deleteLater();
        nm->deleteLater();
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
