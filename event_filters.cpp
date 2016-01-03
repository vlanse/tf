/*
 * event_filters.cpp
 *
 *  Created on: Dec 27, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "event_filters.h"

#include <QDebug>
#include <QKeyEvent>

namespace TF
{
  KeyPressFilter::KeyPressFilter(QObject* parent)
    : QObject(parent)
  {
  }

  bool KeyPressFilter::eventFilter(QObject* object, QEvent* event)
  {
    if (event->type() == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
      emit KeyPressed(*keyEvent);

      if (CodesToIntercept.contains(keyEvent->key()))
      {
        qDebug() << "Intercepted key: " <<  keyEvent->text();
        return true;
      }
    }
    return QObject::eventFilter(object, event);
  }

  void KeyPressFilter::InterceptKey(int keyCode)
  {
    CodesToIntercept << keyCode;
  }

  FocusFilter::FocusFilter(QObject* parent)
    : QObject(parent)
  {
  }

  bool FocusFilter::eventFilter(QObject* object, QEvent* event)
  {
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)
    {
      QFocusEvent *focusEvent = static_cast<QFocusEvent*>(event);
      emit GotFocusEvent(*focusEvent);
    }
    return QObject::eventFilter(object, event);
  }
} // namespace TF
