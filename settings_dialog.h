/*
 * settings_dialog.h
 *
 *  Created on: Dec 27, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QDialog>

class Ui_SettingsDialog;

namespace TF
{
  class SettingsDialog: public QDialog
  {
    Q_OBJECT
  public:
    SettingsDialog(QWidget* parent);
  private slots:
    void OnShowHiddenFilesStateChanged(int state);
    void OnShowSystemFilesStateChanged(int state);
  private:
    void Init();

    Ui_SettingsDialog* Ui;
  };
} // namespace TF
