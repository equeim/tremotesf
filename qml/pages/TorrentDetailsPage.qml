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

import "../components"

Page {
    function remorsePopupRemove(deleteLocalData) {
        remorsePopup.execute(qsTr("Removing torrent"), function() {
            pageStack.pop()
            removeTorrent(deleteLocalData)
        })
    }

    allowedOrientations: Orientation.All

    Connections {
        target: root.torrentModel
        onTorrentRemoved: {
            if (model.index === -1) {
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
            }
        }
    }

    Component.onDestruction: {
        if (root.fileModel.isActive)
            root.fileModel.resetModel()
        if (root.peerModel.isActive)
            root.peerModel.resetModel()
        if (root.trackerModel.isActive)
            root.trackerModel.resetModel()
    }

    RemorsePopup {
        id: remorsePopup
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
                onClicked: model.status = TorrentModel.QueuedForCheckingStatus
            }
            MenuItem {
                text: torrentStatus === TorrentModel.StoppedStatus ? qsTr("Start") : qsTr("Stop")
                onClicked: startStop()
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Details")
            }

            TorrentDelegateItem { }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }

            Grid {
                property int baseItemWidth: 2 * Theme.itemSizeExtraLarge
                property int itemWidth: {
                    var tmp = (parent.width / columns) - spacing * (columns - 1) / columns
                    if (tmp >= baseItemWidth)
                        return baseItemWidth
                    else
                        return tmp
                }
                property int items: children.length

                anchors.horizontalCenter: parent.horizontalCenter
                columns: {
                    var tmp = Math.round(parent.width / baseItemWidth)
                    if (tmp > items)
                        return items
                    return tmp
                }
                spacing: Theme.paddingMedium

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
                value: root.utils.formatByteSize(model.totalSize)
            }

            DetailItem {
                label: qsTr("Downloaded")
                value: qsTr("%1 of %2").arg(root.utils.formatByteSize(model.sizeWhenDone - model.leftUntilDone)).arg(root.utils.formatByteSize(model.sizeWhenDone))
            }

            DetailItem {
                label: qsTr("Uploaded")
                value: root.utils.formatByteSize(model.uploadedEver)
            }

            DetailItem {
                label: qsTr("Ratio")
                value: qsTr("%L1").arg(Math.ceil(model.uploadRatio * 100) / 100)
            }

            DetailItem {
                label: qsTr("Last activity")
                value: {
                    var tmp = model.activityDate
                    if (isNaN(tmp.getTime()))
                        return qsTr("Never")
                    return "%1, %2".arg(Format.formatDate(tmp, Format.DurationElapsed))
                    .arg(Format.formatDate(tmp, Format.Timepoint))
                }
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
