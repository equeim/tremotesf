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

#include <QGuiApplication>
#include <QQuickView>
#include <QScopedPointer>
#include <sailfishapp.h>

#include "appsettings.h"
#include "proxytorrentmodel.h"
#include "transmission.h"
#include "torrentfilemodel.h"
#include "torrentmodel.h"
#include "torrentpeermodel.h"
#include "torrenttrackermodel.h"
#include "folderlistmodel/qquickfolderlistmodel.h"

#include "dbusproxy.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    new DBusProxy(view.data(), view.data());

    qmlRegisterType<AppSettings>("harbour.tremotesf", 0, 1, "AppSettings");
    qmlRegisterType<Transmission>("harbour.tremotesf", 0, 1, "Transmission");
    qmlRegisterType<TorrentModel>("harbour.tremotesf", 0, 1, "TorrentModel");
    qmlRegisterType<ProxyTorrentModel>("harbour.tremotesf", 0, 1, "ProxyTorrentModel");
    qmlRegisterType<TorrentFileModel>("harbour.tremotesf", 0, 1, "TorrentFileModel");
    qmlRegisterType<TorrentPeerModel>("harbour.tremotesf", 0, 1, "TorrentPeerModel");
    qmlRegisterType<TorrentTrackerModel>("harbour.tremotesf", 0, 1, "TorrentTrackerModel");
    qmlRegisterType<QQuickFolderListModel, 1>("harbour.tremotesf", 0, 1, "FolderListModel");
    qmlRegisterType<QAbstractItemModel>();

    view.data()->setSource(SailfishApp::pathTo("qml/tremotesf.qml"));
    view.data()->show();

    return app.data()->exec();
}
