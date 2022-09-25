/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CALALARMCLIENT_H
#define CALALARMCLIENT_H

#include <QDateTime>
#include <QObject>

class NotificationHandler;
class WakeupBackend;
class WakeupManager;

/**
 * @brief Client that orchestrates the parsing of calendars and the display of notifications for event alarms. It exposes a D-Bus Interface containing a set of callable methods.
 *
 */
class CalAlarmClient : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kongressac")

public:
    explicit CalAlarmClient(QObject *parent = nullptr);

    /**
     * @brief The Status of the checker that depends on the existence of a scheduling backend
     *
     */
    enum CheckerStatus {
        Inactive = 0,
        Active = 1
    };

    /**
     * @return The method that should be triggered by the wakeup backend
     */
    void wakeupCallback();

    // DBUS interface
    /**
     * @brief Quits the application
     *
     */
    Q_SCRIPTABLE void quit();

    /**
     * @brief Checks the calendars for event alarms
     *
     */
    Q_SCRIPTABLE void forceAlarmCheck();

    /**
     * @return Schedule alarm check
     */
    Q_SCRIPTABLE void scheduleAlarmCheck();

    /**
     * @return Check if kongressac can handle alarms
     */
    Q_SCRIPTABLE int active() const;

private:
    QString alarmText(const QString &uid) const;
    void checkAlarms();
    void saveLastCheckTime();
    QStringList calendarFileList() const;

    QDateTime m_last_checked;
    NotificationHandler *m_notification_handler;
    WakeupManager *m_wakeup_manager;
};
#endif
