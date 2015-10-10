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
        if (!root.trackerModel.isActive)
            root.torrentModel.loadTrackerModel(root.proxyTorrentModel.getSourceIndex(model.index))
    }

    Connections {
        target: root.torrentModel
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
                root.transmission.changeTorrent(torrentId, "trackerRemove", [model.id])
                root.trackerModel.removeAtIndex(model.index)
            }

            contentHeight: Theme.itemSizeMedium
            menu: contextMenu
            onClicked: pageStack.push(editTrackerPage)
            ListView.onRemove: animateRemoval()

            ListItemMargin {
                Label {
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: model.host
                    truncationMode: TruncationMode.Fade
                    width: parent.width
                }

                Label {
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                    }
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: Format.formatDate(model.lastAnnounceTime, Format.DurationElapsed)
                }
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
        model: root.trackerModel


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
