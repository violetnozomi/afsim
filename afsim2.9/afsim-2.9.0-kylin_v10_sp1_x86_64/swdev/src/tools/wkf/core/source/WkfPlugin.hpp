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

#ifndef WKFPLUGIN_HPP
#define WKFPLUGIN_HPP

#include <memory>

#include <QFlags>
#include <QList>
#include <QObject>
#include <QTreeWidgetItem>
class QAction;
class QMenu;
class QSettings;
class QWidget;
#include "wkf_export.h"

#include "UtPlugin.hpp"
#include "UtQtUiPointer.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfStartupWidgetWrapper.hpp"
#include "WkfTip.hpp"
#include "WkfUpdater.hpp"

#define WKF_PLUGIN_API_VERSION 13
static const char* WKF_PLUGIN_API_COMPILER_STRING = UtPluginCompilerVersionString();

#if defined(_WIN32)
#define WKF_PLUGIN_EXPORT __declspec(dllexport)
#else // Not Win32
#define WKF_PLUGIN_EXPORT
#endif

// Macro that exports the necessary symbols for the PluginManager
// PLUGIN_CLASS        The c++ class name for the class that derives from wkf::Plugin
// PLUGIN_NAME         The pretty name for the plugin intended for display to the users.
// PLUGIN_DESCRIPTION  A string describing the functionality of the plugin.
// PLUGIN_TAGS         A list of vertical bar ('|') separated tags that will be compared to the environment's function
// to determine
//                     the proper set of plugins to load at start-up.
// OPTIONAL ...        A boolean that indicates whether the plugin is to be loaded by default (opt-in or opt-out). If
// not specified, behavior is opt-out.
#define WKF_PLUGIN_DEFINE_SYMBOLS(PLUGIN_CLASS, PLUGIN_NAME, PLUGIN_DESCRIPTION, PLUGIN_TAGS, ...)      \
   extern "C" WKF_PLUGIN_EXPORT void wkf_plugin_registration(wkf::PluginRegistration* aRegistrationPtr) \
   {                                                                                                    \
      aRegistrationPtr->Create(PLUGIN_NAME, PLUGIN_DESCRIPTION, ##__VA_ARGS__);                         \
   }                                                                                                    \
   extern "C" WKF_PLUGIN_EXPORT wkf::Plugin* wkf_plugin_create(const size_t aUniqueId)                  \
   {                                                                                                    \
      return new PLUGIN_CLASS(PLUGIN_NAME, aUniqueId);                                                  \
   }                                                                                                    \
   extern "C" WKF_PLUGIN_EXPORT const char* wkf_plugin_get_tags() { return PLUGIN_TAGS; }

namespace wkf
{
class Action;
class Environment;
class OptionHistoryManager;
class PrefObject;
class PrefWidget;

class PluginRegistration : public UtPluginVersion
{
   friend class PluginData;

public:
   void Create(const char* aName, const char* aDescription, bool aLoadByDefault = true)
   {
      mMajor           = WKF_PLUGIN_API_VERSION;
      mMinor           = 0;
      mCompilerVersion = WKF_PLUGIN_API_COMPILER_STRING;

      mName        = aName;
      mDescription = aDescription;

      mLoadByDefault = aLoadByDefault;
   }

   std::string mName{};
   std::string mDescription{};
   bool        mLoadByDefault{true};

private:
   PluginRegistration()
      : UtPluginVersion(0, 0, nullptr)
   {
   }
};

class WKF_EXPORT Plugin : public QObject
{
   Q_OBJECT

public:
   // Roles describe the functionality of the plug-in in a way that can be understand by Wk and other plugins.
   // A plugin may have multiple roles but some roles should be treated as mutually-exclusive such as primary viewer
   // and the secondary viewer. Roles are to be assigned in the constructor of the plug-in and should not change.
   enum Roles
   {
      eNO_ROLE          = 0,
      ePRIMARY_VIEWER   = 1,
      eSECONDARY_VIEWER = 2,
      eCOCKPIT          = 4,
      eSCENARIO_MANAGER = 8
   };

   Plugin(const QString&        aPluginName,
          const size_t          aUniqueId,
          const QFlags<Roles>&  aRoles                   = eNO_ROLE,
          OptionHistoryManager* aOptionHistoryManagerPtr = nullptr);
   ~Plugin() override = default;

   // Returns a list of preferences widgets, so that the Preferences Dialog and display the widgets.
   // If this list is empty, the plugin will not appear in the list of plug-ins in Preferences.
   virtual QList<PrefWidget*> GetPreferencesWidgets() const { return QList<PrefWidget*>(); }

   // Returns a list of WkActions (shortcuts), so that the Key Bindings Dialog can display the actions.
   virtual QList<Action*> GetActions() const { return QList<Action*>(); }

   // Returns a list of widgets to add to the startup dialog.
   virtual QList<wkf::StartupWidgetWrapper*> GetStartupWidgets() const { return QList<wkf::StartupWidgetWrapper*>(); }


   // Return a list of tips that can be presented to the user to help them understand how to use the
   //  functionality presented in this associated plugin.
   virtual QList<Tip> GetTips() const { return QList<Tip>(); }

   // This method is invoked whenever a widget requests a custom context menu for an entity (platform,
   // track, or POI.)  This method will get a QMenu pointer and the entity pointer the menu is being
   // built for.  The plugin may then add its own actions to the context menu.
   virtual void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) {}

   // This method is invoked whenever a widget requests a custom context menu for an attachment.
   // This method will get a QMenu pointer and the entity pointer the menu is being
   // built for.  The plugin may then add its own actions to the context menu.
   virtual void BuildAttachmentContextMenu(QMenu* aMenu, vespa::VaAttachment* aAttachmentPtr, unsigned int aSubPartId)
   {
   }

   // This method is invoked whenever a widget requests a custom context menu for a view.  This
   // function will get a QMenu pointer and a pointer to the viewer the menu is being build for.
   //  The plugin may then add its own actions to the context menu.
   virtual void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) {}

   // if the plugin supports the type indicated by the aResourceName, it will generate it and indicate success with the return
   // the resource is assumed to operate independently of the requester
   // this is a "slippery slope", talk to an architect before trying to do anything fancy here!
   virtual bool RequestPlatformResourceInstance(const QString& aResourceName, const QString& aPlatformName)
   {
      return false;
   }

   // Return a List of Items to be displayed in the Platform Data Dock Widget
   virtual QList<QTreeWidgetItem*> GetPlatformData(const std::string& aStringId) { return QList<QTreeWidgetItem*>(); }
   // Notifies the plugin when one of the Items return in GetPlatformData() is selected
   virtual void PlatformDataItemSelected(QTreeWidgetItem* aItem) {}
   // Notifies the plugin when one of the Items return in GetPlatformData() is de-selected
   virtual void PlatformDataItemUnSelected() {}

   // Returns platform options relevant to the plugin
   QList<QTreeWidgetItem*> GetPlatformOptions(const std::string& aStringId);

   virtual void ResetOptionStates() {}
   // Set/GetPlatformOptionState get or set the current state of an option for a platform
   virtual void           SetPlatformOptionState(int aOptionId, bool aState, Platform* aPlatformPtr) {}
   virtual Qt::CheckState GetPlatformOptionState(int aOptionId, const Platform* aPlatformPtr) const
   {
      return Qt::Unchecked;
   }

   // Adds a new option rule to the rule list
   void UpdateOptions(int aOptionId, bool aState, wkf::PlatformGrouping aCategory);
   // Defines an option (or group of options) that can be turned on/off per platform/side/group/category.
   QTreeWidgetItem* RegisterOptionGroup(QTreeWidgetItem* aParentPtr, const QString& aString, bool aIndividual = false);
   QTreeWidgetItem* RegisterOption(QTreeWidgetItem* aParentPtr,
                                   const QString&   aString,
                                   const QString&   aOptName        = "",
                                   bool             aIndividual     = false,
                                   bool             aInitCheckState = false,
                                   bool             aSetDefaultTrue = false);
   void             ShowIndividualOptions(bool aState);

   // Shutdowns the plugin
   virtual void Shutdown() {}
   // Returns a type-safe container of OR-combinations of the Roles enumeration
   QFlags<Roles> GetRoles() const { return mRoles; }

   // Returns the name of the plugin
   const QString& GetName() const { return mPluginName; }

   // Returns the name of an option for this plugin
   const QString OptionName(int aOptionId) const;

   virtual std::unique_ptr<Updater> GetUpdater(const QString&     aPlatformName,
                                               const QString&     aDatum,
                                               const unsigned int aContext = 0) const
   {
      return nullptr;
   }

   const size_t UniqueId() const { return mUniqueId; }

protected:
   //! This class exists to clean up plug-in's UI.  A plug-in's UI elements may sometimes either be the
   //! responsibility of the plugin or the MainWindow, depending on if the UI ever shows.
   //! This automates the conditional cleanup.
   //! @note This alias is here for backwards compatibility
   template<class T>
   using PluginUiPointer = ut::qt::UiPointer<T>;

   // Load the settings from the specified QSettings file.
   virtual void LoadSettings(QSettings& aSettings) {}
   // Save the settings to the specified QSettings file.
   virtual void SaveSettings(QSettings& aSettings) {}
   // This slot is triggered on a regular timer to notify the plugin to update its data/displays
   virtual void GuiUpdate() {}

   // Clear individual option (those that only apply to a single platform) TreeWidgetItems from the UI and local data
   virtual void ClearIndividualOptions();
   // Show or hide individual options TreeWidgetItems
   bool ShowIndividualOptions() { return mShowIndividualOptions; }

   void                             SetOptionHistoryManager(wkf::OptionHistoryManager* aOptionHistoryManager);
   const wkf::OptionHistoryManager* GetOptionHistoryManager() const { return mOptionHistoryManagerPtr; }

private:
   // inform the plugin that we are interested in individual options
   virtual void RegisterIndividualOptions(const std::string& aStringId) {}

   vespa::VaCallbackHolder mCallbacks;
   QString                 mPluginName;
   size_t                  mUniqueId;
   QFlags<Roles>           mRoles;

   QList<QTreeWidgetItem*> mPlatformOptions;
   QList<QTreeWidgetItem*> mIndividualRootOptions;
   QList<QTreeWidgetItem*> mIndividualOptions;
   QList<int>              mIndividualOptionIds;
   bool                    mShowIndividualOptions;
   OptionHistoryManager*   mOptionHistoryManagerPtr;
};
} // namespace wkf
#endif // WKFPLUGIN_HPP
