/*
 * dir_view_panel.h
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#pragma once

#include <QDir>
#include <QKeyEvent>
#include <QWidget>

class Ui_DirViewPanel;

namespace TF
{
  class DirModel;
  class KeyPressFilter;

  class DirViewPanel: public QWidget
  {
    Q_OBJECT
  public:
    DirViewPanel(QWidget* parent = 0);
    void SetRootDir(const QDir& dir);
    QDir GetRootDir() const;
    void SetFocusOnView();
  signals:
    void DirChanged();
    void ChangeSideRequest();
  private slots:
    void OnItemActivated(const QModelIndex& index);
    void OnAddressBarEnter();
    void OnKeyPressed(QKeyEvent event);
  private:
    void HandleItemSelection(const QFileInfo& item);
    void HandleDirSelection(const QDir& dir);

    Ui_DirViewPanel* Ui;

    DirModel* Model;
  };
} // namespace TF
