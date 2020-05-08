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

#include <QDate>
#include <KLocalizedString>
#include "conferencemodel.h"
#include "conferencecontroller.h"
#include "conference.h"

ConferenceModel::ConferenceModel(QObject* parent) : QAbstractListModel(parent), m_controller(new ConferenceController()), m_conferences(QVector<Conference*>()), m_filter(QVariantMap())
{
    loadConferences();
    connect(this, &ConferenceModel::filterChanged, this, &ConferenceModel::loadConferences);
    connect(m_controller, &ConferenceController::conferencesChanged, this, &ConferenceModel::loadConferences);
}

QHash<int, QByteArray> ConferenceModel::roleNames() const
{
    return {
        {ConferenceId, "id"},
        {ConferenceName, "name"},
        {ConferenceDescription, "description"},
        {ConferenceIcalUrl, "icalUrl"},
        {ConferenceDays, "days"},
        {ConferenceVenueImageUrl, "venueImageUrl"},
        {ConferenceVenueLatitude, "venueLatitude"},
        {ConferenceVenueLongitude, "venueLongitude"},
        {ConferenceVenueOsmUrl, "venueOsmUrl"},
        {ConferenceTimeZone, "timeZoneId"},
        {PastUpcoming, "pastUpcoming"}
    };
}

int ConferenceModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_conferences.count();
}

QVariant ConferenceModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    switch(role)
    {
        case ConferenceId:
            return m_conferences.at(row)->id();
        case ConferenceName:
            return m_conferences.at(row)->name();
        case ConferenceDescription:
            return m_conferences.at(row)->description();
        case ConferenceIcalUrl:
            return m_conferences.at(row)->icalUrl();
        case ConferenceDays:
            return m_conferences.at(row)->days();
        case ConferenceVenueImageUrl:
            return m_conferences.at(row)->venueImageUrl();
        case ConferenceVenueLatitude:
            return m_conferences.at(row)->venueLatitude();
        case ConferenceVenueLongitude:
            return m_conferences.at(row)->venueLongitude();
        case ConferenceVenueOsmUrl:
            return m_conferences.at(row)->venueOsmUrl();
        case ConferenceTimeZone:
            return m_conferences.at(row)->timeZoneId();
        case PastUpcoming:
            return pastOrUpcoming(row);
        default:
            return m_conferences.at(row)->id();
    }
}

void ConferenceModel::loadConferences()
{
    beginResetModel();

    m_conferences = m_controller->conferences();

    if(!m_filter.isEmpty())
    {
        //TODO: Implement model filtering
    }

    endResetModel();
}

QVariantMap ConferenceModel::filter() const
{
    return m_filter;
}

void ConferenceModel::setFilter(const QVariantMap& filter)
{
    if(m_filter != filter) m_filter = filter;

    Q_EMIT filterChanged();
}

QString ConferenceModel::pastOrUpcoming(const int index) const
{
    auto days = m_conferences.at(index)->days();
    auto pastLabel = i18n("Past");
    auto upcoming = i18n("Upcoming");

    if(days.isEmpty())
    {
        return pastLabel;
    }

    auto lastDayDt = QDate::fromString(days.last(), "yyyy-MM-dd");
    auto currentDt = QDate::currentDate();

    if(lastDayDt.isValid())
    {
        return lastDayDt < currentDt ? pastLabel : upcoming;
    }

    return pastLabel;
}