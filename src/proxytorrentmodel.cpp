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
#include "torrentmodel.h"

#include "proxytorrentmodel.h"

ProxyTorrentModel::ProxyTorrentModel()
{

}

void ProxyTorrentModel::classBegin()
{

}

void ProxyTorrentModel::componentComplete()
{
    setSortRole(m_appSettings->getClientValue("sortRole").toInt());
    sort(0, static_cast<Qt::SortOrder>(m_appSettings->getClientValue("sortOrder").toInt()));

    setFilterMode(m_appSettings->getClientValue("filterMode").toInt());
}

AppSettings* ProxyTorrentModel::appSettings() const
{
    return m_appSettings;
}

int ProxyTorrentModel::filterMode() const
{
    return m_filterMode;
}

void ProxyTorrentModel::setAppSettings(AppSettings *appSettings)
{
    m_appSettings = appSettings;
}

void ProxyTorrentModel::setFilterMode(int mode)
{
    m_filterMode = mode;
    setFilterRegExp(QRegExp());
    m_appSettings->setClientValue("filterMode", mode);

    switch (mode) {
    case AllMode:
        break;
    case ActiveMode:
        setFilterRole(TorrentModel::StatusRole);
        setFilterRegExp("[46]");
        break;
    case FinishedMode:
        setFilterRole(TorrentModel::PercentDoneRole);
        setFilterFixedString("100");
        break;
    case DownloadingMode:
        setFilterRole(TorrentModel::StatusRole);
        setFilterFixedString("4");
        break;
    case SeedingMode:
        setFilterRole(TorrentModel::StatusRole);
        setFilterFixedString("6");
        break;
    case StoppedMode:
        setFilterRole(TorrentModel::StatusRole);
        setFilterFixedString("0");
    }

    emit filterModeChanged();
}

void ProxyTorrentModel::setSortOrder(Qt::SortOrder order)
{
    sort(0, order);
    m_appSettings->setClientValue("sortOrder", order);
}

void ProxyTorrentModel::setSortRole(int role)
{
    QSortFilterProxyModel::setSortRole(role);
    m_appSettings->setClientValue("sortRole", role);
    emit sortRoleChanged();
}

int ProxyTorrentModel::getSourceIndex(int proxyIndex) const
{
    return mapToSource(index(proxyIndex, 0)).row();
}
