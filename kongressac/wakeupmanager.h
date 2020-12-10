/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WAKEUPMANAGER_H
#define WAKEUPMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QVariantMap>

class WakeupBackend;

class WakeupManager : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.PowerManagement")

public:
    explicit WakeupManager(QObject *parent = nullptr);

    /**
     * @brief Schedule a wake-up at the time given
     */
    void scheduleWakeup(const QDateTime wakeupAt);

    /**
     * @return True if there is a backend that offers wake-up features
     */
    bool active() const;

Q_SIGNALS:
    /**
     * @brief To be emited when the parent should take over and manage the wake-up
     *
     */
    void wakeupAlarmClient();

public Q_SLOTS:
    /**
     * @return Handles a wake-up
     */
    void wakeupCallback(const int cookie);

    /**
     * @return Clear a scheduled wakeup
     */
    void removeWakeup(const int cookie);

private:
    WakeupBackend *m_wakeup_backend;
    int m_cookie;
    QVariantMap m_callback_info;
};
#endif //WAKEUPMANAGER_H
