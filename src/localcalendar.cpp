
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

#include "localcalendar.h"
#include "calendarcontroller.h"
#include "alarmchecker.h"
#include <QDebug>
#include <QMetaType>
#include <KLocalizedString>

LocalCalendar::LocalCalendar(QObject *parent)
    : QObject {parent}, m_calendarInfo {QVariantMap {}}, m_calendar {nullptr}, m_cal_controller {nullptr},  m_alarm_checker {new AlarmChecker {this}}
{
    connect(this, &LocalCalendar::eventsChanged, m_alarm_checker, &AlarmChecker::scheduleAlarmCheck);
}

KCalendarCore::MemoryCalendar::Ptr LocalCalendar::memorycalendar() const
{
    return m_calendar;
}

void LocalCalendar::setCalendarInfo(const QVariantMap &calendarInfoMap)
{
    if (!calendarInfoMap.contains("id") || calendarInfoMap["id"].toString().isEmpty()) {
        qDebug() << "No sufficient calendar information provided";

        return;
    }

    m_calendarInfo["id"] = calendarInfoMap["id"].toString();
    m_calendarInfo["timeZoneId"] = calendarInfoMap.contains("timeZoneId") && !(calendarInfoMap["timeZoneId"].toString().isEmpty()) ? calendarInfoMap["timeZoneId"].toString() : QTimeZone::systemTimeZoneId();

    if (calendarInfoMap.contains("url")) {
        qDebug() << "Creating online calendar: " <<  m_calendarInfo["id"].toString() ;

        m_calendarInfo["url"] = calendarInfoMap["url"].toString();

        //Check if a local copy of the calendar already exists and set it accordingly to the member property
        m_calendar =  m_cal_controller->memoryCalendar(m_calendarInfo["id"].toString());

        //Even if a local copy exists, get a fresh copy of the calendar
        loadOnlineCalendar();
    } else {
        qDebug() << "Creating local calendar: " << m_calendarInfo["id"].toString() ;

        m_calendar = m_cal_controller->createLocalCalendar(m_calendarInfo["id"].toString(), m_calendarInfo["timeZoneId"].toByteArray());
    }

    Q_EMIT memorycalendarChanged();
    Q_EMIT categoriesChanged();
    Q_EMIT eventsChanged();
    Q_EMIT calendarInfoChanged();
}

QVariantMap LocalCalendar::calendarInfo() const
{
    return m_calendarInfo;
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

    if (calendarId == m_calendarInfo["id"].toString()) {
        m_calendar = m_cal_controller->memoryCalendar(calendarId);
        m_calendarInfo["lastUpdated"] = QDateTime::currentDateTime();

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
    }
}

QString LocalCalendar::calendarId() const
{
    if (m_calendarInfo.contains("id")) {
        return m_calendarInfo["id"].toString();
    }

    return QString {};
}

void LocalCalendar::loadOnlineCalendar()
{
    if (!(m_calendarInfo.contains("id") || !(m_calendarInfo.contains("url")))) {
        return;
    }

    m_cal_controller->createCalendarFromUrl(m_calendarInfo["id"].toString(), QUrl::fromEncoded(m_calendarInfo["url"].toByteArray()), m_calendarInfo["timeZoneId"].toByteArray());
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
    }

    Q_EMIT calendarControllerChanged();
}
