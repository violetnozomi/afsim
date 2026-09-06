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

#include "WsfCoverageNumberOfAccesses.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfCoverage.hpp"
#include "WsfCoverageMeasureUtils.hpp"

namespace wsf
{
namespace coverage
{

bool NumberOfAccesses::ProcessInput(UtInput& aInput)
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
      case 1: // It must be per-sample
         aInput.ReadValueOfType(mSampleSpan, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mSampleSpan, 0.0);
         if (mSubType != SubType::cMEAN_PER_TIME)
         {
            aInput.ValueLessOrEqual(UtCalendar::cSEC_IN_DAY % static_cast<long>(mSampleSpan), 0L);
         }
         break;
      case 2: // It must be in-span
         aInput.ReadValueOfType(mMinParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMinParameter, 0.0);
         aInput.ReadValueOfType(mMaxParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMaxParameter, 0.0);
         if (mMaxParameter < mMinParameter)
         {
            std::swap(mMaxParameter, mMinParameter);
         }
         break;
      case 3:
         // It must per in-span and per-time.
         aInput.ReadValueOfType(mMinParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMinParameter, 0.0);
         aInput.ReadValueOfType(mMaxParameter, UtInput::ValueType::cTIME);
         aInput.ValueGreaterOrEqual(mMaxParameter, mMinParameter);
         if (mMaxParameter < mMinParameter)
         {
            std::swap(mMaxParameter, mMinParameter);
         }
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
double NumberOfAccesses::GetDefaultValue() const
{
   double retval{};
   switch (mSubType)
   {
   case SubType::cUNKNOWN:
      throw std::runtime_error{"Trying to get default value for unknown subtype in Number Of Accesses"};
      break;
   default:
      retval = 0.0;
      break;
   }
   return retval;
}

//! Return the measured value for the given @p aGridAsset.
double NumberOfAccesses::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second;
   }
   return retval;
}

void NumberOfAccesses::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

//! Return the text header for output files for this MOE.
std::string NumberOfAccesses::GetValueHeader() const
{
   std::string        retval{};
   std::ostringstream ss;
   switch (mSubType)
   {
   case SubType::cIN_SPAN:
      ss << "In Span " << mMinParameter << "-" << mMaxParameter << " Seconds";
      retval = ss.str();
      break;
   case SubType::cIN_SPAN_PER_TIME:
      ss << "In Span " << mMinParameter << "-" << mMaxParameter << " Seconds Per Sample";
      retval = ss.str();
      break;
   case SubType::cMAX_PER_TIME:
      retval = "Maximum Per Sample";
      break;
   case SubType::cMEAN_PER_TIME:
      retval = "Mean Per Sample";
      break;
   case SubType::cMIN_PER_TIME:
      retval = "Minimum Per Sample";
      break;
   case SubType::cTOTAL:
      retval = "Total";
      break;

   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for Number Of Accesses MOE"};
      break;
   }
   return retval;
}

bool NumberOfAccesses::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&NumberOfAccesses::OnAccessIntervalComplete, this));
   mStartEpoch = aCoverage.GetStartEpoch();
   return true;
}

void NumberOfAccesses::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mIntervals)
   {
      mData[gaIter.first] = ComputeValue(gaIter.second);
   }
   mIntervals.clear();
}

//! Compute the measure's value given the intervals, @p aIntervals.
double NumberOfAccesses::ComputeValue(std::vector<AccessInterval>& aIntervals) const
{
   double retval{0.0};
   if (aIntervals.size())
   {
      switch (mSubType)
      {
      case SubType::cIN_SPAN:
         retval = ComputeInSpan(aIntervals);
         break;
      case SubType::cIN_SPAN_PER_TIME:
         retval = ComputePerSampleInSpan(aIntervals);
         break;
      case SubType::cMAX_PER_TIME:
         retval = ComputePerSampleMax(aIntervals);
         break;
      case SubType::cMEAN_PER_TIME:
         retval = ComputePerSampleMean(aIntervals);
         break;
      case SubType::cMIN_PER_TIME:
         retval = ComputePerSampleMin(aIntervals);
         break;
      case SubType::cTOTAL:
         retval = ComputeTotal(aIntervals);
         break;
      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in Number Of Accesses."};
         break;
      }
   }
   return retval;
}

// Count how many intervals there are in each calendar day which overlaps the coverage interval. The
// first day of the coverage interval is considered day 0. If an interval spans more than one day, then
// it is counted for each day it spans. As a result, the sum of the content of aDays may be more
// than the total number of intervals.
// \param aIntervals  A vector of AccessIntervals that represent intervals in coverage.
// \param aDays       A vector of integers. This should be empty when passed, and will be filled
//                    as a side effect. Each index represents a calendar day, and its content represents the
//                    number of intervals in coverage on that day.
void NumberOfAccesses::ComputePerSampleCount(const std::vector<AccessInterval>& aIntervals, std::vector<int>& aDays) const
{
   aDays.resize(MeasureUtils::GetIntervalEndSample(mCoverageInterval,
                                                   mStartEpoch,
                                                   mCoverageInterval.GetStart(),
                                                   mSampleSpan) +
                1); // Make sure that the correct number of days are accounted for.
   for (auto& gap : aIntervals)
   {
      auto startDay = MeasureUtils::GetIntervalStartSample(gap, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
      auto endDay   = MeasureUtils::GetIntervalEndSample(gap, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
      for (int i = startDay; i <= endDay; ++i)
      {
         ++aDays.at(i);
      }
   }
}

// Compute the number of access intervals with a duration beween mMinPrameter and mMaxParameter.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputeInSpan(std::vector<AccessInterval>& aIntervals) const
{
   auto pred = [this](const AccessInterval& aInterval)
   { return (aInterval.GetDuration() <= this->GetMaxParameter() && aInterval.GetDuration() >= this->GetMinParameter()); };

   return static_cast<double>(std::count_if(aIntervals.begin(), aIntervals.end(), pred));
}

// Compute the number of access intervals with a duration beween mMinPrameter and mMaxParameter
// divided by the number of days in the coverage interval.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputePerSampleInSpan(std::vector<AccessInterval>& aIntervals) const
{
   return ComputeInSpan(aIntervals) * mSampleSpan / mCoverageInterval.GetDuration();
}

// Compute the maximum number of access intervals within a single calendar day.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputePerSampleMax(std::vector<AccessInterval>& aIntervals) const
{
   std::vector<int> days{};
   ComputePerSampleCount(aIntervals, days);

   auto maxOp = [](int aMinValue, int aDay) { return std::max<int>(aMinValue, aDay); };

   return static_cast<double>(std::accumulate(days.begin(), days.end(), std::numeric_limits<int>::lowest(), maxOp));
}

// Compute the average number of access intervals per day.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputePerSampleMean(std::vector<AccessInterval>& aIntervals) const
{
   return static_cast<double>(aIntervals.size()) * mSampleSpan / mCoverageInterval.GetDuration();
}

// Compute the minimum number of access intervals within a single calendar day.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputePerSampleMin(std::vector<AccessInterval>& aIntervals) const
{
   std::vector<int> days{};
   ComputePerSampleCount(aIntervals, days);

   auto minOp = [](int aMinValue, int aDay) { return std::min<int>(aMinValue, aDay); };

   return static_cast<double>(std::accumulate(days.begin(), days.end(), std::numeric_limits<int>::max(), minOp));
}

// Compute the total number of access intervals over the whole coverage interval.
// \param aIntervals  A reference to the list of access intervals.
// \return            The computed value.
double NumberOfAccesses::ComputeTotal(std::vector<AccessInterval>& aIntervals) const
{
   return static_cast<double>(aIntervals.size());
}


void NumberOfAccesses::OnAccessIntervalComplete(const GridAsset& aGridAsset,
                                                const FreeAsset& /*aFreeAsset*/,
                                                const AccessInterval& aInterval)
{
   auto gaIter = mIntervals.find(aGridAsset);
   if (gaIter != mIntervals.end())
   {
      // There has been an interval for aGridAsset before now.
      gaIter->second.push_back(aInterval);
   }
   else
   {
      // This is the first interval, so just add it to the interval map.
      mIntervals.emplace(std::make_pair(aGridAsset, std::vector<AccessInterval>{aInterval}));
   }
}

NumberOfAccesses::SubType NumberOfAccesses::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "in_span")
   {
      retval = SubType::cIN_SPAN;
   }
   else if (aIdentifier == "in_span_per")
   {
      retval = SubType::cIN_SPAN_PER_TIME;
   }
   else if (aIdentifier == "maximum_per")
   {
      retval = SubType::cMAX_PER_TIME;
   }
   else if (aIdentifier == "mean_per")
   {
      retval = SubType::cMEAN_PER_TIME;
   }
   else if (aIdentifier == "minimum_per")
   {
      retval = SubType::cMIN_PER_TIME;
   }
   else if (aIdentifier == "total")
   {
      retval = SubType::cTOTAL;
   }

   return retval;
}

int NumberOfAccesses::SubtypeHasParameter(SubType aSubType)
{
   switch (aSubType)
   {
   case SubType::cMAX_PER_TIME:
   case SubType::cMEAN_PER_TIME:
   case SubType::cMIN_PER_TIME:
      return 1;
   case SubType::cIN_SPAN:
      return 2;
   case SubType::cIN_SPAN_PER_TIME:
      return 3;
   default:
      return 0;
   }
}

} // namespace coverage
} // namespace wsf
