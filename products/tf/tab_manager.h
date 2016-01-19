/*
 * tab_manager.h
 *
 *  Created on: Dec 25, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QHash>
#include <QObject>
#include <QTabWidget>

namespace TF
{
  class DirViewPanel;

  struct SideContext
  {
    SideContext()
      : ActiveTabIndex(-1)
      , Active(false)
      , Container(0)
    {}

    QHash<DirViewPanel*, int> TabsIndex;
    QHash<int, DirViewPanel*> ReverseTabsIndex;
    int ActiveTabIndex;
    bool Active;
    QTabWidget* Container;
  };

  class TabManager: public QObject
  {
    Q_OBJECT
  public:
    TabManager(QTabWidget* leftContainer, QTabWidget* rightContainer, QObject* parent);
    void AddTabToTheLeft(DirViewPanel* tab);
    void AddTabToTheRight(DirViewPanel* tab);
    void CloseTab(DirViewPanel* tab);
    void SetFocusOnView();
  private slots:
    void OnDirChange();
    void OnChangeSideRequest();
  signals:
    void ChangeSideRequest();
  private:
    void RestoreContext();
    SideContext* GetActiveSide();
    SideContext* FindSideForTab(DirViewPanel* tab);

    SideContext LeftSide;
    SideContext RightSide;
  };
} // namespace TF
