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

#ifndef WSFCOVERAGENUMBEROFACCESSES_HPP
#define WSFCOVERAGENUMBEROFACCESSES_HPP

#include "wsf_coverage_export.h"

#include <map>
#include <vector>

#include "UtCalendar.hpp"
#include "WsfCoverageAccessInterval.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageMeasure.hpp"

namespace wsf
{
namespace coverage
{

//! An MOE that measures how many times a grid point accesses a free asset.
class WSF_COVERAGE_EXPORT NumberOfAccesses : public Measure
{
public:
   explicit NumberOfAccesses(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   NumberOfAccesses(const NumberOfAccesses& aOther) = default;
   ~NumberOfAccesses() override                     = default;

   // Measure overrides
   NumberOfAccesses* Clone() const override { return new NumberOfAccesses{*this}; }
   bool              ProcessInput(UtInput& aInput) override;
   double            GetDefaultValue() const override;
   double            GetMeasuredValue(const GridAsset& aGridAsset) const override;
   void              CollectionStarting(Coverage& aCoverage, double aSimTime) override;
   std::string       GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_NUMBER_OF_ACCESSES_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cIN_SPAN,
      cIN_SPAN_PER_TIME,
      cMAX_PER_TIME,
      cMEAN_PER_TIME,
      cMIN_PER_TIME,
      cTOTAL,
      cUNKNOWN
   };

   //! Get the subtype of the MoE.
   SubType GetSubType() const { return mSubType; }

   //! Get the duration of each sample for per-time subtypes.
   double GetSampleSpan() const { return mSampleSpan; }

   //! Get the minimum (first) parameter passed to in_span or per_day_in_span, or -1.0 if a different subtype was used.
   double GetMinParameter() const { return mMinParameter; }

   //! Get the maximum (second) parameter passed to in_span or per_day_in_span, or -1.0 if a different subtype was used.
   double GetMaxParameter() const { return mMaxParameter; }

private:
   bool InitializeP(Coverage& aCoverage) override;
   void CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;

   double ComputeValue(std::vector<AccessInterval>& aIntervals) const;

   void ComputePerSampleCount(const std::vector<AccessInterval>& aGaps, std::vector<int>& aSamples) const;

   double ComputeInSpan(std::vector<AccessInterval>& aIntervals) const;
   double ComputePerSampleInSpan(std::vector<AccessInterval>& aIntervals) const;
   double ComputePerSampleMax(std::vector<AccessInterval>& aIntervals) const;
   double ComputePerSampleMean(std::vector<AccessInterval>& aIntervals) const;
   double ComputePerSampleMin(std::vector<AccessInterval>& aIntervals) const;
   double ComputeTotal(std::vector<AccessInterval>& aIntervals) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);
   static int     SubtypeHasParameter(SubType aSubType);

   //! The coverage intervals.
   std::map<GridAsset, std::vector<AccessInterval>> mIntervals{};

   //! The processed data. The value has a different meaning based on the sub-type selected.
   std::map<GridAsset, double> mData{};

   //! The coverage interval.
   AccessInterval mCoverageInterval{};

   //! The date and time at which coverage begins.
   UtCalendar mStartEpoch{};

   //! The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   //! The duration of each sample for per-time subtypes.
   double mSampleSpan{UtCalendar::cSEC_IN_DAY};

   //! The minimum parameter passed to in_span or per_day_in_span.
   double mMinParameter{-1.0};

   //! The maximum parameter passed to in_span or per_day_in_span.
   double mMaxParameter{-1.0};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGENUMBER_OF_ACCESSES_HPP
