/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "notificationhandler.h"
#include "alarmnotification.h"
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDebug>

NotificationHandler::NotificationHandler(QObject *parent)
    : QObject{parent}
    , m_active_notifications{QHash<QString, AlarmNotification *>()}
{
}

void NotificationHandler::addActiveNotification(const QString &uid, const QString &text)
{
    auto *notification = new AlarmNotification{this, uid};
    notification->setText(text);
    m_active_notifications[notification->uid()] = notification;
}

void NotificationHandler::sendNotifications()
{
    for (const auto &n : std::as_const(m_active_notifications)) {
        qDebug() << "sendNotifications:\tSending notification for alarm" << n->uid();
        n->send();
    }

    m_active_notifications.clear();
}

QHash<QString, AlarmNotification *> NotificationHandler::activeNotifications() const
{
    return m_active_notifications;
}

#include "moc_notificationhandler.cpp"
