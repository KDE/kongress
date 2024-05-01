/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "wakeupmanager.h"
#include "powermanagementadaptor.h"
#include "solidwakeupbackend.h"
#include <QDBusConnection>
#include <QDebug>

using namespace Qt::Literals::StringLiterals;

WakeupManager::WakeupManager(QObject *parent)
    : QObject{parent}
    , m_cookie{-1}
{
    m_callback_info = QVariantMap{{{u"dbus-service"_s, u"org.kde.kongressac"_s}, {u"dbus-path"_s, u"/wakeupmanager"_s}}};

    new PowerManagementAdaptor{this};
    m_wakeup_backend = new SolidWakeupBackend{this};

    auto dbus = QDBusConnection::sessionBus();
    dbus.registerObject(m_callback_info["dbus-path"_L1].toString(), this);
}

void WakeupManager::scheduleWakeup(const QDateTime wakeupAt)
{
    if (wakeupAt <= QDateTime::currentDateTime()) {
        qDebug() << "WakeupManager:"
                 << "Requested to schedule wake up at" << wakeupAt << "Can't chedule a wakeup in the past";
        return;
    }

    auto scheduledCookie = m_wakeup_backend->scheduleWakeup(m_callback_info, wakeupAt.toSecsSinceEpoch()).toInt();

    if (scheduledCookie > 0) {
        qDebug() << "WakeupManager: wake up has been scheduled, wakeup time:" << wakeupAt << "Received cookie" << scheduledCookie;

        if (m_cookie > 0) {
            removeWakeup(m_cookie);
        }

        m_cookie = scheduledCookie;
    }
}

void WakeupManager::wakeupCallback(const int cookie)
{
    qDebug() << "WakeupManager: awaken by cookie" << cookie;

    if (m_cookie == cookie) {
        m_cookie = -1;

        Q_EMIT wakeupAlarmClient();
    } else {
        qDebug() << "WakeupManager: the cookie is invalid";
    }
}

void WakeupManager::removeWakeup(const int cookie)
{
    qDebug() << "WakeupManager: clearing cookie" << cookie;

    m_wakeup_backend->clearWakeup(cookie);
    m_cookie = -1;
}

bool WakeupManager::active() const
{
    return m_wakeup_backend->isWakeupBackend();
}

#include "moc_wakeupmanager.cpp"
