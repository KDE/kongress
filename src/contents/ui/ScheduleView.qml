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
        visible: !roCalendar.busyDownlading && (listView.count == 0)
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        anchors.centerIn: parent
        text: eventStartDt.toLocaleDateString() != "" ? i18n("No talks found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No talks found")
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

        enabled: !roCalendar.busyDownlading && count > 0

        model: eventsModel
        section {
            property: "eventDt"
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

                onTriggered: pageStack.push(eventInfo, {event: model})
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
                    visible: model.summary != ""
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
