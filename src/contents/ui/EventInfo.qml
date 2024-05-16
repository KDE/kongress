/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kongress as Kongress

FormCard.FormCardPage {
    id: root

    property var event
    property string viewMode
    property var rwCalendar

    title: root.event && !Kirigami.Settings.isMobile ? event.summary : i18nc("@title", "Event Details")

    FormCard.FormHeader {
        visible: root.event
        title: root.event ? event.summary : ""
    }

    FormCard.FormCard {
        visible: root.event

        FormCard.FormTextDelegate {
            icon.name: "view-calendar-day"
            text: if (viewMode === "favorites") {
                if (Kongress.SettingsController.displayInLocalTimezone) {
                    return event.startEndDtLocal;
                } else {
                    return event.startEndDt;
                }
            } else if (Kongress.SettingsController.displayInLocalTimezone) {
                return event.shiftedStartEndDtLocal;
            } else {
                return event.shiftedStartEndDt;
            }
        }

        FormCard.FormDelegateSeparator {
            visible: root.event && event.location.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.event && event.location.length > 0
            icon.name: "find-location"

            text: root.event ? event.location : ""
        }

        FormCard.FormDelegateSeparator {
            visible: root.event && event.eventCategories.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.event && event.eventCategories.length > 0
            icon.name: "category"

            text: root.event ? event.eventCategories : ""
        }

        FormCard.FormDelegateSeparator {
            visible: root.event && event.description.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.event && event.description.length > 0
            text: root.event ? event.description : ""
            textItem.wrapMode: Text.WordWrap
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormButtonDelegate {
            text: i18n("Web Page")
            icon.name: "internet-services"
            enabled: root.event.url !== ""

            onClicked: {
                if(root.event && (root.event.url)) {
                    Qt.openUrlExternally(event.url);
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: viewMode === "favorites" ? i18n("Delete") : i18n("Favorite")
            icon.name: viewMode === "favorites" ? "delete" : "favorite"

            onClicked: {
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
    }
}
