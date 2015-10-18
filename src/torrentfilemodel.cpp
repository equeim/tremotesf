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

TorrentFile::TorrentFile(TorrentFile *parentDirectory, int row)
{
    this->parentDirectory = parentDirectory;
    this->row = row;
}

TorrentFile::~TorrentFile()
{
    qDeleteAll(childFiles);
}

TorrentFileModelWorker::TorrentFileModelWorker(TorrentFile *rootDirectory, QMutex *mutex)
{
    m_rootDirectory = rootDirectory;
    m_mutex = mutex;
}

void TorrentFileModelWorker::reset()
{
    m_files.clear();
    m_filePaths.clear();
}

void TorrentFileModelWorker::beginWork(const QVariantList &fileList, const QVariantList &fileStatsList)
{
    m_changedFiles.clear();

    QStringList filePaths;
    for (int i = 0; i < fileList.length(); i++) {
        QVariantMap fileMap = fileList.at(i).toMap();
        filePaths.append(fileMap.value("name").toString());
    }

    QMutexLocker locker(m_mutex);

    if (m_filePaths == filePaths) {
        updateTree(fileStatsList);
    } else {
        m_filePaths = filePaths;
        createTree(fileList, fileStatsList);
    }

}

void TorrentFileModelWorker::createTree(const QVariantList &fileList, const QVariantList &fileStatsList)
{
    m_files.clear();
    qDeleteAll(m_rootDirectory->childFiles);
    m_rootDirectory->childFiles.clear();

    for (int i = 0; i < fileList.length(); i++) {
        TorrentFile *currentFile = m_rootDirectory;
        QVariantMap fileMap = fileList.at(i).toMap();
        QVariantMap fileStatsMap = fileStatsList.at(i).toMap();

        QString filePath = m_filePaths.at(i);
        QStringList parts = filePath.split("/", QString::SkipEmptyParts);
        for (int j = 0; j < parts.length(); j++) {
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

    foreach (TorrentFile *file, m_rootDirectory->childFiles) {
        if (file->isDirectory)
            setDirectory(file);
    }

    emit treeCreated();
}

void TorrentFileModelWorker::updateTree(const QVariantList &fileStatsList)
{
    for (int i = 0; i < fileStatsList.length(); i++) {
        QVariantMap fileStatsMap = fileStatsList.at(i).toMap();
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

    foreach (TorrentFile *file, m_rootDirectory->childFiles) {
        if (file->isDirectory)
            updateDirectory(file);
    }

    emit treeUpdated(m_changedFiles);
}

void TorrentFileModelWorker::setDirectory(TorrentFile *directory)
{
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->isDirectory)
            setDirectory(file);
    }

    qint64 bytesCompleted = 0;
    qint64 length = 0;
    int priority = directory->childFiles.first()->priority;
    int wantedStatus = directory->childFiles.first()->wantedStatus;

    foreach (TorrentFile *file, directory->childFiles) {
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
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->isDirectory)
            updateDirectory(file);
    }

    bool changed = false;
    qint64 bytesCompleted = 0;
    int priority = directory->childFiles.first()->priority;
    int wantedStatus = directory->childFiles.first()->wantedStatus;

    foreach (TorrentFile *file, directory->childFiles) {
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
{
    qRegisterMetaType< QList<TorrentFile*> >();

    m_rootDirectory = new TorrentFile;

    m_worker = new TorrentFileModelWorker(m_rootDirectory, &m_mutex);
    connect(this, &TorrentFileModel::requestModelUpdate, m_worker, &TorrentFileModelWorker::beginWork);
    connect(m_worker, &TorrentFileModelWorker::treeCreated, this, &TorrentFileModel::endCreateTree);
    connect(m_worker, &TorrentFileModelWorker::treeUpdated, this, &TorrentFileModel::endUpdateTree);

    m_workerThread = new QThread(this);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_worker->moveToThread(m_workerThread);
    m_workerThread->start(QThread::LowPriority);

    m_isActive = false;
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

QVariant TorrentFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TorrentFile *file = static_cast<TorrentFile*>(index.internalPointer());

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

QModelIndex TorrentFileModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TorrentFile *parentDirectory = static_cast<TorrentFile*>(index.internalPointer())->parentDirectory;
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

bool TorrentFileModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    QMutexLocker locker(&m_mutex);

    TorrentFile *file = static_cast<TorrentFile*>(index.internalPointer());

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
    return m_isActive;
}

int TorrentFileModel::torrentId() const
{
    return m_torrentId;
}

Transmission* TorrentFileModel::transmission() const
{
    return m_transmission;
}

void TorrentFileModel::setIsActive(bool isActive)
{
    m_isActive = isActive;
}

void TorrentFileModel::setTorrentId(int torrentId)
{
    m_torrentId = torrentId;
}

void TorrentFileModel::setTransmission(Transmission *transmission)
{
    m_transmission = transmission;
}

void TorrentFileModel::setAllWanted(bool wanted)
{
    m_mutex.lock();

    if (wanted) {
        foreach (TorrentFile *file, m_rootDirectory->childFiles)
            setFileWantedStatusRecursively(file, AllWanted);
    } else {
        foreach (TorrentFile *file, m_rootDirectory->childFiles)
            setFileWantedStatusRecursively(file, NoWanted);
    }

    foreach (TorrentFile *file, m_rootDirectory->childFiles) {
        if (file->isDirectory)
            updateDirectoryWantedStatus(file);
    }

    if (wanted)
        m_transmission->changeTorrent(m_torrentId, "files-wanted", QVariantList());
    else
        m_transmission->changeTorrent(m_torrentId, "files-unwanted", QVariantList());

    m_mutex.unlock();
}

void TorrentFileModel::beginUpdateModel(const QVariantList &fileList, const QVariantList &fileStatsList)
{
    emit requestModelUpdate(fileList, fileStatsList);
}

void TorrentFileModel::resetModel()
{
    m_mutex.lock();

    beginResetModel();
    qDeleteAll(m_rootDirectory->childFiles);
    m_rootDirectory->childFiles.clear();
    endResetModel();

    m_worker->reset();

    m_isActive = false;

    m_mutex.unlock();
}

void TorrentFileModel::endCreateTree()
{
    beginResetModel();
    endResetModel();
}

void TorrentFileModel::endUpdateTree(const QList<TorrentFile*> &changedFiles)
{
    foreach (TorrentFile *file, changedFiles) {
        QModelIndex modelIndex = createIndex(file->row, 0, file);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void TorrentFileModel::setFilePriority(TorrentFile *file, int priority)
{
    setFilePriorityRecursively(file, priority);

    foreach (TorrentFile *file, m_rootDirectory->childFiles) {
        if (file->isDirectory)
            updateDirectoryPriority(file);
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
        foreach (TorrentFile *childFile, file->childFiles)
            setFilePriorityRecursively(childFile, priority);
    } else {
        file->priority = priority;

        QModelIndex modelIndex = createIndex(file->row, 0, file);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << PriorityRole);
    }
}

void TorrentFileModel::updateDirectoryPriority(TorrentFile *directory)
{
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->isDirectory)
            updateDirectoryPriority(file);
    }

    int priority = directory->childFiles.first()->priority;
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->priority != priority) {
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

    foreach (TorrentFile *file, m_rootDirectory->childFiles) {
        if (file->isDirectory)
            updateDirectoryWantedStatus(file);
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
        foreach (TorrentFile *childFile, file->childFiles)
            setFileWantedStatusRecursively(childFile, wantedStatus);
    } else {
        file->wantedStatus = wantedStatus;

        QModelIndex modelIndex = createIndex(file->row, 0, file);
        emit dataChanged(modelIndex, modelIndex, QVector<int>() << WantedStatusRole);
    }
}

void TorrentFileModel::updateDirectoryWantedStatus(TorrentFile *directory)
{
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->isDirectory)
            updateDirectoryWantedStatus(file);
    }

    int wantedStatus = directory->childFiles.first()->wantedStatus;
    foreach (TorrentFile *file, directory->childFiles) {
        if (file->wantedStatus != wantedStatus) {
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
        foreach (const TorrentFile *childFile, file->childFiles)
            fileIndexes.append(getSubtreeFileIndexes(childFile));
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
