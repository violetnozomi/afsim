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

#ifndef RV_STATISTICS_RULES_HPP
#define RV_STATISTICS_RULES_HPP

#include "WkfPlatformGrouping.hpp"
#include "filter_widget/WkfFilterObject.hpp"

namespace RvStatistics
{
struct StatisticsRule : WkfFilter::Rule
{
   bool TestValue(const QVariant& aModelData, const QString& aFilterData) const final;
   bool Predicate(const QAbstractItemModel* aModel, int aColumn) const final;

   virtual wkf::PlatformGrouping::TypeEnum GetGroupingType() const = 0;
};

struct StatisticsRuleSide final : StatisticsRule
{
   QString                         Name() const override;
   wkf::PlatformGrouping::TypeEnum GetGroupingType() const override;
};

struct StatisticsRuleType final : StatisticsRule
{
   QString                         Name() const override;
   wkf::PlatformGrouping::TypeEnum GetGroupingType() const override;
};

struct StatisticsRuleCategory final : StatisticsRule
{
   QString                         Name() const override;
   wkf::PlatformGrouping::TypeEnum GetGroupingType() const override;
};
} // namespace RvStatistics

#endif
