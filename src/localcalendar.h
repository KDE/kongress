/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOCALCALENDAR_H
#define LOCALCALENDAR_H

#include <QSharedPointer>
#include <KCalendarCore/MemoryCalendar>

class CalendarController;
class AlarmChecker;

class LocalCalendar : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString calendarId READ calendarId WRITE setCalendarId NOTIFY calendarIdChanged)
    Q_PROPERTY(QString calendarTzId READ calendarTzId WRITE setCalendarTzId NOTIFY calendarTzIdChanged)
    Q_PROPERTY(QString calendarUrl READ calendarUrl WRITE setCalendarUrl NOTIFY calendarUrlChanged)
    Q_PROPERTY(int calendarType READ calendarType WRITE setCalendarType NOTIFY calendarTypeChanged)
    Q_PROPERTY(QSharedPointer<KCalendarCore::MemoryCalendar> memorycalendar READ memorycalendar NOTIFY memorycalendarChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(CalendarController *calendarController READ calendarController WRITE setCalendarController NOTIFY calendarControllerChanged)
    Q_PROPERTY(bool busyDownlading READ busyDownlading NOTIFY busyDownladingChanged)

public:
    enum CalendarType {
        None = 0,
        Conference = 1,
        Favorites = 2
    };

    explicit LocalCalendar(QObject *parent = nullptr);

    QString calendarId() const;
    void setCalendarId(const QString &calendarId);

    QString calendarTzId() const;
    void setCalendarTzId(const QString &tzId);

    QString calendarUrl() const;
    void setCalendarUrl(const QString &url);

    int calendarType() const;
    void setCalendarType(const int type);

    KCalendarCore::MemoryCalendar::Ptr memorycalendar() const;
    QStringList categories() const;

    bool busyDownlading() const;

    CalendarController *calendarController() const;
    void setCalendarController(CalendarController *const controller);

    Q_INVOKABLE void loadOnlineCalendar();

public Q_SLOTS:
    void onlineCalendarReady(const QString &calendarId);
    void setBusyStatus(const QString &calendarId, const bool downlading);

Q_SIGNALS:
    void memorycalendarChanged();
    void calendarIdChanged();
    void calendarTzIdChanged();
    void calendarUrlChanged();
    void calendarTypeChanged();
    void categoriesChanged();
    void eventsChanged();
    void calendarControllerChanged();
    void busyDownladingChanged();

private Q_SLOTS:
    void createCalendar();

private:
    QString m_calendar_id;
    QString m_calendar_tz_id;
    QString m_calendar_url;
    int m_calendar_type;
    KCalendarCore::MemoryCalendar::Ptr m_calendar;
    CalendarController *m_cal_controller;
    AlarmChecker *m_alarm_checker;
    bool m_busy_downloading;
};
#endif // LOCALCALENDAR_H
