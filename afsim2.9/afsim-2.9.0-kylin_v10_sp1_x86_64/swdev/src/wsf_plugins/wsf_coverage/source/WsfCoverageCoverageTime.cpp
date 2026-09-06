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

#include "WsfCoverageCoverageTime.hpp"

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

//! Accumulate interval durations. Useful as the predicate for std::accumulate.
//!
//! \param aSum       The accumulated sum up to this point.
//! \param aInterval  The next interval to add.
//! \return           The new sum.
static double AccumulateIntervalDurations(double aSum, const AccessInterval& aInterval)
{
   return aSum + aInterval.GetDuration();
}

// Return the default value for the measure if there were no accesses.
double CoverageTime::GetDefaultValue() const
{
   double retval{0.0};
   switch (mSubType)
   {
   case SubType::cTOTAL: // Intentional fallthrough
   case SubType::cPERCENT:
   case SubType::cTOTAL_TIME_ABOVE:
   case SubType::cPERCENT_TIME_ABOVE:
   case SubType::cMAX_PER_TIME:
   case SubType::cMAX_PERCENT_PER_TIME:
   case SubType::cMIN_PER_TIME:
   case SubType::cMIN_PERCENT_PER_TIME:
   case SubType::cMEAN_PER_TIME:
   case SubType::cMEAN_PERCENT_PER_TIME:
   case SubType::cSTD_DEV_PER_TIME:
   case SubType::cSTD_DEV_PERCENT_PER_TIME:
      break;
   case SubType::cUNKNOWN:
   default:
      throw std::runtime_error{"Trying to get default value for unknown subtype in Coverage Time"};
      break;
   }
   return retval;
}

bool CoverageTime::ProcessInput(UtInput& aInput)
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
      case ParamType::cTIME_AFTER:
         aInput.ReadValue(mParameter);
         aInput.ValueGreater(mParameter, 0);
         break;
      case ParamType::cPER_TIME:
         aInput.ReadValueOfType(mSampleSpan, UtInput::ValueType::cTIME);
         aInput.ValueGreater(mSampleSpan, 0.0);
         break;
      case ParamType::cPER_TIME_LIMITED:
         aInput.ReadValueOfType(mSampleSpan, UtInput::ValueType::cTIME);
         aInput.ValueGreater(mSampleSpan, 0.0);
         if (mSubType != SubType::cMEAN_PER_TIME)
         {
            aInput.ValueLessOrEqual(UtCalendar::cSEC_IN_DAY % static_cast<long>(mSampleSpan), 0L);
         }
         break;
      default:
         break;
      }
   }
   else
   {
      retval = Measure::ProcessInput(aInput);
   }
   return retval;
}

// Return the measured value for the given GridAsset.
// \param aGridAsset  The grid asset (essentially, point on grid) for which we want the measured value.
// \return            The measured value from that grid asset.
double CoverageTime::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second.mData;
   }
   return retval;
}

// Callback for when data collection starts for the given Coverage.
// \param aCoverage  The coverage for which data collection is starting. This is the 'owning' Coverage object.
// \param aSimTime   The simulation time as a double.
void CoverageTime::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

// Return the text header for output files for this MOE.
std::string CoverageTime::GetValueHeader() const
{
   std::string retval{};
   switch (mSubType)
   {
   case SubType::cTOTAL:
      retval = "Total Coverage Time [s]";
      break;
   case SubType::cPERCENT:
      retval = "% Coverage Time";
      break;
   case SubType::cTOTAL_TIME_ABOVE:
      retval = "Total Coverage Above Time [s]";
      break;
   case SubType::cPERCENT_TIME_ABOVE:
      retval = "% Coverage Above Time";
      break;
   case SubType::cMAX_PER_TIME:
      retval = "Max Per Sample [s]";
      break;
   case SubType::cMAX_PERCENT_PER_TIME:
      retval = "% Max Per Sample";
      break;
   case SubType::cMIN_PER_TIME:
      retval = "Min Per Sample [s]";
      break;
   case SubType::cMIN_PERCENT_PER_TIME:
      retval = "% Min Per Sample";
      break;
   case SubType::cMEAN_PER_TIME:
      retval = "Mean Per Sample [s]";
      break;
   case SubType::cMEAN_PERCENT_PER_TIME:
      retval = "% Mean Per Sample";
      break;
   case SubType::cSTD_DEV_PER_TIME:
      retval = "Standard Deviation Per Sample";
      break;
   case SubType::cSTD_DEV_PERCENT_PER_TIME:
      retval = "% Standard Deviation Per Sample";
      break;
   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for Coverage Time MOE"};
      break;
   }
   return retval;
}

// Initialize this CoverageTime with the coverage that it will use for measurement.
// \param aCoverage  The coverage that this MoE will use for measurement.
// \return           True if initialization succeeded, false otherwise.
bool CoverageTime::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&CoverageTime::OnAccessIntervalComplete, this));

   mStartEpoch = aCoverage.GetStartEpoch();
   return true;
}

// Callback for when the data colection finishes.
// \param aCoverage  The coverage that us finishing its data collection.
// \param aSimTime   The simulation time as a double.
void CoverageTime::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& interIter : mData)
   {
      for (auto& layer : interIter.second.mIntervals)
      {
         layer.resize(
            MeasureUtils::GetIntervalEndSample(mCoverageInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan) +
            1);
      }
      // Note that the layer is decided right here, meaning that hypothetically, any of the methods
      // used by ComputeValue can be done for any layer (though we only do total and percent that way).
      interIter.second.mData = ComputeValue(interIter.second.mIntervals.at(mParameter - 1));
      interIter.second.mIntervals.clear();
   }
}

// Compute the measure's value given the intervals.
// \param aIntervals  This vector of vectors of AccessIntervals represents a single 'layer.'
double CoverageTime::ComputeValue(std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   double retval{0.0};
   if (!aIntervals.empty())
   {
      switch (mSubType)
      {
      case SubType::cTOTAL:
         retval = ComputeTotal(aIntervals);
         break;
      case SubType::cPERCENT:
         retval = ComputePercent(aIntervals);
         break;
      case SubType::cTOTAL_TIME_ABOVE:
         retval = ComputeTotal(aIntervals);
         break;
      case SubType::cPERCENT_TIME_ABOVE:
         retval = ComputePercent(aIntervals);
         break;
      case SubType::cMAX_PER_TIME:
         retval = ComputeMaxPerTime(aIntervals);
         break;
      case SubType::cMAX_PERCENT_PER_TIME:
         retval = ComputeMaxPercentPerTime(aIntervals);
         break;
      case SubType::cMIN_PER_TIME:
         retval = ComputeMinPerTime(aIntervals);
         break;
      case SubType::cMIN_PERCENT_PER_TIME:
         retval = ComputeMinPercentPerTime(aIntervals);
         break;
      case SubType::cMEAN_PER_TIME:
         retval = ComputeMeanPerTime(aIntervals);
         break;
      case SubType::cMEAN_PERCENT_PER_TIME:
         retval = ComputeMeanPercentPerTime(aIntervals);
         break;
      case SubType::cSTD_DEV_PER_TIME:
         retval = ComputeStdDevPerTime(aIntervals);
         break;
      case SubType::cSTD_DEV_PERCENT_PER_TIME:
         retval = ComputeStdDevPercentPerTime(aIntervals);
         break;
      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in Coverage Time."};
         break;
      }
   }
   return retval;
}

// Find the total sum of all AccessIntervals for the given layer.
// \param aIntervals  The layer of intervals to sum up.
// \return            The sum of all AccessIntervals in aIntervals.
double CoverageTime::ComputeTotal(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   double ret{0.0};
   for (auto& day : aIntervals)
   {
      ret = std::accumulate(day.begin(), day.end(), ret, &AccumulateIntervalDurations);
   }
   return ret;
}

// Find the total sum of all AccessIntervals for the given layer as a percentage of the overall coverage interval.
// \param aIntervals  The layer of intervals to sum up.
// \return            The sum of all AccessIntervals in aIntervals as a percentage of the overall coverage interval.
double CoverageTime::ComputePercent(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto subTotal = ComputeTotal(aIntervals);
   return 100.0 * subTotal / mCoverageInterval.GetDuration();
}

// Find the maximum amount of coverage time within a single day. A day is midnight to midnight UTC.
// \param aIntervals  The layer of intervals to evaluate.
// \return            The maximum amount of coverage time within a single day.
double CoverageTime::ComputeMaxPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto maxOp = [](double aMinValue, const std::vector<AccessInterval>& aDay)
   { return std::max<double>(aMinValue, std::accumulate(aDay.begin(), aDay.end(), 0.0, &AccumulateIntervalDurations)); };

   return std::accumulate(aIntervals.begin(), aIntervals.end(), std::numeric_limits<double>::lowest(), maxOp);
}

// Find the maximum amount of coverage time within a single day, as a percentage of 24 hours. A day is midnight to
// midnight UTC. \param aIntervals  The layer of intervals to evaluate. \return            The maximum amount of
// coverage time within a single day, as a percentage of 24 hours.
double CoverageTime::ComputeMaxPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto subTotal = ComputeMaxPerTime(aIntervals);
   return 100 * subTotal / mSampleSpan;
}

// Find the minimum amount of coverage time within a single day. A day is midnight to midnight UTC.
// \param aIntervals  The layer of intervals to evaluate.
// \return            The minimum amount of coverage time within a single day.
double CoverageTime::ComputeMinPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto minOp = [](double aMinValue, const std::vector<AccessInterval>& aDay)
   { return std::min<double>(aMinValue, std::accumulate(aDay.begin(), aDay.end(), 0.0, &AccumulateIntervalDurations)); };

   return std::accumulate(aIntervals.begin(), aIntervals.end(), std::numeric_limits<double>::max(), minOp);
}

// Find the minimum amount of coverage time within a single day, as a percentage of 24 hours. A day is midnight to
// midnight UTC. \param aIntervals  The layer of intervals to evaluate. \return            The minimum amount of
// coverage time within a single day, as a percentage of 24 hours.
double CoverageTime::ComputeMinPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto subTotal = ComputeMinPerTime(aIntervals);
   return 100 * subTotal / mSampleSpan;
}

// Find the mean amount of coverage time within a single day. A day is midnight to midnight UTC.
// \param aIntervals  The layer of intervals to evaluate.
// \return            The mean amount of coverage time within a single day.
double CoverageTime::ComputeMeanPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto subTotal = ComputeTotal(aIntervals);
   return subTotal / (mCoverageInterval.GetDuration() / mSampleSpan);
}

// Find the mean amount of coverage time within a single day, as a percentage of the whole coverage
// interval. A day is midnight to midnight UTC.
// \param aIntervals  The layer of intervals to evaluate.
// \return            The mean amount of coverage time within a single day, as a percentage of 24 hours.
double CoverageTime::ComputeMeanPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   auto subTotal = ComputePercent(aIntervals);
   return subTotal / (mCoverageInterval.GetDuration() / mSampleSpan);
}

// Find the standard deviation of coverage time per day during the interval. A day is midnight to midnight UTC.
// \param aIntervals  The layer of intervals to evaluate.
// \return            The standard deviation of coverage time per day during the interval.
double CoverageTime::ComputeStdDevPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   double x{0.0};
   double x2{0.0};
   for (auto& day : aIntervals)
   {
      double duration = std::accumulate(day.begin(), day.end(), 0.0, &AccumulateIntervalDurations);
      x += duration;
      x2 += duration * duration;
   }
   x /= MeasureUtils::GetIntervalEndSample(mCoverageInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan) + 1;
   x2 /= MeasureUtils::GetIntervalEndSample(mCoverageInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan) + 1;
   return sqrt(x2 - x * x);
}

// Find the standard deviation of coverage time per day during the interval, as a percentage of 24 hours. A day is
// midnight to midnight UTC. \param aIntervals  The layer of intervals to evaluate. \return            The standard
// deviation of coverage time per day during the interval, as a percentage of 24 hours.
double CoverageTime::ComputeStdDevPercentPerTime(const std::vector<std::vector<AccessInterval>>& aIntervals) const
{
   double subTotal = ComputeStdDevPerTime(aIntervals);
   return 100 * subTotal / mSampleSpan;
}

// Get midnight for the given sample in seconds since the start of the coverage interval.
// \param aDay  The day relative to the start of the coverage interval. The day on which the coverage interval starts is
// day 0. \return      The double representing the simtime at which midnight occurs for the given day.
double CoverageTime::GetMidnightForSample(int aSample) const
{
   return mCoverageInterval.GetStart() - mStartEpoch.GetTime() + mSampleSpan * aSample;
}

// Callback for when collection of an AccessInterval completes.
// \param aGridAsset  The grid asset associated with the AccessInterval.
// \param aFreeAsset  The free asset associated with the AccessInterval. Unused.
// \param aInterval   The completed interval.
void CoverageTime::OnAccessIntervalComplete(const GridAsset& aGridAsset,
                                            const FreeAsset& /*aFreeAsset*/,
                                            const AccessInterval& aInterval)
{
   auto gaIter = mData.find(aGridAsset);
   if (gaIter == mData.end())
   {
      mData.emplace(std::make_pair(aGridAsset, GridData{}));
   }
   Collapse(aGridAsset, aInterval, 0);
}

// Recursively merges intervals into a single layer, and adds the intersections of merged intervals to the next layer.
// \param aGridAsset  The grid asset for which this interval is relevant.
// \param aInterval   The new AccessInterval.
// \param aLevel      The layer to which aInterval will be applied.
void CoverageTime::Collapse(const GridAsset& aGridAsset, const AccessInterval& aInterval, int aLevel)
{
   if (aInterval.GetDuration() <= 0)
   {
      return; // No point adding an interval with 0 length.
   }

   // Make sure that there are enough layers and days to store the required data.
   if (aLevel == 0)
   {
      mData.at(aGridAsset).mIntervals.resize(mParameter);
      for (auto& layer : mData.at(aGridAsset).mIntervals)
      {
         layer.resize(
            MeasureUtils::GetIntervalEndSample(aInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan) + 1);
      }
   }

   // Partition intervals for aGridAsset based on overlap with aInterval.
   auto pred = [&aInterval](const AccessInterval& aElement) { return !aInterval.HasOverlapWith(aElement); };

   int startDay = MeasureUtils::GetIntervalStartSample(aInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
   int endDay   = MeasureUtils::GetIntervalEndSample(aInterval, mStartEpoch, mCoverageInterval.GetStart(), mSampleSpan);
   if (startDay != endDay)
   {
      // We need to split it into two intervals.
      // We just recurse with the two intervals.
      double midnight = GetMidnightForSample(endDay);
      Collapse(aGridAsset, AccessInterval(aInterval.GetStart(), midnight), aLevel);
      Collapse(aGridAsset, AccessInterval(midnight, aInterval.GetEnd()), aLevel);
      return;
   }

   auto& layer = mData.at(aGridAsset).mIntervals.at(aLevel).at(startDay);
   auto  start = std::partition(layer.begin(), layer.end(), pred);

   if (start == layer.end())
   {
      // No overlap with any interval, so just append it to the vector.
      layer.push_back(aInterval);
   }
   else
   {
      // Some intervals overlap, so accumulate via AccessInterval::UnionWith
      auto           op = [](const AccessInterval& aLHS, const AccessInterval& aRHS) { return aLHS.UnionWith(aRHS); };
      AccessInterval combined = std::accumulate(start, layer.end(), aInterval, op);

      if (aLevel < mParameter - 1)
      {
         // No need to do all this if we're not going to use it.
         for (auto unit = start; unit != layer.end(); ++unit)
         {
            Collapse(aGridAsset, aInterval.IntersectionWith(*unit), aLevel + 1);
         }
      }

      // Set the first value of the partitioned range to the accumulated value
      *start = combined;

      // Erase the rest of the intervals which had overlap, as their range is now
      // included in the element at 'start'.
      ++start;
      layer.erase(start, layer.end());
   }
}

// Takes the string identifier from input and determines the appropriate sub-type.
CoverageTime::SubType CoverageTime::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "total")
   {
      retval = SubType::cTOTAL;
   }
   else if (aIdentifier == "percent")
   {
      retval = SubType::cPERCENT;
   }
   else if (aIdentifier == "total_time_above")
   {
      retval = SubType::cTOTAL_TIME_ABOVE;
   }
   else if (aIdentifier == "percent_time_above")
   {
      retval = SubType::cPERCENT_TIME_ABOVE;
   }
   else if (aIdentifier == "maximum_per")
   {
      retval = SubType::cMAX_PER_TIME;
   }
   else if (aIdentifier == "maximum_percent_per")
   {
      retval = SubType::cMAX_PERCENT_PER_TIME;
   }
   else if (aIdentifier == "minimum_per")
   {
      retval = SubType::cMIN_PER_TIME;
   }
   else if (aIdentifier == "minimum_percent_per")
   {
      retval = SubType::cMIN_PERCENT_PER_TIME;
   }
   else if (aIdentifier == "mean_per")
   {
      retval = SubType::cMEAN_PER_TIME;
   }
   else if (aIdentifier == "mean_percent_per")
   {
      retval = SubType::cMEAN_PERCENT_PER_TIME;
   }
   else if (aIdentifier == "standard_deviation_per")
   {
      retval = SubType::cSTD_DEV_PER_TIME;
   }
   else if (aIdentifier == "standard_deviation_percent_per")
   {
      retval = SubType::cSTD_DEV_PERCENT_PER_TIME;
   }
   return retval;
}

// Returns true if the subtype requires a parameter.
CoverageTime::ParamType CoverageTime::SubtypeHasParameter(SubType aSubType)
{
   switch (aSubType)
   {
   case SubType::cMAX_PER_TIME:             // Intentional fall-through
   case SubType::cMAX_PERCENT_PER_TIME:     // |
   case SubType::cMIN_PER_TIME:             // |
   case SubType::cMIN_PERCENT_PER_TIME:     // |
   case SubType::cSTD_DEV_PER_TIME:         // |
   case SubType::cSTD_DEV_PERCENT_PER_TIME: // v
      return ParamType::cPER_TIME_LIMITED;
   case SubType::cMEAN_PER_TIME: // Intentional fall-through
   case SubType::cMEAN_PERCENT_PER_TIME:
      return ParamType::cPER_TIME;
   case SubType::cTOTAL_TIME_ABOVE: // Intentional fall-through
   case SubType::cPERCENT_TIME_ABOVE:
      return ParamType::cTIME_AFTER;
   default:
      return ParamType::cNONE;
   }
}

} // namespace coverage
} // namespace wsf
