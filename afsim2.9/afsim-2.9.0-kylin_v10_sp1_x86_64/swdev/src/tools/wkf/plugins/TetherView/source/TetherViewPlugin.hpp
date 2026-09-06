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
#ifndef TETHERVIEWPLUGIN_HPP
#define TETHERVIEWPLUGIN_HPP

#include <unordered_set>

#include "TetherViewDockWidget.hpp"
#include "TetherViewPrefWidget.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfEntity.hpp"
#include "WkfPlugin.hpp"

namespace TetherView
{
class Plugin : public wkf::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   void                    PlatformsAddedCB(const wkf::PlatformList& aPlatforms);
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;
   QList<wkf::Action*>     GetActions() const override { return mActions; }

private:
   virtual DockWidget* ConnectToPlatform(const QString& aPlatformName);
   virtual DockWidget* LookAt(const QString& aFrom, const QString& aTo);
   virtual void        Tether();

   // Load the settings from the specified QSettings file.
   void LoadSettings(QSettings& aSettings) override;
   // Save the settings to the specified QSettings file.
   void SaveSettings(QSettings& aSettings) override;

   void HeadingLockEnabledChangedCB(bool aEnabled);
   void LightingEnabledChangedCB(bool aEnabled);

   int                                 mKeyCount;
   std::map<int, QPointer<DockWidget>> mDockWidgets;

   struct Hasher
   {
      size_t operator()(const QString& key) const noexcept { return std::hash<std::string>()(key.toStdString()); }
   };

   std::unordered_set<QString, Hasher> mPlatformTetheredOnStartList;
   vespa::VaCallbackHolder             mCallbacks;
   PluginUiPointer<PrefWidget>         mPrefWidgetPtr;
   QList<wkf::Action*>                 mActions;
};
} // namespace TetherView
#endif
