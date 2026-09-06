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

#include "RvBM_Plugin.hpp"

#include "RvBM_RuleSets.hpp"
#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultMessageArray.hpp"
#include "RvResultPlatform.hpp"
#include "UtQtMemory.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvBM::Plugin, "Battle Management", "", "mystic")

RvBM::Plugin::Plugin(const QString& aName, const size_t aUniqueId)
   : rv::PluginT<wkf::bm::PluginBase>(aName, aUniqueId)
{
}

void RvBM::Plugin::GuiUpdate()
{
   UpdatePlots(mEndTime);
   RedrawPlots();
}

void RvBM::Plugin::InitializePlatformStatus(wkf::bm::PlatformStatus& aPlatformStatus)
{
   aPlatformStatus.AddRuleSet(ut::qt::make_qt_ptr<RvBM::RuleSetDamageFactor>(nullptr));
   aPlatformStatus.AddRuleSet(ut::qt::make_qt_ptr<RvBM::RuleSetFuel>(nullptr));

   aPlatformStatus.ReadUserSettings();
}

void RvBM::Plugin::AddAuxDataRuleSet(wkf::bm::PlatformStatus& aPlatformStatus, QString aKey, wkf::bm::RuleSet::Mode aMode)
{
   aPlatformStatus.AddRuleSet(ut::qt::make_qt_ptr<RvBM::AuxDataRuleSet>(nullptr, aKey, aMode));
}

void RvBM::Plugin::AddResourceAuxDataRuleSet(wkf::bm::PlatformStatus& aPlatformStatus, QString aNumerator, QString aDenominator)
{
   aPlatformStatus.AddRuleSet(ut::qt::make_qt_ptr<RvBM::AuxDataResourceRuleSet>(nullptr, aNumerator, aDenominator));
}

void RvBM::Plugin::RegularRead(const rv::ResultData& aData)
{
   auto* db = aData.GetDb();
   if (db)
   {
      mEndTime = db->GetMaxTime();
   }

   RescanPlatformStatus(aData.GetSimTime());
}

void RvBM::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   SetTimeIndicator(aData.GetSimTime());
   RedrawPlots();

   RescanPlatformStatus(aData.GetSimTime());
}

void RvBM::Plugin::PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform)
{
   auto& data = mPlatformData.AddPlatform(aPlatform.platformIndex());

   data.mStartTime = aPlatform.simTime();

   if (aPlatform.nameValid())
   {
      data.mName = QString::fromStdString(aPlatform.name());
   }
   if (aPlatform.typesValid())
   {
      for (const auto& str : aPlatform.types())
      {
         data.mTypes += QString::fromStdString(str);
      }
   }
   if (aPlatform.categoriesValid())
   {
      for (const auto& str : aPlatform.categories())
      {
         data.mCategories += QString::fromStdString(str);
      }
   }
   if (aPlatform.sideValid())
   {
      data.mSide = QString::fromStdString(aPlatform.side());
   }
}

void RvBM::Plugin::WeaponFiredEventRead(const rv::MsgWeaponFired& aEvent)
{
   // Weapons rely on PlatformAdded to initialize most details.
   if (aEvent.weaponPlatformIndexValid())
   {
      auto& data     = mPlatformData.FindPlatform(aEvent.weaponPlatformIndex());
      data.mIsWeapon = true;
   }
}

void RvBM::Plugin::PlatformStatusEventRead(const rv::MsgPlatformStatus& aEvent)
{
   if (aEvent.removed())
   {
      mPlatformData.RemovePlatform(aEvent.simTime(), aEvent.platformIndex());
   }
}
