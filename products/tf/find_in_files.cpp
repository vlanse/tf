/*
 * find_in_files.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "find_in_files.h"
#include "ui_find_in_files.h"
#include "settings.h"
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
    bool FilterCallback(const std::string& filename, Filesys::FileObjectType ftype);

    bool CancelFlag;
    QString Where;
    QRegExp What;
    QString Content;
    int Step;
    QDir::Filters DirFilters;
  };

  class SearchResultModel: public QAbstractListModel
  {
    Q_OBJECT
  public:
    SearchResultModel(QObject* parent);
    void AddItem(const QString& item);
    void Clear();
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index = QModelIndex(), int role = Qt::DisplayRole) const;
  private:
    QStringList Data;
  };

 #include "find_in_files.moc"

  Worker::Worker(QObject* parent)
    : QThread(parent)
    , CancelFlag(false)
    , Step(0)
    , DirFilters(Settings::LoadDirFilters())
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

  bool Worker::FilterCallback(const std::string& filename, Filesys::FileObjectType ftype)
  {
    if (CancelFlag)
    {
      QThread::terminate();
      return false;
    }

    const QString& fullPath = QString::fromStdString(filename);
    const int lastSep = fullPath.lastIndexOf(Filesys::PATH_SEPARATOR);
    const QString& folder = fullPath.right(fullPath.size() - lastSep + 1);

    if (ftype == Filesys::FILE_DIRECTORY)
    {
      // TODO: use actual file attributes
      if (folder.startsWith(".") && !(DirFilters & QDir::Hidden))
      {
        return false;
      }
      emit Progress(QString::fromStdString(filename));
    }

    const QStringList& items = fullPath.split(Filesys::PATH_SEPARATOR);
    if (items.empty())
    {
      return true;
    }
    const QString& item = items.last();

    if (What.exactMatch(item))
    {
      if (Content.isEmpty())
      {
        emit GotResult(fullPath);
        return true;
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
            return true;
          }
        }
        while (!part.isEmpty());
        f.close();
      }
    }
    return true;
  }

  SearchResultModel::SearchResultModel(QObject* parent)
    : QAbstractListModel(parent)
  {
  }

  void SearchResultModel::AddItem(const QString& item)
  {
    const QModelIndex parent = QModelIndex();
    const int rows = rowCount(parent);
    QAbstractListModel::beginInsertRows(parent, rows, rows);
    Data << item;
    QAbstractListModel::endInsertRows();
  }

  void SearchResultModel::Clear()
  {
    const QModelIndex parent = QModelIndex();
    QAbstractListModel::beginRemoveRows(parent, 0, rowCount() - 1);
    Data.clear();
    QAbstractListModel::endRemoveRows();
  }

  int SearchResultModel::rowCount(const QModelIndex& parent) const
  {
    return Data.size();
  }

  QVariant SearchResultModel::data(const QModelIndex& index, int role) const
  {
    if (!index.isValid())
    {
      return QVariant();
    }

    const QString& currentItem = Data[index.row()];

   if (role == Qt::DisplayRole)
   {
     return currentItem;
   }

   return QVariant();
  }

  FindInFilesDialog::FindInFilesDialog(const Filesys::Dir& startDir, QWidget* parent)
    : QDialog(parent)
    , Ui(new Ui_FindInFilesDialog)
    , Searcher(new Worker(this))
    , Model(new SearchResultModel(this))
  {
    Ui->setupUi(this);
    Ui->SearchInEdit->setText(QString::fromStdWString(startDir.GetPath()));
    connect(Ui->ResultView, SIGNAL(activated(const QModelIndex&)), SLOT(OnResultItemActivated(const QModelIndex&)));
    Ui->ResultView->setModel(Model);
    connect(Searcher, SIGNAL(GotResult(const QString&)), SLOT(OnGotResult(const QString&)));
    connect(Searcher, SIGNAL(Progress(const QString&)), SLOT(OnProgress(const QString&)));
    connect(Searcher, SIGNAL(Complete()), SLOT(OnComplete()));
    Ui->SearchForEdit->setFocus();
  }

  FindInFilesDialog::~FindInFilesDialog()
  {
    Searcher->Cancel();
  }

  void FindInFilesDialog::OnResultItemActivated(const QModelIndex& item)
  {
    Shell::OpenEditorForFile(Model->data(item, Qt::DisplayRole).toString());
  }

  void FindInFilesDialog::StartSearch()
  {
    Model->Clear();
    Searcher->StartSearch(Ui->SearchInEdit->text(), Ui->SearchForEdit->lineEdit()->text(), Ui->FindTextEdit->lineEdit()->text());
  }

  void FindInFilesDialog::OnGotResult(const QString& result)
  {
    Model->AddItem(result);
  }

  void FindInFilesDialog::OnProgress(const QString& folder)
  {
    Ui->ProgressLabel->setText(folder);
  }

  void FindInFilesDialog::OnComplete()
  {
    Ui->ResultView->setFocus();
    qDebug() << "search complete";
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
