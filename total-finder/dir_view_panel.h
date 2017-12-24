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
    explicit DirViewPanel(const TabContext& context, QWidget* parent = nullptr);
    void SetRootDir(const QDir& dir);
    QDir GetRootDir() const;
    QFileInfo GetCurrentSelection() const;
    void SetFocus() override;
    QString GetName() const override;

  private slots:
    void OnHeaderGeometryChanged();
    void OnItemActivated(const QModelIndex& index);
    void OnAddressBarEnter();
    void OnFocusEvent(QFocusEvent event);
    void OnDirModelChange();
    void OnSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void OnShowViewContextMenu(const QPoint& point);
    void OnRevealInFinder();
    void OnOpenTerminal();

  private:
    void HandleItemSelection(const QFileInfo& item);
    void HandleDirSelection(const QDir& dir);

    void KeyHandler(Qt::KeyboardModifiers modifier, Qt::Key key, const QString& text) override;

    Ui_DirViewPanel* Ui;

    DirModel* Model;

    QFileInfo CurrentSelection;
    int CurrentRow;
    TabContext Context;
  };
} // namespace TotalFinder
