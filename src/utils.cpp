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

#include "utils.h"
#include "utils.moc"

#include <notification.h>
#include <QDBusConnection>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QLocale>
#include <QSslCertificate>
#include <QSslKey>

bool Utils::checkLocalCertificate(const QString &filePath)
{
    QFile pemFile(filePath);
    if (pemFile.open(QIODevice::ReadOnly)) {
        QByteArray pemFileData = pemFile.readAll();
        pemFile.close();

        QSslCertificate localCertificate = QSslCertificate(pemFileData);
        if (localCertificate.isNull()) {
            qWarning() << "error loading local certificate";
            return false;
        }

        QSslKey privateKey = QSslKey(pemFileData, QSsl::Rsa);
        if (privateKey.isNull()) {
            qWarning() << "error loading private key";
            return false;
        }

        return true;
    }

    qWarning() << "error loading .pem file: " << pemFile.errorString();

    return false;
}

QString Utils::formatByteSpeed(double rate)
{
    return tr("%1/s").arg(formatByteSize(rate));
}

QString Utils::formatByteSize(double size)
{
    int unit = 0;
    while (size >= 1024.0 && unit < 8) {
        size /= 1024.0;
        unit++;
    }

    QString string;
    if (unit == 0)
        string = QString::number(size);
    else
        string = QLocale::system().toString(size, 'f', 1);

    switch (unit) {
    case 0:
        return tr("%1 B").arg(string);
    case 1:
        return tr("%1 KiB").arg(string);
    case 2:
        return tr("%1 MiB").arg(string);
    case 3:
        return tr("%1 GiB").arg(string);
    case 4:
        return tr("%1 TiB").arg(string);
    case 5:
        return tr("%1 PiB").arg(string);
    case 6:
        return tr("%1 EiB").arg(string);
    case 7:
        return tr("%1 ZiB").arg(string);
    case 8:
        return tr("%1 YiB").arg(string);
    }

    return QString();
}

QString Utils::formatEta(int seconds)
{
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;

    QString etaString;

    if (hours > 0)
        etaString +=  tr("%1 h ").arg(hours);

    if (minutes > 0)
        etaString +=  tr("%1 m ").arg(minutes);

    if (hours == 0 &&
            (seconds > 0 ||
             minutes == 0))
        etaString +=  tr("%1 s").arg(seconds);

    return etaString;
}

float Utils::parseFloat(const QString &string)
{
    bool ok;
    float result;
    result = QLocale::system().toFloat(string, &ok);
    if (ok)
        return result;

    return QLocale::c().toFloat(string);
}

void Utils::publishFinishedNotification(const QString &torrentName)
{
    Notification notification;

    notification.setSummary(tr("Torrent finished"));
    notification.setBody(torrentName);

    if (QGuiApplication::applicationState() != Qt::ApplicationActive) {
        notification.setPreviewSummary(notification.summary());
        notification.setPreviewBody(notification.body());
    }

    notification.setRemoteAction(Notification::remoteAction("default",
                                                            QString(),
                                                            QDBusConnection::sessionBus().baseService(),
                                                            "/",
                                                            "harbour.tremotesf.DBusProxy",
                                                            "activate"));

    notification.publish();
}
