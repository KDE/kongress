/*
 * Copyright (C) 2020 Dimitris Kardarakos
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
#include <KCalendarCore/CalFilter>
#include <KLocalizedString>
using namespace KCalendarCore;

EventModel::EventModel(QObject* parent) :
    QAbstractListModel(parent),
    m_events(Event::List()),
    m_filterdt(QDate()),
    m_category(QString()),
    m_calendar(nullptr)
{
    connect(this, &EventModel::filterdtChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::memorycalendarChanged, this, &EventModel::loadEvents);
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
    emit filterdtChanged();
}

MemoryCalendar::Ptr EventModel::memorycalendar() const
{
    return m_calendar;
}

void EventModel::setMemorycalendar(const MemoryCalendar::Ptr calendarPtr)
{
    m_calendar = calendarPtr;
    emit memorycalendarChanged();
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(Uid, "uid");
    roles.insert(DtStart, "dtstart");
    roles.insert(AllDay, "allday");
    roles.insert(Description, "description");
    roles.insert(Summary, "summary");
    roles.insert(LastModified, "lastmodified");
    roles.insert(Location, "location");
    roles.insert(Categories, "categories");
    roles.insert(Priority, "priority");
    roles.insert(Created, "created");
    roles.insert(Secrecy, "secrecy");
    roles.insert(EndDate, "dtend");
    roles.insert(Transparency, "transparency");
    roles.insert(RepeatPeriodType, "repeatType");
    roles.insert(RepeatEvery, "repeatEvery");
    roles.insert(RepeatStopAfter, "repeatStopAfter");
    roles.insert(IsRepeating, "isRepeating");
    roles.insert(EventCategories, "eventCategories");
    roles.insert(Url, "url");
    return roles;
}

QVariant EventModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch(role)
    {
        case Uid :
            return m_events.at(index.row())->uid();
        case DtStart:
            return m_events.at(index.row())->dtStart();
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
        case EndDate:
            return m_events.at(index.row())->dtEnd();
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

    if(m_calendar != nullptr && m_filterdt.isValid())
    {
        m_events = m_calendar->rawEventsForDate(m_filterdt, m_calendar->timeZone(), EventSortStartDate,SortDirectionAscending);
    }

    if(m_calendar != nullptr && m_filterdt.isNull())
    {
        m_events = m_calendar->rawEvents(EventSortStartDate, SortDirectionAscending);
    }

    if(m_calendar != nullptr && !(m_category.isEmpty()))
    {
        QStringList categories(m_category);
        CalFilter* filter = new CalFilter();
        filter->setCategoryList(categories);
        filter->setCriteria(CalFilter::ShowCategories);
        filter->apply(&m_events);
    }

    endResetModel();
    emit rowCountChanged();
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
        emit eventCategoryChanged();
    }
}

QString EventModel::eventCategory() const
{
    return m_category;
}

