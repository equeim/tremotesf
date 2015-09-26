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

#include "appsettings.h"
#include "appsettings.moc"

#include <QJsonDocument>
#include <QSettings>

#include "proxytorrentmodel.h"
#include "torrentmodel.h"

ServerStatsWorker::ServerStatsWorker()
{

}

void ServerStatsWorker::setReplyData(const QByteArray &replyData)
{
    m_replyData = replyData;
}

void ServerStatsWorker::run() Q_DECL_OVERRIDE
{
    QVariantMap serverStats = QJsonDocument::fromJson(m_replyData).toVariant().toMap().value("arguments").toMap();;
    int downloadSpeed = serverStats.value("downloadSpeed").toInt();
    int uploadSpeed = serverStats.value("uploadSpeed").toInt();
    emit dataParsed(downloadSpeed, uploadSpeed);
}

ServerSettingsWorker::ServerSettingsWorker()
{

}

void ServerSettingsWorker::setReplyData(const QByteArray &replyData)
{
    m_replyData = replyData;
}

void ServerSettingsWorker::run() Q_DECL_OVERRIDE
{
    QVariantMap serverSettings = QJsonDocument::fromJson(m_replyData).toVariant().toMap().value("arguments").toMap();
    emit dataParsed(serverSettings);
}

AppSettings::AppSettings()
{
    m_settingsWorker = new ServerSettingsWorker;
    connect(m_settingsWorker, &ServerSettingsWorker::dataParsed, this, &AppSettings::endUpdateServerSettings);
    m_statsWorker = new ServerStatsWorker;
    connect(m_statsWorker, &ServerStatsWorker::dataParsed, this, &AppSettings::endUpdateServerStats);

    m_clientSettings = new QSettings(this);
    m_clientSettings->setIniCodec("UTF-8");
    checkClientSettings();
}

AppSettings::~AppSettings()
{
    m_settingsWorker->wait();
    m_settingsWorker->deleteLater();
    m_statsWorker->wait();
    m_statsWorker->deleteLater();
}

QStringList AppSettings::accounts() const
{
    return m_clientSettings->childGroups();
}

QString AppSettings::currentAccount() const
{
    return m_clientSettings->value("currentAccount").toString();
}

void AppSettings::setCurrentAccount(QString name)
{
    m_clientSettings->setValue("currentAccount", name);
    emit currentAccountChanged();
}

void AppSettings::removeAccount(QString name)
{
    m_clientSettings->remove(name);
}

void AppSettings::checkClientSettings()
{
    if (!m_clientSettings->value("fileterMode").isValid())
        m_clientSettings->setValue("filterMode", ProxyTorrentModel::AllMode);

    if (!m_clientSettings->value("sortOrder").isValid())
        m_clientSettings->setValue("sortOrder", Qt::AscendingOrder);

    if (!m_clientSettings->value("sortRole").isValid())
        m_clientSettings->setValue("sortRole", TorrentModel::NameRole);
}

QString AppSettings::accountAddress(const QString &account) const
{
    return m_clientSettings->value(account + "/address").toString();
}

void AppSettings::setAccountAddress(const QString &account, const QString &address)
{
    m_clientSettings->setValue(account + "/address", address);
}

QString AppSettings::accountApiPath(const QString &account) const
{
    return m_clientSettings->value(account + "/apiPath").toString();
}

void AppSettings::setAccountApiPath(const QString &account, const QString &apiPath)
{
    m_clientSettings->setValue(account + "/apiPath", apiPath);
}

bool AppSettings::accountAuthentication(const QString &account) const
{
    return m_clientSettings->value(account + "/authentication").toBool();
}

void AppSettings::setAccountAuthentication(const QString &account, bool authentication)
{
    m_clientSettings->setValue(account + "/authentication", authentication);
}

bool AppSettings::accountHttps(const QString &account) const
{
    return m_clientSettings->value(account + "/https").toBool();
}

void AppSettings::setAccountHttps(const QString &account, bool https)
{
    m_clientSettings->setValue(account + "/https", https);
}

bool AppSettings::accountLocalCertificate(const QString &account) const
{
    return m_clientSettings->value(account + "/localCertificate").toBool();
}

void AppSettings::setAccountLocalCertificate(const QString &account, bool localCertificate)
{
    m_clientSettings->setValue(account + "/localCertificate", localCertificate);
}

QString AppSettings::accountPassword(const QString &account) const
{
    return m_clientSettings->value(account + "/password").toString();
}

void AppSettings::setAccountPassword(const QString &account, const QString &password)
{
    m_clientSettings->setValue(account + "/password", password);
}

int AppSettings::accountPort(const QString &account) const
{
    return m_clientSettings->value(account + "/port").toInt();
}

void AppSettings::setAccountPort(const QString &account, int port)
{
    m_clientSettings->setValue(account + "/port", port);
}

int AppSettings::accountTimeout(const QString &account) const
{
    return m_clientSettings->value(account + "/timeout").toInt();
}

void AppSettings::setAccountTimeout(const QString &account, int timeout)
{
    m_clientSettings->setValue(account + "/timeout", timeout);
}

int AppSettings::accountUpdateInterval(const QString &account) const
{
    return m_clientSettings->value(account + "/updateInterval").toInt();
}

void AppSettings::setAccountUpdateInterval(const QString &account, int updateInterval)
{
    m_clientSettings->setValue(account + "/updateInterval", updateInterval);
}

QString AppSettings::accountUsername(const QString &account) const
{
    return m_clientSettings->value(account + "/username").toString();
}

void AppSettings::setAccountUsername(const QString &account, const QString &username)
{
    m_clientSettings->setValue(account + "/username", username);
}

int AppSettings::filterMode() const {
    return m_clientSettings->value("filterMode").toInt();
}

void AppSettings::setFilterMode(int filterMode)
{
    m_clientSettings->setValue("filterMode", filterMode);
}

Qt::SortOrder AppSettings::sortOrder() const
{
    return static_cast<Qt::SortOrder>(m_clientSettings->value("sortOrder").toInt());
}

void AppSettings::setSortOrder(Qt::SortOrder sortOrder)
{
    m_clientSettings->setValue("sortOrder", sortOrder);
}

int AppSettings::sortRole() const
{
    return m_clientSettings->value("sortRole").toInt();
}

void AppSettings::setSortRole(int sortRole)
{
    m_clientSettings->setValue("sortRole", sortRole);
}

int AppSettings::rpcVersion() const
{
    return m_serverSettings.value("rpc-version").toInt();
}

QVariant AppSettings::serverValue(QString key) const
{
    return m_serverSettings.value(key);
}

int AppSettings::downloadSpeed() const
{
    return m_downloadSpeed;
}

int AppSettings::uploadSpeed() const
{
    return m_uploadSpeed;
}

void AppSettings::beginUpdateServerSettings(const QByteArray &replyData)
{
    m_settingsWorker->setReplyData(replyData);
    m_settingsWorker->start(QThread::LowPriority);
}

void AppSettings::beginUpdateServerStats(const QByteArray &replyData)
{
    m_statsWorker->setReplyData(replyData);
    m_statsWorker->start(QThread::LowPriority);
}

void AppSettings::endUpdateServerSettings(const QVariantMap &serverSettings)
{
    m_serverSettings = serverSettings;
    emit serverSettingsUpdated();
}

void AppSettings::endUpdateServerStats(int downloadSpeed, int uploadSpeed)
{
    m_downloadSpeed = downloadSpeed;
    m_uploadSpeed = uploadSpeed;
    emit serverStatsUpdated();
}
