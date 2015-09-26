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

#include "transmission.h"
#include "transmission.moc"

#include <QAuthenticator>
#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTimer>
#include <QSsl>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QVariantMap>

#include "appsettings.h"
#include "torrentmodel.h"

static const QByteArray ModelDataRequest =
        "{\n"
        "    \"arguments\": {\n"
        "        \"fields\": [\n"
        "            \"activityDate\",\n"
        "            \"addedDate\",\n"
        "            \"bandwidthPriority\",\n"
        "            \"downloadDir\",\n"
        "            \"downloadLimit\",\n"
        "            \"downloadLimited\",\n"
        "            \"errorString\",\n"
        "            \"eta\",\n"
        "            \"files\",\n"
        "            \"fileStats\",\n"
        "            \"hashString\",\n"
        "            \"honorsSessionLimits\",\n"
        "            \"id\",\n"
        "            \"leftUntilDone\",\n"
        "            \"name\",\n"
        "            \"peer-limit\",\n"
        "            \"peers\",\n"
        "            \"peersConnected\",\n"
        "            \"peersGettingFromUs\",\n"
        "            \"peersSendingToUs\",\n"
        "            \"percentDone\",\n"
        "            \"rateDownload\",\n"
        "            \"rateUpload\",\n"
        "            \"recheckProgress\",\n"
        "            \"seedRatioLimit\",\n"
        "            \"seedRatioMode\",\n"
        "            \"sizeWhenDone\",\n"
        "            \"status\",\n"
        "            \"totalSize\",\n"
        "            \"trackerStats\",\n"
        "            \"uploadedEver\",\n"
        "            \"uploadLimit\",\n"
        "            \"uploadLimited\",\n"
        "            \"uploadRatio\"\n"
        "        ]\n"
        "    },\n"
        "    \"method\": \"torrent-get\"\n"
        "}";

static const QByteArray ServerSettingsRequest =
        "{\n"
        "    \"method\": \"session-get\"\n"
        "}";

static const QByteArray ServerStatsRequest =
        "{\n"
        "    \"method\": \"session-stats\"\n"
        "}";

static const QByteArray SessionHeader = "X-Transmission-Session-Id";

Transmission::Transmission()
{
    m_network = new QNetworkAccessManager(this);
    connect(m_network, &QNetworkAccessManager::authenticationRequired, this, &Transmission::authenticate);

    m_authenticationRequested = false;
    m_error = ConnectionError;

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, &QTimer::timeout, this, &Transmission::getData);
}

void Transmission::classBegin()
{

}

void Transmission::componentComplete()
{
    connect(m_appSettings, &AppSettings::currentAccountChanged, this, &Transmission::updateAccount);
    updateAccount();
}

AppSettings* Transmission::appSettings() const
{
    return m_appSettings;
}

TorrentModel* Transmission::torrentModel() const
{
    return m_torrentModel;
}

int Transmission::error() const
{
    return m_error;
}

void Transmission::setAppSettings(AppSettings *appSettings)
{
    m_appSettings = appSettings;
}

void Transmission::setTorrentModel(TorrentModel *torrentModel)
{
    m_torrentModel = torrentModel;
}

bool Transmission::isLocal() const
{
    const QString host = m_serverUrl.host();
    if (host == "localhost" || QNetworkInterface::allAddresses().contains(QHostAddress(host)))
        return true;
    return false;
}

void Transmission::addTorrent(const QString &link, const QString &downloadDirectoryPath, bool paused)
{
    QVariantMap request;
    request.insert("method", "torrent-add");
    QVariantMap arguments;

    if (QUrl::fromUserInput(link).isLocalFile()) {
        QFile file(link);
        file.open(QIODevice::ReadOnly);
        arguments.insert("metainfo", QString(file.readAll().toBase64()));
    } else {
        arguments.insert("filename", link);
    }

    arguments.insert("download-dir", downloadDirectoryPath);
    arguments.insert("paused", paused);

    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::changeServerSettings(const QString &key, const QVariant &value)
{
    QVariantMap request;
    request.insert("method", "session-set");
    QVariantMap arguments;
    arguments.insert(key, value);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::changeTorrent(int id, const QString &key, const QVariant &value)
{
    QVariantMap request;
    request.insert("method", "torrent-set");
    QVariantMap arguments;
    arguments.insert("ids", id);
    arguments.insert(key, value);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::removeTorrent(int id, bool deleteLocalData)
{
    QVariantMap request;
    request.insert("method", "torrent-remove");
    QVariantMap arguments;
    arguments.insert("ids", id);
    arguments.insert("delete-local-data", deleteLocalData);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::stopTorrent(int id)
{
    QVariantMap request;
    request.insert("method", "torrent-stop");
    QVariantMap arguments;
    arguments.insert("ids", id);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::startTorrent(int id)
{
    QVariantMap request;
    request.insert("method", "torrent-start");
    QVariantMap arguments;
    arguments.insert("ids", id);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::verifyTorrent(int id)
{
    QVariantMap request;
    request.insert("method", "torrent-verify");
    QVariantMap arguments;
    arguments.insert("ids", id);
    request.insert("arguments", arguments);

    QNetworkReply *reply = rpcPost(QJsonDocument::fromVariant(request).toJson());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::updateAccount()
{
    m_currentAccount = m_appSettings->currentAccount();

    if (m_currentAccount.isEmpty())
        return;

    QUrl serverUrl;
    serverUrl.setHost(m_appSettings->accountAddress(m_currentAccount));
    serverUrl.setPath(m_appSettings->accountApiPath(m_currentAccount));
    serverUrl.setPort(m_appSettings->accountPort(m_currentAccount));

    m_https = m_appSettings->accountHttps(m_currentAccount);
    if (m_https) {
        serverUrl.setScheme("https");

        m_sslConfiguration = QSslConfiguration::defaultConfiguration();
        m_sslConfiguration.setPeerVerifyMode(QSslSocket::QueryPeer);

        if (m_appSettings->accountLocalCertificate(m_currentAccount)) {
            QString pemFilePath = QString("%1/%2.pem")
                    .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
                    .arg(m_currentAccount);

            QFile pemFile(pemFilePath);
            if (pemFile.open(QIODevice::ReadOnly)) {
                QByteArray pemFileData = pemFile.readAll();
                m_sslConfiguration.setLocalCertificate(QSslCertificate(pemFileData));
                m_sslConfiguration.setPrivateKey(QSslKey(pemFileData, QSsl::Rsa));
                pemFile.close();
            }
        }
    } else {
        serverUrl.setScheme("http");
    }

    if (m_serverUrl != serverUrl) {
        m_serverUrl = serverUrl;
        m_torrentModel->resetModel();
    }

    m_authentication = m_appSettings->accountAuthentication(m_currentAccount);
    m_username = m_appSettings->accountUsername(m_currentAccount);
    m_password = m_appSettings->accountPassword(m_currentAccount);

    m_timeout = m_appSettings->accountTimeout(m_currentAccount) * 1000;

    m_updateTimer->setInterval(m_appSettings->accountUpdateInterval(m_currentAccount) * 1000);

    connect(m_appSettings, &AppSettings::serverSettingsUpdated, this, &Transmission::checkRpcVersion);
    beginGettingServerSettings();
}

void Transmission::checkRpcVersion()
{
    disconnect(m_appSettings, &AppSettings::serverSettingsUpdated, this, &Transmission::checkRpcVersion);

    if (m_appSettings->rpcVersion() < 14) {
        m_error = RpcVersionError;
        emit errorChanged();
    } else {
        getData();
    }
}

void Transmission::getData()
{
    beginGettingModelData();
    beginGettingServerStats();
}

void Transmission::beginGettingModelData()
{
    QNetworkReply *reply = rpcPost(ModelDataRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGettingModelData);
    timeoutTimer(reply);
}

void Transmission::endGettingModelData()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!checkSessionId(reply)) {
        reply->deleteLater();
        return beginGettingModelData();
    }
    if (!checkError(reply))
        return reply->deleteLater();

    m_torrentModel->beginUpdateModel(reply->readAll());
    reply->deleteLater();
    m_updateTimer->start();
}

void Transmission::beginGettingServerSettings()
{
    QNetworkReply *reply = rpcPost(ServerSettingsRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGettingServerSettings);
    timeoutTimer(reply);
}

void Transmission::endGettingServerSettings()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!checkSessionId(reply)) {
        reply->deleteLater();
        return beginGettingServerSettings();
    }
    if (!checkError(reply))
        return reply->deleteLater();

    m_appSettings->beginUpdateServerSettings(reply->readAll());
    reply->deleteLater();
}

void Transmission::beginGettingServerStats()
{
    QNetworkReply *reply = rpcPost(ServerStatsRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGettingServerStats);
    timeoutTimer(reply);
}

void Transmission::endGettingServerStats()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (!checkSessionId(reply)) {
        reply->deleteLater();
        return beginGettingServerStats();
    }
    if (!checkError(reply))
        return reply->deleteLater();

    m_appSettings->beginUpdateServerStats(reply->readAll());
    reply->deleteLater();
}

void Transmission::timeoutTimer(const QNetworkReply *reply)
{
    QTimer *timer = new QTimer;
    timer->setInterval(m_timeout);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    QObject::connect(timer, &QTimer::timeout, timer, &QObject::deleteLater);
    timer->start();
}


bool Transmission::checkSessionId(const QNetworkReply *reply)
{   
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 409 &&
            reply->hasRawHeader(SessionHeader)) {
        m_sessionId = reply->rawHeader(SessionHeader);
        return false;
    }
    return true;
}

bool Transmission::checkError(const QNetworkReply *reply)
{
    switch (reply->error()) {
    case QNetworkReply::NoError:
        if (m_error != NoError) {
            m_error = NoError;
            emit errorChanged();
        }
        return true;
    case QNetworkReply::AuthenticationRequiredError:
        qWarning() << "authentication error";
        m_authenticationRequested = false;
        if (m_error != AuthenticationError) {
            m_error = AuthenticationError;
            emit errorChanged();
        }
        break;
    default:
        qWarning() << reply->errorString();
        if (m_error != ConnectionError) {
            m_error = ConnectionError;
            emit errorChanged();
        }
    }

    return false;
}

QNetworkReply *Transmission::rpcPost(const QByteArray &data)
{
    QNetworkRequest request(m_serverUrl);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(SessionHeader, m_sessionId);

    if(m_https)
        request.setSslConfiguration(m_sslConfiguration);

    return m_network->post(request, data);
}

void Transmission::authenticate(const QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);

    if (!m_authenticationRequested) {
        if (m_authentication) {
            authenticator->setUser(m_username);
            authenticator->setPassword(m_password);
        }
        m_authenticationRequested = true;
    }
}
