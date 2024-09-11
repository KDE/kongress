/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ALARM_CHECKER_H
#define ALARM_CHECKER_H

#include <QObject>

class QDBusInterface;

class AlarmChecker : public QObject
{
    Q_OBJECT

public:
    explicit AlarmChecker(QObject *parent = nullptr);

    /**
     * @brief Shedule the next alarm check
     *
     */
    void scheduleAlarmCheck();

    /**
     * @brief Check if the alarm service is active
     */
    int active();

private:
    QDBusInterface *m_interface = nullptr;
};
#endif // ALARM_CHECKER_H
