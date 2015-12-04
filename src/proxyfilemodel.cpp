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

#include "proxyfilemodel.h"
#include "proxyfilemodel.moc"

#include "torrentfilemodel.h"

namespace Tremotesf
{

ProxyFileModel::ProxyFileModel()
{
    setSortRole(TorrentFileModel::NameRole);
}

bool ProxyFileModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool leftIsDirectory = left.data(TorrentFileModel::IsDirectoryRole).toBool();
    bool rightIsDirectory = right.data(TorrentFileModel::IsDirectoryRole).toBool();

    if (leftIsDirectory == rightIsDirectory)
        return BaseProxyModel::lessThan(left, right);

    if (leftIsDirectory)
        return true;
    return false;
}

}
