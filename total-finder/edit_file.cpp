#include "edit_file.h"
#include <ui_edit_file.h>

namespace TotalFinder
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
} // namespace TotalFinder
