/*
 * Copyright (C) 2018-2020 Dimitris Kardarakos
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

#ifndef EVENTCONTROLLER_H
#define EVENTCONTROLLER_H

#include <QObject>
#include <QVariantMap>
#include <QQmlEngine>

class CalendarController;
class LocalCalendar;
class SettingsController;

class EventController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CalendarController *calendarController READ calendarController WRITE setCalendarController NOTIFY calendarControllerChanged)

public:
    enum EventCheck {
        NotExistsNotOverlapping = -1,
        Exists,
        NotExistsButOverlaps,
        NoCalendarExists
    };

    explicit EventController(QObject *parent = nullptr);

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    CalendarController *calendarController();
    void setCalendarController(CalendarController *const controller);

    Q_INVOKABLE void remove(LocalCalendar *calendar, const QVariantMap &event);
    Q_INVOKABLE QVariantMap addEdit(LocalCalendar *calendar, const QVariantMap &event);
Q_SIGNALS:
    void calendarControllerChanged();

private:
    /**
     * @brief Check if an event is already registered or overlapping events exist
     *
     * @param calendar p_calendar: The calendar of the favorites
     * @param event p_event: The to-be-registered event
     * @return A QVariantMap with two members
     * int result:
     *   Set to NotExistsNotOverlapping if the event is not registered and no overlapping events exist
     *   Set to Exists if the event is already registered
     *   Set to NotExistsButOverlaps if the event is not registered but overlapping events exist
     *   Set to NoCalendarExists if calendar argument does not point at a calendar
     * QString events:
     *   If NotExistsNotOverlapping, a success message
     *   If Exists, an information message that the event already exists
     *   If NotExistsButOverlaps, a comma separated list of their subjects
     *   If NoCalendarExists, let the user know that an error has occured
     */
    QVariantMap eventCheck(LocalCalendar *calendar, const QVariantMap &event);

    CalendarController *m_cal_controller;
    SettingsController *m_settings_controller;

};
#endif
