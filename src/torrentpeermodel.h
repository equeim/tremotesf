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

#ifndef TREMOTESF_TORRENTPEERMODEL_H
#define TREMOTESF_TORRENTPEERMODEL_H

#include <QAbstractListModel>
#include <QMutex>

class QThread;

namespace Tremotesf
{

struct TorrentPeer {
    QString address;
    int progress;
    int rateToClient;
    int rateToPeer;

    bool changed;
};

class TorrentPeerModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentPeerModelWorker(QList<TorrentPeer*> *peers, QStringList *addresses);
    void doWork(QVariantList peerVariants);
private:
    QList<TorrentPeer*> *m_peers;
    QStringList *m_addresses;
signals:
    void done(QList<Tremotesf::TorrentPeer*> newPeers, QStringList newAddresses);
};

class TorrentPeerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TorrentPeerRoles)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
public:
    enum TorrentPeerRoles {
        AddressRole = Qt::UserRole,
        ProgressRole,
        RateToClientRole,
        RateToPeerRole
    };

    TorrentPeerModel();
    ~TorrentPeerModel();

    QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    bool isActive() const;
    void setActive(bool active);

    int torrentId() const;
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &peerVariants);

    Q_INVOKABLE void resetModel();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void endUpdateModel(QList<TorrentPeer*> newPeers, QStringList newAddresses);
private:
    QList<TorrentPeer*> m_peers;
    QStringList m_addresses;

    QThread *m_workerThread;

    QMutex m_mutex;

    bool m_active;
    int m_torrentId;
signals:
    void requestModelUpdate(QVariantList peerList);
};

}

Q_DECLARE_METATYPE(Tremotesf::TorrentPeer*)

#endif // TREMOTESF_TORRENTPEERMODEL_H
