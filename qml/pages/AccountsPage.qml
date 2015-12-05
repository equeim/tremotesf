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
    id: accountsPage

    property string currentAccount: Tremotesf.AppSettings.currentAccount

    Connections {
        target: Tremotesf.AppSettings
        onAccountRemoved: {
            if (Tremotesf.AppSettings.accountsCount === 0)
                pageStack.push("../dialogs/AddAccountDialog.qml")
        }
    }

    SilicaListView {
        id: listView

        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Accounts")
        }
        delegate: ListItem {
            id: accountDelegate

            contentHeight: Theme.itemSizeMedium
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: remorseAction(qsTr("Removing account"), function() {
                            Tremotesf.AppSettings.removeAccount(model.name)
                        })
                    }
                }
            }

            onClicked: pageStack.push(editAccountPage)
            ListView.onRemove: animateRemoval()

            Component {
                id: editAccountPage
                EditAccountPage { }
            }

            Switch {
                id: currentSwitch
                anchors {
                    left: parent.left
                    leftMargin: Theme.horizontalPageMargin - Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                automaticCheck: false
                highlighted: accountDelegate.highlighted
                checked: model.name === currentAccount

                onClicked: {
                    if (!checked)
                        Tremotesf.AppSettings.currentAccount = model.name
                }
            }

            Column {
                anchors {
                    left: currentSwitch.right
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    color: highlighted ? Theme.highlightColor : Theme.primaryColor
                    text: model.name
                    truncationMode: TruncationMode.Fade
                    width: parent.width
                }

                Label {
                    color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    text: model.address
                    truncationMode: TruncationMode.Fade
                    width: parent.width
                }
            }
        }
        model: Tremotesf.AccountModel {
            appSettings: Tremotesf.AppSettings
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Add account...")
                onClicked: pageStack.push("../dialogs/AddAccountDialog.qml")
            }
        }

        ViewPlaceholder {
            enabled: listView.count === 0
            text: qsTr("No accounts")
        }

        VerticalScrollDecorator { }
    }
}
