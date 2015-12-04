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

class Torrent {
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

    QVariantList fileList;
    QVariantList fileStatsList;
    QVariantList peerList;
    QVariantList trackerList;
};

class TorrentModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentModelWorker(const QList<Torrent*> *torrents, const QList<int> *torrentIds);
    void doWork(const QByteArray &replyData);
private:
    const QList<Torrent*> *m_torrents;
    const QList<int> *m_torrentIds;
signals:
    void done(const QList<Torrent*> &newTorrents, const QList<int> &newTorrentIds);
};

class TorrentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Role)
    Q_ENUMS(TorrentStatus)
    Q_PROPERTY(TorrentFileModel* fileModel READ fileModel WRITE setFileModel)
    Q_PROPERTY(TorrentPeerModel* peerModel READ peerModel WRITE setPeerModel)
    Q_PROPERTY(TorrentTrackerModel* trackerModel READ trackerModel WRITE setTrackerModel)
    Q_PROPERTY(Transmission* transmission READ transmission WRITE setTransmission)
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

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void resetModel();

    TorrentFileModel* fileModel() const;
    TorrentPeerModel* peerModel() const;
    TorrentTrackerModel* trackerModel() const;
    Transmission* transmission() const;

    void setFileModel(TorrentFileModel *fileModel);
    void setPeerModel(TorrentPeerModel *peerModel);
    void setTrackerModel(TorrentTrackerModel *trackerModel);
    void setTransmission(Transmission *transmission);

    void beginUpdateModel(const QByteArray &replyData);

    Q_INVOKABLE void removeAtIndex(int index);

    Q_INVOKABLE void loadFileModel (int index);
    Q_INVOKABLE void loadPeerModel (int index);
    Q_INVOKABLE void loadTrackerModel (int index);
private:
    void endUpdateModel(const QList<Torrent *> &newTorrents, const QList<int> newTorrentIds);

    void setTorrentBandwidthPriority(Torrent *torrent, const QModelIndex &index, int bandwidthPriority);
    void setTorrentDownloadLimit(Torrent *torrent, const QModelIndex &index, int downloadLimit);
    void setTorrentDownloadLimited(Torrent *torrent, const QModelIndex &index, bool downloadLimited);
    void setTorrentHonorsSessionLimits(Torrent *torrent, const QModelIndex &index, bool honorsSessionLimits);
    void setTorrentPeerLimit(Torrent *torrent, const QModelIndex &index, int peerLimit);
    void setTorrentSeedRatioLimit(Torrent *torrent, const QModelIndex &index, float seedRatioLimit);
    void setTorrentSeedRatioMode(Torrent *torrent, const QModelIndex &index, int seedRatioMode);
    void setTorrentStatus(Torrent *torrent, const QModelIndex &index, int status);
    void setTorrentUploadLimit(Torrent *torrent, const QModelIndex &index, int uploadLimit);
    void setTorrentUploadLimited(Torrent *torrent, const QModelIndex &index, bool uploadLimited);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Torrent*> m_torrents;
    QList<int> m_torrentIds;

    TorrentFileModel *m_fileModel;
    TorrentPeerModel *m_peerModel;
    TorrentTrackerModel *m_trackerModel;
    Transmission *m_transmission;

    TorrentModelWorker *m_worker;
    QThread *m_workerThread;

    QMutex m_mutex;
signals:
    void requestModelUpdate(const QByteArray &replyData);
    void torrentRemoved();
};

}

Q_DECLARE_METATYPE(Tremotesf::Torrent*)

#endif // TREMOTESF_TORRENTMODEL_H
