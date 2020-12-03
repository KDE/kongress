/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef NOTIFICATIONHANDLER_H
#define NOTIFICATIONHANDLER_H

#include "alarmsmodel.h"
#include <QVariantMap>

class AlarmNotification;

/**
 * @brief Manages the creation and triggering of event alarm notifications
 *
 */
class NotificationHandler : public QObject
{
    Q_OBJECT
public:
    explicit NotificationHandler(QObject *parent);

    /**
     * @brief Parses the internal list of active notifications and triggers their sending
     */
    void sendNotifications();

    /**
     * @brief Creates an alarm notification object for the Incidence with \p uid. It sets the text to be displayed according to \p text. It adds this alarm notification to the internal list of active notifications (the list of notifications that should be sent at the next check).
     */
    void addActiveNotification(const QString &uid, const QString &text);

    /**
     * @return The time period to check for alarms
     */
    FilterPeriod period() const;

    /**
     * @brief Sets the time period to check for alarms
     */
    void setPeriod(const FilterPeriod &checkPeriod);

    /**
     * @return The list of active notifications. It is the set of notification that should be sent at the next check
     */
    QHash<QString, AlarmNotification *> activeNotifications() const;

private:
    void sendActiveNotifications();

    QHash<QString, AlarmNotification *> m_active_notifications;
    FilterPeriod m_period;
};
#endif
