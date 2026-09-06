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

#include "WkfEnvironment.hpp"

#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>

#include "UtLog.hpp"
#include "UtLogPublisher.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtPathParser.hpp"
#include "UtRunEnvManager.hpp"
#include "UtoShaders.hpp"
#include "VaModelDatabase.hpp"
#include "VaResourceManager.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "WkfConfigurationObject.hpp"
#include "WkfFactory.hpp"
#include "WkfGeneralPrefObject.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPermissionDialog.hpp"
#include "WkfPermissionManager.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfProxyStyle.hpp"
#include "WkfScenario.hpp"
#include "WkfUpdater.hpp"
#include "WkfVtkEnvironment.hpp"

#ifdef WKFCODETIMERS_AVAILABLE
#include "WkfCodeTimer.hpp"
#endif


// This is here to support Scene dumps, which is available only in debug
#include <cassert>

#include "VaOSG_ScenegraphBrowserQt.hpp"

namespace
{
QTimer* timerPtr = nullptr;
}

wkf::Environment* wkf::Environment::mInstancePtr = nullptr;

wkf::Environment::Environment(const QString& aApplicationName,
                              const QString& aApplicationPrefix,
                              const QString& aSettingsFile,
                              bool           aImportSettings,
                              const QString& aPermissionFile)
   : mStarted(false)
   , mUserSettings(nullptr)
   , mInitialUserSettings(nullptr)
   , mFrameUpdatedEnabled(false)
   , mMainWindowPtr(nullptr)
   , mPluginManagerPtr(new PluginManager(aApplicationName))
   , mApplication(aApplicationName)
   , mApplicationPrefix(aApplicationPrefix)
   , mResourcesDir("")
   , mSourceRoot("")
   , mPermissions(ut::make_unique<wkf::permission::Manager>())
   , mLockFileLoad(false)
{
   // Maps hardware exceptions to ut::HardwareException.
   // Does nothing if PROMOTE_HARDWARE_EXCEPTIONS CMake flag is not set.
   ut::SetupThreadErrorHandling();

   // Check if an instance already exists
   if (mInstancePtr != nullptr)
   {
      ut::log::fatal() << "Only one instance of WkfEnvironment may exist at any time.";
      exit(1);
   }

   // By setting the instance pointer in the constructor we allow
   // users to derive from WkfEnvironment, which is a singleton.
   mInstancePtr = this;

   // Utilize the Fusion application style and display Light theme icons by default
   qApp->setStyle(new ProxyStyle(QStyleFactory::create("Fusion")));
   QIcon::setThemeName("Light");

   mSharedDataDir   = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "//..//WKF_Shared";
   mAppDataDir      = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
   mAppDataSettings = new QSettings(mAppDataDir + "//AppData.ini", QSettings::IniFormat, this);

   // Look up resources directory to determine if this is a development build running in Visual Studio or an installed build
   UtPath path = UtRunEnvManager::GetRunPath();
   path.Up();
   std::string pathStr      = path.GetSystemPath() + "/resources/";
   UtPath      resourcePath = pathStr;
   // This path should exist in all installed loads
   if (resourcePath.Exists())
   {
      mResourcesDir = QString::fromStdString(pathStr);
   }
   else
   {
      { // RAII block
         auto out = ut::log::info() << "Directory not found, assuming development build paths.";
         out.AddNote() << "Directory: " << pathStr;
      }

      // Use the developer path to the resources directory
      const char* sourceRoot   = ::getenv("SOURCE_ROOT");
      const char* resourcePath = ::getenv("RESOURCE_PATH");
      if (sourceRoot && resourcePath)
      {
         mSourceRoot   = sourceRoot;
         mResourcesDir = resourcePath;
      }
      else
      {
         ut::log::warning() << "Resource not found.";
      }
   }

   mSharedUserSettings    = new QSettings(mSharedDataDir + "//Settings.ini", QSettings::IniFormat, this);
   mUserSettings          = new QSettings(mAppDataDir + "//Settings.ini", QSettings::IniFormat, this);
   mInitialSharedSettings = mSharedUserSettings;
   mInitialUserSettings   = mUserSettings;
   if (!aSettingsFile.isEmpty())
   {
      if (QFile(aSettingsFile).exists())
      {
         mInitialUserSettings   = new QSettings(aSettingsFile, QSettings::IniFormat, this);
         mInitialSharedSettings = mInitialUserSettings;
         if (!aImportSettings)
         {
            mSharedUserSettings = mInitialSharedSettings;
            mUserSettings       = mInitialUserSettings;
         }
      }
      else
      {
         auto out = ut::log::error() << "Could not load configuration file.";
         out.AddNote() << "File: " << aSettingsFile.toStdString();
      }
   }

   if (aPermissionFile.isEmpty())
   {
      mPermissionSettings = new QSettings(mAppDataDir + "//Permissions.ini", QSettings::IniFormat, this);
   }
   else
   {
      if (QFile(aPermissionFile).exists())
      {
         mPermissionSettings = new QSettings(aPermissionFile, QSettings::IniFormat, this);
      }
      else
      {
         auto out = ut::log::error() << "Could not load permission file.";
         out.AddNote() << "File: " << aPermissionFile.toStdString();
      }
   }

   mCallbacks.Add(Observer::EntityRemoved.Connect(&Environment::EntityRemovedCB, this));
   connect(this, &Environment::PlatformVisibilityChanged, this, &Environment::PlatformVisibilityChangedHandler);

   // This is here to support Scene dumps, which is available only in debug, via right click on map window
   vespa::VaOSG_ScenegraphBrowserQt::InitializeSubscriptions();

   mMainWindowPtr = new MainWindow;
   mMainWindowPtr->InitializeDialogs();

   mResourceManager = ut::make_unique<ResourceManager>();

   QString modelDirFile = mResourcesDir + "/models/milStdIconMappings.csv";
   if (!mSourceRoot.empty())
   {
      modelDirFile = QString::fromStdString(mSourceRoot) + "/tools/vespatk/models/milStdIconMappings.csv";
   }
   QString siteDirFile = mResourcesDir + "/site/milStdIconMappings.csv";
   mIconLoader         = ut::make_unique<WkfMilStd2525d>(QStringList{modelDirFile, siteDirFile});

   // Manually set up the Theme Object so it can be set the palette to be used immediately after WkfEnvironment
   // creation. This allows dialog to be display prior to Wk Environment initializing, such as a startup dialog.
   GetPreferenceObject<GeneralPrefObject>()->LoadSettings(*mInitialUserSettings, *mInitialSharedSettings);

   ut::log::Publisher::SetConsoleEnabled(false);
}

wkf::Environment::~Environment()
{
   ut::log::Publisher::SetConsoleEnabled(true);
   // Visibility Filters registered from plugin will not be cleaned properly if the plugin
   //  is deleted prior to the std::function within the Visibility Filter is cleaned up.
   //  Therefore clear the filters prior to deleting the Plugin Manager (which deletes the plugins)
   mPlatformVisibilityFilters.clear();

   // Since permissions may include types defined in plugin DLLs, they need to be deleted before
   //  the DLL that defines the destructor is closed.
   mPermissions.reset();

   // Deleting the MainWindow will delete the children of the MainWindow also.  These children may
   //  refer to Wkf::Environment and try to get the MainWindow pointer, which is bad.  Therefore,
   //  set the MainWindow pointer to nullptr, then delete the memory, which allows the check to be
   //  performed against nullptr prior to using the MainWindow pointer return by the Environment.
   auto tempPtr   = mMainWindowPtr;
   mMainWindowPtr = nullptr;
   delete tempPtr;

   delete mPluginManagerPtr;

   mInstancePtr = nullptr;
}

void wkf::Environment::TimerHandler()
{
   vaEnv.GetTime()->UpdateFrame();
   if (mFrameUpdatedEnabled)
   {
      emit UpdateFrame();
   }
}

wkf::Environment& wkf::Environment::GetInstance()
{
   assert(mInstancePtr != nullptr);
   return *mInstancePtr;
}

void wkf::Environment::Create(const QString& aApplicationName,
                              const QString& aApplicationPrefix,
                              const QString& aSettings,
                              bool           aImportSettings,
                              const QString& aPermissionFile)
{
   assert(mInstancePtr == nullptr);
   if (mInstancePtr == nullptr)
   {
      new Environment(aApplicationName, aApplicationPrefix, aSettings, aImportSettings, aPermissionFile);
   }
}

bool wkf::Environment::Exists()
{
   return (mInstancePtr != nullptr);
}

void wkf::Environment::StartUp()
{
   if (!mStarted)
   {
      mStarted = true;

      mApplicationTips = new TipContainer(this);

      // Initialize Main Window after data classes because child dialogs may depend on data classes.
      if (SetupVespaEnvironment())
      {
         for (auto&& it : mPrefObjectMap)
         {
            mMainWindowPtr->GetConfigurationObject()->RegisterPrefObject(it.second);
         }

#ifdef WKFCODETIMERS_AVAILABLE
         auto ct = new CodeTimerObject(mMainWindowPtr); // qobject, mainwindow will clean it up
#endif

         // Settings for plugin manager will determine which plugins get auto-started in call to Initialize()
         mPluginManagerPtr->LoadSettings(*mInitialUserSettings);
         // Load plugins after any dependencies they may have like the Main Window
         mPluginManagerPtr->Initialize();

         emit Initialize();

         mMainWindowPtr->GetConfigurationObject()->LoadSettings(*mInitialUserSettings, *mInitialSharedSettings);
         Observer::LoadSettings(*mInitialUserSettings);

         GetPermissions().ReadFile(GetPermissionFile());
         mMainWindowPtr->GetPermissionDialog()->SetActionVisible(!GetPermissions().IsEmpty());

         mMainWindowPtr->show();

         // Create 30 Hz timer for frame updates
         timerPtr = new QTimer(this);
         connect(timerPtr, &QTimer::timeout, this, &Environment::TimerHandler);
         timerPtr->start(33);
      }
   }
}

void wkf::Environment::LoadSettings(QSettings& aSettings)
{
   Observer::LoadSettings(aSettings);
   emit UpdatePlatformOptions();
}

void wkf::Environment::LoadModelDatabase()
{
   std::string resourcePath = UtRunEnvManager::GetCmeRelBase() + "/models/";
   if (!mSourceRoot.empty())
   {
      std::string path = mSourceRoot + "/tools/vespatk/models/models.txt";
      if (UtPathParser::FileExists(path))
      {
         VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(mSourceRoot + "/tools/vespatk/models/models.txt",
                                                                     true,
                                                                     resourcePath);
      }
   }
   else
   {
      std::string path = resourcePath + "models.txt";
      if (UtPathParser::FileExists(path))
      {
         VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(UtRunEnvManager::GetCmeRelBase() +
                                                                        "/models/models.txt",
                                                                     true);
      }
   }
   std::string siteList = UtRunEnvManager::GetCmeRelBase() + "/site/models.txt";
   if (UtPathParser::FileExists(siteList))
   {
      VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(siteList, false);
   }
   vaEnv.GetModelDatabase()->ReloadModels();
}

QStringList wkf::Environment::GetApplicationTips() const
{
   if (mApplicationTips)
   {
      return mApplicationTips->GetTips();
   }
   return QStringList();
}

void wkf::Environment::Shutdown()
{
   if (mInstancePtr)
   {
      if (timerPtr)
      {
         timerPtr->stop();
      }
      CleanupVespaEnvironment();
      delete mInstancePtr;
   }
}

bool wkf::Environment::SetupVespaEnvironment()
{
   if (LoadVespaResources())
   {
      // Initialize the VTK environment.
      if (!vaEnv.Initialize())
      {
         ut::log::error() << "VespaTk environment has failed to initialize, shutting down...";
         return false;
      }

      return true;
   }
   return false;
}

void wkf::Environment::CleanupVespaEnvironment()
{
   VtkEnvironment::Shutdown();
}

bool wkf::Environment::LoadVespaResources()
{
   bool loadedResources = true;

   UtRunEnvManager::Setup(nullptr, mResourcesDir.toStdString().c_str());

   std::string resourcePath = UtRunEnvManager::GetCmeRelBase() + "/models/";
   if (!mSourceRoot.empty())
   {
      std::string path = mSourceRoot + "/tools/vespatk/models/models.txt";
      if (UtPathParser::FileExists(path))
      {
         VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(mSourceRoot + "/tools/vespatk/models/models.txt",
                                                                     true,
                                                                     resourcePath);
      }
   }
   else
   {
      std::string path = resourcePath + "models.txt";
      if (UtPathParser::FileExists(path))
      {
         VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(UtRunEnvManager::GetCmeRelBase() +
                                                                        "/models/models.txt",
                                                                     true);
      }
   }
   std::string siteList = UtRunEnvManager::GetCmeRelBase() + "/site/models.txt";
   if (UtPathParser::FileExists(siteList))
   {
      VtkEnvironment::Instance().GetModelDatabase()->LoadModelSet(siteList, false);
   }

   if (!mSourceRoot.empty())
   {
      UtoShaders::AddShaderDirectory(mSourceRoot + "/tools/vespatk/shaders");
   }

   std::string shadersDir = UtRunEnvManager::GetCmeFolder("shaders");
   UtoShaders::AddShaderDirectory(shadersDir);

   std::string resources = UtRunEnvManager::GetCmeFolder("data") + "/resources.txt";

   if (!vespa::VaResourceManager::Instance().LoadResources(resources))
   {
      std::string path = mSourceRoot;
      path += "/tools/vespatk/data";
      vespa::VaUtils::AddApplicationDataPath(path);
      resources = path + "/resources.txt";
      if (!vespa::VaResourceManager::Instance().LoadResources(resources))
      {
         auto out = ut::log::error() << "Could not load the VESPA resources.";
         out.AddNote() << "Resources: " << resources;
         out.AddNote() << "Base: " << UtRunEnvManager::GetCmeRelBase();
         out.AddNote() << "Suffix: " << UtRunEnvManager::GetCmeRelSuffix();
         loadedResources = false;
      }
   }
   return loadedResources;
}

void wkf::Environment::EntityRemovedCB(vespa::VaEntity* aEntityPtr)
{
   auto* platform = dynamic_cast<Platform*>(aEntityPtr);
   if (platform)
   {
      if (IsPlatformSelected(platform))
      {
         SetPlatformSelected(platform, false);
      }
   }
}

QString wkf::Environment::GetDocumentationDir() const
{
   return QString::fromStdString(UtRunEnvManager::GetRunPath() + "/../documentation");
}

void wkf::Environment::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntity)
{
   auto plugins = mPluginManagerPtr->GetLoadedPlugins();
   for (auto& p : plugins)
   {
      p.second->BuildEntityContextMenu(aMenu, aEntity);
   }
}

void wkf::Environment::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   auto plugins = mPluginManagerPtr->GetLoadedPlugins();
   for (auto& p : plugins)
   {
      p.second->BuildViewerContextMenu(aMenu, aViewerPtr);
   }
}

void wkf::Environment::ClearPlatformGroupings(int aIndex)
{
   auto it = mPlatformToGroupingMap.find(aIndex);
   if (it != mPlatformToGroupingMap.end())
   {
      // clear the groupings for the particular platform
      for (auto& cit : it->second)
      {
         auto jt = mGroupingToPlatformMap[cit].find(aIndex);
         if (jt != mGroupingToPlatformMap[cit].end())
         {
            mGroupingToPlatformMap[cit].erase(jt);
         }
      }
      // remove any groupings that do not contain any platforms
      for (auto iter = mGroupingToPlatformMap.begin(); iter != mGroupingToPlatformMap.end();)
      {
         if (iter->second.empty())
         {
            // remove the grouping
            iter = mGroupingToPlatformMap.erase(iter);
         }
         else
         {
            ++iter;
         }
      }

      mPlatformToGroupingMap.erase(it);
   }
}

void wkf::Environment::SetPlatformGrouping(int aIndex, const PlatformGrouping aGrouping)
{
   bool newGroup = false;
   mPlatformToGroupingMap[aIndex].insert(aGrouping);
   // check to see whether or not it is a new grouping
   if (mGroupingToPlatformMap.find(aGrouping) == mGroupingToPlatformMap.end())
   {
      newGroup = true;
   }
   mGroupingToPlatformMap[aGrouping].insert(aIndex);
   // add the new grouping
   if (newGroup)
   {
      emit NewGrouping(aGrouping.Type(), QString::fromStdString(aGrouping.Name()), aIndex);
   }
   else
   {
      emit GroupingChanged(aGrouping.Type(), QString::fromStdString(aGrouping.Name()), aIndex);
   }
}

void wkf::Environment::RegisterPrefObject(PrefObject* aObjectPtr)
{
   mPrefObjectMap[aObjectPtr->GetName()] = aObjectPtr;
   if (mMainWindowPtr && mMainWindowPtr->GetConfigurationObject())
   {
      mMainWindowPtr->GetConfigurationObject()->RegisterPrefObject(aObjectPtr);
   }
}

void wkf::Environment::UnregisterPrefObject(PrefObject* aObjectPtr)
{
   mPrefObjectMap.erase(aObjectPtr->GetName());
}

size_t wkf::Environment::RegisterPlatformVisibilityFilter(const VisibilityFilter& aFilter)
{
   size_t id = mNextRegistrationId++;
   mPlatformVisibilityFilters.emplace(id, aFilter);
   emit PlatformVisibilityChanged();
   return id;
}

void wkf::Environment::UnregisterPlatformVisibilityFilter(size_t aId)
{
   if (mPlatformVisibilityFilters.erase(aId))
   {
      emit PlatformVisibilityChanged();
   }
}

bool wkf::Environment::IsPlatformVisible(const wkf::Platform* aPlatform)
{
   if (aPlatform)
   {
      bool visible = true;
      // Check all of the visibility filters to see if the Platform is visible
      for (auto& pair : mPlatformVisibilityFilters)
      {
         visible = pair.second(*aPlatform);
         // If a single filter return false, we can quit processing and return false
         if (!visible)
         {
            return false;
         }
      }
      // If all filters return true, then return true
      return true;
   }
   return false;
}

QStringList wkf::Environment::GetUpdaterList() const
{
   return mUpdaterFactory.keys();
}

std::unique_ptr<wkf::Updater> wkf::Environment::GetUpdater(const QString& aUpdaterId, const std::string& aPlatformName) const
{
   auto createFn = mUpdaterFactory.find(aUpdaterId);
   if (createFn != std::end(mUpdaterFactory))
   {
      return (*createFn)(aPlatformName);
   }
   return nullptr;
}

void wkf::Environment::SetFrameUpdateEnabled(bool aEnabled)
{
   // If transitioning to no longer emit UpdateFrame,
   //  first emit one last UpdateFrame, so that anyone connected objects can process remaining data
   if (mFrameUpdatedEnabled && !aEnabled)
   {
      emit UpdateFrame();
   }
   mFrameUpdatedEnabled = aEnabled;
}

wkf::Platform* wkf::Environment::GetPlatformOfInterest() const
{
   if (!mSelectedPlatforms.empty())
   {
      return mSelectedPlatforms.back();
   }
   else
   {
      return nullptr;
   }
}

void wkf::Environment::SetPlatformSelected(Platform* aPlatform, bool aFocus)
{
   if (aPlatform)
   {
      if (aFocus != IsPlatformSelected(aPlatform)) // If selection of platform is changing
      {
         // If selecting a new platform or un-selecting the platform of interest, notify subscribers with the updated
         // platform of interest
         bool platformOfInterestNeedsUpdate = aFocus || (aPlatform == GetPlatformOfInterest());

         if (aFocus)
         {
            mSelectedPlatforms.push_back(aPlatform);
            emit PlatformSelectionChanged({aPlatform}, {});
         }
         else
         {
            mSelectedPlatforms.removeOne(aPlatform);
            emit PlatformSelectionChanged({}, {aPlatform});
         }

         if (platformOfInterestNeedsUpdate)
         {
            emit PlatformOfInterestChanged(GetPlatformOfInterest());
         }
      }
   }
}

// Platforms that become invisible should not longer be selected
void wkf::Environment::PlatformVisibilityChangedHandler()
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      wkf::PlatformList   platformList;
      vespa::VaEntityList list;
      scenario->GetEntityList(list);
      for (auto& i : list)
      {
         wkf::Platform* platPtr = dynamic_cast<wkf::Platform*>(i);
         if (platPtr != nullptr)
         {
            if (IsPlatformSelected(platPtr))
            {
               if (IsPlatformVisible(platPtr))
               {
                  platformList.push_back(platPtr);
               }
               else
               {
                  platPtr->ClearSelection();
               }
            }
         }
      }
      SetPlatformsSelected(platformList);
   }
}

// Select all platforms in aPlatforms.
// Anything that is currently selected that is not contained in aPlatforms will become unselected
void wkf::Environment::SetPlatformsSelected(const PlatformList& aPlatforms)
{
   PlatformList selectedPlatforms;
   PlatformList unselectedPlatforms;
   Platform*    previousPlatformOfInterest = GetPlatformOfInterest();

   // un-select platforms that are currently selected, but not in aPlatforms (desired selection)
   auto it = mSelectedPlatforms.begin();
   while (it != mSelectedPlatforms.end())
   {
      if (std::find(aPlatforms.begin(), aPlatforms.end(), *it) == aPlatforms.end())
      {
         unselectedPlatforms.push_back(*it);
         it = mSelectedPlatforms.erase(it);
      }
      else
      {
         ++it;
      }
   }

   // select platforms that are not currently selected, but are in aPlatforms (desired selection)
   for (const auto& p : aPlatforms)
   {
      if (!IsPlatformSelected(p))
      {
         mSelectedPlatforms.push_back(p);
         selectedPlatforms.push_back(p);
      }
   }

   if (!selectedPlatforms.empty() || !unselectedPlatforms.empty())
   {
      emit PlatformSelectionChanged(selectedPlatforms, unselectedPlatforms);

      Platform* currentPlatformOfInterest = GetPlatformOfInterest();
      if (previousPlatformOfInterest != currentPlatformOfInterest)
      {
         emit PlatformOfInterestChanged(currentPlatformOfInterest);
      }
   }
}

bool wkf::Environment::IsPlatformSelected(const Platform* aPlatform) const
{
   return std::find(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), aPlatform) != mSelectedPlatforms.end();
}

bool wkf::Environment::IsPlatformSelected(const std::string& aPlatformName) const
{
   auto nameCompare = [&aPlatformName](Platform* a) { return (a->GetName() == aPlatformName); };
   return std::find_if(mSelectedPlatforms.begin(), mSelectedPlatforms.end(), nameCompare) != mSelectedPlatforms.end();
}

// this will look a provider for the requested resource associated with the platform
bool wkf::Environment::RequestPlatformResourceInstance(const QString& aResourceName, const QString& aPlatformName)
{
   auto plugins = mPluginManagerPtr->GetLoadedPlugins();
   for (auto& p : plugins)
   {
      if (p.second->RequestPlatformResourceInstance(aResourceName, aPlatformName))
      {
         return true;
      }
   }
   return false;
}

QString wkf::getOpenFileName(QWidget*             aParent /*= Q_NULLPTR*/,
                             const QString&       aCaption /*= QString()*/,
                             const QString&       aDir /*= QString()*/,
                             const QString&       aFilter /*= QString()*/,
                             QString*             aSelectedFilter /*= Q_NULLPTR*/,
                             QFileDialog::Options aOptions /*= QFileDialog::Options()*/)
{
   if (wkfEnv.IsFileBrowsingLocked())
   {
      QMessageBox::information(nullptr,
                               "File Browsing Disabled",
                               "File Browsing has been disabled.  To enable this please contact an administrator.");
      return QString();
   }
   else
   {
      return QFileDialog::getOpenFileName(aParent, aCaption, aDir, aFilter, aSelectedFilter, aOptions);
   }
}

QStringList wkf::getOpenFileNames(QWidget*             aParent /*= Q_NULLPTR*/,
                                  const QString&       aCaption /*= QString()*/,
                                  const QString&       aDir /*= QString()*/,
                                  const QString&       aFilter /*= QString()*/,
                                  QString*             aSelectedFilter /*= Q_NULLPTR*/,
                                  QFileDialog::Options aOptions /*= QFileDialog::Options()*/)
{
   if (wkfEnv.IsFileBrowsingLocked())
   {
      QMessageBox::information(nullptr,
                               "File Browsing Disabled",
                               "File Browsing has been disabled.  To enable this please contact an administrator.");
      return QStringList();
   }
   else
   {
      return QFileDialog::getOpenFileNames(aParent, aCaption, aDir, aFilter, aSelectedFilter, aOptions);
   }
}

QString wkf::getSaveFileName(QWidget*             aParent /*= Q_NULLPTR*/,
                             const QString&       aCaption /*= QString()*/,
                             const QString&       aDir /*= QString()*/,
                             const QString&       aFilter /*= QString()*/,
                             QString*             aSelectedFilter /*= Q_NULLPTR*/,
                             QFileDialog::Options aOptions /*= QFileDialog::Options()*/)
{
   if (wkfEnv.IsFileBrowsingLocked())
   {
      QMessageBox::information(nullptr,
                               "File Browsing Disabled",
                               "File Browsing has been disabled.  To enable this please contact an administrator.");
      return QString();
   }
   else
   {
      return QFileDialog::getSaveFileName(aParent, aCaption, aDir, aFilter, aSelectedFilter, aOptions);
   }
}

WKF_EXPORT QString wkf::getExistingDirectory(QWidget*             aParent /*= Q_NULLPTR*/,
                                             const QString&       aCaption /*= QString()*/,
                                             const QString&       aDir /*= QString()*/,
                                             QFileDialog::Options aOptions /*= QFileDialog::ShowDirsOnly*/)
{
   if (wkfEnv.IsFileBrowsingLocked())
   {
      QMessageBox::information(nullptr,
                               "File Browsing Disabled",
                               "File Browsing has been disabled.  To enable this please contact an administrator.");
      return QString();
   }
   else
   {
      return QFileDialog::getExistingDirectory(aParent, aCaption, aDir, aOptions);
   }
}
