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

Item {
    property alias label: textField.label
    property alias text: textField.text

    property var dialogNameFilters
    property bool dialogShowFiles
    property bool isLocal: false

    function changed() {
        return textField.changed()
    }

    height: childrenRect.height
    width: parent.width

    CommonTextField {
        id: textField

        anchors {
            left: parent.left
            right: selectButton.left
        }
        inputMethodHints: Qt.ImhUrlCharactersOnly
    }

    IconButton {
        id: selectButton

        anchors {
            right: parent.right
            rightMargin: Theme.horizontalPageMargin
        }
        enabled: isLocal || root.transmission.isLocal()
        icon.source: "image://theme/icon-m-folder"
        icon.sourceSize.width: Theme.iconSizeMedium
        icon.sourceSize.height: Theme.iconSizeMedium

        onClicked: pageStack.push(filePickerDialog)

        Component {
            id: filePickerDialog

            FilePickerDialog {
                //folder: dialogFolder
                nameFilters: dialogNameFilters
                showFiles: dialogShowFiles
                onAccepted: textField.text = path
            }
        }
    }
}
