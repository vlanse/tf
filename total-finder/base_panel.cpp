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
    KeyHandler(event.modifiers(), static_cast<Qt::Key>(event.key()));
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
