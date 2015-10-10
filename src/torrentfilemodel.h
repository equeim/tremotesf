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

#ifndef TORRENTFILEMODEL_H
#define TORRENTFILEMODEL_H

#include <QAbstractListModel>
#include <QMutex>

class QThread;
class Transmission;

class TorrentFile
{
public:
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
Q_DECLARE_METATYPE(TorrentFile*)

class TorrentFileModelWorker : public QObject
{
    Q_OBJECT
public:
    TorrentFileModelWorker(TorrentFile *rootDirectory);
    void reset();
public slots:
    void beginWork(const QVariantList &fileList, const QVariantList &fileStatsList);
private:
    void createTree(const QVariantList &fileList, const QVariantList &fileStatsList);
    void updateTree(const QVariantList &fileStatsList);

    void setDirectory(TorrentFile *directory);
    void updateDirectory(TorrentFile *directory);

    TorrentFile *m_rootDirectory;
    QList<TorrentFile*> m_files;
    QList<TorrentFile*> m_changedFiles;
    QStringList m_filePaths;
signals:
    void treeCreated();
    void treeUpdated(const QList<TorrentFile*> &changedFiles);
};

class TorrentFileModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(Priority)
    Q_ENUMS(TorrentFileRole)
    Q_ENUMS(WantedStatus)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive)
    Q_PROPERTY(Transmission* transmission READ transmission WRITE setTransmission)
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
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool isActive() const;
    int torrentId() const;
    Transmission* transmission() const;
    QString currentDirectoryPath() const;
    QString parentDirectoryPath() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);
    void setTransmission(Transmission *transmission);

    void beginUpdateModel(const QVariantList &fileList, const QVariantList &fileStatsList);
    Q_INVOKABLE void resetModel();
private:
    void endCreateTree();
    void endUpdateTree(const QList<TorrentFile*> &changedFiles);

    void setFilePriority(TorrentFile *file, int priority);
    void setFileWantedStatus(TorrentFile *file, int wantedStatus);

    void setRoleValueRecursively(TorrentFile *file, const QVariant &value, int role);
    QVariantList getSubtreeFileIndexes(const TorrentFile *file);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    TorrentFile *m_rootDirectory;

    TorrentFileModelWorker *m_worker;
    QThread *m_workerThread;

    QMutex m_mutex;

    bool m_isActive;
    int m_torrentId;
    Transmission *m_transmission;
signals:
    void requestModelUpdate(const QVariantList &fileList, const QVariantList &fileStatsList);
};

#endif // TORRENTFILEMODEL_H
