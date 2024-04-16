/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "eventcontroller.h"
#include "calendarcontroller.h"
#include "localcalendar.h"
#include "settingscontroller.h"
#include <QDebug>
#include <KCalendarCore/Event>
#include <KLocalizedString>

using namespace Qt::Literals::StringLiterals;

EventController::EventController(QObject *parent) : QObject {parent}, m_cal_controller {new CalendarController}, m_settings_controller {new SettingsController {this}}
{}

CalendarController *EventController::calendarController()
{
    return m_cal_controller;
}

void EventController::setCalendarController(CalendarController *const controller)
{
    m_cal_controller = controller;
    Q_EMIT calendarControllerChanged();
}

void EventController::remove(LocalCalendar *calendar, const QVariantMap &eventData)
{
    if (calendar == nullptr) {
        qDebug() << "There is no calendar to delete event from";

        return;
    }

    qDebug() << "Deleting event from calendar " << calendar->calendarId();

    auto memoryCalendar = calendar->memorycalendar();
    auto uid = eventData["uid"_L1].toString();
    auto event = memoryCalendar->event(uid);
    memoryCalendar->deleteEvent(event);

    auto deleted {false};
    if (m_cal_controller != nullptr) {
        deleted = m_cal_controller->save(calendar->calendarId());
        Q_EMIT calendar->eventsChanged();
    }

    qDebug() << "Event deleted: " << deleted;
}

QVariantMap EventController::addEdit(LocalCalendar *calendar, const QVariantMap &eventData)
{
    if (calendar == nullptr) {
        return { {u"status"_s, NoCalendarExists}, {u"message"_s, i18n("Error during event creation") } };
    }

    auto eventCheckResult = eventCheck(calendar, eventData);

    if (eventCheckResult["result"_L1].toInt() == Exists) {
        return {
            {u"status"_s, NoCalendarExists}, {u"message"_s, i18n("Already in favorites")}
        };
    }

    qDebug() << "\naddEdit:\tCreating event to calendar " << calendar->calendarId();

    auto memoryCalendar = calendar->memorycalendar();
    auto uid = eventData["uid"_L1].toString();
    auto event = memoryCalendar->event(uid);

    if (event == nullptr) {
        event = KCalendarCore::Event::Ptr {new KCalendarCore::Event {}};
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

    auto merged {false};

    if (m_cal_controller != nullptr) {
        merged = m_cal_controller->save(calendar->calendarId());
        Q_EMIT calendar->eventsChanged();
    }

    qDebug() << "addEdit:\tEvent added/updated: " << merged;

    return {
        { u"status"_s, eventCheckResult["result"_L1] },
        { u"message"_s, (eventCheckResult["result"_L1] == NotExistsButOverlaps) ? i18n("Talk added to favorites, but it overlaps with existing ones:\n%1", eventCheckResult["events"_L1].toString()) : i18n("Talk added to favorites") }
    };

}

QVariantMap EventController::eventCheck(LocalCalendar *calendar, const QVariantMap &event)
{
    QVariantMap response {
        { u"result"_s, QVariant {NotExistsNotOverlapping} },
        { u"events"_s, QString {} }
    };

    auto memoryCalendar = calendar->memorycalendar();
    QStringList overlappingEvents {};

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
            response["result"_L1] = QVariant {Exists};
            return response;
        }
    }

    if (!(overlappingEvents.isEmpty())) {
        response["result"_L1] = QVariant {NotExistsButOverlaps};
        response["events"_L1] = overlappingEvents.join('\n'_L1);
    }

    return response;
}

#include "moc_eventcontroller.cpp"
