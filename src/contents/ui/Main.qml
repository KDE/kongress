/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kongress as Kongress

Kirigami.ApplicationWindow {
    id: root

    property Kongress.Conference activeConference: Kongress.ConferenceController.activeConference

    readonly property bool hasConference: Kongress.ConferenceController.defaultConferenceId !== ""

    width: Kirigami.Units.gridUnit * 65

    globalDrawer: GlobalDrawer {
        modal: !root.wideScreen
        onModalChanged: drawerOpen = !modal
        activeConference: root.activeConference
        hasConference: root.hasConference
        handleVisible: modal
        pageStack: root.pageStack
    }

    contextDrawer: Kirigami.ContextDrawer {}

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
            onSelected: selectedConferenceId => {
                Kongress.ConferenceController.activateConference(selectedConferenceId)
                pageStack.pop(root);
                pageStack.push(scheduleView,  {eventStartDt: ""});
            }
        }
    }

    Component {
        id: mapView

        MapView {
            conference: root.activeConference
        }
    }
    Component {
        id: indoorMapView
        IndoorMapView {
            conference: root.activeConference
        }
    }
}
