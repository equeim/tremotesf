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

import QtQuick 2.2
import Sailfish.Silica 1.0

import harbour.tremotesf 0.1 as Tremotesf

import "../dialogs"

Column {
    property alias name: nameField.text
    property bool nameChanged: nameField.changed()

    property alias address: addressField.text
    property alias port: portField.text
    property alias apiPath: apiField.text
    property alias https: httpsSwitch.checked
    property alias localCertificate: localCertificateSwitch.checked
    property bool localCertificateChanged: false
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

    TextSwitch {
        id: httpsSwitch
        text: qsTr("HTTPS")
        onCheckedChanged: {
            if (checked)
                port = 443
            else
                port = 9091
        }
    }

    TextSwitch {
        id: localCertificateSwitch
        text: qsTr("Local certificate")
        visible: httpsSwitch.checked
    }

    Column {
        spacing: Theme.paddingMedium
        visible: localCertificateSwitch.visible && localCertificateSwitch.checked
        width: parent.width

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: name.length !== 0
            text: qsTr("Load certificate")
            onClicked: pageStack.push(filePickerDialog)

            Component {
                id: filePickerDialog

                FilePickerDialog {
                    nameFilters: ["*.pem"]
                    showFiles: true
                    onAccepted: {
                        if (Tremotesf.Utils.checkLocalCertificate(path)) {
                            Tremotesf.AppSettings.setAccountLocalCertificate(name, path)
                            localCertificateChanged = true
                            removeButton.enabled = appSettings.isAccountLocalCertificateExists(name)
                        } else {
                            pageStack.push(warningDialog)
                        }
                    }
                }
            }

            Component {
                id: warningDialog

                Dialog {
                    SilicaFlickable {
                        anchors.fill: parent
                        contentHeight: column.height

                        Column {
                            id: column
                            width: parent.width

                            DialogHeader {
                                title: qsTr("Error")
                            }

                            Label {
                                anchors {
                                    left: parent.left
                                    leftMargin: Theme.horizontalPageMargin
                                    right: parent.right
                                    rightMargin: Theme.horizontalPageMargin
                                }
                                text: qsTr("Local certificate should be a .pem file containing both certificate and RSA private key.")
                                wrapMode: Text.WordWrap
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: removeButton
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: {
                if (name.length === 0)
                    return false
                return Tremotesf.AppSettings.isAccountLocalCertificateExists(name)
            }
            text: qsTr("Remove certificate")
            onClicked: {
                Tremotesf.AppSettings.removeAccountLocalCertificate(name)
                localCertificateChanged = true
                enabled = Tremotesf.AppSettings.isAccountLocalCertificateExists(name)
            }
        }
    }

    TextSwitch {
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
