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

#include "WsfCoverageN_AssetCoverage.hpp"

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

bool N_AssetCoverage::ProcessInput(UtInput& aInput)
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
double N_AssetCoverage::GetDefaultValue() const
{
   double retval{};
   switch (mSubType)
   {
   case SubType::cUNKNOWN:
      throw std::runtime_error{"Trying to get default value for unknown subtype in N Asset Coverage"};
      break;
   default:
      retval = 0.0;
      break;
   }
   return retval;
}

//! Return the measured value for the given @p aGridAsset.
double N_AssetCoverage::GetMeasuredValue(const GridAsset& aGridAsset) const
{
   double retval = GetDefaultValue();
   auto   gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      retval = gaIter->second.mData;
   }
   return retval;
}

void N_AssetCoverage::CollectionStarting(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetStart(aSimTime);
}

//! Return the text header for output files for this MOE.
std::string N_AssetCoverage::GetValueHeader() const
{
   std::string        retval{};
   std::ostringstream percentString;
   switch (mSubType)
   {
   case SubType::cMEAN:
      retval = "Mean";
      break;
   case SubType::cMAX:
      retval = "Max";
      break;
   case SubType::cMIN:
      retval = "Min";
      break;
   case SubType::cPERCENT_ABOVE:
      percentString << (mParameter * 100.0) << "% Above";
      retval = percentString.str();
      break;
   case SubType::cUNIQUE:
      retval = "Unique";
      break;

   case SubType::cUNKNOWN: // Intentional fall-through
   default:                //
      throw std::runtime_error{"Unknown subtype for N Asset Coverage MOE"};
      break;
   }
   return retval;
}

bool N_AssetCoverage::InitializeP(Coverage& aCoverage)
{
   if (mSubType == SubType::cUNKNOWN)
   {
      auto err = ut::log::error() << "MOE subtype not specified.";
      err.AddNote() << "Measure: " << GetName();
      err.AddNote() << "Type:    " << GetType();
      return false;
   }

   mCallbacks.Add(aCoverage.AccessIntervalComplete().Connect(&N_AssetCoverage::OnAccessIntervalComplete, this));
   return true;
}

void N_AssetCoverage::CollectionCompletingP(Coverage& aCoverage, double aSimTime)
{
   mCoverageInterval.SetEnd(aSimTime);

   for (auto& gaIter : mData)
   {
      mData[gaIter.first].mData = ComputeValue(gaIter.second);
      gaIter.second.mCountFrames.clear();
   }
}

//! Compute the measure's value given the intervals, @p aIntervals.
double N_AssetCoverage::ComputeValue(const GridData& aData) const
{
   double retval{GetDefaultValue()};
   if (!aData.mCountFrames.empty())
   {
      switch (mSubType)
      {
      case SubType::cMEAN:
         retval = ComputeMean(aData.mCountFrames);
         break;
      case SubType::cMAX:
         retval = ComputeMax(aData.mCountFrames);
         break;
      case SubType::cMIN:
         retval = ComputeMin(aData.mCountFrames);
         break;
      case SubType::cPERCENT_ABOVE:
         retval = ComputePercentAbove(aData.mCountFrames);
         break;
      case SubType::cUNIQUE:
         retval = ComputeUnique(aData.mAssetCount);
         break;
      case SubType::cUNKNOWN: // Intentional fallthrough
      default:                //
         throw std::runtime_error{"Attempting to compute for unknown subtype in N Asset Coverage."};
         break;
      }
   }
   return retval;
}

//! Compute the mean number of free assets that can be accessed from a grid point.
//! \param aFrames  A grid point's GridData::mCountFrames.
//! \return         The mean number of free assets that can be accessed.
double N_AssetCoverage::ComputeMean(const std::map<double, int>& aFrames) const
{
   double ret{0.0};
   double prevFrameTime{mCoverageInterval.GetStart()};
   int    acc{0};
   for (const auto& frame : aFrames)
   {
      ret += static_cast<double>(acc) * (frame.first - prevFrameTime);
      acc += frame.second;
      prevFrameTime = frame.first;
   }
   ret += acc * (mCoverageInterval.GetEnd() - prevFrameTime);
   return ret / mCoverageInterval.GetDuration();
}

//! Compute the maximum number of free assets that can be accessed from a grid point.
//! \param aFrames  A grid point's GridData::mCountFrames.
//! \return         The maximum number of free assets that can be accessed.
double N_AssetCoverage::ComputeMax(const std::map<double, int>& aFrames) const
{
   double ret{0.0};
   int    acc{0};
   for (const auto& frame : aFrames)
   {
      ret = std::max(static_cast<double>(acc), ret);
      acc += frame.second;
   }
   ret = std::max(static_cast<double>(acc), ret);
   return ret;
}

//! Compute the minimum number of free assets that can be accessed from a grid point.
//! \param aFrames  A grid point's GridData::mCountFrames.
//! \return         The minimum number of free assets that can be accessed.
double N_AssetCoverage::ComputeMin(const std::map<double, int>& aFrames) const
{
   double ret{std::numeric_limits<double>::max()};
   double prevFrameTime{mCoverageInterval.GetStart()}; // only used to get the last frame
   int    acc{0};
   for (const auto& frame : aFrames)
   {
      if (frame.first > mCoverageInterval.GetStart())
      {
         // The if statement is so that it won't count the 0.0 time between the
         // first time frame and the start of the coverage interval, if they match up.
         ret = std::min(static_cast<double>(acc), ret);
      }
      acc += frame.second;
      prevFrameTime = frame.first;
   }
   if (prevFrameTime < mCoverageInterval.GetEnd())
   {
      // The if statement is so that it won't count the 0.0 time between the
      // last time frame and the end of the coverage interval, if they match up.
      ret = std::min(static_cast<double>(acc), ret);
   }
   return ret;
}

//! Compute the maximum number of free assets that can be accessed from a grid point @p mParameter percent of the time.
//! \param aFrames  A grid point's GridData::mCountFrames.
//! \return         The maximum number of free assets that can be accessed from a grid point @p mParameter percent of the time.
double N_AssetCoverage::ComputePercentAbove(const std::map<double, int>& aFrames) const
{
   std::map<int, double> durations;
   double                prevFrameTime{mCoverageInterval.GetStart()};
   int                   acc{0};
   int                   max{0};
   auto                  dur = durations.find(acc);
   for (const auto& frame : aFrames)
   {
      if (dur == durations.end())
      {
         durations[acc] = frame.first - prevFrameTime;
         max            = std::max(max, acc);
      }
      else
      {
         durations[acc] += frame.first - prevFrameTime;
      }
      acc += frame.second;
      prevFrameTime = frame.first;
      dur           = durations.find(acc);
   }
   if (dur == durations.end())
   {
      durations[acc] = mCoverageInterval.GetEnd() - prevFrameTime;
      max            = std::max(max, acc);
   }
   else
   {
      durations[acc] += mCoverageInterval.GetEnd() - prevFrameTime;
   }

   // Actually find the answer here
   double above{0.0};
   int    ret{max + 1};
   while (above < mParameter)
   {
      --ret;
      dur = durations.find(ret);
      if (dur != durations.end())
      {
         above += dur->second / mCoverageInterval.GetDuration();
      }
   }
   return static_cast<double>(ret);
}

//! Compute the total number of unique free assets that are accessed by the grid point.
//! \param aAssets  The set of all assets that the grid point has accessed.
//! \return         The total number of unique free assets that are accessed by the grid point.
double N_AssetCoverage::ComputeUnique(const std::set<FreeAsset>& aAssets) const
{
   return static_cast<double>(aAssets.size());
}

void N_AssetCoverage::OnAccessIntervalComplete(const GridAsset&      aGridAsset,
                                               const FreeAsset&      aFreeAsset,
                                               const AccessInterval& aInterval)
{
   auto gaIter = mData.find(aGridAsset);
   if (gaIter != mData.end())
   {
      // There has been an interval for aGridAsset before now.
      auto faIter = gaIter->second.mAssetCount.find(aFreeAsset);
      if (faIter == gaIter->second.mAssetCount.end())
      {
         gaIter->second.mAssetCount.emplace(aFreeAsset);
      }

      auto frame = gaIter->second.mCountFrames.find(aInterval.GetStart());
      if (frame == gaIter->second.mCountFrames.end())
      {
         gaIter->second.mCountFrames.emplace(aInterval.GetStart(), 1);
      }
      else
      {
         gaIter->second.mCountFrames.at(aInterval.GetStart()) += 1;
      }

      frame = gaIter->second.mCountFrames.find(aInterval.GetEnd());
      if (frame == gaIter->second.mCountFrames.end())
      {
         gaIter->second.mCountFrames.emplace(aInterval.GetEnd(), -1);
      }
      else
      {
         gaIter->second.mCountFrames.at(aInterval.GetEnd()) -= 1;
      }
   }
   else
   {
      // This is the first interval, so just add it to the interval map.
      gaIter = mData.emplace(aGridAsset, GridData{}).first;
      gaIter->second.mAssetCount.emplace(aFreeAsset);
      gaIter->second.mCountFrames.emplace(aInterval.GetStart(), 1);
      gaIter->second.mCountFrames.emplace(aInterval.GetEnd(), -1);
      gaIter->second.mData = 0.0;
   }
}

N_AssetCoverage::SubType N_AssetCoverage::GetSubtypeFromIdentifier(const std::string& aIdentifier)
{
   SubType retval{SubType::cUNKNOWN};
   if (aIdentifier == "mean")
   {
      retval = SubType::cMEAN;
   }
   else if (aIdentifier == "maximum")
   {
      retval = SubType::cMAX;
   }
   else if (aIdentifier == "minimum")
   {
      retval = SubType::cMIN;
   }
   else if (aIdentifier == "percent_above")
   {
      retval = SubType::cPERCENT_ABOVE;
   }
   else if (aIdentifier == "unique")
   {
      retval = SubType::cUNIQUE;
   }

   return retval;
}

bool N_AssetCoverage::SubtypeHasParameter(SubType aSubType)
{
   return (aSubType == SubType::cPERCENT_ABOVE);
}

} // namespace coverage
} // namespace wsf
