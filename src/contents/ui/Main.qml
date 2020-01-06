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
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.0 as Kirigami
import QtQuick.Controls 2.4 as Controls2
import org.kde.phone.kdefosdem 0.1 as KDEFOSDEM
import "ConferenceData.js" as ConferenceData

Kirigami.ApplicationWindow {
    id: root

    /**
     * To be emitted when data displayed should be refreshed
     */
    signal refreshNeeded;

    property int activeConference: 0
    property string conference: activeConference >= 0 ? ConferenceData.conferences[activeConference].name : []
    property var categories: activeConference >= 0 ? ConferenceData.conferences[activeConference].categories : []
    property var conferenceDays: activeConference >= 0 ? ConferenceData.conferences[activeConference].days : []

    function loadDynamicActions()
    {
        var categoryChildren = [];
        for (var i=0; i < categories.length; ++i)
        {
            categoryChildren.push(categoryAction.createObject(categoryActions, {text: root.categories[i]}));
        }
        categoryActions.children = categoryChildren;

        var dayChildren = [];
        for (var i=0; i < conferenceDays.length; ++i)
        {
            dayChildren.push(dayAction.createObject(dayActions, { conferenceDay: new Date(root.conferenceDays[i]) }));
        }
        dayActions.children = dayChildren;

        var conferenceNames = [];
        for (var i=0; i < ConferenceData.conferences.length; ++i)
        {
            conferenceNames.push(conferenceAction.createObject(conferenceSelector, {idx: i}));
        }
        conferenceSelector.children = conferenceNames;
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer

        title: "Conference Companion"
        actions: [
            Kirigami.Action {
                id: conferenceSelector

                text: i18n("Conferences")
                iconName: "group"
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
                id: dayActions

                text: i18n("Days")
                iconName: "view-calendar-day"
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

        Component.onCompleted: loadDynamicActions()
    }

//     contextDrawer: Kirigami.ContextDrawer {
//         id: contextDrawer
//     }

    pageStack {
        initialPage: [conferencesView]
        separatorVisible: false
    }

    KDEFOSDEM.LocalCalendar {
        id: onlineCalendar

        calendarInfo: {"name": ConferenceData.conferences[root.activeConference].name, "url": ConferenceData.conferences[root.activeConference].url}

        onCalendarInfoChanged: {
            root.refreshNeeded();
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
            root.loadDynamicActions();
        }
    }

    KDEFOSDEM.LocalCalendar {
        id: favoritesCalendar

        calendarInfo: {"name": "favorites"}

        onCalendarInfoChanged: {
            root.refreshNeeded();
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

            onEventsUpdated: root.refreshNeeded()

            Connections {
                target: root

                onRefreshNeeded: reload()
            }
        }
    }

    Component {
        id: eventsListView

        EventsListView {
            roCalendar: onlineCalendar
            rwCalendar: favoritesCalendar
            viewMode: "events"

            onEventsUpdated: root.refreshNeeded()

            Connections {
                target: root

                onRefreshNeeded: reload()
            }
        }
    }

    Component {
        id: favoritesView

        EventsCardListView {
            roCalendar: favoritesCalendar
            rwCalendar: favoritesCalendar
            viewMode: "favorites"

            onEventsUpdated: root.refreshNeeded()

            Connections {
                target: root

                onRefreshNeeded: reload()
            }
        }
    }

    Component {
        id: conferencesView

        Conferences {
            conferencesList: ConferenceData.conferences

            onSelected: {
                pageStack.clear();
                root.activeConference = conferenceId;
                onlineCalendar.calendarInfo = {name: ConferenceData.conferences[conferenceId].name, url: ConferenceData.conferences[conferenceId].url};
                pageStack.push(eventsCardView, {title: i18n("Schedule"), eventStartDt: ""});
            }
        }
    }

    Component {
        id: mapView

        MapView {
            imageUrl: activeConference >= 0 && ConferenceData.conferences[activeConference].map.hasOwnProperty("image") ?ConferenceData.conferences[activeConference].map.image : ""
            latitude: activeConference >= 0 && ConferenceData.conferences[activeConference].map.hasOwnProperty("latitude") ? ConferenceData.conferences[activeConference].map.latitude : "";
            longitude: activeConference >= 0 && ConferenceData.conferences[activeConference].map.hasOwnProperty("longitude") ?ConferenceData.conferences[activeConference].map.longitude : ""
            geoUrl: activeConference >= 0 && ConferenceData.conferences[activeConference].map.hasOwnProperty("url") ? ConferenceData.conferences[activeConference].map.url : ""
        }
    }

    Component {
        id: categoryAction

        Kirigami.Action {
            text: ""
            onTriggered: {
                pageStack.clear();
                pageStack.push(eventsCardView, {title: text, eventStartDt: "", category: text, showCategories: false});
            }
        }
    }

    Component {
        id: dayAction

        Kirigami.Action {
            property date conferenceDay

            text: conferenceDay.toLocaleDateString(Qt.locale(), "dddd")
            onTriggered: {
                pageStack.clear();
                pageStack.push(eventsListView, {title: conferenceDay.toLocaleDateString(Qt.locale(), "dddd"), eventStartDt: conferenceDay});
            }
        }
    }

    Component {
        id: conferenceAction

        Kirigami.Action {
            property string idx

            text: ConferenceData.conferences[idx].name
            iconName: idx == root.activeConference ? "object-select-symbolic" : ""
            onTriggered: {
                pageStack.clear();
                root.activeConference = idx;
                onlineCalendar.calendarInfo = {name: ConferenceData.conferences[idx].name, url: ConferenceData.conferences[idx].url};
                pageStack.push(eventsCardView, {title: i18n("Schedule"), eventStartDt: ""});
            }
        }
    }
}
