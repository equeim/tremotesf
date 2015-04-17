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

import "../components"

Page {
    property int depth

    allowedOrientations: Orientation.All
    Component.onCompleted: depth = pageStack.depth

    Connections {
        target: pageStack
        onDepthChanged: {
            if (depth === pageStack.depth) {
                if (portField.changed())
                    transmission.changeServerSettings("peer-port", parseInt(portField.text))
                if (randomPortSwitch.changed())
                    transmission.changeServerSettings("peer-port-random-on-start", randomPortSwitch.checked)
                if (forwardingSwitch.changed())
                    transmission.changeServerSettings("port-forwarding-enabled", forwardingSwitch.checked)
                if (encryptionCombo.changed()) {
                    switch (encryptionCombo.currentIndex) {
                    case 0:
                        transmission.changeServerSettings("encryption", "tolerated")
                        break
                    case 1:
                        transmission.changeServerSettings("encryption", "preferred")
                        break
                    case 2:
                        transmission.changeServerSettings("encryption", "required")
                    }
                }
                if (utpSwitch.changed())
                    transmission.changeServerSettings("utp-enabled", utpSwitch.checked)
                if (pexSwitch.changed())
                    transmission.changeServerSettings("pex-enabled", pexSwitch.checked)
                if (dhtSwitch.changed())
                    transmission.changeServerSettings("dht-enabled", dhtSwitch.checked)
                if (lpdSwitch.changed())
                    transmission.changeServerSettings("lpd-enabled", lpdSwitch.checked)
                if (peerLimitField.changed())
                    transmission.changeServerSettings("peer-limit-per-torrent", parseInt(peerLimitField.text))
                if (globalPeerLimitField.changed())
                    transmission.changeServerSettings("peer-limit-global", parseInt(globalPeerLimitField.text))
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Network")
            }

            SectionHeader {
                text: qsTr("Connection")
            }

            CommonTextField {
                id: portField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Peer port")
                text: appSettings.getServerValue("peer-port")
            }

            CommonTextSwitch {
                id: randomPortSwitch
                checked: appSettings.getServerValue("peer-port-random-on-start")
                text: qsTr("Random port on Transmission start")
            }

            CommonTextSwitch {
                id: forwardingSwitch
                checked: appSettings.getServerValue("port-forwarding-enabled")
                text: qsTr("Enable port forwarding")
            }

            CommonComboBox {
                id: encryptionCombo

                currentIndex: {
                    switch (appSettings.getServerValue("encryption")) {
                    case "tolerated":
                        return 0
                    case "preferred":
                        return 1
                    case "required":
                        return 2
                    }
                }
                label: qsTr("Encryption")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Allow")
                    }
                    MenuItem {
                        text: qsTr("Prefer")
                    }
                    MenuItem {
                        text: qsTr("Require")
                    }
                }
            }

            CommonTextSwitch {
                id: utpSwitch
                checked: appSettings.getServerValue("utp-enabled")
                text: qsTr("Enable uTP")
            }

            CommonTextSwitch {
                id: pexSwitch
                checked: appSettings.getServerValue("pex-enabled")
                text: qsTr("Enable PEX")
            }

            CommonTextSwitch {
                id: dhtSwitch
                checked: appSettings.getServerValue("dht-enabled")
                text: qsTr("Enable DHT")
            }

            CommonTextSwitch {
                id: lpdSwitch
                checked: appSettings.getServerValue("lpd-enabled")
                text: qsTr("Enable LPD")
            }

            SectionHeader {
                text: qsTr("Peer limits")
            }

            CommonTextField {
                id: peerLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum peers per torrent")
                text: appSettings.getServerValue("peer-limit-per-torrent")
            }

            CommonTextField {
                id: globalPeerLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum peers globally")
                text: appSettings.getServerValue("peer-limit-global")
            }
        }

        VerticalScrollDecorator { }
    }
}
