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
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        if (!peerModel.isActive)
            root.torrentModel.loadPeerModel(proxyModel.getSourceIndex(model.index))
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
        header: PageHeader {
            title: qsTr("Peers")
        }
        delegate: ListItem {
            contentHeight: leftColumn.height + 2 * Theme.paddingMedium

            ListView.onRemove: animateRemoval()

            ListItemMargin {
                Column {
                    id: leftColumn

                    Label {
                        color: highlighted ? Theme.highlightColor : Theme.primaryColor
                        text: model.address
                        width: parent.width
                    }

                    Label {
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("↓ %1/s").arg(Format.formatFileSize(model.rateToClient))
                    }

                    Label {
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        text: qsTr("↑ %1/s").arg(Format.formatFileSize(model.rateToPeer))
                    }
                }

                Label {
                    anchors.right: parent.right
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: model.progress + "%"
                }
            }
        }
        model: peerModel

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
