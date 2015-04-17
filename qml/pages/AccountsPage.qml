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

import "../dialogs"
import "../components"

Page {
    id: accountsPage

    property string currentAccount: appSettings.currentAccount

    function getData(index) {
        var accountName = appSettings.accounts[index]
        return {
            isCurrent: currentAccount === accountName,
            name: accountName,
            address: appSettings.getClientValue(accountName + "/address")
        }
    }

    function updateModel() {
        var existCount = Math.min(appSettings.accounts.length, accountModel.count)
        for (var i = 0; i < existCount; i++) {
            accountModel.set(i, getData(i))
        }

        if (appSettings.accounts.length > accountModel.count) {
            for (var i = accountModel.count; i < appSettings.accounts.length; i++) {
                accountModel.append(getData(i))
            }
        } else if (appSettings.accounts.length < accountModel.count) {
            for (var i = accountModel.count; i > appSettings.accounts.length; i--) {
                accountModel.remove(i - 1)
            }
        }
    }

    allowedOrientations: Orientation.All

    ListModel {
        id: accountModel

        Component.onCompleted: {
            for (var i = 0; i < appSettings.accounts.length; i++) {
                append(getData(i))
            }
        }
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Accounts")
        }
        delegate: ListItem {
            function setFirstCurrent() {
                appSettings.currentAccount = appSettings.accounts[0]
                updateModel()
            }

            function removeAccount() {
                appSettings.removeAccount(model.name)
                if (model.isCurrent) {
                    if (appSettings.accounts.length > 0) {
                        setFirstCurrent()
                    } else {
                        var dialog = pageStack.push(addAccountDialog)
                        dialog.accepted.connect(setFirstCurrent)
                    }
                } else {
                    updateModel()
                }
            }

            contentHeight: Theme.itemSizeMedium
            menu: contextMenu
            ListView.onRemove: animateRemoval()
            onClicked: pageStack.push(editAccountPage)

            Component {
                id: editAccountPage
                EditAccountPage { }
            }

            Switch {
                id: currentSwitch
                anchors.verticalCenter: parent.verticalCenter
                automaticCheck: false
                checked: model.isCurrent

                onClicked: {
                    if (!checked) {
                        appSettings.currentAccount = model.name
                        updateModel()
                    }
                }
            }

            Column {
                anchors {
                    top: parent.top
                    topMargin: Theme.paddingMedium
                    left: currentSwitch.right
                    right: parent.right
                }

                Label {
                    id: nameLabel
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: model.name
                    truncationMode: TruncationMode.Fade
                }

                Label {
                    id: addressLabel
                    color: Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: model.address
                    truncationMode: TruncationMode.Fade
                }
            }

            Component {
                id: contextMenu

                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: remorseAction(qsTr("Removing account"), removeAccount)
                    }
                }
            }
        }
        model: accountModel

        PullDownMenu {
            MenuItem {
                text: qsTr("Add account...")
                onClicked: {
                    var dialog = pageStack.push(addAccountDialog)
                    dialog.accepted.connect(updateModel)
                }
            }
        }
    }

    VerticalScrollDecorator { }
}
