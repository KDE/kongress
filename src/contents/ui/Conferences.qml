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
import org.kde.kirigami 2.4 as Kirigami
import org.kde.phone.kdefosdem 0.1 as KDEFOSDEM

Kirigami.Page {
    id: root

    title: i18n("Conferences")

    property var conferencesList

    signal selected(int conferenceId)

    Kirigami.CardsGridView {
        id: view
        anchors.fill: parent

        model: conferencesList


        delegate: Kirigami.Card {
            id: card
            banner {
                title: modelData.name
            }
            contentItem: Controls.Label {
                wrapMode: Text.WordWrap
                text: modelData.details
            }
            actions: [
                Kirigami.Action {
                    text: i18n("Select")
                    iconName: "object-select-symbolic"
                    onTriggered: root.selected(model.index)
                }
            ]
        }
    }
}
