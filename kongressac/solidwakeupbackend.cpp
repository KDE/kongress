/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "solidwakeupbackend.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDateTime>
#include <QDebug>

using namespace Qt::Literals::StringLiterals;

SolidWakeupBackend::SolidWakeupBackend(QObject *parent)
    : WakeupBackend{parent}
{
    m_interface = new QDBusInterface{QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this};
}

void SolidWakeupBackend::clearWakeup(const QVariant &scheduledWakeup)
{
    m_interface->call(QStringLiteral("clearWakeup"), scheduledWakeup.toInt());
}

QVariant SolidWakeupBackend::scheduleWakeup(const QVariantMap &callbackInfo, const quint64 wakeupAt)
{
    auto scheduledAt = QDateTime::fromSecsSinceEpoch(wakeupAt);

    qDebug() << "SolidWakeupBackend::scheduleWakeup at" << scheduledAt << " epoch" << wakeupAt;

    if (m_interface->isValid()) {
        QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"),
                                                   callbackInfo["dbus-service"_L1].toString(),
                                                   QDBusObjectPath{callbackInfo["dbus-path"_L1].toString()},
                                                   wakeupAt);
        return reply.value();
    }

    return 0;
}

bool SolidWakeupBackend::isWakeupBackend() const
{
    auto callMessage = QDBusMessage::createMethodCall(m_interface->service(),
                                                      m_interface->path(),
                                                      QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                      QStringLiteral("Introspect"));
    QDBusReply<QString> result = QDBusConnection::sessionBus().call(callMessage);

    if (result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0) {
        return true;
    }

    return false;
}
