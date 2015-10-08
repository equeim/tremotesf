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

    Connections {
        target: root.torrentModel
        onTorrentRemoved: {
            if (model.index === -1)
                pageStack.pop(torrentsPage, PageStackAction.Immediate)
        }
    }

    RemorsePopup {
        id: remorse
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Remove")
                onClicked: remorse.execute(qsTr("Removing tracker"), function() {
                    pageStack.pop()
                    removeTracker()
                })
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Edit tracker")
            }

            CommonTextField {
                inputMethodHints: Qt.ImhUrlCharactersOnly
                label: qsTr("Tracker announce URL")

                Component.onCompleted: {
                    text = model.announce
                    oldText = text
                }
                Component.onDestruction: {
                    if (changed())
                        root.transmission.changeTorrent(torrentId, "trackerReplace", [model.id, text])
                }
            }
        }
    }
}
