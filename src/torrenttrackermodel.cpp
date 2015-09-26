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

#include "torrenttrackermodel.h"
#include "torrenttrackermodel.moc"

#include <QEventLoop>

TorrentTrackerModelWorker::TorrentTrackerModelWorker()
{

}

void TorrentTrackerModelWorker::setTrackerList(const QVariantList &trackerList)
{
    m_trackerList = trackerList;
}

void TorrentTrackerModelWorker::run() Q_DECL_OVERRIDE
{
    QList<TorrentTracker> trackers;

    for (int i = 0; i < m_trackerList.length(); i++) {
        TorrentTracker tracker;
        QVariantMap trackerMap = m_trackerList.at(i).toMap();

        tracker.announce = trackerMap.value("announce").toString();
        tracker.host = trackerMap.value("host").toString();
        tracker.id = trackerMap.value("id").toInt();
        if (trackerMap.value("lastAnnounceTime").toUInt() != 0)
            tracker.lastAnnounceTime = QDateTime::fromTime_t(trackerMap.value("lastAnnounceTime").toUInt());

        trackers.append(tracker);
    }

    emit done(trackers);
}

TorrentTrackerModel::TorrentTrackerModel()
{
   m_worker = new TorrentTrackerModelWorker;
   connect(m_worker, &TorrentTrackerModelWorker::done, this, &TorrentTrackerModel::endUpdateModel);

   m_isActive = false;

   m_changingModel = false;
}

TorrentTrackerModel::~TorrentTrackerModel()
{
    m_worker->wait();
    m_worker->deleteLater();
}

int TorrentTrackerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_trackers.length();
}

QVariant TorrentTrackerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TorrentTracker &tracker = m_trackers.at(index.row());

    switch (role) {
    case AnnounceRole:
        return tracker.announce;
    case HostRole:
        return tracker.host;
    case IdRole:
        return tracker.id;
    case LastAnnounceTimeRole:
        return tracker.lastAnnounceTime;
    default:
        return QVariant();
    }
}

bool TorrentTrackerModel::isActive() const
{
    return m_isActive;
}

int TorrentTrackerModel::torrentId() const
{
    return m_torrentId;
}

void TorrentTrackerModel::setIsActive(bool isActive)
{
    m_isActive = isActive;
}

void TorrentTrackerModel::setTorrentId(int torrentId)
{
    m_torrentId = torrentId;
}

void TorrentTrackerModel::beginUpdateModel(const QVariantList &trackerList)
{
    m_worker->wait();
    m_worker->setTrackerList(trackerList);
    m_worker->start(QThread::LowPriority);
}

void TorrentTrackerModel::removeAtIndex(int index)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentTrackerModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginRemoveRows(QModelIndex(), index, index);
    m_trackers.removeAt(index);
    endRemoveRows();

    m_changingModel = false;
    emit modelChanged();
}

void TorrentTrackerModel::resetModel()
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentTrackerModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginResetModel();
    m_trackers.clear();
    endResetModel();

    m_isActive = false;

    m_changingModel = false;
    emit modelChanged();
}

void TorrentTrackerModel::endUpdateModel(const QList<TorrentTracker> &trackers)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentTrackerModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    int oldCount = m_trackers.length();
    m_trackers = trackers;

    if (m_trackers.length() > oldCount) {
        for (int i = oldCount; i < m_trackers.length(); i++) {
            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();
        }
    } else if (m_trackers.length() < oldCount) {
        for (int i = oldCount; i > m_trackers.length(); i--) {
            beginRemoveRows(QModelIndex(), i - 1, i - 1);
            endRemoveRows();
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1));

    m_changingModel = false;
    emit modelChanged();
}

QHash<int, QByteArray> TorrentTrackerModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(AnnounceRole, "announce");
    roles.insert(HostRole, "host");
    roles.insert(IdRole, "id");
    roles.insert(LastAnnounceTimeRole, "lastAnnounceTime");

    return roles;
}
