#pragma once

#include <QHash>
#include <QObject>
#include <QTabWidget>

namespace TotalFinder
{
  class DirViewPanel;
  class BasePanel;

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
    BasePanel* GetOppositeTab(BasePanel* current) const;

  public slots:
    void OnChangeSideRequest(bool force);

  private slots:
    void OnTabTitleChange(const QString& newTitle);
    void OnAddNewTabRequest();
    void OnCloseTabRequest();
    void SaveContext();

  private:
    void RestoreContext();
    void SetFocusOnView();

    const SideContext* GetActiveSide() const;
    const SideContext* FindSideForTab(BasePanel* tab) const;

    SideContext LeftSide;
    SideContext RightSide;
  };
} // namespace TotalFinder
