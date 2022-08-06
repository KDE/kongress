/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONFERENCE_CONTROLLER_H
#define CONFERENCE_CONTROLLER_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QJsonObject>
#include <QQmlEngine>
#include <QNetworkAccessManager>

class Conference;

class ConferenceController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Conference *activeConference READ activeConference NOTIFY activeConferenceChanged)
    Q_PROPERTY(QString defaultConferenceId READ defaultConferenceId WRITE setDefaultConferenceId NOTIFY defaultConferenceIdChanged)

public:
    explicit ConferenceController(QObject *parent = nullptr);
    void setNetworkAccessManager(QNetworkAccessManager *nam);

    Conference *activeConference() const;

    QString defaultConferenceId() const;
    void setDefaultConferenceId(const QString &confId);

    QVector<Conference *> conferences() const;
    Q_INVOKABLE void activateConference(const QString &conferenceId);
    Q_INVOKABLE void activateDefaultConference();
    Q_INVOKABLE void clearActiveConference();
public Q_SLOTS:
    void loadConferences();
Q_SIGNALS:
    void conferencesLoaded();
    void activeConferenceChanged();
    void defaultConferenceIdChanged();
    void downlading(const bool downlading);

private:
    void loadConference(const QJsonObject &jsonObj);
    void loadConferencesFromFile(QFile &jsonFile);
    QVector<Conference *> m_conferences;
    Conference *m_active_conference;
    QFile *m_conferences_file;

    class Private;
    Private *d;

};
#endif
