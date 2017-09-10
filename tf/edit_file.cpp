/*
 * edit_file.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "edit_file.h"
#include <ui_edit_file.h>

namespace TF
{
  EditFileDialog::EditFileDialog(QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_EditFileDialog())
  {
    Ui->setupUi(this);
  }

  QString EditFileDialog::GetFileName() const
  {
    return FileName;
  }

  void EditFileDialog::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
      FileName = Ui->FileNameEdit->text();
      accept();
    }
    QDialog::keyPressEvent(event);
  }
} // namespace TF
