/*
 * tab_context.h
 *
 *  Created on: Jan 17, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QDir>
#include <QFileInfo>

namespace TF
{
  class FileManagerTab : QObject
  {
    Q_OBJECT
  public:
    FileManagerTab(QObject* parent) : QObject(parent) {}
    virtual ~FileManagerTab() {}
    virtual QFileInfo GetSelection() const = 0;
    virtual QDir GetRootDir() const = 0;
    virtual void SetRootDir(const QDir& dir) = 0;
    virtual void SetFocus() = 0;
  signals:
    void DirChanged();
  };

  class TabContext
  {
  public:
    virtual ~TabContext() {}
    virtual QFileInfo GetOppositeTabSelection(FileManagerTab* currentTab) const = 0;
    virtual void ChangeSide(FileManagerTab* currentTab) = 0;
    virtual void AddTabToTheLeft(FileManagerTab* tab) = 0;
    virtual void AddTabToTheRight(FileManagerTab* tab) = 0;
    virtual void CloseTab(FileManagerTab* tab) = 0;
  };
} // namespace TF
