#include "event_filters.h"

#include <QDebug>

namespace TotalFinder
{
  KeyPressFilter::KeyPressFilter(QObject* parent)
    : QObject(parent)
  {
  }

  bool KeyPressFilter::eventFilter(QObject* object, QEvent* event)
  {
    if (event->type() == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
      emit KeyPressed(*keyEvent);

      if (CodesToIntercept.contains(keyEvent->key()))
      {
        qDebug() << "Intercepted key: " <<  keyEvent->text();
        return true;
      }

      for (auto m: ModifiersToIntercept)
      {
        if (keyEvent->modifiers() & m)
        {
          qDebug() << "Intercepted key modifier: " <<  m;
          return true;
        }
      }
    }
    return QObject::eventFilter(object, event);
  }

  void KeyPressFilter::InterceptKey(int keyCode)
  {
    CodesToIntercept << keyCode;
  }

  void KeyPressFilter::InterceptModifier(int modifier)
  {
    ModifiersToIntercept << modifier;
  }

  FocusFilter::FocusFilter(QObject* parent)
    : QObject(parent)
  {
  }

  bool FocusFilter::eventFilter(QObject* object, QEvent* event)
  {
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)
    {
      QFocusEvent *focusEvent = dynamic_cast<QFocusEvent*>(event);
      emit GotFocusEvent(*focusEvent);
    }
    return QObject::eventFilter(object, event);
  }
} // namespace TotalFinder
