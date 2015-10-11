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

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Speed")
            }

            CommonTextSwitch {
                text: qsTr("Honor global limits")

                Component.onCompleted: {
                    checked = model.honorsSessionLimits
                    oldChecked = checked
                }
                Component.onDestruction: {
                    if (changed())
                        model.honorsSessionLimits = checked
                }
            }

            CommonTextSwitch {
                id: downloadLimitSwitch
                text: qsTr("Limit download speed")

                Component.onCompleted: {
                    checked = model.downloadLimited
                    oldChecked = checked
                }
                Component.onDestruction: {
                    if (changed())
                        model.downloadLimited = checked
                }
            }

            CommonTextField {
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                visible: downloadLimitSwitch.checked

                Component.onCompleted: {
                    text = model.downloadLimit
                    oldText = text
                }
                Component.onDestruction: {
                    if (changed())
                        model.downloadLimit = parseInt(text)
                }
            }

            CommonTextSwitch {
                id: uploadLimitSwitch
                text: qsTr("Limit upload speed")

                Component.onCompleted: {
                    checked = model.uploadLimited
                    oldChecked = checked
                }
                Component.onDestruction: {
                    if (changed())
                        model.uploadLimited = checked
                }
            }

            CommonTextField {
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                visible: uploadLimitSwitch.checked

                Component.onCompleted: {
                    text = model.uploadLimit
                    oldText = text
                }
                Component.onDestruction: {
                    if (changed())
                        model.uploadLimit = parseInt(text)
                }
            }

            CommonComboBox {
                label: qsTr("Torrent priority")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("High")
                    }
                    MenuItem {
                        text: qsTr("Normal")
                    }
                    MenuItem {
                        text: qsTr("Low")
                    }
                }

                Component.onCompleted: {
                    currentIndex = (1 - model.bandwidthPriority)
                    oldIndex = currentIndex
                }
                Component.onDestruction: {
                    if (changed())
                        model.bandwidthPriority = (1 - currentIndex)
                }
            }

            SectionHeader {
                text: qsTr("Seeding")
            }

            CommonComboBox {
                id: seedRatioModeComboBox

                label: qsTr("Ratio limit")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Use global settings")
                    }
                    MenuItem {
                        text: qsTr("Stop seeding at ratio")
                    }
                    MenuItem {
                        text: qsTr("Seed regardless of ratio")
                    }
                }

                Component.onCompleted: {
                    currentIndex = model.seedRatioMode
                    oldIndex = currentIndex
                }
                Component.onDestruction: {
                    if (changed())
                        model.seedRatioMode = currentIndex
                }
            }

            CommonTextField {
                inputMethodHints: Qt.ImhDigitsOnly
                visible: seedRatioModeComboBox.currentIndex === 1

                Component.onCompleted: {
                    text = qsTr("%L1").arg(model.seedRatioLimit)
                    oldText = text
                }
                Component.onDestruction: {
                    if (changed()) {
                        model.seedRatioLimit = root.utils.parseFloat(text)
                    }
                }
            }

            SectionHeader {
                text: qsTr("Peers")
            }

            CommonTextField {
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum peers")

                Component.onCompleted: {
                    text = model.peerLimit
                    oldText = text
                }
                Component.onDestruction: {
                    if (changed())
                        model.peerLimit = parseInt(text)
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
