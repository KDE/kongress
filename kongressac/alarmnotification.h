/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
