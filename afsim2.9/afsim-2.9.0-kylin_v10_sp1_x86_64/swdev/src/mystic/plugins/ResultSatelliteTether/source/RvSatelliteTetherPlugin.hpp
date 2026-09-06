// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVSATELLITETETHERPLUGIN_HPP
#define RVSATELLITETETHERPLUGIN_HPP

#include "RvPlugin.hpp"
#include "satellite_tether/WkfSatelliteTetherPrefWidget.hpp"

namespace wkf
{
class Platform;
class SatelliteTetherDockWidget;
} // namespace wkf

namespace RvSatelliteTether
{
class DockWidget;

class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t& aUniqueId);
   ~Plugin() override = default;
   QList<wkf::Action*> GetActions() const override { return mActions; }

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;
   void ClearScenario(bool aFullReset) override;
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

protected:
private:
   void Tether();
   void ConnectToPlatform(const QString& aPlatformName);

   PluginUiPointer<wkf::SatelliteTetherPrefWidget> mPrefWidget;
   QList<wkf::Action*>                             mActions;
   std::map<unsigned int, DockWidget*>             mWidgetList;
   unsigned int                                    mWidgetNamer;
};
} // namespace RvSatelliteTether
#endif
