/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "alarmchecker.h"

#ifndef Q_OS_ANDROID
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#endif

AlarmChecker::AlarmChecker(QObject *parent)
    : QObject{parent}
{
#ifndef Q_OS_ANDROID
    m_interface = new QDBusInterface{QStringLiteral("org.kde.kongressac"),
                                     QStringLiteral("/kongressac"),
                                     QStringLiteral("org.kde.kongressac"),
                                     QDBusConnection::sessionBus(),
                                     this};
#endif
}

void AlarmChecker::scheduleAlarmCheck()
{
#ifndef Q_OS_ANDROID
    m_interface->call(QStringLiteral("scheduleAlarmCheck"));
#endif
}

int AlarmChecker::active()
{
#ifndef Q_OS_ANDROID
    auto kongressacActive{false};

    QDBusReply<int> reply = m_interface->call(QStringLiteral("active"));
    if (reply.isValid()) {
        kongressacActive = reply.value();
    }

    return kongressacActive;
#else
    return false;
#endif
}

#include "moc_alarmchecker.cpp"
