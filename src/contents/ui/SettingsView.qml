/*
 *   Copyright 2020 David Barchiesi <david@barchie.si>
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
import org.kde.kongress 0.1 as Kongress

Kirigami.ScrollablePage {
    id: root

    title: i18n("Settings")

    ColumnLayout {
        Controls2.CheckBox {
            checked: Kongress.SettingsController.displayInLocalTimezone
            text: i18n("Use local timezone")
            onCheckedChanged: Kongress.SettingsController.displayInLocalTimezone = checked
        }
    }
}
