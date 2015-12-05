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

import "../components"

Page {
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

            ServerSettingsTextField {
                key: "peer-port"
                label: qsTr("Peer port")
            }

            ServerSettingsTextSwitch {
                key: "peer-port-random-on-start"
                text: qsTr("Random port on Transmission start")
            }

            ServerSettingsTextSwitch {
                id: forwardingSwitch
                key: "port-forwarding-enabled"
                text: qsTr("Enable port forwarding")
            }

            CommonComboBox {
                currentIndex: {
                    switch (Tremotesf.AppSettings.serverValue("encryption")) {
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

                Component.onDestruction: {
                    if (changed()) {
                        switch (currentIndex) {
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
                }
            }

            ServerSettingsTextSwitch {
                key: "utp-enabled"
                text: qsTr("Enable uTP")
            }

            ServerSettingsTextSwitch {
                key: "pex-enabled"
                text: qsTr("Enable PEX")
            }

            ServerSettingsTextSwitch {
                key: "dht-enabled"
                text: qsTr("Enable DHT")
            }

            ServerSettingsTextSwitch {
                key: "lpd-enabled"
                text: qsTr("Enable LPD")
            }

            SectionHeader {
                text: qsTr("Peer limits")
            }

            ServerSettingsTextField {
                key: "peer-limit-per-torrent"
                label: qsTr("Maximum peers per torrent")
            }

            ServerSettingsTextField {
                key: "peer-limit-global"
                label: qsTr("Maximum peers globally")
            }
        }

        VerticalScrollDecorator { }
    }
}
