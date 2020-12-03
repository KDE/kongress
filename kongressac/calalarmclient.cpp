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

#include "calalarmclient.h"
#include "alarmnotification.h"
#include "alarmsmodel.h"
#include "notificationhandler.h"
#include "kongressacadaptor.h"
#include "solidwakeupbackend.h"
#include "wakeupmanager.h"
#include <QDebug>
#include <QVariantMap>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

CalAlarmClient::CalAlarmClient(QObject *parent)
    : QObject {parent}, m_alarms_model {new AlarmsModel {this}}, m_notification_handler {new NotificationHandler {this}}, m_wakeup_manager {new WakeupManager {this}}
{
    new KongressacAdaptor {this};
    auto dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/kongressac", this);

    KConfigGroup generalGroup {KSharedConfig::openConfig(), "General"};
    m_last_checked = generalGroup.readEntry("CalendarsLastChecked", QDateTime {});

    qDebug() << "CalAlarmClient:\tLastChecked:" << m_last_checked.toString("dd.MM.yyyy hh:mm:ss");

    checkAlarms();

    if ((m_wakeup_manager != nullptr) && (m_wakeup_manager->active())) {
        qDebug() << "CalAlarmClient: wake up manager offers an active backend with wakeup features";
        connect(m_wakeup_manager, &WakeupManager::wakeupAlarmClient, this, &CalAlarmClient::wakeupCallback);
        scheduleAlarmCheck();
    }
}

QStringList CalAlarmClient::calendarFileList() const
{
    QStringList filesList {};
    KConfigGroup kongressCfgGeneral {KSharedConfig::openConfig("kongressrc"), "general"};
    auto calendars = kongressCfgGeneral.readEntry("favoritesCalendars", QString {});
    auto calendarList = calendars.split(";");

    for (const auto &c : calendarList) {
        auto fileName = KSharedConfig::openConfig("kongressrc")->group(c).readEntry("file");

        if (!(fileName.isNull())) {
            filesList.append(fileName);
        }
    }

    qDebug() << "\ncalendarFileList:\tKongress calendars:" << filesList.join(",");

    return filesList;
}

void CalAlarmClient::checkAlarms()
{
    KConfigGroup cfg {KSharedConfig::openConfig(), "General"};

    if (!cfg.readEntry("Enabled", true)) {
        return;
    }
    auto checkFrom = m_last_checked.addSecs(1);
    m_last_checked = QDateTime::currentDateTime();

    qDebug() << "\ncheckAlarms:Check:" << checkFrom.toString() << " -" << m_last_checked.toString();

    FilterPeriod fPeriod {.from = checkFrom, .to = m_last_checked};
    m_alarms_model->setCalendarFiles(calendarFileList());
    m_alarms_model->setPeriod(fPeriod);
    m_notification_handler->setPeriod(fPeriod);

    const auto alarms = m_alarms_model->alarms();
    qDebug() << "checkAlarms:Alarms Found: " << alarms.count();

    KConfigGroup notificationsConfig {KSharedConfig::openConfig("kongressrc"), "notifications"};

    if (notificationsConfig.readEntry("remindFavorites", true)) {
        for (const auto &alarm : qAsConst(alarms)) {
            m_notification_handler->addActiveNotification(alarm->parentUid(), QString {"%1\n%2"}.arg(alarm->time().toString("hh:mm"), alarm->text()));
        }
        m_notification_handler->sendNotifications();
    }

    saveLastCheckTime();
}

void CalAlarmClient::saveLastCheckTime()
{
    KConfigGroup generalGroup {KSharedConfig::openConfig(), "General"};
    generalGroup.writeEntry("CalendarsLastChecked", m_last_checked);
    KSharedConfig::openConfig()->sync();
}

void CalAlarmClient::quit()
{
    saveLastCheckTime();
    qDebug("\nquit");
    qApp->quit();
}

void CalAlarmClient::forceAlarmCheck()
{
    checkAlarms();
    saveLastCheckTime();
}

QString CalAlarmClient::alarmText(const QString &uid) const
{
    AlarmsModel model {};
    model.setCalendarFiles(calendarFileList());
    model.setPeriod({.from = QDateTime {}, .to = QDateTime::currentDateTime()});
    const auto alarms = model.alarms();

    for (const auto &alarm : qAsConst(alarms)) {
        if (alarm->parentUid() == uid) {
            return alarm->text();
        }
    }

    return QString {};
}

void CalAlarmClient::scheduleAlarmCheck()
{
    if ((m_wakeup_manager == nullptr) || !(m_wakeup_manager->active())) {
        return;
    }

    AlarmsModel model {};
    model.setCalendarFiles(calendarFileList());
    model.setPeriod({.from =  m_last_checked.addSecs(1), .to = m_last_checked.addDays(1)});

    auto wakeupAt = model.firstAlarmTime();
    qDebug() << "scheduleAlarmCheck:" << "Shecdule next alarm check at" << wakeupAt.toString("dd.MM.yyyy hh:mm:ss");
    m_wakeup_manager->scheduleWakeup(wakeupAt.addSecs(1));
}

void CalAlarmClient::wakeupCallback()
{
    checkAlarms();
    scheduleAlarmCheck();
}

int CalAlarmClient::active() const
{
    if ((m_wakeup_manager != nullptr) && (m_wakeup_manager->active())) {
        return CalAlarmClient::CheckerStatus::Active;
    }

    return CalAlarmClient::CheckerStatus::Inactive;
}
