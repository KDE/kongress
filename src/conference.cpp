/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "conference.h"
#include <QDebug>

Conference::Conference(QObject *parent) : QObject {parent} {};

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

void Conference::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        Q_EMIT idChanged();
    }
}

void Conference::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged();
    }
}

void Conference::setDescription(const QString &description)
{
    if (description != m_description) {
        m_description = description;
        Q_EMIT descriptionChanged();
    }
}

void Conference::setDays(const QStringList &days)
{
    if (days != m_days) {
        m_days = days;
        Q_EMIT daysChanged();
    }
}

void Conference::setIcalUrl(const QString &icalUrl)
{
    if (icalUrl != m_ical_url) {
        m_ical_url = icalUrl;
        Q_EMIT icalUrlChanged();
    }
}

void Conference::setVenueImageUrl(const QString &url)
{
    if (url != m_venue_image_url) {
        m_venue_image_url = url;
        Q_EMIT venueImageUrlChanged();
    }
}

void Conference::setVenueLongitude(const QString &longitude)
{
    if (longitude != m_venue_longitude) {
        m_venue_longitude = longitude;
        Q_EMIT venueLongitudeChanged();
    }
}

void Conference::setVenueLatitude(const QString &latitude)
{
    if (latitude != m_venue_latitude) {
        m_venue_latitude = latitude;
        Q_EMIT venueLatitudeChanged();
    }
}

void Conference::setVenueOsmUrl(const QString &url)
{
    if (url != m_venue_osm_url) {
        m_venue_osm_url = url;
        Q_EMIT venueOsmUrlChanged();
    }
}

void Conference::setTimeZoneId(const QString &id)
{
    if (id != m_tz_id) {
        m_tz_id = id;
        Q_EMIT timeZoneIdChanged();
    }
}
