/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "conference.h"

#include <QJsonObject>

using namespace Qt::Literals::StringLiterals;

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

QString Conference::timeZoneId() const
{
    return m_tz_id;
}

Conference Conference::fromJson(const QJsonObject &obj)
{
    Conference c;
    c.m_id = obj["id"_L1].toString();
    c.m_name = obj["name"_L1].toString();
    c.m_description = obj["description"_L1].toString();
    c.m_ical_url = obj["icalUrl"_L1].toString();
    auto jsonDays = obj["days"_L1].toVariant();
    c.m_days = jsonDays.toStringList();
    c.m_venue_image_url = obj["venueImageUrl"_L1].toString();
    c.m_venue_latitude = obj["venueLatitude"_L1].toString();
    c.m_venue_longitude = obj["venueLongitude"_L1].toString();
    c.m_venue_osm_url = obj["venueOsmUrl"_L1].toString();
    c.m_tz_id = obj["timeZoneId"_L1].toString();
    return c;
}

#include "moc_conference.cpp"
