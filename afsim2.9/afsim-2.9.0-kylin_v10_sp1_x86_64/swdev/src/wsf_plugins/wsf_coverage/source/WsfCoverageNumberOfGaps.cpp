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

#include "WsfCoverageNumberOfGaps.hpp"

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

bool NumberOfGaps::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "subtype")
   {
      std::string subtype;
      aInput.ReadCommand(subtype);
      mSubType = GetSubtypeFromIdentifier(subtype);
      if (mSubType == SubType::cUNKNOWN)
      {
         std::stringstream msg{};
         msg << "Unknown subtype '" << subtype << "' for MOE of type '" << GetType() << "'.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
      switch (SubtypeHasParameter(mSubType))
      {
      case 1:
         // It must be per-time
         aInput.ReadValueOfType(mSampleSpan, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mSampleSpan, 0.0);
         if (mSubType != SubType::cMEAN_PER_TIME)
         {
            aInput.ValueLessOrEqual(UtCalendar::cSEC_IN_DAY % static_cast<long>(mSampleSpan), 0L);
         }
         break;
      case 2:
         // It must be in-span.
         aInput.ReadValueOfType(mMinParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMinParameter, 0.0);
         aInput.ReadValueOfType(mMaxParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMaxParameter, mMinParameter);
         break;
      case 3:
         // It must per in-span and per-time.
         aInput.ReadValueOfType(mMinParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMinParameter, 0.0);
         aInput.ReadValueOfType(mMaxParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMaxParameter, mMinParameter);
         aInput.ReadValueOfType(mSampleSpan, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mSampleSpan, 0.0);
         break;
      default: // No additional input needed, so do nothing.
         break;
      }
   }
   else
   {
      retval = Measure::ProcessInput(aInput);
   }
   return retval;
}

//! Return the default value for the measure if there were no accesses.
double NumberOfGaps::GetDefaultValue() const
{
   double retval{};
   switch (mSubType)
   {
   case SubType::cTOTAL:            // Intentional fallthrough
   case SubType::cIN_SPAN:          //    |
   case SubType::cMAXIMUM_PER_TIME: //    |
   case SubType::cMINIMUM_PER_TIME: //    |
   case SubType::cMEAN_PER_TIME:    //    |
   case SubType::cIN_SPAN_PER_TIME: //    v
      retval = 1.0;
      break;
   case SubType::cUNKNOWN:
   default:
      throw std::runtime_error{"Trying to get default value for unknown subtype in Number of Gaps"};
      break;
   }
   return retval;
}

double NumberOfGaps::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second;
   }
   return retval;
}

void NumberOfGaps::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

std::string NumberOfGaps::GetValueHeader() const
{
   std::string        retval{};
   std::ostringstream ss;
   switch (mSubType)
   {
   case SubType::cTOTAL:
      retval = "Total Gaps";
      break;
   case SubType::cIN_SPAN:
      retval = "Gaps in Span";
      break;
   case SubType::cMINIMUM_PER_TIME:
      retval = "Min. Gaps Per Sample";
      break;
   case SubType::cMAXIMUM_PER_TIME:
      retval = "Max. Gaps Per Sample";
      break;
   case SubType::cMEAN_PER_TIME:
      retval = "Mean Gaps Per Sample";
      break;
   case SubType::cIN_SPAN_PER_TIME:
      ss << "Gaps In Span " << mMinParameter << "-" << mMaxParameter << " Seconds Per Sample";
      retval = ss.str();
      break;
   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for Number of Gaps MOE"};
      break;
   }
   return retval;
}

bool NumberOfGaps::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&NumberOfGaps::OnAccessIntervalComplete, this));

   mStartEpoch = aCoverage.GetStartEpoch();
   return true;
}

void NumberOfGaps::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mIntervals)
   {
      std::vector<AccessInterval> gaps{};
      MeasureUtils::ComputeGaps(gaps, gaIter.second, mCoverageInterval, "Number of Gaps MoE", GetName());
      mData[gaIter.first] = ComputeValue(gaps);
   }
   mIntervals.clear();
}

//! Compute the measure's value given the gap intervals, @p aGaps.
double NumberOfGaps::ComputeValue(std::vector<AccessInterval>& aGaps) const
{
   double retval{0.0};
   if (!aGaps.empty())
   {
      switch (mSubType)
      {
      case SubType::cTOTAL:
         retval = ComputeTotal(aGaps);
         break;
      case SubType::cIN_SPAN:
         retval = ComputeInSpan(aGaps);
         break;
      case SubType::cMINIMUM_PER_TIME:
         retval = ComputeMin(aGaps);
         break;
      case SubType::cMAXIMUM_PER_TIME:
         retval = ComputeMax(aGaps);
         break;
      case SubType::cMEAN_PER_TIME:
         retval = ComputeMean(aGaps);
         break;
      case SubType::cIN_SPAN_PER_TIME:
         retval = ComputeSpanPerTime(aGaps);
         break;
      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in Number of Gaps."};
         break;
      }
   }
   return retval;
}

//! Compute the total number gaps in the entire coverage interval.
//! \param aGaps  A vector of AccessIntervals that represent gaps in coverage.
//! \return       The total number of gaps in the entire coverage interval.
double NumberOfGaps::ComputeTotal(const std::vector<AccessInterval>& aGaps) const
{
   return static_cast<double>(aGaps.size());
}

//! Compute the total number gaps in the entire coverage interval with a duration between mMinParameter and
//! mMaxParameter. \param aGaps  A vector of AccessIntervals that represent gaps in coverage. \return       The total
//! number of gaps in the entire coverage interval with a duration between mMinParameter and mMaxParameter.
double NumberOfGaps::ComputeInSpan(const std::vector<AccessInterval>& aGaps) const
{
   int ret = 0;
   for (auto& gap : aGaps)
   {
      // Static casting a bool to an int results in 0 (false) or 1 (true)
      ret += static_cast<int>(gap.GetDuration() >= mMinParameter && gap.GetDuration() <= mMaxParameter);
   }
   return static_cast<double>(ret);
}

//! Compute the maximum number gaps in a single calendar day.
//! \param aGaps  A vector of AccessIntervals that represent gaps in coverage.
//! \return       The maximum number gaps in a single calendar day.
double NumberOfGaps::ComputeMax(const std::vector<AccessInterval>& aGaps) const
{
   std::vector<int> samples{};
   ComputeSampleCount(aGaps, samples);

   auto maxOp = [](int aMinValue, int aDay) { return std::max<int>(aMinValue, aDay); };

   return static_cast<double>(std::accumulate(samples.begin(), samples.end(), std::numeric_limits<int>::lowest(), maxOp));
}

//! Compute the minimum number gaps in a single calendar day.
//! \param aGaps  A vector of AccessIntervals that represent gaps in coverage.
//! \return       The minimum number gaps in a single calendar day.
double NumberOfGaps::ComputeMin(const std::vector<AccessInterval>& aGaps) const
{
   std::vector<int> samples{};
   ComputeSampleCount(aGaps, samples);

   auto minOp = [](int aMinValue, int aDay) { return std::min<int>(aMinValue, aDay); };

   return static_cast<double>(std::accumulate(samples.begin(), samples.end(), std::numeric_limits<int>::max(), minOp));
}

//! Compute the mean number gaps per day within the coverage interval.
//! \param aGaps  A vector of AccessIntervals that represent gaps in coverage.
//! \return       The mean number gaps per day within the coverage interval.
double NumberOfGaps::ComputeMean(const std::vector<AccessInterval>& aGaps) const
{
   return static_cast<double>(ComputeTotal(aGaps)) * mSampleSpan / mCoverageInterval.GetDuration();
}

//! Compute the total number gaps in the entire coverage interval with a duration between mMinParameter and
//! mMaxParameter, divided by the number of days within the coverage interval. \param aGaps  A vector of AccessIntervals
//! that represent gaps in coverage. \return       The total number of gaps in the entire coverage interval with a
//! duration between mMinParameter and mMaxParameter,
//!               divided by the number of days within the coverage interval.
double NumberOfGaps::ComputeSpanPerTime(const std::vector<AccessInterval>& aGaps) const
{
   return static_cast<double>(ComputeInSpan(aGaps)) * mSampleSpan / mCoverageInterval.GetDuration();
}

//! Count how many gaps there are in each calendar day which overlaps the coverage interval. The
//! first day of the coverage interval is considered day 0. If a gap spans more than one day, then
//! it is counted for each day it spans. As a result, the sum of the content of aDays may be more
//! than the total number of gaps.
//! \param aGaps  A vector of AccessIntervals that represent gaps in coverage.
//! \param aDays  A vector of integers. This should be empty when passed, and will be filled
//!               as a side effect. Each index represents a calendar day, and its content represents the
//!               number of gaps in coverage on that day.
void NumberOfGaps::ComputeSampleCount(const std::vector<AccessInterval>& aGaps, std::vector<int>& aSamples) const
{
   aSamples.resize(MeasureUtils::GetIntervalEndSample(mCoverageInterval,
                                                      mStartEpoch,
                                                      mCoverageInterval.GetStart(),
                                                      mSampleSpan) +
                   1); // Make sure that the correct number of days are accounted for.
   for (auto& gap : aGaps)
   {
      auto startDay = MeasureUtils::GetIntervalStartSample(gap, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
      auto endDay   = MeasureUtils::GetIntervalEndSample(gap, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
      for (int i = startDay; i <= endDay; ++i)
      {
         ++aSamples.at(i);
      }
   }
}

//! Add a new interval to the internal interval container when the interval completes.
//! \param aGridAsset  The relevant grid asset.
//! \param aFreeAsset  Unused.
//! \param aInterval   A reference to the new interval.
void NumberOfGaps::OnAccessIntervalComplete(const GridAsset& aGridAsset,
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

//! Given a string identifier, return the appropriate SubType enum. If the string identifier
//! is unrecognized, then return cUNKNOWN.
NumberOfGaps::SubType NumberOfGaps::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "total")
   {
      retval = SubType::cTOTAL;
   }
   else if (aIdentifier == "in_span")
   {
      retval = SubType::cIN_SPAN;
   }
   else if (aIdentifier == "minimum_per")
   {
      retval = SubType::cMINIMUM_PER_TIME;
   }
   else if (aIdentifier == "maximum_per")
   {
      retval = SubType::cMAXIMUM_PER_TIME;
   }
   else if (aIdentifier == "mean_per")
   {
      retval = SubType::cMEAN_PER_TIME;
   }
   else if (aIdentifier == "in_span_per")
   {
      retval = SubType::cIN_SPAN_PER_TIME;
   }
   return retval;
}

int NumberOfGaps::SubtypeHasParameter(SubType aSubType)
{
   switch (aSubType)
   {
   case SubType::cTOTAL:
   case SubType::cUNKNOWN:
      return 0;
   case SubType::cMAXIMUM_PER_TIME:
   case SubType::cMINIMUM_PER_TIME:
   case SubType::cMEAN_PER_TIME:
      return 1;
   case SubType::cIN_SPAN:
      return 2;
   case SubType::cIN_SPAN_PER_TIME:
      return 3;
   default: // This should never happen.
      return 0;
   }
}

} // namespace coverage
} // namespace wsf
