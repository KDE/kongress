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

#ifndef CONFERENCE_CONTROLLER_H
#define CONFERENCE_CONTROLLER_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QJsonObject>

class Conference;

class ConferenceController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Conference *activeConference READ activeConference NOTIFY activeConferenceChanged)
    Q_PROPERTY(QString defaultConferenceId READ defaultConferenceId WRITE setDefaultConferenceId NOTIFY defaultConferenceIdChanged)

public:
    explicit ConferenceController(QObject *parent = nullptr);

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
