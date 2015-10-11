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

BackgroundItem {
    Image {
        id: icon

        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        rotation: -90
        source: "image://theme/icon-m-shortcut"
        sourceSize.width: Theme.itemSizeMedium / 2
        sourceSize.height: Theme.itemSizeMedium / 2
    }

    Label {
        anchors {
            left: icon.right
            leftMargin: Theme.paddingMedium
            verticalCenter: parent.verticalCenter
        }
        color: highlighted ? Theme.highlightColor : Theme.primaryColor
        text: ".."
    }
}
