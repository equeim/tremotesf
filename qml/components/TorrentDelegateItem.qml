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

Item {
    height: column.height + Theme.paddingMedium
    width: parent.width

    ListItemMargin {
        Column {
            id: column
            width: parent.width

            Item {
                height: childrenRect.height
                width: parent.width

                Image {
                    id: statusIcon

                    rotation: {
                        if (torrentStatus === TorrentModel.StalledStatus ||
                                torrentStatus === TorrentModel.DownloadingStatus)
                            return 90
                        if (torrentStatus === TorrentModel.QueuedForSeedingStatus ||
                                torrentStatus === TorrentModel.SeedingStatus)
                            return -90
                        return 0
                    }
                    source: {
                        if (torrentStatus === TorrentModel.StoppedStatus ||
                                torrentStatus === TorrentModel.QueuedForCheckingStatus ||
                                torrentStatus === TorrentModel.CheckingStatus ||
                                torrentStatus === TorrentModel.IsolatedStatus)
                            return "image://theme/icon-m-pause"
                        if (torrentStatus === TorrentModel.StalledStatus ||
                                torrentStatus === TorrentModel.DownloadingStatus ||
                                torrentStatus === TorrentModel.QueuedForSeedingStatus ||
                                torrentStatus === TorrentModel.SeedingStatus)
                            return "image://theme/icon-m-play"
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
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: {
                        if (model.percentDone === 100)
                            return qsTr("%1, uploaded %2").arg(Format.formatFileSize(model.sizeWhenDone)).arg(Format.formatFileSize(model.uploadedEver))
                        else
                            return qsTr("%1 of %2 (%L3%)").arg(Format.formatFileSize(model.sizeWhenDone - model.leftUntilDone))
                        .arg(Format.formatFileSize(model.sizeWhenDone))
                        .arg(model.percentDone);
                    }
                }

                Label {
                    anchors.right: parent.right
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: {
                        if (model.eta < 0 ||
                                torrentStatus === TorrentModel.StoppedStatus)
                            return "∞"
                        return root.torrentModel.formatEta(model.eta)
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
                        if (torrentStatus === TorrentModel.CheckingStatus)
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
                            left: parent.horizontalCenter
                            right: parent.right
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
                            case TorrentModel.StoppedStatus:
                                return qsTr("Stopped")
                            case TorrentModel.QueuedForCheckingStatus:
                                return qsTr("Queued for checking")
                            case TorrentModel.CheckingStatus:
                                return qsTr("Checking")
                            case TorrentModel.StalledStatus:
                                return qsTr("Stalled")
                            case TorrentModel.DownloadingStatus:
                                return qsTr("Downloading\nfrom %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersSendingToUs)
                            case TorrentModel.QueuedForSeedingStatus:
                                return qsTr("Queued to seeding")
                            case TorrentModel.SeedingStatus:
                                return qsTr("Seeding\nto %1 of %n peer(s)", String(), model.peersConnected).arg(model.peersGettingFromUs)
                            case TorrentModel.IsolatedStatus:
                                return qsTr("Isolated")
                            case TorrentModel.ErrorStatus:
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
}
