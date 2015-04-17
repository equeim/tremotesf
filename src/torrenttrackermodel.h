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
#include <QThread>

struct TorrentTracker {
    QString announce;
    QString host;
    int id;
    QDateTime lastAnnounceTime;
};
Q_DECLARE_METATYPE(TorrentTracker)

class TorrentTrackerModelWorker: public QThread
{
    Q_OBJECT
public:
    TorrentTrackerModelWorker();
    void setTrackerList(const QVariantList &trackerList);
private:
    QVariantList m_trackerList;
protected:
    void run();
signals:
    void done(const QList<TorrentTracker> &trackers);
};

class TorrentTrackerModel : public QAbstractListModel
{
    Q_OBJECT
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

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool isActive() const;
    int torrentId() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &trackerList);

    Q_INVOKABLE void removeAtIndex(int index);
    Q_INVOKABLE void resetModel();
public slots:
    void endUpdateModel(const QList<TorrentTracker> &trackers);
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<TorrentTracker> m_trackers;
    TorrentTrackerModelWorker *m_worker;

    bool m_isActive;
    int m_torrentId;

    bool m_changingModel;
signals:
    void modelChanged();
};

#endif // TORRENTTRACKERMODEL_H
