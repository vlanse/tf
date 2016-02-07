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
      : Active(false)
      , Container(0)
    {}

    bool Active;
    QTabWidget* Container;
  };

  class TabManager: public QObject
  {
    Q_OBJECT
  public:
    TabManager(QTabWidget* leftContainer, QTabWidget* rightContainer, QObject* parent);
    DirViewPanel* GetOppositeTab(DirViewPanel* current) const;
  public slots:
    void OnChangeSideRequest(bool force);
  private slots:
    void OnDirChange();
    void OnAddNewTabRequest();
    void OnCloseTabRequest();
    void SaveContext();
  private:
    void RestoreContext();
    void SetFocusOnView();

    const SideContext* GetActiveSide() const;
    const SideContext* FindSideForTab(DirViewPanel* tab) const;

    SideContext LeftSide;
    SideContext RightSide;
  };
} // namespace TF
