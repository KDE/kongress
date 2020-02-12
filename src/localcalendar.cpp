
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
#include <QDebug>
#include <QMetaType>
#include <KLocalizedString>

using namespace KCalendarCore;

LocalCalendar::LocalCalendar(QObject* parent)
: QObject(parent), m_calendarInfo(QVariantMap()), m_calendar(nullptr), m_cal_controller(nullptr)
{
}

LocalCalendar::~LocalCalendar() = default;

MemoryCalendar::Ptr LocalCalendar::memorycalendar() const
{
    return m_calendar;
}

void LocalCalendar::setCalendarInfo(const QVariantMap& calendarInfoMap)
{
    if(!calendarInfoMap.contains("id") || !calendarInfoMap.contains("controller") || !calendarInfoMap.contains("timeZoneId"))
    {
        qDebug() << "No sufficient calendar information provided";

        return;
    }

    m_cal_controller = calendarInfoMap["controller"].value<CalendarController*>();
    m_calendarInfo["id"] = calendarInfoMap["id"].toString();
    m_calendarInfo["timeZoneId"] = calendarInfoMap["timeZoneId"].toString();

    if(m_cal_controller != nullptr)
    {
        connect(m_cal_controller, &CalendarController::calendarDownloaded, this, &LocalCalendar::onlineCalendarReady);
    }

    if (calendarInfoMap.contains("url"))
    {
        qDebug() << "Creating online calendar: " << calendarInfoMap["id"].toString() ;

        m_calendarInfo["url"] = calendarInfoMap["url"].toString();

        //Check if a local copy of the calendar already exists
        m_calendar =  m_cal_controller->memoryCalendar(m_calendarInfo["id"].toString());

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();

        //Even if a local copy exists, get a fresh copy of the calendar
        loadOnlineCalendar();
    }
    else
    {
        qDebug() << "Creating local calendar: " << calendarInfoMap["id"].toString() ;

        m_calendarInfo["id"] = calendarInfoMap["id"].toString();
        m_calendar = m_cal_controller->createLocalCalendar(calendarInfoMap["id"].toString(), m_calendarInfo["timeZoneId"].toByteArray());

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
    }

    Q_EMIT calendarInfoChanged();
}

QVariantMap LocalCalendar::calendarInfo() const
{
    return m_calendarInfo;
}

QStringList LocalCalendar::categories() const
{
    if(m_calendar)
    {
        return m_calendar->categories();
    }

    return QStringList();
}

void LocalCalendar::onlineCalendarReady(const QString& calendarId)
{
    qDebug() << "Calendar " << calendarId << " is ready";

    if(calendarId == m_calendarInfo["id"].toString())
    {
        m_calendar = m_cal_controller->memoryCalendar(calendarId);
        m_calendarInfo["lastUpdated"] = QDateTime::currentDateTime();

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
        Q_EMIT loadDateStrChanged();
    }
}

QString LocalCalendar::calendarId() const
{
    if(m_calendarInfo.contains("id"))
    {
        return m_calendarInfo["id"].toString();
    }

    return QString();
}

QString LocalCalendar::loadDateStr() const
{
    if(!(m_calendarInfo.contains("lastUpdated")) || !(m_calendarInfo["lastUpdated"].canConvert(QMetaType::QDateTime)) || !(m_calendarInfo["lastUpdated"].toDateTime().isValid()))
    {
        return QString();
    }

    auto todayDate = QDate::currentDate();
    auto loadDate = m_calendarInfo["lastUpdated"].toDate();

    if(loadDate == todayDate)
    {
        return i18n("today at %1", m_calendarInfo["lastUpdated"].toDateTime().toString("HH:mm"));
    }

    if(loadDate.addDays(1) == todayDate)
    {
        return i18n("yesterday at %1", m_calendarInfo["lastUpdated"].toDateTime().toString("HH:mm"));
    }

    return i18n("%1 at %2", loadDate.toString(Qt::SystemLocaleShortDate), m_calendarInfo["lastUpdated"].toDateTime().toString("HH:mm"));
}

void LocalCalendar::loadOnlineCalendar()
{
    if(!(m_calendarInfo.contains("id") || !(m_calendarInfo.contains("url"))))
    {
        return;
    }

    m_cal_controller->createCalendarFromUrl(m_calendarInfo["id"].toString(), QUrl::fromEncoded(m_calendarInfo["url"].toByteArray()), m_calendarInfo["timeZoneId"].toByteArray());
}
