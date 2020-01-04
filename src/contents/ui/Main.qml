/*
 *   Copyright 2019 Dimitris Kardarakos <dimkard@posteo.net>
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

Kirigami.ApplicationWindow {
    id: root

    /**
     * To be emitted when data displayed should be refreshed
     */
    signal refreshNeeded;

    property var categories: ["Ada","Backup and Recovery","BSD","Certification","Coding for Language Communities","Collaborative Information and Content Management Applications","Community and Ethics","Community devroom","Containers","Containers and Security","Continuous Integration and Continuous Deployment","Databases","Debugging Tools","Decentralized Internet and Privacy","Dependency Management","Distributions","DNS","Embedded, Mobile and Automotive","Erlang, Elixir and Friends","Freedom","Free Java","Free Software Radio","Free Tools and Editors","Game Development","Geospatial","Go","Graphics","Graph Systems and Algorithms","Hardware-aided Trusted Computing","Hardware Enablement","History","HPC, Big Data, and Data Science","Infra Management Devroom","Internet of Things","JavaScript","Keynotes","Kotlin","Legal and Policy Issues","Lightning Talks","LLVM","Microkernels and Component-based OS","Minimalistic, Experimental and Emerging Languages","Miscellaneous","Monitoring and Observability","Mozilla","MySQL, MariaDB and Friends","Open Document Editors","Open Media","Open Research Tools and Technologies","Open Source Computer Aided Modeling and Design","Open Source Design","Open Source Firmware, BMC and Bootloader","PostgreSQL","Python","Real Time Communications","Retrocomputing","RISC-V","Rust","Security","Software Defined Networking","Software Defined Storage","Testing and Automation","Virtualization and IaaS","Web Performance","Workshops"]

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer

        title: "KDE FOSDEM Companion"
        actions: [
            Kirigami.Action {
                text: i18n("Full Schedule", onlineCalendar.name)
                iconName: "view-calendar-agenda"
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(eventsView, {title: i18n("Schedule"),eventStartDt: ""});
                }
            },
            Kirigami.Action {
                text: i18n("Saturday", onlineCalendar.name)
                iconName: "view-calendar-day"
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(eventsView, {title: i18n("Saturday"), eventStartDt: new Date('2020-02-01')});
                }
            },
            Kirigami.Action {
                text: i18n("Sunday")
                iconName: "view-calendar-day"
                onTriggered: {
                    pageStack.clear();
                    pageStack.push(eventsView, {title: i18n("Sunday"), eventStartDt: new Date('2020-02-02')});
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

        Component.onCompleted: {
            var childrenActions = [];
            for (var i=0; i < categories.length; ++i)
            {
                childrenActions.push(categoryAction.createObject(categoryActions, { text: categories[i] }));
            }
            categoryActions.children = childrenActions;
        }
    }

//     contextDrawer: Kirigami.ContextDrawer {
//         id: contextDrawer
//     }

    pageStack {
        initialPage: [eventsView]
        separatorVisible: false
    }

//     KDEFOSDEM.Config {
//         id: kdefosdemConfig
//     }

    KDEFOSDEM.LocalCalendar {
        id: onlineCalendar

        name: "fosdemonline"

        onNameChanged: {
            root.refreshNeeded();
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }
    }

    KDEFOSDEM.LocalCalendar {
        id: favoritesCalendar

        name: "favorites"

        onNameChanged: {
            root.refreshNeeded();
            if (root.pageStack.depth > 1) {
                root.pageStack.pop(null);
            }
        }
    }

    Component {
        id: eventsView

        EventsView {
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

        EventsView {
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
        id: mapView

        MapView {}
    }

    Component {
        id: categoryAction

        Kirigami.Action {
            text: ""
            onTriggered: {
                pageStack.clear();
                pageStack.push(eventsView, {title: text, eventStartDt: "", category: text});
            }
        }
    }
}
