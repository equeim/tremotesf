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
#include <QThread>

struct TorrentPeer {
    QString address;
    int progress;
    int rateToClient;
    int rateToPeer;
};
Q_DECLARE_METATYPE(TorrentPeer)

class TorrentPeerModelWorker: public QThread
{
    Q_OBJECT
public:
    TorrentPeerModelWorker();
    void setPeerList(const QVariantList &peerList);
private:
    QVariantList m_peerList;
protected:
    void run();
signals:
    void done(const QList<TorrentPeer> &peers);
};


class TorrentPeerModel : public QAbstractListModel
{
    Q_OBJECT
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

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool isActive() const;
    int torrentId() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &peerList);

    Q_INVOKABLE void resetModel();
public slots:
    void endUpdateModel(const QList<TorrentPeer> &peers);
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<TorrentPeer> m_peers;
    TorrentPeerModelWorker *m_worker;

    bool m_isActive;
    int m_torrentId;

    bool m_changingModel;
signals:
    void modelChanged();
};
#endif // TORRENTPEERMODEL_H
