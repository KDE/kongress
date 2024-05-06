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

    leftPadding: 0
    rightPadding: 0

    Kirigami.PlaceholderMessage {
        visible: !roCalendar.busyDownlading && (listView.count === 0)
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        anchors.centerIn: parent
        text: eventStartDt.toLocaleDateString() !== "" ? i18n("No talks found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No talks found")
    }

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

        visible: !roCalendar.busyDownlading && (count > 0)

        model: eventsModel
        section {
            property: Kongress.SettingsController.displayInLocalTimezone ? "shiftedEventDtLocal" : "shiftedEventDt"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                width: ListView.view.width

                label: section
            }
        }

        delegate: Controls.ItemDelegate {
            id: itemDelegate

            width: ListView.view.width

            action: Kirigami.Action {
                text: i18n("Details")
                icon.name: "documentinfo"

                onTriggered: {
                    pageStack.pop(root);
                    pageStack.push(eventInfo, {event: model});
                }
            }

            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing * 2
                width: parent.width

                Controls.Label {
                    width: Kirigami.Units.gridUnit * 20
                    text: Kongress.SettingsController.displayInLocalTimezone ? model.shiftedStartEndTimeLocal : model.shiftedStartEndTime
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                }

                Controls.Label {
                    visible: model.summary !== ""
                    elide: Text.ElideRight
                    text: model.summary
                    Layout.fillWidth: true
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 2
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
