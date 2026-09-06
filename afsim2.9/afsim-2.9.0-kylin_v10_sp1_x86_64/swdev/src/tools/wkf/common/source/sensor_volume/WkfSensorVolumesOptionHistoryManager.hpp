// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SENSORVOLUMEOPTIONHISTORYMANAGER
#define SENSORVOLUMEOPTIONHISTORYMANAGER

#include "wkf_common_export.h"

#include "WkfOptionHistoryManager.hpp"
#include "WkfPlatformGrouping.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT SensorVolumesOptionHistoryManager : public wkf::OptionHistoryManager
{
public:
   SensorVolumesOptionHistoryManager(const QString& mPluginName, QObject* aParent);
   ~SensorVolumesOptionHistoryManager() override = default;

   void UpdateOptionsRulesList(int aOptionId, bool aState, const wkf::PlatformGrouping& aCategory) override;
   void ApplyOptions(const wkf::PlatformList& aPlatforms) override;
   void Clear() override;
   int  GetParentOption(int aType) const override;

private:
   void SetParentOption(int aType, int aParent) override;
   // Write the options rules out to QSettings
   void Save(QSettings& aSettings) override;
   // Read the options rules in from QSettings
   void Load(QSettings& aSettings) override;
   void LoadRules(int aCount, int aParentType, QSettings& aSettings);

   using Rule     = std::pair<wkf::PlatformGrouping, std::pair<int, bool>>;
   using RuleList = std::list<Rule>;
   std::map<int, RuleList> mOptionRules;
   std::map<int, int>      mParent;
   std::set<int>           mParents;
};
} // namespace wkf

#endif
