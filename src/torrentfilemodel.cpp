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

#include "torrentfilemodel.h"
#include "torrentfilemodel.moc"

#include <QMutexLocker>
#include <QThread>

#include "transmission.h"

namespace Tremotesf
{

TorrentFile::TorrentFile(TorrentFile *parentDirectory, int row)
    : parentDirectory(parentDirectory),
      row(row)
{

}

TorrentFile::~TorrentFile()
{
    qDeleteAll(childFiles);
}

TorrentFileModelWorker::TorrentFileModelWorker(TorrentFile *rootDirectory, QMutex *mutex)
    : m_rootDirectory(rootDirectory),
      m_mutex(mutex)
{

}

void TorrentFileModelWorker::reset()
{
    m_files.clear();
    m_filePaths.clear();
}

void TorrentFileModelWorker::beginWork(QVariantList fileVariants, QVariantList fileStatsVariants)
{
    m_changedFiles.clear();

    QStringList filePaths;
    for (int i = 0, filesCount = fileVariants.size(); i < filesCount; i++)
        filePaths.append(fileVariants.at(i).toMap().value("name").toString());

    QMutexLocker locker(m_mutex);

    if (m_filePaths == filePaths) {
        updateTree(fileStatsVariants);
    } else {
        m_filePaths = filePaths;
        createTree(fileVariants, fileStatsVariants);
    }

}

void TorrentFileModelWorker::createTree(const QVariantList &fileVariants, const QVariantList &fileStatsVariants)
{
    m_files.clear();
    qDeleteAll(m_rootDirectory->childFiles);
    m_rootDirectory->childFiles.clear();

    for (int i = 0, filesCount = fileVariants.size(); i < filesCount; i++) {
        TorrentFile *currentFile = m_rootDirectory;
        QVariantMap fileMap = fileVariants.at(i).toMap();
        QVariantMap fileStatsMap = fileStatsVariants.at(i).toMap();

        QString filePath = m_filePaths.at(i);
        QStringList parts = filePath.split('/', QString::SkipEmptyParts);
        for (int j = 0, partsCount = parts.size(); j < partsCount; j++) {
            bool isFile = (j == (parts.length() - 1));

            QString part = parts.at(j);

            if (!currentFile->childFiles.contains(part)) {
                TorrentFile *child = new TorrentFile(currentFile, currentFile->childFiles.count());
                currentFile->childFiles.insert(part, child);
                currentFile = child;
                if (isFile)
                    m_files.append(currentFile);
            } else {
                currentFile = currentFile->childFiles.value(part);
            }

            currentFile->isDirectory = !isFile;
            currentFile->name = part;
            if (isFile) {
                currentFile->bytesCompleted = fileStatsMap.value("bytesCompleted").toLongLong();
                currentFile->length = fileMap.value("length").toLongLong();
                currentFile->progress = qRound(10000.0 * currentFile->bytesCompleted / currentFile->length) / 100.0;
                currentFile->priority = fileStatsMap.value("priority").toInt();
                currentFile->wantedStatus = fileStatsMap.value("wanted").toBool() ? TorrentFileModel::AllWanted :
                                                                                    TorrentFileModel::NoWanted;
                currentFile->fileIndex = i;
            }
        }
    }

    for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            setDirectory(iterator.value());
    }

    emit treeCreated();
}

void TorrentFileModelWorker::updateTree(const QVariantList &fileStatsVariants)
{
    for (int i = 0, filesCount = fileStatsVariants.size(); i < filesCount; i++) {
        QVariantMap fileStatsMap = fileStatsVariants.at(i).toMap();
        TorrentFile *file = m_files.at(i);
        bool changed = false;

        qint64 bytesCompleted = fileStatsMap.value("bytesCompleted").toLongLong();
        if (bytesCompleted != file->bytesCompleted) {
            file->bytesCompleted = bytesCompleted;
            file->progress = qRound(10000.0 * bytesCompleted / file->length) / 100.0;
            changed = true;
        }

        int priority = fileStatsMap.value("priority").toInt();
        if (priority != file->priority) {
            file->priority = priority;
            changed = true;
        }

        int wantedStatus = fileStatsMap.value("wanted").toBool() ? TorrentFileModel::AllWanted :
                                                                   TorrentFileModel::NoWanted;

        if (wantedStatus != file->wantedStatus) {
            file->wantedStatus = wantedStatus;
            changed = true;
        }

        if (changed)
            m_changedFiles.append(file);
    }

    for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectory(iterator.value());
    }

    emit treeUpdated(m_changedFiles);
}

void TorrentFileModelWorker::setDirectory(TorrentFile *directory)
{
    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            setDirectory(iterator.value());
    }

    qint64 bytesCompleted = 0;
    qint64 length = 0;
    int priority = directory->childFiles.first()->priority;
    int wantedStatus = directory->childFiles.first()->wantedStatus;

    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        TorrentFile *file = iterator.value();

        bytesCompleted += file->bytesCompleted;
        length += file->length;
        if (file->priority != priority)
            priority = TorrentFileModel::MixedPriority;
        if (file->wantedStatus != wantedStatus)
            wantedStatus = TorrentFileModel::SomeWanted;
    }

    directory->bytesCompleted = bytesCompleted;
    directory->length = length;
    directory->progress = qRound(10000.0 * bytesCompleted / length) / 100.0;
    directory->priority = priority;
    directory->wantedStatus = wantedStatus;
}

void TorrentFileModelWorker::updateDirectory(TorrentFile *directory)
{
    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectory(iterator.value());
    }

    bool changed = false;
    qint64 bytesCompleted = 0;
    int priority = directory->childFiles.first()->priority;
    int wantedStatus = directory->childFiles.first()->wantedStatus;

    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        TorrentFile *file = iterator.value();

        bytesCompleted += file->bytesCompleted;
        if (file->priority != priority)
            priority = TorrentFileModel::MixedPriority;
        if (file->wantedStatus != wantedStatus)
            wantedStatus = TorrentFileModel::SomeWanted;
    }

    if (bytesCompleted != directory->bytesCompleted) {
        directory->bytesCompleted = bytesCompleted;
        directory->progress = qRound(10000.0 * bytesCompleted / directory->length) / 100.0;
        changed = true;
    }

    if (priority != directory->priority) {
        directory->priority = priority;
        changed = true;
    }

    if (wantedStatus != directory->wantedStatus) {
        directory->wantedStatus = wantedStatus;
        changed = true;
    }

    if (changed)
        m_changedFiles.append(directory);
}

TorrentFileModel::TorrentFileModel()
    : m_rootDirectory(new TorrentFile),
      m_worker(new TorrentFileModelWorker(m_rootDirectory, &m_mutex)),
      m_workerThread(new QThread(this)),
      m_active(false)
{
    qRegisterMetaType< QList<TorrentFile*> >();

    connect(this, &TorrentFileModel::requestModelUpdate, m_worker, &TorrentFileModelWorker::beginWork);
    connect(m_worker, &TorrentFileModelWorker::treeCreated, this, &TorrentFileModel::endCreateTree);
    connect(m_worker, &TorrentFileModelWorker::treeUpdated, this, &TorrentFileModel::endUpdateTree);

    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);
}

TorrentFileModel::~TorrentFileModel()
{
    m_workerThread->quit();
    m_workerThread->wait();

    m_mutex.lock();
    delete m_rootDirectory;
    m_mutex.unlock();
}

int TorrentFileModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant TorrentFileModel::data(const QModelIndex &modelIndex, int role) const
{
    if (!modelIndex.isValid())
        return QVariant();

    TorrentFile *file = static_cast<TorrentFile*>(modelIndex.internalPointer());

    switch (role) {
    case BytesCompletedRole:
        return file->bytesCompleted;
    case FileIndexRole:
        return file->fileIndex;
    case IsDirectoryRole:
        return file->isDirectory;
    case LengthRole:
        return file->length;
    case NameRole:
        return file->name;
    case PathRole:
        return file->path;
    case PriorityRole:
        return file->priority;
    case ProgressRole:
        return file->progress;
    case WantedStatusRole:
        return file->wantedStatus;
    default:
        return QVariant();
    }
}

QModelIndex TorrentFileModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TorrentFile *parentDirectory;
    if (parent.isValid())
        parentDirectory = static_cast<TorrentFile*>(parent.internalPointer());
    else
        parentDirectory = m_rootDirectory;

    return createIndex(row, column, parentDirectory->childFiles.values().at(row));
}

QModelIndex TorrentFileModel::parent(const QModelIndex &modelIndex) const
{
    if (!modelIndex.isValid())
        return QModelIndex();

    TorrentFile *parentDirectory = static_cast<TorrentFile*>(modelIndex.internalPointer())->parentDirectory;
    if (parentDirectory == m_rootDirectory)
        return QModelIndex();

    return createIndex(parentDirectory->row, 0, parentDirectory);
}

int TorrentFileModel::rowCount(const QModelIndex &parent) const
{
    TorrentFile *directory;

    if (parent.isValid())
        directory = static_cast<TorrentFile*>(parent.internalPointer());
    else
        directory = m_rootDirectory;

    return directory->childFiles.count();
}

bool TorrentFileModel::setData(const QModelIndex &modelIndex, const QVariant &value, int role)
{
    if (!modelIndex.isValid())
        return false;

    QMutexLocker locker(&m_mutex);

    TorrentFile *file = static_cast<TorrentFile*>(modelIndex.internalPointer());

    if (role == PriorityRole) {
        setFilePriority(file, value.toInt());
        return true;
    }
    if (role == WantedStatusRole) {
        setFileWantedStatus(file, value.toInt());
        return true;
    }
    return false;
}

bool TorrentFileModel::isActive() const
{
    return m_active;
}

void TorrentFileModel::setActive(bool active)
{
    m_active = active;
}

int TorrentFileModel::torrentId() const
{
    return m_torrentId;
}

void TorrentFileModel::setTorrentId(int id)
{
    m_torrentId = id;
}

Transmission* TorrentFileModel::transmission() const
{
    return m_transmission;
}

void TorrentFileModel::setTransmission(Transmission *newTransmission)
{
    m_transmission = newTransmission;
}

void TorrentFileModel::setAllWanted(bool wanted)
{
    m_mutex.lock();

    if (wanted) {
        for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
             iterator != cend;
             iterator++) {

            setFileWantedStatusRecursively(iterator.value(), AllWanted);
        }
    } else {
        for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
             iterator != cend;
             iterator++) {

            setFileWantedStatusRecursively(iterator.value(), NoWanted);
        }
    }

    for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectoryWantedStatus(iterator.value());
    }

    if (wanted)
        m_transmission->changeTorrent(m_torrentId, "files-wanted", QVariantList());
    else
        m_transmission->changeTorrent(m_torrentId, "files-unwanted", QVariantList());

    m_mutex.unlock();
}

void TorrentFileModel::beginUpdateModel(const QVariantList &fileVariants, const QVariantList &fileStatsVariants)
{
    emit requestModelUpdate(fileVariants, fileStatsVariants);
}

void TorrentFileModel::resetModel()
{
    m_mutex.lock();

    beginResetModel();
    qDeleteAll(m_rootDirectory->childFiles);
    m_rootDirectory->childFiles.clear();
    endResetModel();

    m_worker->reset();

    m_active = false;

    m_mutex.unlock();
}

void TorrentFileModel::endCreateTree()
{
    beginResetModel();
    endResetModel();
}

void TorrentFileModel::endUpdateTree(QList<TorrentFile*> changedFiles)
{
    for (QList<TorrentFile*>::const_iterator iterator = changedFiles.cbegin(), cend = changedFiles.cend();
         iterator != cend;
         iterator++) {

        QModelIndex modelIndex = createIndex((*iterator)->row, 0, *iterator);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void TorrentFileModel::setFilePriority(TorrentFile *file, int priority)
{
    setFilePriorityRecursively(file, priority);

    for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectoryPriority(iterator.value());
    }

    QString key;
    if (priority == LowPriority)
        key = "priority-low";
    else if (priority == NormalPriority)
        key = "priority-normal";
    else
        key = "priority-high";

    m_transmission->changeTorrent(m_torrentId, key, getSubtreeFileIndexes(file));
}

void TorrentFileModel::setFilePriorityRecursively(TorrentFile *file, int priority)
{
    if (file->isDirectory) {
        for (QMap<QString, TorrentFile*>::const_iterator iterator = file->childFiles.cbegin(), cend = file->childFiles.cend();
             iterator != cend;
             iterator++) {

            setFilePriorityRecursively(iterator.value(), priority);
        }
    } else {
        file->priority = priority;

        QModelIndex modelIndex = createIndex(file->row, 0, file);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << PriorityRole);
    }
}

void TorrentFileModel::updateDirectoryPriority(TorrentFile *directory)
{
    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectoryPriority(iterator.value());
    }

    int priority = directory->childFiles.first()->priority;

    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->priority != priority) {
            priority = TorrentFileModel::MixedPriority;
            break;
        }
    }

    if (priority != directory->priority) {
        directory->priority = priority;
        QModelIndex modelIndex = createIndex(directory->row, 0, directory);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << PriorityRole);
    }
}

void TorrentFileModel::setFileWantedStatus(TorrentFile *file, int wantedStatus)
{
    setFileWantedStatusRecursively(file, wantedStatus);

    for (QMap<QString, TorrentFile*>::const_iterator iterator = m_rootDirectory->childFiles.cbegin(), cend = m_rootDirectory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectoryWantedStatus(iterator.value());
    }

    QString key;
    if (wantedStatus == NoWanted)
        key = "files-unwanted";
    else
        key = "files-wanted";

    m_transmission->changeTorrent(m_torrentId, key, getSubtreeFileIndexes(file));
}

void TorrentFileModel::setFileWantedStatusRecursively(TorrentFile *file, int wantedStatus)
{
    if (file->isDirectory) {
        for (QMap<QString, TorrentFile*>::const_iterator iterator = file->childFiles.cbegin(), cend = file->childFiles.cend();
             iterator != cend;
             iterator++) {

            setFileWantedStatusRecursively(iterator.value(), wantedStatus);
        }
    } else {
        file->wantedStatus = wantedStatus;

        QModelIndex modelIndex = createIndex(file->row, 0, file);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << WantedStatusRole);
    }
}

void TorrentFileModel::updateDirectoryWantedStatus(TorrentFile *directory)
{
    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->isDirectory)
            updateDirectoryWantedStatus(iterator.value());
    }

    int wantedStatus = directory->childFiles.first()->wantedStatus;

    for (QMap<QString, TorrentFile*>::const_iterator iterator = directory->childFiles.cbegin(), cend = directory->childFiles.cend();
         iterator != cend;
         iterator++) {

        if (iterator.value()->wantedStatus != wantedStatus) {
            wantedStatus = TorrentFileModel::SomeWanted;
            break;
        }
    }

    if (wantedStatus != directory->wantedStatus) {
        directory->wantedStatus = wantedStatus;
        QModelIndex modelIndex = createIndex(directory->row, 0, directory);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << WantedStatusRole);
    }
}

QVariantList TorrentFileModel::getSubtreeFileIndexes(const TorrentFile *file) {
    QVariantList fileIndexes;

    if (file->isDirectory) {
        for (QMap<QString, TorrentFile*>::const_iterator iterator = file->childFiles.cbegin(), cend = file->childFiles.cend();
             iterator != cend;
             iterator++) {

            fileIndexes.append(getSubtreeFileIndexes(iterator.value()));
        }
    } else {
        fileIndexes.append(file->fileIndex);
    }

    return fileIndexes;
}

QHash<int, QByteArray> TorrentFileModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles.insert(BytesCompletedRole, "bytesCompleted");
    roles.insert(FileIndexRole, "fileIndex");
    roles.insert(IsDirectoryRole, "isDirectory");
    roles.insert(LengthRole, "length");
    roles.insert(NameRole, "name");
    roles.insert(PathRole, "path");
    roles.insert(PriorityRole, "priority");
    roles.insert(ProgressRole, "progress");
    roles.insert(WantedStatusRole, "wantedStatus");

    return roles;
}

}
