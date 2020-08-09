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

#ifndef CONFERENCEMODEL_H
#define CONFERENCEMODEL_H

#include <QAbstractListModel>
#include <QVariantMap>
#include <QVector>
#include <QHash>

class Conference;
class ConferenceController;

class ConferenceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(ConferenceController *controller READ controller WRITE setController NOTIFY controllerChanged)

public:
    enum Roles
    {
        ConferenceId=Qt::UserRole+1,
        ConferenceName,
        ConferenceDescription,
        ConferenceIcalUrl,
        ConferenceDays,
        ConferenceVenueImageUrl,
        ConferenceVenueLongitude,
        ConferenceVenueLatitude,
        ConferenceVenueOsmUrl,
        ConferenceTimeZone,
        PastUpcoming
    };

    explicit ConferenceModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    QVariantMap filter() const;
    void setFilter(const QVariantMap& filter);

    ConferenceController *controller() const;
    void setController(ConferenceController *conferenceController);

Q_SIGNALS:
    void filterChanged();
    void controllerChanged();

private:
    void loadConferences();
    QString pastOrUpcoming(const int index) const;

    ConferenceController *m_controller;
    QVector<Conference*> m_conferences;
    QVariantMap m_filter;
};
#endif
