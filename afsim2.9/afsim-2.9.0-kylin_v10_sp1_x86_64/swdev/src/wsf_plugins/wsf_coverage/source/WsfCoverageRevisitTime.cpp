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

#include "WsfCoverageRevisitTime.hpp"

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

bool RevisitTime::ProcessInput(UtInput& aInput)
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
      if (SubtypeHasParameter(mSubType))
      {
         aInput.ReadValue(mParameter);
         aInput.ValueGreater(mParameter, 0.0);
         aInput.ValueLess(mParameter, 100.0);
         mParameter /= 100.0;
      }
   }
   else
   {
      retval = Measure::ProcessInput(aInput);
   }
   return retval;
}

//! Return the measured value for the given @p aGridAsset.
double RevisitTime::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second;
   }
   return retval;
}

//! Return the default value for the measure if there were no accesses.
double RevisitTime::GetDefaultValue() const
{
   double retval{};
   switch (mSubType)
   {
   case SubType::cMEAN:    // Intentional fallthrough
   case SubType::cMINIMUM: //    |
   case SubType::cMAXIMUM: //    v
      retval = mCoverageInterval.GetDuration();
      break;
   case SubType::cSTD_DEV:
      retval = 0.0;
      break;
   case SubType::cNUM_PERCENT_BELOW:       // Intentional fallthrough
   case SubType::cPERCENT_BELOW:           //    |
   case SubType::cPERCENT_BELOW_GAPS_ONLY: //    v
      retval = mCoverageInterval.GetDuration();
      break;
   case SubType::cUNKNOWN:
   default:
      throw std::runtime_error{"Trying to get default value for unknown subtype in Revisit Time"};
      break;
   }
   return retval;
}

void RevisitTime::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

//! Return the text header for output files for this MOE.
std::string RevisitTime::GetValueHeader() const
{
   std::string retval{};
   switch (mSubType)
   {
   case SubType::cMEAN:
      retval = "Mean Gap Time [s]";
      break;
   case SubType::cMINIMUM:
      retval = "Min. Gap Time [s]";
      break;
   case SubType::cMAXIMUM:
      retval = "Max. Gap Time [s]";
      break;
   case SubType::cSTD_DEV:
      retval = "Gap Std. Dev. [s]";
      break;
   case SubType::cNUM_PERCENT_BELOW:
      retval = "Num. % Below [s]";
      break;
   case SubType::cPERCENT_BELOW:
      retval = "% Below [s]";
      break;
   case SubType::cPERCENT_BELOW_GAPS_ONLY:
      retval = "% Below (gaps) [s]";
      break;
   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for Revisit Time MOE"};
      break;
   }
   return retval;
}

bool RevisitTime::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&RevisitTime::OnAccessIntervalComplete, this));
   return true;
}

void RevisitTime::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mIntervals)
   {
      std::vector<AccessInterval> gaps;
      MeasureUtils::ComputeGaps(gaps, gaIter.second, mCoverageInterval, "Revisit Time MOE", GetName());
      mData[gaIter.first] = ComputeValue(gaps);
   }
   mIntervals.clear();
}

//! Compute the measure's value given the gap intervals, @p aGaps.
double RevisitTime::ComputeValue(std::vector<AccessInterval>& aGaps) const
{
   double retval{0.0};
   if (aGaps.size())
   {
      switch (mSubType)
      {
      case SubType::cMEAN:
         retval = ComputeMean(aGaps);
         break;
      case SubType::cMINIMUM:
         retval = ComputeMin(aGaps);
         break;
      case SubType::cMAXIMUM:
         retval = ComputeMax(aGaps);
         break;
      case SubType::cSTD_DEV:
         retval = ComputeStdDev(aGaps);
         break;
      case SubType::cNUM_PERCENT_BELOW:
         retval = ComputeNumPercentBelow(aGaps);
         break;
      case SubType::cPERCENT_BELOW:
         retval = ComputePercentBelow(aGaps);
         break;
      case SubType::cPERCENT_BELOW_GAPS_ONLY:
         retval = ComputePercentBelowGapsOnly(aGaps);
         break;
      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in Revisit Time."};
         break;
      }
   }
   return retval;
}

//! Compute the mean gap time.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The mean duration of the provided gap intervals.
double RevisitTime::ComputeMean(const std::vector<AccessInterval>& aGaps) const
{
   auto   op     = [](double aAccum, const AccessInterval& aInterval) { return aAccum + aInterval.GetDuration(); };
   double retval = std::accumulate(aGaps.begin(), aGaps.end(), 0.0, op);
   retval /= aGaps.size();
   return retval;
}

//! Compute the minimum gap time.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The minimum duration of the provided gap intervals.
double RevisitTime::ComputeMin(const std::vector<AccessInterval>& aGaps) const
{
   auto op = [](double aMinValue, const AccessInterval& aInterval)
   { return std::min(aMinValue, aInterval.GetDuration()); };
   return std::accumulate(aGaps.begin(), aGaps.end(), std::numeric_limits<double>::max(), op);
}

//! Compute the maximum gap time.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The maximum duration of the provided gap intervals.
double RevisitTime::ComputeMax(const std::vector<AccessInterval>& aGaps) const
{
   auto op = [](double aMaxValue, const AccessInterval& aInterval)
   { return std::max(aMaxValue, aInterval.GetDuration()); };
   return std::accumulate(aGaps.begin(), aGaps.end(), 0.0, op);
}

//! Compute the standard deviation of the gap times.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The standard deviation of the provided gap intervals' duration.
double RevisitTime::ComputeStdDev(const std::vector<AccessInterval>& aGaps) const
{
   double x{0.0};
   double x2{0.0};
   for (auto& gap : aGaps)
   {
      double duration = gap.GetDuration();
      x += duration;
      x2 += duration * duration;
   }
   x /= aGaps.size();
   x2 /= aGaps.size();
   return sqrt(x2 - x * x);
}

//! Compute the number below percentage gap time.
//!
//! This method computes the duration of the gap for which X percent of the gaps
//! have a shorter duration, where X is the subtype parameter.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The duration that X percent of the gaps are shorter than.
double RevisitTime::ComputeNumPercentBelow(std::vector<AccessInterval>& aGaps) const
{
   SortGapsByDuration(aGaps);
   int index = static_cast<int>(mParameter * aGaps.size());
   return aGaps[index].GetDuration();
}

//! Compute the below percentage gap time.
//!
//! If X is the subtype parameter input by the user, then this will return the
//! gap time such that X percent of the time in the coverage interval, the grid
//! point will have a gap time less than the returned value.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The measured value.
double RevisitTime::ComputePercentBelow(std::vector<AccessInterval>& aGaps) const
{
   double totalDuration = mCoverageInterval.GetDuration();
   double offset{totalDuration};
   for (auto& gap : aGaps)
   {
      offset -= gap.GetDuration();
   }
   offset /= totalDuration;
   return ComputePercentageOffset(aGaps, offset);
}

//! Compute the below percentage gaps only gap time.
//!
//! If X is the subtype parameter input by the user, then this will return the
//! gap time such that X percent of the time in the gaps during the coverage interval,
//! the grid point will have a gap time less than the returned value.
//!
//! This method requires that @p aGaps is not empty. See ComputeValue.
//!
//! \param aGaps - The gap intervals.
//! \returns     - The measured value.
double RevisitTime::ComputePercentBelowGapsOnly(std::vector<AccessInterval>& aGaps) const
{
   return ComputePercentageOffset(aGaps, 0.0);
}

double RevisitTime::ComputePercentageOffset(std::vector<AccessInterval>& aGaps, double aOffset) const
{
   SortGapsByDuration(aGaps);

   double retval{0.0};
   double totalDuration = mCoverageInterval.GetDuration();
   double cumulative    = aGaps[0].GetDuration() / totalDuration + aOffset;
   if (cumulative <= mParameter)
   {
      double previousGap{aGaps[0].GetDuration()};
      for (std::size_t i = 1; i < aGaps.size(); ++i)
      {
         double current = aGaps[i].GetDuration() / totalDuration;
         if (cumulative + current > mParameter)
         {
            retval = previousGap;
            break;
         }
         cumulative += current;
         previousGap = aGaps[i].GetDuration();
      }
   }

   return retval;
}

void RevisitTime::SortGapsByDuration(std::vector<AccessInterval>& aGaps) const
{
   auto byDuration = [](const AccessInterval& aLHS, const AccessInterval& aRHS)
   { return aLHS.GetDuration() < aRHS.GetDuration(); };
   std::sort(aGaps.begin(), aGaps.end(), byDuration);
}

void RevisitTime::OnAccessIntervalComplete(const GridAsset& aGridAsset,
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

RevisitTime::SubType RevisitTime::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "mean")
   {
      retval = SubType::cMEAN;
   }
   else if (aIdentifier == "minimum")
   {
      retval = SubType::cMINIMUM;
   }
   else if (aIdentifier == "maximum")
   {
      retval = SubType::cMAXIMUM;
   }
   else if (aIdentifier == "standard_deviation")
   {
      retval = SubType::cSTD_DEV;
   }
   else if (aIdentifier == "number_percent_below")
   {
      retval = SubType::cNUM_PERCENT_BELOW;
   }
   else if (aIdentifier == "percent_below")
   {
      retval = SubType::cPERCENT_BELOW;
   }
   else if (aIdentifier == "percent_below_gaps_only")
   {
      retval = SubType::cPERCENT_BELOW_GAPS_ONLY;
   }
   return retval;
}

bool RevisitTime::SubtypeHasParameter(SubType aSubType)
{
   return (aSubType == SubType::cNUM_PERCENT_BELOW || aSubType == SubType::cPERCENT_BELOW ||
           aSubType == SubType::cPERCENT_BELOW_GAPS_ONLY);
}

} // namespace coverage
} // namespace wsf
