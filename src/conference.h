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

#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <QObject>

class Conference : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString icalUrl READ icalUrl WRITE setIcalUrl NOTIFY icalUrlChanged)
    Q_PROPERTY(QStringList days READ days WRITE setDays NOTIFY daysChanged)
    Q_PROPERTY(QString venueImageUrl READ venueImageUrl WRITE setVenueImageUrl NOTIFY venueImageUrlChanged)
    Q_PROPERTY(QString venueLatitude READ venueLatitude WRITE setVenueLatitude NOTIFY venueLatitudeChanged)
    Q_PROPERTY(QString venueLongitude READ venueLongitude WRITE setVenueLongitude NOTIFY venueLongitudeChanged)
    Q_PROPERTY(QString venueOsmUrl READ venueOsmUrl WRITE setVenueOsmUrl NOTIFY venueOsmUrlChanged)
    Q_PROPERTY(QString timeZoneId READ timeZoneId WRITE setTimeZoneId NOTIFY timeZoneIdChanged)

public:
    explicit Conference(QObject *parent = nullptr);

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

    void setId(const QString &id);
    void setName(const QString &name);
    void setDescription(const QString &description);
    void setIcalUrl(const QString &icalUrl);
    void setDays(const QStringList &days);
    void setVenueImageUrl(const QString &url);
    void setVenueLatitude(const QString &latitude);
    void setVenueLongitude(const QString &longitude);
    void setVenueOsmUrl(const QString &url);
    void setTimeZoneId(const QString &id);

Q_SIGNALS:
    void idChanged();
    void nameChanged();
    void descriptionChanged();
    void icalUrlChanged();
    void daysChanged();
    void venueImageUrlChanged();
    void venueLatitudeChanged();
    void venueLongitudeChanged();
    void venueOsmUrlChanged();
    void timeZoneIdChanged();
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
