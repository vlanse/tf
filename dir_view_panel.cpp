/*
 * dir_view_panel.cpp
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#include "dir_view_panel.h"
#include "ui_dir_view_panel.h"

#include "dir_model.h"
#include "event_filters.h"

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
  {
    Ui = new Ui_DirViewPanel();
    Ui->setupUi(this);
    Ui->SearchEdit->hide();

    Model->SetRoot(QDir("/"));
    Ui->DirView->setModel(Model);

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

        const QModelIndex& selection = Ui->DirView->selectionModel()->currentIndex();
        if (!selection.isValid())
        {
          return;
        }
        const QFileInfo& selectedItem = Model->GetItem(selection);
        HandleItemSelection(selectedItem);
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
    if (event.key() == Qt::Key_Return)
    {
      const QModelIndex& selection = Ui->DirView->selectionModel()->currentIndex();
      if (!selection.isValid())
      {
        return;
      }
      const QFileInfo& selectedItem = Model->GetItem(selection);
      HandleItemSelection(selectedItem);
    }
    else if (event.key() == Qt::Key_Tab)
    {
      qDebug("Request to change side detected");
      emit ChangeSideRequest();
    }
    else if (event.key() == Qt::Key_F4)
    {
      const QModelIndex& selection = Ui->DirView->selectionModel()->currentIndex();
      QFileInfo file = Model->GetItem(selection);
      qDebug() << "Request to edit file detected:" << file.absoluteFilePath();
      QStringList args;
      args << "-a" << "Sublime Text 2" << file.absoluteFilePath();
      qDebug() << args;
      QProcess::startDetached("open", args);
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
    const QDir& previousDir = Model->GetRoot();
    Model->SetRoot(dir);
    Ui->AddressBar->setText(Model->GetRoot().absolutePath());

    const QModelIndex& index = Model->GetIndex(previousDir);
    Ui->DirView->setCurrentIndex(index.isValid() ? index : Model->index(0,0));
    Ui->DirView->scrollTo(Ui->DirView->selectionModel()->currentIndex());

    emit DirChanged();
  }
} // namespace TF

