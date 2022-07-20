/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.4 as Controls2
import QtQuick.Layouts 1.11
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kongress 0.1 as Kongress

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

    Controls2.BusyIndicator {
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
                label: section
            }
        }

        delegate: Kirigami.AbstractListItem {
            id: itemDelegate

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

                Controls2.Label {
                    property string timeFormat: model.allday ? "" : "hh:mm"

                    width: Kirigami.Units.gridUnit * 20
                    text: Kongress.SettingsController.displayInLocalTimezone ? model.shiftedStartEndTimeLocal : model.shiftedStartEndTime
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                }

                Controls2.Label {
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
