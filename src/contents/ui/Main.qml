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
import QtQuick.Controls 2.4 as Controls2
import org.kde.kongress 0.1 as Kongress

Kirigami.ApplicationWindow {
    id: root

    property Kongress.Conference activeConference: conferenceController.activeConferenceInfo

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer

        title: "Kongress"

        actions: [
            Kirigami.Action {
                id: conferenceSelector

                text: root.activeConference.name != "" ? root.activeConference.name : i18n("Conference")
                iconName: "group"
                expandible: true

                Kirigami.Action {
                    text: i18n("Check for updates")
                    enabled: root.activeConference.id != ""

                    onTriggered: {
                        showPassiveNotification(i18n("Loading conference data"));
                        onlineCalendar.loadOnlineCalendar();
                    }
                }
            },

            Kirigami.Action {
                id: dayActions

                text: i18n("Daily Schedule")
                iconName: "view-calendar-day"
                expandible: true
            },

            Kirigami.Action {
                text: i18n("Full Schedule")
                iconName: "view-calendar-agenda"
                enabled: root.activeConference.id != ""
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(scheduleView, {title: i18n("Schedule"), eventStartDt: ""});
                }
            },

            Kirigami.Action {
                id: categoryActions

                text: i18n("Categories")
                iconName: "view-categories"
                enabled: root.activeConference.id != "" && children.length > 0
            },

            Kirigami.Action {
                text: i18n("Map")
                iconName: "find-location"
                enabled: root.activeConference.venueOsmUrl != ""
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(mapView);
                }
            },

            Kirigami.Action {
                text: i18n("Favorites")
                iconName: "favorite"
                enabled: root.activeConference.id != ""

                onTriggered: {
                    pageStack.clear();
                    pageStack.push(favoritesView, {title: i18n("Favorites"), eventStartDt: ""});
                }
            },

            Kirigami.Action {
                text: i18n("Configure")
                iconName: "settings-configure"
                expandible: true

                Kirigami.Action {
                    text: root.activeConference.id != "" ? i18n("Change conference") : i18n("Select conference")
                    iconName: root.activeConference.id != "" ? 'exchange-positions' : 'edit-select'

                    onTriggered: {
                        pageStack.clear();
                        pageStack.push(conferencesView)
                    }
                }
            }

        ]
    }

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
        calendarInfo: root.activeConference.id != "" ? {"id": root.activeConference.id, "url": root.activeConference.icalUrl, "timeZoneId": root.activeConference.timeZoneId, } : {}

        onCalendarInfoChanged: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }

        onLoadDateStrChanged: showPassiveNotification(i18n("Conference data loaded"))
    }

    Kongress.LocalCalendar {
        id: favoritesCalendar

        calendarController: _calendarController
        calendarInfo: root.activeConference.id != "" ? {"id": "favorites_" +  root.activeConference.id, "timeZoneId": root.activeConference.timeZoneId } : {}

        onCalendarInfoChanged: {
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
                showPassiveNotification(i18n("Loading conference data"));
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

    Instantiator { //TODO: When swithcing to Qt >= 5.14, it will be found in QtQml.Models 2.14
        id: daysInstantiator

        model: root.activeConference.days != "" ? root.activeConference.days: []

        delegate: Kirigami.Action {
            property date conferenceDay: new Date(modelData)

            text: conferenceDay.toLocaleDateString(Qt.locale(), "dddd")

            onTriggered: {
                pageStack.clear();
                pageStack.push(scheduleView, {title: conferenceDay.toLocaleDateString(Qt.locale(), "dddd"), eventStartDt: conferenceDay });
            }
        }

        onObjectAdded: {
            if(!isNaN(object.conferenceDay)) {
                dayActions.children.push(object)
            }
        }
        onObjectRemoved: {
            if(dayActions.children) {
                dayActions.children = [];
            }
        }
    }

    Instantiator { //TODO: When swithcing to Qt >= 5.14, it will be found in QtQml.Models 2.14
        model: (onlineCalendar && onlineCalendar.categories) ? onlineCalendar.categories : []

        delegate: Kirigami.Action {
            text: modelData

            onTriggered: {
                pageStack.clear();
                pageStack.push(scheduleView, {title: text, eventStartDt: "", category: text, showCategories: false});
            }
        }

        onObjectAdded: {
            if(object.text != "") {
                categoryActions.children.push(object)
            }
        }

        onObjectRemoved: {
            if(categoryActions.children) {
                categoryActions.children = [];
            }
        }
    }
}
