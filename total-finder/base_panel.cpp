#include "base_panel.h"
#include "event_filters.h"

#include <QDesktopServices>

namespace TotalFinder
{
  namespace
  {
    void InstallFilterForAllChildren(QWidget* parent, QObject* filter)
    {
      QList<QWidget*> widgets = parent->findChildren<QWidget*>();
      for (auto &widget : widgets) {
        widget->installEventFilter(filter);
      }
    }
  }

  BasePanel::BasePanel(QWidget* parent)
    : QWidget(parent)
  {
  }

  void BasePanel::OnKeyPressed(QKeyEvent event)
  {
    const Qt::KeyboardModifiers modifiers = event.modifiers();
    auto key = static_cast<Qt::Key>(event.key());

    if (modifiers == Qt::NoModifier) {
      if (key == Qt::Key_Tab)
      {
        qDebug("Request to change side detected");
        emit ChangeSideRequest(true);
        return;
      }
    }
    else if (modifiers == (Qt::KeypadModifier | Qt::ControlModifier))
    {
      if (key == Qt::Key_Left)
      {
        emit SwitchNextTabRequest(BasePanel::Left);
      }
      else if (key == Qt::Key_Right)
      {
        emit SwitchNextTabRequest(BasePanel::Right);
      }
    }
    else if (modifiers == Qt::ControlModifier)
    {
      if (key == Qt::Key_T)
      {
        emit AddNewTabRequest();
      }
      else if (key == Qt::Key_W)
      {
        emit CloseTabRequest();
      }
    }

    KeyHandler(modifiers, key);
  }

  KeyPressFilter* BasePanel::InstallKeyEventFilter(const QWidgetList& widgets)
  {
    KeyPressFilter* baseKeyDetector = new KeyPressFilter(this);
    baseKeyDetector->InterceptKey(Qt::Key_Tab);
    baseKeyDetector->InterceptModifier(Qt::ControlModifier);
    connect(baseKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), SLOT(OnKeyPressed(QKeyEvent)));
    if (widgets.empty())
    {
      InstallFilterForAllChildren(this, baseKeyDetector);
    }
    else
    {
      for (auto w: widgets)
      {
        w->installEventFilter(baseKeyDetector);
      }
    }
    return baseKeyDetector;
  }
} // namespace TotalFinder
