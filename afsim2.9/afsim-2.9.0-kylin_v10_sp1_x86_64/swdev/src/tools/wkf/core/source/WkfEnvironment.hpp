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

#ifndef WKFENVIRONMENT_HPP
#define WKFENVIRONMENT_HPP

#include <memory>

class QMenu;
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

namespace vespa
{
class VaEntity;
}

#include "wkf_export.h"

#include "VaCallbackHolder.hpp"
#include "WkfEntity.hpp"
#include "WkfMilStd2525d.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfPrefObject.hpp"
#include "WkfResourceManager.hpp"
#include "WkfTipContainer.hpp"
#include "WkfUpdater.hpp"

#define wkfEnv wkf::Environment::GetInstance()

namespace wkf
{
class Action;
class Factory;
class MainWindow;
class PluginManager;
class VtkEnvironment;

namespace permission
{
class Manager;
}

class WKF_EXPORT Environment : public QObject
{
   Q_OBJECT

public:
   static Environment& GetInstance();

   static void Create(const QString& aApplicationName,
                      const QString& aApplicationPrefix,
                      const QString& aSettings        = "",
                      bool           aImportSettings  = false,
                      const QString& aPermissionsFile = "");

   static bool Exists();

   Environment(const Environment& aSrc) = delete;

   void StartUp();
   void Shutdown();

   MainWindow*    GetMainWindow() const { return mMainWindowPtr; }
   PluginManager* GetPluginManager() const { return mPluginManagerPtr; }

   wkf::permission::Manager& GetPermissions() const { return *mPermissions; }
   QSettings&                GetPermissionFile() const { return *mPermissionSettings; }

   const QString& GetApplicationName() const { return mApplication; }
   const QString& GetApplicationPrefix() const { return mApplicationPrefix; }

   const QString& GetAppDataDir() const { return mAppDataDir; }
   QString        GetConfigurationsDir() const { return mAppDataDir + "//configs"; }
   QString        GetDemosDir() const { return QString::fromStdString(mSourceRoot) + "/../demos"; }
   QString        GetDocumentationDir() const;
   const QString& GetResourcesDir() const { return mResourcesDir; }
   QSettings&     GetUserSettingsFile() const { return *mUserSettings; }
   QSettings&     GetSharedSettingsFile() const { return *mSharedUserSettings; }
   QSettings&     GetAppDataSettingsFile() const { return *mAppDataSettings; }
   void           BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntity);
   void           BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr);

   void                                             ClearPlatformGroupings(const int aIndex);
   void                                             SetPlatformGrouping(int aIndex, const PlatformGrouping aGrouping);
   const std::map<PlatformGrouping, std::set<int>>& GetGroupings() const { return mGroupingToPlatformMap; }
   //! @note This will implicitly create the entry if it does not exist
   const std::set<PlatformGrouping>& GetGroupings(int aIndex) { return mPlatformToGroupingMap[aIndex]; }

   void RegisterPrefObject(PrefObject* aObjectPtr);
   void UnregisterPrefObject(PrefObject* aObjectPtr);

   using VisibilityFilter = std::function<bool(const wkf::Platform&)>;
   size_t RegisterPlatformVisibilityFilter(const VisibilityFilter& aFilter);
   void   UnregisterPlatformVisibilityFilter(size_t aId);
   bool   IsPlatformVisible(const wkf::Platform* aPlatform);

   using PrefObjectMap = std::map<QString, PrefObject*>;
   const PrefObjectMap& GetPrefObjectMap() const { return mPrefObjectMap; }

   template<class PREF_OBJECT>
   PREF_OBJECT* GetPreferenceObject() const;

   //! Register an updater with the Environment, providing a "context" object.
   //! @note The updater will automatically be unregistered when the context object is destroyed.
   template<class UPDATER>
   void RegisterUpdater(const QObject& context)
   {
      static_assert(std::is_base_of<Updater, UPDATER>::value,
                    "Template argument to RegisterUpdater must be subclass of wkf::Updater");

      mUpdaterFactory.insert(UPDATER::cUPDATER_NAME,
                             [](const std::string& aPlatformName)
                             { return std::unique_ptr<Updater>(new UPDATER(aPlatformName)); });
      connect(&context, &QObject::destroyed, [this]() { mUpdaterFactory.remove(UPDATER::cUPDATER_NAME); });
   }

   QStringList              GetUpdaterList() const;
   std::unique_ptr<Updater> GetUpdater(const QString& aUpdaterId, const std::string& aPlatformName) const;

   //! Enable/Disable the emit FrameUpdate signal.
   //! @note The FrameUpdate signal will be emitted once after calling this function with an argument of false prior to
   //! being disabled.
   void SetFrameUpdateEnabled(bool aEnabled);

   //! @return the Platform Of Interest which is the most recent platform that was selected.
   //! @note This is to support access for a single platform to shown in a display when multiple
   //! platforms may be selected. So that all displays interested only in a single platform,
   //! all select the same platform to display.
   Platform* GetPlatformOfInterest() const;
   // Sets a group of platforms to be selected, is a platform is not in the list it will be deselected.
   // Passing an empty list to this functions clears the selection.
   void SetPlatformsSelected(const PlatformList& aPlatforms);
   //! @return a list of the selected platforms
   const PlatformList& GetSelectedPlatforms() const { return mSelectedPlatforms; }

   //! @return whether the platform is selected or not
   bool IsPlatformSelected(const std::string& aPlatformName) const;
   bool IsPlatformSelected(const Platform* aPlatform) const;

   //! @note this will look a provider for the requested resource associated with the platform
   bool RequestPlatformResourceInstance(const QString& aResourceName, const QString& aPlatformName);

   void LoadSettings(QSettings& aSettings);

   void LoadModelDatabase();

   //! @name Permission related functions
   //@{
   void             SetFileBrowsingLocked(bool aLocked) { mLockFileLoad = aLocked; }
   bool             IsFileBrowsingLocked() const { return mLockFileLoad; }
   ResourceManager& GetResourceManager() const { return *mResourceManager; }
   //@}

   QStringList GetApplicationTips() const;

   WkfMilStd2525d& GetMilStdIconLoader() const { return *mIconLoader; }

signals:
   void Initialize();
   //! @note Connect to this to get a regular GUI update command.
   void UpdateFrame();

   void PlatformOfInterestChanged(Platform* aPlatform);
   void PlatformSelectionChanged(const PlatformList& aSelectedPlatforms, const PlatformList& aUnselectedPlatforms);

   void MoveSelectionComplete(const PlatformList& aSelectedPlatforms);

   //! @brief signal to notify connected objects that there has been a change to a selection of platforms and associated
   //! attachments
   //! @param aSelectedPlatforms is the list of selected platforms
   //! @param aSelectedNonPlatforms is the list of selected entities that are not classified as platforms (such as
   //! points of interest)
   //! @param aAttachmentList is the list of selected attachments
   void MoveSelectionAndAttachmentComplete(const PlatformList&            aSelectedPlatforms,
                                           const EntityList&              aSelectedNonPlatforms,
                                           const vespa::VaAttachmentList& aAttachmentList);

   void ClearPlatformSettings();
   void CloseDialogs();
   void UpdatePlatformOptions();
   void DeleteShortcutTriggered();

   //! @note These signals should be able to pass a wkf::PlatformGrouping, however it does not always work.
   //! @see comment above the Q_DECLARE_METATYPE macro in WkfPlatformGrouping.hpp
   void NewGrouping(int aType, const QString& aName, int aPlatformIndex);
   void GroupingChanged(int aType, const QString& aName, int aPlatformIndex);

   void PlatformVisibilityChanged();

protected:
   Environment(const QString& aApplicationName,
               const QString& aApplicationPrefix,
               const QString& aSettingsFile,
               bool           aImportSettings,
               const QString& aPermissionFile);

   ~Environment() override;

private:
   //! Sets the platform selected if aFocus == true, or deselects if aFocus == false
   void SetPlatformSelected(Platform* aPlatform, bool aFocus);
   void PlatformVisibilityChangedHandler();

   void TimerHandler();

   bool SetupVespaEnvironment();
   void CleanupVespaEnvironment();
   bool LoadVespaResources();

   void EntityRemovedCB(vespa::VaEntity* aEntityPtr);

   static Environment* mInstancePtr;
   bool                mStarted;

   vespa::VaCallbackHolder mCallbacks;
   PlatformList            mSelectedPlatforms;
   EntityList              mBullseyeList;

   QSettings*     mAppDataSettings;
   QSettings*     mSharedUserSettings;
   QSettings*     mUserSettings;
   QSettings*     mInitialSharedSettings;
   QSettings*     mInitialUserSettings;
   QSettings*     mPermissionSettings;
   bool           mFrameUpdatedEnabled;
   MainWindow*    mMainWindowPtr;    // The main window for the Wk framework
   PluginManager* mPluginManagerPtr; // The manager responsible for maintaining a list of connected plug-ins
   TipContainer*  mApplicationTips;
   QString        mApplication;
   QString        mApplicationPrefix;
   QString        mSharedDataDir; // Path to the AFSIM shared data directory
   QString        mAppDataDir;    // Path to the OS's Application Data directory
   QString        mResourcesDir;  // Path to the Resources directory
   std::string    mSourceRoot;    // Path to the Source Directory used to build WKF
   PrefObjectMap  mPrefObjectMap;
   Action*        mDeleteEntityAction;

   // Updater name -> Creation function
   using UpdaterFactory = QMap<QString, std::function<std::unique_ptr<Updater>(const std::string& aPlatformName)>>;
   UpdaterFactory mUpdaterFactory;

   std::map<int, std::set<PlatformGrouping>> mPlatformToGroupingMap;
   std::map<PlatformGrouping, std::set<int>> mGroupingToPlatformMap;
   std::unique_ptr<ResourceManager>          mResourceManager;
   std::unique_ptr<WkfMilStd2525d>           mIconLoader;
   std::unique_ptr<wkf::permission::Manager> mPermissions;

   size_t                             mNextRegistrationId{1};
   std::map<size_t, VisibilityFilter> mPlatformVisibilityFilters;

   // Permission variables
   bool mLockFileLoad;
};

template<class PREF_OBJECT>
PREF_OBJECT* Environment::GetPreferenceObject() const
{
   static_assert(std::is_base_of<PrefObject, PREF_OBJECT>::value, "Template argument must derive from wkf::PrefObject");

   auto it = mPrefObjectMap.find(PREF_OBJECT::cNAME);
   if (it != mPrefObjectMap.end())
   {
      return dynamic_cast<PREF_OBJECT*>(it->second);
   }
   return nullptr;
}

WKF_EXPORT QString getOpenFileName(QWidget*             aParent         = Q_NULLPTR,
                                   const QString&       aCaption        = QString(),
                                   const QString&       aDir            = QString(),
                                   const QString&       aFilter         = QString(),
                                   QString*             aSelectedFilter = Q_NULLPTR,
                                   QFileDialog::Options aOptions        = QFileDialog::Options());

WKF_EXPORT QStringList getOpenFileNames(QWidget*             aParent         = Q_NULLPTR,
                                        const QString&       aCaption        = QString(),
                                        const QString&       aDir            = QString(),
                                        const QString&       aFilter         = QString(),
                                        QString*             aSelectedFilter = Q_NULLPTR,
                                        QFileDialog::Options aOptions        = QFileDialog::Options());

WKF_EXPORT QString getSaveFileName(QWidget*             aParent         = Q_NULLPTR,
                                   const QString&       aCaption        = QString(),
                                   const QString&       aDir            = QString(),
                                   const QString&       aFilter         = QString(),
                                   QString*             aSelectedFilter = Q_NULLPTR,
                                   QFileDialog::Options aOptions        = QFileDialog::Options());

WKF_EXPORT QString getExistingDirectory(QWidget*             aParent  = Q_NULLPTR,
                                        const QString&       aCaption = QString(),
                                        const QString&       aDir     = QString(),
                                        QFileDialog::Options aOptions = QFileDialog::ShowDirsOnly);
} // namespace wkf
#endif // WKFENVIRONMENT_HPP
