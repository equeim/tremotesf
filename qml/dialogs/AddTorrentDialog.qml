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

Dialog {
    canAccept: torrentTextField.text.length !== 0
    onAccepted: transmission.addTorrent(torrentTextField.text,
                                        downloadDirectoryTextField.text,
                                        !pauseSwitch.checked)

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                title: qsTr("Add torrent")
            }

            FilePickerTextField {
                id: torrentTextField
                label: qsTr("HTTP, Magnet or local file path")
                nameFilters: ["*.torrent"]
                showFiles: true
                isLocal: true
            }

            FilePickerTextField {
                id: downloadDirectoryTextField
                label: qsTr("Download directory")
                text: Tremotesf.AppSettings.serverValue("download-dir")
                showFiles: false
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
