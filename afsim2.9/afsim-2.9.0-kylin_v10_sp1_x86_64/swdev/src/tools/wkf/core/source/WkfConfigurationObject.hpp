// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFCONFIGURATIONDIALOG_HPP
#define WKFCONFIGURATIONDIALOG_HPP

#include <QObject>

class QAction;
class QActionGroup;
class QMenu;
class QSettings;

namespace wkf
{
class PrefObject;

class ConfigurationObject : public QObject
{
   Q_OBJECT

public:
   ConfigurationObject(QObject* parent = nullptr);
   ~ConfigurationObject() override = default;

   void RegisterPrefObject(PrefObject* aObjectPtr);

signals:
   void LoadSettings(QSettings& aAppSettings, QSettings& aSharedSettings);
   void SaveSettings(QSettings& aAppSettings, QSettings& aSharedSettings);

private:
   void Initialize();
   void LoadActionHandler();
   void RecentActionHandler();
   void SaveActionHandler();
   void ImportActionHandler();
   void ClearPlatformSettingsHandler();

   void NumRecentConfigsChanged(int aSize);

   void LoadRecentsSettings();
   void SaveRecentsSettings();

   void     CheckForConfigDirectory();
   QAction* CreateAction(const QString& aName, const QString& aPath);
   void     LoadConfig(const QString& aName, const QString& aPath);
   void     LoadFilteredConfig(const QString& aPath, const QStringList& aGroupList);
   void     UpdateRecent(const QString& aName, const QString& aPath);
   void     PopulateMenu(QMenu* aMenu);

   QMenu* mLoadRecentMenu;

   // std::list (vs std::map) to maintain order, std::pair<File Name, File Path>
   std::list<std::pair<QString, QString>> mRecentConfigurations;
};
} // namespace wkf
#endif
