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
        if (!peerModel.active)
            torrentModel.loadPeerModel(torrentIndex)
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
        header: PageHeader {
            title: qsTr("Peers")
        }
        delegate: ListItem {
            contentHeight: leftColumn.height + 2 * Theme.paddingMedium

            ListView.onRemove: animateRemoval()

            Column {
                id: leftColumn

                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: model.address
                    width: parent.width
                }

                Label {
                    color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: "↓ %1".arg(Tremotesf.Utils.formatByteSpeed(model.rateToClient))
                }

                Label {
                    color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: "↑ %1".arg(Tremotesf.Utils.formatByteSpeed(model.rateToPeer))
                }
            }

            Label {
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    top: parent.top
                    topMargin: Theme.paddingMedium
                }
                color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: model.progress + "%"
            }
        }
        model: Tremotesf.BaseProxyModel {
            sortRole: Tremotesf.TorrentPeerModel.AddressRole
            sourceModel: peerModel
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No peers")
        }

        VerticalScrollDecorator { }
    }
}
