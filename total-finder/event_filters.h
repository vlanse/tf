#pragma once

#include <QFocusEvent>
#include <QKeyEvent>
#include <QList>
#include <QObject>

namespace TotalFinder
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

  class FocusFilter: public QObject
  {
    Q_OBJECT
  public:
    FocusFilter(QObject* parent);
  signals:
    void GotFocusEvent(QFocusEvent event);
  protected:
    bool eventFilter(QObject* object, QEvent* event);
  };
} // namespace TotalFinder
