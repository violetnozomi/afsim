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

#include "WsfCoverageMeasureUtils.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include "UtCalendar.hpp"
#include "UtLog.hpp"

namespace wsf
{
namespace coverage
{
namespace MeasureUtils
{

void ComputeGaps(std::vector<AccessInterval>& aGaps,
                 std::vector<AccessInterval>& aIntervals,
                 const AccessInterval&        aCoverageInterval,
                 const std::string&           aMoE_Type,
                 const std::string&           aMoE_ObjectName)
{
   // The intervals may not be time ordered, so sort them.
   auto startsBefore = [](const AccessInterval& aLHS, const AccessInterval& aRHS)
   { return aLHS.GetStart() < aRHS.GetStart(); };
   std::sort(aIntervals.begin(), aIntervals.end(), startsBefore);

   // Now we are sure that the intervals are non-overlapping, and ordered.
   // Walk through them and save off the gaps as the complement of the access intervals.

   double start = aCoverageInterval.GetStart();
   for (auto& interval : aIntervals)
   {
      double end = interval.GetStart();
      if (end > start)
      {
         aGaps.emplace_back(start, end);
      }
      else if (end < start)
      {
         // Something has gone wrong
         auto err = ut::log::error() << "Gap interval inverted.";
         err.AddNote() << "In " << aMoE_Type << ", a gap has an end before it starts.";
         err.AddNote() << "MOE: " << aMoE_ObjectName;
         throw std::runtime_error{"Inverted interval"};
      }
      // else, start == end, an empty gap. Do not add to gaps.

      start = interval.GetEnd();
   }
   if (aCoverageInterval.GetEnd() > start)
   {
      aGaps.emplace_back(start, aCoverageInterval.GetEnd());
   }
}

long GetIntervalStartSample(const AccessInterval& aInterval,
                            const UtCalendar&     aStartEpoch,
                            double                aCoverageStart,
                            double                aSampleDuration)
{
   double startSecond = aStartEpoch.GetTime();
   long startSample = static_cast<long>(startSecond) / static_cast<long>(aSampleDuration); // I *do* want this to truncate.
   double endSecond = aInterval.GetStart() - aCoverageStart + startSecond;
   long endSample = static_cast<long>(endSecond) / static_cast<long>(aSampleDuration); // I *do* want this to truncate.
   return endSample - startSample;
}

long GetIntervalEndSample(const AccessInterval& aInterval,
                          const UtCalendar&     aStartEpoch,
                          double                aCoverageStart,
                          double                aSampleDuration)
{
   // If the interval ends exactly at midnight, we want it to count as the day before.
   double startSecond = aStartEpoch.GetTime();
   long startSample = static_cast<long>(startSecond) / static_cast<long>(aSampleDuration); // I *do* want this to truncate.
   double endSecond = aInterval.GetEnd() - aCoverageStart + startSecond;
   long endSample = static_cast<long>(endSecond) / static_cast<long>(aSampleDuration); // I *do* want this to truncate.
   long ret       = endSample - startSample;
   double secondsSinceMidnight = endSecond - aSampleDuration * endSample;
   if (secondsSinceMidnight <= 1e-15)
   {
      ret -= 1;
   }
   return ret;
}

} // namespace MeasureUtils
} // namespace coverage
} // namespace wsf
