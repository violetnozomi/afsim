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
#ifndef RVPLUGIN_HPP
#define RVPLUGIN_HPP

#include <QObject>
class UtCalendar;
#include "RvEnvironment.hpp"
#include "RvExport.hpp"
#include "RvPlotUpdater.hpp"
#include "WkfPlugin.hpp"

namespace rv
{
class MsgComment;
class MsgDrawCommand;
class MsgPlatformInfo;
class MsgEmitterModeDefinition;
class MsgVisualPartDefinition;
class MsgWeaponFired;
class MsgWeaponTerminated;
class MsgBookmark;
class PlotUpdater;
class ResultData;
class ResultDb;

template<class PClass>
class PluginT : public PClass
{
   static_assert(std::is_base_of<wkf::Plugin, PClass>::value,
                 "Template argument to WkRvPluginT must be a subclass of wkf::Plugin");

   using ThisClass = PluginT<PClass>;

public:
   PluginT(const QString& aPluginName, const size_t aUniqueId)
      : PClass(aPluginName, aUniqueId)
   {
      // Use a Direct connection to invoke the slot immediately, in the simulation (main) thread
      QObject::connect(&rvEnv, &Environment::AdvanceTimeRead, this, &ThisClass::AdvanceTimeRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::RegularRead, this, &ThisClass::RegularRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::AdvanceTimeWrite, this, &ThisClass::AdvanceTimeWrite, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::SensorModeDefinition, this, &ThisClass::NewSensorModeDefinition, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::WeaponModeDefinition, this, &ThisClass::NewWeaponModeDefinition, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::ClearScenario, this, &ThisClass::ClearScenario, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::PlatformAddedRead, this, &ThisClass::PlatformAddedRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::DrawCommandRead, this, &ThisClass::DrawCommandRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::CommentRead, this, &ThisClass::CommentRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::CommInfoRead, this, &ThisClass::CommInfoRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::RouterInfoRead, this, &ThisClass::RouterInfoRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::NetworkInfoRead, this, &ThisClass::NetworkInfoRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::WeaponFiredRead, this, &ThisClass::WeaponFiredEventRead, Qt::DirectConnection);
      QObject::connect(&rvEnv,
                       &Environment::WeaponTerminatedRead,
                       this,
                       &ThisClass::WeaponTerminatedEventRead,
                       Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::PlatformStatusRead, this, &ThisClass::PlatformStatusEventRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::SetEpoch, this, &ThisClass::SetEpoch, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::MetaDataRead, this, &ThisClass::MetaDataRead, Qt::DirectConnection);
      QObject::connect(&rvEnv, &Environment::BookmarkRead, this, &ThisClass::BookmarkRead, Qt::DirectConnection);
      QObject::connect(&wkfEnv, &wkf::Environment::Initialize, this, &ThisClass::PluginsLoaded, Qt::DirectConnection);
   }
   ~PluginT() override = default;


   virtual std::unique_ptr<rv::PlotUpdater> GetPlotUpdater(const QString&               aX,
                                                           const QString&               aY,
                                                           const wkf::PlatformGrouping& aGrouping,
                                                           const QString&               aSubCategory) const
   {
      return nullptr;
   }

   virtual void BuildItemContextMenu(QMenu* aMenu, QTreeWidgetItem* aItem) {}

private:
   //      virtual void TimeInCacheChanged(float aMin, float aMax) {}
   //      virtual void MaxTimeChanged(float aMax) {}
   //      virtual void Pausing() {}
   //      virtual void Resuming() {}
   //      virtual void PlatformAdded(double aSimTime, const std::string& aName) {}


   // this is for time changes
   virtual void AdvanceTimeRead(const rv::ResultData& aData) {}
   // this is a 2 Hz regular update when time isn't changing (for things that need updates outside of time changes)
   virtual void RegularRead(const rv::ResultData& aData) {}
   virtual void AdvanceTimeWrite(rv::ResultData& aData) {}
   virtual void NewSensorModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode) {}
   virtual void NewWeaponModeDefinition(const rv::MsgEmitterModeDefinition& aNewMode) {}
   virtual void ClearScenario(bool aFullReset) {}
   // note that the following event do NOT happen as they occur, but as they are read
   virtual void PlatformAddedRead(double aSimTime, ResultDb& aData, const rv::MsgPlatformInfo& aPlatform) {}
   virtual void DrawCommandRead(ResultDb& aData, const rv::MsgDrawCommand& aDrawCommand) {}
   virtual void MetaDataRead(const rv::MsgExecData& aExecMsg) {}
   virtual void CommentRead(ResultDb& aData, const rv::MsgComment& aComment) {}
   virtual void CommInfoRead(ResultDb& aData, const rv::MsgCommInfo& aCommInfo) {}
   virtual void NetworkInfoRead(ResultDb& aData, const rv::MsgNetworkInfo& aCommInfo) {}
   virtual void RouterInfoRead(ResultDb& aData, const rv::MsgRouterInfo& aRouterInfo) {}
   virtual void WeaponFiredEventRead(const rv::MsgWeaponFired& aEvent) {}
   virtual void WeaponTerminatedEventRead(const rv::MsgWeaponTerminated& aEvent) {}
   virtual void PlatformStatusEventRead(const rv::MsgPlatformStatus& aEvent) {}
   virtual void SetEpoch(const UtCalendar& aEpoch) {}
   virtual void BookmarkRead(const rv::MsgBookmark& aBookmark) {}
   // This is called after all plug-ins are loaded.  This will allow plug-ins to gather registration information from
   // data plug-ins ie. EventMarkers can add events denoted by a plugin that should have event-markers
   virtual void PluginsLoaded() {}
};
using Plugin = PluginT<wkf::Plugin>;
} // namespace rv

#endif // RVPLUGIN_HPP
