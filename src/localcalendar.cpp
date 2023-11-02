/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "localcalendar.h"
#include "calendarcontroller.h"
#include "alarmchecker.h"
#include <QDebug>
#include <KLocalizedString>

LocalCalendar::LocalCalendar(QObject *parent)
    : QObject {parent}, m_calendar_id {QString {}}, m_calendar_tz_id {QString {}}, m_calendar_url {QString {}}, m_calendar_type {LocalCalendar::CalendarType::None}, m_calendar {nullptr}, m_cal_controller {nullptr}, m_alarm_checker {new AlarmChecker {this}}, m_busy_downloading {false}
{
    connect(this, &LocalCalendar::calendarIdChanged, this, &LocalCalendar::createCalendar);
    connect(this, &LocalCalendar::calendarTzIdChanged, this, &LocalCalendar::createCalendar);
    connect(this, &LocalCalendar::calendarUrlChanged, this, &LocalCalendar::createCalendar);
    connect(this, &LocalCalendar::calendarTypeChanged, this, &LocalCalendar::createCalendar);
    connect(this, &LocalCalendar::eventsChanged, m_alarm_checker, &AlarmChecker::scheduleAlarmCheck);
}

KCalendarCore::MemoryCalendar::Ptr LocalCalendar::memorycalendar() const
{
    return m_calendar;
}

void LocalCalendar::createCalendar()
{
    if (m_calendar_id.isEmpty() || m_calendar_tz_id.isEmpty() || (m_calendar_type == LocalCalendar::CalendarType::None) || ((m_calendar_type == LocalCalendar::CalendarType::Conference) && m_calendar_url.isEmpty())) {
        qDebug() << "No sufficient calendar information provided";
        return;
    }

    if (m_calendar_type == LocalCalendar::CalendarType::Conference) {
        qDebug() << "Creating online calendar: " <<  m_calendar_id;

        //Check if a local copy of the calendar already exists and set it accordingly to the member property
        m_calendar =  m_cal_controller->memoryCalendar(m_calendar_id);

        //Even if a local copy exists, get a fresh copy of the calendar
        loadOnlineCalendar();
    } else {
        qDebug() << "Creating local calendar: " << m_calendar_id;

        m_calendar = m_cal_controller->createLocalCalendar(m_calendar_id, m_calendar_tz_id.toUtf8());
    }
    Q_EMIT memorycalendarChanged();
    Q_EMIT categoriesChanged();
    Q_EMIT eventsChanged();

}

QStringList LocalCalendar::categories() const
{
    if (m_calendar) {
        return m_calendar->categories();
    }

    return QStringList {};
}

void LocalCalendar::onlineCalendarReady(const QString &calendarId)
{
    qDebug() << "Calendar " << calendarId << " is ready";

    if (calendarId == m_calendar_id) {
        m_calendar = m_cal_controller->memoryCalendar(calendarId);

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
    }
}

void LocalCalendar::loadOnlineCalendar()
{
    m_cal_controller->createCalendarFromUrl(m_calendar_id, QUrl {m_calendar_url}, m_calendar_tz_id.toUtf8());
}

CalendarController *LocalCalendar::calendarController() const
{
    return m_cal_controller;
}

void LocalCalendar::setCalendarController(CalendarController *const controller)
{
    m_cal_controller = controller;

    if (m_cal_controller != nullptr) {
        connect(m_cal_controller, &CalendarController::calendarDownloaded, this, &LocalCalendar::onlineCalendarReady);
        connect(m_cal_controller, &CalendarController::downlading, this, &LocalCalendar::setBusyStatus);
    }

    Q_EMIT calendarControllerChanged();
}

QString LocalCalendar::calendarId() const
{
    return m_calendar_id;
}

void LocalCalendar::setCalendarId(const QString &id)
{
    if (m_calendar_id != id) {
        m_calendar_id = id;
        Q_EMIT calendarIdChanged();
    }
}

int LocalCalendar::calendarType() const
{
    return m_calendar_type;
}

void LocalCalendar::setCalendarType(const int type)
{
    if (m_calendar_type != type) {
        m_calendar_type = type;
        Q_EMIT calendarTypeChanged();
    }
}

QString LocalCalendar::calendarUrl() const
{
    return m_calendar_url;
}

void LocalCalendar::setCalendarUrl(const QString &url)
{
    if (m_calendar_url != url) {
        m_calendar_url = url;
        Q_EMIT calendarUrlChanged();
    }
}

QString LocalCalendar::calendarTzId() const
{
    return m_calendar_tz_id;
}

void LocalCalendar::setCalendarTzId(const QString &tzId)
{
    if (m_calendar_tz_id != tzId) {
        m_calendar_tz_id = tzId;
        Q_EMIT calendarTzIdChanged();
    }
}

bool LocalCalendar::busyDownlading() const
{
    return m_busy_downloading;
}

void LocalCalendar::setBusyStatus(const QString &calendarId, const bool downlading)
{
    if ((m_calendar_id == calendarId) && (m_busy_downloading != downlading)) {
        m_busy_downloading = downlading;
        Q_EMIT busyDownladingChanged();
    }
}

#include "moc_localcalendar.cpp"
