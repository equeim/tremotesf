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

import "dialogs"
import "pages"

ApplicationWindow
{
    id: root

    property alias appSettings: appSettings
    property alias transmission: transmission
    property alias torrentModel: torrentModel
    property alias proxyModel: proxyModel
    property alias fileModel: fileModel
    property alias peerModel: peerModel
    property alias trackerModel: trackerModel

    initialPage: Component { TorrentsPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    Component.onCompleted: {
        if (root.appSettings.accountCount === 0)
            pageStack.push(addAccountDialog, {}, PageStackAction.Immediate)
    }

    Component {
        id: addAccountDialog
        AddAccountDialog { }
    }

    AppSettings {
        id: appSettings
    }

    Transmission {
        id: transmission
        appSettings: root.appSettings
        torrentModel: root.torrentModel
    }

    TorrentModel {
        id: torrentModel
        fileModel: root.fileModel
        peerModel: root.peerModel
        trackerModel: root.trackerModel
        transmission: root.transmission
    }

    ProxyTorrentModel {
        id: proxyModel
        appSettings: root.appSettings
        sourceModel: root.torrentModel
    }

    TorrentFileModel {
        id: fileModel
        transmission: root.transmission
    }

    TorrentPeerModel {
        id: peerModel
    }

    TorrentTrackerModel {
        id: trackerModel
    }
}
