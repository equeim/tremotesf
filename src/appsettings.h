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

#ifndef TREMOTESF_APPSETTINGS_H
#define TREMOTESF_APPSETTINGS_H

#include <QObject>
#include <QVariantMap>

class QThread;
class QSettings;

namespace Tremotesf
{

class AppSettingsWorker : public QObject
{
    Q_OBJECT
public:
    void parseServerSettings(QByteArray replyData);
    void parseServerStats(QByteArray replyData);
signals:
    void serverSettingsParsed(QVariantMap serverSettings);
    void serverStatsParsed(int downloadSpeed, int uploadSpeed);
};

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int accountsCount READ accountsCount)
    Q_PROPERTY(QString currentAccount READ currentAccount WRITE setCurrentAccount NOTIFY currentAccountChanged)
    Q_PROPERTY(int downloadSpeed READ downloadSpeed NOTIFY serverStatsUpdated)
    Q_PROPERTY(int uploadSpeed READ uploadSpeed NOTIFY serverStatsUpdated)
public:
    static const QString CertificatesDirectory;

    AppSettings();
    ~AppSettings();

    //
    // Client settings
    //
    void checkClientSettings();
    void checkLocalCertificates();

    int accountsCount() const;
    QStringList accounts() const;

    QString currentAccount() const;
    void setCurrentAccount(const QString &name);

    Q_INVOKABLE bool isAccountLocalCertificateExists(const QString &account) const;
    Q_INVOKABLE void setAccountLocalCertificate(const QString &account, const QString &filePath);
    Q_INVOKABLE void removeAccountLocalCertificate(const QString &account);

    Q_INVOKABLE QString accountAddress(const QString &account) const;
    Q_INVOKABLE QString accountApiPath(const QString &account) const;
    Q_INVOKABLE bool accountAuthentication(const QString &account) const;
    Q_INVOKABLE bool accountHttps(const QString &account) const;
    Q_INVOKABLE bool accountLocalCertificate(const QString &account) const;
    Q_INVOKABLE QString accountPassword(const QString &account) const;
    Q_INVOKABLE int accountPort(const QString &account) const;
    Q_INVOKABLE int accountTimeout(const QString &account) const;
    Q_INVOKABLE int accountUpdateInterval(const QString &account) const;
    Q_INVOKABLE QString accountUsername(const QString &account) const;

    Q_INVOKABLE void addAccount(const QString &name,
                                const QString &address,
                                int port,
                                const QString &apiPath,
                                bool https,
                                bool localCertificate,
                                bool authentication,
                                const QString &username,
                                const QString &password,
                                int updateInterval,
                                int timeout);

    Q_INVOKABLE void removeAccount(const QString &name);

    Q_INVOKABLE void setAccount(const QString &name,
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
                                int timeout);

    int filterMode() const;
    void setFilterMode(int filterMode);

    Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder sortOrder);

    int sortRole() const;
    void setSortRole(int sortRole);

    // Server settings
    int rpcVersion() const;
    Q_INVOKABLE QVariant serverValue(const QString &key) const;

    // Server stats
    int downloadSpeed() const;
    int uploadSpeed() const;

    void beginUpdateServerSettings(const QByteArray &replyData);
    void beginUpdateServerStats(const QByteArray &replyData);
private:
    void endUpdateServerSettings(QVariantMap serverSettings);
    void endUpdateServerStats(int downloadSpeed, int uploadSpeed);
private:
    QSettings *m_clientSettings;
    QVariantMap m_serverSettings;

    QThread *m_workerThread;

    int m_downloadSpeed;
    int m_uploadSpeed;
signals:
    void accountAdded(const QString &name, int index);
    void accountRemoved(int index);
    void accountChanged(int index);

    void currentAccountChanged();

    void requestUpdateServerSettings(QByteArray replyData);
    void requestUpdateServerStats(QByteArray replyData);

    void serverSettingsUpdated();
    void serverStatsUpdated();
};

}

#endif // TREMOTESF_APPSETTINGS_H
