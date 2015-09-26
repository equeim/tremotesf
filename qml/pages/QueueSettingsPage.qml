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
                if (downloadQueueSwitch.changed())
                    transmission.changeServerSettings("download-queue-enabled", downloadQueueSwitch.checked)
                if (downloadQueueField.changed())
                    transmission.changeServerSettings("download-queue-size", parseInt(downloadQueueField.text))
                if (seedQueueSwitch.changed())
                    transmission.changeServerSettings("seed-queue-enabled", seedQueueSwitch.checked)
                if (seedQueueField.changed())
                    transmission.changeServerSettings("seed-queue-size", parseInt(seedQueueField.text))
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
                title: qsTr("Queue")
            }

            CommonTextSwitch {
                id: downloadQueueSwitch
                checked: appSettings.serverValue("download-queue-enabled")
                text: qsTr("Enable download queue")
            }

            CommonTextField {
                id: downloadQueueField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum active downloads")
                text: appSettings.serverValue("download-queue-size")
                visible: downloadQueueSwitch.checked
            }

            CommonTextSwitch {
                id: seedQueueSwitch
                checked: appSettings.serverValue("seed-queue-enabled")
                text: qsTr("Enable seed queue")
            }

            CommonTextField {
                id: seedQueueField
                inputMethodHints: Qt.ImhDigitsOnly
                label: qsTr("Maximum active uploads")
                text: appSettings.serverValue("seed-queue-size")
                visible: seedQueueSwitch.checked
            }
        }

        VerticalScrollDecorator { }
    }
}
