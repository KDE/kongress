/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <QObject>

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
    Q_PROPERTY(QString venueLatitude READ venueLatitude)
    Q_PROPERTY(QString venueLongitude READ venueLongitude)
    Q_PROPERTY(QString venueOsmUrl READ venueOsmUrl)
    Q_PROPERTY(QString timeZoneId READ timeZoneId)

public:
    QString id() const;
    QString name() const;
    QString description() const;
    QString icalUrl() const;
    QStringList days() const;
    QString venueImageUrl() const;
    QString venueLatitude() const;
    QString venueLongitude() const;
    QString venueOsmUrl() const;
    QString timeZoneId() const;

    [[nodiscard]] static Conference fromJson(const QJsonObject &obj);

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QString m_ical_url;
    QStringList m_days;
    QString m_venue_image_url;
    QString m_venue_latitude;
    QString m_venue_longitude;
    QString m_venue_osm_url;
    QString m_tz_id;
};
#endif
