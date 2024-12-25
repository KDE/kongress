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
    Q_PROPERTY(LocalCalendar *calendar READ calendar WRITE setCalendar NOTIFY calendarChanged)
    Q_PROPERTY(QString eventUid READ eventUid WRITE setEventUid NOTIFY eventUidChanged)
    Q_PROPERTY(bool isFavorite READ isFavorite NOTIFY isFavoriteChanged)

public:
    enum EventCheck {
        NotExistsNotOverlapping = -1,
        Exists,
        NotExistsButOverlaps,
        NoCalendarExists
    };

    explicit EventController(QObject *parent = nullptr);

    LocalCalendar *calendar() const;
    void setCalendar(LocalCalendar *calendar);

    QString eventUid() const;
    void setEventUid(const QString &eventUid);

    bool isFavorite() const;

    CalendarController *calendarController();
    void setCalendarController(CalendarController *const controller);

    Q_INVOKABLE bool remove(const QVariantMap &event);
    Q_INVOKABLE QVariantMap addEdit(const QVariantMap &event);

Q_SIGNALS:
    void calendarControllerChanged();
    void calendarChanged();
    void eventUidChanged();
    void isFavoriteChanged();

private:
    /**
     * @brief Check if an event is already registered or overlapping events exist
     *
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
    QVariantMap eventCheck(const QVariantMap &event) const;

    CalendarController *m_cal_controller = nullptr;
    SettingsController *const m_settings_controller;

    LocalCalendar *m_calendar = nullptr;
    QString m_eventUid;
};
#endif
