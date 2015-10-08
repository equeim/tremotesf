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

CommonTextField {
    property bool isFloat: false
    property string key

    inputMethodHints: Qt.ImhDigitsOnly
    text: {
        if (isFloat)
            return qsTr("%L1").arg(Math.ceil(root.appSettings.serverValue(key) * 100) / 100)
        else
            return root.appSettings.serverValue(key)
    }

    Component.onDestruction: {
        if (changed) {
            if (isFloat)
                root.transmission.changeServerSettings(key,
                                                       parseFloat(text.replace(",", ".")))
            else
                root.transmission.changeServerSettings(key, parseInt(text));
        }
    }
}
