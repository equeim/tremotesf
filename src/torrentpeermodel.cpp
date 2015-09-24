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

#include <QEventLoop>

TorrentPeerModelWorker::TorrentPeerModelWorker()
{

}

void TorrentPeerModelWorker::setPeerList(const QVariantList &peerList)
{
    m_peerList = peerList;
}

void TorrentPeerModelWorker::run() Q_DECL_OVERRIDE
{
    QList<TorrentPeer> peers;

    for (int i = 0; i < m_peerList.length(); i++) {
        TorrentPeer peer;
        QVariantMap peerMap = m_peerList.at(i).toMap();

        peer.address = peerMap.value("address").toString();
        peer.progress = peerMap.value("progress").toDouble();
        peer.rateToClient = peerMap.value("rateToClient").toInt();
        peer.rateToPeer = peerMap.value("rateToPeer").toInt();

        peers.append(peer);
    }

    emit done(peers);
}

TorrentPeerModel::TorrentPeerModel()
{
    m_worker = new TorrentPeerModelWorker;
    connect(m_worker, &TorrentPeerModelWorker::done, this, &TorrentPeerModel::endUpdateModel);

    m_isActive = false;

    m_changingModel = false;
}

TorrentPeerModel::~TorrentPeerModel()
{
    m_worker->wait();
    m_worker->deleteLater();
}

int TorrentPeerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_peers.length();
}

QVariant TorrentPeerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const TorrentPeer &peer = m_peers.at(index.row());

    switch (role) {
    case AddressRole:
        return peer.address;
    case ProgressRole:
        return peer.progress;
    case RateToClientRole:
        return peer.rateToClient;
    case RateToPeerRole:
        return peer.rateToPeer;
    default:
        return QVariant();
    }
}

bool TorrentPeerModel::isActive() const
{
    return m_isActive;
}

int TorrentPeerModel::torrentId() const
{
    return m_torrentId;
}

void TorrentPeerModel::setIsActive(bool isActive)
{
    m_isActive = isActive;
}

void TorrentPeerModel::setTorrentId(int torrentId)
{
    m_torrentId = torrentId;
}

void TorrentPeerModel::beginUpdateModel(const QVariantList &peerList)
{
    m_worker->wait();
    m_worker->setPeerList(peerList);
    m_worker->start(QThread::LowPriority);
}

void TorrentPeerModel::endUpdateModel(const QList<TorrentPeer> &peers)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentPeerModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    int oldCount = m_peers.length();

    m_peers = peers;

    if (m_peers.length() > oldCount) {
        for (int i = oldCount; i < m_peers.length(); i++) {
            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();
        }
    } else if (m_peers.length() < oldCount) {
        for (int i = oldCount; i > m_peers.length(); i--) {
            beginRemoveRows(QModelIndex(), i - 1, i - 1);
            endRemoveRows();
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1));

    m_changingModel = false;
    emit modelChanged();
}

void TorrentPeerModel::resetModel()
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentPeerModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginResetModel();
    m_peers.clear();
    endResetModel();

    m_isActive = false;

    m_changingModel = false;
    emit modelChanged();
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
