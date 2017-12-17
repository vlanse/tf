#pragma once

#include <QDialog>
#include <QKeyEvent>

class  Ui_CreateDirDialog;

namespace TotalFinder
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
} // namespace TotalFinder
