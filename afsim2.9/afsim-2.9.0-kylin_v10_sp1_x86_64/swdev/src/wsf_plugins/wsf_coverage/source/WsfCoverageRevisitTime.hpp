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

#ifndef WSFCOVERAGEREVISITTIME_HPP
#define WSFCOVERAGEREVISITTIME_HPP

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
class WSF_COVERAGE_EXPORT RevisitTime : public Measure
{
public:
   explicit RevisitTime(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   RevisitTime(const RevisitTime& aOther) = default;
   ~RevisitTime() override                = default;

   // Measure overrides
   RevisitTime* Clone() const override { return new RevisitTime{*this}; }
   bool         ProcessInput(UtInput& aInput) override;
   double       GetMeasuredValue(const GridAsset& aGridAsset) const override;
   double       GetDefaultValue() const override;
   void         CollectionStarting(Coverage& aCoverage, double aSimTime) override;
   std::string  GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_REVISIT_TIME_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cUNKNOWN,
      cMEAN,
      cMINIMUM,
      cMAXIMUM,
      cSTD_DEV,
      cNUM_PERCENT_BELOW,
      cPERCENT_BELOW,
      cPERCENT_BELOW_GAPS_ONLY
   };

   SubType GetSubType() const { return mSubType; }
   double  GetParameter() const { return mParameter * 100.0; }

private:
   bool   InitializeP(Coverage& aCoverage) override;
   void   CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;
   double ComputeValue(std::vector<AccessInterval>& aGaps) const;
   double ComputeMean(const std::vector<AccessInterval>& aGaps) const;
   double ComputeMin(const std::vector<AccessInterval>& aGaps) const;
   double ComputeMax(const std::vector<AccessInterval>& aGaps) const;
   double ComputeStdDev(const std::vector<AccessInterval>& aGaps) const;
   double ComputeNumPercentBelow(std::vector<AccessInterval>& aGaps) const;
   double ComputePercentBelow(std::vector<AccessInterval>& aGaps) const;
   double ComputePercentBelowGapsOnly(std::vector<AccessInterval>& aGaps) const;
   double ComputePercentageOffset(std::vector<AccessInterval>& aGaps, double aOffset) const;
   void   SortGapsByDuration(std::vector<AccessInterval>& aGaps) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);
   static bool    SubtypeHasParameter(SubType aSubType);

   //! The coverage intervals.
   std::map<GridAsset, std::vector<AccessInterval>> mIntervals{};

   //! The processed data. The value has a different meaning based on the sub-type selected.
   std::map<GridAsset, double> mData{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};

   //! The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   //! The parameter value for those sub-types needing one.
   double mParameter{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEREVISITTIME_HPP
