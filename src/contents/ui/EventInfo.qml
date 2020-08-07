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

Kirigami.ScrollablePage {
    id: root

    property var event
    property string viewMode
    property var rwCalendar

    title: root.event ? event.summary : ""

    actions.left:
        Kirigami.Action {
            text: i18n("Web Page")
            icon.name: "internet-services"

            onTriggered: {
                if(root.event && (root.event.url)) {
                    Qt.openUrlExternally(event.url);
                }
            }
        }

    actions.main:
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
                    var vevent = { "uid" : event.uid, "startDate": event.scheduleStartDt, "summary": event.summary, "description": event.description, "allDay": event.allDay, "location": event.location, "endDate": event.scheduleEndDt, "categories": event.eventCategories, "url": event.url /*"alarms": incidenceAlarmsModel.alarms()*/};

                    var addEditResult = _eventController.addEdit(root.rwCalendar, vevent);
                    showPassiveNotification(addEditResult["message"]);

                }
            }
        }

    Kirigami.Card {
        id: cardDelegate

        visible: root.event
        banner.title: root.event ? event.summary : ""
        banner.titleLevel: 3

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

                    wrapMode: Text.WordWrap
                    text: viewMode == "favorites" ? event.startEndDt : event.shiftedStartEndDt
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
