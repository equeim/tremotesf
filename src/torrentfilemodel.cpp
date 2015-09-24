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

#include "torrentfilemodel.moc"

#include <QCollator>
#include <QEventLoop>

TorrentFileModelWorker::TorrentFileModelWorker(TorrentFile *rootEntry, QMap<QString, TorrentFile *> *allEntries)
{
    m_rootEntry = rootEntry;
    m_allEntries = allEntries;
}

void TorrentFileModelWorker::setData(const QVariantList &fileList, const QVariantList &fileStatsList)
{
    m_fileList = fileList;
    m_fileStatsList = fileStatsList;
}

void TorrentFileModelWorker::run() Q_DECL_OVERRIDE
{
    if (!m_rootEntry->entries.isEmpty()) {
        for (int i = 0; i < m_fileList.length(); i++) {
            QVariantMap fileStatsMap = m_fileStatsList.at(i).toMap();
            QString filePath = m_fileList.at(i).toMap().value("name").toString();
            TorrentFile *file = m_allEntries->value("/" + filePath);

            file->bytesCompleted = fileStatsMap.value("bytesCompleted").toLongLong();
            file->priority = fileStatsMap.value("priority").toInt();
            file->wanted = fileStatsMap.value("wanted").toBool();
        }
        emit done(false);
    } else if (!m_fileList.isEmpty()) {
        for (int i = 0; i < m_fileList.length(); i++) {
            TorrentFile *current = m_rootEntry;
            QVariantMap fileStatsMap = m_fileStatsList.at(i).toMap();
            QVariantMap fileMap = m_fileList.at(i).toMap();
            QString filePath = fileMap.value("name").toString();
            filePath.insert(0, "/");
            QString path;
            QStringList parts = filePath.split("/", QString::SkipEmptyParts);
            foreach (QString part, parts) {
                path.append("/" + part);
                if (!current->entries.contains(part)) {
                    TorrentFile *child = new TorrentFile;
                    current->entries.insert(part, child);
                    m_allEntries->insert(path, child);
                }
                current = current->entries.value(part);
                current->name = part;
                current->path = path;
                if (path == filePath) {
                    current->bytesCompleted = fileStatsMap.value("bytesCompleted").toLongLong();
                    current->isDirectory = false;
                    current->length = fileMap.value("length").toLongLong();
                    current->priority = fileStatsMap.value("priority").toInt();
                    current->wanted = fileStatsMap.value("wanted").toBool();
                    current->fileIndex = i;
                }
                else {
                    current->isDirectory = true;
                }
            }
        }
        emit done(true);
    }
}

TorrentFileModel::TorrentFileModel()
{
    m_rootEntry = new TorrentFile;
    m_allEntries = new QMap<QString, TorrentFile*>;
    m_worker = new TorrentFileModelWorker(m_rootEntry, m_allEntries);
    connect(m_worker, &TorrentFileModelWorker::done, this, &TorrentFileModel::endUpdateModel);

    m_collator = new QCollator;
    m_collator->setNumericMode(true);
    m_collator->setCaseSensitivity(Qt::CaseInsensitive);

    m_isActive = false;

    m_changingModel = false;
}

TorrentFileModel::~TorrentFileModel()
{
    m_worker->wait();
    m_worker->deleteLater();
    qDeleteAll(m_allEntries->values());
    delete m_allEntries;
    delete m_rootEntry;

    delete m_collator;
}

int TorrentFileModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_currentDirectoryEntries.length();
}

QVariant TorrentFileModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    const TorrentFile *file = m_currentDirectoryEntries.at(index.row());

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
    case WantedRole:
        return file->wanted;
    default:
        return QVariant();
    }
}

bool TorrentFileModel::isActive() const
{
    return m_isActive;
}

int TorrentFileModel::torrentId() const
{
    return m_torrentId;
}

QString TorrentFileModel::currentDirectoryPath() const
{
    return m_currentDirectoryPath;
}

QString TorrentFileModel::parentDirectoryPath() const
{
    return m_parentDirectoryPath;
}

void TorrentFileModel::setIsActive(bool isActive)
{
    m_isActive = isActive;
}

void TorrentFileModel::setTorrentId(int torrentId)
{
    m_torrentId = torrentId;
}

void TorrentFileModel::beginUpdateModel(const QVariantList &fileList, const QVariantList &fileStatsList)
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentFileModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    m_worker->setData(fileList, fileStatsList);
    m_worker->start(QThread::LowestPriority);
}

void TorrentFileModel::resetModel()
{
    if (m_changingModel) {
        QEventLoop loop;
        connect(this, &TorrentFileModel::modelChanged, &loop, &QEventLoop::quit);
        loop.exec();
    }
    m_changingModel = true;

    beginResetModel();
    qDeleteAll(m_allEntries->values());
    m_allEntries->clear();
    m_rootEntry->entries.clear();
    m_currentDirectoryEntries.clear();
    endResetModel();

    m_currentDirectoryPath.clear();
    m_parentDirectoryPath.clear();

    m_isActive = false;

    m_changingModel = false;
    emit modelChanged();
}

void TorrentFileModel::loadDirectory(QString path)
{
    beginResetModel();
    if (path.isEmpty())
        m_currentDirectoryEntries = m_rootEntry->entries.values();
    else
        m_currentDirectoryEntries = m_allEntries->value(path)->entries.values();
    m_currentDirectoryPath = path;
    m_parentDirectoryPath = path.left(path.lastIndexOf("/"));
    sort();
    endResetModel();
}

int TorrentFileModel::getDirectoryWantedStatus(int index) const
{
    TorrentFile* directory = m_currentDirectoryEntries.at(index);

    int allFiles = countFilesInDirectory(directory);
    int wantedFiles = countWantedFilesInDirectory(directory);

    if (wantedFiles == 0)
        return NoWanted;
    if (wantedFiles == allFiles)
        return AllWanted;
    return SomeWanted;
}

int TorrentFileModel::getDirectoryPriority(int index) const
{
    TorrentFile* directory = m_currentDirectoryEntries.at(index);

    int allFiles = countFilesInDirectory(directory);
    int allPriorities = countPrioritiesInDirectory(directory);

    if (allPriorities == 0)
        return NormalPriority;

    float priority = allPriorities / allFiles;
    if (priority == -1)
        return LowPriority;
    if (priority == 1)
        return HighPriority;

    return MixedPriority;
}

QVariantList TorrentFileModel::getDirectoryFileIndexes(int index) const
{
    TorrentFile* directory = m_currentDirectoryEntries.at(index);
    return countFileIndexesInDirectory(directory);
}

void TorrentFileModel::endUpdateModel(bool resetModel)
{
    if (resetModel) {
        beginResetModel();
        m_currentDirectoryEntries = m_rootEntry->entries.values();
        sort();
        endResetModel();
    } else {
        emit dataChanged(index(0), index(rowCount() - 1));
    }

    m_changingModel = false;
    emit modelChanged();
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
    roles.insert(WantedRole, "wanted");

    return roles;
}

void TorrentFileModel::sort()
{
    for (int i = 0; i < m_currentDirectoryEntries.length(); i++) {
        for (int j = i+1; j < m_currentDirectoryEntries.length(); j++) {
            QString leftFileName = m_currentDirectoryEntries[i]->name;
            bool isLeftDir = m_currentDirectoryEntries[i]->isDirectory;
            QString rightFileName = m_currentDirectoryEntries[j]->name;
            bool isRightDir = m_currentDirectoryEntries[j]->isDirectory;

            if (isLeftDir == isRightDir) {
                int nameCompare = m_collator->compare(leftFileName, rightFileName);
                if(nameCompare > 0)
                    m_currentDirectoryEntries.swap(i, j);
            }
            else if (isRightDir) {
                m_currentDirectoryEntries.swap(i, j);
            }
        }
    }
}

int TorrentFileModel::countFilesInDirectory(const TorrentFile *directory) const
{
    int files = 0;
    QList<TorrentFile*> entries = directory->entries.values();

    for (int i = 0; i < entries.length(); i++) {
        TorrentFile *currentFile = entries.at(i);
        if (currentFile->isDirectory)
            files += countFilesInDirectory(currentFile);
        else
            files++;
    }
    return files;
}

int TorrentFileModel::countWantedFilesInDirectory(const TorrentFile *directory) const
{
    int wantedFiles = 0;
    QList<TorrentFile*> entries = directory->entries.values();

    for (int i = 0; i < entries.length(); i++) {
        TorrentFile *currentFile = entries.at(i);
        if (currentFile->isDirectory) {
            wantedFiles += countWantedFilesInDirectory(currentFile);
        }
        else {
            if (currentFile->wanted)
                wantedFiles++;
        }
    }
    return wantedFiles;
}


int TorrentFileModel::countPrioritiesInDirectory(const TorrentFile *directory) const
{
    int priorities = 0;
    QList<TorrentFile*> entries = directory->entries.values();

    for (int i = 0; i < entries.length(); i++) {
        TorrentFile *currentFile = entries.at(i);
        if (currentFile->isDirectory)
            priorities += countPrioritiesInDirectory(currentFile);
        else
            priorities += currentFile->priority;
    }
    return priorities;
}

QVariantList TorrentFileModel::countFileIndexesInDirectory(const TorrentFile *directory) const
{
    QVariantList fileIndexes;
    QList<TorrentFile*> entries = directory->entries.values();

    for (int i = 0; i < entries.length(); i++) {
        TorrentFile *currentFile = entries.at(i);
        if (currentFile->isDirectory)
            fileIndexes.append(countFileIndexesInDirectory(currentFile));
        else
            fileIndexes.append(currentFile->fileIndex);
    }
    return fileIndexes;
}
