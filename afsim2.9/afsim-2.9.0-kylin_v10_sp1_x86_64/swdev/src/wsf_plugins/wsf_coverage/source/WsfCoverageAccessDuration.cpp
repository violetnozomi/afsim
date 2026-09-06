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

#include "WsfCoverageAccessDuration.hpp"

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

namespace wsf
{
namespace coverage
{

bool AccessDuration::ProcessInput(UtInput& aInput)
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
         aInput.ValueGreaterOrEqual(mParameter, 0.0);
         aInput.ValueLessOrEqual(mParameter, 100.0);
         mParameter /= 100.0;
      }
   }
   else
   {
      retval = Measure::ProcessInput(aInput);
   }
   return retval;
}

//! Return the default value for the measure if there were no accesses.
double AccessDuration::GetDefaultValue() const
{
   double retval{};
   switch (mSubType)
   {
   case SubType::cUNKNOWN:
      throw std::runtime_error{"Trying to get default value for unknown subtype in Access Duration"};
      break;
   default:
      retval = 0.0;
      break;
   }
   return retval;
}

//! Return the measured value for the given @p aGridAsset.
double AccessDuration::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second;
   }
   return retval;
}

void AccessDuration::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

//! Return the text header for output files for this MOE.
std::string AccessDuration::GetValueHeader() const
{
   std::string        retval{};
   std::ostringstream percentString;
   switch (mSubType)
   {
   case SubType::cMEAN:
      retval = std::string("Mean [s]");
      break;
   case SubType::cMIN:
      retval = std::string("Min [s]");
      break;
   case SubType::cMAX:
      retval = std::string("Max [s]");
      break;
   case SubType::cPERCENT_ABOVE:
      percentString << (mParameter * 100.0) << "% Above [s]";
      retval = percentString.str();
      break;
   case SubType::cSTDDEV:
      retval = std::string("Stddev [s]");
      break;
   case SubType::cSUM:
      retval = std::string("Sum [s]");
      break;
   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for Access Duration MOE"};
      break;
   }
   return retval;
}

bool AccessDuration::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&AccessDuration::OnAccessIntervalComplete, this));
   return true;
}

void AccessDuration::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mIntervals)
   {
      mData[gaIter.first] = ComputeValue(gaIter.second);
   }
   mIntervals.clear();
}

//! Compute the measure's value given the intervals, @p aIntervals.
double AccessDuration::ComputeValue(std::vector<AccessInterval>& aIntervals) const
{
   double retval{0.0};
   if (aIntervals.size())
   {
      switch (mSubType)
      {
      case SubType::cMEAN:
         retval = ComputeMean(aIntervals);
         break;
      case SubType::cMIN:
         retval = ComputeMin(aIntervals);
         break;
      case SubType::cMAX:
         retval = ComputeMax(aIntervals);
         break;
      case SubType::cPERCENT_ABOVE:
         retval = ComputePercentAbove(aIntervals);
         break;
      case SubType::cSTDDEV:
         retval = ComputeStddev(aIntervals);
         break;
      case SubType::cSUM:
         retval = ComputeSum(aIntervals);
         break;

      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in Access Duration."};
         break;
      }
   }
   return retval;
}

// Compute the mean of all access interval durations.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The mean access interval duration.
double AccessDuration::ComputeMean(std::vector<AccessInterval>& aIntervals) const
{
   return ComputeSum(aIntervals) / aIntervals.size();
}

// Compute the minimum of all access interval durations.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The minimum access interval duration.
double AccessDuration::ComputeMin(std::vector<AccessInterval>& aIntervals) const
{
   auto minOp = [](double aSum, const AccessInterval& aInterval) { return std::min(aSum, aInterval.GetDuration()); };

   return std::accumulate(aIntervals.begin(), aIntervals.end(), std::numeric_limits<double>::max(), minOp);
}

// Compute the max of all access interval durations.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The maximum access interval duration.
double AccessDuration::ComputeMax(std::vector<AccessInterval>& aIntervals) const
{
   auto maxOp = [](double aSum, const AccessInterval& aInterval) { return std::max(aSum, aInterval.GetDuration()); };

   return std::accumulate(aIntervals.begin(), aIntervals.end(), std::numeric_limits<double>::lowest(), maxOp);
}

// If X is the subtype parameter input by the user, then this will return the
// access interval duration such that X percent of the time in the coverage
// interval, the grid point will be experiencing a coverage interval with a
// duration greater than or equal to the returned value.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The computed access interval duration.
double AccessDuration::ComputePercentAbove(std::vector<AccessInterval>& aIntervals) const
{
   SortIntervalsByDuration(aIntervals);
   std::vector<AccessInterval> totalCoverage;

   auto combineOp = [](const AccessInterval& aLHS, const AccessInterval& aRHS) { return aLHS.UnionWith(aRHS); };

   for (auto& interval : aIntervals)
   {
      auto overlapOp = [&interval](const AccessInterval& aInterval) { return !aInterval.HasOverlapWith(interval); };

      auto part = std::partition(totalCoverage.begin(), totalCoverage.end(), overlapOp);
      if (part != totalCoverage.end())
      {
         // Some intervals overlap, so accumulate via AccessInterval::UnionWith
         AccessInterval combined = std::accumulate(part, totalCoverage.end(), interval, combineOp);

         // Set the first value of the partitioned range to the accumulated value
         *part = combined;

         // Erase the rest of the intervals which had overlap, as their range is now
         // included in the element at 'start'.
         ++part;
         totalCoverage.erase(part, totalCoverage.end());
      }
      else
      {
         totalCoverage.push_back(interval);
      }

      double lengthSoFar = ComputeSum(totalCoverage);
      if (lengthSoFar / mCoverageInterval.GetDuration() >= mParameter)
      {
         return interval.GetDuration();
      }
   }
   // If all else fails
   return 0.0;
}

// Compute the standard deviation of all access interval durations.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The standard deviation of the set of access interval durations.
double AccessDuration::ComputeStddev(std::vector<AccessInterval>& aIntervals) const
{
   double x{0.0};
   double x2{0.0};
   for (auto& gap : aIntervals)
   {
      double duration = gap.GetDuration();
      x += duration;
      x2 += duration * duration;
   }
   x /= aIntervals.size();
   x2 /= aIntervals.size();
   return sqrt(x2 - x * x);
}

// Compute the sum of all access interval durations.
// \param aIntervals  A reference to a vector of access intervals.
// \return            The sum of the access interval durations.
double AccessDuration::ComputeSum(std::vector<AccessInterval>& aIntervals) const
{
   auto sumOp = [](double aSum, const AccessInterval& aInterval) { return aSum + aInterval.GetDuration(); };

   return std::accumulate(aIntervals.begin(), aIntervals.end(), 0.0, sumOp);
}

// Longest to shortest
void AccessDuration::SortIntervalsByDuration(std::vector<AccessInterval>& aIntervals) const
{
   auto byDuration = [](const AccessInterval& aLHS, const AccessInterval& aRHS)
   { return aLHS.GetDuration() > aRHS.GetDuration(); };
   std::sort(aIntervals.begin(), aIntervals.end(), byDuration);
}

void AccessDuration::OnAccessIntervalComplete(const GridAsset& aGridAsset,
                                              const FreeAsset& /*aFreeAsset*/,
                                              const AccessInterval& aInterval)
{
   auto gaIter = mIntervals.find(aGridAsset);
   if (gaIter != mIntervals.end())
   {
      // There has been an interval for aGridAsset before now.
      // For this MoE, we do not combine overlapping access intervals.
      gaIter->second.push_back(aInterval);
   }
   else
   {
      // This is the first interval, so just add it to the interval map.
      mIntervals.emplace(std::make_pair(aGridAsset, std::vector<AccessInterval>{aInterval}));
   }
}

AccessDuration::SubType AccessDuration::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "mean")
   {
      retval = SubType::cMEAN;
   }
   else if (aIdentifier == "minimum")
   {
      retval = SubType::cMIN;
   }
   else if (aIdentifier == "maximum")
   {
      retval = SubType::cMAX;
   }
   else if (aIdentifier == "percent_above")
   {
      retval = SubType::cPERCENT_ABOVE;
   }
   else if (aIdentifier == "standard_deviation")
   {
      retval = SubType::cSTDDEV;
   }
   else if (aIdentifier == "sum")
   {
      retval = SubType::cSUM;
   }

   return retval;
}

bool AccessDuration::SubtypeHasParameter(SubType aSubType)
{
   return (aSubType == SubType::cPERCENT_ABOVE);
}

} // namespace coverage
} // namespace wsf
