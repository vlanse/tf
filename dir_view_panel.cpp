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

namespace TF
{
  DirViewPanel::DirViewPanel(QWidget* parent)
    : QWidget(parent)
    , Model(new DirModel(this))
  {
    Ui = new Ui_DirViewPanel();
    Ui->setupUi(this);

    Model->SetRoot(QDir("/"));
    Ui->DirView->setModel(Model);

    KeyPressFilter* keyDetector = new KeyPressFilter(this);
    keyDetector->InterceptKey(Qt::Key_Tab);
    connect(keyDetector, SIGNAL(KeyPressed(QKeyEvent)), SLOT(OnKeyPressed(QKeyEvent)));
    Ui->DirView->installEventFilter(keyDetector);

    connect(Ui->DirView, SIGNAL(activated(const QModelIndex&)), SLOT(OnItemActivated(const QModelIndex&)));
    connect(Ui->AddressBar, SIGNAL(returnPressed()), SLOT(OnAddressBarEnter()));
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

  void DirViewPanel::OnItemActivated(const QModelIndex& index)
  {
    HandleItemSelection(Model->GetItem(index).absoluteFilePath());
  }

  void DirViewPanel::OnAddressBarEnter()
  {
    HandleItemSelection(QFileInfo(Ui->AddressBar->text()));
    Ui->DirView->setFocus();
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

