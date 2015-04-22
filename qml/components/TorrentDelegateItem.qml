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

Item {
    width: parent.width
    height: Theme.itemSizeExtraLarge * 1.8

    Image {
        id: statusIcon

        anchors {
            top: parent.top
            topMargin: Theme.paddingSmall
            left: parent.left
            leftMargin: Theme.paddingSmall
        }
        rotation: {
            if (torrentStatus === 3 || torrentStatus === 4)
                return 90
            else if (torrentStatus === 5 || torrentStatus === 6)
                return -90
            else if (torrentStatus === 1 || torrentStatus=== 2 || torrentStatus=== 7)
                return 0
            return 0
        }
        source: {
            if (torrentStatus === 0 || torrentStatus === 1 || torrentStatus === 2 || torrentStatus === 7)
                return "image://theme/icon-m-pause"
            else if (torrentStatus === 3 || torrentStatus === 4 || torrentStatus === 5 || torrentStatus === 6)
                return "image://theme/icon-m-play"
        }
        sourceSize.height: Theme.iconSizeMedium
        sourceSize.width: Theme.iconSizeMedium
    }

    Label {
        anchors {
            left: statusIcon.right
            leftMargin: Theme.paddingMedium
            verticalCenter: statusIcon.verticalCenter
            right: parent.right
        }
        color: highlighted ? Theme.highlightColor : Theme.primaryColor
        text: model.name
        truncationMode: TruncationMode.Fade
    }

    Label {
        anchors {
            top: statusIcon.bottom
            left: parent.left
            leftMargin: Theme.paddingLarge
        }
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
        text: {
            if (model.percentDone === 100)
                return qsTr("%1, uploaded %2").arg(Format.formatFileSize(model.sizeWhenDone)).arg(Format.formatFileSize(model.uploadedEver))
            else
                return qsTr("%1 of %2 (%L3%)").arg(Format.formatFileSize(model.sizeWhenDone - model.leftUntilDone))
            .arg(Format.formatFileSize(model.sizeWhenDone))
            .arg(Math.floor(model.percentDone * 100) / 100);
        }
    }

    Label {
        anchors {
            top: statusIcon.bottom
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
        text: torrentModel.formatEta(model.eta)
    }

    ProgressBar {
        anchors {
            bottom: speedColumn.top
            bottomMargin: -Theme.paddingMedium
        }
        maximumValue: 100
        value: {
            if (torrentStatus === 2) {
                return Math.floor(model.recheckProgress * 100) / 100
            }
            return Math.floor(model.percentDone * 100) / 100
        }
        width: parent.width
    }

    Column {
        id: speedColumn

        anchors {
            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium
            left: parent.left
            leftMargin: Theme.paddingLarge
            right: parent.horizontalCenter
        }

        Label {
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
            text: qsTr("↓ %1/s").arg(Format.formatFileSize(model.rateDownload))
        }

        Label {
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
            text: qsTr("↑ %1/s").arg(Format.formatFileSize(model.rateUpload))
        }
    }

    Label {
        anchors {
            top: speedColumn.top
            left: parent.horizontalCenter
            right: parent.right
            rightMargin: Theme.paddingLarge
        }
        color: Theme.secondaryColor
        elide: Text.ElideRight
        font.pixelSize: Theme.fontSizeSmall
        horizontalAlignment: Text.AlignRight
        maximumLineCount: 2
        text: {
            if (model.errorString)
                return model.errorString

            switch (torrentStatus) {
            case 0:
                return qsTr("Stopped")
            case 1:
                return qsTr("Queued for checking")
            case 2:
                return qsTr("Checking")
            case 3:
                return qsTr("Stalled")
            case 4:
                return qsTr("Downloading\nfrom %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersSendingToUs)
            case 5:
                return qsTr("Queued to seed")
            case 6:
                return qsTr("Seeding\nto %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersGettingFromUs)
            case 7:
                return qsTr("Isolated")
            case 8:
                return qsTr("Error")
            }
        }
        wrapMode: Text.WordWrap
    }
}
