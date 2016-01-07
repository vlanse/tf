/*
 * main_window.cpp
 *
 *  Created on: Dec 19, 2015
 *      Author: Vladimir Semenov
 */

#include "main_window.h"
#include "ui_main_window.h"

#include "dir_model.h"
#include "dir_view_panel.h"
#include "event_filters.h"
#include "settings.h"
#include "settings_dialog.h"
#include "tab_manager.h"

#include <QDebug>
#include <QDir>

namespace TF
{
  namespace
  {
    void InitTabManager(TabManager*& tabs, QTabWidget* container, QObject* parent)
    {
      tabs = new TabManager(container, parent);

      DirViewPanel* tab = new DirViewPanel();

      // TODO: restore tabs from settings
      tab->SetRootDir(QDir("/Users/vsemenov/test"));

      tabs->AddTab(tab);
      QObject::connect(tabs, SIGNAL(ChangeSideRequest()), parent, SLOT(OnChangeSideRequest()));

      tab->SetFocusOnView();
    }
  } // namespace

  MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
  {
    Ui = new Ui_MainWindow();
    Ui->setupUi(this);

    InitTabManager(RightTabs, Ui->RightTabs, this);
    InitTabManager(LeftTabs, Ui->LeftTabs, this);
    ActiveTabs = LeftTabs;

    connect(Ui->ActionPreferences, SIGNAL(triggered()), SLOT(ShowSettings()));
    restoreGeometry(Settings::LoadMainWindowGeometry());
  }

  void MainWindow::OnChangeSideRequest()
  {
    ActiveTabs = ActiveTabs == LeftTabs ? RightTabs : LeftTabs;
    ActiveTabs->SetFocusOnView();
  }

  void MainWindow::ShowSettings()
  {
    qDebug() << "Settings window requested";
    SettingsDialog settings(this);
    settings.exec();
  }

  void MainWindow::closeEvent(QCloseEvent* event)
  {
    Settings::SaveMainWindowGeometry(saveGeometry());
    QMainWindow::closeEvent(event);
  }
} // namespace TF
