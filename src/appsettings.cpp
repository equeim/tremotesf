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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include "proxytorrentmodel.h"
#include "torrentmodel.h"
#include "utils.h"

namespace Tremotesf
{

void AppSettingsWorker::parseServerSettings(QByteArray replyData)
{
    emit serverSettingsParsed(QJsonDocument::fromJson(replyData).toVariant().toMap().value("arguments").toMap());
}

void AppSettingsWorker::parseServerStats(QByteArray replyData)
{
    QVariantMap serverStats = QJsonDocument::fromJson(replyData).toVariant().toMap().value("arguments").toMap();
    emit serverStatsParsed(serverStats.value("downloadSpeed").toInt(),
                           serverStats.value("uploadSpeed").toInt());
}

const QString AppSettings::CertificatesDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

AppSettings::AppSettings()
    : m_clientSettings(new QSettings(this)),
      m_workerThread(new QThread(this))
{
    AppSettingsWorker *worker = new AppSettingsWorker;
    connect(this, &AppSettings::requestUpdateServerSettings, worker, &AppSettingsWorker::parseServerSettings);
    connect(this, &AppSettings::requestUpdateServerStats, worker, &AppSettingsWorker::parseServerStats);
    connect(worker, &AppSettingsWorker::serverSettingsParsed, this, &AppSettings::endUpdateServerSettings);
    connect(worker, &AppSettingsWorker::serverStatsParsed, this, &AppSettings::endUpdateServerStats);

    connect(m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);

    m_clientSettings->setIniCodec("UTF-8");
    checkClientSettings();

    checkLocalCertificates();
}

AppSettings::~AppSettings()
{
    m_workerThread->quit();
    m_workerThread->wait();
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

void AppSettings::checkLocalCertificates()
{
    QFileInfoList certificates = QDir(CertificatesDirectory).entryInfoList(QStringList() << "*.pem");

    QStringList existingAccounts = accounts();

    for (QFileInfoList::const_iterator iterator = certificates.cbegin(), cend = certificates.cend(); iterator != cend; iterator++) {
        if (existingAccounts.contains(iterator->completeBaseName()))
            if (!Utils::checkLocalCertificate(iterator->filePath()))
                QFile::remove(iterator->filePath());
    }
}

int AppSettings::accountsCount() const
{
    return m_clientSettings->childGroups().size();
}

QStringList AppSettings::accounts() const
{
    return m_clientSettings->childGroups();
}

QString AppSettings::currentAccount() const
{
    return m_clientSettings->value("currentAccount").toString();
}

void AppSettings::setCurrentAccount(const QString &name)
{
    m_clientSettings->setValue("currentAccount", name);
    emit currentAccountChanged();
}

bool AppSettings::isAccountLocalCertificateExists(const QString &account) const
{
    return QFile::exists(CertificatesDirectory + QDir::separator() + account + ".pem");
}

void AppSettings::setAccountLocalCertificate(const QString &account, const QString &filePath)
{
    QFile pemFile(CertificatesDirectory + QDir::separator() + account + ".pem");
    if (pemFile.exists())
        pemFile.remove();

    QFile originFile(filePath);
    if (!originFile.copy(pemFile.fileName()))
        qWarning() << "error setting local certificate: " << originFile.errorString();
}

void AppSettings::removeAccountLocalCertificate(const QString &account)
{
    QFile pemFile(CertificatesDirectory + QDir::separator() + account + ".pem");

    if (!pemFile.remove())
        qWarning() << "error removing local certificate: " << pemFile.errorString();
}

QString AppSettings::accountAddress(const QString &account) const
{
    return m_clientSettings->value(account + "/address").toString();
}

QString AppSettings::accountApiPath(const QString &account) const
{
    return m_clientSettings->value(account + "/apiPath").toString();
}

bool AppSettings::accountAuthentication(const QString &account) const
{
    return m_clientSettings->value(account + "/authentication").toBool();
}

bool AppSettings::accountHttps(const QString &account) const
{
    return m_clientSettings->value(account + "/https").toBool();
}

bool AppSettings::accountLocalCertificate(const QString &account) const
{
    return m_clientSettings->value(account + "/localCertificate").toBool();
}

QString AppSettings::accountPassword(const QString &account) const
{
    return m_clientSettings->value(account + "/password").toString();
}

int AppSettings::accountPort(const QString &account) const
{
    return m_clientSettings->value(account + "/port").toInt();
}

int AppSettings::accountTimeout(const QString &account) const
{
    return m_clientSettings->value(account + "/timeout").toInt();
}

int AppSettings::accountUpdateInterval(const QString &account) const
{
    return m_clientSettings->value(account + "/updateInterval").toInt();
}

QString AppSettings::accountUsername(const QString &account) const
{
    return m_clientSettings->value(account + "/username").toString();
}

void AppSettings::addAccount(const QString &name,
                             const QString &address,
                             int port,
                             const QString &apiPath,
                             bool https,
                             bool localCertificate,
                             bool authentication,
                             const QString &username,
                             const QString &password,
                             int updateInterval,
                             int timeout)
{
    m_clientSettings->setValue(name + "/address", address);
    m_clientSettings->setValue(name + "/port", port);
    m_clientSettings->setValue(name + "/apiPath", apiPath);
    m_clientSettings->setValue(name + "/https", https);
    m_clientSettings->setValue(name + "/localCertificate", localCertificate);
    m_clientSettings->setValue(name + "/authentication", authentication);
    m_clientSettings->setValue(name + "/username", username);
    m_clientSettings->setValue(name + "/password", password);
    m_clientSettings->setValue(name + "/updateInterval", updateInterval);
    m_clientSettings->setValue(name + "/timeout", timeout);

    emit accountAdded(name, accounts().indexOf(name));

    if (accountsCount() == 1)
        setCurrentAccount(name);
}

void AppSettings::removeAccount(const QString &name)
{
    int index = accounts().indexOf(name);

    m_clientSettings->remove(name);

    if (isAccountLocalCertificateExists(name))
        removeAccountLocalCertificate(name);

    if (name == currentAccount()) {
        if (accounts().isEmpty())
            setCurrentAccount(QString());
        else
            setCurrentAccount(accounts().first());
    }

    emit accountRemoved(index);
}

void AppSettings::setAccount(const QString &name,
                             const QString &address,
                             int port,
                             const QString &apiPath,
                             bool https,
                             bool localCertificate,
                             bool localCertificateChanged,
                             bool authentication,
                             const QString &username,
                             const QString &password,
                             int updateInterval,
                             int timeout)
{
    bool changed = false;

    if (address != accountAddress(name)) {
        m_clientSettings->setValue(name + "/address", address);
        changed = true;
    }

    if (port != accountPort(name)) {
        m_clientSettings->setValue(name + "/port", port);
        changed = true;
    }

    if (apiPath != accountApiPath(name)) {
        m_clientSettings->setValue(name + "/apiPath", apiPath);
        changed = true;
    }

    if (https != accountHttps(name)) {
        m_clientSettings->setValue(name + "/https", https);
        changed = true;
    }

    if (localCertificate != accountLocalCertificate(name)) {
        m_clientSettings->setValue(name + "/localCertificate", localCertificate);
        changed = true;
    }

    if (localCertificateChanged)
        changed = true;

    if (authentication != accountAuthentication(name)) {
        m_clientSettings->setValue(name + "/authentication", authentication);
        changed = true;
    }

    if (username != accountUsername(name)) {
        m_clientSettings->setValue(name + "/username", username);
        changed = true;
    }

    if (password != accountPassword(name)) {
        m_clientSettings->setValue(name + "/password", password);
        changed = true;
    }

    if (updateInterval != accountUpdateInterval(name)) {
        m_clientSettings->setValue(name + "/updateInterval", updateInterval);
        changed = true;
    }

    if (timeout != accountTimeout(name)) {
        m_clientSettings->setValue(name + "/timeout", timeout);
        changed = true;
    }

    if (changed) {
        emit accountChanged(accounts().indexOf(name));

        if (name == currentAccount())
            emit currentAccountChanged();
    }
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

QVariant AppSettings::serverValue(const QString &key) const
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
    emit requestUpdateServerSettings(replyData);
}

void AppSettings::beginUpdateServerStats(const QByteArray &replyData)
{
    emit requestUpdateServerStats(replyData);
}

void AppSettings::endUpdateServerSettings(QVariantMap serverSettings)
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

}
