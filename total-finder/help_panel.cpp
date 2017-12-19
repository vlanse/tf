#include "help_panel.h"
#include "ui_help_panel.h"

#include "settings.h"

#include <QDebug>
#include <QMenu>

#include <functional>

namespace TotalFinder
{
  HelpPanel::HelpPanel(const TabContext& context, QWidget* parent)
    : BasePanel(parent)
    , Context(context)
  {
    Ui = new Ui_HelpPanel();
    Ui->setupUi(this);

    BasePanel::InstallKeyEventFilter();
  }

  QString HelpPanel::GetName() const
  {
    return QString("Keymap");
  }

  void HelpPanel::SetFocus()
  {
    this->setFocus();
  }

  void HelpPanel::KeyHandler(Qt::KeyboardModifiers modifiers, Qt::Key key)
  {
  }
} // namespace TotalFinder
