/*
 * find_in_files.h
 *
 *  Created on: Jan 31, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>

#include <common/filesystem.h>

class  Ui_FindInFilesDialog;

namespace TF
{
  class FindInFilesDialog: public QDialog
  {
    Q_OBJECT
  public:
    FindInFilesDialog(const Filesys::Dir& startDir, QWidget* parent);
  protected:
    virtual void keyPressEvent(QKeyEvent* event);
  private slots:
    void OnResultItemActivated(QListWidgetItem* item);
  private:
    void StartSearch();

    Ui_FindInFilesDialog* Ui;
  };
} // namespace TF
