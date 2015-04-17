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

#include <QDBusConnection>
#include <QGuiApplication>
#include <QQuickView>
#include <sailfishapp.h>

#include "appsettings.h"
#include "transmission.h"
#include "torrentmodel.h"
#include "proxytorrentmodel.h"
#include "torrentfilemodel.h"
#include "torrentpeermodel.h"
#include "torrenttrackermodel.h"
#include "folderlistmodel/qquickfolderlistmodel.h"

#include "dbusproxy.h"

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    DBusProxy *proxy = new DBusProxy(view);
    QDBusConnection::sessionBus().registerObject("/", proxy, QDBusConnection::ExportAllSlots);

    qmlRegisterType<AppSettings>("harbour.tremotesf", 0, 1, "AppSettings");
    qmlRegisterType<Transmission>("harbour.tremotesf", 0, 1, "Transmission");
    qmlRegisterType<TorrentModel>("harbour.tremotesf", 0, 1, "TorrentModel");
    qmlRegisterType<ProxyTorrentModel>("harbour.tremotesf", 0, 1, "ProxyTorrentModel");
    qmlRegisterType<TorrentFileModel>("harbour.tremotesf", 0, 1, "TorrentFileModel");
    qmlRegisterType<TorrentPeerModel>("harbour.tremotesf", 0, 1, "TorrentPeerModel");
    qmlRegisterType<TorrentTrackerModel>("harbour.tremotesf", 0, 1, "TorrentTrackerModel");
    qmlRegisterType<QQuickFolderListModel,1>("harbour.tremotesf", 0, 1, "FolderListModel");
    qmlRegisterType<QAbstractItemModel>();

    view->setSource(SailfishApp::pathTo("qml/tremotesf.qml"));
    view->show();

    int result = app->exec();

    proxy->deleteLater();
    view->deleteLater();
    app->deleteLater();

    return result;
}
