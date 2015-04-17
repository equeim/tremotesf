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
import "../dialogs"

Page {
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        if (!trackerModel.isActive)
            torrentModel.loadTrackerModel(proxyModel.getSourceIndex(model.index))
    }

    Connections {
        target: torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
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
                trackerModel.removeAtIndex(model.index)
            }

            contentHeight: Theme.itemSizeMedium
            menu: contextMenu
            onClicked: pageStack.push(editTrackerPage)
            ListView.onRemove: animateRemoval()

            Label {
                anchors {
                    top: parent.top
                    topMargin: Theme.paddingMedium
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                }
                color: highlighted ? Theme.highlightColor : Theme.primaryColor
                text: model.host
                width: parent.width
            }

            Label {
                anchors {
                    bottom: parent.bottom
                    bottomMargin: Theme.paddingMedium
                    right: parent.right
                    rightMargin: Theme.paddingMedium
                }
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                text: Format.formatDate(model.lastAnnounceTime, Format.DurationElapsed)
            }

            Component {
                id: contextMenu

                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: remorseAction(qsTr("Removing tracker"), removeTracker)
                    }
                }
            }

            Component {
                id: editTrackerPage
                EditTrackerPage { }
            }
        }
        model: trackerModel


        PullDownMenu {
            MenuItem {
                text: qsTr("Add tracker...")
                onClicked: pageStack.push(addTorrentTrackerDialog)

                Component {
                    id: addTorrentTrackerDialog
                    AddTrackerDialog { }
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
