/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "conferencemodel.h"
#include "conference.h"
#include "conferencecontroller.h"
#include <KLocalizedString>
#include <QDate>

ConferenceModel::ConferenceModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_controller{nullptr}
    , m_busy_downloading{false}
{
}

QHash<int, QByteArray> ConferenceModel::roleNames() const
{
    return {{ConferenceId, "id"},
            {ConferenceName, "name"},
            {ConferenceDescription, "description"},
            {ConferenceIcalUrl, "icalUrl"},
            {ConferenceDays, "days"},
            {ConferenceVenueImageUrl, "venueImageUrl"},
            {ConferenceVenueLatitude, "venueLatitude"},
            {ConferenceVenueLongitude, "venueLongitude"},
            {ConferenceVenueOsmUrl, "venueOsmUrl"},
            {ConferenceTimeZone, "timeZoneId"},
            {PastUpcoming, "pastUpcoming"}};
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
        return m_conferences.at(row).id();
    case ConferenceName:
        return m_conferences.at(row).name();
    case ConferenceDescription:
        return m_conferences.at(row).description();
    case ConferenceIcalUrl:
        return m_conferences.at(row).icalUrl();
    case ConferenceDays:
        return m_conferences.at(row).days();
    case ConferenceVenueImageUrl:
        return m_conferences.at(row).venueImageUrl();
    case ConferenceVenueLatitude:
        return m_conferences.at(row).venueLatitude();
    case ConferenceVenueLongitude:
        return m_conferences.at(row).venueLongitude();
    case ConferenceVenueOsmUrl:
        return m_conferences.at(row).venueOsmUrl();
    case ConferenceTimeZone:
        return m_conferences.at(row).timeZoneId();
    case PastUpcoming:
        return pastOrUpcoming(row);
    default:
        return m_conferences.at(row).id();
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
    auto days = m_conferences.at(index).days();
    auto pastLabel = i18n("Past");
    auto upcoming = i18n("Upcoming");

    if (days.isEmpty()) {
        return pastLabel;
    }

    auto lastDayDt = QDate::fromString(days.last(), u"yyyy-MM-dd");
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

#include "moc_conferencemodel.cpp"
