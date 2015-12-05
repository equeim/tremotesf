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

#include "torrentpeermodel.h"
#include "torrentpeermodel.moc"

#include <QDebug>
#include <QThread>

namespace Tremotesf
{

TorrentPeerModelWorker::TorrentPeerModelWorker(QList<TorrentPeer *> *peers, QStringList *addresses)
    : m_peers(peers),
      m_addresses(addresses)
{

}

void TorrentPeerModelWorker::doWork(QVariantList peerVariants)
{
    QList<TorrentPeer*> newPeers;
    QStringList newAddresses;

    for (QVariantList::const_iterator iterator = peerVariants.cbegin(), cend = peerVariants.cend();
         iterator != cend;
         iterator++) {

        QVariantMap peerMap = iterator->toMap();

        TorrentPeer *peer;
        QString address = peerMap.value("address").toString();
        int index = m_addresses->indexOf(address);
        if (index == -1) {
            peer = new TorrentPeer;
            peer->address = address;
        } else {
            peer = m_peers->at(index);
        }
        newPeers.append(peer);
        newAddresses.append(address);

        peer->changed = false;

        float progress = peerMap.value("progress").toFloat() * 100.0;
        if (progress != peer->progress) {
            peer->progress = progress;
            peer->changed = true;
        }

        int rateToClient = peerMap.value("rateToClient").toInt();
        if (rateToClient != peer->rateToClient) {
            peer->rateToClient = rateToClient;
            peer->changed = true;
        }

        int rateToPeer = peerMap.value("rateToPeer").toInt();
        if (rateToPeer != peer->rateToPeer) {
            peer->rateToPeer = rateToPeer;
            peer->changed = true;
        }
    }

    emit done(newPeers, newAddresses);
}

TorrentPeerModel::TorrentPeerModel()
    : m_workerThread(new QThread(this)),
      m_active(false)
{
    qRegisterMetaType< QList<TorrentPeer*> >();

    TorrentPeerModelWorker *worker = new TorrentPeerModelWorker(&m_peers, &m_addresses);
    connect(this, &TorrentPeerModel::requestModelUpdate, worker, &TorrentPeerModelWorker::doWork);
    connect(worker, &TorrentPeerModelWorker::done, this, &TorrentPeerModel::endUpdateModel);

    connect(m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);
}

TorrentPeerModel::~TorrentPeerModel()
{
    m_workerThread->quit();
    m_workerThread->wait();

    m_mutex.lock();
    qDeleteAll(m_peers);
    m_mutex.unlock();
}

QVariant TorrentPeerModel::data(const QModelIndex &modelIndex, int role) const
{
    if (!modelIndex.isValid())
        return QVariant();

    const TorrentPeer *peer = m_peers.at(modelIndex.row());

    switch (role) {
    case AddressRole:
        return peer->address;
    case ProgressRole:
        return peer->progress;
    case RateToClientRole:
        return peer->rateToClient;
    case RateToPeerRole:
        return peer->rateToPeer;
    default:
        return QVariant();
    }
}

int TorrentPeerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_peers.length();
}

bool TorrentPeerModel::isActive() const
{
    return m_active;
}

void TorrentPeerModel::setActive(bool active)
{
    m_active = active;
}

int TorrentPeerModel::torrentId() const
{
    return m_torrentId;
}

void TorrentPeerModel::setTorrentId(int torrentId)
{
    m_torrentId = torrentId;
}

void TorrentPeerModel::beginUpdateModel(const QVariantList &peerVariants)
{
    emit requestModelUpdate(peerVariants);
}

void TorrentPeerModel::resetModel()
{
    m_mutex.lock();

    beginResetModel();
    qDeleteAll(m_peers);
    m_peers.clear();
    m_addresses.clear();
    endResetModel();

    m_mutex.unlock();
}

QHash<int, QByteArray> TorrentPeerModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(AddressRole, "address");
    roles.insert(ProgressRole, "progress");
    roles.insert(RateToClientRole, "rateToClient");
    roles.insert(RateToPeerRole, "rateToPeer");

    return roles;
}

void TorrentPeerModel::endUpdateModel(QList<TorrentPeer*> newPeers, QStringList newAddresses)
{
    m_mutex.lock();

    for (int i = 0, addressesCount = m_addresses.size(); i < addressesCount; i++) {
        QString address = m_addresses.at(i);
        if (!newAddresses.contains(address)) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_peers.takeAt(i);
            m_addresses.removeAt(i);
            endRemoveRows();

            i--;
        }
    }

    for (int i = 0, newAddressesCount = newAddresses.size(); i < newAddressesCount; i++) {
        QString address = newAddresses.at(i);
        if (m_addresses.contains(address)) {
            int row = m_addresses.indexOf(address);
            TorrentPeer *peer = m_peers.at(row);
            if (peer->changed) {
                QModelIndex modelIndex = index(row, 0);
                emit dataChanged(modelIndex, modelIndex);
            }
        } else {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_peers.append(newPeers.at(i));
            m_addresses.append(address);
            endInsertRows();
        }
    }

    m_mutex.unlock();
}

}
