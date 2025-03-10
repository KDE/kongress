/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kongress as Kongress

Kirigami.ScrollablePage {
    id: root

    property date eventStartDt
    property var roCalendar
    property var rwCalendar
    property string category

    leftPadding: 0
    rightPadding: 0

    Component {
        id: eventInfo

        EventInfo {
            rwCalendar: root.rwCalendar
            viewMode: "events"
        }
    }

    Controls.BusyIndicator {
        anchors.centerIn: parent

        running: roCalendar.busyDownlading
        implicitWidth: Kirigami.Units.iconSizes.enormous
        implicitHeight: width

    }

    ListView {
        id: listView

        Kirigami.PlaceholderMessage {
            visible: !roCalendar.busyDownlading && (listView.count === 0)
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            anchors.centerIn: parent
            text: eventStartDt.toLocaleDateString() !== "" ? i18n("No talks found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No talks found")
        }

        model: eventsModel
        section {
            property: Kongress.SettingsController.displayInLocalTimezone ? "shiftedEventDtLocal" : "shiftedEventDt"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                width: ListView.view.width

                label: section
            }
        }

        delegate: Delegates.RoundedItemDelegate {
            id: itemDelegate

            width: ListView.view.width

            onClicked: {
                pageStack.pop(root);
                pageStack.push(eventInfo, {
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
                });
            }

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                ColumnLayout {
                    spacing: 0

                    Layout.alignment: Qt.AlignTop

                    Controls.Label {
                        text: {
                            if (root.width > Kirigami.Units.gridUnit * 25) {
                                return Kongress.SettingsController.displayInLocalTimezone ? model.shiftedStartEndTimeLocal : model.shiftedStartEndTime;
                            }

                            return Kongress.SettingsController.displayInLocalTimezone ? model.shiftedStartEndTimeLocalMobile : model.shiftedStartEndTimeMobile;
                        }
                    }

                    Controls.Label {
                        id: location
                        text: model.location
                        font: Kirigami.Theme.smallFont
                        opacity: 0.7
                    }
                }

                Controls.Label {
                    visible: model.summary !== ""
                    text: model.summary
                    wrapMode: Text.WordWrap

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                }

                Kirigami.Icon {
                    visible: model.favorite
                    source: "starred-symbolic"
                    opacity: 0.7

                    Layout.alignment: Qt.AlignTop
                    Layout.maximumHeight: Kirigami.Units.iconSizes.smallMedium
                    Layout.maximumWidth: Kirigami.Units.iconSizes.smallMedium
                }
            }
        }
    }

    Kongress.EventModel {
        id: eventsModel

        filterdt: root.eventStartDt
        calendar: root.roCalendar
        eventCategory: root.category
        favoritesCalendar: root.rwCalendar
    }
}
