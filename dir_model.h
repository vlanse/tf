/*
 * dir_model.h
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#pragma once

#include <QAbstractTableModel>
#include <QFileSystemWatcher>
#include <QDir>

namespace TF
{
  class DirModel: public QAbstractTableModel
  {
    Q_OBJECT
  public:
    DirModel(QObject* parent);
    void SetRoot(const QDir& dir);
    QDir GetRoot() const;
    QFileInfo GetItem(const QModelIndex& index) const;
    QModelIndex GetIndex(const QFileInfo& info) const;
    QModelIndex GetIndex(const QDir& dir) const;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  private slots:
    void OnSettingsChange();
    void OnDirectoryChanged();
  private:
    QDir RootDir;
    QFileSystemWatcher *FileWatcher;
  };

  bool IsParentDir(const QDir& parent, const QDir& child);
  bool IsParentDir(const QDir& parent, const QFileInfo& child);
} // namespace TF
