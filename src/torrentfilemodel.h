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

#ifndef TREMOTESF_TORRENTFILEMODEL_H
#define TREMOTESF_TORRENTFILEMODEL_H

#include <QAbstractListModel>
#include <QMutex>

class QThread;

namespace Tremotesf
{

class Transmission;

struct TorrentFile
{
    TorrentFile(TorrentFile *parentDirectory = 0, int row = 0);
    ~TorrentFile();

    TorrentFile *parentDirectory;
    int row;
    QMap<QString, TorrentFile*> childFiles;

    qint64 bytesCompleted;
    int fileIndex;
    bool isDirectory;
    qint64 length;
    QString name;
    QString path;
    int priority;
    float progress;
    int wantedStatus;
};

class TorrentFileModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentFileModelWorker(TorrentFile *rootDirectory, QMutex *mutex);
    void reset();
    void beginWork(QVariantList fileVariants, QVariantList fileStatsVariants);
private:
    void createTree(const QVariantList &fileVariants, const QVariantList &fileStatsVariants);
    void updateTree(const QVariantList &fileStatsVariants);

    void setDirectory(TorrentFile *directory);
    void updateDirectory(TorrentFile *directory);

    TorrentFile *m_rootDirectory;
    QMutex *m_mutex;

    QList<TorrentFile*> m_files;
    QList<TorrentFile*> m_changedFiles;
    QStringList m_filePaths;
signals:
    void treeCreated();
    void treeUpdated(QList<Tremotesf::TorrentFile*> changedFiles);
};

class TorrentFileModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(Priority)
    Q_ENUMS(TorrentFileRole)
    Q_ENUMS(WantedStatus)
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(Tremotesf::Transmission* transmission READ transmission WRITE setTransmission)
public:
    enum TorrentFileRole {
        BytesCompletedRole = Qt::UserRole,
        FileIndexRole,
        IsDirectoryRole,
        LengthRole,
        NameRole,
        PathRole,
        PriorityRole,
        ProgressRole,
        WantedStatusRole
    };

    enum Priority {
        LowPriority = -1,
        NormalPriority,
        HighPriority,
        MixedPriority
    };

    enum WantedStatus {
        NoWanted,
        SomeWanted,
        AllWanted
    };

    TorrentFileModel();
    ~TorrentFileModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &modelIndex) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &modelIndex, const QVariant &value, int role);

    bool isActive() const;
    void setActive(bool active);

    int torrentId() const;
    void setTorrentId(int id);

    Transmission* transmission() const;
    void setTransmission(Transmission *newTransmission);

    Q_INVOKABLE void setAllWanted(bool wanted);

    void beginUpdateModel(const QVariantList &fileVariants, const QVariantList &fileStatsVariants);
    Q_INVOKABLE void resetModel();
private:
    void endCreateTree();
    void endUpdateTree(QList<TorrentFile*> changedFiles);

    void setFilePriority(TorrentFile *file, int priority);
    void setFilePriorityRecursively(TorrentFile *file, int priority);
    void updateDirectoryPriority(TorrentFile *directory);

    void setFileWantedStatus(TorrentFile *file, int wantedStatus);
    void setFileWantedStatusRecursively(TorrentFile *file, int wantedStatus);
    void updateDirectoryWantedStatus(TorrentFile *directory);

    QVariantList getSubtreeFileIndexes(const TorrentFile *file);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    TorrentFile *m_rootDirectory;

    TorrentFileModelWorker *m_worker;
    QThread *m_workerThread;

    QMutex m_mutex;

    bool m_active;
    int m_torrentId;
    Transmission *m_transmission;
signals:
    void requestModelUpdate(QVariantList fileVariants, QVariantList fileStatsVariants);
};

}

Q_DECLARE_METATYPE(Tremotesf::TorrentFile*)

#endif // TREMOTESF_TORRENTFILEMODEL_H
