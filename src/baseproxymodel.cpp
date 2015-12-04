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

#include "baseproxymodel.h"
#include "baseproxymodel.moc"

#include <QDebug>

namespace Tremotesf
{

BaseProxyModel::BaseProxyModel()
{
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);
    sort(0);
}

int BaseProxyModel::getSourceIndex(int proxyIndex) const
{
    return mapToSource(index(proxyIndex, 0)).row();
}

bool BaseProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftVariant = left.data(sortRole());
    QVariant rightVariant = right.data(sortRole());

    if (leftVariant.userType() == QVariant::String) {
        int compare = m_collator.compare(leftVariant.toString(), rightVariant.toString());
        if (compare < 0)
            return true;
        return false;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

}
