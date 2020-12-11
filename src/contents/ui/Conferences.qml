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
import QtQuick.Controls 2.4 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.kongress 0.1 as Kongress

Kirigami.ScrollablePage {
    id: root

    title: i18n("Conferences")

    signal selected(var selectedConferenceId)

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent

        visible: !conferenceModel.busyDownlading && view.count == 0
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: i18n("No conference found")

        helpfulAction: Kirigami.Action {
            text: i18n("Reload")
            onTriggered: Kongress.ConferenceController.loadConferences()
        }
    }

    Controls.BusyIndicator {
        anchors.centerIn: parent

        running: conferenceModel.busyDownlading
        implicitWidth: Kirigami.Units.iconSizes.enormous
        implicitHeight: width

    }

    Kirigami.CardsListView {
        id: view

        enabled: !conferenceModel.busyDownlading && count > 0
        model: conferenceModel
        section {
            property: "pastUpcoming"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                label: section
            }
        }
        delegate: Kirigami.Card {
            id: card

            banner {
                title: model.name
            }

            contentItem: Controls.Label {
                wrapMode: Text.WordWrap
                text: model.description
            }

            actions: [
                Kirigami.Action {
                    text: i18n("Select")
                    iconName: "edit-select"

                    onTriggered: selected(model.id)
                }
            ]
        }
    }

    Kongress.ConferenceModel {
        id: conferenceModel

        controller: Kongress.ConferenceController
    }

}
