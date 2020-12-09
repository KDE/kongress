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

AlarmChecker::AlarmChecker(QObject *parent) : QObject {parent}
{
    m_interface = new QDBusInterface {QStringLiteral("org.kde.kongressac"), QStringLiteral("/kongressac"), QStringLiteral("org.kde.kongressac"), QDBusConnection::sessionBus(), this};
}

void AlarmChecker::scheduleAlarmCheck()
{
    m_interface->call(QStringLiteral("scheduleAlarmCheck"));
}

int AlarmChecker::active()
{
    auto kongressacActive {false};

    QDBusReply<int> reply = m_interface->call(QStringLiteral("active"));
    if (reply.isValid()) {
        kongressacActive = reply.value();
    }

    return kongressacActive;
}
