/*
 *   Copyright 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQml 2.1
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.0 as Kirigami
import org.kde.kongress 0.1 as Kongress

Kirigami.ApplicationWindow {
    id: root

    property Kongress.Conference activeConference: conferenceController.activeConferenceInfo

    globalDrawer: GlobalDrawer {}

    pageStack {
        initialPage: activeConference.id != "" ? scheduleView : conferencesView
        defaultColumnWidth: Kirigami.Units.gridUnit * 40
    }

    Kongress.ConferenceController {
        id: conferenceController

        onActiveConferenceInfoChanged: {
            if (activeConferenceInfo.id != "") {
                pageStack.clear();
                pageStack.push(scheduleView, {title: i18n("Schedule"), eventStartDt: ""});
            }
        }
    }

    Kongress.ConferenceModel {
        id: conferenceModel

        controller: conferenceController
    }

    Kongress.LocalCalendar {
        id: onlineCalendar

        calendarController: _calendarController
        calendarId: root.activeConference && root.activeConference.id
        calendarUrl: root.activeConference && root.activeConference.icalUrl
        calendarTzId: root.activeConference && root.activeConference.timeZoneId
        calendarType: 1

        onCalendarIdChanged: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }
    }

    Kongress.LocalCalendar {
        id: favoritesCalendar

        calendarController: _calendarController

        calendarId: root.activeConference && ("favorites_" +  root.activeConference.id)
        calendarUrl: ""
        calendarTzId: root.activeConference && root.activeConference.timeZoneId
        calendarType: 2

        onCalendarIdChanged: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }
    }

    Component {
        id: scheduleView

        ScheduleView {
            roCalendar: onlineCalendar
            rwCalendar: favoritesCalendar

            title: i18n("Schedule")
        }
    }

    Component {
        id: favoritesView

        FavoritesView {
            roCalendar: favoritesCalendar
            rwCalendar: favoritesCalendar

            title: i18n("Favorites")

        }
    }

    Component {
        id: conferencesView

        Conferences {
            conferencesList: conferenceModel

            /**
             * Expects @selectedConferenceId variable object to provide the information of the selected conference
             */
            onSelected: {
                conferenceController.defaultConferenceId = selectedConferenceId;
            }
        }
    }

    Component {
        id: mapView

        MapView {
            imageUrl: root.activeConference.venueImageUrl
            latitude: root.activeConference.venueLatitude
            longitude: root.activeConference.venueLongitude
            geoUrl: root.activeConference.venueOsmUrl
        }
    }
}
