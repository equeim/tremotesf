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

#ifndef TORRENTTRACKERMODEL_H
#define TORRENTTRACKERMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QMutex>

struct TorrentTracker {
    QString announce;
    QString host;
    int id;
    QDateTime lastAnnounceTime;

    bool changed;
};
Q_DECLARE_METATYPE(TorrentTracker*)

class TorrentTrackerModelWorker: public QObject
{
    Q_OBJECT
public:
    TorrentTrackerModelWorker(const QList<TorrentTracker*> *trackers, const QList<int> *trackerIds);
    void doWork(const QVariantList &trackerList);
private:
    const QList<TorrentTracker*> *m_trackers;
    const QList<int> *m_trackerIds;
signals:
    void done(const QList<TorrentTracker*> &newTrackers, const QList<int> &newTrackerIds);
};

class TorrentTrackerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TorrentPeerRoles)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive)
public:
    enum TorrentPeerRoles {
        AnnounceRole = Qt::UserRole,
        HostRole,
        IdRole,
        LastAnnounceTimeRole
    };

    TorrentTrackerModel();
    ~TorrentTrackerModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    bool isActive() const;
    int torrentId() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &trackerList);

    Q_INVOKABLE void removeAtIndex(int index);
    Q_INVOKABLE void resetModel();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void endUpdateModel(const QList<TorrentTracker*> &newTrackers, const QList<int> &newTrackerIds);
private:
    QList<TorrentTracker*> m_trackers;
    QList<int> m_trackerIds;

    TorrentTrackerModelWorker *m_worker;
    QThread *m_workerThread;

    bool m_isActive;
    int m_torrentId;

    QMutex m_mutex;
signals:
    void requestModelUpdate(const QVariantList &peerList);
};

#endif // TORRENTTRACKERMODEL_H
