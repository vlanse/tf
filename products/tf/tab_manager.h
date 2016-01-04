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

  class TabManager: public QObject
  {
    Q_OBJECT
  public:
    TabManager(QTabWidget* container, QObject* parent);
    void AddTab(DirViewPanel* tab);
    void CloseTab(DirViewPanel* tab);
    void SetFocusOnView();
  private slots:
    void OnDirChange();
  signals:
    void ChangeSideRequest();
  private:
    QTabWidget* Container;
    QHash<DirViewPanel*, int> TabsIndex;
    QHash<int, DirViewPanel*> ReverseTabsIndex;
    int ActiveTabIndex;
  };
} // namespace TF
