/*
 * SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls 2.4 as Controls2


import QtQuick
import QtQuick.Layouts
import QtPositioning
import QtLocation as QtLocation
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: root
    required property variant conference

    title: i18nc("@title", "Map")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    // prevent swipe to the right changing pages, we want to pan the map instead
    Kirigami.ColumnView.preventStealing: true

    actions: [
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

    QtLocation.Map {
        id: map
        plugin: QtLocation.Plugin {
            name: "osm"
            QtLocation.PluginParameter {
                name: "osm.useragent"
                value: "org.kde.kongress/" + _about.version
            }
            QtLocation.PluginParameter {
                name: "osm.mapping.providersrepository.address"
                value: "https://autoconfig.kde.org/qtlocation/"
            }
        }
        onCopyrightLinkActivated: Qt.openUrlExternally(link)

        center: QtPositioning.coordinate(root.conference.venueLatitude, root.conference.venueLongitude)
        zoomLevel: 18

        anchors.fill: parent

        // standard interaction handling, unfortunately no longer built-in...
        property geoCoordinate startCentroid
        PinchHandler {
            id: pinch
            target: null
            onActiveChanged: if (active) {
                map.startCentroid = map.toCoordinate(pinch.centroid.position, false)
            }
            onScaleChanged: (delta) => {
                map.zoomLevel += Math.log2(delta)
                map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
            }
            xAxis.enabled: false
            yAxis.enabled: false
            minimumRotation: 0.0
            maximumRotation: 0.0
        }
        WheelHandler {
            id: wheel
            rotationScale: 1/120
            orientation: Qt.Vertical
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: (event) => {
                map.startCentroid = map.toCoordinate(wheel.point.position, false)
                map.zoomLevel += event.angleDelta.y * rotationScale
                map.alignCoordinateToPoint(map.startCentroid, wheel.point.position)
            }
        }
        DragHandler {
            id: drag
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }
        Shortcut {
            enabled: map.zoomLevel < map.maximumZoomLevel
            sequence: StandardKey.ZoomIn
            onActivated: map.zoomLevel = Math.round(map.zoomLevel + 1)
        }
        Shortcut {
            enabled: map.zoomLevel > map.minimumZoomLevel
            sequence: StandardKey.ZoomOut
            onActivated: map.zoomLevel = Math.round(map.zoomLevel - 1)
        }

        QtLocation.MapQuickItem {
            coordinate: QtPositioning.coordinate(root.conference.venueLatitude, root.conference.venueLongitude)
            anchorPoint {
                x: icon.width / 2
                y: icon.height
            }
            sourceItem: Kirigami.Icon {
                id: icon
                source: "map-symbolic"
                width: height
                height: Kirigami.Units.iconSizes.large
                color: Kirigami.Theme.highlightColor
                isMask: true
            }
        }
    }
}
