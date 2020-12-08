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

ConferenceModel::ConferenceModel(QObject *parent) : QAbstractListModel {parent}, m_controller {nullptr}, m_conferences {QVector<Conference*> {}}, m_busy_downloading {false}
{
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

int ConferenceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_conferences.count();
}

QVariant ConferenceModel::data(const QModelIndex &index, int role) const
{
    auto row = index.row();

    switch (role) {
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

    if (m_controller != nullptr) {
        m_conferences = m_controller->conferences();
    }

    endResetModel();
}

QString ConferenceModel::pastOrUpcoming(const int index) const
{
    auto days = m_conferences.at(index)->days();
    auto pastLabel = i18n("Past");
    auto upcoming = i18n("Upcoming");

    if (days.isEmpty()) {
        return pastLabel;
    }

    auto lastDayDt = QDate::fromString(days.last(), "yyyy-MM-dd");
    auto currentDt = QDate::currentDate();

    if (lastDayDt.isValid()) {
        return lastDayDt < currentDt ? pastLabel : upcoming;
    }

    return pastLabel;
}

void ConferenceModel::setController(ConferenceController *conferenceController)
{
    if (m_controller == conferenceController) {
        return;
    }

    m_controller = conferenceController;

    if (m_controller != nullptr) {
        m_controller->loadConferences();
        connect(m_controller, &ConferenceController::conferencesLoaded, this, &ConferenceModel::loadConferences);
        connect(m_controller, &ConferenceController::downlading, this, &ConferenceModel::setBusyStatus);
    }

    Q_EMIT controllerChanged();
}

ConferenceController *ConferenceModel::controller() const
{
    return m_controller;
}

void ConferenceModel::setBusyStatus(const bool downlading)
{
    if (m_busy_downloading != downlading) {
        m_busy_downloading = downlading;
        Q_EMIT busyDownladingChanged();
    }
}

bool ConferenceModel::busyDownlading() const
{
    return m_busy_downloading;
}
