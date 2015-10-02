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

#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>

//class Account;
class AppSettings;

/*struct Account {
    QString name;
    QString address;
    int port;
    QString apiPath;
    bool https;
    bool localCertificate;
    bool authentication;
    QString username;
    QString password;
    int updateInterval;
    int timeout;
    //bool isCurrent;
};
Q_DECLARE_METATYPE(Account)*/

class Account
{
public:
    Account(const QString &name, AppSettings *appSettings);
    void update();
public:
    QString name;
    QString address;
    int port;
    QString apiPath;
    bool https;
    bool localCertificate;
    bool authentication;
    QString username;
    QString password;
    int updateInterval;
    int timeout;
private:
    AppSettings *m_appSettings;
};

class AccountModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AppSettings* appSettings READ appSettings WRITE setAppSettings)
public:
    enum AccountRoles {
        NameRole = Qt::UserRole,
        AddressRole,
        PortRole,
        ApiPathRole,
        HttpsRole,
        LocalCertificateRole,
        AuthenticationRole,
        UsernameRole,
        PasswordRole,
        UpdateIntervalRole,
        TimeoutRole
    };

    AccountModel();
    void classBegin();
    void componentComplete();

    AppSettings* appSettings() const;
    void setAppSettings(AppSettings *appSettings);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    void setupModel();
    void addAccount(const QString &name, int index);
    void removeAccount(int index);
    void updateAccount(int index);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Account> m_accounts;
    QStringList m_accountNames;

    AppSettings *m_appSettings;
};

#endif // ACCOUNTMODEL_H
