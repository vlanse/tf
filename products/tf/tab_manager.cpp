/*
 * tab_manager.cpp
 *
 *  Created on: Dec 25, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "tab_manager.h"

#include "dir_view_panel.h"

#include <QDebug>

namespace TF
{
  namespace
  {
    const char ROOT_DIR_NAME[] = "/";

    int AddTab(TabManager* tabs, DirViewPanel* tab, SideContext& side)
    {
      QObject::connect(tab, SIGNAL(DirChanged()), tabs, SLOT(OnDirChange()));
      const int tabIndex = side.Container->insertTab(0, tab, QIcon(), tab->GetRootDir().dirName());;
      side.Container->setCurrentIndex(tabIndex);

      QObject::connect(tab, SIGNAL(DirChanged()), tabs, SLOT(OnDirChange()));
      QObject::connect(tab, SIGNAL(ChangeSideRequest()), tabs, SLOT(OnChangeSideRequest()));

      side.TabsIndex[tab] = tabIndex;
      side.ReverseTabsIndex[tabIndex] = tab;
      side.ActiveTabIndex = tabIndex;

      return tabIndex;
    }
  } // namespace

  TabManager::TabManager(QTabWidget* leftContainer, QTabWidget* rightContainer, QObject* parent)
    : QObject(parent)
  {
    LeftSide.Active = true;
    LeftSide.Container = leftContainer;
    RightSide.Container = rightContainer;

    RestoreContext();
  }

  void TabManager::RestoreContext()
  {
    // TODO: restore tabs from settings

    DirViewPanel* tab = new DirViewPanel();
    tab->SetRootDir(QDir("/Users/vsemenov/test"));
    AddTabToTheLeft(tab);
    tab->SetFocusOnView();

    tab = new DirViewPanel();
    tab->SetRootDir(QDir("/Users/vsemenov/test"));
    AddTabToTheRight(tab);
  }

  void TabManager::OnChangeSideRequest()
  {
    LeftSide.Active = !LeftSide.Active;
    RightSide.Active = !RightSide.Active;

    SetFocusOnView();
  }

  void TabManager::AddTabToTheLeft(DirViewPanel* tab)
  {
    AddTab(this, tab, LeftSide);
  }

  void TabManager::AddTabToTheRight(DirViewPanel* tab)
  {
    AddTab(this, tab, RightSide);
  }

  void TabManager::CloseTab(DirViewPanel* tab)
  {
  }

  SideContext* TabManager::GetActiveSide()
  {
    if (LeftSide.Active)
    {
      return &LeftSide;
    }
    if (RightSide.Active)
    {
      return &RightSide;
    }
    return 0;
  }

  SideContext* TabManager::FindSideForTab(DirViewPanel* tab)
  {
    if (LeftSide.TabsIndex.contains(tab))
    {
      return &LeftSide;
    }
    if (RightSide.TabsIndex.contains(tab))
    {
      return &RightSide;
    }
    return 0;
  }

  void TabManager::SetFocusOnView()
  {
    SideContext* side = GetActiveSide();
    if (!side)
    {
      qWarning("Where is no active side");
      return;
    }
    side->Container->setCurrentIndex(side->ActiveTabIndex);
    side->ReverseTabsIndex[side->ActiveTabIndex]->SetFocusOnView();
  }

  void TabManager::OnDirChange()
  {
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(sender());
    if (!tab)
    {
      return;
    }
    SideContext* side = FindSideForTab(tab);

    const QDir& tabDir = tab->GetRootDir();
    side->Container->setTabText(side->TabsIndex[tab], tabDir.isRoot() ? ROOT_DIR_NAME : tabDir.dirName());
  }
} // namespace TF
