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
    BasePanel(QWidget* parent = 0);

  signals:
    void ChangeSideRequest(bool force);
    void AddNewTabRequest();
    void CloseTabRequest();

  private slots:
    void OnKeyPressed(QKeyEvent event);

  private:
    virtual void KeyHandler(Qt::KeyboardModifiers modifier, Qt::Key key) = 0;

  protected:
    KeyPressFilter* InstallKeyEventFilter();
  };
} // namespace TotalFinder
