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
            address: appSettings.accountAddress(accountName),
            isCurrent: currentAccount === accountName,
            name: accountName
        }
    }

    function updateModel() {
        var allCount = appSettings.accounts.length

        for (var i = 0; i < Math.min(allCount, accountModel.count); i++)
            accountModel.set(i, getData(i))

        if (allCount > accountModel.count) {
            for (i = accountModel.count; i < allCount; i++)
                accountModel.append(getData(i))
        } else if (allCount < accountModel.count) {
            for (i = accountModel.count; i > allCount; i--)
                accountModel.remove(i - 1)
        }
    }

    allowedOrientations: Orientation.All

    ListModel {
        id: accountModel
        Component.onCompleted: updateModel()
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Accounts")
        }
        delegate: ListItem {
            function removeAccount() {
                appSettings.removeAccount(model.name)
                if (model.isCurrent) {
                    if (accountModel.count === 1) {
                        var dialog = pageStack.push(addAccountDialog)
                        dialog.accepted.connect(function() {
                            appSettings.currentAccount = dialog.accountName
                            accountModel.append({
                                                    address: dialog.address,
                                                    isCurrent: true,
                                                    name: dialog.accountName
                                                })
                            accountModel.remove(model.index)
                        })
                        return
                    } else {
                        appSettings.currentAccount = appSettings.accounts[0]
                        accountModel.setProperty(0, "isCurrent", true)
                    }
                }
                accountModel.remove(model.index)
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
                        var currentIndex
                        for (var i = 0; i < accountModel.count; i++) {
                            if (accountModel.get(i).isCurrent) {
                                currentIndex = i
                                break
                            }
                        }
                        accountModel.setProperty(currentIndex, "isCurrent", false)
                        accountModel.setProperty(model.index, "isCurrent", true)
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
