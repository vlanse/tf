/*
 * find_in_files.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "find_in_files.h"
#include "ui_find_in_files.h"
#include "shell_utils.h"

#include <common/filesystem.h>

#include <QDebug>

#include <string>
#include <functional>

namespace TF
{
  namespace
  {
    void FilterCallback(const QRegExp& searchFor, const QString& findText, QStringList& result, const std::string& filename)
    {
      const QString& fullPath = QString::fromStdString(filename);
      const QStringList& items = fullPath.split(Filesys::PATH_SEPARATOR);
      if (items.empty())
      {
        return;
      }
      const QString& item = items.last();

      if (searchFor.exactMatch(item))
      {
        if (findText.isEmpty())
        {
          result.append(fullPath);
          return;
        }

        QFile f(fullPath);
        f.open(QIODevice::ReadOnly);
        QByteArray data;
        do
        {
          const QString& part = f.read(1024);
          if (part.indexOf(findText, Qt::CaseInsensitive) != -1)
          {
            result.append(fullPath);
            return;
          }
        }
        while (!data.isEmpty());
        f.close();
      }
    }
  } // namespace

  FindInFilesDialog::FindInFilesDialog(const Filesys::Dir& startDir, QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_FindInFilesDialog)
  {
    Ui->setupUi(this);
    Ui->SearchInEdit->setText(QString::fromStdWString(startDir.GetPath()));
    connect(Ui->ResultView, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(OnResultItemActivated(QListWidgetItem*)));
  }

  void FindInFilesDialog::OnResultItemActivated(QListWidgetItem* item)
  {
    Shell::OpenEditorForFile(item->text());
  }

  void FindInFilesDialog::StartSearch()
  {
    Ui->ResultView->clear();
    const QString& searchIn = Ui->SearchInEdit->text();

    QRegExp search = QRegExp(Ui->SearchForEdit->text());
    search.setCaseSensitivity(Qt::CaseInsensitive);
    search.setPatternSyntax(QRegExp::Wildcard);

    QStringList result;

    const QString& findText = Ui->FindTextEdit->text();

    Filesys::WalkDir(
      Filesys::Dir(searchIn.toStdWString()),
      std::bind(
        FilterCallback,
        std::cref(search),
        std::cref(findText),
        std::ref(result),
        std::placeholders::_1
      )
    );

    foreach(const QString& item, result)
    {
      Ui->ResultView->addItem(item);
    }
  }

  void FindInFilesDialog::keyPressEvent(QKeyEvent* event)
  {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
      StartSearch();
    }
    else if (event->key() == Qt::Key_Escape)
    {
      accept();
    }
    QDialog::keyPressEvent(event);
  }
} // namespace TF
