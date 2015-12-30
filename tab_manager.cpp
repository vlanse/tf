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
  } // namespace

  TabManager::TabManager(QTabWidget* container, QObject* parent)
    : QObject(parent)
    , Container(container)
    , ActiveTabIndex(-1)
  {
  }

  void TabManager::AddTab(DirViewPanel* tab)
  {
    connect(tab, SIGNAL(DirChanged()), SLOT(OnDirChange()));
    const int tabIndex = Container->insertTab(0, tab, QIcon(), tab->GetRootDir().dirName());;

    TabsIndex[tab] = tabIndex;
    ReverseTabsIndex[tabIndex] = tab;
    Container->setCurrentIndex(tabIndex);
    ActiveTabIndex = tabIndex;

    connect(tab, SIGNAL(DirChanged()), SLOT(OnDirChange()));
    connect(tab, SIGNAL(ChangeSideRequest()), SIGNAL(ChangeSideRequest()));
  }

  void TabManager::CloseTab(DirViewPanel* tab)
  {
  }

  void TabManager::SetFocusOnView()
  {
    if (ActiveTabIndex == -1)
    {
      qDebug("Invalid tab index in tab manager");
      return;
    }
    Container->setCurrentIndex(ActiveTabIndex);
    ReverseTabsIndex[ActiveTabIndex]->SetFocusOnView();
  }

  void TabManager::OnDirChange()
  {
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(sender());
    if (!tab || !TabsIndex.contains(tab))
    {
      return;
    }
    const QDir& tabDir = tab->GetRootDir();
    Container->setTabText(TabsIndex[tab], tabDir.isRoot() ? ROOT_DIR_NAME : tabDir.dirName());
  }
} // namespace TF
