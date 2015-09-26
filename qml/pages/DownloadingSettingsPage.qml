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
                if (downloadDirectoryField.changed())
                    transmission.changeServerSettings("download-dir", downloadDirectoryField.text)
                if (renamePartialSwitch.changed())
                    transmission.changeServerSettings("rename-partial-files", renamePartialSwitch.checked)
                if (trashTorrentSwitch.changed())
                    transmission.changeServerSettings("trash-original-torrent-files", trashTorrentSwitch.checked)
                if (incompleteDirectorySwitch.changed())
                    transmission.changeServerSettings("incomplete-dir-enabled", incompleteDirectorySwitch.checked)
                if (incompleteDirectoryField.changed())
                    transmission.changeServerSettings("incomplete-dir", incompleteDirectoryField.text)
                if (seedRatioLimitSwitch.changed())
                    transmission.changeServerSettings("seedRatioLimited", seedRatioLimitSwitch.checked)
                if (seedRatioLimitField.changed())
                    transmission.changeServerSettings("seedRatioLimit", parseFloat(seedRatioLimitField.text.replace(",", ".")))
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
                title: qsTr("Downloading")
            }

            SelectDirectoryTextField {
                id: downloadDirectoryField
                label: qsTr("Download directory")
                text: appSettings.serverValue("download-dir")
            }

            CommonTextSwitch {
                id: renamePartialSwitch
                checked: appSettings.serverValue("rename-partial-files")
                text: qsTr("Append \".part\" to names of incomplete files")
            }

            CommonTextSwitch {
                id: incompleteDirectorySwitch
                checked: appSettings.serverValue("incomplete-dir-enabled")
                text: qsTr("Separate directory fo incomplete files")
            }

            SelectDirectoryTextField {
                id: incompleteDirectoryField
                text: appSettings.serverValue("incomplete-dir")
                visible: incompleteDirectorySwitch.checked
            }

            CommonTextSwitch {
                id: trashTorrentSwitch
                checked: appSettings.serverValue("trash-original-torrent-files")
                text: qsTr("Trash .torrent files")
            }

            CommonTextSwitch {
                id: seedRatioLimitSwitch
                checked: appSettings.serverValue("seedRatioLimited")
                text: qsTr("Ratio limit")
            }

            CommonTextField {
                id: seedRatioLimitField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Stop seeding at ratio")
                text: qsTr("%L1").arg(Math.ceil(appSettings.serverValue("seedRatioLimit") * 100) / 100)
                visible: seedRatioLimitSwitch.checked
            }
        }

        VerticalScrollDecorator { }
    }
}
