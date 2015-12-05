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

#ifndef TREMOTESF_TORRENTMODEL_H
#define TREMOTESF_TORRENTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QMutex>

class QThread;

namespace Tremotesf
{

class TorrentFileModel;
class TorrentPeerModel;
class TorrentTrackerModel;
class Transmission;

struct Torrent {
public:
    Torrent(const QString &hashString, int id, const QString &name);
    void update(const QVariantMap &torrentMap);
public:
    bool changed;

    QDateTime activityDate;
    QDateTime addedDate;
    int bandwidthPriority;
    QString downloadDir;
    int downloadLimit;
    bool downloadLimited;
    QString errorString;
    int eta;
    QString hashString;
    bool honorsSessionLimits;
    int id;
    bool isActive;
    qint64 leftUntilDone;
    QString name;
    int peerLimit;
    int peersConnected;
    int peersGettingFromUs;
    int peersSendingToUs;

    float percentDone;
    bool finished;

    int rateDownload;
    int rateUpload;
    float recheckProgress;
    float seedRatioLimit;
    int seedRatioMode;
    qint64 sizeWhenDone;
    int status;
    qint64 totalSize;
    qint64 uploadedEver;
    int uploadLimit;
    bool uploadLimited;
    float uploadRatio;

    QVariantList files;
    QVariantList fileStats;
    QVariantList peers;
    QVariantList trackers;
};

class TorrentModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentModelWorker(const QList<Torrent*> *torrents, const QList<int> *torrentIds);
    void doWork(QByteArray replyData);
private:
    const QList<Torrent*> *m_torrents;
    const QList<int> *m_torrentIds;
signals:
    void done(QList<Tremotesf::Torrent*> newTorrents, QList<int> newTorrentIds);
};

class TorrentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Role)
    Q_ENUMS(TorrentStatus)
    Q_PROPERTY(Tremotesf::TorrentFileModel* fileModel READ fileModel WRITE setFileModel)
    Q_PROPERTY(Tremotesf::TorrentPeerModel* peerModel READ peerModel WRITE setPeerModel)
    Q_PROPERTY(Tremotesf::TorrentTrackerModel* trackerModel READ trackerModel WRITE setTrackerModel)
    Q_PROPERTY(Tremotesf::Transmission* transmission READ transmission WRITE setTransmission)
public:
    enum Role {
        ActivityDateRole = Qt::UserRole,
        AddedDateRole,
        BandwidthPriorityRole,
        DownloadDirRole,
        DownloadLimitRole,
        DownloadLimitedRole,
        ErrorStringRole,
        EtaRole,
        HashStringRole,
        HonorsSessionLimitsRole,
        IdRole,
        LeftUntilDoneRole,
        NameRole,
        PeerLimitRole,
        PeersConnectedRole,
        PeersGettingFromUsRole,
        PeersSendingToUsRole,
        PercentDoneRole,
        RateDownloadRole,
        RateUploadRole,
        RecheckProgressRole,
        SeedRatioLimitRole,
        SeedRatioModeRole,
        SizeWhenDoneRole,
        StatusRole,
        TotalSizeRole,
        UploadedEverRole,
        UploadLimitRole,
        UploadLimitedRole,
        UploadRatioRole
    };

    enum TorrentStatus {
        StoppedStatus,
        QueuedForCheckingStatus,
        CheckingStatus,
        StalledStatus,
        DownloadingStatus,
        QueuedForSeedingStatus,
        SeedingStatus,
        IsolatedStatus,
        ErrorStatus
    };

    TorrentModel();
    ~TorrentModel();

    QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &modelIndex, const QVariant &value, int role);

    void resetModel();

    TorrentFileModel* fileModel() const;
    void setFileModel(TorrentFileModel *model);

    TorrentPeerModel* peerModel() const;
    void setPeerModel(TorrentPeerModel *model);

    TorrentTrackerModel* trackerModel() const;
    void setTrackerModel(TorrentTrackerModel *model);

    Transmission* transmission() const;
    void setTransmission(Transmission *newTransmission);

    void beginUpdateModel(const QByteArray &replyData);

    Q_INVOKABLE void removeAtIndex(int torrentIndex);

    Q_INVOKABLE void loadFileModel(int torrentIndex);
    Q_INVOKABLE void loadPeerModel(int torrentIndex);
    Q_INVOKABLE void loadTrackerModel(int torrentIndex);
private:
    void endUpdateModel(QList<Torrent*> newTorrents, QList<int> newTorrentIds);

    void setTorrentBandwidthPriority(Torrent *torrent, const QModelIndex &modelIndex, int bandwidthPriority);
    void setTorrentDownloadLimit(Torrent *torrent, const QModelIndex &modelIndex, int downloadLimit);
    void setTorrentDownloadLimited(Torrent *torrent, const QModelIndex &modelIndex, bool downloadLimited);
    void setTorrentHonorsSessionLimits(Torrent *torrent, const QModelIndex &modelIndex, bool honorsSessionLimits);
    void setTorrentPeerLimit(Torrent *torrent, const QModelIndex &modelIndex, int peerLimit);
    void setTorrentSeedRatioLimit(Torrent *torrent, const QModelIndex &modelIndex, float seedRatioLimit);
    void setTorrentSeedRatioMode(Torrent *torrent, const QModelIndex &modelIndex, int seedRatioMode);
    void setTorrentStatus(Torrent *torrent, const QModelIndex &modelIndex, int status);
    void setTorrentUploadLimit(Torrent *torrent, const QModelIndex &modelIndex, int uploadLimit);
    void setTorrentUploadLimited(Torrent *torrent, const QModelIndex &modelIndex, bool uploadLimited);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Torrent*> m_torrents;
    QList<int> m_torrentIds;

    TorrentFileModel *m_fileModel;
    TorrentPeerModel *m_peerModel;
    TorrentTrackerModel *m_trackerModel;
    Transmission *m_transmission;

    QThread *m_workerThread;

    QMutex m_mutex;
signals:
    void requestModelUpdate(QByteArray replyData);
    void torrentRemoved();
};

}

Q_DECLARE_METATYPE(Tremotesf::Torrent*)

#endif // TREMOTESF_TORRENTMODEL_H
