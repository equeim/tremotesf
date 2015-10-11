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
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Speed")
            }

            SectionHeader {
                text: qsTr("Limits")
            }

            ServerSettingsTextSwitch {
                id: downloadLimitSwitch
                key: "speed-limit-down-enabled"
                text: qsTr("Limit download speed")
            }

            ServerSettingsTextField {
                key: "speed-limit-down"
                label: qsTr("kB/s")
                visible: downloadLimitSwitch.checked
            }

            ServerSettingsTextSwitch {
                id: uploadLimitSwitch
                key: "speed-limit-up-enabled"
                text: qsTr("Limit upload speed")
            }

            ServerSettingsTextField {
                label: qsTr("kB/s")
                key: "speed-limit-up"
                visible: uploadLimitSwitch.checked
            }

            SectionHeader {
                text: qsTr("Alternative limits")
            }

            ServerSettingsTextSwitch {
                id: alternativeLimitsSwitch
                key: "alt-speed-enabled"
                text: qsTr("Enable alternative speed limits")
            }

            ServerSettingsTextField {
                key: "alt-speed-down"
                label: qsTr("Alternative download speed, kB/s")
                visible: alternativeLimitsSwitch.checked
            }

            ServerSettingsTextField {
                key: "alt-speed-up"
                label: qsTr("Alternative upload speed, kB/s")
                visible: alternativeLimitsSwitch.checked
            }

            ServerSettingsTextSwitch {
                id: scheduleSwitch
                key: "alt-speed-time-enabled"
                text: qsTr("Schedule")
                visible: alternativeLimitsSwitch.checked
            }

            Row {
                height: Theme.itemSizeSmall
                visible: scheduleSwitch.visible && scheduleSwitch.checked

                AlternativeLimitsTimeButton {
                    key: "alt-speed-time-begin"
                    label: qsTr("From")
                    width: column.width / 2
                }

                AlternativeLimitsTimeButton {
                    key: "alt-speed-time-end"
                    label: qsTr("to")
                    width: column.width / 2
                }
            }

            CommonComboBox {
                currentIndex: {
                    switch (root.appSettings.serverValue("alt-speed-time-day")) {
                    case 127: // every day
                        return 0
                    case 62: // weekdays
                        return 1
                    case 65: // weekends
                        return 2
                    case 1: // sunday
                        return 3
                    case 2: // monday
                        return 4
                    case 4: // tuesday
                        return 5
                    case 8: // wednesday
                        return 6
                    case 16: // thursday
                        return 7
                    case 32: // friday
                        return 8
                    case 64: // saturday
                        return 9
                    }
                }
                label: qsTr("Days")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Every day")
                    }
                    MenuItem {
                        text: qsTr("Weekdays")
                    }
                    MenuItem {
                        text: qsTr("Weekends")
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Sunday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Monday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Tuesday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Wednesday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Thursday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Friday)
                    }
                    MenuItem {
                        text: Qt.locale().dayName(Qt.Saturday)
                    }
                }
                visible: scheduleSwitch.checked && scheduleSwitch.visible

                Component.onDestruction: {
                    if (changed()) {
                        switch (currentIndex) {
                        case 0:
                            root.transmission.changeServerSettings("alt-speed-time-day", 127)
                            break
                        case 1:
                            root.transmission.changeServerSettings("alt-speed-time-day", 62)
                            break
                        case 2:
                            root.transmission.changeServerSettings("alt-speed-time-day", 65)
                            break
                        case 3:
                            root.transmission.changeServerSettings("alt-speed-time-day", 1)
                            break
                        case 4:
                            root.transmission.changeServerSettings("alt-speed-time-day", 2)
                            break
                        case 5:
                            root.transmission.changeServerSettings("alt-speed-time-day", 4)
                            break
                        case 6:
                            root.transmission.changeServerSettings("alt-speed-time-day", 8)
                            break
                        case 7:
                            root.transmission.changeServerSettings("alt-speed-time-day", 16)
                            break
                        case 8:
                            root.transmission.changeServerSettings("alt-speed-time-day", 32)
                            break
                        case 9:
                            root.transmission.changeServerSettings("alt-speed-time-day", 64)
                        }
                    }
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
