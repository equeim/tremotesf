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

#include "accountmodel.h"
#include "accountmodel.moc"

#include <QDebug>

#include "appsettings.h"

namespace Tremotesf
{

Account::Account(const QString &name, AppSettings *appSettings)
{
    this->name = name;
    m_appSettings = appSettings;
    update();
}

void Account::update()
{
    address = m_appSettings->accountAddress(name);
    port = m_appSettings->accountPort(name);
    apiPath = m_appSettings->accountApiPath(name);
    https = m_appSettings->accountHttps(name);
    localCertificate = m_appSettings->accountLocalCertificate(name);
    authentication = m_appSettings->accountAuthentication(name);
    username = m_appSettings->accountUsername(name);
    updateInterval = m_appSettings->accountUpdateInterval(name);
    timeout = m_appSettings->accountTimeout(name);
}

void AccountModel::classBegin()
{

}

void AccountModel::componentComplete()
{
    connect(m_appSettings, &AppSettings::accountAdded, this, &AccountModel::addAccount);
    connect(m_appSettings, &AppSettings::accountRemoved, this, &AccountModel::removeAccount);
    connect(m_appSettings, &AppSettings::accountChanged, this, &AccountModel::updateAccount);

    setupModel();
}

AppSettings* AccountModel::appSettings() const
{
    return m_appSettings;
}

void AccountModel::setAppSettings(AppSettings *appSettings)
{
    m_appSettings = appSettings;
}

int AccountModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_accounts.length();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Account &account = m_accounts.at(index.row());

    switch (role) {
    case NameRole:
        return account.name;
    case AddressRole:
        return account.address;
    case PortRole:
        return account.port;
    case ApiPathRole:
        return account.apiPath;
    case HttpsRole:
        return account.https;
    case LocalCertificateRole:
        return account.localCertificate;
    case AuthenticationRole:
        return account.authentication;
    case UsernameRole:
        return account.username;
    case PasswordRole:
        return account.password;
    case UpdateIntervalRole:
        return account.updateInterval;
    case TimeoutRole:
        return account.timeout;
    default:
        return QVariant();
    }
}

void AccountModel::setupModel()
{
    foreach (const QString &name, m_appSettings->accounts()) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_accounts.append(Account(name, m_appSettings));
        endInsertRows();
    }
}

void AccountModel::addAccount(const QString &name, int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_accounts.insert(index, Account(name, m_appSettings));
    endInsertRows();
}

void AccountModel::removeAccount(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_accounts.removeAt(index);
    endRemoveRows();
}

void AccountModel::updateAccount(int index)
{
    m_accounts[index].update();
    QModelIndex modelIndex = this->index(index);
    emit dataChanged(modelIndex, modelIndex);
}

QHash<int, QByteArray> AccountModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(NameRole, "name");
    roles.insert(AddressRole, "address");
    roles.insert(PortRole, "port");
    roles.insert(ApiPathRole, "apiPath");
    roles.insert(HttpsRole, "https");
    roles.insert(LocalCertificateRole, "localCertificate");
    roles.insert(AuthenticationRole, "authentication");
    roles.insert(UsernameRole, "username");
    roles.insert(PasswordRole, "password");
    roles.insert(UpdateIntervalRole, "updateInterval");
    roles.insert(TimeoutRole, "timeout");

    return roles;
}

}
