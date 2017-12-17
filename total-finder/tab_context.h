#pragma once

#include <QDir>
#include <QFileInfo>

namespace TotalFinder
{
  class DirViewPanel;
  class TabManager;

  class TabContext
  {
  public:
    TabContext(TabManager* tabs);
    ~TabContext() {}
    bool IsOppositeTabDirView(DirViewPanel* currentTab) const;
    QFileInfo GetOppositeTabSelection(DirViewPanel* currentTab) const;
    QDir GetOppositeTabRootDir(DirViewPanel* currentTab) const;
    void ChangeSide(bool force = false);
  private:
    TabManager* Tabs;
  };
} // namespace TotalFinder
