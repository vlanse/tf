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
  MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
  {
    Ui = new Ui_MainWindow();
    Ui->setupUi(this);
    Tabs = new TabManager(Ui->LeftTabs, Ui->RightTabs, this);
    connect(Ui->ActionPreferences, SIGNAL(triggered()), SLOT(ShowSettings()));

    restoreGeometry(Settings::LoadMainWindowGeometry());
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
