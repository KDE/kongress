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

import QtQuick 2.7
import QtQuick.Controls 2.4 as Controls2
import QtQuick.Layouts 1.11
import org.kde.kirigami 2.4 as Kirigami

Kirigami.Page {
    id: root

    property var event
    property string viewMode
    property var rwCalendar

    title: root.event ? event.summary : ""

    Kirigami.Card {
        id: cardDelegate

        anchors {
            left: parent.left
            right: parent.right
        }

        visible: root.event
        banner.title: root.event ? event.summary : ""
        banner.titleLevel: 3

        actions: [
            Kirigami.Action {
                text: i18n("Web Page")
                icon.name: "internet-services"

                onTriggered: {
                    if(root.event && (root.event.url)) {
                        Qt.openUrlExternally(event.url);
                    }
                }
            },

            Kirigami.Action {
                text: viewMode == "favorites" ? i18n("Delete") : i18n("Favorite")
                icon.name: viewMode == "favorites" ? "delete" : "favorite"

                onTriggered: {
                    if(root.event && root.viewMode == "favorites") {
                        var vevent = { uid: root.event.uid } ;
                        _eventController.remove(root.rwCalendar, vevent);
                        pageStack.pop();
                    }
                    else if(root.event) {
                        console.log("Adding event to favorites");
                        var vevent = { "uid" : event.uid, "startDate": event.dtstart, "summary": event.summary, "description": event.description, "allDay": event.allDay, "location": event.location, "endDate": event.dtend, "categories": event.eventCategories, "url": event.url/*"alarms": incidenceAlarmsModel.alarms()*/};

                        var addEditResult = _eventController.addEdit(root.rwCalendar, vevent);

                        if(addEditResult == 201)
                        {
                            showPassiveNotification(i18n("Added to favorites"));
                        }
                        else if (addEditResult == 304)
                        {
                            showPassiveNotification(i18n("Already in favorites"));
                        }
                    }
                }
            }
        ]

        contentItem: Column {
            spacing: Kirigami.Units.largeSpacing
            topPadding: 0
            bottomPadding: 0
            visible: root.event

            RowLayout {
                width: cardDelegate.availableWidth
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: "view-calendar-day"
                    width: Kirigami.Units.iconSizes.small
                    height: width
                }

                Controls2.Label {
                    property bool sameEndStart: root.event && event.dtstart && !isNaN(event.dtstart) && event.dtend && !isNaN(event.dtend) && event.dtstart.toLocaleString(Qt.locale(), "dd.MM.yyyy") == event.dtend.toLocaleString(Qt.locale(), "dd.MM.yyyy")
                    property string timeFormat: root.event && event.allday ? "" : "hh:mm"
                    property string dateFormat: root.event && event.allday ? "ddd d MMM yyyy" : "ddd d MMM yyyy hh:mm"
                    property string separator: root.event && event.allday ? "" : " - "

                    wrapMode: Text.WordWrap
                    text: ((root.event && event.dtstart && !isNaN(event.dtstart)) ? event.dtstart.toLocaleString(Qt.locale(), dateFormat ) : "") +
                        (root.event && event.dtend && !isNaN(event.dtend) ? separator +
                            event.dtend.toLocaleString(Qt.locale(), sameEndStart ? timeFormat : dateFormat ) : "")
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                visible: root.event && (event.location != "")
                width: cardDelegate.availableWidth
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: "find-location"
                    width: Kirigami.Units.iconSizes.small
                    height: width
                }

                Controls2.Label {
                    wrapMode: Text.WordWrap
                    text:  root.event ? event.location : ""
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                visible: root.event && (event.eventCategories != "")
                width: cardDelegate.availableWidth
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: "view-categories"
                    width: Kirigami.Units.iconSizes.small
                    height: width
                }

                Controls2.Label {
                    wrapMode: Text.WordWrap
                    text: root.event ? event.eventCategories : ""
                    Layout.fillWidth: true
                }
            }

            Controls2.Label {
                visible: root.event && event.description != ""
                width: cardDelegate.availableWidth
                wrapMode: Text.WordWrap
                text: root.event ? event.description : ""
            }
        }
    }
}
