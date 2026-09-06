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

#ifndef WSFCOVERAGENUMBEROFGAPS_HPP
#define WSFCOVERAGENUMBEROFGAPS_HPP

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

//! An MOE that measures the number of gaps in coverage.
class WSF_COVERAGE_EXPORT NumberOfGaps : public Measure
{
public:
   explicit NumberOfGaps(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   NumberOfGaps(const NumberOfGaps& aOther) = default;
   ~NumberOfGaps() override                 = default;

   // Measure overrides

   NumberOfGaps* Clone() const override { return new NumberOfGaps{*this}; }

   bool ProcessInput(UtInput& aInput) override;

   double GetDefaultValue() const override;

   //! Return the measured value for the given @p aGridAsset.
   double GetMeasuredValue(const GridAsset& aGridAsset) const override;

   void CollectionStarting(Coverage& aCoverage, double aSimTime) override;

   //! Return the text header for output files for this MOE.
   std::string GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_NUMBER_OF_GAPS_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cTOTAL,
      cIN_SPAN,
      cMINIMUM_PER_TIME,
      cMAXIMUM_PER_TIME,
      cMEAN_PER_TIME,
      cIN_SPAN_PER_TIME,
      cUNKNOWN
   };

   SubType GetSubType() const { return mSubType; }

   //! If the subtype is a per-time subtype, then this is the duration of the sample in seconds.
   double GetSampleSpan() const { return mSampleSpan; }

   //! If the subtype is cIN_SPAN or cPER_DAY_IN_SPAN, then get the minimum duration of coverage gaps to be
   //! counted. Otherwise, return -1.0.
   double GetMinParameter() const { return mMinParameter; }

   //! If the subtype is cIN_SPAN or cPER_DAY_IN_SPAN, then get the maximum duration of coverage gaps to be
   //! counted. Otherwise, return -1.0.
   double GetMaxParameter() const { return mMaxParameter; }

private:
   bool InitializeP(Coverage& aCoverage) override;
   void CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;

   double ComputeValue(std::vector<AccessInterval>& aGaps) const;

   double ComputeTotal(const std::vector<AccessInterval>& aGaps) const;

   double ComputeInSpan(const std::vector<AccessInterval>& aGaps) const;

   double ComputeMax(const std::vector<AccessInterval>& aGaps) const;

   double ComputeMin(const std::vector<AccessInterval>& aGaps) const;

   double ComputeMean(const std::vector<AccessInterval>& aGaps) const;

   double ComputeSpanPerTime(const std::vector<AccessInterval>& aGaps) const;

   void ComputeSampleCount(const std::vector<AccessInterval>& aGaps, std::vector<int>& aSamples) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);

   //! Returns the number of parameters the subtype has.
   static int SubtypeHasParameter(SubType aSubType);

   //! The coverage intervals.
   std::map<GridAsset, std::vector<AccessInterval>> mIntervals{};

   //! The processed data. The value has a different meaning based on the sub-type selected.
   std::map<GridAsset, double> mData{};

   //! The date and time of the start of the coverage interval.
   UtCalendar mStartEpoch{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};

   //! The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   //! The length per time sample in per-time sub types. "per ____."
   double mSampleSpan{UtCalendar::cSEC_IN_DAY};

   //! The minimum parameter value for those sub-types needing one.
   double mMinParameter{-1.0};

   //! The maximum parameter value for those sub-types needing one.
   double mMaxParameter{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGENUMBEROFGAPS_HPP
