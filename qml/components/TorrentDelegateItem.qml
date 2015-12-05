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

Item {
    height: column.height + Theme.paddingMedium
    width: parent.width

    Column {
        id: column

        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }

        width: parent.width

        Item {
            height: childrenRect.height
            width: parent.width

            Image {
                id: statusIcon

                rotation: {
                    if (torrentStatus === Tremotesf.TorrentModel.StalledStatus ||
                            torrentStatus === Tremotesf.TorrentModel.DownloadingStatus)
                        return 90
                    if (torrentStatus === Tremotesf.TorrentModel.QueuedForSeedingStatus ||
                            torrentStatus === Tremotesf.TorrentModel.SeedingStatus)
                        return -90
                    return 0
                }
                source: {
                    var iconSource
                    if (torrentStatus === Tremotesf.TorrentModel.StoppedStatus ||
                            torrentStatus === Tremotesf.TorrentModel.QueuedForCheckingStatus ||
                            torrentStatus === Tremotesf.TorrentModel.CheckingStatus ||
                            torrentStatus === Tremotesf.TorrentModel.IsolatedStatus)
                        iconSource = "image://theme/icon-m-pause"
                    if (torrentStatus === Tremotesf.TorrentModel.StalledStatus ||
                            torrentStatus === Tremotesf.TorrentModel.DownloadingStatus ||
                            torrentStatus === Tremotesf.TorrentModel.QueuedForSeedingStatus ||
                            torrentStatus === Tremotesf.TorrentModel.SeedingStatus)
                        iconSource = "image://theme/icon-m-play"

                    if (highlighted)
                        iconSource += "?" + Theme.highlightColor

                    return iconSource
                }
                sourceSize.height: Theme.iconSizeMedium
                sourceSize.width: Theme.iconSizeMedium
            }

            Label {
                anchors {
                    left: statusIcon.right
                    leftMargin: Theme.paddingMedium
                    right: parent.right
                    verticalCenter: statusIcon.verticalCenter
                }
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
                text: model.name
                truncationMode: TruncationMode.Fade
            }
        }

        Item {
            height: childrenRect.height
            width: parent.width

            Label {
                color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: {
                    if (model.percentDone === 100)
                        return qsTr("%1, uploaded %2").arg(Tremotesf.Utils.formatByteSize(model.sizeWhenDone)).arg(Tremotesf.Utils.formatByteSize(model.uploadedEver))
                    else
                        return qsTr("%1 of %2 (%L3%)").arg(Tremotesf.Utils.formatByteSize(model.sizeWhenDone - model.leftUntilDone))
                    .arg(Tremotesf.Utils.formatByteSize(model.sizeWhenDone))
                    .arg(model.percentDone);
                }
            }

            Label {
                anchors.right: parent.right
                color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: {
                    if (model.eta < 0 ||
                            torrentStatus === Tremotesf.TorrentModel.StoppedStatus)
                        return "∞"
                    return Tremotesf.Utils.formatEta(model.eta)
                }
            }
        }

        Item {
            height: childrenRect.height
            width: parent.width

            ProgressBar {
                id: progressBar

                anchors {
                    top: parent.top
                    topMargin: -Theme.paddingMedium
                }
                maximumValue: 100
                value: {
                    if (torrentStatus === Tremotesf.TorrentModel.CheckingStatus)
                        return model.recheckProgress
                    return model.percentDone
                }
                width: parent.width
            }

            Item {
                anchors {
                    top: progressBar.bottom
                    topMargin: -Theme.paddingMedium
                }
                height: childrenRect.height
                width: parent.width

                Column {
                    anchors {
                        left: parent.left
                        right: parent.horizontalCenter
                    }

                    Label {
                        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: "↓ %1".arg(Tremotesf.Utils.formatByteSpeed(model.rateDownload))
                    }

                    Label {
                        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: "↑ %1".arg(Tremotesf.Utils.formatByteSpeed(model.rateUpload))
                    }
                }

                Label {
                    anchors {
                        left: parent.horizontalCenter
                        right: parent.right
                    }
                    color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    elide: Text.ElideRight
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignRight
                    maximumLineCount: 2
                    text: {
                        if (model.errorString)
                            return model.errorString

                        switch (torrentStatus) {
                        case Tremotesf.TorrentModel.StoppedStatus:
                            return qsTr("Stopped")
                        case Tremotesf.TorrentModel.QueuedForCheckingStatus:
                            return qsTr("Queued for checking")
                        case Tremotesf.TorrentModel.CheckingStatus:
                            return qsTr("Checking")
                        case Tremotesf.TorrentModel.StalledStatus:
                            return qsTr("Stalled")
                        case Tremotesf.TorrentModel.DownloadingStatus:
                            return qsTr("Downloading\nfrom %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersSendingToUs)
                        case Tremotesf.TorrentModel.QueuedForSeedingStatus:
                            return qsTr("Queued to seeding")
                        case Tremotesf.TorrentModel.SeedingStatus:
                            return qsTr("Seeding\nto %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersGettingFromUs)
                        case Tremotesf.TorrentModel.IsolatedStatus:
                            return qsTr("Isolated")
                        case Tremotesf.TorrentModel.ErrorStatus:
                            return qsTr("Error")
                        default:
                            return String()
                        }
                    }
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
