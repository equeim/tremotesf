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

    property alias nameFilters: folderModel.nameFilters
    property alias showFiles: folderModel.showFiles

    onAccepted: {
        if (!showFiles) {
            path = folderModel.folder
            path = path.slice(7, path.length)
        }
    }

    allowedOrientations: Orientation.All
    canAccept: path || !showFiles

    DialogHeader {
        id: header
        title: showFiles ? qsTr("Select file") :
                           qsTr("Select directory")
    }

    SilicaListView {
        anchors {
            top: header.bottom
            bottom: parent.bottom
        }
        clip: true
        header: Column {
            width: parent.width
            ParentDirectoryItem {
                visible: folderModel.parentFolder.toString().length !== 0
                onClicked: folderModel.folder = folderModel.parentFolder
            }
        }
        delegate: BackgroundItem {
            Image {
                id: icon

                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                asynchronous: true
                source: model.fileIsDir ? "image://theme/icon-m-folder"
                                        : "image://theme/icon-m-other"
            }

            Label {
                anchors {
                    left: icon.right
                    leftMargin: Theme.paddingMedium
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }
                text: model.fileName
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
                truncationMode: TruncationMode.Fade
            }

            onClicked: {
                if (model.fileIsDir) {
                    folderModel.folder = model.filePath
                } else {
                    path = model.filePath
                    accept()
                }
            }
        }
        model: FolderListModel {
            id: folderModel
            showDirsFirst: true
        }
        width: parent.width

        VerticalScrollDecorator { }
    }

    ViewPlaceholder {
        enabled: folderModel.count === 0
        text: qsTr("No files")
    }
}
