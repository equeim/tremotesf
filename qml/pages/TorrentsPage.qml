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
                if (!root.transmission.accountConfigured)
                    return qsTr("No account connected")

                if (root.transmission.error === Transmission.NoError)
                    return "↓ %1  ↑ %2".arg(root.utils.formatByteSpeed(root.appSettings.downloadSpeed))
                    .arg(root.utils.formatByteSpeed(root.appSettings.uploadSpeed))

                return root.transmission.errorString
            }
        }
        delegate: ListItem {
            id: torrentDelegate

            property int torrentId: model.id
            property int torrentStatus: model.status

            function removeTorrent(deleteLocalData) {
                root.transmission.removeTorrent(torrentId, deleteLocalData)
                root.torrentModel.removeAtIndex(root.proxyTorrentModel.getSourceIndex(model.index))
            }

            function remorseItemRemove(deleteLocalData) {
                remorseAction(qsTr("Removing torrent"), function() {
                    removeTorrent(deleteLocalData)
                })
            }

            function startStop() {
                if (torrentStatus === TorrentModel.StoppedStatus) {
                    if (model.percentDone === 100)
                        model.status = TorrentModel.QueuedForSeedingStatus
                    else
                        model.status = TorrentModel.DownloadingStatus
                } else {
                    model.status = TorrentModel.StoppedStatus
                }
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
                            if (torrentStatus === TorrentModel.StoppedStatus)
                                return qsTr("Start")
                            return qsTr("Stop")
                        }
                        onClicked: startStop()
                    }
                    MenuItem {
                        text: qsTr("Verify")
                        onClicked: model.status = TorrentModel.QueuedForCheckingStatus
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
        model: root.proxyTorrentModel

        PullDownMenu {
            id: pullDownMenu
            MenuItem {
                text: qsTr("Accounts")
                onClicked: pageStack.push(accountsPage)

                Component {
                    id: accountsPage
                    AccountsPage { }
                }
            }

            MenuItem {
                enabled: root.transmission.accountConnected
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
                enabled: root.transmission.accountConnected
                text: qsTr("Add torrent...")
                onClicked: pageStack.push(addTorrentDialog)

                Component {
                    id: addTorrentDialog
                    AddTorrentDialog { }
                }
            }
        }

        Button {
            id: reconnectButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Reconnect")
            visible: root.transmission.accountConfigured &&
                     !root.transmission.accountConnected
            y: parent.height / 2 - parent.headerItem.height - parent.contentY - height / 2
            onClicked: root.transmission.checkRpcVersion()
        }

        ViewPlaceholder {
            id: placeholder
            enabled: listView.count === 0 && !reconnectButton.visible
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
