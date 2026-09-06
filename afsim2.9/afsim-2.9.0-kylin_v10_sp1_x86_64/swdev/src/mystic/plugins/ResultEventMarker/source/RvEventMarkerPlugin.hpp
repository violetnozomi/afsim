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
#ifndef RVEVENTMARKERPLUGIN_HPP
#define RVEVENTMARKERPLUGIN_HPP

#include "RvDataExtension.hpp"
#include "RvPlugin.hpp"
#include "event_marker/WkfEventMarkerPluginBase.hpp"

namespace wkf
{
class Platform;
}

// Developers can add event markers from data plug-ins by doing the following (ResultDataSpace is the standard example)
// 1. Implement custom event handlers (base class in RvDataExtension) for your events
// 2. Implement your derived GetEventList() to communicate the event handlers to this plug-in

namespace RvEventMarker
{
class Plugin : public rv::PluginT<wkf::EventMarkerPluginBase>
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;

private:
   vespa::VaPosition GetPositionAtTime(double aTime, int aPlatformIndex, rv::ResultDb* aDatabase);
   void              ClearScenario(bool aFullReset) override;
   void              PluginsLoaded() override;

   float                                                                   mLastTime;
   std::map<std::string, std::unique_ptr<rv::DataExtension::EventHandler>> mCustomEvents;
};
} // namespace RvEventMarker
#endif
