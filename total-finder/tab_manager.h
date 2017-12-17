#pragma once

#include <QHash>
#include <QObject>
#include <QTabWidget>

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
      QObject* parent
    );
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
} // namespace TotalFinder
