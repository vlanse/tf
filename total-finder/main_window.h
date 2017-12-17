#pragma once

#include <QMainWindow>
#include <QDir>
#include <QCloseEvent>

class Ui_MainWindow;

namespace TotalFinder
{
  class TabManager;

  class MainWindow: public QMainWindow
  {
    Q_OBJECT
  public:
    MainWindow(QWidget* parent = 0);
  private slots:
    void ShowSettings();
  protected:
    virtual void closeEvent(QCloseEvent* event);
  private:
    Ui_MainWindow* Ui;
    TabManager* Tabs;
  };
} // namespace TotalFinder
