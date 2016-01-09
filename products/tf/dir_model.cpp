/*
 * dir_model.cpp
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "dir_model.h"
#include "settings.h"

#include <QDateTime>
#include <QDebug>
#include <QFileIconProvider>

namespace TF
{
  namespace
  {
    bool IsParentFileItem(const QString& parent, const QString& child)
    {
      const int pos = child.indexOf(parent);
      if (pos != 0 || child.mid(parent.size() + (child.endsWith(QDir::separator()) ? 0 : 1)).contains(QDir::separator()))
      {
        return false;
      }
      return true;
    }

    enum MODEL_COLUMNS
    {
      COL_NAME,
      COL_EXT,
      COL_SIZE,
      COL_MTIME,
      COL_PERMISSIONS,
      COL_COUNT
    };

    QPair<QString, QString> GetNameAndExtension(const QFileInfo& item)
    {
      if (item.isDir() || item.completeBaseName().isEmpty())
      {
        return QPair<QString, QString>(item.fileName(), "");
      }
      return QPair<QString, QString>(item.completeBaseName(), item.suffix());
    }

    QString DecodePermissions(QFile::Permissions permissions, bool isDir)
    {
      QString result;
      result += isDir ? "d" : "-";

      result += permissions & QFile::ReadOwner ? "r" : "-";
      result += permissions & QFile::WriteOwner ? "w" : "-";
      result += permissions & QFile::ExeOwner ? "x" : "-";

      result += permissions & QFile::ReadGroup ? "r" : "-";
      result += permissions & QFile::WriteGroup ? "w" : "-";
      result += permissions & QFile::ExeGroup ? "x" : "-";

      result += permissions & QFile::ReadOther ? "r" : "-";
      result += permissions & QFile::WriteOther ? "w" : "-";
      result += permissions & QFile::ExeOther ? "x" : "-";

      return result;
    }
  } // namespace

  DirModel::DirModel(QObject* parent)
    : QAbstractTableModel(parent)
    , FileWatcher(new QFileSystemWatcher(this))
  {
    connect(&Settings::SettingsChangeMonitor::Instance(), SIGNAL(SettingsChanged()), SLOT(OnSettingsChange()));
    // TODO: it's potentially expensive to watch every tab - maybe only visible ones
    connect(FileWatcher, SIGNAL(directoryChanged(const QString&)), SLOT(OnDirectoryChanged()));
  }

  void DirModel::OnSettingsChange()
  {
    SetRoot(RootDir);
  }

  void DirModel::SetRoot(const QDir& dir)
  {
    beginResetModel();
    RootDir = dir;
    RootDir.setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    RootDir.setFilter(Settings::LoadDirFilters() | (RootDir.isRoot() ? QDir::NoDotDot : QDir::AllEntries));
    if (!FileWatcher->directories().empty() && !FileWatcher->removePaths(FileWatcher->directories()).empty())
    {
      qWarning() << "Failed to remove directories from watch";
    }
    if (!FileWatcher->addPath(RootDir.absolutePath()))
    {
      qWarning() << "Failed to add directory to watch:" << RootDir.absolutePath();
    }
    endResetModel();
  }

  void DirModel::OnDirectoryChanged()
  {
    qDebug() << "Directory change notification " << RootDir.absolutePath();
    RootDir.refresh();
    emit dataChanged(QModelIndex(), QModelIndex());
    emit layoutChanged();
  }

  QDir DirModel::GetRoot() const
  {
    return RootDir;
  }

  QFileInfo DirModel::GetItem(const QModelIndex& index) const
  {
    if (!index.isValid())
    {
      return QFileInfo();
    }
    return RootDir.entryInfoList()[index.row()];
  }

  QModelIndex DirModel::GetIndex(const QFileInfo& file) const
  {
    if (!IsParentDir(RootDir, file))
    {
      return QModelIndex();
    }
    return index(RootDir.entryList().indexOf(file.fileName()), 0);
  }

  QModelIndex DirModel::GetIndex(const QDir& dir) const
  {
    if (!IsParentDir(RootDir, dir))
    {
      return QModelIndex();
    }
    return index(RootDir.entryList().indexOf(dir.dirName()), 0);
  }

  QModelIndexList DirModel::Search(const QString& search) const
  {
    QStringList nameFilters;
    nameFilters << search;
    QModelIndexList result;
    foreach(const QFileInfo& entry, RootDir.entryInfoList(nameFilters))
    {
      result << GetIndex(entry);
    }
    return result;
  }

  QVariant DirModel::data(const QModelIndex& index, int role) const
  {
    if (!index.isValid() || RootDir.path().isEmpty())
    {
      return QVariant();
    }
    const QFileInfo currentItem = RootDir.entryInfoList()[index.row()];

    if (role == Qt::DisplayRole)
    {
      switch (index.column())
      {
        case COL_NAME:
          return GetNameAndExtension(currentItem).first;
        case COL_EXT:
          return GetNameAndExtension(currentItem).second;
        case COL_SIZE:
          return currentItem.isDir() ? "[DIR]" : QString::number(currentItem.size());
        case COL_MTIME:
          return currentItem.lastModified();
        case COL_PERMISSIONS:
          return DecodePermissions(currentItem.permissions(), currentItem.isDir());
      }
      return QVariant();
    }
    if (role == Qt::DecorationRole && index.column() == 0)
    {
      QFileIconProvider icons;
      return icons.icon(currentItem);
    }
    return QVariant();
  }

  QVariant DirModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
      switch (section)
      {
        case COL_NAME:
          return "Name";
        case COL_EXT:
          return "Ext";
        case COL_SIZE:
          return "Size";
        case COL_MTIME:
          return "Mtime";
        case COL_PERMISSIONS:
          return "Perm";
      }
    }
    return QVariant();
  }

  int DirModel::rowCount(const QModelIndex& /*parent*/) const
  {
    return RootDir.entryList().size();
  }

  int DirModel::columnCount(const QModelIndex& /*parent*/) const
  {
    return COL_COUNT;
  }

  bool IsParentDir(const QDir& parent, const QDir& child)
  {
    return IsParentFileItem(parent.absolutePath(), child.absolutePath());
  }

  bool IsParentDir(const QDir& parent, const QFileInfo& child)
  {
    return IsParentFileItem(parent.absolutePath(), child.absolutePath());
  }
} // namespace TF
