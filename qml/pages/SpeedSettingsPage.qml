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
                if (downloadLimitSwitch.changed())
                    transmission.changeServerSettings("speed-limit-down-enabled", downloadLimitSwitch.checked)
                if (downloadLimitField.changed())
                    transmission.changeServerSettings("speed-limit-down", parseInt(downloadLimitField.text))
                if (uploadLimitSwitch.changed())
                    transmission.changeServerSettings("speed-limit-up-enabled", uploadLimitSwitch.checked)
                if (uploadLimitField.changed())
                    transmission.changeServerSettings("speed-limit-up", parseInt(uploadLimitField.text))
                if (alternativeLimitsSwitch.changed())
                    transmission.changeServerSettings("alt-speed-enabled", alternativeLimitsSwitch.checked)
                if (alternativeDownloadLimitField.changed())
                    transmission.changeServerSettings("alt-speed-down", parseInt(alternativeDownloadLimitField.text))
                if (alternativeUploadLimitField.changed())
                    transmission.changeServerSettings("alt-speed-up", parseInt(alternativeUploadLimitField.text))
                if (scheduleSwitch.changed())
                    transmission.changeServerSettings("alt-speed-time-enabled", scheduleSwitch.checked)
                if (beginTimeButton.changed())
                    transmission.changeServerSettings("alt-speed-time-begin", beginTimeButton.timeInMinutes)
                if (endTimeButton.changed())
                    transmission.changeServerSettings("alt-speed-time-end", endTimeButton.timeInMinutes)
                if (dayComboBox.changed()) {
                    switch (dayComboBox.currentIndex) {
                    case 0:
                        transmission.changeServerSettings("alt-speed-time-day", 127)
                        break
                    case 1:
                        transmission.changeServerSettings("alt-speed-time-day", 62)
                        break
                    case 2:
                        transmission.changeServerSettings("alt-speed-time-day", 65)
                        break
                    case 3:
                        transmission.changeServerSettings("alt-speed-time-day", 1)
                        break
                    case 4:
                        transmission.changeServerSettings("alt-speed-time-day", 2)
                        break
                    case 5:
                        transmission.changeServerSettings("alt-speed-time-day", 4)
                        break
                    case 6:
                        transmission.changeServerSettings("alt-speed-time-day", 8)
                        break
                    case 7:
                        transmission.changeServerSettings("alt-speed-time-day", 16)
                        break
                    case 8:
                        transmission.changeServerSettings("alt-speed-time-day", 32)
                        break
                    case 9:
                        transmission.changeServerSettings("alt-speed-time-day", 64)
                    }
                }
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
                title: qsTr("Speed")
            }

            SectionHeader {
                text: qsTr("Limits")
            }

            CommonTextSwitch {
                id: downloadLimitSwitch
                checked: appSettings.serverValue("speed-limit-down-enabled")
                text: qsTr("Limit download speed")
            }

            CommonTextField {
                id: downloadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                text: appSettings.serverValue("speed-limit-down")
                visible: downloadLimitSwitch.checked
            }

            CommonTextSwitch {
                id: uploadLimitSwitch
                checked: appSettings.serverValue("speed-limit-up-enabled")
                text: qsTr("Limit upload speed")
            }

            CommonTextField {
                id: uploadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                text: appSettings.serverValue("speed-limit-up")
                visible: uploadLimitSwitch.checked
            }

            SectionHeader {
                text: qsTr("Alternative limits")
            }

            CommonTextSwitch {
                id: alternativeLimitsSwitch
                checked: appSettings.serverValue("alt-speed-enabled")
                text: qsTr("Enable alternative speed limits")
            }

            CommonTextField {
                id: alternativeDownloadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Alternative download speed, kB/s")
                text: appSettings.serverValue("alt-speed-down")
                visible: alternativeLimitsSwitch.checked
            }

            CommonTextField {
                id: alternativeUploadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Alternative upload speed, kB/s")
                text: appSettings.serverValue("alt-speed-up")
                visible: alternativeLimitsSwitch.checked
            }

            CommonTextSwitch {
                id: scheduleSwitch
                checked: appSettings.serverValue("alt-speed-time-enabled")
                text: qsTr("Schedule")
                visible: alternativeLimitsSwitch.checked
            }

            Row {
                height: Theme.itemSizeSmall
                visible: scheduleSwitch.checked && scheduleSwitch.visible

                AlternativeLimitsTimeButton {
                    id: beginTimeButton
                    label: qsTr("From")
                    timeInMinutes: appSettings.serverValue("alt-speed-time-begin")
                    width: column.width / 2
                }

                AlternativeLimitsTimeButton {
                    id: endTimeButton
                    label: qsTr("to")
                    timeInMinutes: appSettings.serverValue("alt-speed-time-end")
                    width: column.width / 2
                }
            }

            CommonComboBox {
                id: dayComboBox

                currentIndex: {
                    switch (appSettings.serverValue("alt-speed-time-day")) {
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
            }
        }

        VerticalScrollDecorator { }
    }
}
