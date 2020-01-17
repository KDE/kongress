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
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4 as Controls2
import org.kde.kirigami 2.4 as Kirigami

Kirigami.Page {
    id: root

    property string imageUrl
    property string latitude
    property string longitude
    property string geoUrl

    title: i18n("Map")

    ColumnLayout {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

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
            text: i18n("OpenStreetMaps")

            onClicked: Qt.openUrlExternally(root.geoUrl)
        }
    }
}
