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

#ifndef WSFCOVERAGECOVERAGETIME_HPP
#define WSFCOVERAGECOVERAGETIME_HPP

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

//! An MoE that measures the amount of time covered.
class WSF_COVERAGE_EXPORT CoverageTime : public Measure
{
public:
   explicit CoverageTime(const WsfScenario& aScenario)
      : Measure{aScenario}
   {
      SetType(cTYPE);
   }
   CoverageTime(const CoverageTime& aOther) = default;
   ~CoverageTime() override                 = default;

   // Measure overrides
   CoverageTime* Clone() const override { return new CoverageTime{*this}; }

   double GetDefaultValue() const override;

   bool ProcessInput(UtInput& aInput) override;

   double GetMeasuredValue(const GridAsset& aGridAsset) const override;

   void CollectionStarting(Coverage& aCoverage, double aSimTime) override;

   std::string GetValueHeader() const override;

   constexpr static const char* cTYPE{"WSF_COVERAGE_TIME_MOE"};

   //! The available sub-types for this MOE
   enum class SubType
   {
      cUNKNOWN,
      cTOTAL,
      cPERCENT,
      cTOTAL_TIME_ABOVE,
      cPERCENT_TIME_ABOVE,
      cMAX_PER_TIME,
      cMAX_PERCENT_PER_TIME,
      cMIN_PER_TIME,
      cMIN_PERCENT_PER_TIME,
      cMEAN_PER_TIME,
      cMEAN_PERCENT_PER_TIME,
      cSTD_DEV_PER_TIME,
      cSTD_DEV_PERCENT_PER_TIME
   };

   //! Get the sub-type enumeration that describes this CoverageTime instance.
   SubType GetSubType() const { return mSubType; }

   //! Get the duration of samples for per-time subtypes.
   double GetSampleSpan() const { return mSampleSpan; }

   //! Get the value of the parameter given to total_time_above or percent_time_above.
   //! \return  The value of the parameter given to total_time_above or percent_time_above
   //!          if one of those is the sub-type, 1 otherwise.
   double GetParameter() const { return mParameter; }

private:
   enum class ParamType
   {
      cPER_TIME,
      cPER_TIME_LIMITED,
      cTIME_AFTER,
      cNONE
   };

   // GridData contains all data required for calculating the value for a single GridAsset.
   struct GridData
   {
      // We need to know, for two of the sub-types, how much coverage time happened when there
      // is coverage from at least 'n' free assets, where 'n' is a positive integer.
      // free asset 1: #######
      // free asset 2:    #######
      // free asset 3:       #######
      // free asset 4:          #######
      // Suppose the '#' in the above graph represent access intervals. There are 4 assets with intervals that have some
      // overlap. layer 4     : layer 3     :       #  # layer 2     :    ########## layer 1     : ################ What
      // we have done here is 'collapsed' them such that each layer has the total acces intervals with at least that
      // many accesses. This way, in order to know the total/percent time above a certain number of accesses, we can
      // just find the total time of the respective layer. There can be multiple days represented per layer.
      // mIntervals[layer][sample][interval]
      std::vector<std::vector<std::vector<AccessInterval>>> mIntervals;

      // mData is the calculated value.
      double mData{};
   };

   bool InitializeP(Coverage& aCoverage) override;

   void CollectionCompletingP(Coverage& aCoverage, double aSimTime) override;

   double ComputeValue(std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeTotal(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputePercent(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMaxPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMaxPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMinPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMinPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMeanPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeMeanPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeStdDevPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double ComputeStdDevPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const;

   double GetMidnightForSample(int aSample) const;

   void OnAccessIntervalComplete(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval);

   void Collapse(const GridAsset& aGridAsset, const AccessInterval& aInterval, int aLevel);

   static SubType GetSubtypeFromIdentifier(const std::string& aIdentifier);

   static ParamType SubtypeHasParameter(SubType aSubType);

   // The coverage intervals.
   std::map<GridAsset, GridData> mData{};

   // The dUTC calendar date and time when mCoverageInterval starts.
   UtCalendar mStartEpoch{};

   // The coverage interval.
   AccessInterval mCoverageInterval{};

   // The selected sub-type.
   SubType mSubType{SubType::cUNKNOWN};

   // The duration of samples for per-time subtypes.
   double mSampleSpan{UtCalendar::cSEC_IN_DAY};

   // The parameter value for those sub-types needing one.
   int mParameter{1};
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGECOVERAGETIME_HPP
