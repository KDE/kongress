/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "alarmnotification.h"
#include "notificationhandler.h"
#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

AlarmNotification::AlarmNotification(NotificationHandler *handler, const QString &uid)
    : QObject{handler}
    , m_uid{uid}
    , m_notification_handler{handler}
{
    m_notification = new KNotification{u"alarm"_s};
}

void AlarmNotification::send() const
{
    m_notification->sendEvent();
}

QString AlarmNotification::uid() const
{
    return m_uid;
}

QString AlarmNotification::text() const
{
    return m_notification->text();
}

void AlarmNotification::setText(const QString &alarmText)
{
    m_notification->setText(alarmText);
}

#include "moc_alarmnotification.cpp"
