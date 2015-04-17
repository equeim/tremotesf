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

#include <QJsonDocument>
#include <QSettings>

#include "torrentmodel.h"

#include "appsettings.h"

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
    if(m_clientSettings->allKeys().isEmpty())
        setDefaultClientSettings();
}

AppSettings::~AppSettings()
{
    m_settingsWorker->wait();
    m_settingsWorker->deleteLater();
    m_statsWorker->wait();
    m_statsWorker->deleteLater();
}

void AppSettings::setDefaultClientSettings()
{
    m_clientSettings->setValue("filterMode", 0);
    m_clientSettings->setValue("sortOrder", Qt::AscendingOrder);
    m_clientSettings->setValue("sortRole", TorrentModel::NameRole);
}

QStringList AppSettings::accounts() const
{
    return m_clientSettings->childGroups();
}

QString AppSettings::currentAccount() const
{
    return m_clientSettings->value("currentAccount").toString();
}

int AppSettings::downloadSpeed() const
{
    return m_downloadSpeed;
}

int AppSettings::uploadSpeed() const
{
    return m_uploadSpeed;
}

void AppSettings::setCurrentAccount(QString name)
{
    m_clientSettings->setValue("currentAccount", name);
    emit currentAccountChanged();
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

void AppSettings::removeAccount(QString name)
{
    m_clientSettings->remove(name);
}

QVariant AppSettings::getClientValue(QString key) const
{
    return m_clientSettings->value(key);
}

void AppSettings::setClientValue(QString key, const QVariant &value)
{
    m_clientSettings->setValue(key, value);
}

QVariant AppSettings::getServerValue(QString key) const
{
    return m_serverSettings.value(key);
}

void AppSettings::endUpdateServerSettings(const QVariantMap &serverSettings)
{
    m_serverSettings = serverSettings;
}

void AppSettings::endUpdateServerStats(int downloadSpeed, int uploadSpeed)
{
    m_downloadSpeed = downloadSpeed;
    m_uploadSpeed = uploadSpeed;
    emit serverStatsUpdated();
}
