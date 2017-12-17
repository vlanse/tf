#include "tab_context.h"
#include "dir_view_panel.h"
#include "tab_manager.h"

namespace TotalFinder
{
  TabContext::TabContext(TabManager* tabs)
    : Tabs(tabs)
  {
  }

  QFileInfo TabContext::GetOppositeTabSelection(DirViewPanel* currentTab) const
  {
    DirViewPanel* opposite = Tabs->GetOppositeTab(currentTab);
    return opposite->GetCurrentSelection();
  }

  QDir TabContext::GetOppositeTabRootDir(DirViewPanel* currentTab) const
  {
    DirViewPanel* opposite = Tabs->GetOppositeTab(currentTab);
    return opposite->GetRootDir();
  }

  void TabContext::ChangeSide(bool force)
  {
    Tabs->OnChangeSideRequest(force);
  }
} // namespace TotalFinder
