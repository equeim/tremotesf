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

#ifndef TORRENTMODEL_H
#define TORRENTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QThread>

class TorrentFileModel;
class TorrentPeerModel;
class TorrentTrackerModel;

struct Torrent {
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
Q_DECLARE_METATYPE(Torrent)

class TorrentModelWorker : public QThread
{
    Q_OBJECT
public:
    TorrentModelWorker();
    void setReplyData(const QByteArray &replyData);
private:
    QByteArray m_replyData;
protected:
    void run();
signals:
    void done(const QList<Torrent> &torrents, const QList<int> &torrentIds);
};

class TorrentModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Role)
    Q_PROPERTY(TorrentFileModel* fileModel READ fileModel WRITE setFileModel)
    Q_PROPERTY(TorrentPeerModel* peerModel READ peerModel WRITE setPeerModel)
    Q_PROPERTY(TorrentTrackerModel* trackerModel READ trackerModel WRITE setTrackerModel)
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

    TorrentModel();
    ~TorrentModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void beginUpdateModel(const QByteArray &replyData);
    void resetModel();

    TorrentFileModel* fileModel() const;
    TorrentPeerModel* peerModel() const;
    TorrentTrackerModel* trackerModel() const;

    void setFileModel(TorrentFileModel *fileModel);
    void setPeerModel(TorrentPeerModel *peerModel);
    void setTrackerModel(TorrentTrackerModel *trackerModel);

    Q_INVOKABLE void removeAtIndex(int index);
    Q_INVOKABLE QString formatEta(int eta) const;

    Q_INVOKABLE void loadFileModel (int index);
    Q_INVOKABLE void loadPeerModel (int index);
    Q_INVOKABLE void loadTrackerModel (int index);
public slots:
    void endUpdateModel(const QList<Torrent> &torrents, const QList<int> torrentIds);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void finishedNotification(QString torrentName);

    QList<Torrent> m_torrents;
    QList<int> m_torrentIds;

    TorrentFileModel *m_fileModel;
    TorrentPeerModel *m_peerModel;
    TorrentTrackerModel *m_trackerModel;

    TorrentModelWorker *m_worker;
    bool m_changingModel;
signals:
    void modelChanged();
    void torrentRemoved();
};

#endif // TORRENTMODEL_H
