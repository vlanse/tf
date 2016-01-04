/*
 * settings_dialog.cpp
 *
 *  Created on: Dec 27, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include <settings.h>

namespace TF
{
  SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_SettingsDialog)
  {
    Ui->setupUi(this);
    Ui->AdvancedSettingsView->setModel(new Settings::SettingsModel(this));
    Init();
  }

  void SettingsDialog::Init()
  {
    const QDir::Filters dirFilters = Settings::LoadDirFilters();
    Ui->ShowHiddenFilesBox->setChecked(dirFilters & QDir::Hidden);
    connect(Ui->ShowHiddenFilesBox, SIGNAL(stateChanged(int)), SLOT(OnShowHiddenFilesStateChanged(int)));

    Ui->ShowSystemFilesBox->setChecked(dirFilters & QDir::System);
    connect(Ui->ShowSystemFilesBox, SIGNAL(stateChanged(int)), SLOT(OnShowSystemFilesStateChanged(int)));
  }

  void SettingsDialog::OnShowHiddenFilesStateChanged(int state)
  {
    const QDir::Filters dirFilters = Settings::LoadDirFilters();
    Settings::SaveDirFilters(state == Qt::Checked ? dirFilters | QDir::Hidden : dirFilters & ~QDir::Hidden);
  }

  void SettingsDialog::OnShowSystemFilesStateChanged(int state)
  {
    const QDir::Filters dirFilters = Settings::LoadDirFilters();
    Settings::SaveDirFilters(state == Qt::Checked ? dirFilters | QDir::System : dirFilters & ~QDir::System);
  }
} // namespace TF
