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

#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <QSslConfiguration>
#include <QUrl>
#include <QQmlParserStatus>

class QAuthenticator;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class AppSettings;
class TorrentModel;

class Transmission : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AppSettings* appSettings READ appSettings WRITE setAppSettings)
    Q_PROPERTY(TorrentModel* torrentModel READ torrentModel WRITE setTorrentModel)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)
public:
    enum Error {
        NoError,
        AuthenticationError,
        ConnectionError,
        RpcVersionError,
        SslHandshakeFailedError,
        TimeoutError
    };

    Transmission();
    void classBegin();
    void componentComplete();

    AppSettings* appSettings() const;
    TorrentModel* torrentModel() const;
    int error() const;

    void setAppSettings(AppSettings *appSettings);
    void setTorrentModel(TorrentModel *torrentModel);

    Q_INVOKABLE bool isLocal() const;

    Q_INVOKABLE void addTorrent(const QString &link, const QString &downloadDirectoryPath, bool paused);
    Q_INVOKABLE void changeServerSettings(const QString &key, const QVariant &value);
    Q_INVOKABLE void changeTorrent(int id, const QString &key, const QVariant &value);
    Q_INVOKABLE void removeTorrent(int id, bool deleteLocalData);
    Q_INVOKABLE void stopTorrent(int id);
    Q_INVOKABLE void startTorrent(int id);
    Q_INVOKABLE void verifyTorrent(int id);

    Q_INVOKABLE void updateAccount();
private:
    void checkRpcVersion();
    void getData();

    void beginGettingServerSettings();
    void endGettingServerSettings();

    void beginGettingModelData();
    void endGettingModelData();

    void beginGettingServerStats();
    void endGettingServerStats();

    void timeoutTimer(const QNetworkReply *reply);
    bool checkSessionId(const QNetworkReply *reply);

    void checkError(const QNetworkReply *reply);
    void setError(int error);

    QNetworkReply* rpcPost(const QByteArray &data);

    void authenticate(const QNetworkReply *reply, QAuthenticator *authenticator);
private:
    QNetworkAccessManager *m_network;
    bool m_authenticationRequested;

    QTimer *m_updateTimer;

    AppSettings *m_appSettings;
    TorrentModel *m_torrentModel;
    int m_error;

    QByteArray m_sessionId;

    QString m_currentAccount;
    QUrl m_serverUrl;
    int m_timeout;
    bool m_authentication;
    QString m_username;
    QString m_password;
    bool m_https;

    QSslConfiguration m_sslConfiguration;
signals:
    void errorChanged();
};

#endif // TRANSMISSION_H
