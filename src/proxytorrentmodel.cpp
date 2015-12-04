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

#include "proxytorrentmodel.h"
#include "proxytorrentmodel.moc"

#include "appsettings.h"
#include "torrentmodel.h"

namespace Tremotesf
{

void ProxyTorrentModel::classBegin()
{

}

void ProxyTorrentModel::componentComplete()
{
    setSortRole(m_appSettings->sortRole());
    sort(0, m_appSettings->sortOrder());

    setFilterMode(m_appSettings->filterMode());
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

void ProxyTorrentModel::setFilterMode(int filterMode)
{
    m_filterMode = filterMode;
    setFilterRegExp(QRegExp());
    m_appSettings->setFilterMode(filterMode);

    switch (filterMode) {
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

void ProxyTorrentModel::setSortOrder(Qt::SortOrder sortOrder)
{
    sort(0, sortOrder);
    m_appSettings->setSortOrder(sortOrder);
}

void ProxyTorrentModel::setSortRole(int sortRole)
{
    QSortFilterProxyModel::setSortRole(sortRole);
    m_appSettings->setSortRole(sortRole);
    emit sortRoleChanged();
}

}
