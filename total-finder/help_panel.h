#pragma once

#include "base_panel.h"
#include "tab_context.h"

#include <QFocusEvent>
#include <QKeyEvent>
#include <QWidget>

class Ui_HelpPanel;

namespace TotalFinder
{
  class TabContext;

  class HelpPanel: public BasePanel
  {
  Q_OBJECT

  public:
    HelpPanel(const TabContext& context, QWidget* parent = 0);
    virtual void SetFocus();
    virtual QString GetName() const;

  private:
    virtual void KeyHandler(Qt::KeyboardModifiers modifier, Qt::Key key);

    Ui_HelpPanel* Ui;
    TabContext Context;
  };
} // namespace TotalFinder
