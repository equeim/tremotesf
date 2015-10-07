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

Dialog {
    property string path

    allowedOrientations: Orientation.All

    onAccepted: {
        path = folderModel.folder
        path = path.slice(7, path.length)
    }

    DialogHeader {
        id: header
        title: qsTr("Select directory")
    }

    SilicaListView {
        id: listView

        anchors {
            top: header.bottom
            bottom: parent.bottom
        }
        clip: true
        header: Column {
            width: listView.width

            ParentDirectoryItem {
                visible: !!folderModel.parentFolder.toString()
                onClicked: folderModel.folder = folderModel.parentFolder
            }
        }
        delegate: BackgroundItem {
            ListItemMargin {
                Image {
                    id: icon

                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    asynchronous: true
                    source: "image://theme/icon-m-folder"
                    sourceSize.height: Theme.iconSizeMedium
                    sourceSize.width: Theme.iconSizeMedium
                }

                Label {
                    anchors {
                        left: icon.right
                        leftMargin: Theme.paddingMedium
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    text: model.fileName
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    truncationMode: TruncationMode.Fade
                }
            }

            onClicked: folderModel.folder = model.filePath
        }
        model: folderModel
        width: parent.width

        FolderListModel {
            id: folderModel
            folder: path
            showFiles: false
        }

        VerticalScrollDecorator { }
    }

    ViewPlaceholder {
        enabled: listView.count === 0
        text: qsTr("No items")
    }
}
