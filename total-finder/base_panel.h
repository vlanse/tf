#pragma once

#include <QFocusEvent>
#include <QKeyEvent>
#include <QWidget>

#include "event_filters.h"

namespace TotalFinder
{
  class QuickSearchKeyEventHandler;

  class BasePanel: public QWidget
  {
  Q_OBJECT

  public:
    enum TabSwitchDirection
    {
      Right = 1,
      Left = 2
    };

    BasePanel(QWidget* parent = 0);
    virtual void SetFocus() = 0;
    virtual QString GetName() const = 0;

  signals:
    void TitleChanged(const QString& newTitle);
    void ChangeSideRequest(bool force);
    void AddNewTabRequest();
    void CloseTabRequest();
    void SwitchNextTabRequest(int direction);

  private slots:
    void OnKeyPressed(QKeyEvent event);

  private:
    virtual void KeyHandler(Qt::KeyboardModifiers modifier, Qt::Key key) = 0;

  protected:
    KeyPressFilter* InstallKeyEventFilter(const QWidgetList& widgets = QWidgetList());
  };
} // namespace TotalFinder
