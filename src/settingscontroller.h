/*
 * Copyright (C) 2020 David Barchiesi <david@barchie.si>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
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
    Q_PROPERTY(int remindBeforeStart READ remindBeforeStart WRITE setRemindBeforeStart  NOTIFY remindBeforeStartChanged)
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
