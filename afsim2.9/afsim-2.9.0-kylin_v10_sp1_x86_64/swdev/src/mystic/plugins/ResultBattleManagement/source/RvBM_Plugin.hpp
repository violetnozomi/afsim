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

#ifndef RV_BATTLE_MANAGER_PLUGIN_HPP
#define RV_BATTLE_MANAGER_PLUGIN_HPP

#include "RvPlugin.hpp"
#include "statistics/WkfBM_PlatformDatabase.hpp"
#include "statistics/WkfBM_PluginBase.hpp"

namespace rv
{
class MsgPlatformInfo;
class MsgWeaponFired;
class MsgPlatformStatus;
} // namespace rv

namespace RvBM
{
class Plugin : public rv::PluginT<wkf::bm::PluginBase>
{
   Q_OBJECT
public:
   Plugin(const QString& aName, const size_t aUniqueId);

private:
   void GuiUpdate() override;

   void InitializePlatformStatus(wkf::bm::PlatformStatus& aPlatformStatus) override;
   void AddAuxDataRuleSet(wkf::bm::PlatformStatus& aPlatformStatus, QString aKey, wkf::bm::RuleSet::Mode aMode) override;
   void AddResourceAuxDataRuleSet(wkf::bm::PlatformStatus& aPlatformStatus, QString aNumerator, QString aDenominator) override;

   void RegularRead(const rv::ResultData& aData) override;
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform) override;
   void WeaponFiredEventRead(const rv::MsgWeaponFired& aEvent) override;
   void PlatformStatusEventRead(const rv::MsgPlatformStatus& aEvent) override;

   double mEndTime = 0;
};
} // namespace RvBM

#endif
