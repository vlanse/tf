/*
 * settings.h
 *
 *  Created on: Dec 27, 2015
 *      Author: Vladimir Semenov (vlanse@gmail.com)
 */

#pragma once

#include <QAbstractTableModel>
#include <QByteArray>
#include <QDir>
#include <QJsonDocument>

namespace TF
{
  namespace Settings
  {
    class SettingsModel: public QAbstractTableModel
    {
      Q_OBJECT
    public:
      SettingsModel(QObject* parent);

      virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
      virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
      virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
      virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    };

    class SettingsChangeMonitor: public QObject
    {
      Q_OBJECT
    public:
      static SettingsChangeMonitor& Instance();
      void NotifySettingsChanged();
    signals:
      void SettingsChanged();
    private:
      SettingsChangeMonitor();
    };

    void SaveMainWindowGeometry(const QByteArray& geometry);
    QByteArray LoadMainWindowGeometry();

    void SaveDirFilters(QDir::Filters filters);
    QDir::Filters LoadDirFilters();

    void SaveViewHeaderState(const QByteArray& state);
    QByteArray LoadViewHeaderState();

    void SaveTabs(const QJsonDocument& data);
    QJsonDocument LoadTabs();
  } // namespace Settings
} // namespace TF
