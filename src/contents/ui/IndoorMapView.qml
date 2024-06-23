/*
 * SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components
import org.kde.kosmindoormap

Kirigami.Page {
    id: root
    required property variant conference

    title: {
        if (map.mapLoader.isLoading || map.hasError || map.floorLevels.rowCount() == 0)
            return i18nc("@title", "Map");
        if (map.floorLevels.hasName(map.view.floorLevel))
            return map.floorLevels.name(map.view.floorLevel);
        return i18n("Floor %1", map.floorLevels.name(map.view.floorLevel));
    }

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    // prevent swipe to the right changing pages, we want to pan the map instead
    Kirigami.ColumnView.preventStealing: true

    actions: [
        Kirigami.Action {
            text: i18n("Find Room")
            icon.name: "search"
            visible: !roomModel.isEmpty
            onTriggered: roomSearchDialog.open()
        },
        Kirigami.Action {
            text: i18n("Find Amenity")
            icon.name: "search"
            visible: !amenityModel.isEmpty
            onTriggered: amenitySearchDialog.open()
        },
        Kirigami.Action {
            text: i18n("Open Map")
            icon.name: "map-globe"
            onTriggered: {
                if (root.conference.venueOsmUrl !== "") {
                    Qt.openUrlExternally(root.conference.venueOsmUrl);
                } else {
                    Qt.openUrlExternally("geo:" + root.conference.venueLatitude + "," + root.conference.venueLongitude + "?z=19");
                }
            }
        }
    ]

    FloorLevelSelector {
        id: floorLevelSelector
        model: FloorLevelChangeModel {
            id: floorLevelChangeModel
            currentFloorLevel: map.view.floorLevel
            floorLevelModel: map.floorLevels
        }
        onFloorLevelSelected: (level) => { map.view.floorLevel = level; }
    }

    OSMElementInformationDialog {
        id: infoDialog
        model: OSMElementInformationModel {
            id: infoModel
            allowOnlineContent: true
        }
        regionCode: map.mapData.regionCode
        timeZone: map.mapData.timeZone
    }

    RoomSearchDialog {
        id: roomSearchDialog
        roomModel: RoomModel {
            id: roomModel
            mapData: map.mapData
        }
        onRoomSelected: (room) => {
            map.view.floorLevel = room.level;
            map.view.setZoomLevel(21, Qt.point(map.width / 2.0, map.height/ 2.0));
            map.view.centerOnGeoCoordinate(room.element.center);
        }
    }

    AmenitySearchDialog {
        id: amenitySearchDialog
        amenityModel: AmenityModel {
            id: amenityModel
            mapData: map.mapData
        }
        onAmenitySelected: (amenity) => {
            map.view.floorLevel = amenity.level;
            map.view.setZoomLevel(21, Qt.point(map.width / 2.0, map.height/ 2.0));
            map.view.centerOnGeoCoordinate(amenity.element.center);
        }
    }

    IndoorMap {
        id: map
        anchors.fill: parent
        styleSheet: root.conference.indoorMapStyle

        IndoorMapScale {
            map: map
            anchors.left: map.left
            anchors.top: map.top
            width: 0.3 * map.width
        }

        IndoorMapAttributionLabel {
            anchors.right: map.right
            anchors.bottom: map.bottom
        }

       onElementPicked: (element) => {
            floorLevelChangeModel.element = element;
            if (floorLevelChangeModel.hasSingleLevelChange) {
                showPassiveNotification(i18n("Switched to floor %1", floorLevelChangeModel.destinationLevelName), "short");
                map.view.floorLevel = floorLevelChangeModel.destinationLevel;
                return;
            } else if (floorLevelChangeModel.hasMultipleLevelChanges) {
                floorLevelSelector.open();
                return;
            }

            infoModel.element = element;
            if (infoModel.name != "" || infoModel.debug) {
                infoDialog.open();
            }
        }
        onElementLongPressed: (element) => {
            // this provides info model access for elements with other interactions
            infoModel.element = element;
            if (infoModel.name != "" || infoModel.debug) {
                elementDetailsSheet.open();
            }
        }
    }

    onConferenceChanged: {
        map.mapLoader.loadForBoundingBox(root.conference.indoorMapBoundingBox.top, root.conference.indoorMapBoundingBox.left,
                                         root.conference.indoorMapBoundingBox.bottom, root.conference.indoorMapBoundingBox.right)
    }

    Connections {
        target: map.mapLoader
        function onDone() {
            map.timeZone = root.conference.timeZoneId;
            // TODO this is only approximately correct due to how JS deals with date-only values and timezones, but good enough for now
            map.view.beginTime = new Date(root.conference.days[0]);
            let end = new Date(root.conference.days[root.conference.days.length - 1]);
            end.setDate(end.getDate() + 1);
            map.view.endTime = end;
        }
    }

    Components.DoubleFloatingButton {
        anchors {
            right: parent.right
            rightMargin: Kirigami.Units.largeSpacing
            bottom: parent.bottom
            bottomMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing // to not hide the copyright information
        }

        leadingAction: Kirigami.Action {
            icon.name: "go-down-symbolic"
            text: i18nc("@action:intoolbar Go down one floor", "Floor down")
            enabled: map.floorLevels.hasFloorLevelBelow(map.view.floorLevel)
            onTriggered: map.view.floorLevel = map.floorLevels.floorLevelBelow(map.view.floorLevel)
            visible: map.floorLevels.hasFloorLevels
            tooltip: text
        }

        trailingAction: Kirigami.Action {
            icon.name: "go-up-symbolic"
            text: i18nc("@action:intoolbar Go up one floor", "Floor up")
            enabled: map.floorLevels.hasFloorLevelAbove(map.view.floorLevel)
            onTriggered: map.view.floorLevel = map.floorLevels.floorLevelAbove(map.view.floorLevel)
            visible: map.floorLevels.hasFloorLevels
            tooltip: text
        }
    }
}
