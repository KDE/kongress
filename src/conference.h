/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <QObject>
#include <QRectF>

class QJsonObject;

class Conference
{
    Q_GADGET

    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString icalUrl READ icalUrl)
    Q_PROPERTY(QStringList days READ days)
    Q_PROPERTY(QString venueImageUrl READ venueImageUrl)
    Q_PROPERTY(double venueLatitude READ venueLatitude)
    Q_PROPERTY(double venueLongitude READ venueLongitude)
    Q_PROPERTY(QString venueOsmUrl READ venueOsmUrl)
    Q_PROPERTY(QString timeZoneId READ timeZoneId)
    Q_PROPERTY(QRectF indoorMapBoundingBox MEMBER m_indoorMapBbox)
    Q_PROPERTY(QString indoorMapStyle MEMBER m_indoorMapStyle)

    Q_PROPERTY(bool hasVenueCoordinate READ hasVenueCoordinate)
    Q_PROPERTY(bool hasVenueIndoorMap READ hasVenueIndoorMap)

public:
    QString id() const;
    QString name() const;
    QString description() const;
    QString icalUrl() const;
    QStringList days() const;
    QString venueImageUrl() const;
    [[nodiscard]] double venueLatitude() const;
    [[nodiscard]] double venueLongitude() const;
    QString venueOsmUrl() const;
    QString timeZoneId() const;

    [[nodiscard]] bool hasVenueCoordinate() const;
    [[nodiscard]] bool hasVenueIndoorMap() const;

    [[nodiscard]] static Conference fromJson(const QJsonObject &obj);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QString m_ical_url;
    QStringList m_days;
    QString m_venue_image_url;
    double m_venue_latitude = NAN;
    double m_venue_longitude = NAN;
    QString m_venue_osm_url;
    QString m_tz_id;
    QRectF m_indoorMapBbox;
    QString m_indoorMapStyle;
};
#endif
