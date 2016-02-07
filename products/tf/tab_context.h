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
  class DirViewPanel;
  class TabManager;

  class TabContext
  {
  public:
    TabContext(TabManager* tabs);
    ~TabContext() {}
    QFileInfo GetOppositeTabSelection(DirViewPanel* currentTab) const;
    QDir GetOppositeTabRootDir(DirViewPanel* currentTab) const;
    void ChangeSide(bool force = false);
  private:
    TabManager* Tabs;
  };
} // namespace TF
