#include "tab_context.h"
#include "dir_view_panel.h"
#include "tab_manager.h"

namespace TotalFinder
{
  TabContext::TabContext(TabManager* tabs)
    : Tabs(tabs)
  {
  }

  bool TabContext::IsOppositeTabDirView(DirViewPanel* currentTab) const
  {
    DirViewPanel* opposite = qobject_cast<DirViewPanel*>(Tabs->GetOppositeTab(currentTab));
    return opposite != NULL;
  }

  QFileInfo TabContext::GetOppositeTabSelection(DirViewPanel* currentTab) const
  {
    DirViewPanel* opposite = qobject_cast<DirViewPanel*>(Tabs->GetOppositeTab(currentTab));
    if (opposite) {
      return opposite->GetCurrentSelection();
    }
    return QFileInfo();
  }

  QDir TabContext::GetOppositeTabRootDir(DirViewPanel* currentTab) const
  {
    DirViewPanel* opposite = qobject_cast<DirViewPanel*>(Tabs->GetOppositeTab(currentTab));
    if (opposite) {
      return opposite->GetRootDir();
    }
    return QDir();
  }

  void TabContext::ChangeSide(bool force)
  {
    Tabs->OnChangeSideRequest(force);
  }
} // namespace TotalFinder
