/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "eventcontroller.h"
#include "calendarcontroller.h"
#include "localcalendar.h"
#include "settingscontroller.h"
#include <KCalendarCore/Event>
#include <KLocalizedString>
#include <QDebug>

using namespace Qt::Literals::StringLiterals;

EventController::EventController(QObject *parent)
    : QObject{parent}
    , m_cal_controller{nullptr}
    , m_settings_controller{new SettingsController{this}}
{
}

CalendarController *EventController::calendarController()
{
    return m_cal_controller;
}

void EventController::setCalendarController(CalendarController *const controller)
{
    m_cal_controller = controller;
    Q_EMIT calendarControllerChanged();
}

bool EventController::remove(const QVariantMap &eventData)
{
    if (m_calendar == nullptr) {
        qDebug() << "There is no calendar to delete event from";

        return false;
    }

    qDebug() << "Deleting event from calendar " << m_calendar->calendarId();

    auto memoryCalendar = m_calendar->memorycalendar();
    auto uid = eventData["uid"_L1].toString();
    auto event = memoryCalendar->event(uid);
    if (!event) {
        qWarning() << "no event found";
        return false;
    }
    memoryCalendar->deleteEvent(event);

    auto deleted{false};
    if (m_cal_controller != nullptr) {
        deleted = m_cal_controller->save(m_calendar->calendarId());
        Q_EMIT m_calendar->eventsChanged();
    }

    Q_EMIT isFavoriteChanged();

    qDebug() << "Event deleted: " << deleted;
    return true;
}

QVariantMap EventController::addEdit(const QVariantMap &eventData)
{
    if (m_calendar == nullptr) {
        return {{u"status"_s, NoCalendarExists}, {u"message"_s, i18n("Error during event creation")}};
    }

    auto eventCheckResult = eventCheck(eventData);

    if (eventCheckResult["result"_L1].toInt() == Exists) {
        Q_EMIT isFavoriteChanged();
        return {{u"status"_s, NoCalendarExists}, {u"message"_s, i18n("Already in favorites")}};
    }

    qDebug() << "\naddEdit:\tCreating event to calendar " << m_calendar->calendarId() << eventData;

    auto memoryCalendar = m_calendar->memorycalendar();
    auto uid = eventData["uid"_L1].toString();
    auto event = memoryCalendar->event(uid);

    if (event == nullptr) {
        event = KCalendarCore::Event::Ptr{new KCalendarCore::Event{}};
    }

    event->setUid(uid);
    event->setDtStart(eventData["startDate"_L1].toDateTime().toTimeZone(QTimeZone::utc()));
    event->setDtEnd(eventData["endDate"_L1].toDateTime().toTimeZone(QTimeZone::utc()));
    event->setDescription(eventData["description"_L1].toString());
    event->setCategories(eventData["categories"_L1].toString());
    event->setSummary(eventData["summary"_L1].toString());
    event->setAllDay(eventData["allDay"_L1].toBool());
    event->setLocation(eventData["location"_L1].toString());
    event->setUrl(QUrl(eventData["url"_L1].toString()));
    const auto speakers = eventData["speakers"_L1].toStringList();
    for (const auto &speaker : speakers) {
        event->addAttendee(KCalendarCore::Attendee(speaker, {}));
    }

    event->clearAlarms();

    auto newAlarm = event->newAlarm();
    newAlarm->setStartOffset(KCalendarCore::Duration(-60 * m_settings_controller->remindBeforeStart(), KCalendarCore::Duration::Type::Seconds));
    newAlarm->setType(KCalendarCore::Alarm::Type::Display);
    newAlarm->setEnabled(true);
    auto alarmText = (event->summary()).isEmpty() ? event->description() : event->summary();

    if (!event->location().isEmpty()) {
        alarmText.append("\n%1"_L1.arg(event->location()));
    }

    newAlarm->setText(alarmText);
    memoryCalendar->addEvent(event);

    auto merged{false};

    if (m_cal_controller != nullptr) {
        merged = m_cal_controller->save(m_calendar->calendarId());
        Q_EMIT m_calendar->eventsChanged();
    }

    qDebug() << "addEdit:\tEvent added/updated: " << merged;

    Q_EMIT isFavoriteChanged();

    return {{u"status"_s, eventCheckResult["result"_L1]},
            {u"message"_s,
             (eventCheckResult["result"_L1] == NotExistsButOverlaps)
                 ? i18n("Talk added to favorites, but it overlaps with existing ones:\n%1", eventCheckResult["events"_L1].toString())
                 : i18n("Talk added to favorites")}};
}

QVariantMap EventController::eventCheck(const QVariantMap &event) const
{
    QVariantMap response{{u"result"_s, QVariant{NotExistsNotOverlapping}}, {u"events"_s, QString{}}};

    auto memoryCalendar = m_calendar->memorycalendar();
    QStringList overlappingEvents{};

    auto eventStart = event["startDate"_L1].toDateTime();
    auto eventEnd = event["endDate"_L1].toDateTime();
    auto eventUid = event["uid"_L1].toString();

    // If the event-to-check has no valid start or end date, assume that there is no overlap
    if (!(eventStart.isValid()) || !(eventEnd.isValid())) {
        return response;
    }

    auto existingEvents = memoryCalendar->rawEventsForDate(eventStart.date(), memoryCalendar->timeZone());

    for (const auto &e : existingEvents) {
        if ((eventStart < e->dtEnd()) && (eventEnd > e->dtStart())) {
            overlappingEvents.append(e->summary());
        }

        if (!(eventUid.isEmpty()) && e->uid() == eventUid) {
            response["result"_L1] = QVariant{Exists};
            return response;
        }
    }

    if (!(overlappingEvents.isEmpty())) {
        response["result"_L1] = QVariant{NotExistsButOverlaps};
        response["events"_L1] = overlappingEvents.join('\n'_L1);
    }

    return response;
}

LocalCalendar *EventController::calendar() const
{
    return m_calendar;
}

void EventController::setCalendar(LocalCalendar *calendar)
{
    if (m_calendar == calendar) {
        return;
    }
    m_calendar = calendar;
    Q_EMIT calendarChanged();
    Q_EMIT isFavoriteChanged();
}

QString EventController::eventUid() const
{
    return m_eventUid;
}

void EventController::setEventUid(const QString &eventUid)
{
    if (m_eventUid == eventUid) {
        return;
    }
    m_eventUid = eventUid;
    Q_EMIT eventUidChanged();
    Q_EMIT isFavoriteChanged();
}

bool EventController::isFavorite() const
{
    if (m_calendar == nullptr || m_eventUid.isEmpty()) {
        return false;
    }

    auto memoryCalendar = m_calendar->memorycalendar();
    QStringList overlappingEvents{};

    auto existingEvents = memoryCalendar->rawEvents();

    for (const auto &e : existingEvents) {
        if (e->uid() == m_eventUid) {
            return true;
        }
    }

    return false;
}

#include "moc_eventcontroller.cpp"
