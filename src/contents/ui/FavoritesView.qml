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

    property date eventStartDt
    property var roCalendar
    property var rwCalendar
    property string category
    property bool showCategories: true

    leftPadding: 0
    rightPadding: 0

    actions: [
        Kirigami.Action {
            text: i18n("Export")
            icon.name: "document-export"

            onTriggered: {
                var exportResult = Kongress.CalendarController.exportData(Kongress.ConferenceController.activeConference.id, root.rwCalendar);
                footerMessage.text = exportResult.reason;

                if (!(exportResult.success)) {
                    footerMessage.type = Kirigami.MessageType.Warning;
                }
                else {
                    footerMessage.targetFolder = exportResult.targetFolder;
                    footerMessage.type = Kirigami.MessageType.Positive;
                }
                footerMessage.visible = true;
            }
        }
    ]

    footer: Kirigami.InlineMessage {
            id: footerMessage

            property string targetFolder

            position: Kirigami.InlineMessage.Footer
            showCloseButton: true
            visible: false
            actions: [
                Kirigami.Action {
                    id: openFolderAction

                    text: i18n("Open folder")
                    icon.name: "folder-open"

                    onTriggered: {
                        Qt.openUrlExternally(footerMessage.targetFolder);
                        footerMessage.visible = false;
                    }
                }
            ]
    }

    Component {
        id: eventInfo

        EventInfo {
            rwCalendar: root.rwCalendar
            viewMode: "favorites"
        }
    }

    Kirigami.CardsListView {
        id: cardsListview

        anchors.fill: parent
        clip: true
        model: eventsModel

        Kirigami.PlaceholderMessage {
            visible: cardsListview.count === 0
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            text: eventStartDt.toLocaleDateString() !== "" ? i18n("No favorite talks found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No favorite talks found")
            icon.name: "favorite"

            anchors.centerIn: parent
        }

        delegate: Kirigami.AbstractCard {
            id: cardDelegate

            showClickFeedback: true

            header: Kirigami.Heading {
                text: model && model.summary
                wrapMode: Text.WordWrap
            }

            onClicked: pageStack.push(eventInfo, {
                uid: model.uid,
                summary: model.summary,
                startEndDtLocal: model.startEndDtLocal,
                startEndDt: model.startEndDt,
                shiftedStartEndTimeLocal: model.shiftedStartEndTimeLocal,
                shiftedStartEndTime: model.shiftedStartEndTime,
                shiftedStartEndDtLocal: model.shiftedStartEndDtLocal,
                shiftedStartEndDt: model.shiftedStartEndDt,
                location: model.location,
                scheduleStartDt: model.scheduleStartDt,
                scheduleEndDt: model.scheduleEndDt,
                speakers: model.speakers,
                eventCategories: model.eventCategories,
                description: model.description,
                eventUrl: model.url,
                allDay: model.allDay,
            })

            contentItem: Column {
                spacing: Kirigami.Units.largeSpacing
                topPadding: 0
                bottomPadding: 0

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
                        text: Kongress.SettingsController.displayInLocalTimezone ? (model && model.startEndDtLocal) : (model && model.startEndDt)
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    visible: model && model.location !== ""
                    width: cardDelegate.availableWidth
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "find-location"
                        width: Kirigami.Units.iconSizes.small
                        height: width
                    }

                    Controls.Label {
                        wrapMode: Text.WordWrap
                        text: model && model.location
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    visible: root.showCategories && model && (model.eventCategories !== "")
                    width: cardDelegate.availableWidth
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "category"
                        width: Kirigami.Units.iconSizes.small
                        height: width
                    }

                    Controls.Label {
                        wrapMode: Text.WordWrap
                        text: model && model.eventCategories
                        Layout.fillWidth: true
                    }
                }

                Kirigami.InlineMessage {
                    visible: model && model.overlapping > 0
                    type: Kirigami.MessageType.Information
                    text: model && i18np("Overlaps with another talk", "Overlaps with %1 other talks", model.overlapping)
                    width: cardDelegate.availableWidth
                }

            }
        }
    }

    Kongress.EventModel {
        id: eventsModel

        filterdt: root.eventStartDt
        calendar: root.roCalendar
        eventCategory: root.category
    }

}
