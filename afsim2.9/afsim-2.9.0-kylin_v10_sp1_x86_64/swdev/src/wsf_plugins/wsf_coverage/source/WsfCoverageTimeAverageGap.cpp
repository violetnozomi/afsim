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

#include "WsfCoverageTimeAverageGap.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageMeasureUtils.hpp"

namespace wsf
{
namespace coverage
{

//! Return the default value for the measure if there were no accesses.
double TimeAverageGap::GetDefaultValue() const
{
   return 0.0;
}

//! Return the measured value for the given @p aGridAsset.
double TimeAverageGap::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second;
   }
   return retval;
}

void TimeAverageGap::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

//! Return the text header for output files for this MOE.
std::string TimeAverageGap::GetValueHeader() const
{
   return std::string("Time Average Gap [s]");
}

bool TimeAverageGap::InitializeP(Coverage& aCoverage)
{
   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&TimeAverageGap::OnAccessIntervalComplete, this));
   return true;
}

void TimeAverageGap::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mIntervals)
   {
      std::vector<AccessInterval> gaps;
      MeasureUtils::ComputeGaps(gaps, gaIter.second, mCoverageInterval, "Time Average Gap MoE", GetName());
      mData[gaIter.first] = ComputeValue(gaps);
   }
   mIntervals.clear();
}

//! Compute the measure's value given the gap intervals, @p aGaps.
double TimeAverageGap::ComputeValue(std::vector<AccessInterval>& aGaps) const
{
   auto op = [](double aSum, const AccessInterval& aGap) { return aGap.GetDuration() * aGap.GetDuration() + aSum; };

   return std::accumulate(aGaps.begin(), aGaps.end(), 0.0, op) / mCoverageInterval.GetDuration();
}

void TimeAverageGap::OnAccessIntervalComplete(const GridAsset& aGridAsset,
                                              const FreeAsset& /*aFreeAsset*/,
                                              const AccessInterval& aInterval)
{
   auto gaIter = mIntervals.find(aGridAsset);
   if (gaIter != mIntervals.end())
   {
      // There has been an interval for aGridAsset before now.

      // Partition intervals for aGridAsset based on overlap with aInterval.
      auto pred  = [&aInterval](const AccessInterval& aElement) { return !aInterval.HasOverlapWith(aElement); };
      auto start = std::partition(gaIter->second.begin(), gaIter->second.end(), pred);

      if (start == gaIter->second.end())
      {
         // No overlap with any interval, so just append it to the vector.
         gaIter->second.push_back(aInterval);
      }
      else
      {
         // Some intervals overlap, so accumulate via AccessInterval::UnionWith
         auto op = [](const AccessInterval& aLHS, const AccessInterval& aRHS) { return aLHS.UnionWith(aRHS); };
         AccessInterval combined = std::accumulate(start, gaIter->second.end(), aInterval, op);

         // Set the first value of the partitioned range to the accumulated value
         *start = combined;

         // Erase the rest of the intervals which had overlap, as their range is now
         // included in the element at 'start'.
         ++start;
         gaIter->second.erase(start, gaIter->second.end());
      }
   }
   else
   {
      // This is the first interval, so just add it to the interval map.
      mIntervals.emplace(std::make_pair(aGridAsset, std::vector<AccessInterval>{aInterval}));
   }
}

} // namespace coverage
} // namespace wsf
