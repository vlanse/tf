/*
 * edit_file.h
 *
 *  Created on: Jan 9, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QDialog>
#include <QKeyEvent>

class  Ui_EditFileDialog;

namespace TF
{
  class EditFileDialog: public QDialog
  {
    Q_OBJECT
  public:
    EditFileDialog(QWidget* parent);
    QString GetFileName() const;
  protected:
    virtual void keyPressEvent(QKeyEvent* event);
  private:
    QString FileName;
    Ui_EditFileDialog* Ui;
  };
} // namespace TF
