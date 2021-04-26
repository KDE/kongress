/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

        visible: !conferenceModel.busyDownlading && view.count === 0
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: i18n("No conference found")

        helpfulAction: Kirigami.Action {
            text: i18n("Reload")
            onTriggered: Kongress.ConferenceController.loadConferences()
        }
    }

    Controls.BusyIndicator {
        anchors.centerIn: parent

        running: conferenceModel && conferenceModel.busyDownlading
        implicitWidth: Kirigami.Units.iconSizes.enormous
        implicitHeight: width

    }


    Kirigami.CardsListView {
        id: view

        enabled: conferenceModel && !conferenceModel.busyDownlading && count > 0
        model: conferenceModel

        section {
            property: "pastUpcoming"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                label: section
            }
        }

        delegate: Kirigami.AbstractCard {
            id: card

            showClickFeedback: true

            header: Kirigami.Heading {
                text: model && model.name
                wrapMode: Text.WordWrap
            }

            contentItem: Controls.Label {
                wrapMode: Text.WordWrap
                text: model && model.description
            }

            onClicked: selected(model.id)
        }
    }

    Kongress.ConferenceModel {
        id: conferenceModel

        controller: Kongress.ConferenceController
    }
}
