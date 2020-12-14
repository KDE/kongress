/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.1
import QtQml 2.1
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.0 as Kirigami
import org.kde.kongress 0.1 as Kongress

Kirigami.ApplicationWindow {
    id: root

    property Kongress.Conference activeConference: Kongress.ConferenceController.activeConference

    width: Kirigami.Units.gridUnit * 65

    globalDrawer: GlobalDrawer {
        modal: !root.wideScreen
        onModalChanged: drawerOpen = !modal
        activeConference: root.activeConference
        handleVisible: modal
        pageStack: root.pageStack
    }

    pageStack {
        initialPage: Kongress.ConferenceController.defaultConferenceId ? scheduleView : conferencesView
        defaultColumnWidth: Kirigami.Units.gridUnit * 40
    }

    Kongress.LocalCalendar {
        id: onlineCalendar

        calendarController: Kongress.CalendarController
        calendarId: root.activeConference && root.activeConference.id
        calendarUrl: root.activeConference && root.activeConference.icalUrl
        calendarTzId: root.activeConference && root.activeConference.timeZoneId
        calendarType: 1

    }

    Kongress.LocalCalendar {
        id: favoritesCalendar

        calendarController: Kongress.CalendarController

        calendarId: root.activeConference && ("favorites_" +  root.activeConference.id)
        calendarUrl: ""
        calendarTzId: root.activeConference && root.activeConference.timeZoneId
        calendarType: 2
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

            /**
             * Expects @selectedConferenceId variable object to provide the information of the selected conference
             */
            onSelected: {
                Kongress.ConferenceController.activateConference(selectedConferenceId)
                pageStack.pop(root);
                pageStack.push(scheduleView,  {eventStartDt: ""});
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

    Component.onCompleted: Kongress.EventController.calendarController = Kongress.CalendarController
}
