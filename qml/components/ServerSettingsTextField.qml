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

import harbour.tremotesf 0.1 as Tremotesf

CommonTextField {
    property bool isFloat: false
    property string key

    inputMethodHints: Qt.ImhDigitsOnly
    text: {
        if (isFloat)
            return "%L1".arg(Tremotesf.AppSettings.serverValue(key))
        else
            return Tremotesf.AppSettings.serverValue(key)
    }

    Component.onDestruction: {
        if (changed()) {
            if (isFloat)
                transmission.changeServerSettings(key, Tremotesf.Utils.parseFloat(text))
            else
                transmission.changeServerSettings(key, parseInt(text));
        }
    }
}
