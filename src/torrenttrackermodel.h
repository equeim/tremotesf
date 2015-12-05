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

#ifndef TREMOTESF_TORRENTTRACKERMODEL_H
#define TREMOTESF_TORRENTTRACKERMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QMutex>

namespace Tremotesf
{

struct TorrentTracker {
    QString announce;
    QString host;
    int id;
    QDateTime lastAnnounceTime;

    bool changed;
};

class TorrentTrackerModelWorker: public QObject
{
    Q_OBJECT
public:
    TorrentTrackerModelWorker(const QList<TorrentTracker*> *trackers, const QList<int> *trackerIds);
    void doWork(QVariantList trackerVariants);
private:
    const QList<TorrentTracker*> *m_trackers;
    const QList<int> *m_trackerIds;
signals:
    void done(QList<Tremotesf::TorrentTracker*> newTrackers, QList<int> newTrackerIds);
};

class TorrentTrackerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(TorrentPeerRoles)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
public:
    enum TorrentPeerRoles {
        AnnounceRole = Qt::UserRole,
        HostRole,
        IdRole,
        LastAnnounceTimeRole
    };

    TorrentTrackerModel();
    ~TorrentTrackerModel();

    QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    bool isActive() const;
    void setActive(bool active);

    int torrentId() const;
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &trackerVariants);

    Q_INVOKABLE void removeAtIndex(int trackerIndex);
    Q_INVOKABLE void resetModel();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void endUpdateModel(QList<TorrentTracker*> newTrackers, QList<int> newTrackerIds);
private:
    QList<TorrentTracker*> m_trackers;
    QList<int> m_trackerIds;

    QThread *m_workerThread;

    bool m_active;
    int m_torrentId;

    QMutex m_mutex;
signals:
    void requestModelUpdate(QVariantList peerList);
};

}

Q_DECLARE_METATYPE(Tremotesf::TorrentTracker*)

#endif // TREMOTESF_TORRENTTRACKERMODEL_H
