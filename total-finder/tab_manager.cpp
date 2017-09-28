/*
 * tab_manager.cpp
 *
 *  Created on: Dec 25, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */
#include "tab_manager.h"
#include "tab_context.h"

#include "dir_view_panel.h"
#include "settings.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QTabBar>

namespace TF
{
  namespace
  {
    const char ROOT_DIR_NAME[] = "/";

    void AddTab(TabManager* tabs, DirViewPanel* tab, const SideContext& side, int index = -1)
    {
      const QString& tabName = tab->GetRootDir().isRoot() ? ROOT_DIR_NAME : tab->GetRootDir().dirName();
      const int tabIndex = side.Container->insertTab(index, tab, QIcon(), tabName);
      side.Container->setCurrentIndex(tabIndex);

      QObject::connect(tab, SIGNAL(DirChanged()), tabs, SLOT(OnDirChange()));
      QObject::connect(tab, SIGNAL(ChangeSideRequest(bool)), tabs, SLOT(OnChangeSideRequest(bool)));
      QObject::connect(tab, SIGNAL(AddNewTabRequest()), tabs, SLOT(OnAddNewTabRequest()));
      QObject::connect(tab, SIGNAL(CloseTabRequest()), tabs, SLOT(OnCloseTabRequest()));
      QObject::connect(side.Container, SIGNAL(currentChanged(int)), tabs, SLOT(SaveContext()));
    }

    void RestoreTabs(const QJsonValue& tabsObject, TabManager* tabs, SideContext& side)
    {
      QJsonValue tabsData = tabsObject.toObject()["tabs"];
      int activeIndex = tabsObject.toObject()["active_index"].toInt();
      side.Active = tabsObject.toObject()["active"].toBool(false);
      QJsonArray tabsArray;

      bool addDefaultView = false;
      if (!tabsData.isArray())
      {
        addDefaultView = true;
      }
      else
      {
        tabsArray = tabsData.toArray();
        if (tabsArray.empty())
        {
          addDefaultView = true;
        }
      }

      if (addDefaultView)
      {
        QJsonObject obj;
        obj["root"] = ROOT_DIR_NAME;
        tabsArray.append(obj);
        activeIndex = 0;
      }

      foreach(const QJsonValue val, tabsArray)
      {
        if (!val.isObject())
        {
          continue;
        }
        const QJsonObject tab = val.toObject();

        DirViewPanel* tabPanel = new DirViewPanel(TabContext(tabs));
        tabPanel->SetRootDir(QDir(tab["root"].toString(ROOT_DIR_NAME)));
        AddTab(tabs, tabPanel, side);
        tabPanel->SetFocusOnView();
      }

      qDebug() << "set current index from settings" << activeIndex;
      side.Container->setCurrentIndex(activeIndex);
    }

    QJsonObject SaveTabs(const SideContext& side)
    {
      QJsonObject result;
      QJsonArray tabs;
      for (int i = 0; i < side.Container->count(); ++i)
      {
        DirViewPanel* tab = qobject_cast<DirViewPanel*>(side.Container->widget(i));
        if (tab)
        {
          QJsonObject obj;
          obj["root"] = tab->GetRootDir().path();
          tabs.append(obj);
        }
      }
      result["tabs"] = tabs;
      result["active_index"] = side.Container->currentIndex();
      result["active"] = side.Active;
      return result;
    }
  } // namespace

  class MyTabBar: public QTabBar
  {
    Q_OBJECT
  public:
    MyTabBar(QWidget* parent);

  protected:
    QSize tabSizeHint(int index) const;
  };

#include "tab_manager.moc"

  MyTabBar::MyTabBar(QWidget *parent)
    : QTabBar(parent)
  {
  }

  QSize MyTabBar::tabSizeHint(int index) const
  {
    return {qBound(100, width() / count(), width()), 25};
  }

  MyTabWidget::MyTabWidget(QWidget *parent)
    : QTabWidget(parent)
  {
    setTabBar(new MyTabBar(this));
  }

  void MyTabWidget::resizeEvent(QResizeEvent *event)
  {
    tabBar()->setMinimumWidth(event->size().width());
  }

  TabManager::TabManager(
    QTabWidget* leftContainer,
    QTabWidget* rightContainer,
    QObject* parent
  )
    : QObject(parent)
  {
    LeftSide.Active = true;
    LeftSide.Container = leftContainer;
    RightSide.Container = rightContainer;

    RestoreContext();
  }

  DirViewPanel* TabManager::GetOppositeTab(DirViewPanel* current) const
  {
    const SideContext* side = FindSideForTab(current);
    if (side == nullptr)
    {
      return nullptr;
    }
    const SideContext* oppositeSide = (side == &LeftSide ? &RightSide : &LeftSide);
    return qobject_cast<DirViewPanel*>(oppositeSide->Container->currentWidget());
  }

  void TabManager::RestoreContext()
  {
    const QJsonDocument data = Settings::LoadTabs();
    RestoreTabs(data.object().value("left"), this, LeftSide);
    RestoreTabs(data.object().value("right"), this, RightSide);
    if (!LeftSide.Active && !RightSide.Active)
    {
      LeftSide.Active = true;
    }
    SetFocusOnView();
  }

  void TabManager::SaveContext()
  {
    QJsonObject obj;
    obj["left"] = SaveTabs(LeftSide);
    obj["right"] = SaveTabs(RightSide);
    Settings::SaveTabs(QJsonDocument(obj));
  }

  void TabManager::OnChangeSideRequest(bool force)
  {
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(sender());
    if (!force && GetActiveSide() == FindSideForTab(tab))
    {
      return;
    }

    LeftSide.Active = !LeftSide.Active;
    RightSide.Active = !RightSide.Active;

    SaveContext();
    SetFocusOnView();
  }

  void TabManager::OnAddNewTabRequest()
  {
    const SideContext* side = GetActiveSide();
    DirViewPanel* tab = new DirViewPanel(TabContext(this));

    const DirViewPanel* currentTab = qobject_cast<const DirViewPanel*>(side->Container->currentWidget());

    tab->SetRootDir(currentTab->GetRootDir());
    AddTab(this, tab, *side, side->Container->currentIndex() + 1);
    tab->SetFocusOnView();

    SaveContext();
  }

  void TabManager::OnCloseTabRequest()
  {
    const SideContext* side = GetActiveSide();
    if (!side || side->Container->count() == 1)
    {
      return;
    }

    const int indexToRemove = side->Container->currentIndex();
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->widget(indexToRemove));
    side->Container->removeTab(indexToRemove);
    tab->deleteLater();

    const int currentIndex = side->Container->currentIndex();
    if (currentIndex != -1)
    {
      DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->widget(currentIndex));
      tab->SetFocusOnView();
    }

    SaveContext();
  }

  const SideContext* TabManager::GetActiveSide() const
  {
    if (LeftSide.Active)
    {
      return &LeftSide;
    }
    if (RightSide.Active)
    {
      return &RightSide;
    }
    return 0;
  }

  const SideContext* TabManager::FindSideForTab(DirViewPanel* tab) const
  {
    if (LeftSide.Container->indexOf(tab) != -1)
    {
      return &LeftSide;
    }
    if (RightSide.Container->indexOf(tab) != -1)
    {
      return &RightSide;
    }
    return nullptr;
  }

  void TabManager::SetFocusOnView()
  {
    const SideContext* side = GetActiveSide();
    if (!side)
    {
      qWarning("Where is no active side");
      return;
    }

    DirViewPanel* tab = qobject_cast<DirViewPanel*>(side->Container->currentWidget());
    tab->SetFocusOnView();
  }

  void TabManager::OnDirChange()
  {
    DirViewPanel* tab = qobject_cast<DirViewPanel*>(sender());
    if (!tab)
    {
      return;
    }
    const SideContext* side = FindSideForTab(tab);
    const int tabIndex = side->Container->indexOf(tab);

    const QDir& tabDir = tab->GetRootDir();
    side->Container->setTabText(tabIndex, tabDir.isRoot() ? ROOT_DIR_NAME : tabDir.dirName());
    side->Container->setTabToolTip(tabIndex, tabDir.path());

    SaveContext();
  }
} // namespace TF
