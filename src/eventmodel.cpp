/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "eventmodel.h"
#include "localcalendar.h"
#include <KCalendarCore/CalFilter>
#include <KCalendarCore/MemoryCalendar>
#include <KLocalizedString>
#include <KCalendarCore/Sorting>
#include "settingscontroller.h"

EventModel::EventModel(QObject *parent) :
    QAbstractListModel {parent},
    m_events {KCalendarCore::Event::List {}},
    m_filterdt {QDate {}},
    m_category {QString {}},
    m_local_calendar {nullptr},
    m_settings_controller {new SettingsController}
{
    connect(this, &EventModel::filterdtChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::calendarChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::eventCategoryChanged, this, &EventModel::loadEvents);
}

QDate EventModel::filterdt() const
{
    return m_filterdt;
}

void EventModel::setFilterdt(const QDate &filterDate)
{
    m_filterdt = filterDate;

    Q_EMIT filterdtChanged();
}

LocalCalendar *EventModel::calendar() const
{
    return m_local_calendar;
}

void EventModel::setCalendar(LocalCalendar *const calendarPtr)
{
    m_local_calendar = calendarPtr;

    connect(m_local_calendar, &LocalCalendar::eventsChanged, this, &EventModel::loadEvents);

    Q_EMIT calendarChanged();
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    return {
        { Uid, "uid" },
        { EventStartDt, "eventStartDt" },
        { EventDt, "eventDt" },
        { ShiftedEventDt, "shiftedEventDt" },
        { ShiftedEventDtLocal, "shiftedEventDtLocal" },
        { ScheduleStartDt, "scheduleStartDt" },
        { AllDay, "allday" },
        { Description, "description" },
        { Summary, "summary" },
        { LastModified, "lastmodified" },
        { Location, "location" },
        { Categories, "categories" },
        { Priority, "priority" },
        { Created, "created" },
        { Secrecy, "secrecy" },
        { EventEndDt, "eventEndDt" },
        { ScheduleEndDt, "scheduleEndDt" },
        { Transparency, "transparency" },
        { RepeatPeriodType, "repeatType" },
        { RepeatEvery, "repeatEvery" },
        { RepeatStopAfter, "repeatStopAfter" },
        { IsRepeating, "isRepeating" },
        { EventCategories, "eventCategories" },
        { Url, "url" },
        { ShiftedStartEndDt, "shiftedStartEndDt" },
        { ShiftedStartEndDtLocal, "shiftedStartEndDtLocal" },
        { ShiftedStartEndTime, "shiftedStartEndTime" },
        { ShiftedStartEndTimeLocal, "shiftedStartEndTimeLocal" },
        { StartEndDt, "startEndDt" },
        { StartEndDtLocal, "startEndDtLocal" },
        { Overlapping, "overlapping" },
        { ConferenceTzId, "conferenceTzId" }
    };
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant {};
    }

    auto row = index.row();
    auto calendarTz = m_local_calendar->memorycalendar()->timeZone();
    auto startDtTime = m_events.at(row)->dtStart();
    auto endDtTime = m_events.at(row)->dtEnd();
    auto allDay = m_events.at(row)->allDay();

    switch (role) {
    case Uid :
        return m_events.at(row)->uid();
    case EventStartDt:
        return startDtTime;
    case EventDt:
        return startDtTime.toString("dddd d MMMM");
    case ShiftedEventDt: {
        startDtTime.setTimeZone(calendarTz);
        return startDtTime.toString("dddd d MMMM");
    }
    case ShiftedEventDtLocal: {
        startDtTime.setTimeZone(calendarTz);
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        return startDtTime.toString("dddd d MMMM");
    }
    case ScheduleStartDt: {
        startDtTime.setTimeZone(calendarTz);
        return startDtTime;
    }
    case AllDay:
        return allDay;
    case Description:
        return m_events.at(row)->description();
    case Summary:
        return m_events.at(row)->summary();
    case LastModified:
        return m_events.at(row)->lastModified();
    case Location:
        return m_events.at(row)->location();
    case Categories:
        return m_events.at(row)->categories();
    case Priority:
        return m_events.at(row)->priority();
    case Created:
        return m_events.at(row)->created();
    case Secrecy:
        return m_events.at(row)->secrecy();
    case EventEndDt:
        return endDtTime;
    case ScheduleEndDt: {
        auto endDtWConferenceTz = endDtTime;
        endDtWConferenceTz.setTimeZone(calendarTz);

        return endDtWConferenceTz;
    }
    case Transparency:
        return m_events.at(row)->transparency();
    case RepeatPeriodType:
        return repeatPeriodType(row);
    case RepeatEvery:
        return repeatEvery(row);
    case RepeatStopAfter:
        return repeatStopAfter(row);
    case IsRepeating:
        return m_events.at(row)->recurs();
    case EventCategories:
        return m_events.at(row)->categoriesStr();
    case Url:
        return m_events.at(row)->url();
    case ShiftedStartEndDt: {
        // Shift and convert time to the time zone of the conference
        startDtTime.setTimeZone(calendarTz);
        startDtTime = startDtTime.toTimeZone(calendarTz);
        endDtTime.setTimeZone(calendarTz);
        endDtTime = endDtTime.toTimeZone(calendarTz);

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case ShiftedStartEndDtLocal: {
        // Shift time to the time zone of the conference and convert to the system time zone
        startDtTime.setTimeZone(calendarTz);
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime.setTimeZone(calendarTz);
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case ShiftedStartEndTime: {
        // Shift time to the time zone of the conference
        startDtTime.setTimeZone(calendarTz);
        endDtTime.setTimeZone(calendarTz);

        return formatStartEndTime(startDtTime, endDtTime);
    }
    case ShiftedStartEndTimeLocal: {
        // Shift time to the conference time zone and convert time to the system time zone
        startDtTime.setTimeZone(calendarTz);
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime.setTimeZone(calendarTz);
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndTime(startDtTime, endDtTime);
    }
    case StartEndDt: {
        // Convert time to the time zone of the conference
        startDtTime = startDtTime.toTimeZone(calendarTz);
        endDtTime = endDtTime.toTimeZone(calendarTz);

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case StartEndDtLocal: {
        //Convert time to the system time zone
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case Overlapping:
        return overlappingEvents(row);
    case ConferenceTzId: {
        return calendarTz.id();
    }
    default:
        return QVariant {};
    }
}

int EventModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_events.count();
}

void EventModel::loadEvents()
{
    beginResetModel();
    m_events.clear();

    if (m_local_calendar != nullptr && m_local_calendar->memorycalendar() != nullptr && m_filterdt.isValid()) {
        auto filterTz = m_settings_controller->displayInLocalTimezone() ? QTimeZone::systemTimeZone() : m_local_calendar->memorycalendar()->timeZone();
        auto dayEvents = m_local_calendar->memorycalendar()->rawEvents(m_filterdt, m_filterdt, filterTz, true);
        m_events = KCalendarCore::Calendar::sortEvents(dayEvents, KCalendarCore::EventSortField::EventSortStartDate, KCalendarCore::SortDirection::SortDirectionAscending);
    }

    if (m_local_calendar != nullptr && m_local_calendar->memorycalendar() != nullptr && m_filterdt.isNull()) {
        m_events = m_local_calendar->memorycalendar()->rawEvents(KCalendarCore::EventSortStartDate, KCalendarCore::SortDirectionAscending);
    }

    if (m_local_calendar != nullptr && !(m_category.isEmpty())) {
        QStringList categories {m_category};
        KCalendarCore::CalFilter filter {};
        filter.setCategoryList(categories);
        filter.setCriteria(KCalendarCore::CalFilter::ShowCategories);
        filter.apply(&m_events);
    }

    endResetModel();
}

int EventModel::repeatEvery(const int idx) const
{
    if (!(m_events.at(idx)->recurs())) {
        return 0;
    }

    return m_events.at(idx)->recurrence()->frequency();
}

int EventModel::repeatStopAfter(const int idx) const
{

    if (!(m_events.at(idx)->recurs())) {
        return -1;
    }

    return m_events.at(idx)->recurrence()->duration();
}

ushort EventModel::repeatPeriodType(const int idx) const
{
    if (!(m_events.at(idx)->recurs())) {
        return KCalendarCore::Recurrence::rNone;
    }

    return m_events.at(idx)->recurrence()->recurrenceType();
}

void EventModel::setEventCategory(const QString &category)
{
    if (m_category != category) {
        m_category = category;
        Q_EMIT eventCategoryChanged();
    }
}

QString EventModel::eventCategory() const
{
    return m_category;
}

int EventModel::overlappingEvents(const int idx) const
{
    int cnt = 0;

    for (const auto &e : m_events) {
        if ((m_events.at(idx)->dtStart() < e->dtEnd()) && (m_events.at(idx)->dtEnd() > e->dtStart()) && (m_events.at(idx)->uid() != e->uid())) {
            ++cnt;
        }
    }

    return cnt;
}

QString EventModel::formatStartEndTime(const QDateTime &startDtTime, const QDateTime &endDtTime) const
{
    if (startDtTime.date() == endDtTime.date()) {
        return QString {"%1 - %2"}.arg(startDtTime.time().toString("hh:mm"), endDtTime.time().toString("hh:mm"));
    }

    auto displayStartDtTime = QString {"%1 %2"}.arg(startDtTime.date().toString("ddd d MMM yyyy"), startDtTime.time().toString("hh:mm"));
    auto displayEndDtTime = QString {"%1 %2"}.arg(endDtTime.date().toString("ddd d MMM yyyy"), endDtTime.time().toString("hh:mm"));

    return QString {"%1 - %2 %3"}.arg(displayStartDtTime, displayEndDtTime, startDtTime.timeZoneAbbreviation());
}

QString EventModel::formatStartEndDt(const QDateTime &startDtTime, const QDateTime &endDtTime, bool allDay) const
{
    if (allDay) {
        return startDtTime.date().toString("ddd d MMM yyyy");
    }

    if (startDtTime.date() == endDtTime.date()) {

        auto displayDt = startDtTime.date().toString("ddd d MMM yyyy");
        auto displayTime = QString {"%1 - %2"}.arg(startDtTime.time().toString("hh:mm"), endDtTime.time().toString("hh:mm"));

        return QString {"%1 %2 %3"}.arg(displayDt, displayTime, startDtTime.timeZoneAbbreviation());
    }

    auto displayStartDtTime = QString {"%1 %2"}.arg(startDtTime.date().toString("ddd d MMM yyyy"), startDtTime.time().toString("hh:mm"));
    auto displayEndDtTime = QString {"%1 %2"}.arg(endDtTime.date().toString("ddd d MMM yyyy"), endDtTime.time().toString("hh:mm"));

    return QString {"%1 - %2 %3"}.arg(displayStartDtTime, displayEndDtTime, startDtTime.timeZoneAbbreviation());
}
