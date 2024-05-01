/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "calalarmclient.h"
#include "alarmnotification.h"
#include "alarmsmodel.h"
#include "kongressacadaptor.h"
#include "notificationhandler.h"
#include "solidwakeupbackend.h"
#include "wakeupmanager.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDebug>

using namespace Qt::Literals::StringLiterals;

CalAlarmClient::CalAlarmClient(QObject *parent)
    : QObject{parent}
    , m_notification_handler{new NotificationHandler{this}}
    , m_wakeup_manager{new WakeupManager{this}}
{
    new KongressacAdaptor{this};
    auto dbus = QDBusConnection::sessionBus();
    dbus.registerObject(u"/kongressac"_s, this);

    KConfigGroup generalGroup{KSharedConfig::openConfig(), u"General"_s};
    m_last_checked = generalGroup.readEntry("CalendarsLastChecked", QDateTime{});

    qDebug() << "CalAlarmClient:\tLastChecked:" << m_last_checked;

    checkAlarms();

    if ((m_wakeup_manager != nullptr) && (m_wakeup_manager->active())) {
        qDebug() << "CalAlarmClient: wake up manager offers an active backend with wakeup features";
        connect(m_wakeup_manager, &WakeupManager::wakeupAlarmClient, this, &CalAlarmClient::wakeupCallback);
        scheduleAlarmCheck();
    }
}

QStringList CalAlarmClient::calendarFileList() const
{
    QStringList filesList{};
    KConfigGroup kongressCfgGeneral{KSharedConfig::openConfig(u"kongressrc"_s), u"general"_s};
    auto calendars = kongressCfgGeneral.readEntry("favoritesCalendars", QString{});
    auto calendarList = calendars.split(';'_L1);

    for (const auto &c : calendarList) {
        auto fileName = KSharedConfig::openConfig(u"kongressrc"_s)->group(c).readEntry("file");

        if (!(fileName.isNull())) {
            filesList.append(fileName);
        }
    }

    qDebug() << "\ncalendarFileList:\tKongress calendars:" << filesList.join(','_L1);

    return filesList;
}

void CalAlarmClient::checkAlarms()
{
    KConfigGroup cfg{KSharedConfig::openConfig(), u"General"_s};

    if (!cfg.readEntry("Enabled", true)) {
        return;
    }
    auto checkFrom = m_last_checked.addSecs(1);
    m_last_checked = QDateTime::currentDateTime();

    qDebug() << "\ncheckAlarms:Check:" << checkFrom << " -" << m_last_checked;

    FilterPeriod fPeriod{.from = checkFrom, .to = m_last_checked};
    AlarmsModel alarmsModel;
    alarmsModel.setCalendarFiles(calendarFileList());
    alarmsModel.setPeriod(fPeriod);

    const auto alarms = alarmsModel.alarms();
    qDebug() << "checkAlarms:Alarms Found: " << alarms.count();

    KConfigGroup notificationsConfig{KSharedConfig::openConfig(u"kongressrc"_s), u"notifications"_s};

    if (notificationsConfig.readEntry("remindFavorites", true)) {
        for (const auto &alarm : std::as_const(alarms)) {
            m_notification_handler->addActiveNotification(
                alarm->parentUid(),
                "%1\n%2"_L1.arg(alarm->time().toTimeZone(QTimeZone::systemTimeZone()).toString(u"hh:mm"), alarm->text()));
        }
        m_notification_handler->sendNotifications();
    }

    saveLastCheckTime();
}

void CalAlarmClient::saveLastCheckTime()
{
    KConfigGroup generalGroup{KSharedConfig::openConfig(), u"General"_s};
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
    AlarmsModel model{};
    model.setCalendarFiles(calendarFileList());
    model.setPeriod({.from = QDateTime{}, .to = QDateTime::currentDateTime()});
    const auto alarms = model.alarms();

    for (const auto &alarm : std::as_const(alarms)) {
        if (alarm->parentUid() == uid) {
            return alarm->text();
        }
    }

    return QString{};
}

void CalAlarmClient::scheduleAlarmCheck()
{
    if ((m_wakeup_manager == nullptr) || !(m_wakeup_manager->active())) {
        return;
    }

    AlarmsModel model{};
    model.setCalendarFiles(calendarFileList());
    model.setPeriod({.from = m_last_checked.addSecs(1), .to = m_last_checked.addDays(1)});

    auto wakeupAt = model.firstAlarmTime();
    qDebug() << "scheduleAlarmCheck:"
             << "Shecdule next alarm check at" << wakeupAt;
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

#include "moc_calalarmclient.cpp"
