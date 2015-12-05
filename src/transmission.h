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

#ifndef TREMOTESF_TRANSMISSION_H
#define TREMOTESF_TRANSMISSION_H

#include <QSslConfiguration>
#include <QUrl>
#include <QQmlParserStatus>

class QAuthenticator;
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

namespace Tremotesf
{

class AppSettings;
class TorrentModel;

class Transmission : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(Tremotesf::AppSettings* appSettings READ appSettings WRITE setAppSettings)
    Q_PROPERTY(Tremotesf::TorrentModel* torrentModel READ torrentModel WRITE setTorrentModel)
    Q_PROPERTY(bool accountConfigured READ isAccountConfigured NOTIFY accountConfiguredChanged)
    Q_PROPERTY(bool accountConnected READ isAccountConnected NOTIFY accountConnectedChanged)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
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
    void setAppSettings(AppSettings *settings);

    TorrentModel* torrentModel() const;
    void setTorrentModel(TorrentModel *model);

    bool isAccountConfigured() const;
    bool isAccountConnected() const;
    int error() const;
    QString errorString() const;

    Q_INVOKABLE bool isLocal() const;

    Q_INVOKABLE void addTorrent(const QString &link, const QString &downloadDirectoryPath, bool paused);
    Q_INVOKABLE void changeServerSettings(const QString &key, const QVariant &value);
    Q_INVOKABLE void changeTorrent(int id, const QString &key, const QVariant &value);
    Q_INVOKABLE void removeTorrent(int id, bool deleteLocalData);
    void stopTorrent(int id);
    void startTorrent(int id);
    void verifyTorrent(int id);

    Q_INVOKABLE void checkRpcVersion();
private:
    void beginCheckingRpcVersion();
    void endCheckingRpcVersion();

    void getData();

    void beginGettingServerSettings();
    void endGettingServerSettings();

    void beginGettingModelData();
    void endGettingModelData();

    void beginGettingServerStats();
    void endGettingServerStats();

    void updateAccount();

    void timeoutTimer(const QNetworkReply *reply);
    bool checkSessionId(const QNetworkReply *reply);

    void checkError(const QNetworkReply *reply);
    void setError(int error);

    QNetworkReply* rpcPost(const QByteArray &data);

    void authenticate(const QNetworkReply *reply, QAuthenticator *authenticator);
private:
    QNetworkAccessManager *m_network;
    bool m_authenticationRequested;
    bool m_accountConfigured;

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
    void accountConfiguredChanged();
    void accountConnectedChanged();
    void errorChanged();
};

}

#endif // TREMOTESF_TRANSMISSION_H
