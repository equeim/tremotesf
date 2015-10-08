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

Dialog {
    allowedOrientations: Orientation.All
    canAccept: linkField.text
    onAccepted: root.transmission.addTorrent(linkField.text, downloadDirectoryTextField.text, !pauseSwitch.checked)

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                title: qsTr("Add torrent")
            }

            Item {
                height: linkField.height
                width: parent.width

                CommonTextField {
                    id: linkField

                    anchors {
                        left: parent.left
                        right: selectFileButton.left
                    }
                    inputMethodHints: Qt.ImhUrlCharactersOnly
                    label: qsTr("HTTP, Magnet or local file path")
                }

                IconButton {
                    id: selectFileButton

                    anchors {
                        right: parent.right
                        rightMargin: Theme.paddingMedium
                    }
                    icon.source: "image://theme/icon-m-folder"
                    icon.sourceSize.height: Theme.iconSizeMedium
                    icon.sourceSize.width: Theme.iconSizeMedium

                    onClicked: pageStack.push(torrentFilePickerDialog)

                    Component {
                        id: torrentFilePickerDialog

                        TorrentFilePickerDialog {
                            onAccepted: linkField.text = path
                        }
                    }
                }
            }

            SelectDirectoryTextField {
                id: downloadDirectoryTextField
                label: qsTr("Download directory")
                text: root.appSettings.serverValue("download-dir")
            }

            TextSwitch {
                id: pauseSwitch
                text: qsTr("Start torrent")
                description: qsTr("Start downloading after adding")
            }
        }

        VerticalScrollDecorator { }
    }
}
