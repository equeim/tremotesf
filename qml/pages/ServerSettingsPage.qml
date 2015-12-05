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

import "../components"

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Server settings")
            }

            CommonListItem {
                text: qsTr("Downloading")
                onClicked: pageStack.push("DownloadingSettingsPage.qml")
            }

            CommonListItem {
                text: qsTr("Speed")
                onClicked: pageStack.push("SpeedSettingsPage.qml")
            }

            CommonListItem {
                text: qsTr("Queue")
                onClicked: pageStack.push("QueueSettingsPage.qml")
            }

            CommonListItem {
                text: qsTr("Network")
                onClicked: pageStack.push("NetworkSettingsPage.qml")
            }
        }

        VerticalScrollDecorator { }
    }
}
