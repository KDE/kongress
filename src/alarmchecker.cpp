/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "alarmchecker.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

AlarmChecker::AlarmChecker(QObject *parent) : QObject {parent}, m_active {false}
{
    m_interface = new QDBusInterface {QStringLiteral("org.kde.kongressac"), QStringLiteral("/kongressac"), QStringLiteral("org.kde.kongressac"), QDBusConnection::sessionBus(), this};

    if (m_interface->isValid()) {
        QDBusReply<int> reply = m_interface->call(QStringLiteral("active"));
        if (reply.isValid()) {
            setActive(reply.value());
        }
    }
}

void AlarmChecker::scheduleAlarmCheck()
{
    if (m_interface->isValid()) {
        m_interface->call(QStringLiteral("scheduleAlarmCheck"));
    }
}

int AlarmChecker::active()
{
    return m_active;
}

void AlarmChecker::setActive(const int activeReply)
{
    if (m_active != activeReply) {
        m_active = activeReply;
        Q_EMIT activeChanged();
    }
}
