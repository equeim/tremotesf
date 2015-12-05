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
            elide: Text.ElideRight
            font.pixelSize: Theme.fontSizeLarge
            maximumLineCount: 2
            text: {
                if (!transmission.accountConfigured)
                    return qsTr("No account connected")
                return transmission.errorString
            }
            visible: transmission.error !== Tremotesf.Transmission.NoError ||
                     !transmission.accountConfigured
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
                text: "↓ %1".arg(Tremotesf.Utils.formatByteSpeed(Tremotesf.AppSettings.downloadSpeed))
                font.pixelSize: Theme.fontSizeLarge
                truncationMode: TruncationMode.Fade
                width: parent.width
            }

            Label {
                id: upLabel
                text: "↑ %1".arg(Tremotesf.Utils.formatByteSpeed(Tremotesf.AppSettings.uploadSpeed))
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
            visible: transmission.accountConfigured
            width: parent.width

            Label {
                id: accountLabel
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: Tremotesf.AppSettings.currentAccount
                truncationMode: TruncationMode.Fade
                width: parent.width
            }

            Label {
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: Tremotesf.AppSettings.accountAddress(accountLabel.text)
                truncationMode: TruncationMode.Fade
                width: parent.width
            }
        }
    }
}
