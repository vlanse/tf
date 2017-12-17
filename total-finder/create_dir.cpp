#include "create_dir.h"
#include "ui_create_dir.h"

namespace TotalFinder
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
} // namespace TotalFinder
