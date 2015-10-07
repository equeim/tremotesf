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
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        if (!fileModel.isActive)
            torrentModel.loadFileModel(proxyModel.getSourceIndex(model.index))
    }

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
        }
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        header: Column {
            width: listView.width

            PageHeader {
                id: title
                title: qsTr("Files")
            }

            ParentDirectoryItem {
                visible: fileModel.currentDirectoryPath ? true : false
                onClicked: fileModel.loadDirectory(fileModel.parentDirectoryPath)
            }
        }
        delegate: ListItem {
            contentHeight: Math.max(Theme.itemSizeSmall,
                                    labelColumn.height + 2 * Theme.paddingMedium)
            menu: contextMenu

            onClicked: {
                if (model.isDirectory) {
                    fileModel.loadDirectory(model.path)
                } else {
                    if (wantedSwitch.checked) {
                        wantedSwitch.checked = false
                        transmission.changeTorrent(torrentId, "files-unwanted", [model.fileIndex])
                    } else {
                        wantedSwitch.checked = true
                        transmission.changeTorrent(torrentId, "files-wanted", [model.fileIndex])
                    }
                }
            }

            ListItemMargin {
                Image {
                    id: icon

                    anchors.verticalCenter: parent.verticalCenter
                    asynchronous: true
                    source: model.isDirectory ? "image://theme/icon-m-folder"
                                                       : "image://theme/icon-m-other"
                    sourceSize.height: Theme.iconSizeMedium
                    sourceSize.width: Theme.iconSizeMedium
                }

                Column {
                    id: labelColumn

                    anchors {
                        left: icon.right
                        leftMargin: Theme.paddingMedium
                        right: wantedSwitch.left
                    }

                    Label {
                        id: label

                        color: highlighted ? Theme.highlightColor : Theme.primaryColor
                        maximumLineCount: 3
                        text: model.name
                        wrapMode: Text.WordWrap
                    }

                    Label {
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("%1 of %2 (%3%)").arg(Format.formatFileSize(model.bytesCompleted))
                        .arg(Format.formatFileSize(model.length))
                        .arg(Math.floor(model.bytesCompleted*10000/model.length)/100)
                        visible: !model.isDirectory
                    }
                }

                Switch {
                    id: wantedSwitch

                    anchors {
                        right: parent.right
                        rightMargin: -Theme.paddingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    checked: {
                        if (model.isDirectory) {
                            var wantedStatus = fileModel.getDirectoryWantedStatus(model.index)
                            if (wantedStatus === TorrentFileModel.SomeWanted) {
                                opacity = 0.5
                                return true
                            }
                            opacity = 1
                            if (wantedStatus === TorrentFileModel.AllWanted)
                                return true
                            return false
                        }
                        if (model.wanted)
                            return true
                        return false
                    }

                    onClicked: {
                        if (checked) {
                            if (model.isDirectory)
                                transmission.changeTorrent(torrentId, "files-wanted", fileModel.getDirectoryFileIndexes(model.index))
                            else
                                transmission.changeTorrent(torrentId, "files-wanted", [model.fileIndex])
                        }
                        else {
                            if (model.isDirectory)
                                transmission.changeTorrent(torrentId, "files-unwanted", fileModel.getDirectoryFileIndexes(model.index))
                            else
                                transmission.changeTorrent(torrentId, "files-unwanted", [model.fileIndex])
                        }
                    }
                }
            }

            Component {
                id: contextMenu

                ContextMenu {
                    property int priority: {
                        if (model.isDirectory) {
                            return fileModel.getDirectoryPriority(model.index)
                        }
                        return model.priority
                    }

                    MenuItem {
                        text: qsTr("High priority")
                        font.weight: priority === TorrentFileModel.HighPriority ? Font.Bold : Font.Normal

                        onClicked: {
                            if (model.isDirectory)
                                transmission.changeTorrent(torrentId, "priority-high", fileModel.getDirectoryFileIndexes(model.index))
                            else
                                transmission.changeTorrent(torrentId, "priority-high", [model.fileIndex])
                        }
                    }

                    MenuItem {
                        text: qsTr("Normal")
                        font.weight: priority === TorrentFileModel.NormalPriority ? Font.Bold : Font.Normal

                        onClicked: {
                            if (model.isDirectory)
                                transmission.changeTorrent(torrentId, "priority-normal", fileModel.getDirectoryFileIndexes(model.index))
                            else
                                transmission.changeTorrent(torrentId, "priority-normal", [model.fileIndex])
                        }
                    }

                    MenuItem {
                        text: qsTr("Low")
                        font.weight: priority === TorrentFileModel.LowPriority ? Font.Bold : Font.Normal

                        onClicked: {
                            if (model.isDirectory)
                                transmission.changeTorrent(torrentId, "priority-low", fileModel.getDirectoryFileIndexes(model.index))
                            else
                                transmission.changeTorrent(torrentId, "priority-low", [model.fileIndex])
                        }
                    }

                    MenuItem {
                        text: qsTr("Mixed")
                        font.weight: Font.Bold
                        visible: priority === TorrentFileModel.MixedPriority ? true : false
                    }
                }
            }
        }
        model: fileModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Download all files")
                onClicked: transmission.changeTorrent(torrentId, "files-wanted", [])
            }
            MenuItem {
                text: qsTr("Ignore all files")
                onClicked: transmission.changeTorrent(torrentId, "files-unwanted", [])
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
