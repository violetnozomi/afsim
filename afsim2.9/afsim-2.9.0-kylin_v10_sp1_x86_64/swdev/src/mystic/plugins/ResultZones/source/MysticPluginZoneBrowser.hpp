// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MysticPluginZoneBrowser_HPP
#define MysticPluginZoneBrowser_HPP

#include <unordered_map>

#include "MysticPluginZoneDockWidget.hpp"
#include "RvPlugin.hpp"
#include "zone_browser/WkfZoneBrowserDataContainer.hpp"
#include "zone_browser/WkfZoneBrowserPrefWidget.hpp"

namespace rv
{
class ColorF;
}

namespace MysticZoneBrowser
{
class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void ZoneRead(rv::MsgBase& aMessage);

private:
   void ClearScenario(bool aFullReset) override;
   void AddColors(wkf::ZoneSetData& aData, const rv::ColorF& aLineColor, const rv::ColorF& aFillColor);
   void PlatformActiveChangedCB(wkf::Platform* aPlatformPtr, bool aActive);
   void ApplyColorPrefs(wkf::ColorOption aColorChoice);

   wkf::ZoneBrowserDataContainer               mDataContainer;
   PluginUiPointer<wkf::ZoneBrowserPrefWidget> mPrefWidgetPtr;
   PluginUiPointer<DockWidget>                 mDockWidgetPtr;
   vespa::VaCallbackHolder                     mCallbacks;
   ut::Random                                  mRandom;

   // Necessary to store all data to handle platform destruction and delayed message processing
   std::unordered_map<std::string, std::vector<wkf::ZoneSetData>> mPlatformZoneData; // Maps messages to owning platforms
};
} // namespace MysticZoneBrowser

#endif
