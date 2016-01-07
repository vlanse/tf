/*
 * create_dir.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#pragma once

#include <QDialog>
#include <QKeyEvent>

class  Ui_CreateDirDialog;

namespace TF
{
  class CreateDirDialog: public QDialog
  {
    Q_OBJECT
  public:
    CreateDirDialog(QWidget* parent);
    QString GetDirName() const;
  protected:
    virtual void keyPressEvent(QKeyEvent* event);
  private:
    QString DirName;
    Ui_CreateDirDialog* Ui;
  };
} // namespace TF
