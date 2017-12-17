#include "dir_view_panel.h"
#include "ui_dir_view_panel.h"

#include "create_dir.h"
#include "dir_model.h"
#include "edit_file.h"
#include "event_filters.h"
#include "find_in_files.h"
#include "settings.h"
#include "shell_utils.h"

#include <common/filesystem.h>

#include <QDebug>
#include <QDesktopServices>
#include <QMenu>
#include <QProcess>
#include <QUrl>

#include <functional>

namespace TotalFinder
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

  DirViewPanel::DirViewPanel(const TabContext& context, QWidget* parent)
    : BasePanel(parent)
    , Model(new DirModel(this))
    , QuickSearchHandlerDelegate(
        new QuickSearchKeyEventHandler(
          std::bind(&DirViewPanel::QuickSearchHandler, this, std::placeholders::_1),
          this
        )
      )
    , QuickSearchMode(false)
    , CurrentRow(0)
    , Context(context)
  {
    Ui = new Ui_DirViewPanel();
    Ui->setupUi(this);
    Ui->SearchEdit->hide();

    // TODO: move in settings
    setFont(QFont("Menlo Regular", 11));

    Model->SetRoot(QDir("/"));
    Ui->DirView->setModel(Model);
    connect(Model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(OnDirModelChange()));
    connect(
      Ui->DirView->selectionModel(),
      SIGNAL(currentChanged(const QModelIndex, const QModelIndex&)),
      SLOT(OnSelectionChanged(const QModelIndex&, const QModelIndex&))
    );

    connect(Ui->DirView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), SLOT(OnHeaderGeometryChanged()));
    Ui->DirView->horizontalHeader()->restoreState(Settings::LoadViewHeaderState());

    Ui->DirView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(Ui->DirView, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(OnShowViewContextMenu(const QPoint&)));

    KeyPressFilter* searchKeyDetector = new KeyPressFilter(this);
    connect(searchKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), QuickSearchHandlerDelegate, SLOT(OnKeyPressed(QKeyEvent)));
    Ui->SearchEdit->installEventFilter(searchKeyDetector);

    FocusFilter* focusDetector = new FocusFilter(this);
    connect(focusDetector, SIGNAL(GotFocusEvent(QFocusEvent)), SLOT(OnFocusEvent(QFocusEvent)));
    Ui->DirView->installEventFilter(focusDetector);

    connect(Ui->DirView, SIGNAL(activated(const QModelIndex&)), SLOT(OnItemActivated(const QModelIndex&)));
    connect(Ui->AddressBar, SIGNAL(returnPressed()), SLOT(OnAddressBarEnter()));
    connect(Ui->SearchEdit, SIGNAL(textEdited(const QString&)), SLOT(OnQuickSearch(const QString&)));

    KeyPressFilter* baseKeyDetector = BasePanel::InstallKeyEventFilter();
    connect(baseKeyDetector, SIGNAL(KeyPressed(QKeyEvent)), QuickSearchHandlerDelegate, SLOT(OnKeyPressed(QKeyEvent)));
  }

  QString DirViewPanel::GetName() const
  {
    return this->GetRootDir().isRoot() ? "/" : this->GetRootDir().dirName();
  }

  QFileInfo DirViewPanel::GetCurrentSelection() const
  {
    return CurrentSelection;
  }

  void DirViewPanel::OnShowViewContextMenu(const QPoint& point)
  {
    QMenu menu;
    QAction* revealAction = menu.addAction("Reveal in Finder");
    connect(revealAction, SIGNAL(triggered(bool)), SLOT(OnRevealInFinder()));

    QAction* openTerminalAction = menu.addAction("Open Terminal");
    connect(openTerminalAction, SIGNAL(triggered(bool)), SLOT(OnOpenTerminal()));

    menu.exec(Ui->DirView->viewport()->mapToGlobal(point));
  }

  void DirViewPanel::OnRevealInFinder()
  {
    Shell::RevealInFinder(CurrentSelection.absoluteFilePath());
  }

  void DirViewPanel::OnOpenTerminal()
  {
    Shell::OpenTerminal(Model->GetRoot().absolutePath());
  }

  void DirViewPanel::OnHeaderGeometryChanged()
  {
    qDebug() << "Save view header state";
    const QByteArray& state = Ui->DirView->horizontalHeader()->saveState();
    Settings::SaveViewHeaderState(state);
  }

  void DirViewPanel::OnDirModelChange()
  {
    // adjust selection after model has changed
    QModelIndex currentIndex = Model->GetIndex(CurrentSelection);
    qDebug() << currentIndex;
    if (!currentIndex.isValid())
    {
      currentIndex = Model->index(0, 0);
      if (Model->rowCount() >= CurrentRow)
      {
        // case when item has been deleted
        currentIndex = Model->index(CurrentRow - 1, 0);
      }
    }
    Ui->DirView->selectionModel()->setCurrentIndex(
      currentIndex,
      QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows
    );

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

  void DirViewPanel::SetFocus()
  {
    Ui->DirView->setFocus();
  }

  void DirViewPanel::OnFocusEvent(QFocusEvent event)
  {
    if (event.gotFocus())
    {
      if (QuickSearchMode)
      {
        SwitchQuickSearchMode();
      }
      emit ChangeSideRequest(false);
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
      if (event.key() == Qt::Key_Up || event.key() == Qt::Key_Down || event.key() == Qt::Key_F4)
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

  void DirViewPanel::KeyHandler(Qt::KeyboardModifiers modifiers, Qt::Key key)
  {
    if (modifiers == Qt::NoModifier)
    {
      if (key == Qt::Key_Return)
      {
        HandleItemSelection(CurrentSelection);
      }
      else if (key == Qt::Key_Tab)
      {
        qDebug("Request to change side detected");
        emit ChangeSideRequest(true);
      }
      else if (key == Qt::Key_F4)
      {
        qDebug() << "Request to edit file detected:" << CurrentSelection.absoluteFilePath();
        Shell::OpenEditorForFile(CurrentSelection.absoluteFilePath());
      }
      else if (key == Qt::Key_Delete) // Fn + Backspace
      {
        qDebug() << "Request to delete item, path is" << CurrentSelection.absoluteFilePath();
        Filesys::RemoveDirRecursive(Filesys::Dir(CurrentSelection.absoluteFilePath().toStdWString()));
      }
      else if (key == Qt::Key_F7)
      {
        QString newDirPath = Model->GetRoot().absolutePath() + "/";
        CreateDirDialog* dlg = new CreateDirDialog(this);
        dlg->exec();

        newDirPath += dlg->GetDirName();
        qDebug() << "Request to create directory, path is" << newDirPath;
        Filesys::CreateDir(newDirPath.toStdWString());
      }
      else if (key == Qt::Key_F5)
      {
        if (!Context.IsOppositeTabDirView(this))
        {
          qDebug() << "Opposite tab is not dir view, skip copy request";
          return;
        }
        const QDir& dest = Context.GetOppositeTabRootDir(this);
        qDebug() << "Request to copy file or dir" << CurrentSelection.absoluteFilePath() << "to" << dest.absolutePath();
        Filesys::Copy(
          Filesys::FileInfo(CurrentSelection.absoluteFilePath().toStdWString()),
          Filesys::FileInfo(dest.absolutePath().toStdWString())
        );
      }
    }
    else if (modifiers == Qt::ShiftModifier)
    {
      if (key == Qt::Key_F4)
      {
        QString filePath = Model->GetRoot().absolutePath() + "/";
        EditFileDialog* dlg = new EditFileDialog(this);
        dlg->exec();

        filePath += dlg->GetFileName();
        qDebug() << "Request to edit file by entered name, full path is" << filePath;
        EnsureFileExists(filePath);
        Shell::OpenEditorForFile(filePath);
      }
    }
    else if (modifiers == Qt::ControlModifier)
    {
      if (key == Qt::Key_T)
      {
        qDebug() << "New tab request";
        emit AddNewTabRequest();
      }
      else if (key == Qt::Key_W)
      {
        qDebug() << "Close tab request";
        emit CloseTabRequest();
      }
      else if (key == Qt::Key_F)
      {
        const QString& searchRoot = Model->GetRoot().absolutePath();
        qDebug() << "Find in files request, root dir" << searchRoot;
        FindInFilesDialog dlg(Filesys::Dir(searchRoot.toStdWString()), this);
        dlg.exec();
      }
    }
    else if (modifiers == Qt::MetaModifier)
    {
      if (key == Qt::Key_C)
      {
        qDebug() << "Open terminal request";
        Shell::OpenTerminal(Model->GetRoot().absolutePath());
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

    emit TitleChanged(GetName());
  }
} // namespace TotalFinder
