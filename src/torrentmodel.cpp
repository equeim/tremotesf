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

#include <MNotification>
#include <MRemoteAction>
#include <QEventLoop>
#include <QDBusConnection>
#include <QGuiApplication>
#include <QJsonDocument>

#ifndef NDEBUG
#include <QDebug>
#endif

#include "torrentfilemodel.h"
#include "torrentpeermodel.h"
#include "torrenttrackermodel.h"

#include "torrentmodel.h"

TorrentModelWorker::TorrentModelWorker()
{

}

void TorrentModelWorker::setReplyData(const QByteArray &replyData)
{
    m_replyData = replyData;
}

void TorrentModelWorker::run() Q_DECL_OVERRIDE
{
    QVariantList torrentList = QJsonDocument::fromJson(m_replyData).toVariant().toMap().value("arguments").toMap().value("torrents").toList();
    QList<Torrent> torrents;
    QList<int> torrentIds;

    for (int i = 0; i < torrentList.length(); i++) {
        Torrent torrent;
        QVariantMap torrentMap = torrentList.at(i).toMap();

        torrent.activityDate = QDateTime::fromTime_t(torrentMap.value("activityDate").toInt());
        torrent.addedDate = QDateTime::fromTime_t(torrentMap.value("addedDate").toInt());
        torrent.bandwidthPriority = torrentMap.value("bandwidthPriority").toInt();
        torrent.downloadDir = torrentMap.value("downloadDir").toString();
        torrent.downloadLimit = torrentMap.value("downloadLimit").toInt();
        torrent.downloadLimited = torrentMap.value("downloadLimited").toBool();
        torrent.errorString = torrentMap.value("errorString").toString();
        torrent.eta = torrentMap.value("eta").toInt();
        torrent.hashString = torrentMap.value("hashString").toString();
        torrent.honorsSessionLimits = torrentMap.value("honorsSessionLimits").toBool();
        torrent.id = torrentMap.value("id").toInt();
        torrent.leftUntilDone = torrentMap.value("leftUntilDone").toLongLong();
        torrent.name = torrentMap.value("name").toString();
        torrent.peerLimit = torrentMap.value("peer-limit").toInt();
        torrent.peersConnected = torrentMap.value("peersConnected").toInt();
        torrent.peersGettingFromUs = torrentMap.value("peersGettingFromUs").toInt();
        torrent.peersSendingToUs = torrentMap.value("peersSendingToUs").toInt();
        torrent.percentDone = torrentMap.value("percentDone").toFloat() * 100;
        torrent.rateDownload = torrentMap.value("rateDownload").toInt();
        torrent.rateUpload = torrentMap.value("rateUpload").toInt();
        torrent.recheckProgress = torrentMap.value("recheckProgress").toFloat() * 100;
        torrent.seedRatioLimit = torrentMap.value("seedRatioLimit").toFloat();
        torrent.seedRatioMode = torrentMap.value("seedRatioMode").toInt();
        torrent.sizeWhenDone = torrentMap.value("sizeWhenDone").toLongLong();
        torrent.status = torrentMap.value("status").toInt();
        torrent.totalSize = torrentMap.value("totalSize").toLongLong();
        torrent.uploadedEver = torrentMap.value("uploadedEver").toLongLong();
        torrent.uploadLimit = torrentMap.value("uploadLimit").toInt();
        torrent.uploadLimited = torrentMap.value("uploadLimited").toBool();
        torrent.uploadRatio = torrentMap.value("uploadRatio").toFloat();

        torrent.fileList = torrentMap.value("files").toList();
        torrent.fileStatsList = torrentMap.value("fileStats").toList();
        torrent.peerList = torrentMap.value("peers").toList();
        torrent.trackerList = torrentMap.value("trackerStats").toList();

        torrents.append(torrent);
        torrentIds.append(torrent.id);
    }

    emit done(torrents, torrentIds);
}

TorrentModel::TorrentModel()
{
    qRegisterMetaType<Torrent>();
    qRegisterMetaType< QList<Torrent> >();

    qRegisterMetaType<TorrentPeer>();
    qRegisterMetaType< QList<TorrentPeer> >();

    qRegisterMetaType<TorrentTracker>();
    qRegisterMetaType< QList<TorrentTracker> >();

    m_worker = new TorrentModelWorker;
    connect(m_worker, &TorrentModelWorker::done, this, &TorrentModel::endUpdateModel);
    m_changingModel = false;
}

TorrentModel::~TorrentModel()
{
    m_worker->wait();
    m_worker->deleteLater();
}

QVariant TorrentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Torrent &torrent = m_torrents.at(index.row());

    switch (role) {
    case ActivityDateRole:
        return torrent.activityDate;
    case AddedDateRole:
        return torrent.addedDate;
    case BandwidthPriorityRole:
        return torrent.bandwidthPriority;
    case DownloadDirRole:
        return torrent.downloadDir;
    case DownloadLimitRole:
        return torrent.downloadLimit;
    case DownloadLimitedRole:
        return torrent.downloadLimited;
    case ErrorStringRole:
        return torrent.errorString;
    case EtaRole:
        return torrent.eta;
    case HashStringRole:
        return torrent.hashString;
    case HonorsSessionLimitsRole:
        return torrent.honorsSessionLimits;
    case IdRole:
        return torrent.id;
    case LeftUntilDoneRole:
        return torrent.leftUntilDone;
    case NameRole:
        return torrent.name;
    case PeerLimitRole:
        return torrent.peerLimit;
    case PeersConnectedRole:
        return torrent.peersConnected;
    case PeersGettingFromUsRole:
        return torrent.peersGettingFromUs;
    case PeersSendingToUsRole:
        return torrent.peersSendingToUs;
    case PercentDoneRole:
        return torrent.percentDone;
    case RateDownloadRole:
        return torrent.rateDownload;
    case RateUploadRole:
        return torrent.rateUpload;
    case RecheckProgressRole:
        return torrent.recheckProgress;
    case SeedRatioLimitRole:
        return torrent.seedRatioLimit;
    case SeedRatioModeRole:
        return torrent.seedRatioMode;
    case SizeWhenDoneRole:
        return torrent.sizeWhenDone;
    case StatusRole:
        return torrent.status;
    case TotalSizeRole:
        return torrent.totalSize;
    case UploadedEverRole:
        return torrent.uploadedEver;
    case UploadLimitRole:
        return torrent.uploadLimit;
    case UploadLimitedRole:
        return torrent.uploadLimited;
    case UploadRatioRole:
        return torrent.uploadRatio;
    default:
        return QVariant();
    }
}

int TorrentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_torrents.length();
}

void TorrentModel::beginUpdateModel(const QByteArray &replyData)
{
    m_worker->wait();
    m_worker->setReplyData(replyData);
    m_worker->start(QThread::LowPriority);
}

void TorrentModel::resetModel()
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginResetModel();
    m_torrents.clear();
    endResetModel();
    m_torrentIds.clear();

    if (m_fileModel->isActive())
        m_fileModel->resetModel();

    if (m_peerModel->isActive())
        m_fileModel->resetModel();

    if (m_trackerModel->isActive())
        m_trackerModel->resetModel();

    m_changingModel = false;
    emit modelChanged();
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

void TorrentModel::removeAtIndex(int index)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginRemoveRows(QModelIndex(), index, index);
    m_torrents.removeAt(index);
    endRemoveRows();

    if (m_fileModel->isActive())
        if (m_fileModel->torrentId() == m_torrentIds.at(index))
            m_fileModel->resetModel();

    if (m_peerModel->isActive())
        if (m_peerModel->torrentId() == m_torrentIds.at(index))
            m_peerModel->resetModel();

    if (m_trackerModel->isActive())
        if (m_trackerModel->torrentId() == m_torrentIds.at(index))
            m_trackerModel->resetModel();

    m_torrentIds.removeAt(index);

    m_changingModel = false;
    emit modelChanged();
}

QString TorrentModel::formatEta(int eta) const
{
    if(eta < 0)
        return "∞";

    QDateTime date1 = QDateTime::fromTime_t(0);
    date1 = date1.toUTC();
    QDateTime date2 = QDateTime::fromTime_t(eta);
    date2 = date2.toUTC();

    QString etaString;
    bool moreThanDay = false;
    bool moreThanHour = false;

    if(date1.daysTo(date2) > 0) {
        etaString = QString::number(date1.daysTo(date2)) + "d ";
        moreThanDay = true;
        moreThanHour = true;
    }
    if(date2.time().toString("h").toInt() > 0) {
        etaString += date2.time().toString("h") + tr(" h ");
        moreThanHour = true;
    }
    if(date2.time().toString("m").toInt() > 0 && !moreThanDay)
        etaString += date2.time().toString("m") + tr(" m ");
    if(date2.time().toString("s").toInt() > 0 && !moreThanHour)
        etaString += date2.time().toString("s") + tr(" s");

    return etaString;
}

void TorrentModel::loadFileModel(int index)
{
    m_fileModel->setIsActive(true);
    m_fileModel->setTorrentId(m_torrentIds.at(index));
    m_fileModel->beginUpdateModel(m_torrents.at(index).fileList, m_torrents.at(index).fileStatsList);
}

void TorrentModel::loadPeerModel(int index)
{
    m_peerModel->setIsActive(true);
    m_peerModel->setTorrentId(m_torrentIds.at(index));
    m_peerModel->beginUpdateModel(m_torrents.at(index).peerList);
}

void TorrentModel::loadTrackerModel(int index)
{
    m_trackerModel->setIsActive(true);
    m_trackerModel->setTorrentId(m_torrentIds.at(index));
    m_trackerModel->beginUpdateModel(m_torrents.at(index).trackerList);
}

void TorrentModel::endUpdateModel(const QList<Torrent> &torrents, const QList<int> torrentIds)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

#ifndef NDEBUG
    qDebug() << "update";
#endif

    for (int i = 0; i < m_torrentIds.length(); i++) {
        if (!torrentIds.contains(m_torrentIds.at(i))) {
            beginRemoveRows(QModelIndex(), i, i);
            m_torrents.removeAt(i);
            endRemoveRows();

            int torrentId = m_torrentIds.at(i);
            m_torrentIds.removeAt(i);

            if (m_fileModel->isActive())
                if (m_fileModel->torrentId() == torrentId)
                    m_fileModel->resetModel();

            if (m_peerModel->isActive())
                if (m_peerModel->torrentId() == torrentId)
                    m_peerModel->resetModel();

            if (m_trackerModel->isActive())
                if (m_trackerModel->torrentId() == torrentId)
                    m_trackerModel->resetModel();
        }
    }

    for (int i = 0; i < torrentIds.length(); i++) {
        if (!m_torrentIds.contains(torrentIds.at(i))) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            m_torrents.append(torrents.at(i));
            endInsertRows();

            m_torrentIds.append(torrentIds.at(i));
        } else {
            int index = m_torrentIds.indexOf(torrentIds.at(i));

            if (torrents.at(i).percentDone == 100 && m_torrents.at(index).percentDone != 100)
                finishedNotification(m_torrents.at(index).name);

            m_torrents[index] = torrents.at(i);
        }
    }
    m_torrentIds = torrentIds;

    emit dataChanged(index(0), index(rowCount() - 1));

    m_changingModel = false;
    emit modelChanged();

    if (m_fileModel->isActive()) {
        int index = m_torrentIds.indexOf(m_fileModel->torrentId());
        m_fileModel->beginUpdateModel(m_torrents.at(index).fileList, m_torrents.at(index).fileStatsList);
    }

    if (m_peerModel->isActive()) {
        int index = m_torrentIds.indexOf(m_peerModel->torrentId());
        m_peerModel->beginUpdateModel(m_torrents.at(index).peerList);
    }

    if (m_trackerModel->isActive()) {
        int index = m_torrentIds.indexOf(m_trackerModel->torrentId());
        m_trackerModel->beginUpdateModel(m_torrents.at(index).trackerList);
    }
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

void TorrentModel::finishedNotification(QString torrentName)
{
    if (QGuiApplication::applicationState() == Qt::ApplicationActive)
        return;

    MRemoteAction action(QDBusConnection::sessionBus().baseService(), "/", "harbour.tremotesf.DBusProxy", "activate");
    MNotification notification(MNotification::TransferCompleteEvent, tr("Torrent finished"), torrentName);
    notification.setAction(action);
    notification.setImage("icon-m-share");
    notification.publish();
}
