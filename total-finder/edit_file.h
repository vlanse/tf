#pragma once

#include <QDialog>
#include <QKeyEvent>

class  Ui_EditFileDialog;

namespace TotalFinder
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
} // namespace TotalFinder
