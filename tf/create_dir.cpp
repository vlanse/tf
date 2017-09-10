/*
 * create_dir.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "create_dir.h"
#include "ui_create_dir.h"

namespace TF
{
  CreateDirDialog::CreateDirDialog(QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_CreateDirDialog())
  {
    Ui->setupUi(this);
  }

  QString CreateDirDialog::GetDirName() const
  {
    return DirName;
  }

  void CreateDirDialog::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
      DirName = Ui->DirNameEdit->text();
      accept();
    }
    QDialog::keyPressEvent(event);
  }
} // namespace TF
