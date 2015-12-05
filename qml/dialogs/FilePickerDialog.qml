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

Dialog {
    property string filePath

    property alias nameFilters: folderListModel.nameFilters
    property alias showFiles: folderListModel.showFiles

    onAccepted: {
        if (!showFiles)
            filePath = Tremotesf.Utils.urlToPath(folderListModel.folder)
    }

    canAccept: filePath.length !== 0 || !showFiles

    DialogHeader {
        id: header
        title: showFiles ? qsTr("Select file") :
                           qsTr("Select directory")
    }

    SilicaListView {
        id: listView

        anchors {
            top: header.bottom
            bottom: parent.bottom
        }
        width: parent.width

        clip: true
        header: Column {
            width: listView.width

            ParentDirectoryItem {
                visible: folderListModel.folder.toString() !== "file:///"
                onClicked: folderListModel.folder = folderListModel.parentFolder
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
                source: {
                    var iconSource = model.fileIsDir ? "image://theme/icon-m-folder"
                                                     : "image://theme/icon-m-other"

                    if (highlighted)
                        iconSource += "?" + Theme.highlightColor

                    return iconSource
                }
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
                    folderListModel.folder = model.filePath
                } else {
                    filePath = model.filePath
                    accept()
                }
            }
        }
        model: Tremotesf.FolderListModel {
            id: folderListModel
            showDirsFirst: true
        }

        ViewPlaceholder {
            enabled: folderListModel.count === 0
            text: qsTr("No files")
        }

        VerticalScrollDecorator { }
    }
}
