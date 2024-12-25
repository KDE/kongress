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

    required property string uid
    required property string summary
    required property string startEndDtLocal
    required property string startEndDt
    required property string shiftedStartEndTimeLocal
    required property string shiftedStartEndTime
    required property string shiftedStartEndDtLocal
    required property string shiftedStartEndDt
    required property string scheduleStartDt
    required property string scheduleEndDt
    required property string eventCategories
    required property string speaker
    required property string location
    required property string description
    required property string eventUrl
    required property bool allDay

    required property string viewMode
    required property var rwCalendar

    title: !Kirigami.Settings.isMobile ? root.summary : i18nc("@title", "Event Details")

    FormCard.FormHeader {
        title: root.summary
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            icon.name: "view-calendar-day"
            text: if (viewMode === "favorites") {
                if (Kongress.SettingsController.displayInLocalTimezone) {
                    return root.startEndDtLocal;
                }
                return root.startEndDt;
            } else if (Kongress.SettingsController.displayInLocalTimezone) {
                return root.shiftedStartEndDtLocal;
            } else {
                return root.shiftedStartEndDt;
            }
        }

        FormCard.FormDelegateSeparator {
            visible: root.location.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.location.length > 0 && !Kongress.ConferenceController.activeConference.hasVenueIndoorMap
            icon.name: "find-location"
            text: root.location
        }
        FormCard.FormButtonDelegate {
            visible: root.location.length > 0 && Kongress.ConferenceController.activeConference.hasVenueIndoorMap
            icon.name: "find-location"
            text: root.location
            onClicked: {
                pageStack.pop();
                pageStack.push(indoorMapView, {
                    roomName: event.location,
                    beginTime: event.scheduleStartDt,
                    endTime: event.scheduleEndDt
                });
            }
        }

        FormCard.FormDelegateSeparator {
            visible: root.speaker !== ""
        }
        FormCard.FormTextDelegate {
            visible: root.speaker !== ""
            text: root.speaker ?? ""
            icon.name: "user-symbolic"
        }

        FormCard.FormDelegateSeparator {
            visible: root.eventCategories.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.eventCategories.length > 0
            icon.name: "category"
            text: root.eventCategories
        }

        FormCard.FormDelegateSeparator {
            visible: root.description.length > 0
        }

        FormCard.FormTextDelegate {
            visible: root.description.length > 0
            text: root.description
            textItem.wrapMode: Text.WordWrap
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormButtonDelegate {
            text: i18n("Web Page")
            icon.name: "internet-services"
            enabled: root.eventUrl !== ""

            onClicked: if(root.eventUrl) {
                Qt.openUrlExternally(eventUrl);
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: eventController.isFavorite ? i18nc("@action:button", "Delete from Favorites") : i18nc("@action:button", "Add to Favorites")
            icon.name: eventController.isFavorite ? "favorite-favorited-symbolic" : "favorite-symbolic"

            Kongress.EventController {
                id: eventController

                calendarController: Kongress.CalendarController
                eventUid: root.uid
                calendar: root.rwCalendar
            }

            enabled: root.uid.length > 0

            onClicked: {
                if (eventController.isFavorite) {
                    const vevent = { uid: root.uid };
                    const ok = eventController.remove(vevent);
                    if (ok && root.viewMode === "favorites") {
                        pageStack.pop();
                    }

                    showPassiveNotification(ok ? i18nc("@info:status", "Talk removed from favorites.") : i18nc("@info:status", "Unable to remove talk from favorites."));
                    return;
                }

                const vevent = {
                    uid : root.uid,
                    startDate: root.scheduleStartDt,
                    summary: root.summary,
                    description: root.description,
                    allDay: root.allDay,
                    location: root.location,
                    endDate: root.scheduleEndDt,
                    categories: root.eventCategories,
                    url: eventUrl,
                    // alarms: incidenceAlarmsModel.alarms()
                };

                const addEditResult = eventController.addEdit(vevent);
                showPassiveNotification(addEditResult["message"]);
            }
        }
    }
}
