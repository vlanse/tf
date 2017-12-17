#pragma once

#include "base_panel.h"
#include "tab_context.h"

#include <QDir>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QWidget>

class Ui_DirViewPanel;

namespace TotalFinder
{
  class DirModel;
  class QuickSearchKeyEventHandler;
  class TabContext;

  class DirViewPanel: public BasePanel
  {
    Q_OBJECT
  public:
    DirViewPanel(const TabContext& context, QWidget* parent = 0);
    void SetRootDir(const QDir& dir);
    QDir GetRootDir() const;
    QFileInfo GetCurrentSelection() const;
    virtual void SetFocus();
    virtual QString GetName() const;
  signals:
    void TitleChanged(const QString& newTitle);
    void ChangeSideRequest(bool force);
    void AddNewTabRequest();
    void CloseTabRequest();
  private slots:
    void OnHeaderGeometryChanged();
    void OnItemActivated(const QModelIndex& index);
    void OnAddressBarEnter();
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

    virtual void KeyHandler(Qt::KeyboardModifiers modifier, Qt::Key key);

    Ui_DirViewPanel* Ui;

    DirModel* Model;
    QuickSearchKeyEventHandler* QuickSearchHandlerDelegate;
    bool QuickSearchMode;

    QFileInfo CurrentSelection;
    int CurrentRow;
    TabContext Context;
  };
} // namespace TotalFinder
