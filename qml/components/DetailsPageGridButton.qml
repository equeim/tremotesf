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

BackgroundItem {
    property alias text: label.text

    function isFirstOrLastInRow() {
        return Positioner.index % parent.columns === 0 || (Positioner.index + 1) % parent.columns === 0
    }

    height: Theme.itemSizeLarge
    width: {
        if (isFirstOrLastInRow())
            return (parent.width / parent.columns) - parent.spacing / 2
        else
            return (parent.width / parent.columns) - parent.spacing
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        opacity: 0.3
    }

    Label {
        id: label
        anchors.centerIn: parent
        color: highlighted ? Theme.highlightColor : Theme.primaryColor
    }
}
