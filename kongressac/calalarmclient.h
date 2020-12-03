/*
  This program used korgac as a starting point. korgac can be found here: https://cgit.kde.org/korganizer.git/tree/korgac. It has been created by Cornelius Schumacher.

  Copyright (c) 2020 Dimitris Kardarakos <dimkard@posteo.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/
#ifndef CALALARMCLIENT_H
#define CALALARMCLIENT_H

#include <QTimer>
#include <QDateTime>

class AlarmsModel;
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

    AlarmsModel *m_alarms_model;
    QDateTime m_last_checked;
    NotificationHandler *m_notification_handler;
    WakeupManager *m_wakeup_manager;
};
#endif
