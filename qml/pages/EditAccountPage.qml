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
    property int depth

    function remorsePopupRemove() {
        remorse.execute(qsTr("Removing account"), function() {
            if (accountModel.count === 1)
                pageStack.pop(accountsPage, PageStackAction.Immediate)
            else
                pageStack.pop()
            removeAccount()
        })
    }

    allowedOrientations: Orientation.All
    Component.onCompleted: depth = pageStack.depth

    RemorsePopup {
        id: remorse
    }

    Connections {
        target: pageStack
        onDepthChanged: {
            if (depth === pageStack.depth) {
                if (model.name !== editItem.accountName) {
                    appSettings.removeAccount(model.name)
                    editItem.saveAll()
                    if (model.isCurrent) {
                        appSettings.currentAccount = editItem.accountName
                    }
                    updateModel()
                } else {
                    if (editItem.saveIfChanged()) {
                        accountModel.setProperty(model.index, "address", editItem.address)
                        if (model.isCurrent)
                            transmission.updateAccount()
                    }
                }
            }
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Remove")
                onClicked: remorsePopupRemove()
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

                Component.onCompleted: {
                    accountName = model.name
                    address = appSettings.accountAddress(model.name)
                    port = appSettings.accountPort(model.name)
                    apiPath = appSettings.accountApiPath(model.name)
                    updateInterval = appSettings.accountUpdateInterval(model.name)
                    timeout = appSettings.accountTimeout(model.name)
                    authentication = appSettings.accountAuthentication(model.name)
                    username = appSettings.accountUsername(model.name)
                    password = appSettings.accountPassword(model.name)
                }
            }
        }
    }
}
