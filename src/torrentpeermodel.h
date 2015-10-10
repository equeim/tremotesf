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

#ifndef TORRENTPEERMODEL_H
#define TORRENTPEERMODEL_H

#include <QAbstractListModel>
#include <QMutex>

class QThread;

struct TorrentPeer {
    QString address;
    int progress;
    int rateToClient;
    int rateToPeer;

    bool changed;
};
Q_DECLARE_METATYPE(TorrentPeer*)

class TorrentPeerModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentPeerModelWorker(QList<TorrentPeer*> *peers, QStringList *addresses);
    void doWork(const QVariantList &peerList);
private:
    QList<TorrentPeer*> *m_peers;
    QStringList *m_addresses;

    QList<TorrentPeer*> m_newPeers;
    QStringList m_newAddresses;
signals:
    void done(const QList<TorrentPeer*> &newPeers, const QStringList &newAddresses);
};

class TorrentPeerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TorrentPeerRoles)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive)
public:
    enum TorrentPeerRoles {
        AddressRole = Qt::UserRole,
        ProgressRole,
        RateToClientRole,
        RateToPeerRole
    };

    TorrentPeerModel();
    ~TorrentPeerModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    bool isActive() const;
    int torrentId() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &peerList);

    Q_INVOKABLE void resetModel();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void endUpdateModel(const QList<TorrentPeer*> &newPeers, const QStringList &newAddresses);
private:
    QList<TorrentPeer*> m_peers;
    QStringList m_addresses;

    TorrentPeerModelWorker *m_worker;
    QThread *m_workerThread;

    QMutex m_mutex;

    bool m_isActive;
    int m_torrentId;
signals:
    void requestModelUpdate(const QVariantList &peerList);
};
#endif // TORRENTPEERMODEL_H
