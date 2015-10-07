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

    function remorsePopupRemove(deleteLocalData) {
        remorse.execute(qsTr("Removing torrent"), function() {
            pageStack.pop()
            removeTorrent(deleteLocalData)
        })
    }

    allowedOrientations: Orientation.All
    Component.onCompleted: depth = pageStack.depth

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1) {
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
            }
        }
    }

    Connections {
        target: pageStack
        onDepthChanged: {
            if (depth === pageStack.depth) {
                if (fileModel.isActive)
                    fileModel.resetModel()
                if (peerModel.isActive)
                    peerModel.resetModel()
                if (trackerModel.isActive)
                    trackerModel.resetModel()
            }
        }
    }

    RemorsePopup {
        id: remorse
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Delete files and remove")
                onClicked: remorsePopupRemove(true)
            }
            MenuItem {
                text: qsTr("Remove")
                onClicked: remorsePopupRemove(false)
            }
            MenuItem {
                text: qsTr("Verify")
                onClicked: transmission.verifyTorrent(torrentId)
            }
            MenuItem {
                text: torrentStatus === 0 ? qsTr("Start") : qsTr("Stop")
                onClicked: torrentStatus === 0 ? transmission.startTorrent(torrentId) : transmission.stopTorrent(torrentId)
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Details")
            }

            TorrentDelegateItem { }

            Grid {
                property int baseItemWidth: 2 * Theme.itemSizeExtraLarge
                property int itemWidth: {
                    var tmp = (parent.width / columns) - spacing * (columns - 1) / columns
                    if (tmp >= baseItemWidth)
                        return baseItemWidth
                    else
                        return tmp
                }
                property int items: 4

                anchors.horizontalCenter: parent.horizontalCenter
                columns: {
                    var tmp = Math.round(parent.width / baseItemWidth)
                    if (tmp > items)
                        return items
                    return tmp
                }
                spacing: Theme.paddingSmall * 2
                width: columns * itemWidth + (columns - 1) * spacing

                DetailsPageGridButton {
                    text: qsTr("Files")
                    onClicked: pageStack.push(torrentFilesPage)

                    Component {
                        id: torrentFilesPage
                        TorrentFilesPage { }
                    }
                }

                DetailsPageGridButton {
                    text: qsTr("Peers")
                    onClicked: pageStack.push(torrentPeersPage)

                    Component {
                        id: torrentPeersPage
                        TorrentPeersPage { }
                    }
                }

                DetailsPageGridButton {
                    text: qsTr("Trackers")
                    onClicked: pageStack.push(torrentTrackersPage)

                    Component {
                        id: torrentTrackersPage
                        TorrentTrackersPage { }
                    }
                }

                DetailsPageGridButton {
                    text: qsTr("Settings")
                    onClicked: pageStack.push(torrentSettingsPage)

                    Component {
                        id: torrentSettingsPage
                        TorrentSettingsPage { }
                    }
                }
            }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }

            DetailItem {
                label: qsTr("Total size")
                value: Format.formatFileSize(model.totalSize)
            }

            DetailItem {
                label: qsTr("Downloaded")
                value: qsTr("%1 of %2").arg(Format.formatFileSize(model.sizeWhenDone - model.leftUntilDone)).arg(Format.formatFileSize(model.sizeWhenDone))
            }

            DetailItem {
                label: qsTr("Uploaded")
                value: Format.formatFileSize(model.uploadedEver)
            }

            DetailItem {
                label: qsTr("Ratio")
                value: qsTr("%L1").arg(Math.ceil(model.uploadRatio * 100) / 100)
            }

            DetailItem {
                label: qsTr("Last activity")
                value: "%1, %2".arg(Format.formatDate(model.activityDate, Format.DurationElapsed)).arg(Format.formatDate(model.activityDate, Format.Timepoint))
            }

            DetailItem {
                label: qsTr("Added")
                value: Format.formatDate(model.addedDate, Format.Timepoint)
            }

            DetailItem {
                label: qsTr("Location")
                value: model.downloadDir
            }

            Item {
                height: hashItem.height + contextMenu.height
                width: parent.width

                DetailItem {
                    id: hashItem
                    label: qsTr("Hash")
                    value: model.hashString
                }

                MouseArea {
                    anchors.fill: parent
                    onPressAndHold: contextMenu.show(parent)
                }

                ContextMenu {
                    id: contextMenu
                    MenuItem {
                        text: qsTr("Copy")
                        onClicked: Clipboard.text = model.hashString
                    }
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
