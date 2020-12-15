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
    property bool showCategories: true

    leftPadding: 0
    rightPadding: 0

    Kirigami.PlaceholderMessage {
        visible: cardsListview.count === 0
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: eventStartDt.toLocaleDateString() !== "" ? i18n("No favorite talks found for %1", eventStartDt.toLocaleDateString(Qt.locale(), Locale.ShortFormat)) : i18n("No favorite talks found")

        anchors.centerIn: parent
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

        model: eventsModel

        delegate: Kirigami.Card {
            id: cardDelegate

            banner.title: model && model.summary
            banner.titleLevel: 3

            actions: [
                Kirigami.Action {
                    text: i18n("Details")
                    icon.name: "documentinfo"

                    onTriggered: pageStack.push(eventInfo, {event: model})
                },

                Kirigami.Action {
                    text: i18n("Delete")
                    icon.name: "delete"

                    onTriggered: Kongress.EventController.remove(root.rwCalendar, {uid: model.uid})
                }
            ]

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

                    Controls2.Label {
                        wrapMode: Text.WordWrap
                        text: Kongress.SettingsController.displayInLocalTimezone ? model.startEndDtLocal : model.startEndDt
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

                    Controls2.Label {
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

                    Controls2.Label {
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
