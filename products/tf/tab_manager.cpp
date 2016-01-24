/*
 * tab_manager.cpp
 *
 *  Created on: Dec 25, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "tab_manager.h"

#include "dir_view_panel.h"
#include "settings.h"

#include <QPushButton>
#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

namespace TF
{
  namespace
  {
    const char ROOT_DIR_NAME[] = "/";

    int AddTab(TabManager* tabs, DirViewPanel* tab, SideContext& side, int index = -1)
    {
      QObject::connect(tab, SIGNAL(DirChanged()), tabs, SLOT(OnDirChange()));

      const QString& tabName = tab->GetRootDir().isRoot() ? ROOT_DIR_NAME : tab->GetRootDir().dirName();
      const int tabIndex = side.Container->insertTab(index, tab, QIcon(), tabName);
      side.Container->setCurrentIndex(tabIndex);

      QObject::connect(tab, SIGNAL(DirChanged()), tabs, SLOT(OnDirChange()));
      QObject::connect(tab, SIGNAL(ChangeSideRequest()), tabs, SLOT(OnChangeSideRequest()));
      QObject::connect(tab, SIGNAL(AddNewTabRequest()), tabs, SLOT(OnAddNewTabRequest()));
      QObject::connect(tab, SIGNAL(CloseTabRequest()), tabs, SLOT(OnCloseTabRequest()));

      return tabIndex;
    }

    void RestoreTabs(const QJsonValue& tabsData, TabManager* tabs, SideContext& side)
    {
      QJsonArray tabsArray;
      if (!tabsData.isArray())
      {
        QJsonObject obj;
        obj["root"] = ROOT_DIR_NAME;
        tabsArray.append(obj);
      }
      else
      {
        tabsArray = tabsData.toArray();
      }

      foreach(const QJsonValue val, tabsArray)
      {
        if (!val.isObject())
        {
          continue;
        }
        const QJsonObject tab = val.toObject();

        DirViewPanel* tabPanel = new DirViewPanel();
        tabPanel->SetRootDir(QDir(tab["root"].toString(ROOT_DIR_NAME)));
        AddTab(tabs, tabPanel, side);
        tabPanel->SetFocusOnView();
      }
    }

    QJsonArray SaveTabs(const SideContext& side)
    {
      QJsonArray result;
      for (int i = 0; i < side.Container->count(); ++i)
      {
        DirViewPanel* tab = qobject_cast<DirViewPanel*>(side.Container->widget(i));
        QJsonObject obj;
        obj["root"] = tab->GetRootDir().path();
        result.append(obj);
      }
      return result;
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
    const QJsonDocument data = Settings::LoadTabs();
    RestoreTabs(data.object().value("left"), this, LeftSide);
    RestoreTabs(data.object().value("right"), this, RightSide);
  }

  void TabManager::SaveContext()
  {
    QJsonObject obj;
    obj["left"] = SaveTabs(LeftSide);
    obj["right"] = SaveTabs(RightSide);
    Settings::SaveTabs(QJsonDocument(obj));
  }

  void TabManager::OnChangeSideRequest()
  {
    LeftSide.Active = !LeftSide.Active;
    RightSide.Active = !RightSide.Active;

    SetFocusOnView();
  }

  void TabManager::OnAddNewTabRequest()
  {
    SideContext* side = GetActiveSide();
    DirViewPanel* tab = new DirViewPanel();

    const DirViewPanel* currentTab = qobject_cast<const DirViewPanel*>(side->Container->currentWidget());

    tab->SetRootDir(currentTab->GetRootDir());
    AddTab(this, tab, *side, side->Container->currentIndex() + 1);
    tab->SetFocusOnView();

    SaveContext();
  }

  void TabManager::OnCloseTabRequest()
  {
    SideContext* side = GetActiveSide();
    if (!side || side->Container->count() == 1)
    {
      return;
    }

    const int indexToRemove = side->Container->currentIndex();
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->widget(indexToRemove));
    side->Container->removeTab(indexToRemove);
    tab->deleteLater();

    const int currentIndex = side->Container->currentIndex();
    if (currentIndex != -1)
    {
      DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->widget(currentIndex));
      tab->SetFocusOnView();
    }

    SaveContext();
  }

  void TabManager::AddTabToTheLeft(DirViewPanel* tab)
  {
    AddTab(this, tab, LeftSide);

    SaveContext();
  }

  void TabManager::AddTabToTheRight(DirViewPanel* tab)
  {
    AddTab(this, tab, RightSide);

    SaveContext();
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
    if (LeftSide.Container->indexOf(tab) != -1)
    {
      return &LeftSide;
    }
    if (RightSide.Container->indexOf(tab) != -1)
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

    DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->currentWidget());
    tab->SetFocusOnView();
  }

  void TabManager::OnDirChange()
  {
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(sender());
    if (!tab)
    {
      return;
    }
    SideContext* side = FindSideForTab(tab);
    const int tabIndex = side->Container->indexOf(tab);

    const QDir& tabDir = tab->GetRootDir();
    side->Container->setTabText(tabIndex, tabDir.isRoot() ? ROOT_DIR_NAME : tabDir.dirName());
    side->Container->setTabToolTip(tabIndex, tabDir.path());

    SaveContext();
  }
} // namespace TF
