/*
 * SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>
 * SPDX-FileCopyrightText: 2020 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kongress as Kongress

FormCard.FormCardPage {
    id: root

    title: i18n("Settings")

    FormCard.FormHeader {
        title: i18n("Appearance")
    }

    FormCard.FormCard {
    	FormCard.FormCheckDelegate {
            checked: Kongress.SettingsController.displayInLocalTimezone
            text: i18n("Use local timezone")

            onToggled: Kongress.SettingsController.displayInLocalTimezone = checked
        }
    }

    FormCard.FormHeader {
        title: i18n("Notifications")
    }

    FormCard.FormCard {
	Kirigami.InlineMessage {
  	    Kirigami.FormData.isSection: true
 	    visible: !Kongress.SettingsController.canAddReminder
  	    type: Kirigami.MessageType.Warning
  	    text: i18n("Notifications are not supported on your platform")
            Layout.topMargin: visible ? Kirigami.Units.smallSpacing : 0
            Layout.leftMargin: visible ? Kirigami.Units.smallSpacing : 0
            Layout.rightMargin: visible ? Kirigami.Units.smallSpacing : 0
            Layout.fillWidth: true
    	}
    	FormCard.FormCheckDelegate {
            text: i18n("Add reminder to favorite talks")

            enabled: Kongress.SettingsController.canAddReminder
            checked: Kongress.SettingsController.remindFavorites
            onToggled: Kongress.SettingsController.remindFavorites = checked
        }
        FormCard.FormSpinBoxDelegate {
            label: i18n("Remind before talk start (minutes)")

            enabled: Kongress.SettingsController.canAddReminder
            from: 0
            value: Kongress.SettingsController.remindBeforeStart
            onValueChanged: Kongress.SettingsController.remindBeforeStart = value
        }
    }
}
