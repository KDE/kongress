/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SETTINGS_CONTROLLER_H
#define SETTINGS_CONTROLLER_H

#include <QObject>
#include <QQmlEngine>

class AlarmChecker;

class SettingsController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool displayInLocalTimezone READ displayInLocalTimezone WRITE setDisplayInLocalTimezone NOTIFY displayInLocalTimezoneChanged)
    Q_PROPERTY(bool remindFavorites READ remindFavorites WRITE setRemindFavorites NOTIFY remindFavoritesChanged)
    Q_PROPERTY(int remindBeforeStart READ remindBeforeStart WRITE setRemindBeforeStart NOTIFY remindBeforeStartChanged)
    Q_PROPERTY(bool canAddReminder READ canAddReminder NOTIFY canAddReminderChanged)

public:
    explicit SettingsController(QObject *parent = nullptr);

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    bool displayInLocalTimezone() const;
    void setDisplayInLocalTimezone(const bool displayInLocalTimezone);

    bool remindFavorites() const;
    void setRemindFavorites(const bool remind);

    int remindBeforeStart() const;
    void setRemindBeforeStart(const int remindBeforeStart);

    bool canAddReminder() const;

Q_SIGNALS:
    void displayInLocalTimezoneChanged();
    void remindFavoritesChanged();
    void remindBeforeStartChanged();
    void canAddReminderChanged();

private:
    class Private;
    Private *d;

    AlarmChecker *m_alarm_checker;
};

#endif
