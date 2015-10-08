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
    id: editAccountPage

    property bool removing: false

    allowedOrientations: Orientation.All

    Component.onDestruction: {
        if (!removing)
            root.appSettings.setAccount(editItem.name,
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

    RemorsePopup {
        id: remorsePopup
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Remove")
                onClicked: remorsePopup.execute(qsTr("Removing account"), function() {
                    removing = true
                    if (root.appSettings.accountCount === 1)
                        pageStack.pop(accountsPage, PageStackAction.Immediate)
                    else
                        pageStack.pop()
                    removeAccount()
                })
            }
        }

        Column {
            id: column
            width: parent.width

            PageHeader {
                title: qsTr("Edit account")
            }

            EditAccountItem {
                id: editItem

                name: model.name
                address: model.address
                port: model.port
                apiPath: model.apiPath

                // FIXME
                //https:
                //localCertificate:

                authentication: model.authentication
                username: model.username
                password: model.password
                updateInterval: model.updateInterval
                timeout: model.timeout
            }
        }
    }
}
