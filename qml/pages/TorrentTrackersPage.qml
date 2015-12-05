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
import "../dialogs"

Page {
    id: page

    property int torrentIndex
    property int torrentId

    Component.onCompleted: {
        if (!trackerModel.active)
            torrentModel.loadTrackerModel(torrentIndex)
    }

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(pageStack.previousPage(pageStack.previousPage()), PageStackAction.Immediate)
        }
    }

    SilicaListView {
        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Trackers")
        }
        delegate: ListItem {
            function removeTracker() {
                transmission.changeTorrent(torrentId, "trackerRemove", [model.id])
                trackerModel.removeAtIndex(proxyTrackerModel.sourceIndex(model.index))
            }

            contentHeight: Theme.itemSizeMedium
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: remorseAction(qsTr("Removing tracker"), removeTracker)
                    }
                }
            }

            onClicked: pageStack.push(editTrackerPage)
            ListView.onRemove: animateRemoval()

            Label {
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin
                    top: parent.top
                    topMargin: Theme.paddingMedium
                }
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
                text: model.host
                truncationMode: TruncationMode.Fade
                width: parent.width
            }

            Label {
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    bottom: parent.bottom
                    bottomMargin: Theme.paddingMedium
                }
                color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: Format.formatDate(model.lastAnnounceTime, Format.DurationElapsed)
            }

            Component {
                id: editTrackerPage
                EditTrackerPage { }
            }
        }
        model: Tremotesf.BaseProxyModel {
            id: proxyTrackerModel
            sortRole: Tremotesf.TorrentTrackerModel.AnnounceRole
            sourceModel: trackerModel
        }


        PullDownMenu {
            MenuItem {
                text: qsTr("Add tracker...")
                onClicked: pageStack.push("../dialogs/AddTrackerDialog.qml", { torrentId: page.torrentId })
            }
        }

        VerticalScrollDecorator { }
    }
}
