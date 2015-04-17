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
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
        }
    }

    Connections {
        target: pageStack
        onDepthChanged: {
            if (depth === pageStack.depth) {
                if (sessionLimitsSwitch.changed())
                    transmission.changeTorrent(torrentId, "honorsSessionLimits", sessionLimitsSwitch.checked)
                if (downloadLimitSwitch.changed())
                    transmission.changeTorrent(torrentId, "downloadLimited", downloadLimitSwitch.checked)
                if (downloadLimitField.changed())
                    transmission.changeTorrent(torrentId, "downloadLimit", parseInt(downloadLimitField.text))
                if (uploadLimitSwitch.changed())
                    transmission.changeTorrent(torrentId, "uploadLimited", uploadLimitSwitch.checked)
                if (uploadLimitField.changed())
                    transmission.changeTorrent(torrentId, "uploadLimit", parseInt(uploadLimitField.text))
                if (priorityComboBox.changed())
                    transmission.changeTorrent(torrentId, "bandwidthPriority", 1 - priorityComboBox.currentIndex)
                if (seedRatioModeComboBox.changed())
                    transmission.changeTorrent(torrentId, "seedRatioMode", seedRatioModeComboBox.currentIndex)
                if (ratioLimitField.changed())
                    transmission.changeTorrent(torrentId, "seedRatioLimit", parseFloat(ratioLimitField.text.replace(",", ".")))
                if (peerLimitField.changed())
                    transmission.changeTorrent(torrentId, "peer-limit", parseInt(peerLimitField.text))
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
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Speed")
            }

            CommonTextSwitch {
                id: sessionLimitsSwitch
                text: qsTr("Honor global limits")

                Component.onCompleted: {
                    checked = model.honorsSessionLimits
                    oldChecked = checked
                }
            }

            CommonTextSwitch {
                id: downloadLimitSwitch
                text: qsTr("Limit download speed")

                Component.onCompleted: {
                    checked = model.downloadLimited
                    oldChecked = checked
                }
            }

            CommonTextField {
                id: downloadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                visible: downloadLimitSwitch.checked

                Component.onCompleted: {
                    text = model.downloadLimit
                    oldText = text
                }
            }

            CommonTextSwitch {
                id: uploadLimitSwitch
                text: qsTr("Limit upload speed")

                Component.onCompleted: {
                    checked = model.uploadLimited
                    oldChecked = checked
                }
            }

            CommonTextField {
                id: uploadLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("kB/s")
                visible: uploadLimitSwitch.checked

                Component.onCompleted: {
                    text = model.uploadLimit
                    oldText = text
                }
            }

            CommonComboBox {
                id: priorityComboBox

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
                    currentIndex = 1 - model.bandwidthPriority
                    oldIndex = currentIndex
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
            }

            CommonTextField {
                id: ratioLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                visible: seedRatioModeComboBox.currentIndex === 1

                Component.onCompleted: {
                    text = qsTr("%L1").arg(Math.ceil(model.seedRatioLimit * 100) / 100)
                    oldText = text
                }
            }

            SectionHeader {
                text: qsTr("Peers")
            }

            CommonTextField {
                id: peerLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum peers")

                Component.onCompleted: {
                    text = model.peerLimit
                    oldText = text
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
