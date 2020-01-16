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

#include "eventcontroller.h"
#include "calendarcontroller.h"
#include "localcalendar.h"
#include <KCalendarCore/Event>
#include <KCalendarCore/MemoryCalendar>
#include <QDebug>

EventController::EventController(QObject* parent) : QObject(parent), m_cal_controller(nullptr) {}

EventController::~EventController() = default;

CalendarController * EventController::calendarController()
{
    return m_cal_controller;
}

void EventController::setCalendarController(CalendarController *const controller)
{
    m_cal_controller = controller;
}

void EventController::remove(LocalCalendar *calendar, const QVariantMap &eventData)
{
    if(calendar == nullptr)
    {
        qDebug() << "There is no calendar to delete event from";

        return;
    }

    qDebug() << "Deleting event from calendar " << calendar->calendarId();

    MemoryCalendar::Ptr memoryCalendar = calendar->memorycalendar();
    QString uid = eventData["uid"].toString();
    Event::Ptr event = memoryCalendar->event(uid);
    memoryCalendar->deleteEvent(event);

    bool deleted = false;
    if(m_cal_controller != nullptr)
    {
        deleted = m_cal_controller->save(calendar->calendarId());
        Q_EMIT calendar->eventsChanged();
    }

    qDebug() << "Event deleted: " << deleted;
}

int EventController::addEdit(LocalCalendar *calendar, const QVariantMap &eventData)
{
    if(calendar == nullptr)
    {
        qDebug() << "There is no calendar to add event to";

        return 500;
    }

    qDebug() << "\naddEdit:\tCreating event to calendar " << calendar->calendarId();

    MemoryCalendar::Ptr memoryCalendar = calendar->memorycalendar();
    QDateTime now = QDateTime::currentDateTime();
    QString uid = eventData["uid"].toString();
    QString summary = eventData["summary"].toString();

    Event::Ptr event = memoryCalendar->event(uid);

    if (event == nullptr)
    {
        event = Event::Ptr(new Event());
    }
    else
    {
        return 304;
    }

    QDateTime startDateTime = eventData["startDate"].toDateTime();
    QDateTime endDateTime = eventData["endDate"].toDateTime();
    bool allDayFlg= eventData["allDay"].toBool();

    event->setUid(eventData["uid"].toString());
    event->setDtStart(startDateTime);
    event->setDtEnd(endDateTime);
    event->setDescription(eventData["description"].toString());
    event->setCategories(eventData["categories"].toString());
    event->setSummary(summary);
    event->setAllDay(allDayFlg);
    event->setLocation(eventData["location"].toString());
    event->setUrl(eventData["url"].toString());

    //TODO: add alarms
    /*
    event->clearAlarms();
    QVariantList newAlarms = eventData["alarms"].value<QVariantList>();
    QVariantList::const_iterator itr = newAlarms.constBegin();

    while(itr != newAlarms.constEnd())
    {
        Alarm::Ptr newAlarm = event->newAlarm();
        QHash<QString, QVariant> newAlarmHashMap = (*itr).value<QHash<QString, QVariant>>();
        int startOffsetValue = newAlarmHashMap["startOffsetValue"].value<int>();
        int startOffsetType = newAlarmHashMap["startOffsetType"].value<int>();
        int actionType = newAlarmHashMap["actionType"].value<int>();

        qDebug() << "addEdit:\tAdding alarm with start offset value " << startOffsetValue;
        newAlarm->setStartOffset(Duration(startOffsetValue, static_cast<Duration::Type>(startOffsetType)));
        newAlarm->setType(static_cast<Alarm::Type>(actionType));
        newAlarm->setEnabled(true);
        newAlarm->setText((event->summary()).isEmpty() ?  event->description() : event->summary());
        ++itr;
    }
    */

    memoryCalendar->addEvent(event);

    bool merged = false;

    if(m_cal_controller != nullptr)
    {
        merged = m_cal_controller->save(calendar->calendarId());
        Q_EMIT calendar->eventsChanged();
    }

    qDebug() << "addEdit:\tEvent added/updated: " << merged;

    return 201;
}
