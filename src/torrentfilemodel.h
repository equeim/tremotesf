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
#include <QThread>

class QCollator;

struct TorrentFile
{
    QMap<QString, TorrentFile*> entries;

    qint64 bytesCompleted;
    int fileIndex;
    bool isDirectory;
    qint64 length;
    QString name;
    QString path;
    int priority;
    bool wanted;
};
Q_DECLARE_METATYPE(TorrentFile)

class TorrentFileModelWorker : public QThread
{
    Q_OBJECT
public:
    TorrentFileModelWorker(TorrentFile *rootEntry, QMap<QString, TorrentFile*> *allEntries);
    void setData(const QVariantList &fileList, const QVariantList &fileStatsList);
private:
    TorrentFile *m_rootEntry;
    QMap<QString, TorrentFile*> *m_allEntries;
    QVariantList m_fileList;
    QVariantList m_fileStatsList;
protected:
    void run();
signals:
    void done(bool resetModel);
};

class TorrentFileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Priority)
    Q_ENUMS(DirectoryWantedStatus)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive)
    Q_PROPERTY(QString currentDirectoryPath READ currentDirectoryPath NOTIFY currentDirectoryPathChanged)
    Q_PROPERTY(QString parentDirectoryPath READ parentDirectoryPath NOTIFY parentDirectoryPathChanged)
public:
    enum TorrentFileRole {
        BytesCompletedRole = Qt::UserRole,
        FileIndexRole,
        IsDirectoryRole,
        LengthRole,
        NameRole,
        PathRole,
        PriorityRole,
        WantedRole
    };

    enum Priority {
        LowPriority = -1,
        NormalPriority,
        HighPriority,
        MixedPriority
    };

    enum DirectoryWantedStatus {
        NoWanted,
        SomeWanted,
        AllWanted
    };

    TorrentFileModel();
    ~TorrentFileModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool isActive() const;
    int torrentId() const;
    QString currentDirectoryPath() const;
    QString parentDirectoryPath() const;

    void setIsActive(bool isActive);
    void setTorrentId(int torrentId);

    void beginUpdateModel(const QVariantList &fileList, const QVariantList &fileStatsList);

    Q_INVOKABLE void resetModel();
    Q_INVOKABLE void loadDirectory(QString path);
    Q_INVOKABLE int getDirectoryWantedStatus(int index) const;
    Q_INVOKABLE int getDirectoryPriority(int index) const;
    Q_INVOKABLE QVariantList getDirectoryFileIndexes(int index) const;
public slots:
    void endUpdateModel(bool resetModel);
signals:
    void currentDirectoryPathChanged();
    void parentDirectoryPathChanged();
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    void sort();
    int countFilesInDirectory(const TorrentFile *directory) const;
    int countWantedFilesInDirectory(const TorrentFile *directory) const;
    int countPrioritiesInDirectory(const TorrentFile *directory) const;
    QVariantList countFileIndexesInDirectory(const TorrentFile *directory) const;

    TorrentFile *m_rootEntry;
    QMap<QString, TorrentFile*> *m_allEntries;
    QList<TorrentFile*> m_currentDirectoryEntries;
    TorrentFileModelWorker *m_worker;
    QCollator *m_collator;

    bool m_isActive;
    int m_torrentId;
    QString m_currentDirectoryPath;
    QString m_parentDirectoryPath;

    bool m_changingModel;
signals:
    void modelChanged();
};

#endif // TORRENTFILEMODEL_H
