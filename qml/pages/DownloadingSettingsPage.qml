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
                title: qsTr("Downloading")
            }

            FilePickerTextField {
                label: qsTr("Download directory")
                text: Tremotesf.AppSettings.serverValue("download-dir")
                showFiles: false

                Component.onDestruction: {
                    if (changed())
                        transmission.changeServerSettings("download-dir", text)
                }
            }

            ServerSettingsTextSwitch {
                key: "rename-partial-files"
                text: qsTr("Append \".part\" to names of incomplete files")
            }

            ServerSettingsTextSwitch {
                id: incompleteDirectorySwitch
                key: "incomplete-dir-enabled"
                text: qsTr("Separate directory fo incomplete files")
            }

            FilePickerTextField {
                text: Tremotesf.AppSettings.serverValue("incomplete-dir")
                showFiles: false
                visible: incompleteDirectorySwitch.checked

                Component.onDestruction: {
                    if (changed())
                        transmission.changeServerSettings("incomplete-dir", text)
                }
            }

            ServerSettingsTextSwitch {
                key: "trash-original-torrent-files"
                text: qsTr("Trash .torrent files")
            }

            ServerSettingsTextSwitch {
                id: seedRatioLimitSwitch
                key: "seedRatioLimited"
                text: qsTr("Ratio limit")
            }

            ServerSettingsTextField {
                isFloat: true
                key: "seedRatioLimit"
                label: qsTr("Stop seeding at ratio")
                visible: seedRatioLimitSwitch.checked
            }
        }

        VerticalScrollDecorator { }
    }
}
