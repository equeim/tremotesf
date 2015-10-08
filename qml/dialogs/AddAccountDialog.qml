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

Dialog {
    property alias name: editItem.name

    allowedOrientations: Orientation.All
    canAccept: {
        if (editItem.name &&
                editItem.address &&
                editItem.port &&
                editItem.apiPath &&
                editItem.updateInterval
                && editItem.timeout) {
            if (editItem.authentication) {
                if (editItem.username && editItem.password)
                    return true
                return false
            }
            return true
        }
        return false
    }

    onAccepted: {
        root.appSettings.addAccount(editItem.name,
                               editItem.address,
                               editItem.port,
                               editItem.apiPath,
                               editItem.https,
                               editItem.localCertificate,
                               editItem.authentication,
                               editItem.username,
                               editItem.password,
                               editItem.updateInterval,
                               editItem.timeout)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            width: parent.width

            DialogHeader {
                id: header
                title: qsTr("Add account")
            }

            EditAccountItem {
                id: editItem
                port: "9091"
                apiPath: "/transmission/rpc"
                updateInterval: "5"
                timeout: "10"
            }
        }

        VerticalScrollDecorator { }
    }
}
