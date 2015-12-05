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
    property var proxyTorrentModel

    Connections {
        target: proxyTorrentModel
        onFilterModeChanged: pageStack.pop()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Filter")
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.AllMode
                text: qsTr("Show all")
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.ActiveMode
                text: qsTr("Active")
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.DownloadingMode
                text: qsTr("Downloading")
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.SeedingMode
                text: qsTr("Seeding")
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.StoppedMode
                text: qsTr("Stopped")
            }

            FilterTorrentsListItem {
                filterMode: Tremotesf.ProxyTorrentModel.FinishedMode
                text: qsTr("Finished")
            }
        }

        VerticalScrollDecorator { }
    }
}
