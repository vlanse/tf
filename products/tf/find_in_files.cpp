/*
 * find_in_files.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "find_in_files.h"
#include "ui_find_in_files.h"

namespace TF
{
  FindInFilesDialog::FindInFilesDialog(const Filesys::Dir& startDir, QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_FindInFilesDialog)
  {
    Ui->setupUi(this);
    Ui->SearchInEdit->setText(QString::fromStdWString(startDir.GetPath()));
  }

  void FindInFilesDialog::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
      accept();
    }
    QDialog::keyPressEvent(event);
  }
} // namespace TF
