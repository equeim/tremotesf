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

#include "torrentmodel.h"
#include "torrentmodel.moc"

#include <QJsonDocument>
#include <QMutex>
#include <QThread>

#include <QDebug>

#include "torrentfilemodel.h"
#include "torrentpeermodel.h"
#include "torrenttrackermodel.h"
#include "transmission.h"
#include "utils.h"

Torrent::Torrent(const QString &hashString, int id, const QString &name)
{
    this->hashString = hashString;
    this->id = id;
    this->name = name;

    finished = false;
}

void Torrent::update(const QVariantMap &torrentMap)
{
    changed = false;
    finished = false;

    QDateTime activityDate = QDateTime::fromTime_t(torrentMap.value("activityDate").toInt());
    if (activityDate != this->activityDate) {
        this->activityDate = activityDate;
        changed = true;
    }

    QDateTime addedDate = QDateTime::fromTime_t(torrentMap.value("addedDate").toInt());
    if (addedDate != this->addedDate) {
        this->addedDate = addedDate;
        changed = true;
    }

    int bandwidthPriority = torrentMap.value("bandwidthPriority").toInt();
    if (bandwidthPriority != this->bandwidthPriority) {
        this->bandwidthPriority = bandwidthPriority;
        changed = true;
    }


    QString downloadDir = torrentMap.value("downloadDir").toString();
    if (downloadDir != this->downloadDir) {
        this->downloadDir = downloadDir;
        changed = true;
    }

    int downloadLimit = torrentMap.value("downloadLimit").toInt();
    if (downloadLimit != this->downloadLimit) {
        this->downloadLimit = downloadLimit;
        changed = true;
    }

    bool downloadLimited = torrentMap.value("downloadLimited").toBool();
    if (downloadLimited != this->downloadLimited) {
        this->downloadLimited = downloadLimited;
        changed = true;
    }

    QString errorString = torrentMap.value("errorString").toString();
    if (errorString != this->errorString) {
        this->errorString = errorString;
        changed = true;
    }

    int eta = torrentMap.value("eta").toInt();
    if (eta != this->eta) {
        this->eta = eta;
        changed = true;
    }

    bool honorsSessionLimits = torrentMap.value("honorsSessionLimits").toBool();
    if (honorsSessionLimits != this->honorsSessionLimits) {
        this->honorsSessionLimits = honorsSessionLimits;
        changed = true;
    }

    qint64 leftUntilDone = torrentMap.value("leftUntilDone").toLongLong();
    if (leftUntilDone != this->leftUntilDone) {
        this->leftUntilDone = leftUntilDone;
        changed = true;
    }

    int peerLimit = torrentMap.value("peer-limit").toInt();
    if (peerLimit != this->peerLimit) {
        this->peerLimit = peerLimit;
        changed = true;
    }

    int peersConnected = torrentMap.value("peersConnected").toInt();
    if (peersConnected != this->peersConnected) {
        this->peersConnected = peersConnected;
        changed = true;
    }

    int peersGettingFromUs = torrentMap.value("peersGettingFromUs").toInt();
    if (peersGettingFromUs != this->peersGettingFromUs) {
        this->peersGettingFromUs = peersGettingFromUs;
        changed = true;
    }

    int peersSendingToUs = torrentMap.value("peersSendingToUs").toInt();
    if (peersSendingToUs != this->peersSendingToUs) {
        this->peersSendingToUs = peersSendingToUs;
        changed = true;
    }

    float percentDone = torrentMap.value("percentDone").toFloat() * 100.0;
    if (percentDone != this->percentDone) {
        if (percentDone == 100.0)
            finished = true;
        this->percentDone = percentDone;
        changed = true;
    }

    int rateDownload = torrentMap.value("rateDownload").toInt();
    if (rateDownload != this->rateDownload) {
        this->rateDownload = rateDownload;
        changed = true;
    }

    int rateUpload = torrentMap.value("rateUpload").toInt();
    if (rateUpload != this->rateUpload) {
        this->rateUpload = rateUpload;
        changed = true;
    }

    float recheckProgress = torrentMap.value("recheckProgress").toFloat() * 100.0;
    if (recheckProgress != this->recheckProgress) {
        this->recheckProgress = recheckProgress;
        changed = true;
    }

    float seedRatioLimit = torrentMap.value("seedRatioLimit").toFloat();
    if (seedRatioLimit != this->seedRatioLimit) {
        this->seedRatioLimit = seedRatioLimit;
        changed = true;
    }

    int seedRatioMode = torrentMap.value("seedRatioMode").toInt();
    if (seedRatioMode != this->seedRatioMode) {
        this->seedRatioMode = seedRatioMode;
        changed = true;
    }

    qint64 sizeWhenDone = torrentMap.value("sizeWhenDone").toLongLong();
    if (sizeWhenDone != this->sizeWhenDone) {
        this->sizeWhenDone = sizeWhenDone;
        changed = true;
    }

    int status = torrentMap.value("status").toInt();
    if (status != this->status) {
        this->status = status;
        changed = true;
    }

    qint64 totalSize = torrentMap.value("totalSize").toLongLong();
    if (totalSize != this->totalSize) {
        this->totalSize = totalSize;
        changed = true;
    }

    qint64 uploadedEver = torrentMap.value("uploadedEver").toLongLong();
    if (uploadedEver != this->uploadedEver) {
        this->uploadedEver = uploadedEver;
        changed = true;
    }

    int uploadLimit = torrentMap.value("uploadLimit").toInt();
    if (uploadLimit != this->uploadLimit) {
        this->uploadLimit = uploadLimit;
        changed = true;
    }

    bool uploadLimited = torrentMap.value("uploadLimited").toBool();
    if (uploadLimited != this->uploadLimited) {
        this->uploadLimited = uploadLimited;
        changed = true;
    }

    float uploadRatio = torrentMap.value("uploadRatio").toFloat();
    if (uploadRatio != this->uploadRatio) {
        this->uploadRatio = uploadRatio;
        changed = true;
    }

    fileList = torrentMap.value("files").toList();
    fileStatsList = torrentMap.value("fileStats").toList();
    peerList = torrentMap.value("peers").toList();
    trackerList = torrentMap.value("trackerStats").toList();
}

TorrentModelWorker::TorrentModelWorker(const QList<Torrent*> *torrents, const QList<int> *torrentIds)
{
    m_torrents = torrents;
    m_torrentIds = torrentIds;
}

void TorrentModelWorker::doWork(const QByteArray &replyData)
{
    QList<Torrent*> newTorrents;
    QList<int> newTorrentIds;

    QVariantList torrentList = QJsonDocument::fromJson(replyData).toVariant()
            .toMap()
            .value("arguments")
            .toMap()
            .value("torrents")
            .toList();

    for (int i = 0; i < torrentList.length(); i++) {
        QVariantMap torrentMap = torrentList.at(i).toMap();

        Torrent *torrent;
        int id = torrentMap.value("id").toInt();
        int index = m_torrentIds->indexOf(id);
        if (index == -1)
            torrent = new Torrent(torrentMap.value("hashString").toString(),
                                  id,
                                  torrentMap.value("name").toString());
        else
            torrent = m_torrents->at(index);
        newTorrents.append(torrent);
        newTorrentIds.append(id);
        torrent->update(torrentMap);
    }

    emit done(newTorrents, newTorrentIds);
}

TorrentModel::TorrentModel()
{
    qRegisterMetaType< QList<Torrent*> >();

    m_worker = new TorrentModelWorker(&m_torrents, &m_torrentIds);
    connect(this, &TorrentModel::requestModelUpdate, m_worker, &TorrentModelWorker::doWork);
    connect(m_worker, &TorrentModelWorker::done, this, &TorrentModel::endUpdateModel);

    m_workerThread = new QThread(this);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);
}

TorrentModel::~TorrentModel()
{
    m_workerThread->quit();
    m_workerThread->wait();

    m_mutex.lock();
    qDeleteAll(m_torrents);
    m_mutex.unlock();
}

QVariant TorrentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Torrent *torrent = m_torrents.at(index.row());

    switch (role) {
    case ActivityDateRole:
        return torrent->activityDate;
    case AddedDateRole:
        return torrent->addedDate;
    case BandwidthPriorityRole:
        return torrent->bandwidthPriority;
    case DownloadDirRole:
        return torrent->downloadDir;
    case DownloadLimitRole:
        return torrent->downloadLimit;
    case DownloadLimitedRole:
        return torrent->downloadLimited;
    case ErrorStringRole:
        return torrent->errorString;
    case EtaRole:
        return torrent->eta;
    case HashStringRole:
        return torrent->hashString;
    case HonorsSessionLimitsRole:
        return torrent->honorsSessionLimits;
    case IdRole:
        return torrent->id;
    case LeftUntilDoneRole:
        return torrent->leftUntilDone;
    case NameRole:
        return torrent->name;
    case PeerLimitRole:
        return torrent->peerLimit;
    case PeersConnectedRole:
        return torrent->peersConnected;
    case PeersGettingFromUsRole:
        return torrent->peersGettingFromUs;
    case PeersSendingToUsRole:
        return torrent->peersSendingToUs;
    case PercentDoneRole:
        return torrent->percentDone;
    case RateDownloadRole:
        return torrent->rateDownload;
    case RateUploadRole:
        return torrent->rateUpload;
    case RecheckProgressRole:
        return torrent->recheckProgress;
    case SeedRatioLimitRole:
        return torrent->seedRatioLimit;
    case SeedRatioModeRole:
        return torrent->seedRatioMode;
    case SizeWhenDoneRole:
        return torrent->sizeWhenDone;
    case StatusRole:
        return torrent->status;
    case TotalSizeRole:
        return torrent->totalSize;
    case UploadedEverRole:
        return torrent->uploadedEver;
    case UploadLimitRole:
        return torrent->uploadLimit;
    case UploadLimitedRole:
        return torrent->uploadLimited;
    case UploadRatioRole:
        return torrent->uploadRatio;
    default:
        return QVariant();
    }
}

int TorrentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_torrents.length();
}

bool TorrentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    QMutexLocker locker(&m_mutex);

    Torrent *torrent = m_torrents.at(index.row());

    switch (role) {
    case BandwidthPriorityRole:
        setTorrentBandwidthPriority(torrent, index, value.toInt());
        return true;
    case DownloadLimitRole:
        setTorrentDownloadLimit(torrent, index, value.toInt());
        return true;
    case DownloadLimitedRole:
        setTorrentDownloadLimited(torrent, index, value.toBool());
        return true;
    case HonorsSessionLimitsRole:
        setTorrentHonorsSessionLimits(torrent, index, value.toBool());
        return true;
    case PeerLimitRole:
        setTorrentPeerLimit(torrent, index, value.toInt());
        return true;
    case SeedRatioLimitRole:
        setTorrentSeedRatioLimit(torrent, index, value.toFloat());
        return true;
    case SeedRatioModeRole:
        setTorrentSeedRatioMode(torrent, index, value.toInt());
        return true;
    case StatusRole:
        setTorrentStatus(torrent, index, value.toInt());
        return true;
    case UploadLimitRole:
        setTorrentUploadLimit(torrent, index, value.toInt());
        return true;
    case UploadLimitedRole:
        setTorrentUploadLimited(torrent, index, value.toBool());
        return true;
    default:
        return false;
    }
}

void TorrentModel::resetModel()
{
    m_mutex.lock();

    beginResetModel();
    qDeleteAll(m_torrents);
    m_torrents.clear();
    m_torrentIds.clear();
    endResetModel();

    if (m_fileModel->isActive())
        m_fileModel->resetModel();

    if (m_peerModel->isActive())
        m_fileModel->resetModel();

    if (m_trackerModel->isActive())
        m_trackerModel->resetModel();

    m_mutex.unlock();
}

TorrentFileModel* TorrentModel::fileModel() const
{
    return m_fileModel;
}

TorrentPeerModel* TorrentModel::peerModel() const
{
    return m_peerModel;
}

TorrentTrackerModel* TorrentModel::trackerModel() const
{
    return m_trackerModel;
}

Transmission* TorrentModel::transmission() const
{
    return m_transmission;
}

void TorrentModel::setFileModel(TorrentFileModel *fileModel)
{
    m_fileModel = fileModel;
}

void TorrentModel::setPeerModel(TorrentPeerModel *peerModel)
{
    m_peerModel = peerModel;
}

void TorrentModel::setTrackerModel(TorrentTrackerModel *trackerModel)
{
    m_trackerModel = trackerModel;
}

void TorrentModel::setTransmission(Transmission *transmission)
{
    m_transmission = transmission;
}

void TorrentModel::beginUpdateModel(const QByteArray &replyData)
{
    emit requestModelUpdate(replyData);
}

void TorrentModel::removeAtIndex(int index)
{
    if (index >= rowCount())
        return;

    m_mutex.lock();

    if (m_fileModel->isActive())
        if (m_fileModel->torrentId() == m_torrentIds.at(index))
            m_fileModel->resetModel();

    if (m_peerModel->isActive())
        if (m_peerModel->torrentId() == m_torrentIds.at(index))
            m_peerModel->resetModel();

    if (m_trackerModel->isActive())
        if (m_trackerModel->torrentId() == m_torrentIds.at(index))
            m_trackerModel->resetModel();

    beginRemoveRows(QModelIndex(), index, index);
    delete m_torrents.takeAt(index);
    m_torrentIds.removeAt(index);
    endRemoveRows();

    m_mutex.unlock();
}

void TorrentModel::loadFileModel(int index)
{
    m_fileModel->setIsActive(true);
    m_fileModel->setTorrentId(m_torrentIds.at(index));
    m_fileModel->beginUpdateModel(m_torrents.at(index)->fileList, m_torrents.at(index)->fileStatsList);
}

void TorrentModel::loadPeerModel(int index)
{
    m_peerModel->setIsActive(true);
    m_peerModel->setTorrentId(m_torrentIds.at(index));
    m_peerModel->beginUpdateModel(m_torrents.at(index)->peerList);
}

void TorrentModel::loadTrackerModel(int index)
{
    m_trackerModel->setIsActive(true);
    m_trackerModel->setTorrentId(m_torrentIds.at(index));
    m_trackerModel->beginUpdateModel(m_torrents.at(index)->trackerList);
}

void TorrentModel::endUpdateModel(const QList<Torrent*> &newTorrents, const QList<int> newTorrentIds)
{
    m_mutex.lock();

    qDebug() << "update";

    for (int i = 0; i < m_torrentIds.length(); i++) {
        int id = m_torrentIds.at(i);

        if (!newTorrentIds.contains(id)) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_torrents.takeAt(i);
            m_torrentIds.removeAt(i);
            endRemoveRows();

            i--;

            emit torrentRemoved();

            if (m_fileModel->isActive())
                if (m_fileModel->torrentId() == id)
                    m_fileModel->resetModel();

            if (m_peerModel->isActive())
                if (m_peerModel->torrentId() == id)
                    m_peerModel->resetModel();

            if (m_trackerModel->isActive())
                if (m_trackerModel->torrentId() == id)
                    m_trackerModel->resetModel();
        }
    }

    for (int i = 0; i < newTorrentIds.length(); i++) {
        int id = newTorrentIds.at(i);
        if (m_torrentIds.contains(id)) {
            int row = m_torrentIds.indexOf(id);
            Torrent *torrent = m_torrents.at(row);
            if (torrent->changed) {
                QModelIndex modelIndex = index(row, 0);
                emit dataChanged(modelIndex, modelIndex);

                if (torrent->finished)
                    Utils::publishFinishedNotification(torrent->name);
            }
        } else {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_torrents.append(newTorrents.at(i));
            m_torrentIds.append(id);
            endInsertRows();
        }
    }

    if (m_fileModel->isActive()) {
        int index = m_torrentIds.indexOf(m_fileModel->torrentId());
        m_fileModel->beginUpdateModel(m_torrents.at(index)->fileList, m_torrents.at(index)->fileStatsList);
    }

    if (m_peerModel->isActive()) {
        int index = m_torrentIds.indexOf(m_peerModel->torrentId());
        m_peerModel->beginUpdateModel(m_torrents.at(index)->peerList);
    }

    if (m_trackerModel->isActive()) {
        int index = m_torrentIds.indexOf(m_trackerModel->torrentId());
        m_trackerModel->beginUpdateModel(m_torrents.at(index)->trackerList);
    }

    m_mutex.unlock();
}

void TorrentModel::setTorrentBandwidthPriority(Torrent *torrent, const QModelIndex &index, int bandwidthPriority)
{
    torrent->bandwidthPriority = bandwidthPriority;
    emit dataChanged(index, index, QVector<int>() << BandwidthPriorityRole);
    m_transmission->changeTorrent(torrent->id, "bandwidthPriority", bandwidthPriority);
}

void TorrentModel::setTorrentDownloadLimit(Torrent *torrent, const QModelIndex &index, int downloadLimit)
{
    torrent->downloadLimit = downloadLimit;
    emit dataChanged(index, index, QVector<int>() << DownloadLimitRole);
    m_transmission->changeTorrent(torrent->id, "downloadLimit", downloadLimit);
}

void TorrentModel::setTorrentDownloadLimited(Torrent *torrent, const QModelIndex &index, bool downloadLimited)
{
    torrent->downloadLimited = downloadLimited;
    emit dataChanged(index, index, QVector<int>() << DownloadLimitedRole);
    m_transmission->changeTorrent(torrent->id, "downloadLimited", downloadLimited);
}

void TorrentModel::setTorrentHonorsSessionLimits(Torrent *torrent, const QModelIndex &index, bool honorsSessionLimits)
{
    torrent->honorsSessionLimits = honorsSessionLimits;
    emit dataChanged(index, index, QVector<int>() << HonorsSessionLimitsRole);
    m_transmission->changeTorrent(torrent->id, "honorsSessionLimits", honorsSessionLimits);
}

void TorrentModel::setTorrentPeerLimit(Torrent *torrent, const QModelIndex &index, int peerLimit)
{
    torrent->peerLimit = peerLimit;
    emit dataChanged(index, index, QVector<int>() << PeerLimitRole);
    m_transmission->changeTorrent(torrent->id, "peer-limit", peerLimit);
}

void TorrentModel::setTorrentSeedRatioLimit(Torrent *torrent, const QModelIndex &index, float seedRatioLimit)
{
    torrent->seedRatioLimit = seedRatioLimit;
    emit dataChanged(index, index, QVector<int>() << SeedRatioLimitRole);
    m_transmission->changeTorrent(torrent->id, "seedRatioLimit", seedRatioLimit);
}

void TorrentModel::setTorrentSeedRatioMode(Torrent *torrent, const QModelIndex &index, int seedRatioMode)
{
    torrent->seedRatioMode = seedRatioMode;
    emit dataChanged(index, index, QVector<int>() << SeedRatioModeRole);
    m_transmission->changeTorrent(torrent->id, "seedRatioMode", seedRatioMode);
}

void TorrentModel::setTorrentStatus(Torrent *torrent, const QModelIndex &index, int status)
{
    torrent->status = status;
    emit dataChanged(index, index, QVector<int>() << StatusRole);

    if (status == StoppedStatus)
        m_transmission->stopTorrent(torrent->id);
    else if (status == QueuedForCheckingStatus)
        m_transmission->verifyTorrent(torrent->id);
    else if (status == DownloadingStatus ||
             status == QueuedForSeedingStatus)
        m_transmission->startTorrent(torrent->id);
}

void TorrentModel::setTorrentUploadLimit(Torrent *torrent, const QModelIndex &index, int uploadLimit)
{
    torrent->uploadLimit = uploadLimit;
    emit dataChanged(index, index, QVector<int>() << UploadLimitRole);
    m_transmission->changeTorrent(torrent->id, "uploadLimit", uploadLimit);
}

void TorrentModel::setTorrentUploadLimited(Torrent *torrent, const QModelIndex &index, bool uploadLimited)
{
    torrent->downloadLimited = uploadLimited;
    emit dataChanged(index, index, QVector<int>() << UploadLimitedRole);
    m_transmission->changeTorrent(torrent->id, "downloadLimited", uploadLimited);
}

QHash<int, QByteArray> TorrentModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(ActivityDateRole, "activityDate");
    roles.insert(AddedDateRole, "addedDate");
    roles.insert(BandwidthPriorityRole, "bandwidthPriority");
    roles.insert(DownloadDirRole, "downloadDir");
    roles.insert(DownloadLimitRole, "downloadLimit");
    roles.insert(DownloadLimitedRole, "downloadLimited");
    roles.insert(ErrorStringRole, "errorString");
    roles.insert(EtaRole, "eta");
    roles.insert(HashStringRole, "hashString");
    roles.insert(HonorsSessionLimitsRole, "honorsSessionLimits");
    roles.insert(IdRole, "id");
    roles.insert(LeftUntilDoneRole, "leftUntilDone");
    roles.insert(NameRole, "name");
    roles.insert(PeerLimitRole, "peerLimit");
    roles.insert(PeersConnectedRole, "peersConnected");
    roles.insert(PeersGettingFromUsRole, "peersGettingFromUs");
    roles.insert(PeersSendingToUsRole, "peersSendingToUs");
    roles.insert(PercentDoneRole, "percentDone");
    roles.insert(RateDownloadRole, "rateDownload");
    roles.insert(RateUploadRole, "rateUpload");
    roles.insert(RecheckProgressRole, "recheckProgress");
    roles.insert(SeedRatioLimitRole, "seedRatioLimit");
    roles.insert(SeedRatioModeRole, "seedRatioMode");
    roles.insert(SizeWhenDoneRole, "sizeWhenDone");
    roles.insert(StatusRole, "status");
    roles.insert(TotalSizeRole, "totalSize");
    roles.insert(UploadedEverRole, "uploadedEver");
    roles.insert(UploadLimitRole, "uploadLimit");
    roles.insert(UploadLimitedRole, "uploadLimited");
    roles.insert(UploadRatioRole, "uploadRatio");

    return roles;
}
