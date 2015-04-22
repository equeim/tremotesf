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

import harbour.tremotesf 0.1

CoverBackground {
    Image {
        anchors.centerIn: parent
        sourceSize.height: parent.height
        sourceSize.width: parent.height
        opacity: 0.15
        source: "../../cover.png"
    }

    Item {
        anchors {
            top: parent.top
            topMargin: Theme.paddingLarge
            bottom: parent.bottom
            left: parent.left
            leftMargin: Theme.paddingLarge
            right: parent.right
            rightMargin: Theme.paddingMedium
        }

        Label {
            id: errorLabel
            text: {
                if (transmission.error === Transmission.AuthenticationError)
                    return qsTr("Authentication error")
                if (transmission.error === Transmission.ConnectionError)
                    return qsTr("Connection error")
                return String()
            }
            elide: Text.ElideRight
            font.pixelSize: Theme.fontSizeLarge
            maximumLineCount: 2
            visible: transmission.error !== Transmission.NoError
            width: parent.width
            wrapMode: Text.Wrap
        }

        Column {
            id: speedColumn
            spacing: Theme.paddingLarge
            width: parent.width
            visible: !errorLabel.visible

            Label {
                id: downLabel
                text: qsTr("↓ %1/s").arg(Format.formatFileSize(appSettings.downloadSpeed))
                font.pixelSize: Theme.fontSizeLarge
                truncationMode: TruncationMode.Fade
                width: parent.width
            }

            Label {
                id: upLabel
                text: qsTr("↑ %1/s").arg(Format.formatFileSize(appSettings.uploadSpeed))
                font.pixelSize: Theme.fontSizeLarge
                truncationMode: TruncationMode.Fade
                width: parent.width
            }
        }

        Column {
            anchors {
                top: errorLabel.visible ? errorLabel.bottom : speedColumn.bottom
                topMargin: Theme.paddingLarge
            }
            width: parent.width

            Label {
                id: accountLabel
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: appSettings.currentAccount
                truncationMode: TruncationMode.Fade
                width: parent.width
            }

            Label {
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: {
                    if (accountLabel.text)
                        return appSettings.getClientValue(accountLabel.text + "/address")
                    return String()
                }
                truncationMode: TruncationMode.Fade
                width: parent.width
            }
        }
    }
}
