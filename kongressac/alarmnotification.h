/*
 *  Copyright (c) 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  As a special exception, permission is given to link this program
 *  with any edition of Qt, and distribute the resulting executable,
 *  without including the source code for Qt in the source distribution.
 */

#ifndef ALARMNOTIFICATION_H
#define ALARMNOTIFICATION_H

#include <KNotification>
#include <QDateTime>

class NotificationHandler;

/**
 * @brief The alarm notification that should be displayed. It is a wrapper of a KNotification enhanced with alarm properties, like uid and remind time
 *
 */
class AlarmNotification : public QObject
{
    Q_OBJECT
public:
    explicit AlarmNotification(NotificationHandler *handler, const QString &uid);

    /**
     * @brief Sends the notification so as to be displayed
     */
    void send() const;

    /**
     * @return The uid of the Incidence of the alarm of the notification
     */
    QString uid() const;

    /**
     * @brief The text of the notification that should be displayed
     */
    QString text() const;

    /**
     * @brief Sets the to-be-displayed text of the notification
     */
    void setText(const QString &alarmText);

private:
    KNotification *m_notification;
    QString m_uid;
    NotificationHandler *m_notification_handler;
};
#endif
