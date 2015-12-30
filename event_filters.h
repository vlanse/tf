/*
 * event_filters.h
 *
 *  Created on: Dec 27, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QKeyEvent>
#include <QList>
#include <QObject>

namespace TF
{
  class KeyPressFilter: public QObject
  {
    Q_OBJECT
  public:
    KeyPressFilter(QObject* parent);
    void InterceptKey(int keyCode);
  signals:
    void KeyPressed(QKeyEvent event);
  protected:
    bool eventFilter(QObject* object, QEvent* event);
  private:
    QList<int> CodesToIntercept;
  };
} // namespace TF
