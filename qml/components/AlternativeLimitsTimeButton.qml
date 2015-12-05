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

ValueButton {
    property string key
    property int oldTime
    property int timeInMinutes: Tremotesf.AppSettings.serverValue(key)

    function changed() {
        return oldTime !== timeInMinutes
    }

    value: Qt.formatTime(new Date((timeInMinutes + (new Date).getTimezoneOffset()) * 60000), "hh:mm")

    onClicked: pageStack.push(timePickerDialog)
    Component.onCompleted: oldTime = timeInMinutes
    Component.onDestruction: {
        if (changed())
            transmission.changeServerSettings(key, timeInMinutes)
    }

    Component {
        id: timePickerDialog

        TimePickerDialog {
            hourMode: DateTime.TwentyFourHours
            hour: (timeInMinutes - minute) / 60
            minute: timeInMinutes % 60

            onAccepted: {
                value = timeText
                timeInMinutes = (hour * 60) + minute
            }
        }
    }
}
