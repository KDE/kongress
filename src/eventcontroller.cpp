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
#include <KCalendarCore/MemoryCalendar>
#include <KLocalizedString>

EventController::EventController(QObject *parent) : QObject {parent}, m_cal_controller {new CalendarController}, m_settings_controller {new SettingsController {this}}
{}

QObject *EventController::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new EventController;
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

void EventController::remove(LocalCalendar *calendar, const QVariantMap &eventData)
{
    if (calendar == nullptr) {
        qDebug() << "There is no calendar to delete event from";

        return;
    }

    qDebug() << "Deleting event from calendar " << calendar->calendarId();

    auto memoryCalendar = calendar->memorycalendar();
    auto uid = eventData["uid"].toString();
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
        return { {"status", NoCalendarExists}, {"message", i18n("Error during event creation") } };
    }

    auto eventCheckResult = eventCheck(calendar, eventData);

    if (eventCheckResult["result"].toInt() == Exists) {
        return {
            {"status", NoCalendarExists}, {"message", i18n("Already in favorites")}
        };
    }

    qDebug() << "\naddEdit:\tCreating event to calendar " << calendar->calendarId();

    auto memoryCalendar = calendar->memorycalendar();
    auto uid = eventData["uid"].toString();
    auto event = memoryCalendar->event(uid);

    if (event == nullptr) {
        event = KCalendarCore::Event::Ptr {new KCalendarCore::Event {}};
    }

    event->setUid(uid);
    event->setDtStart(eventData["startDate"].toDateTime().toTimeZone(QTimeZone::utc()));
    event->setDtEnd(eventData["endDate"].toDateTime().toTimeZone(QTimeZone::utc()));
    event->setDescription(eventData["description"].toString());
    event->setCategories(eventData["categories"].toString());
    event->setSummary(eventData["summary"].toString());
    event->setAllDay(eventData["allDay"].toBool());
    event->setLocation(eventData["location"].toString());
    event->setUrl(eventData["url"].toString());

    event->clearAlarms();

    auto newAlarm = event->newAlarm();
    newAlarm->setStartOffset(KCalendarCore::Duration(-60 & m_settings_controller->remindBeforeStart(), KCalendarCore::Duration::Type::Seconds));
    newAlarm->setType(KCalendarCore::Alarm::Type::Display);
    newAlarm->setEnabled(true);
    auto alarmText = (event->summary()).isEmpty() ? event->description() : event->summary();

    if (!event->location().isEmpty()) {
        alarmText.append((QString {"\n%1"}).arg(event->location()));
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
        { "status", eventCheckResult["result"] },
        { "message", (eventCheckResult["result"] == NotExistsButOverlaps) ? i18n("Talk added to favorites, but it overlaps with existing ones:\n%1", eventCheckResult["events"].toString()) : i18n("Talk added to favorites") }
    };

}

QVariantMap EventController::eventCheck(LocalCalendar *calendar, const QVariantMap &event)
{
    QVariantMap response {
        { "result", QVariant {NotExistsNotOverlapping} },
        { "events", QString {} }
    };

    auto memoryCalendar = calendar->memorycalendar();
    QStringList overlappingEvents {};

    auto eventStart = event["startDate"].toDateTime();
    auto eventEnd = event["endDate"].toDateTime();
    auto eventUid = event["uid"].toString();

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
            response["result"] = QVariant {Exists};
            return response;
        }
    }

    if (!(overlappingEvents.isEmpty())) {
        response["result"] = QVariant {NotExistsButOverlaps};
        response["events"] = overlappingEvents.join("\n");
    }

    return response;
}
