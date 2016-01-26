/*
 * dir_view_panel.h
 *
 *  Created on: Dec 24, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#pragma once

#include <QDir>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QWidget>

class Ui_DirViewPanel;

namespace TF
{
  class DirModel;
  class QuickSearchKeyEventHandler;

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
    void ChangeSideRequest(bool force);
    void AddNewTabRequest();
    void CloseTabRequest();
  private slots:
    void OnHeaderGeometryChanged();
    void OnItemActivated(const QModelIndex& index);
    void OnAddressBarEnter();
    void OnKeyPressed(QKeyEvent event);
    void OnQuickSearch(const QString&);
    void OnFocusEvent(QFocusEvent event);
    void OnDirModelChange();
    void OnSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void OnShowViewContextMenu(const QPoint& point);
    void OnRevealInFinder();
    void OnOpenTerminal();
  private:
    void HandleItemSelection(const QFileInfo& item);
    void HandleDirSelection(const QDir& dir);
    void QuickSearchHandler(QKeyEvent event);
    void SwitchQuickSearchMode();

    void UpdateCurrentSelection();

    Ui_DirViewPanel* Ui;

    DirModel* Model;
    QuickSearchKeyEventHandler* QuickSearchHandlerDelegate;
    bool QuickSearchMode;

    QFileInfo CurrentSelection;
    int CurrentRow;
  };
} // namespace TF
