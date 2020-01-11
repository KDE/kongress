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

#include "conference.h"

QString Conference::id() const
{
    return m_id;
}

QString Conference::name() const
{
    return m_name;
}

QString Conference::description() const
{
    return m_description;
}

QString Conference::icalUrl() const
{
    return m_ical_url;
}

QStringList Conference::days() const
{
    return m_days;
}

QString Conference::venueImageUrl() const
{
    return m_venue_image_url;
}

QString Conference::venueLatitude() const
{
    return m_venue_latitude;
}

QString Conference::venueLongitude() const
{
    return m_venue_longitude;

}

QString Conference::venueOsmUrl() const
{
    return m_venue_osm_url;
}

void Conference::setId(const QString& id)
{
    if(id != m_id)
    {
        m_id = id;
    }
}

void Conference::setName(const QString& name)
{
    if(name != m_name)
    {
        m_name = name;
    }
}

void Conference::setDescription(const QString& description)
{
    if(description != m_description)
    {
        m_description = description;
    }
}

void Conference::setDays(const QStringList& days)
{
    if(days != m_days)
    {
        m_days = days;
    }
}

void Conference::setIcalUrl(const QString& icalUrl)
{
    if(icalUrl != m_description)
    {
        m_ical_url = icalUrl;
    }
}

void Conference::setVenueImageUrl(const QString& url)
{
    if(url != m_venue_image_url)
    {
        m_venue_image_url = url;
    }
}

void Conference::setVenueLongitude(const QString& longitude)
{
    if(longitude != m_venue_longitude)
    {
        m_venue_longitude = longitude;
    }
}

void Conference::setVenueLatitude(const QString& latitude)
{
    if(latitude != m_venue_latitude)
    {
        m_venue_latitude = latitude;
    }
}

void Conference::setVenueOsmUrl(const QString& url)
{
    if(url != m_venue_osm_url)
    {
        m_venue_osm_url = url;
    }
}
