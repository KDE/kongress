/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQml.Models
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kongress as Kongress

Kirigami.GlobalDrawer {
    id: drawer

    property var activeConference
    property var pageStack

    /**
     * Starting from the last page in the stack, remove every page of the stack
     */
    function popAll()
    {
        while (pageStack.depth > 0) {
            pageStack.pop();
        }
    }

    header: Kirigami.AbstractApplicationHeader {
        topPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.smallSpacing
        implicitHeight: Kirigami.Units.gridUnit * 2
        Kirigami.Heading {
            level: 1
            text: activeConference ? activeConference.name : i18n("Conference")
            Layout.fillWidth: true
        }
    }

    actions: [
        Kirigami.Action {
            text: i18n("Check for updates")
            visible: activeConference
            icon.name: "update-none"

            onTriggered: {
                onlineCalendar.loadOnlineCalendar();
            }
        },

        Kirigami.Action {
            id: dayActions

            text: i18n("Daily Schedule")
            icon.name: "view-calendar-day"
            expandible: true
        },

        Kirigami.Action {
            text: i18n("Full Schedule")
            icon.name: "view-calendar-agenda"
            visible: activeConference

            onTriggered: {
                popAll();
                pageStack.push(scheduleView, {title: i18n("Schedule"), eventStartDt: ""});
            }
        },

        Kirigami.Action {
            id: categoryActions

            text: i18n("Categories")
            icon.name: "category"
            visible: activeConference && children.length > 0
        },

        Kirigami.Action {
            text: i18n("Map")
            icon.name: "find-location"
            visible: activeConference && (activeConference.venueOsmUrl !== "" || activeConference.hasVenueCoordinate)

            onTriggered: {
                if (drawer.activeConference.hasVenueCoordinate) {
                    popAll();
                    pageStack.push(mapView);
                } else {
                    Qt.openUrlExternally(drawer.activeConference.venueOsmUrl);
                }
            }
        },

        Kirigami.Action {
            text: i18n("Favorites")
            icon.name: "favorite"
            visible: activeConference

            onTriggered: {
                popAll();
                pageStack.push(favoritesView, {title: i18n("Favorites"), eventStartDt: ""});
            }
        },

        Kirigami.Action {
            text: i18n("Configure")
            icon.name: "settings-configure"
            expandible: true

            Kirigami.Action {
                text: activeConference ? i18n("Change conference") : i18n("Select conference")
                icon.name: activeConference ? 'exchange-positions' : 'edit-select'

                onTriggered: {
                    Kongress.ConferenceController.clearActiveConference();
                }
            }

            Kirigami.Action {
                text: i18n("Settings")
                icon.name: "settings-configure"
                onTriggered: {
                    popAll();
                    pageStack.push(settingsView);
                }
            }
        },

        Kirigami.Action {
            id: aboutAction

            icon.name: "help-about-symbolic"
            text: i18n("About")

            onTriggered: {
                popAll();
                pageStack.push(aboutInfoPage);
            }
        }
    ]

    Component {
        id: settingsView

        SettingsView {}
    }

    Instantiator { 
        id: daysInstantiator

        model: activeConference && (activeConference.days !== null) ? activeConference.days: []

        delegate: Kirigami.Action {
            property date conferenceDay: new Date(modelData)

            text: conferenceDay.toLocaleDateString(Qt.locale(), "dddd")

            onTriggered: {
                popAll();
                pageStack.push(scheduleView, {title: conferenceDay.toLocaleDateString(Qt.locale(), "dddd"), eventStartDt: conferenceDay });
            }
        }

        onObjectAdded: (index, object) => {
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

    Instantiator {
        model: (onlineCalendar && onlineCalendar.categories) ? onlineCalendar.categories : []

        delegate: Kirigami.Action {
            text: modelData

            onTriggered: {
                popAll();
                pageStack.push(scheduleView, {title: text, eventStartDt: "", category: text, showCategories: false});
            }
        }

        onObjectAdded: {
            if(object.text !== "") {
                categoryActions.children.push(object)
            }
        }

        onObjectRemoved: {
            if(categoryActions.children) {
                categoryActions.children = [];
            }
        }
    }

    Component {
        id: aboutInfoPage

        FormCard.AboutPage {
            aboutData: _about
        }
    }
}
