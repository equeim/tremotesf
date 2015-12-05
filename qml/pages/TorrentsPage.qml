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

import harbour.tremotesf 0.1 as Tremotesf

import "../dialogs"
import "../components"

Page {
    SilicaListView {
        id: listView

        anchors.fill: parent
        header: PageHeader {
            title: "Tremotesf"
            description: {
                if (!transmission.accountConfigured)
                    qsTr("No account connected")

                if (transmission.error === Tremotesf.Transmission.NoError)
                    return "↓ %1  ↑ %2".arg(Tremotesf.Utils.formatByteSpeed(Tremotesf.AppSettings.downloadSpeed))
                .arg(Tremotesf.Utils.formatByteSpeed(Tremotesf.AppSettings.uploadSpeed))

                return transmission.errorString
            }
        }
        delegate: ListItem {
            property int torrentStatus: model.status

            function removeTorrent(deleteLocalData) {
                transmission.removeTorrent(torrentId, deleteLocalData)
                torrentModel.removeAtIndex(proxyTorrentModel.sourceIndex(model.index))
            }

            function remorseItemRemove(deleteLocalData) {
                remorseAction(qsTr("Removing torrent"), function() {
                    removeTorrent(deleteLocalData)
                })
            }

            function startStop() {
                if (torrentStatus === Tremotesf.TorrentModel.StoppedStatus) {
                    if (model.percentDone === 100)
                        model.status = Tremotesf.TorrentModel.QueuedForSeedingStatus
                    else
                        model.status = Tremotesf.TorrentModel.DownloadingStatus
                } else {
                    model.status = Tremotesf.TorrentModel.StoppedStatus
                }
            }

            contentHeight: delegateItem.height
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: {
                            if (torrentStatus === Tremotesf.TorrentModel.StoppedStatus)
                                return qsTr("Start")
                            return qsTr("Stop")
                        }
                        onClicked: startStop()
                    }
                    MenuItem {
                        text: qsTr("Verify")
                        onClicked: model.status = Tremotesf.TorrentModel.QueuedForCheckingStatus
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

            onClicked: pageStack.push(torrentDetailsPage)
            ListView.onRemove: animateRemoval()

            Component {
                id: torrentDetailsPage
                TorrentDetailsPage { }
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            TorrentDelegateItem {
                id: delegateItem
            }
        }
        model: Tremotesf.ProxyTorrentModel {
            id: proxyTorrentModel
            appSettings: Tremotesf.AppSettings
            sourceModel: torrentModel
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push("SettingsPage.qml")
            }

            MenuItem {
                text: qsTr("Filter")
                onClicked: pageStack.push("FilterTorrentsPage.qml", { proxyTorrentModel: listView.model })
            }

            MenuItem {
                text: qsTr("Sort by")
                onClicked: pageStack.push("SortTorrentsPage.qml", { proxyTorrentModel: listView.model })
            }

            MenuItem {
                enabled: transmission.accountConnected
                text: qsTr("Add torrent...")
                onClicked: pageStack.push("../dialogs/AddTorrentDialog.qml")
            }
        }

        Button {
            id: reconnectButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Reconnect")
            visible: transmission.accountConfigured &&
                     !transmission.accountConnected
            y: parent.height / 2 - parent.headerItem.height - parent.contentY - height / 2
            onClicked: transmission.checkRpcVersion()
        }

        ViewPlaceholder {
            enabled: listView.count === 0 && !reconnectButton.visible
            text: qsTr("No items")
        }

        VerticalScrollDecorator { }
    }
}
