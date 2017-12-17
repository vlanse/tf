#include "settings.h"

#include <QDebug>
#include <QSettings>

namespace TotalFinder
{
  namespace Settings
  {
    namespace
    {
      const char COMPANY_NAME[] = "TotalFinder Inc";
      const char PRODUCT_NAME[] = "TotalFinder";

      void SaveValue(const char* key, const QVariant& value, bool notifyChange = true)
      {
        QSettings settings(COMPANY_NAME, PRODUCT_NAME);
        settings.setValue(key, value);
        if (notifyChange)
        {
          SettingsChangeMonitor::Instance().NotifySettingsChanged();
        }
      }

      QVariant LoadValue(const char* key, const QVariant& defaultVal = QVariant())
      {
        QSettings settings(COMPANY_NAME, PRODUCT_NAME);
        return settings.value(key, defaultVal);
      }

      const char KEY_MAIN_WINDOW_GEOMETRY[] = "main_window_geometry";

      const char KEY_DIR_FILTERS[] = "dir_filters";
      const QDir::Filters DEFAULT_DIR_FILTERS = QDir::AllEntries | QDir::System | QDir::Hidden | QDir::NoDot;

      const char VIEW_HEADER_STATE[] = "view_header_state";

      const char TABS_STATE[] = "tabs_state";
    } // namespace

    void SaveMainWindowGeometry(const QByteArray& geometry)
    {
      SaveValue(KEY_MAIN_WINDOW_GEOMETRY, geometry);
    }

    QByteArray LoadMainWindowGeometry()
    {
      return LoadValue(KEY_MAIN_WINDOW_GEOMETRY).toByteArray();
    }

    void SaveDirFilters(QDir::Filters filters)
    {
      SaveValue(KEY_DIR_FILTERS, static_cast<int>(filters));
    }

    QDir::Filters LoadDirFilters()
    {
      return static_cast<QDir::Filters>(
        LoadValue(
          KEY_DIR_FILTERS,
          static_cast<int>(DEFAULT_DIR_FILTERS)).toInt()
      );
    }

    void SaveViewHeaderState(const QByteArray& state)
    {
      SaveValue(VIEW_HEADER_STATE, state, false);
    }

    QByteArray LoadViewHeaderState()
    {
      return LoadValue(VIEW_HEADER_STATE).toByteArray();
    }

    void SaveTabs(const QJsonDocument& data)
    {
      SaveValue(TABS_STATE, data.toBinaryData(), false);
    }

    QJsonDocument LoadTabs()
    {
      return QJsonDocument::fromBinaryData(LoadValue(TABS_STATE).toByteArray());
    }

    SettingsModel::SettingsModel(QObject* parent)
      : QAbstractTableModel(parent)
    {
    }

    int SettingsModel::rowCount(const QModelIndex& parent) const
    {
      QSettings settings(COMPANY_NAME, PRODUCT_NAME);
      return settings.allKeys().size();
    }

    int SettingsModel::columnCount(const QModelIndex& parent) const
    {
      return 2;
    }

    QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
      if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
      {
        switch (section)
        {
        case 0:
          return "Key";
        case 1:
          return "Value";
        }
      }
      return QVariant();
    }

    QVariant SettingsModel::data(const QModelIndex& index, int role) const
    {
      if (!index.isValid())
      {
        return QVariant();
      }
      if (role == Qt::DisplayRole)
      {
        QSettings settings(COMPANY_NAME, PRODUCT_NAME);
        const QString key = settings.allKeys()[index.row()];
        switch(index.column())
        {
        case 0:
          return key;
        case 1:
          return settings.value(key);
        }
      }
      return QVariant();
    }

    SettingsChangeMonitor::SettingsChangeMonitor()
      : QObject()
    {
    }

    SettingsChangeMonitor& SettingsChangeMonitor::Instance()
    {
      static SettingsChangeMonitor monitor;
      return monitor;
    }

    void SettingsChangeMonitor::NotifySettingsChanged()
    {
      qDebug() << "Sending settings change notification";
      emit SettingsChanged();
    }
  } // namespace Settings
} // namespace TotalFinder
