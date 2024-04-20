/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EVENTCONTROLLER_H
#define EVENTCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <QVariantMap>

class CalendarController;
class LocalCalendar;
class SettingsController;

class EventController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CalendarController *calendarController READ calendarController WRITE setCalendarController NOTIFY calendarControllerChanged)

public:
    enum EventCheck { NotExistsNotOverlapping = -1, Exists, NotExistsButOverlaps, NoCalendarExists };

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
