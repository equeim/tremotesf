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

import "../components"

Page {
    property int torrentIndex

    Component.onCompleted: {
        if (!fileModel.active)
            torrentModel.loadFileModel(torrentIndex)
    }

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(pageStack.previousPage(pageStack.previousPage()), PageStackAction.Immediate)
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
                visible: delegateModel.rootIndex !== delegateModel.parentModelIndex()
                onClicked: delegateModel.rootIndex = delegateModel.parentModelIndex()
            }
        }
        model: VisualDataModel {
            id: delegateModel

            delegate: ListItem {
                id: fileDelegate

                contentHeight: labelColumn.height + 2 * Theme.paddingMedium
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr("High priority")
                            font.weight: model.priority === Tremotesf.TorrentFileModel.HighPriority ? Font.Bold : Font.Normal

                            onClicked: model.priority = Tremotesf.TorrentFileModel.HighPriority
                        }

                        MenuItem {
                            text: qsTr("Normal")
                            font.weight: model.priority === Tremotesf.TorrentFileModel.NormalPriority ? Font.Bold : Font.Normal

                            onClicked: model.priority = Tremotesf.TorrentFileModel.NormalPriority
                        }

                        MenuItem {
                            text: qsTr("Low")
                            font.weight: model.priority === Tremotesf.TorrentFileModel.LowPriority ? Font.Bold : Font.Normal

                            onClicked: model.priority = Tremotesf.TorrentFileModel.LowPriority
                        }

                        MenuItem {
                            text: qsTr("Mixed")
                            font.weight: Font.Bold
                            visible: model.priority === Tremotesf.TorrentFileModel.MixedPriority
                        }
                    }
                }

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
                    source: {
                        var iconSource = model.isDirectory ? "image://theme/icon-m-folder"
                                                           : "image://theme/icon-m-other"

                        if (highlighted)
                            iconSource += "?" + Theme.highlightColor

                        return iconSource
                    }
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
                        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("%1 of %2 (%3%)").arg(Tremotesf.Utils.formatByteSize(model.bytesCompleted))
                        .arg(Tremotesf.Utils.formatByteSize(model.length))
                        .arg(model.progress)
                        truncationMode: TruncationMode.Fade
                        width: parent.width
                    }
                }

                Switch {
                    id: wantedSwitch

                    function toggle() {
                        if (model.wantedStatus === Tremotesf.TorrentFileModel.AllWanted)
                            model.wantedStatus = Tremotesf.TorrentFileModel.NoWanted
                        else
                            model.wantedStatus = Tremotesf.TorrentFileModel.AllWanted
                    }

                    anchors {
                        right: parent.right
                        rightMargin: Theme.horizontalPageMargin - Theme.paddingLarge
                        verticalCenter: parent.verticalCenter
                    }
                    automaticCheck: false
                    checked: model.wantedStatus !== Tremotesf.TorrentFileModel.NoWanted
                    highlighted: fileDelegate.highlighted
                    opacity: model.wantedStatus === Tremotesf.TorrentFileModel.SomeWanted ? 0.6 : 1

                    onClicked: toggle()
                }
            }
            model: Tremotesf.ProxyFileModel {
                sourceModel: fileModel
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Download all files")
                onClicked: fileModel.setAllWanted(true)
            }
            MenuItem {
                text: qsTr("Ignore all files")
                onClicked: fileModel.setAllWanted(false)
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
