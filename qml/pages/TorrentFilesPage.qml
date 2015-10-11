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
        if (!root.fileModel.isActive)
            root.torrentModel.loadFileModel(root.proxyTorrentModel.getSourceIndex(model.index))
    }

    Connections {
        target: root.torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
        }
    }

    VisualDataModel {
        id: delegateModel
        delegate: ListItem {
            contentHeight: labelColumn.height + 2 * Theme.paddingMedium
            menu: contextMenu

            onClicked: {
                if (model.isDirectory)
                    delegateModel.rootIndex = delegateModel.modelIndex(model.index)
                else
                    wantedSwitch.toggle()
            }

            Image {
                id: icon

                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                asynchronous: true
                source: model.isDirectory ? "image://theme/icon-m-folder"
                                          : "image://theme/icon-m-other"
            }

            Column {
                id: labelColumn

                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    right: wantedSwitch.left
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    id: label
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    maximumLineCount: 3
                    text: model.name
                    width: parent.width
                    wrapMode: Text.WordWrap
                }

                Label {
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: qsTr("%1 of %2 (%3%)").arg(root.utils.formatByteSize(model.bytesCompleted))
                    .arg(root.utils.formatByteSize(model.length))
                    .arg(model.progress)
                    truncationMode: TruncationMode.Fade
                    width: parent.width
                }
            }

            Switch {
                id: wantedSwitch

                function toggle() {
                    if (model.wantedStatus === TorrentFileModel.AllWanted)
                        model.wantedStatus = TorrentFileModel.NoWanted
                    else
                        model.wantedStatus = TorrentFileModel.AllWanted
                }

                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin - Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                automaticCheck: false
                checked: model.wantedStatus !== TorrentFileModel.NoWanted
                opacity: model.wantedStatus === TorrentFileModel.SomeWanted ? 0.6 : 1

                onClicked: toggle()
            }

            Component {
                id: contextMenu

                ContextMenu {
                    MenuItem {
                        text: qsTr("High priority")
                        font.weight: model.priority === TorrentFileModel.HighPriority ? Font.Bold : Font.Normal

                        onClicked: model.priority = TorrentFileModel.HighPriority
                    }

                    MenuItem {
                        text: qsTr("Normal")
                        font.weight: model.priority === TorrentFileModel.NormalPriority ? Font.Bold : Font.Normal

                        onClicked: model.priority = TorrentFileModel.NormalPriority
                    }

                    MenuItem {
                        text: qsTr("Low")
                        font.weight: model.priority === TorrentFileModel.LowPriority ? Font.Bold : Font.Normal

                        onClicked: model.priority = TorrentFileModel.LowPriority
                    }

                    MenuItem {
                        text: qsTr("Mixed")
                        font.weight: Font.Bold
                        visible: model.priority === TorrentFileModel.MixedPriority
                    }
                }
            }
        }
        model: root.proxyFileModel
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
                visible: delegateModel.rootIndex !== delegateModel.parentModelIndex()
                onClicked: delegateModel.rootIndex = delegateModel.parentModelIndex()
            }
        }
        model: delegateModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Download all files")
                onClicked: root.fileModel.setAllWanted(true)
            }
            MenuItem {
                text: qsTr("Ignore all files")
                onClicked: root.fileModel.setAllWanted(false)
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
