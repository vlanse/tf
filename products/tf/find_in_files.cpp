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
#include <QThread>

#include <string>
#include <functional>

namespace TF
{
  class Worker: public QThread
  {
    Q_OBJECT
  public:
    Worker(QObject* parent);
    void StartSearch(const QString& where, const QString& what, const QString& content);
    void Cancel();
  protected:
    virtual void run();
  signals:
    void GotResult(const QString& item);
    void Progress(const QString& currentFolder);
    void Complete();
  private:
    void FilterCallback(const std::string& filename, Filesys::FileObjectType ftype);

    bool CancelFlag;
    QString Where;
    QRegExp What;
    QString Content;
    int Step;
  };

 #include "find_in_files.moc"

  Worker::Worker(QObject* parent)
    : QThread(parent)
    , CancelFlag(false)
    , Step(0)
  {
  }

  void Worker::StartSearch(const QString& where, const QString& what, const QString& content)
  {
    Where = where;
    What = QRegExp(what);
    What.setCaseSensitivity(Qt::CaseInsensitive);
    What.setPatternSyntax(QRegExp::Wildcard);
    Content = content;
    start();
  }

  void Worker::Cancel()
  {
    if (!isRunning())
    {
      return;
    }
    CancelFlag = true;
    QThread::wait();
    qDebug() << "Search canceled";
  }

  void Worker::run()
  {
    qDebug() << "Start search";
    QRegExp search = QRegExp(What);
    Step = 0;

    Filesys::WalkDir(
      Filesys::Dir(Where.toStdWString()),
      std::bind(
        &Worker::FilterCallback,
        this,
        std::placeholders::_1,
        std::placeholders::_2
      )
    );

    emit Complete();
  }

  void Worker::FilterCallback(const std::string& filename, Filesys::FileObjectType ftype)
  {
    if (CancelFlag)
    {
      QThread::terminate();
    }

    const QString& fullPath = QString::fromStdString(filename);
    const int lastSep = fullPath.indexOf(Filesys::PATH_SEPARATOR);
    const QString& folder = fullPath.left(lastSep);
    if (ftype == Filesys::FILE_DIRECTORY)
    {
      emit Progress(QString::fromStdString(filename));
    }

    const QStringList& items = fullPath.split(Filesys::PATH_SEPARATOR);
    if (items.empty())
    {
      return;
    }
    const QString& item = items.last();

    if (What.exactMatch(item))
    {
      if (Content.isEmpty())
      {
        emit GotResult(fullPath);
        return;
      }

      if (ftype == Filesys::FILE_REGULAR)
      {
        QFile f(fullPath);
        f.open(QIODevice::ReadOnly);
        QString part;
        do
        {
          if (CancelFlag)
          {
            QThread::terminate();
          }

          part = f.read(1024);
          if (part.indexOf(Content, Qt::CaseInsensitive) != -1)
          {
            emit GotResult(fullPath);
            return;
          }
        }
        while (!part.isEmpty());
        f.close();
      }
    }
  }

  FindInFilesDialog::FindInFilesDialog(const Filesys::Dir& startDir, QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_FindInFilesDialog)
    , Searcher(new Worker(this))
  {
    Ui->setupUi(this);
    Ui->SearchInEdit->setText(QString::fromStdWString(startDir.GetPath()));
    connect(Ui->ResultView, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(OnResultItemActivated(QListWidgetItem*)));
    connect(Searcher, SIGNAL(GotResult(const QString&)), SLOT(OnGotResult(const QString&)));
    connect(Searcher, SIGNAL(Progress(const QString&)), SLOT(OnProgress(const QString&)));
    connect(Searcher, SIGNAL(Complete()), SLOT(OnComplete()));
    Ui->SearchForEdit->setFocus();
  }

  FindInFilesDialog::~FindInFilesDialog()
  {
    Searcher->Cancel();
  }

  void FindInFilesDialog::OnResultItemActivated(QListWidgetItem* item)
  {
    Shell::OpenEditorForFile(item->text());
  }

  void FindInFilesDialog::StartSearch()
  {
    Ui->ResultView->clear();
    Searcher->StartSearch(Ui->SearchInEdit->text(), Ui->SearchForEdit->lineEdit()->text(), Ui->FindTextEdit->lineEdit()->text());
  }

  void FindInFilesDialog::OnGotResult(const QString& result)
  {
    Ui->ResultView->addItem(result);
  }

  void FindInFilesDialog::OnProgress(const QString& folder)
  {
    Ui->ProgressLabel->setText(folder);
  }

  void FindInFilesDialog::OnComplete()
  {
    Ui->ProgressLabel->setText("Search complete");
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
