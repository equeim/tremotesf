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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QThread>
#include <QVariant>

class QSettings;

class ServerStatsWorker : public QThread
{
    Q_OBJECT
public:
    ServerStatsWorker();

    void setReplyData(const QByteArray &replyData);
private:
    QByteArray m_replyData;
protected:
    void run();
signals:
    void dataParsed(int downloadSpeed, int uploadSpeed);
};

class ServerSettingsWorker : public QThread
{
    Q_OBJECT
public:
    ServerSettingsWorker();

    void setReplyData(const QByteArray &replyData);
private:
    QByteArray m_replyData;
protected:
    void run();
signals:
    void dataParsed(const QVariantMap &serverSettings);
};

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList accounts READ accounts)
    Q_PROPERTY(QString currentAccount READ currentAccount WRITE setCurrentAccount NOTIFY currentAccountChanged)
    Q_PROPERTY(int downloadSpeed READ downloadSpeed NOTIFY serverStatsUpdated)
    Q_PROPERTY(int uploadSpeed READ uploadSpeed NOTIFY serverStatsUpdated)
public:
    AppSettings();
    ~AppSettings();

    void checkClientSettings();

    QStringList accounts() const;
    QString currentAccount() const;
    int downloadSpeed() const;
    int uploadSpeed() const;

    void setCurrentAccount(QString name);

    void beginUpdateServerSettings(const QByteArray &replyData);
    void beginUpdateServerStats(const QByteArray &replyData);

    Q_INVOKABLE void removeAccount(QString name);
    Q_INVOKABLE QVariant getClientValue(QString key) const;
    Q_INVOKABLE void setClientValue(QString key, const QVariant &value);
    Q_INVOKABLE QVariant getServerValue(QString key) const;
public slots:
    void endUpdateServerSettings(const QVariantMap &serverSettings);
    void endUpdateServerStats(int downloadSpeed, int uploadSpeed);
private:
    QSettings *m_clientSettings;
    ServerStatsWorker *m_statsWorker;
    ServerSettingsWorker *m_settingsWorker;
    QVariantMap m_serverSettings;

    int m_downloadSpeed;
    int m_uploadSpeed;
signals:
    void currentAccountChanged();
    void serverStatsUpdated();
};

#endif // APPSETTINGS_H
