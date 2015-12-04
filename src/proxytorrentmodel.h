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

#ifndef TREMOTESF_PROXYTORRENTMODEL_H
#define TREMOTESF_PROXYTORRENTMODEL_H

#include <QQmlParserStatus>

#include "baseproxymodel.h"

namespace Tremotesf
{

class AppSettings;

class ProxyTorrentModel : public BaseProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_ENUMS(FilterMode)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AppSettings* appSettings READ appSettings WRITE setAppSettings)
    Q_PROPERTY(int filterMode READ filterMode WRITE setFilterMode NOTIFY filterModeChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole NOTIFY sortRoleChanged)
public:
    enum FilterMode {
        AllMode,
        ActiveMode,
        FinishedMode,
        DownloadingMode,
        SeedingMode,
        StoppedMode
    };

    void classBegin();
    void componentComplete();

    AppSettings* appSettings() const;
    int filterMode() const;

    void setAppSettings(AppSettings *appSettings);
    void setFilterMode(int filterMode);
    void setSortOrder(Qt::SortOrder sortOrder);
    void setSortRole(int sortRole);
private:
    AppSettings *m_appSettings;
    int m_filterMode;
signals:
    void filterModeChanged();
    void sortRoleChanged();
};

}

#endif // TREMOTESF_PROXYTORRENTMODEL_H
