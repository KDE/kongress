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

    Q_PROPERTY(QString defaultConferenceId READ defaultConferenceId WRITE setDefaultConferenceId NOTIFY defaultConferenceIdChanged)
    Q_PROPERTY(Conference *activeConferenceInfo READ activeConferenceInfo NOTIFY activeConferenceInfoChanged)

public:
    explicit ConferenceController(QObject *parent = nullptr);

    QString defaultConferenceId() const;
    void setDefaultConferenceId(const QString &confId);

    Conference *activeConferenceInfo() const;

    QVector<Conference *> conferences() const;
    void writeConference(const Conference *const conference);
Q_SIGNALS:
    void conferencesChanged();
    void defaultConferenceIdChanged();
    void activeConferenceInfoChanged();
private:
    QVector<Conference *> m_conferences;
    Conference *m_activeConferenceInfo;
    void loadDefaultConference(const QString &conferenceId);

    class Private;
    Private *d;

    void loadConference(const QJsonObject &jsonObj);
    void loadConferences();
    void loadConferencesFromFile(QFile &jsonFile);
};
#endif
