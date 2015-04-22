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

import "../dialogs"
import "../components"

Page {
    id: torrentsPage

    allowedOrientations: Orientation.All

    SilicaListView {
        id: listView

        anchors.fill: parent
        header: PageHeader {
            title: "Tremotesf"
            description: {
                if (transmission.error !== Transmission.NoError) {
                    if (transmission.error === Transmission.AuthenticationError)
                        return qsTr("Authentication error")
                    if (transmission.error === Transmission.ConnectionError)
                        return qsTr("Connection error")
                }
                return "↓ %1/s  ↑ %2/s".arg(Format.formatFileSize(appSettings.downloadSpeed)).arg(Format.formatFileSize(appSettings.uploadSpeed))
            }
        }
        delegate: ListItem {
            id: torrentDelegate

            property int torrentId: model.id
            property int torrentStatus: model.status

            function removeTorrent(deleteLocalData) {
                transmission.removeTorrent(torrentId, deleteLocalData)
                torrentModel.removeAtIndex(proxyModel.getSourceIndex(model.index))
            }

            function remorseItemRemove(deleteLocalData) {
                remorseAction(qsTr("Removing torrent"), function() {
                    removeTorrent(deleteLocalData)
                })
            }

            contentHeight: delegateItem.height
            menu: contextMenu
            onClicked: pageStack.push(torrentDetailsPage)
            ListView.onRemove: animateRemoval()

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            TorrentDelegateItem {
                id: delegateItem
            }

            Component {
                id: contextMenu
                ContextMenu {
                    MenuItem {
                        text: {
                            if (torrentStatus === 0)
                                return qsTr("Start")
                            return qsTr("Stop")
                        }
                        onClicked: torrentStatus === 0 ? transmission.startTorrent(torrentId) : transmission.stopTorrent(torrentId)
                    }
                    MenuItem {
                        text: qsTr("Verify")
                        onClicked: transmission.verifyTorrent(torrentId)
                    }
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: remorseItemRemove(false)
                    }
                    MenuItem {
                        text: qsTr("Delete files and remove")
                        onClicked: remorseItemRemove(true)
                    }
                }
            }

            Component {
                id: torrentDetailsPage
                TorrentDetailsPage { }
            }
        }
        model: proxyModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Accounts")
                onClicked: pageStack.push(accountsPage)

                Component {
                    id: accountsPage
                    AccountsPage { }
                }
            }

            MenuItem {
                text: qsTr("Server settings")
                onClicked: pageStack.push(serverSettingsPage)

                Component {
                    id: serverSettingsPage
                    ServerSettingsPage { }
                }
            }

            MenuItem {
                text: qsTr("Filter")
                onClicked: pageStack.push(filterTorrentsPage)

                Component {
                    id: filterTorrentsPage
                    FilterTorrentsPage { }
                }
            }

            MenuItem {
                text: qsTr("Sort by")
                onClicked: pageStack.push(sortTorrentsPage)

                Component {
                    id: sortTorrentsPage
                    SortTorrentsPage { }
                }
            }

            MenuItem {
                text: qsTr("Add torrent...")
                onClicked: pageStack.push(addTorrentDialog)

                Component {
                    id: addTorrentDialog
                    AddTorrentDialog { }
                }
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
