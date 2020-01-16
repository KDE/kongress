
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
#include <KLocalizedString>

using namespace KCalendarCore;

LocalCalendar::LocalCalendar(QObject* parent)
: QObject(parent), m_calendarInfo(QVariantMap()), m_cal_controller(nullptr)
{
}

LocalCalendar::~LocalCalendar() = default;

MemoryCalendar::Ptr LocalCalendar::memorycalendar() const
{
    return m_calendar;
}

void LocalCalendar::setCalendarInfo(const QVariantMap& calendarInfoMap)
{
    if(!calendarInfoMap.contains("id") || !calendarInfoMap.contains("controller"))
    {
        qDebug() << "No sufficient calendar information provided";

        return;
    }

    m_cal_controller = calendarInfoMap["controller"].value<CalendarController*>();

    if(m_cal_controller != nullptr)
    {
        connect(m_cal_controller, &CalendarController::calendarDownloaded, this, &LocalCalendar::onlineCalendarReady);
    }

    if (calendarInfoMap.contains("url"))
    {
        qDebug() << "Creating online calendar: " << calendarInfoMap["id"].toString() ;

        m_calendarInfo["id"] = calendarInfoMap["id"].toString();
        m_calendarInfo["url"] = calendarInfoMap["url"].toString();

        m_cal_controller->createCalendarFromUrl(calendarInfoMap["id"].toString(), QUrl::fromEncoded(calendarInfoMap["url"].toByteArray()));
    }
    else
    {
        qDebug() << "Creating local calendar: " << calendarInfoMap["id"].toString() ;

        m_calendarInfo["id"] = calendarInfoMap["id"].toString();
        m_calendar = m_cal_controller->createLocalCalendar(calendarInfoMap["id"].toString());

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
    }
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

        Q_EMIT memorycalendarChanged();
        Q_EMIT categoriesChanged();
        Q_EMIT eventsChanged();
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
