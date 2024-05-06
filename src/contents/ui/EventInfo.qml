/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kongress as Kongress

Kirigami.ScrollablePage {
    id: root

    property var event
    property string viewMode
    property var rwCalendar

    title: root.event ? event.summary : ""

    actions: [
        Kirigami.Action {
            text: i18n("Web Page")
            icon.name: "internet-services"
            enabled: root.event.url !== ""

            onTriggered: {
                if(root.event && (root.event.url)) {
                    Qt.openUrlExternally(event.url);
                }
            }
        },
        Kirigami.Action {
            text: viewMode === "favorites" ? i18n("Delete") : i18n("Favorite")
            icon.name: viewMode === "favorites" ? "delete" : "favorite"

            onTriggered: {
                if(root.event && root.viewMode === "favorites") {
                    var vevent = { uid: root.event.uid } ;
                    Kongress.EventController.remove(root.rwCalendar, vevent);
                    pageStack.pop();
                }
                else if(root.event) {
                    var vevent = { "uid" : event.uid, "startDate": event.scheduleStartDt, "summary": event.summary, "description": event.description, "allDay": event.allDay, "location": event.location, "endDate": event.scheduleEndDt, "categories": event.eventCategories, "url": event.url /*"alarms": incidenceAlarmsModel.alarms()*/};

                    var addEditResult = Kongress.EventController.addEdit(root.rwCalendar, vevent);
                    showPassiveNotification(addEditResult["message"]);

                }
            }
        }
    ]

    Kirigami.AbstractCard {
        id: cardDelegate

        visible: root.event
        header: Kirigami.Heading {
            text: root.event ? event.summary : ""
            wrapMode: Text.WordWrap
        }

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

                Controls.Label {

                    wrapMode: Text.WordWrap
                    text: {
                        if (viewMode === "favorites") {
                            if (Kongress.SettingsController.displayInLocalTimezone) {
                                return event.startEndDtLocal;
                            }
                            else {
                                return event.startEndDt;
                            }
                        }
                        else {
                            if (Kongress.SettingsController.displayInLocalTimezone) {
                                return event.shiftedStartEndDtLocal;
                            }
                            else {
                                return event.shiftedStartEndDt;
                            }
                        }
                    }
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                visible: root.event && (event.location !== "")
                width: cardDelegate.availableWidth
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: "find-location"
                    width: Kirigami.Units.iconSizes.small
                    height: width
                }

                Controls.Label {
                    wrapMode: Text.WordWrap
                    text:  root.event ? event.location : ""
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                visible: root.event && (event.eventCategories !== "")
                width: cardDelegate.availableWidth
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: "category"
                    width: Kirigami.Units.iconSizes.small
                    height: width
                }

                Controls.Label {
                    wrapMode: Text.WordWrap
                    text: root.event ? event.eventCategories : ""
                    Layout.fillWidth: true
                }
            }

            Controls.Label {
                visible: root.event && event.description !== ""
                width: cardDelegate.availableWidth
                wrapMode: Text.WordWrap
                text: root.event ? event.description : ""
            }
        }
    }
}
