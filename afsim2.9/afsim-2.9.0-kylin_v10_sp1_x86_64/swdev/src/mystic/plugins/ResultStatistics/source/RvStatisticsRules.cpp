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

#include "RvStatisticsRules.hpp"

#include "UtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

namespace RvStatistics
{
static unsigned int PlatformIndex(const QString& aName, bool& aValid)
{
   aValid         = false;
   auto* scenario = vaEnv.GetStandardScenario();
   if (scenario == nullptr)
   {
      return 0; // dummy return
   }
   auto* platform = scenario->FindPlatformByName(aName.toStdString());
   if (platform == nullptr)
   {
      return 0; // dummy return
   }
   aValid = true;
   return platform->GetIndex();
}

bool StatisticsRule::TestValue(const QVariant& aModelData, const QString& aFilterData) const
{
   bool                            valid;
   wkf::PlatformGrouping           group(GetGroupingType(), aFilterData.toStdString());
   std::set<wkf::PlatformGrouping> cat = wkfEnv.GetGroupings(PlatformIndex(aModelData.toString(), valid));
   return valid && (cat.find(group) != cat.end());
}

bool StatisticsRule::Predicate(const QAbstractItemModel* aModel, int aColumn) const
{
   if (aModel != nullptr)
   {
      const QString columnTitle = aModel->headerData(aColumn, Qt::Horizontal).toString();

      return columnTitle == "platform" || columnTitle == "interactor";
   }
   return false;
}

// Side

QString StatisticsRuleSide::Name() const
{
   return "is on side";
}

wkf::PlatformGrouping::TypeEnum StatisticsRuleSide::GetGroupingType() const
{
   return wkf::PlatformGrouping::eSide;
}

// Type

QString StatisticsRuleType::Name() const
{
   return "is of type";
}

wkf::PlatformGrouping::TypeEnum StatisticsRuleType::GetGroupingType() const
{
   return wkf::PlatformGrouping::eType;
}

// Category

QString StatisticsRuleCategory::Name() const
{
   return "is of category";
}

wkf::PlatformGrouping::TypeEnum StatisticsRuleCategory::GetGroupingType() const
{
   return wkf::PlatformGrouping::eCategory;
}
} // namespace RvStatistics
