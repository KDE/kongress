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

#ifndef LOCALCALENDAR_H
#define LOCALCALENDAR_H

#include <QSharedPointer>
#include <QVariantMap>
#include <KCalendarCore/MemoryCalendar>

class CalendarController;

class LocalCalendar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap calendarInfo READ calendarInfo WRITE setCalendarInfo NOTIFY calendarInfoChanged)
    Q_PROPERTY(QSharedPointer<KCalendarCore::MemoryCalendar> memorycalendar READ memorycalendar NOTIFY memorycalendarChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(QString loadDateStr READ loadDateStr NOTIFY loadDateStrChanged)
    Q_PROPERTY(CalendarController *calendarController READ calendarController WRITE setCalendarController NOTIFY calendarControllerChanged)

public:
    explicit LocalCalendar(QObject *parent = nullptr);
    ~LocalCalendar() override;

    QVariantMap calendarInfo() const;
    void setCalendarInfo(const QVariantMap &calendarInfoMap);

    KCalendarCore::MemoryCalendar::Ptr memorycalendar() const;
    QStringList categories() const;

    CalendarController *calendarController() const;
    void setCalendarController(CalendarController *controller);

    QString calendarId() const;
    QString loadDateStr() const;
    Q_INVOKABLE void loadOnlineCalendar();

public Q_SLOTS:
    void onlineCalendarReady(const QString &calendarId);

Q_SIGNALS:
    void memorycalendarChanged();
    void calendarInfoChanged();
    void categoriesChanged();
    void eventsChanged();
    void loadDateStrChanged();
    void calendarControllerChanged();

private:
    QVariantMap m_calendarInfo;
    KCalendarCore::MemoryCalendar::Ptr m_calendar;
    CalendarController *m_cal_controller;
};
#endif // LOCALCALENDAR_H

