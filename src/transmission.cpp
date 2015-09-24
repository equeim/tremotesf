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

#include "transmission.moc"

#include <QAuthenticator>
#include <QFile>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QVariantMap>

#include "appsettings.h"
#include "torrentmodel.h"

const QByteArray Transmission::ModelDataRequest =
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

const QByteArray Transmission::ServerSettingsRequest =
        "{\n"
        "    \"method\": \"session-get\"\n"
        "}";

const QByteArray Transmission::ServerStatsRequest =
        "{\n"
        "    \"method\": \"session-stats\"\n"
        "}";

Transmission::Transmission()
{
    m_netMan = new QNetworkAccessManager(this);
    connect(m_netMan, &QNetworkAccessManager::authenticationRequired, this, &Transmission::authenticate);
    m_authenticationRequested = false;

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &Transmission::getData);

    m_error = ConnectionError;
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
    if (m_serverUrl.host() == "localhost" || m_serverUrl.host() == "127.0.0.1")
        return true;
    return false;
}

void Transmission::updateAccount()
{
    m_currentAccount = m_appSettings->currentAccount();

    if (m_currentAccount.isEmpty())
        return;

    QUrl serverUrl = QUrl::fromUserInput(m_appSettings->getClientValue(m_currentAccount + "/address").toString()
                                         + ":" + m_appSettings->getClientValue(m_currentAccount + "/port").toString()
                                         + m_appSettings->getClientValue(m_currentAccount + "/apiPath").toString());
    if (m_serverUrl != serverUrl) {
        m_serverUrl = serverUrl;
        m_torrentModel->resetModel();
    }

    m_timeout = m_appSettings->getClientValue(m_currentAccount + "/timeout").toInt() * 1000;
    m_authentication = m_appSettings->getClientValue(m_currentAccount + "/authentication").toBool();
    m_username = m_appSettings->getClientValue(m_currentAccount + "/username").toString();
    m_password = m_appSettings->getClientValue(m_currentAccount + "/password").toString();

    m_updateTimer->stop();
    m_updateTimer->setInterval(m_appSettings->getClientValue(m_currentAccount + "/updateInterval").toInt() * 1000);
    m_updateTimer->start();

    getData();
}

void Transmission::addTorrent(QString link, QString downloadDirectoryPath, bool paused)
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

void Transmission::changeServerSettings(QString key, const QVariant &value)
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

void Transmission::changeTorrent(int id, QString key, const QVariant &value)
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

void Transmission::getData()
{
    beginGetModelData();
    beginGetServerSettings();
    beginGetServerStats();
}

void Transmission::endGetModelData()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        if (!checkSessionId(reply)) {
            reply->deleteLater();
            return beginGetModelData();
        }
        getError(reply);
        reply->deleteLater();
        return;
    }
    if (m_error != NoError) {
        m_error = NoError;
        emit errorChanged();
    }

    const QByteArray &replyData = reply->readAll();
    m_torrentModel->beginUpdateModel(replyData);
    reply->deleteLater();
}

void Transmission::endGetServerSettings()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        if (!checkSessionId(reply)) {
            reply->deleteLater();
            return beginGetServerSettings();
        }
        getError(reply);
        reply->deleteLater();
        return;
    }
    if (m_error != NoError) {
        m_error = NoError;
        emit errorChanged();
    }

    const QByteArray &replyData = reply->readAll();
    m_appSettings->beginUpdateServerSettings(replyData);
    reply->deleteLater();
}

void Transmission::endGetServerStats()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        if (!checkSessionId(reply)) {
            reply->deleteLater();
            return beginGetServerStats();
        }
        getError(reply);
        reply->deleteLater();
        return;
    }
    if (m_error != NoError) {
        m_error = NoError;
        emit errorChanged();
    }

    const QByteArray &replyData = reply->readAll();
    m_appSettings->beginUpdateServerStats(replyData);
    reply->deleteLater();
}

void Transmission::beginGetModelData()
{
    QNetworkReply *reply = rpcPost(ModelDataRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGetModelData);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::beginGetServerSettings()
{
    QNetworkReply *reply = rpcPost(ServerSettingsRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGetServerSettings);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

void Transmission::beginGetServerStats()
{
    QNetworkReply *reply = rpcPost(ServerStatsRequest);
    connect(reply, &QNetworkReply::finished, this, &Transmission::endGetServerStats);
    QTimer::singleShot(m_timeout, reply, SLOT(abort()));
}

bool Transmission::checkSessionId(const QNetworkReply *reply)
{   
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 409 && reply->hasRawHeader("X-Transmission-Session-Id")) {
        m_sessionId = reply->rawHeader("X-Transmission-Session-Id");
        return false;
    }
    return true;
}

void Transmission::getError(const QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
        m_authenticationRequested = false;
        qWarning() << "authentication error";
        if (m_error != AuthenticationError) {
            m_error = AuthenticationError;
            emit errorChanged();
        }
    } else {
        qWarning() << reply->errorString();
        if (m_error != ConnectionError) {
            m_error = ConnectionError;
            emit errorChanged();
        }
    }
}

QNetworkReply *Transmission::rpcPost(const QByteArray &data)
{
    QNetworkRequest request(m_serverUrl);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Transmission-Session-Id", m_sessionId);

    return m_netMan->post(request, data);
}
