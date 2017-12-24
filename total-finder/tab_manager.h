#pragma once

#include <QHash>
#include <QMainWindow>
#include <QObject>
#include <QTabWidget>

#include "base_panel.h"

namespace TotalFinder
{
  class DirViewPanel;

  struct SideContext
  {
    SideContext()
      : Active(false)
      , Container(nullptr)
    {}

    bool Active;
    QTabWidget* Container;
  };

  class MyTabWidget: public QTabWidget
  {
    Q_OBJECT
  public:
    MyTabWidget(QWidget* parent);
  protected:
    void resizeEvent(QResizeEvent *event);
  };

  class TabManager: public QObject
  {
    Q_OBJECT

  public:
    TabManager(
      QTabWidget* leftContainer,
      QTabWidget* rightContainer,
      QMainWindow* mainWindow
    );
    BasePanel* GetOppositeTab(BasePanel* current) const;
    void AddHelpPanel();

  public slots:
    void OnChangeSideRequest(bool force);

  private slots:
    void OnTabTitleChange(const QString& newTitle);
    void OnAddNewTabRequest();
    void OnCloseTabRequest();
    void OnSwitchNextTab(int direction);
    void OnUpdateStatusText(const QString& text);
    void SaveContext();

  private:
    void RestoreContext();
    void SetFocusOnView();

    const SideContext* GetActiveSide() const;
    const SideContext* FindSideForTab(BasePanel* tab) const;

    SideContext LeftSide;
    SideContext RightSide;
    QMainWindow* MainWindow;
  };
} // namespace TotalFinder
