/*
 * tab_context.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#include "tab_context.h"
#include "dir_view_panel.h"
#include "tab_manager.h"

namespace TF
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
} // namespace TF
