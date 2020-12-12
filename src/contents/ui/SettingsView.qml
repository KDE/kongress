/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.4 as Controls2
import org.kde.kirigami 2.4 as Kirigami
import org.kde.kongress 0.1 as Kongress

Kirigami.ScrollablePage {
    id: root

    title: i18n("Settings")

    Kirigami.FormLayout {

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Appearance")
        }

        Controls2.CheckBox {
            checked: Kongress.SettingsController.displayInLocalTimezone
            Kirigami.FormData.label: i18n("Use local timezone")

            onToggled: Kongress.SettingsController.displayInLocalTimezone = checked
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Notifications")
        }

        Controls2.CheckBox {
            Kirigami.FormData.label: i18n("Add reminder to favorite talks")

            enabled: Kongress.SettingsController.canAddReminder
            checked: Kongress.SettingsController.remindFavorites
            onToggled: Kongress.SettingsController.remindFavorites = checked
        }

        Controls2.SpinBox {
            Kirigami.FormData.label: i18n("Remind before talk start (minutes)")

            enabled: Kongress.SettingsController.canAddReminder
            from: 0
            value: Kongress.SettingsController.remindBeforeStart
            onValueModified: Kongress.SettingsController.remindBeforeStart = value
        }

        Kirigami.InlineMessage {
            Kirigami.FormData.isSection: true
            visible: !Kongress.SettingsController.canAddReminder
            type: Kirigami.MessageType.Warning
            text: i18n("Notifications are not supported on your platform")
        }
    }

}
