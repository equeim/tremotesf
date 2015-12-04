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

#include <QThread>

namespace Tremotesf
{

TorrentTrackerModelWorker::TorrentTrackerModelWorker(const QList<TorrentTracker*> *trackers, const QList<int> *trackerIds)
{
    m_trackers = trackers;
    m_trackerIds = trackerIds;
}

void TorrentTrackerModelWorker::doWork(const QVariantList &trackerList)
{
    QList<TorrentTracker*> newTrackers;
    QList<int> newTrackerIds;

    for (int i = 0; i < trackerList.length(); i++) {
        QVariantMap trackerMap = trackerList.at(i).toMap();

        TorrentTracker *tracker;
        int trackerId = trackerMap.value("id").toInt();
        int index = m_trackerIds->indexOf(trackerId);
        if (index == -1) {
            tracker = new TorrentTracker;
            tracker->announce = trackerMap.value("announce").toString();
            tracker->host = trackerMap.value("host").toString();
            tracker->id = trackerId;
        } else {
            tracker = m_trackers->at(index);
        }
        newTrackers.append(tracker);
        newTrackerIds.append(trackerId);

        tracker->changed = false;

        QDateTime lastAnnounceTime = QDateTime::fromTime_t(trackerMap.value("lastAnnounceTime").toUInt());
        if (lastAnnounceTime != tracker->lastAnnounceTime) {
            tracker->lastAnnounceTime = lastAnnounceTime;
            tracker->changed = true;
        }
    }

    emit done(newTrackers, newTrackerIds);
}

TorrentTrackerModel::TorrentTrackerModel()
{
    qRegisterMetaType< QList<TorrentTracker*> >();

    m_worker = new TorrentTrackerModelWorker(&m_trackers, &m_trackerIds);
    connect(this, &TorrentTrackerModel::requestModelUpdate, m_worker, &TorrentTrackerModelWorker::doWork);
    connect(m_worker, &TorrentTrackerModelWorker::done, this, &TorrentTrackerModel::endUpdateModel);

    m_workerThread = new QThread(this);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);

    m_isActive = false;
}

TorrentTrackerModel::~TorrentTrackerModel()
{
    m_workerThread->quit();
    m_workerThread->wait();

    m_mutex.lock();
    qDeleteAll(m_trackers);
    m_mutex.unlock();
}

QVariant TorrentTrackerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TorrentTracker *tracker = m_trackers.at(index.row());

    switch (role) {
    case AnnounceRole:
        return tracker->announce;
    case HostRole:
        return tracker->host;
    case IdRole:
        return tracker->id;
    case LastAnnounceTimeRole:
        return tracker->lastAnnounceTime;
    default:
        return QVariant();
    }
}

int TorrentTrackerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_trackers.length();
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
    emit requestModelUpdate(trackerList);
}

void TorrentTrackerModel::removeAtIndex(int index)
{
    if (index >= rowCount())
        return;

    m_mutex.lock();

    beginRemoveRows(QModelIndex(), index, index);
    delete m_trackers.takeAt(index);
    m_trackerIds.removeAt(index);
    endRemoveRows();

    m_mutex.unlock();
}

void TorrentTrackerModel::resetModel()
{
    m_mutex.lock();

    beginResetModel();
    qDeleteAll(m_trackers);
    m_trackers.clear();
    m_trackerIds.clear();
    endResetModel();

    m_mutex.unlock();
}

void TorrentTrackerModel::endUpdateModel(const QList<TorrentTracker *> &newTrackers, const QList<int> &newTrackerIds)
{
    m_mutex.lock();

    for (int i = 0; i < m_trackerIds.length(); i++) {
        int trackerId = m_trackerIds.at(i);
        if (!newTrackerIds.contains(trackerId)) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_trackers.takeAt(i);
            m_trackerIds.removeAt(i);
            endRemoveRows();

            i--;
        }
    }

    for (int i = 0; i < newTrackerIds.length(); i++) {
        int trackerId = newTrackerIds.at(i);
        if (m_trackerIds.contains(trackerId)) {
            int row = m_trackerIds.indexOf(trackerId);
            if (m_trackers.at(row)->changed) {
                QModelIndex modelIndex = index(row, 0);
                emit dataChanged(modelIndex, modelIndex);
            }
        } else {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_trackers.append(newTrackers.at(i));
            m_trackerIds.append(newTrackerIds.at(i));
            endInsertRows();
        }
    }

    m_mutex.unlock();
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

}
