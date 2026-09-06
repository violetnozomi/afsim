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

#ifndef WSFCOVERAGETIMEAVERAGEGAP_HPP
#define WSFCOVERAGETIMEAVERAGEGAP_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <vector>

#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

//! An MOE that measures timing of gaps in coverage.
class WSF_COVERAGE_EXPORT TimeAverageGap : public Measure
{
public:
   explicit TimeAverageGap(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   TimeAverageGap(const TimeAverageGap& aOther) = default;
   ~TimeAverageGap() override                   = default;

   // Measure overrides
   TimeAverageGap* Clone() const override { return new TimeAverageGap{*this}; }
   double          GetMeasuredValue(const GridAsset& aGridAsset) const override;
   double          GetDefaultValue() const override;
   void            CollectionStarting(Coverage& aCoverage, double aSimTime) override;
   std::string     GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_TIME_AVERAGE_GAP_MOE"};

private:
   bool InitializeP(Coverage& aCoverage) override;
   void CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;

   double ComputeValue(std::vector<AccessInterval>& aGaps) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   //! The coverage intervals.
   std::map<GridAsset, std::vector<AccessInterval>> mIntervals{};

   //! The processed data.
   std::map<GridAsset, double> mData{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGETIMEAVERAGEGAP_HPP
