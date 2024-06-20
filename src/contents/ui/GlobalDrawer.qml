/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls 2 as Controls
import QtQml.Models
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kongress as Kongress

Kirigami.OverlayDrawer {
    id: root

    property var activeConference
    property var pageStack

    /**
     * Starting from the last page in the stack, remove every page of the stack
     */
    function popAll(): void {
        while (pageStack.depth > 0) {
            pageStack.pop();
        }
        if (modal) {
            root.close();
        }
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    width: Kirigami.Units.gridUnit * 16

    contentItem: ColumnLayout {
        spacing: 0

        Controls.ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: applicationWindow().pageStack.globalToolBar.preferredHeight

            topPadding: Kirigami.Units.smallSpacing
            bottomPadding: Kirigami.Units.largeSpacing / 2
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.smallSpacing

            contentItem: Kirigami.Heading {
                text: activeConference && activeConference.name.length > 0 ? activeConference.name : i18n("Conference")
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        Controls.ScrollView {
            id: scrollView

            Layout.fillWidth: true
            Layout.fillHeight: true

            contentWidth: availableWidth

            ColumnLayout {
                width: scrollView.availableWidth

                spacing: 0
                Delegates.RoundedItemDelegate {
                    text: activeConference ? i18n("Change conference") : i18n("Select conference")
                    icon.name: activeConference ? 'exchange-positions' : 'edit-select'

                    Layout.topMargin: Kirigami.Units.smallSpacing / 2

                    onClicked: {
                        popAll();
                        Kongress.ConferenceController.clearActiveConference();
                    }

                    Layout.fillWidth: true
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                    Layout.margins: Kirigami.Units.smallSpacing
                }

                Delegates.RoundedItemDelegate {
                    text: i18n("Check for updates")
                    visible: activeConference
                    icon.name: "update-none"

                    onClicked: {
                        onlineCalendar.loadOnlineCalendar();
                    }

                    Layout.fillWidth: true
                    Controls.ButtonGroup.group: pageButtonGroup
                }

                Delegates.RoundedItemDelegate {
                    id: mapAction
                    text: i18n("Map")
                    icon.name: "find-location"
                    visible: activeConference && (activeConference.venueOsmUrl !== "" || activeConference.hasVenueCoordinate)

                    onClicked: {
                        if (root.activeConference.hasVenueIndoorMap) {
                            popAll();
                            pageStack.push(indoorMapView);
                        } else if (root.activeConference.hasVenueCoordinate) {
                            popAll();
                            pageStack.push(mapView);
                        } else {
                            Qt.openUrlExternally(root.activeConference.venueOsmUrl);
                        }
                    }

                    Layout.fillWidth: true
                    Controls.ButtonGroup.group: pageButtonGroup
                }

                Delegates.RoundedItemDelegate {
                    id: favoriteAction
                    text: i18n("Favorites")
                    icon.name: "favorite"
                    visible: activeConference

                    onClicked: {
                        popAll();
                        pageStack.push(favoritesView, {title: i18n("Favorites"), eventStartDt: ""});
                    }

                    Layout.fillWidth: true
                    Controls.ButtonGroup.group: pageButtonGroup
                }

                Delegates.RoundedItemDelegate {
                    text: i18n("Full Schedule")
                    icon.name: "view-calendar-agenda"
                    visible: activeConference

                    onClicked: {
                        popAll();
                        pageStack.push(scheduleView, {title: i18n("Schedule"), eventStartDt: ""});
                    }

                    Layout.fillWidth: true
                    Controls.ButtonGroup.group: pageButtonGroup
                }

                Kirigami.ListSectionHeader {
                    id: dailyScheduleToggle

                    text: i18n("Daily Schedule")

                    checkable: true
                    checked: true
                    visible: activeConference && activeConference.days.length > 0

                    Layout.fillWidth: true

                    Kirigami.Icon {
                        source: dailyScheduleToggle.checked ? 'expand-symbolic' : 'collapse-symbolic'
                        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                    }
                }

                Repeater {
                    model: dailyScheduleToggle.checked && activeConference && activeConference.days.length > 0 ? activeConference.days: []

                    Delegates.RoundedItemDelegate {
                        required property var modelData

                        property date conferenceDay: new Date(modelData)

                        text: conferenceDay.toLocaleDateString(Qt.locale(), "dddd")
                        icon.name: "view-calendar-day-symbolic"

                        onClicked: {
                            popAll();
                            pageStack.push(scheduleView, {title: conferenceDay.toLocaleDateString(Qt.locale(), "dddd"), eventStartDt: conferenceDay });
                        }

                        Layout.fillWidth: true
                        Controls.ButtonGroup.group: pageButtonGroup
                    }
                }

                Kirigami.ListSectionHeader {
                    id: categoryToggle

                    text: i18n("Categories")
                    icon.name: "category"
                    visible: activeConference && onlineCalendar && onlineCalendar.categories.length > 0

                    checkable: true
                    checked: true

                    Layout.fillWidth: true

                    Kirigami.Icon {
                        source: categoryToggle.checked ? 'expand-symbolic' : 'collapse-symbolic'
                        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                    }
                }

                Repeater {
                    model: categoryToggle.checked && onlineCalendar && onlineCalendar.categories.length > 0 ? onlineCalendar.categories : []

                    Delegates.RoundedItemDelegate {
                        text: modelData

                        onClicked: {
                            popAll();
                            pageStack.push(scheduleView, {title: text, eventStartDt: "", category: text, showCategories: false});
                        }

                        Layout.fillWidth: true
                        Controls.ButtonGroup.group: pageButtonGroup
                    }
                }

                Controls.ButtonGroup {
                    id: pageButtonGroup
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            visible: scrollView.contentHeight > scrollView.availableHeight
        }

        Delegates.RoundedItemDelegate {
            text: i18n("Settings")
            icon.name: "settings-configure"
            onClicked: {
                popAll();
                pageStack.push(Qt.createComponent('org.kde.kongress', 'SettingsView'));
            }

            Controls.ButtonGroup.group: pageButtonGroup

            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing / 2
        }

        Delegates.RoundedItemDelegate {
            icon.name: "help-about-symbolic"
            text: i18n("About")

            onClicked: {
                popAll();
                pageStack.push(Qt.createComponent('org.kde.kirigamiaddons.formcard', 'AboutPage'));
            }

            Controls.ButtonGroup.group: pageButtonGroup

            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.smallSpacing / 2
        }
    }
}
