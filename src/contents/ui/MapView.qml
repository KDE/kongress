/*
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4 as Controls2
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ScrollablePage {
    id: root

    property string imageUrl
    property string latitude
    property string longitude
    property string geoUrl

    title: i18n("Map")

    ColumnLayout {

        Image {
            Layout.fillWidth: true
            visible: imageUrl != ""
            source: root.imageUrl
            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        Controls2.Label {
            Layout.alignment : Qt.AlignHCenter
            visible: root.latitude != ""
            text: i18n("Latitude: %1", root.latitude)
        }

        Controls2.Label {
            Layout.alignment : Qt.AlignHCenter
            visible: root.longitude != ""
            text: i18n("Longitude: %1", root.longitude)
        }

        Controls2.Button {
            Layout.alignment : Qt.AlignHCenter
            visible: root.geoUrl != ""
            text: i18n("OpenStreetMap")

            onClicked: Qt.openUrlExternally(root.geoUrl)
        }
    }
}
