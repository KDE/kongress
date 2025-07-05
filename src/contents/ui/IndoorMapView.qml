/*
 * SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kosmindoormap
import org.kde.kosmindoorrouting

Kirigami.Page {
    id: root
    required property variant conference

    /** The room to show/select initially. */
    property string roomName
    /** The time range for which to show the map. */
    property date beginTime
    property date endTime

    title: {
        if (map.mapLoader.isLoading || map.hasError || map.floorLevels.rowCount() === 0)
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

    RoomSearchDialog {
        id: roomSearchDialog
        roomModel: RoomModel {
            id: roomModel
            mapData: map.mapData
            beginTime: map.view.beginTime
            endTime: map.view.endTime

            onMapDataChanged: {
                const row = roomModel.findRoom(root.roomName);
                if (row >= 0) {
                    const idx = roomModel.index(row, 0);
                    map.view.centerOn(roomModel.data(idx, RoomModel.CoordinateRole), roomModel.data(idx, RoomModel.LevelRole), 21);
                }
            }
        }
        onRoomSelected: (room) => {
            map.view.centerOn(room.element.center, room.level, 21);
        }
    }

    AmenitySearchDialog {
        id: amenitySearchDialog
        amenityModel: AmenityModel {
            id: amenityModel
            mapData: map.mapData
        }
        onAmenitySelected: (amenity) => {
            map.view.centerOn(amenity.element.center, amenity.level, 21);
        }
    }

    RoutingController {
        id: routingController
        mapData: map.mapData
    }

    IndoorMapView {
        id: map
        anchors.fill: parent
        styleSheet: root.conference.indoorMapStyle
        overlaySources: [ routingController.routeOverlay ]

        elementInfoModel {
            allowOnlineContent: true
        }

        Controls.Menu {
            id: contextMenu
            property mapPointerEvent ev
            Controls.MenuItem {
                text: i18n("Navigate from here")
                icon.name: "go-next"
                visible: routingController.available && root.conference.indoorRoutingEnabled
                onTriggered: {
                    routingController.setStartPosition(contextMenu.ev.geoPosition.y, contextMenu.ev.geoPosition.x, map.view.floorLevel);
                    routingController.searchRoute();
                }
            }
            Controls.MenuItem {
                text: i18n("Navigate to here")
                icon.name: "map-symbolic"
                visible: routingController.available && root.conference.indoorRoutingEnabled
                onTriggered: {
                    routingController.setEndPosition(contextMenu.ev.geoPosition.y, contextMenu.ev.geoPosition.x, map.view.floorLevel);
                    routingController.searchRoute();
                }
            }
            Controls.MenuItem {
                id: contextMenuInfoAction
                enabled: !contextMenu.ev.element.isNull && (map.elementInfoModel.name !== "" || map.elementInfoModel.debug)
                text: i18n("Show information")
                icon.name: "documentinfo"
                onTriggered: map.elementInfoDialog.open()
            }
        }

        function showContextMenu(ev) {
            map.elementInfoModel.element = ev.element;
            contextMenuInfoAction.enabled = !ev.element.isNull && (map.elementInfoModel.name !== "" || map.elementInfoModel.debug);
            contextMenu.ev = ev;
            contextMenu.popup(map, ev.screenPosition);
        }
    }

    onConferenceChanged: {
        map.mapLoader.loadForBoundingBox(root.conference.indoorMapBoundingBox.top, root.conference.indoorMapBoundingBox.left,
                                         root.conference.indoorMapBoundingBox.bottom, root.conference.indoorMapBoundingBox.right);
        if (root.conference.indoorMapExtraContent !== "") {
            map.mapLoader.addChangeSet(root.conference.indoorMapExtraContent);
        }
    }

    Connections {
        target: map.mapLoader
        function onDone() {
            map.timeZone = root.conference.timeZoneId;
            if (isNaN(root.beginTime.getTime()) && isNaN(root.endTime.getTime())) {
                // TODO this is only approximately correct due to how JS deals with date-only values and timezones, but good enough for now
                map.view.beginTime = new Date(root.conference.days[0]);
                let end = new Date(root.conference.days[root.conference.days.length - 1]);
                end.setDate(end.getDate() + 1);
                map.view.endTime = end;
            } else {
                map.view.beginTime = root.beginTime;
                map.view.endTime = root.endTime;
            }
        }
    }
}
