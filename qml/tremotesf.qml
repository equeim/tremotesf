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

ApplicationWindow
{
    id: root

    property alias transmission: transmission
    property alias torrentModel: torrentModel
    property alias fileModel: fileModel
    property alias peerModel: peerModel
    property alias trackerModel: trackerModel

    _defaultPageOrientations: Orientation.All
    allowedOrientations: defaultAllowedOrientations

    initialPage: Qt.resolvedUrl("pages/TorrentsPage.qml")
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    Component.onCompleted: {
        if (Tremotesf.AppSettings.accountsCount === 0)
            pageStack.push("dialogs/AddAccountDialog.qml", {}, PageStackAction.Immediate)
    }

    Tremotesf.Transmission {
        id: transmission
        appSettings: Tremotesf.AppSettings
        torrentModel: root.torrentModel
    }

    Tremotesf.TorrentModel {
        id: torrentModel

        fileModel: root.fileModel
        peerModel: root.peerModel
        trackerModel: root.trackerModel
        transmission: root.transmission
    }

    Tremotesf.TorrentFileModel {
        id: fileModel
        transmission: root.transmission
    }

    Tremotesf.TorrentPeerModel {
        id: peerModel
    }

    Tremotesf.TorrentTrackerModel {
        id: trackerModel
    }
}
