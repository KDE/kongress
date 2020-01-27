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

    property var activeConference

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer

        title: "Kongress"

        actions: [
            Kirigami.Action {
                id: conferenceSelector

                text: root.activeConference && root.activeConference.name ? root.activeConference.name : i18n("Conference")
                iconName: "group"
                expandible: true

                Kirigami.Action {
                    text: root.activeConference ? i18n("Change conference") : i18n("Select conference")

                    onTriggered: {
                        pageStack.clear();
                        pageStack.push(conferencesView)
                    }
                }

                Kirigami.Action {
                    visible: onlineCalendar && onlineCalendar.loadDateStr != ""
                    text: i18n("Check for updates")

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
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(eventsCardView, {title: i18n("Schedule"), eventStartDt: ""});
                }
            },

            Kirigami.Action {
                id: categoryActions

                text: i18n("Categories")
                iconName: "view-categories"
            },

            Kirigami.Action {
                text: i18n("Favorites")
                iconName: "favorite"
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(favoritesView, {title: i18n("Favorites"), eventStartDt: ""});
                }
            },

            Kirigami.Action {
                text: i18n("Map")
                iconName: "find-location"
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(mapView);
                }
            }
        ]

    }

//     contextDrawer: Kirigami.ContextDrawer {
//         id: contextDrawer
//     }

    pageStack {
        initialPage: [conferencesView]
        separatorVisible: false
    }

    Kongress.ConferenceModel {
        id: conferenceModel
    }

    Kongress.LocalCalendar {
        id: onlineCalendar

        onCalendarInfoChanged: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }

        onLoadDateStrChanged: showPassiveNotification(i18n("Conference data loaded"))
    }

    Kongress.LocalCalendar {
        id: favoritesCalendar

        calendarInfo: {"id": "favorites",  "controller": _calendarController}

        onCalendarInfoChanged: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }
    }

    Component {
        id: eventsCardView

        EventsCardListView {
            roCalendar: onlineCalendar
            rwCalendar: favoritesCalendar
            viewMode: "events"

            title: i18n("Schedule")
        }
    }

    Component {
        id: eventsListView

        EventsListView {
            roCalendar: onlineCalendar
            rwCalendar: favoritesCalendar
            viewMode: "events"
        }
    }

    Component {
        id: favoritesView

        EventsCardListView {
            roCalendar: favoritesCalendar
            rwCalendar: favoritesCalendar
            viewMode: "favorites"
        }
    }

    Component {
        id: conferencesView

        Conferences {
            conferencesList: conferenceModel

            /**
             * Expects @selectedConference variable object to provide the information of the selected conference
             */
            onSelected: {
                pageStack.clear();
                root.activeConference = selectedConference;
                onlineCalendar.calendarInfo = {"id": root.activeConference.id, "controller": _calendarController, "url": root.activeConference.icalUrl};
                showPassiveNotification(i18n("Loading conference data"));
                pageStack.push(eventsCardView, {title: i18n("Schedule"), eventStartDt: ""});
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

        model: (root.activeConference && root.activeConference.days) ? root.activeConference.days: []

        delegate: Kirigami.Action {
            property date conferenceDay: new Date(modelData)

            text: conferenceDay.toLocaleDateString(Qt.locale(), "dddd")

            onTriggered: {
                pageStack.clear();
                pageStack.push(eventsListView, {title: conferenceDay.toLocaleDateString(Qt.locale(), "dddd"), eventStartDt: conferenceDay});
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
                pageStack.push(eventsCardView, {title: text, eventStartDt: "", category: text, showCategories: false});
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
