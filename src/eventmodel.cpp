/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "eventmodel.h"
#include "settingscontroller.h"
#include <KCalendarCore/CalFilter>
#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

EventModel::EventModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_events{KCalendarCore::Event::List{}}
    , m_filterdt{QDate{}}
    , m_category{QString{}}
    , m_local_calendar{nullptr}
    , m_settings_controller{new SettingsController}
    , m_favorites_calendar{nullptr}
{
    connect(this, &EventModel::filterdtChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::calendarChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::eventCategoryChanged, this, &EventModel::loadEvents);
    connect(this, &EventModel::favoritesCalendarChanged, this, &EventModel::loadEvents);
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

LocalCalendar *EventModel::favoritesCalendar() const
{
    return m_favorites_calendar;
}

void EventModel::setFavoritesCalendar(LocalCalendar *const calendarPtr)
{
    m_favorites_calendar = calendarPtr;

    connect(m_favorites_calendar, &LocalCalendar::eventsChanged, this, &EventModel::loadEvents);

    Q_EMIT favoritesCalendarChanged();
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    return {
        {Uid, "uid"},
        {EventStartDt, "eventStartDt"},
        {EventDt, "eventDt"},
        {ShiftedEventDt, "shiftedEventDt"},
        {ShiftedEventDtLocal, "shiftedEventDtLocal"},
        {ScheduleStartDt, "scheduleStartDt"},
        {Description, "description"},
        {Summary, "summary"},
        {Location, "location"},
        {Speakers, "speakers"},
        {Categories, "categories"},
        {EventEndDt, "eventEndDt"},
        {ScheduleEndDt, "scheduleEndDt"},
        {EventCategories, "eventCategories"},
        {Url, "url"},
        {ShiftedStartEndDt, "shiftedStartEndDt"},
        {ShiftedStartEndDtLocal, "shiftedStartEndDtLocal"},
        {ShiftedStartEndTime, "shiftedStartEndTime"},
        {ShiftedStartEndTimeLocal, "shiftedStartEndTimeLocal"},
        {ShiftedStartEndTimeMobile, "shiftedStartEndTimeMobile"},
        {ShiftedStartEndTimeLocalMobile, "shiftedStartEndTimeLocalMobile"},
        {StartEndDt, "startEndDt"},
        {StartEndDtLocal, "startEndDtLocal"},
        {Overlapping, "overlapping"},
        {ConferenceTzId, "conferenceTzId"},
        {Favorite, "favorite"},
        {AllDay, "allDay"},
    };
}

static void applyTimeZone(QDateTime &dt, const QTimeZone &tz)
{
    if (dt.timeSpec() == Qt::LocalTime) {
        dt.setTimeZone(tz);
    } else {
        dt = dt.toTimeZone(tz);
    }
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant{};
    }

    auto row = index.row();
    auto calendarTz = m_local_calendar->memorycalendar()->timeZone();
    auto startDtTime = m_events.at(row)->dtStart();
    auto endDtTime = m_events.at(row)->dtEnd();
    auto allDay = m_events.at(row)->allDay();

    switch (role) {
    case Uid:
        return m_events.at(row)->uid();
    case EventStartDt:
        return startDtTime;
    case EventDt:
        return startDtTime.toString(u"dddd d MMMM");
    case ShiftedEventDt: {
        applyTimeZone(startDtTime, calendarTz);
        return startDtTime.toString(u"dddd d MMMM");
    }
    case ShiftedEventDtLocal: {
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        return startDtTime.toString(u"dddd d MMMM");
    }
    case ScheduleStartDt: {
        applyTimeZone(startDtTime, calendarTz);
        return startDtTime;
    }
    case Description:
        return m_events.at(row)->description();
    case Summary:
        return m_events.at(row)->summary();
    case Location:
        return m_events.at(row)->location();
    case Speakers: {
        QStringList speakers;
        speakers.reserve(m_events.at(row)->attendees().size());
        for (const auto &attendee : m_events.at(row)->attendees()) {
            speakers.push_back(attendee.name());
        }
        return speakers;
    }
    case Categories:
        return m_events.at(row)->categories();
    case EventEndDt:
        return endDtTime;
    case ScheduleEndDt: {
        applyTimeZone(endDtTime, calendarTz);
        return endDtTime;
    }
    case EventCategories:
        return m_events.at(row)->categoriesStr();
    case Url:
        return m_events.at(row)->url();
    case ShiftedStartEndDt: {
        // Shift and convert time to the time zone of the conference
        applyTimeZone(startDtTime, calendarTz);
        applyTimeZone(endDtTime, calendarTz);

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case ShiftedStartEndDtLocal: {
        // Shift time to the time zone of the conference and convert to the system time zone
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case ShiftedStartEndTime: {
        // Shift time to the time zone of the conference
        applyTimeZone(startDtTime, calendarTz);
        applyTimeZone(endDtTime, calendarTz);

        return formatStartEndTime(startDtTime, endDtTime, false);
    }
    case ShiftedStartEndTimeLocal: {
        // Shift time to the conference time zone and convert time to the system time zone
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndTime(startDtTime, endDtTime, false);
    }
    case ShiftedStartEndTimeMobile: {
        // Shift time to the time zone of the conference
        applyTimeZone(startDtTime, calendarTz);
        applyTimeZone(endDtTime, calendarTz);

        return formatStartEndTime(startDtTime, endDtTime, true);
    }
    case ShiftedStartEndTimeLocalMobile: {
        // Shift time to the conference time zone and convert time to the system time zone
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndTime(startDtTime, endDtTime, true);
    }
    case StartEndDt: {
        // Convert time to the time zone of the conference
        applyTimeZone(startDtTime, calendarTz);
        applyTimeZone(endDtTime, calendarTz);

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case StartEndDtLocal: {
        // Convert time to the system time zone
        startDtTime = startDtTime.toTimeZone(QTimeZone::systemTimeZone());
        endDtTime = endDtTime.toTimeZone(QTimeZone::systemTimeZone());

        return formatStartEndDt(startDtTime, endDtTime, allDay);
    }
    case Overlapping:
        return overlappingEvents(row);
    case ConferenceTzId: {
        return calendarTz.id();
    }
    case Favorite: {
        auto favorites_memorycalendar = m_favorites_calendar->memorycalendar();
        auto uid = m_events.at(row)->uid();

        auto favorite_event = favorites_memorycalendar->event(uid);

        if (favorite_event == nullptr) {
            return false;
        } else {
            return true;
        }
    }
    case AllDay:
        return allDay;
    default:
        return QVariant{};
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
        m_events = KCalendarCore::Calendar::sortEvents(std::move(dayEvents),
                                                       KCalendarCore::EventSortField::EventSortStartDate,
                                                       KCalendarCore::SortDirection::SortDirectionAscending);
    }

    if (m_local_calendar != nullptr && m_local_calendar->memorycalendar() != nullptr && m_filterdt.isNull()) {
        m_events = m_local_calendar->memorycalendar()->rawEvents(KCalendarCore::EventSortStartDate, KCalendarCore::SortDirectionAscending);
    }

    if (m_local_calendar != nullptr && !(m_category.isEmpty())) {
        QStringList categories{m_category};
        KCalendarCore::CalFilter filter{};
        filter.setCategoryList(categories);
        filter.setCriteria(KCalendarCore::CalFilter::ShowCategories);
        filter.apply(&m_events);
    }

    endResetModel();
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

QString EventModel::formatStartEndTime(const QDateTime &startDtTime, const QDateTime &endDtTime, bool mobile) const
{
    if (startDtTime.date() == endDtTime.date()) {
        return (mobile ? "%1<br />%2"_L1 : "%1 - %2"_L1)
            .arg(QLocale().toString(startDtTime.time(), QLocale::ShortFormat), QLocale().toString(endDtTime.time(), QLocale::ShortFormat));
    }

    auto displayStartDtTime = (mobile ? "%1<br />%2"_L1 : "%1 - %2"_L1)
                                  .arg(startDtTime.date().toString(u"ddd d MMM yyyy"), QLocale().toString(startDtTime.time(), QLocale::ShortFormat));
    auto displayEndDtTime =
        (mobile ? "%1<bt />%2"_L1 : "%1 - %2"_L1).arg(endDtTime.date().toString(u"ddd d MMM yyyy"), QLocale().toString(endDtTime.time(), QLocale::ShortFormat));

    return (mobile ? "%1 %3<br />%2 %3"_L1 : "%1 - %2 %3"_L1).arg(displayStartDtTime, displayEndDtTime, startDtTime.timeZoneAbbreviation());
}

QString EventModel::formatStartEndDt(const QDateTime &startDtTime, const QDateTime &endDtTime, bool allDay) const
{
    if (allDay) {
        return startDtTime.date().toString(u"ddd d MMM yyyy");
    }

    if (startDtTime.date() == endDtTime.date()) {
        auto displayDt = startDtTime.date().toString(u"ddd d MMM yyyy");
        auto displayTime =
            "%1 - %2"_L1.arg(QLocale().toString(startDtTime.time(), QLocale::ShortFormat), QLocale().toString(endDtTime.time(), QLocale::ShortFormat));

        return "%1 %2 %3"_L1.arg(displayDt, displayTime, startDtTime.timeZoneAbbreviation());
    }

    auto displayStartDtTime = "%1 %2"_L1.arg(startDtTime.date().toString(u"ddd d MMM yyyy"), QLocale().toString(startDtTime.time(), QLocale::ShortFormat));
    auto displayEndDtTime = "%1 %2"_L1.arg(endDtTime.date().toString(u"ddd d MMM yyyy"), QLocale().toString(endDtTime.time(), QLocale::ShortFormat));

    return "%1 - %2 %3"_L1.arg(displayStartDtTime, displayEndDtTime, startDtTime.timeZoneAbbreviation());
}

#include "moc_eventmodel.cpp"
