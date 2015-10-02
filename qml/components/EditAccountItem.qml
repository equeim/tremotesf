/*
 * Tremotesf
 * Copyright (C) 2015 Alexey Rochev <equeim@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import Sailfish.Silica 1.0

Column {
    property alias name: nameField.text
    property alias address: addressField.text
    property alias port: portField.text
    property alias apiPath: apiField.text

    // FIXME
    property bool https: false
    property bool localCertificate: false

    property alias authentication: authenticationSwitch.checked
    property alias username: usernameField.text
    property alias password: passwordField.text
    property alias timeout: timeoutField.text
    property alias updateInterval: updateField.text

    width: parent.width

    CommonTextField {
        id: nameField
        label: qsTr("Account name")
    }

    CommonTextField {
        id: addressField
        inputMethodHints: Qt.ImhUrlCharactersOnly
        label: qsTr("Server address")
    }

    CommonTextField {
        id: portField
        inputMethodHints: Qt.ImhDigitsOnly
        label: qsTr("Port")
    }

    CommonTextField {
        id: apiField
        inputMethodHints: Qt.ImhUrlCharactersOnly
        label: qsTr("API path")
    }

    CommonTextSwitch {
        id: authenticationSwitch
        text: qsTr("Authentication")
    }

    CommonTextField {
        id: usernameField
        label: qsTr("Username")
        inputMethodHints: Qt.ImhNoAutoUppercase
        visible: authenticationSwitch.checked
    }

    CommonTextField {
        id: passwordField
        echoMode: TextInput.Password
        label: qsTr("Password")
        visible: authenticationSwitch.checked
    }

    CommonTextField {
        id: updateField
        inputMethodHints: Qt.ImhDigitsOnly
        label: qsTr("Update interval, s")
    }

    CommonTextField {
        id: timeoutField
        inputMethodHints: Qt.ImhDigitsOnly
        label: qsTr("Timeout, s")
    }
}
