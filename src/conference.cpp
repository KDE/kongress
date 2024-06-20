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

double Conference::venueLatitude() const
{
    return m_venue_latitude;
}

double Conference::venueLongitude() const
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

bool Conference::hasVenueCoordinate() const
{
    return m_venue_longitude >= -180.0 && m_venue_longitude <= 180.0 && m_venue_latitude >= -90.0 && m_venue_latitude <= 90.0;
}

bool Conference::hasVenueIndoorMap() const
{
    return m_indoorMapBbox.isValid() && !m_indoorMapBbox.isEmpty();
}

[[nodiscard]] static double readDoubleValue(const QJsonValue &v)
{
    if (v.isDouble()) {
        return v.toDouble();
    }
    if (v.isString()) {
        bool ok = false;
        const auto n = v.toString().toDouble(&ok);
        return ok ? n : NAN;
    }
    return NAN;
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
    c.m_venue_latitude = readDoubleValue(obj["venueLatitude"_L1]);
    c.m_venue_longitude = readDoubleValue(obj["venueLongitude"_L1]);
    c.m_venue_osm_url = obj["venueOsmUrl"_L1].toString();
    c.m_tz_id = obj["timeZoneId"_L1].toString();

    const auto indoorMap = obj["indoorMap"_L1].toObject();
    if (indoorMap.size() >= 4) {
        c.m_indoorMapBbox = {QPointF(indoorMap["minLongitude"_L1].toDouble(), indoorMap["minLatitude"_L1].toDouble()),
                             QPointF(indoorMap["maxLongitude"_L1].toDouble(), indoorMap["maxLatitude"_L1].toDouble())};
    }

    return c;
}

#include "moc_conference.cpp"
