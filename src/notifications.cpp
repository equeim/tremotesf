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

#include "notifications.h"

#include <notification.h>
#include <QDBusConnection>
#include <QGuiApplication>

void Notifications::torrentFinished(const QString &name)
{
    if (QGuiApplication::applicationState() == Qt::ApplicationActive)
            return;

    Notification notification;

    notification.setSummary(QObject::tr("Torrent finished"));
    notification.setBody(name);

    notification.setPreviewSummary(notification.summary());
    notification.setPreviewBody(notification.body());

    notification.setRemoteAction(Notification::remoteAction("default",
                                                            QString(),
                                                            QDBusConnection::sessionBus().baseService(),
                                                            "/",
                                                            "harbour.tremotesf.DBusProxy",
                                                            "activate"));

    notification.publish();
}
