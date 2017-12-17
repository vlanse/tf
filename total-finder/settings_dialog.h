#pragma once

#include <QDialog>

class Ui_SettingsDialog;

namespace TotalFinder
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
} // namespace TotalFinder
