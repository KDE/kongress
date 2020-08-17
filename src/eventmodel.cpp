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

#include "eventmodel.h"
#include "localcalendar.h"
#include <KCalendarCore/CalFilter>
#include <KCalendarCore/MemoryCalendar>
#include <KLocalizedString>

using namespace KCalendarCore;

EventModel::EventModel(QObject* parent) :
    QAbstractListModel(parent),
    m_events(Event::List()),
    m_filterdt(QDate()),
    m_category(QString()),
    m_local_calendar(nullptr)
{
    connect(this, &EventModel::filterdtChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::calendarChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::eventCategoryChanged, this, &EventModel::loadEvents);
}

EventModel::~EventModel() = default;

QDate EventModel::filterdt() const
{
    return m_filterdt;
}

void EventModel::setFilterdt(const QDate& filterDate)
{
    m_filterdt = filterDate;

    Q_EMIT filterdtChanged();
}

LocalCalendar* EventModel::calendar() const
{
    return m_local_calendar;
}

void EventModel::setCalendar(LocalCalendar* const calendarPtr)
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
        { ShiftedStartEndTime, "shiftedStartEndTime" },
        { StartEndDt, "startEndDt" },
        { Overlapping, "overlapping" }
    };
}

QVariant EventModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto calendarTz = m_local_calendar->memorycalendar()->timeZone();

    switch(role)
    {
        case Uid :
            return m_events.at(index.row())->uid();
        case EventStartDt:
            return m_events.at(index.row())->dtStart();
        case EventDt:
            return m_events.at(index.row())->dtStart().toString("dddd d MMMM");
        case ScheduleStartDt:
        {
            auto startDtWConferenceTz = m_events.at(index.row())->dtStart();
            startDtWConferenceTz.setTimeZone(calendarTz);
            return startDtWConferenceTz;
        }
        case AllDay:
            return m_events.at(index.row())->allDay();
        case Description:
            return m_events.at(index.row())->description();
        case Summary:
            return m_events.at(index.row())->summary();
        case LastModified:
            return m_events.at(index.row())->lastModified();
        case Location:
            return m_events.at(index.row())->location();
        case Categories:
            return m_events.at(index.row())->categories();
        case Priority:
            return m_events.at(index.row())->priority();
        case Created:
            return m_events.at(index.row())->created();
        case Secrecy:
            return m_events.at(index.row())->secrecy();
        case EventEndDt:
            return m_events.at(index.row())->dtEnd();
        case ScheduleEndDt:
        {
            auto endDtWConferenceTz = m_events.at(index.row())->dtEnd();
            endDtWConferenceTz.setTimeZone(calendarTz);
            return endDtWConferenceTz;
        }
        case Transparency:
            return m_events.at(index.row())->transparency();
        case RepeatPeriodType:
            return repeatPeriodType(index.row());
        case RepeatEvery:
            return repeatEvery(index.row());
        case RepeatStopAfter:
            return repeatStopAfter(index.row());
        case IsRepeating:
            return m_events.at(index.row())->recurs();
        case EventCategories:
            return m_events.at(index.row())->categoriesStr();
        case Url:
            return m_events.at(index.row())->url();
        case ShiftedStartEndDt:
        {
            auto startDtTime = m_events.at(index.row())->dtStart();
            auto endDtTime = m_events.at(index.row())->dtEnd();

            if(m_events.at(index.row())->allDay())
            {
                return startDtTime.date().toString("ddd d MMM yyyy");
            }

            //Remedy for ical files that TZ-ID of events cannot be read; it should be fixed in framework
            startDtTime.setTimeZone(calendarTz);
            endDtTime.setTimeZone(calendarTz);

            if(startDtTime.date() == endDtTime.date())
            {

                auto displayDt = startDtTime.date().toString("ddd d MMM yyyy");
                auto displayTime = QString("%1 - %2").arg(startDtTime.time().toString("hh:mm")).arg(endDtTime.time().toString("hh:mm"));

                return QString("%1 %2 %3").arg(displayDt).arg(displayTime).arg(startDtTime.timeZoneAbbreviation());
            }

            auto displayStartDtTime = QString("%1 %2").arg(startDtTime.date().toString("ddd d MMM yyyy")).arg(startDtTime.time().toString("hh:mm"));
            auto displayEndDtTime = QString("%1 %2").arg(endDtTime.date().toString("ddd d MMM yyyy")).arg(endDtTime.time().toString("hh:mm"));

            return QString("%1 - %2 %3").arg(displayStartDtTime).arg(displayEndDtTime).arg(startDtTime.timeZoneAbbreviation());
        }
        case ShiftedStartEndTime:
        {
            auto startDtTime = m_events.at(index.row())->dtStart();
            auto endDtTime = m_events.at(index.row())->dtEnd();

            //Convert time to the time zone of the conference
            startDtTime.setTimeZone(calendarTz);
            endDtTime.setTimeZone(calendarTz);

            if(startDtTime.date() == endDtTime.date())
            {
                return QString("%1 - %2").arg(startDtTime.time().toString("hh:mm")).arg(endDtTime.time().toString("hh:mm"));
            }

            auto displayStartDtTime = QString("%1 %2").arg(startDtTime.date().toString("ddd d MMM yyyy")).arg(startDtTime.time().toString("hh:mm"));
            auto displayEndDtTime = QString("%1 %2").arg(endDtTime.date().toString("ddd d MMM yyyy")).arg(endDtTime.time().toString("hh:mm"));

            return QString("%1 - %2 %3").arg(displayStartDtTime).arg(displayEndDtTime).arg(startDtTime.timeZoneAbbreviation());
        }
        case StartEndDt:
        {
            auto startDtTime = m_events.at(index.row())->dtStart();
            auto endDtTime = m_events.at(index.row())->dtEnd();

            startDtTime = startDtTime.toTimeZone(calendarTz);
            endDtTime = endDtTime.toTimeZone(calendarTz);

            if(m_events.at(index.row())->allDay())
            {
                return startDtTime.date().toString("ddd d MMM yyyy");
            }

            if(startDtTime.date() == endDtTime.date())
            {

                auto displayDt = startDtTime.date().toString("ddd d MMM yyyy");
                auto displayTime = QString("%1 - %2").arg(startDtTime.time().toString("hh:mm")).arg(endDtTime.time().toString("hh:mm"));

                return QString("%1 %2 %3").arg(displayDt).arg(displayTime).arg(startDtTime.timeZoneAbbreviation());
            }

            auto displayStartDtTime = QString("%1 %2").arg(startDtTime.date().toString("ddd d MMM yyyy")).arg(startDtTime.time().toString("hh:mm"));
            auto displayEndDtTime = QString("%1 %2").arg(endDtTime.date().toString("ddd d MMM yyyy")).arg(endDtTime.time().toString("hh:mm"));

            return QString("%1 - %2 %3").arg(displayStartDtTime).arg(displayEndDtTime).arg(startDtTime.timeZoneAbbreviation());
        }
        case Overlapping:
            return overlappingEvents(index.row());
        default:
            return QVariant();
    }
}

int EventModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_events.count();
}

void EventModel::loadEvents()
{
    beginResetModel();
    m_events.clear();

    if(m_local_calendar != nullptr && m_local_calendar->memorycalendar() != nullptr && m_filterdt.isValid())
    {
        m_events = m_local_calendar->memorycalendar()->rawEventsForDate(m_filterdt, m_local_calendar->memorycalendar()->timeZone(), EventSortStartDate,SortDirectionAscending);
    }

    if(m_local_calendar != nullptr && m_local_calendar->memorycalendar() != nullptr && m_filterdt.isNull())
    {
        m_events =  m_local_calendar->memorycalendar()->rawEvents(EventSortStartDate, SortDirectionAscending);
    }

    if(m_local_calendar != nullptr && !(m_category.isEmpty()))
    {
        QStringList categories(m_category);
        CalFilter* filter = new CalFilter();
        filter->setCategoryList(categories);
        filter->setCriteria(CalFilter::ShowCategories);
        filter->apply(&m_events);
    }

    endResetModel();

    Q_EMIT rowCountChanged();
}

int EventModel::repeatEvery(const int idx) const
{
    if(!(m_events.at(idx)->recurs())) return 0;

    return m_events.at(idx)->recurrence()->frequency();
}

int EventModel::repeatStopAfter(const int idx) const
{

    if(!(m_events.at(idx)->recurs())) return -1;

    return m_events.at(idx)->recurrence()->duration();
}

ushort EventModel::repeatPeriodType(const int idx) const
{
    if(!(m_events.at(idx)->recurs())) return Recurrence::rNone;

    return m_events.at(idx)->recurrence()->recurrenceType();
}

void EventModel::setEventCategory(const QString& category)
{
    if(m_category != category)
    {
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

    for (const auto& e : m_events)
    {
        if( (m_events.at(idx)->dtStart() < e->dtEnd()) && (m_events.at(idx)->dtEnd() > e->dtStart()) && (m_events.at(idx)->uid() != e->uid()) )
        {
            ++cnt;
        }
    }

    return cnt;
}
