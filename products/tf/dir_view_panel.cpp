/*
 * dir_view_panel.cpp
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#include "dir_view_panel.h"
#include "ui_dir_view_panel.h"

#include "create_dir.h"
#include "dir_model.h"
#include "edit_file.h"
#include "event_filters.h"
#include "settings.h"

#include <common/filesystem.h>

#include <QDebug>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

#include <functional>

namespace TF
{
  namespace
  {
    void PostKeyEvent(QWidget* widget, const QKeyEvent& event)
    {
      QKeyEvent * pressEvent = new QKeyEvent (QEvent::KeyPress, event.key(), event.modifiers(), event.text());
      QKeyEvent * releaseEvent = new QKeyEvent (QEvent::KeyRelease, event.key(), event.modifiers(), event.text());
      qApp->postEvent(widget, pressEvent);
      qApp->postEvent(widget, releaseEvent);
    }

    void ShellOpenEditorForFile(const QString& file)
    {
      QStringList args;
      args << "-a" << "Sublime Text 2" << file;
      qDebug() << "Open editor for file with args " << args;
      QProcess::startDetached("open", args);
    }

    void EnsureFileExists(const QString& filePath)
    {
      QFile f(filePath);
      if (!f.exists())
      {
        f.open(QIODevice::ReadWrite);
        f.close();
      }
    }
  } // namespace

  class QuickSearchKeyEventHandler: public QObject
  {
    Q_OBJECT
  public:
    typedef std::function<void (QKeyEvent)> HandlerFunc;

    QuickSearchKeyEventHandler(HandlerFunc handler, QObject* parent);
  private slots:
    void OnKeyPressed(QKeyEvent event);
  private:
    HandlerFunc Handler;
  };

#include "dir_view_panel.moc"

  QuickSearchKeyEventHandler::QuickSearchKeyEventHandler(HandlerFunc handler, QObject* parent)
    : QObject(parent)
    , Handler(handler)
  {}

  void QuickSearchKeyEventHandler::OnKeyPressed(QKeyEvent event)
  {
    Handler(event);
  }

  DirViewPanel::DirViewPanel(QWidget* parent)
    : QWidget(parent)
    , Model(new DirModel(this))
    , QuickSearchHandlerDelegate(new QuickSearchKeyEventHandler(std::bind(&DirViewPanel::QuickSearchHandler, this, std::placeholders::_1), this))
    , QuickSearchMode(false)
    , CurrentRow(0)
  {
    Ui = new Ui_DirViewPanel();
    Ui->setupUi(this);
    Ui->SearchEdit->hide();

    Model->SetRoot(QDir("/"));
    Ui->DirView->setModel(Model);
    connect(Model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(OnDirModelChange()));
    connect(Ui->DirView->selectionModel(), SIGNAL(currentChanged(const QModelIndex, const QModelIndex&)), SLOT(OnSelectionChanged(const QModelIndex&, const QModelIndex&)));

    connect(Ui->DirView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), SLOT(OnHeaderGeometryChanged()));
    Ui->DirView->horizontalHeader()->restoreState(Settings::LoadViewHeaderState());

    KeyPressFilter* viewKeyDetector = new KeyPressFilter(this);
    viewKeyDetector->InterceptKey(Qt::Key_Tab);
    connect(viewKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), SLOT(OnKeyPressed(QKeyEvent)));
    connect(viewKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), QuickSearchHandlerDelegate, SLOT(OnKeyPressed(QKeyEvent)));
    Ui->DirView->installEventFilter(viewKeyDetector);

    KeyPressFilter* searchKeyDetector = new KeyPressFilter(this);
    connect(searchKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), QuickSearchHandlerDelegate, SLOT(OnKeyPressed(QKeyEvent)));
    Ui->SearchEdit->installEventFilter(searchKeyDetector);

    FocusFilter* focusDetector = new FocusFilter(this);
    connect(focusDetector, SIGNAL(GotFocusEvent(QFocusEvent)), SLOT(OnFocusEvent(QFocusEvent)));
    Ui->DirView->installEventFilter(focusDetector);

    connect(Ui->DirView, SIGNAL(activated(const QModelIndex&)), SLOT(OnItemActivated(const QModelIndex&)));
    connect(Ui->AddressBar, SIGNAL(returnPressed()), SLOT(OnAddressBarEnter()));
    connect(Ui->SearchEdit, SIGNAL(textEdited(const QString&)), SLOT(OnQuickSearch(const QString&)));
  }

  void DirViewPanel::OnHeaderGeometryChanged()
  {
    qDebug() << "Save view header state";
    const QByteArray& state = Ui->DirView->horizontalHeader()->saveState();
    Settings::SaveViewHeaderState(state);
  }

  void DirViewPanel::OnDirModelChange()
  {
    // adjust currently selected item
    QModelIndex currentIndex = Model->GetIndex(CurrentSelection);
    if (!currentIndex.isValid())
    {
      currentIndex = Model->index(0, 0);
      if (Model->rowCount() >= CurrentRow)
      {
        currentIndex = Model->index(CurrentRow - 1, 0);
      }
    }
    Ui->DirView->selectionModel()->setCurrentIndex(currentIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

    CurrentRow = currentIndex.row();
    CurrentSelection = Model->GetItem(currentIndex);
  }

  void DirViewPanel::OnSelectionChanged(const QModelIndex& current, const QModelIndex& /*previous*/)
  {
    if (!current.isValid())
    {
      return;
    }
    CurrentSelection = Model->GetItem(current);
    CurrentRow = current.row();
  }

  void DirViewPanel::SetRootDir(const QDir& dir)
  {
    if (!Model)
    {
      return;
    }
    HandleDirSelection(dir);
  }

  QDir DirViewPanel::GetRootDir() const
  {
    if (!Model)
    {
      return QDir();
    }
    return Model->GetRoot();
  }

  void DirViewPanel::SetFocusOnView()
  {
    Ui->DirView->setFocus();
  }

  void DirViewPanel::OnFocusEvent(QFocusEvent event)
  {
    if (QuickSearchMode && event.gotFocus())
    {
      SwitchQuickSearchMode();
    }
  }

  void DirViewPanel::OnItemActivated(const QModelIndex& index)
  {
    HandleItemSelection(Model->GetItem(index).absoluteFilePath());
  }

  void DirViewPanel::OnAddressBarEnter()
  {
    HandleItemSelection(QFileInfo(Ui->AddressBar->text()));
    Ui->DirView->setFocus();
  }

  void DirViewPanel::SwitchQuickSearchMode()
  {
    QuickSearchMode = !QuickSearchMode;

    Ui->SearchEdit->setVisible(QuickSearchMode);
    Ui->SearchEdit->clear();

    if (QuickSearchMode)
    {
      Ui->SearchEdit->setFocus();
    }
    else
    {
      Ui->DirView->setFocus();
    }
  }

  void DirViewPanel::QuickSearchHandler(QKeyEvent event)
  {
    if (
      !event.text().isEmpty() && !QuickSearchMode &&
      !(event.key() == Qt::Key_Return || event.key() == Qt::Key_Tab)
      )
    {
      qDebug() << "Quick search mode switch, key is\"" << event.text() << "\"";
      SwitchQuickSearchMode();
      PostKeyEvent(Ui->SearchEdit, event);
      return;
    }

    if (QuickSearchMode)
    {
      if (event.key() == Qt::Key_Up || event.key() == Qt::Key_Down)
      {
        SwitchQuickSearchMode();
        PostKeyEvent(Ui->DirView, event);
      }
      else if ((event.modifiers() & Qt::MetaModifier) && event.key() == Qt::Key_C)
      {
        Ui->SearchEdit->clear();
      }
      else if (event.key() == Qt::Key_Escape)
      {
        SwitchQuickSearchMode();
      }
      else if (event.key() == Qt::Key_Return)
      {
        SwitchQuickSearchMode();
        HandleItemSelection(CurrentSelection);
      }
    }
  }

  void DirViewPanel::OnQuickSearch(const QString& search)
  {
    qDebug() << "Quick search for:" << search;
    const QModelIndexList& indices = Model->Search(search + "*");
    if (indices.empty())
    {
      QKeyEvent ev(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
      PostKeyEvent(Ui->SearchEdit, ev);
      return;
    }
    Ui->DirView->setCurrentIndex(indices[0]);
  }

  void DirViewPanel::OnKeyPressed(QKeyEvent event)
  {
    if (event.modifiers() == Qt::NoModifier)
    {
      if (event.key() == Qt::Key_Return)
      {
        HandleItemSelection(CurrentSelection);
      }
      else if (event.key() == Qt::Key_Tab)
      {
        qDebug("Request to change side detected");
        emit ChangeSideRequest();
      }
      else if (event.key() == Qt::Key_F4)
      {
        qDebug() << "Request to edit file detected:" << CurrentSelection.absoluteFilePath();
        ShellOpenEditorForFile(CurrentSelection.absoluteFilePath());
      }
      else if (event.key() == Qt::Key_Delete) // Fn + Backspace
      {
        qDebug() << "Request to delete item, path is" << CurrentSelection.absoluteFilePath();
        Filesys::RemoveDirRecursive(Filesys::Dir(CurrentSelection.absoluteFilePath().toStdWString()));
      }
      else if (event.key() == Qt::Key_F7)
      {
        QString newDirPath = Model->GetRoot().absolutePath() + "/";
        CreateDirDialog* dlg = new CreateDirDialog(this);
        dlg->exec();

        newDirPath += dlg->GetDirName();
        qDebug() << "Request to create directory, path is" << newDirPath;
        Filesys::CreateDir(newDirPath.toStdWString());
      }
      else if (event.key() == Qt::Key_F5)
      {
        qDebug() << "Request to copy file or dir" << CurrentSelection.absoluteFilePath();
        Filesys::Copy(
          Filesys::FileInfo(CurrentSelection.absoluteFilePath().toStdWString()),
          Filesys::FileInfo(L"/Users/vsemenov/test/zzzz/")
        );
      }
    }
    else if (event.modifiers() == Qt::ShiftModifier)
    {
      if (event.key() == Qt::Key_F4)
      {
        QString filePath = Model->GetRoot().absolutePath() + "/";
        EditFileDialog* dlg = new EditFileDialog(this);
        dlg->exec();

        filePath += dlg->GetFileName();
        qDebug() << "Request to edit file by entered name, full path is" << filePath;
        EnsureFileExists(filePath);
        ShellOpenEditorForFile(filePath);
      }
    }
    else if (event.modifiers() == Qt::ControlModifier)
    {
      if (event.key() == Qt::Key_T)
      {
        qDebug() << "New tab request";
        emit AddNewTabRequest();
      }
      else if (event.key() == Qt::Key_W)
      {
        qDebug() << "Close tab request";
        emit CloseTabRequest();
      }
    }
  }

  void DirViewPanel::HandleItemSelection(const QFileInfo& item)
  {
    if (item.isDir())
    {
      HandleDirSelection(item.absoluteFilePath());
      return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(item.absoluteFilePath()));
  }

  void DirViewPanel::HandleDirSelection(const QDir& dir)
  {
    qDebug() << "Set dir in view:" << dir.absolutePath();
    const QDir& previousDir = Model->GetRoot();
    Model->SetRoot(dir);
    Ui->AddressBar->setText(Model->GetRoot().absolutePath());

    const QModelIndex& index = Model->GetIndex(previousDir);
    Ui->DirView->setCurrentIndex(index.isValid() ? index : Model->index(0,0));
    Ui->DirView->scrollTo(Ui->DirView->selectionModel()->currentIndex());

    emit DirChanged();
  }
} // namespace TF

