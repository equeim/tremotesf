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

Page {
    allowedOrientations: Orientation.All

    Connections {
        target: root.proxyTorrentModel
        onSortRoleChanged: pageStack.pop()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Sort by")
            }

            ComboBox {
                label: qsTr("Order")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Ascending")
                        onClicked: root.proxyTorrentModel.sortOrder = Qt.AscendingOrder
                    }

                    MenuItem {
                        text: qsTr("Descending")
                        onClicked: root.proxyTorrentModel.sortOrder = Qt.DescendingOrder
                    }
                }

                Component.onCompleted: {
                    if (root.proxyTorrentModel.sortOrder === Qt.AscendingOrder)
                        currentIndex = 0
                    else
                        currentIndex = 1
                }
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.ActivityDateRole
                text: qsTr("Activity")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.AddedDateRole
                text: qsTr("Added date")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.EtaRole
                text: qsTr("Eta")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.NameRole
                text: qsTr("Name")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.PercentDoneRole
                text: qsTr("Progress")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.UploadRatioRole
                text: qsTr("Ratio")
            }

            SortTorrentsListItem {
                sortRole: TorrentModel.TotalSizeRole
                text: qsTr("Size")
            }
        }

        VerticalScrollDecorator { }
    }
}
