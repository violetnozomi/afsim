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

#include "WsfCoverage.hpp"

#include <sstream>
#include <stdexcept>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfCoverageGrid.hpp"
#include "WsfCoverageGridAction.hpp"
#include "WsfCoverageMeasureTypes.hpp"
#include "WsfCoverageOverlayOutput.hpp"
#include "WsfCoverageSimulationExtension.hpp"
#include "WsfDateTime.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

bool Coverage::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "output_dir")
   {
      aInput.ReadCommand(mOutputDir);
   }
   else if (command == "raw_data_file")
   {
      std::string filename;
      aInput.ReadCommand(filename);
      mRawData = ut::make_unique<RawDataOutput>(filename);
   }
   else if (command == "overlay_file")
   {
      aInput.ReadCommand(mOverlayOutputFileName);
      mOverlayOutputFileName += ".cvg";
   }
   else if (command == "start_time")
   {
      double start;
      aInput.ReadValueOfType(start, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(start, 0.0);
      mCoverageInterval.SetStart(start);
   }
   else if (command == "start_epoch")
   {
      aInput.ReadValue(mStartEpoch);
   }
   else if (command == "end_time")
   {
      double end;
      aInput.ReadValueOfType(end, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(end, 0.0);
      mCoverageInterval.SetEnd(end);
   }
   else if (command == "end_epoch")
   {
      aInput.ReadValue(mEndEpoch);
   }
   else if (command == "grid")
   {
      aInput.ReadCommand(mGridName);
   }
   else if (command == "assets")
   {
      UtInputBlock block{aInput};
      block.ProcessInput(&mAssetSpecification);
   }
   else if (command == "moe")
   {
      UtInputBlock block{aInput};
      std::string  moeName;
      if (!block.ReadCommand(moeName))
      {
         throw UtInput::BadValue{aInput, "Expected MOE name."};
      }
      std::string moeTypeName;
      if (!block.ReadCommand(moeTypeName))
      {
         throw UtInput::BadValue{aInput, "Expected MOE type."};
      }
      auto moeTypePtr = MeasureTypes::Get(mScenario).Find(moeTypeName);
      if (!moeTypePtr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized MOE type."};
      }
      auto moePtr = ut::clone(moeTypePtr);
      moePtr->SetName(moeName);
      block.ProcessInput(moePtr.get());
      mMeasures[moeName] = std::move(moePtr);
   }
   else if (command == "interval_constraint")
   {
      UtInputBlock block{aInput};
      mConstraints.emplace_back();
      block.ProcessInput(&mConstraints.back());
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! Initialize this coverage object.
//!
//! This is called by the simulation extension when the simulation is being
//! initialized. This is a template pattern method; most of the work
//! occurs in methods on this class, InitializeP is called on the derived
//! classes to set up any connections with simulation events.
//!
//! \param aSimulation - The simulation that this coverage object will be part of.
//! \returns           - true on successful initialization; false otherwise.
bool Coverage::Initialize(WsfSimulation& aSimulation)
{
   bool retval = AssureFilterFileUniqueness();

   retval             = retval && FindGrid(aSimulation);
   mGridAssetCategory = GetName() + "_" + mGridPtr->GetName() + "_" + "grid_asset";
   mFreeAssetCategory = GetName() + "_" + mGridPtr->GetName() + "_" + "free_asset";

   retval = retval && NullDeviceCheck();

   retval = retval && InitializeCoverageInterval(aSimulation);

   // This must occur after this object has found its Grid.
   retval = retval && InitializeMeasures();

   retval = retval && InitializeP(aSimulation);

   return retval;
}

//! Resolve the asset specifications.
//!
//! The user is able to specify assets in groups, so this method resolves
//! the user-specified assets into a single platform per asset form used
//! by the coverage object. If there is any problem making this resolution,
//! then this will return false.
//!
//! \param aSimulation - The simulation in which this coverage object participates.
//! \returns           - true if all assets are successfully resolved; false otherwise.
bool Coverage::ResolveAssets(WsfSimulation& aSimulation)
{
   bool retval{true};
   for (auto& asset : mAssetSpecification.GetAssets())
   {
      switch (asset.mType)
      {
      case AssetSpecification::AssetClass::cPLATFORM:
         retval = ResolvePlatformFreeAsset(aSimulation, asset);
         break;
      case AssetSpecification::AssetClass::cPLATFORM_TYPE:
         retval = ResolvePlatformTypeFreeAsset(aSimulation, asset);
         break;
      case AssetSpecification::AssetClass::cCATEGORY:
         retval = ResolveCategoryFreeAssets(aSimulation, asset);
         break;
      case AssetSpecification::AssetClass::cGROUP:
         retval = ResolveGroupFreeAssets(aSimulation, asset);
         break;
      case AssetSpecification::AssetClass::cUNKNOWN: // Intentional fall-through
      default:
      {
         ut::log::error() << "Unknown asset class for specified free asset.";
         retval = false;
         break;
      }
      }
      if (!retval)
      {
         break;
      }
   }
   return retval;
}

//! Categorize all platforms in the simulation relative to this coverage object.
//!
//! Categorization allows for fast detection of the status of a platform, and allows
//! for sensors on platforms to ignore irrelevant platforms.
//!
//! \param aSimulation - The simulation with platforms to be categorized.
void Coverage::CategorizePlatforms(const WsfSimulation& aSimulation)
{
   mGridPtr->CategorizeAssets(aSimulation, GetGridAssetCategory());
   CategorizeAssets(aSimulation);
}

void Coverage::PendingStart(WsfSimulation& aSimulation)
{
   mSimulationPtr = &aSimulation;

   // Notify the measures that the interval is starting, either immediately or via an
   // event in the simulation at the appropriate time.
   if (mCoverageInterval.GetStart() > 0.0)
   {
      aSimulation.AddEvent(ut::make_unique<WsfOneShotEvent>(mCoverageInterval.GetStart(),
                                                            [this]() { StartInterval(mCoverageInterval.GetStart()); }));
   }
   else
   {
      StartInterval(0.0);
   }

   // Schedule an event for the end of the coverage interval to finalize the work of this coverage object.
   // If the coverage interval ends before the simulation, this event will finalize the work of
   // the coverage object. Finalize() is also called via the simulation extension when the simulation
   // is complete to catch cases where the simulation ends unexpectedly before the end of the
   // interval.
   UtCalendar epoch = aSimulation.GetDateTime().GetStartDateAndTime();
   aSimulation.AddEvent(ut::make_unique<WsfOneShotEvent>(mCoverageInterval.GetEnd(),
                                                         [this, epoch]() { Finalize(mCoverageInterval.GetEnd(), epoch); }));

   // Initialize any Constraints
   for (auto& iter : mConstraints)
   {
      iter.Initialize(*this, mOutputDir);
   }

   // Allow subclasses to do any preparation they need once all platforms have been added and categorized.
   PendingStartP(aSimulation);
}

//! Finalize the work of this coverage object.
//!
//! This will close any open intervals, disconnect the object from simulation
//! events, and write out the raw interaction data at the end of the
//! coverage interval. This method is called either as a result of a coverage
//! interval end-point that is before the end of the simulation, or due to the
//! end of the simulation.
//!
//! \param aSimTime       - The time at which the coverage is being finalized.
//! \param aSimStartEpoch - The start epoch of the simulation.
void Coverage::Finalize(double aSimTime, const UtCalendar& aSimStartEpoch)
{
   // Do any final operations. This is called either at the end of the interval,
   // or if the simulation ends.
   if (!mFinalized)
   {
      CloseAllOpenIntervals(aSimTime);

      for (auto& measure : mMeasures)
      {
         measure.second->CollectionCompleting(*this, aSimTime);
      }

      // Disconnect from simulation events.
      mCallbacks.Clear();

      // Write out the raw data if selected.
      if (mRawData)
      {
         mRawData->SetOutputDir(mOutputDir);
         mRawData->Write(aSimStartEpoch);
      }

      // Finalize any Constraints
      for (auto& iter : mConstraints)
      {
         iter.Finalize(aSimStartEpoch);
      }

      // Write overlay data if selected.
      WriteOverlayData();

      mFinalized = true;
   }
}

bool Coverage::IsPlatformOfType(const std::string& aPlatformName, const std::string& aPlatformType) const
{
   bool retval{false};
   if (mSimulationPtr)
   {
      const WsfPlatform* platformPtr = mSimulationPtr->GetPlatformByName(aPlatformName);
      if (platformPtr && platformPtr->IsA_TypeOf(aPlatformType))
      {
         retval = true;
      }
   }
   return retval;
}

bool Coverage::IsPlatformDeviceOfType(const std::string& aPlatformName,
                                      const std::string& aDeviceName,
                                      const std::string& aDeviceType) const
{
   bool retval{false};
   if (mSimulationPtr)
   {
      const WsfPlatform* platformPtr = mSimulationPtr->GetPlatformByName(aPlatformName);
      if (platformPtr)
      {
         retval = VerifyDeviceAndType(*platformPtr, aDeviceName, aDeviceType);
      }
   }
   return retval;
}

//! Start an interval for the given assets with the given start time.
//!
//! This will start an access interval between the given @p aGridAsset and
//! @p aFreeAsset with the given @p aSimTime as the start of the interval.
//! The provided assets should already be filtered to be relevant for this
//! coverage object, and the time of this event should be inside the
//! coverage interval. This method handles notification of any MOE observing
//! interval start times.
//!
//! \param aGridAsset - The grid asset for the access interval.
//! \param aFreeAsset - The free asset for the access interval.
//! \param aSimTime   - The simulation time of the start of the access interval.
void Coverage::AddIntervalStartToData(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, double aSimTime)
{
   auto gaIter = mData.find(aGridAsset);
   if (gaIter == mData.end())
   {
      // first event for that grid asset, so add it to the data map.
      auto result = mData.emplace(std::make_pair(aGridAsset, std::map<FreeAsset, AccessInterval>{}));
      gaIter      = result.first;
   }

   auto faIter = gaIter->second.find(aFreeAsset);
   if (faIter == gaIter->second.end())
   {
      // first event for that free asset, so add it to the data map
      auto result = gaIter->second.emplace(std::make_pair(aFreeAsset, AccessInterval{}));
      faIter      = result.first;
   }

   if (!faIter->second.HasStart())
   {
      faIter->second.SetStart(aSimTime);
      if (InCoverageInterval(aSimTime))
      {
         mIntervalStart(aGridAsset, aFreeAsset, aSimTime);
      }
   }
   else
   {
      throw std::runtime_error{"Attempting to start an access interval that has already started."};
   }
}

//! End an interval for the given assets at the given time.
//!
//! This will end an access interval between the given @p aGridAsset and
//! @p aFreeAsset with the given @p aSimTime as the end of the interval.
//! The provided assets should already be filtered to be relevant for this
//! coverage object, and the time of this event should be inside the coverage
//! interval. This method handles notification of MOEs about the end of an
//! interval, the completion of an interval, and in some cases the start of
//! an interval. This method also collects the full interval data into the
//! raw data store in case the output of this data has been requested by
//! the user.
//!
//! \param aGridAsset - The grid asset for the access interval.
//! \param aFreeAsset - The free asset for the access interval.
//! \param aSimTime   - The simulation time of the end of the access interval.
void Coverage::AddIntervalEndToData(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, double aSimTime)
{
   auto gaIter = mData.find(aGridAsset);
   if (gaIter == mData.end())
   {
      throw std::runtime_error{"Attempting to end an interval for a grid asset for which no intervals have started."};
   }

   auto faIter = gaIter->second.find(aFreeAsset);
   if (faIter == gaIter->second.end())
   {
      throw std::runtime_error{"Attempting to end an interval for a free asset for which no interval has started."};
   }

   if (!faIter->second.HasEnd())
   {
      if (aSimTime < faIter->second.GetStart())
      {
         // End before the beginning. Something went wrong
         throw std::runtime_error{"Attempting to end an access interval before it started."};
      }

      faIter->second.SetEnd(aSimTime);
      auto reducedInterval = mCoverageInterval.IntersectionWith(faIter->second);
      if (reducedInterval.IsValid())
      {
         // If the start of reducedInterval is different than the start of faIter->second, then
         // it must have been that the intersection operation clamped the access interval to
         // the coverage interval. In this case, the interval start had not been called during
         // AddIntervalStartToData because the start was before the coverage interval began.
         if (faIter->second.GetStart() != reducedInterval.GetStart())
         {
            mIntervalStart(aGridAsset, aFreeAsset, reducedInterval.GetStart());
         }
         mIntervalEnd(aGridAsset, aFreeAsset, reducedInterval.GetEnd());
         ProcessInterval(aGridAsset, aFreeAsset, reducedInterval);
      }

      // Now that the interval is complete, reset the interval stored in the
      // data map to prepare for the next.
      faIter->second = AccessInterval{};
   }
   else
   {
      throw std::runtime_error{"Attempting to end an access interval that has already ended."};
   }
}

bool Coverage::FindGrid(const WsfSimulation& aSimulation)
{
   bool retval{true};
   auto gridPtr = SimulationExtension::Get(aSimulation).GetGrid(mGridName);
   if (gridPtr)
   {
      gridPtr->SetUsedFlag();
      mGridPtr = gridPtr;
   }
   else
   {
      auto err = ut::log::error() << "Grid required by coverage object does not exist.";
      err.AddNote() << "Grid: " << mGridName;
      err.AddNote() << "Coverage: " << GetName();
      retval = false;
   }
   return retval;
}

bool Coverage::NullDeviceCheck()
{
   // Decide if the grid asset has a null device
   bool gridNull;
   bool allGridNull;
   mGridPtr->HasNullDevice(gridNull, allGridNull);

   // Deecide if any free asset has a null device
   bool freeNull{false};
   bool allFreeNull{true};
   for (std::size_t i = 0; i < mAssetSpecification.NumSpecifications(); ++i)
   {
      if (mAssetSpecification.GetAsset(i).mDeviceName == cNULL_COVERAGE_DEVICE_NAME)
      {
         freeNull = true;
      }
      else
      {
         allFreeNull = false;
      }
   }

   return AreNullDevicesAllowed(gridNull, allGridNull, freeNull, allFreeNull);
}

bool Coverage::InitializeCoverageInterval(const WsfSimulation& aSimulation)
{
   bool retval{true};

   const WsfDateTime& dateTime      = aSimulation.GetDateTime();
   UtCalendar         simStartEpoch = dateTime.GetStartDateAndTime();
   double             endTime       = aSimulation.GetEndTime();

   // Handle coverage interval start time initialization. Note this also assures that the
   // start of the coverage interval is not before the start epoch of the simulation.
   if (GetStartTime() < 0.0)
   {
      if (!IsStartEpochDefined())
      {
         mStartEpoch = dateTime.GetStartDateAndTime();
         mCoverageInterval.SetStart(0.0);
      }
      if (mStartEpoch < dateTime.GetStartDateAndTime())
      {
         mStartEpoch = dateTime.GetStartDateAndTime();
         mCoverageInterval.SetStart(0.0);
         auto msg = ut::log::warning() << "Coverage interval start epoch before simulation start epoch.";
         msg.AddNote() << "Coverage interval start reset to simulation start time.";
         msg.AddNote() << "Coverage:    " << GetName();
         msg.AddNote() << "start_time:  " << GetStartTime();
         msg.AddNote() << "start_epoch: " << mStartEpoch;
      }
      else
      {
         mCoverageInterval.SetStart(mStartEpoch.GetTimeSince(simStartEpoch));
      }
   }
   else
   {
      if (IsStartEpochDefined())
      {
         auto err = ut::log::error() << "Multiply defined coverage interval start.";
         err.AddNote() << "Coverage:    " << GetName();
         err.AddNote() << "start_time:  " << GetStartTime();
         err.AddNote() << "start_epoch: " << mStartEpoch;
         retval = false;
      }
      else
      {
         dateTime.GetCurrentTime(GetStartTime(), mStartEpoch);
      }
   }

   // Handle coverage interval end time initialization.
   if (GetEndTime() < 0.0)
   {
      if (!IsEndEpochDefined())
      {
         mCoverageInterval.SetEnd(endTime);
         dateTime.GetCurrentTime(GetEndTime(), mEndEpoch);
      }
      else
      {
         mCoverageInterval.SetEnd(mEndEpoch.GetTimeSince(simStartEpoch));
      }
   }
   else
   {
      if (IsEndEpochDefined())
      {
         auto err = ut::log::error() << "Multiply defined coverage interval end.";
         err.AddNote() << "Coverage:  " << GetName();
         err.AddNote() << "end_time:  " << GetEndTime();
         err.AddNote() << "end_epoch: " << mEndEpoch;
         retval = false;
      }
      else
      {
         dateTime.GetCurrentTime(GetEndTime(), mEndEpoch);
      }
   }

   // If either interval endpoint is past the end of the sim, set it to the simulation time
   if (retval)
   {
      if (GetStartTime() > endTime)
      {
         auto err = ut::log::error() << "Coverage start time is after simulation end.";
         err.AddNote() << "Coverage: " << GetName();
         err.AddNote() << "start_time: " << GetStartTime();
         err.AddNote() << "simulation end time: " << endTime;
         retval = false;
      }

      if (GetEndTime() > endTime)
      {
         mCoverageInterval.SetEnd(endTime);
         dateTime.GetCurrentTime(endTime, mEndEpoch);
         auto msg = ut::log::warning() << "Coverage end time is after simulation end.";
         msg.AddNote() << "Coverage end time reset to simulation end time.";
         msg.AddNote() << "Coverage:  " << GetName();
         msg.AddNote() << "end_time:  " << GetEndTime();
         msg.AddNote() << "end_epoch: " << mEndEpoch;
      }
   }

   // Check that the interval so defined has an end later than the start
   if (retval)
   {
      if (GetStartTime() > GetEndTime())
      {
         auto err = ut::log::error() << "Coverage interval is inverted.";
         err.AddNote() << "Coverage:   " << GetName();
         err.AddNote() << "start_time: " << GetStartTime();
         err.AddNote() << "end_time:   " << GetEndTime();
         retval = false;
      }
      else if (GetEndTime() - GetStartTime() == 0.0)
      {
         auto err = ut::log::error() << "Coverage interval has zero duration.";
         err.AddNote() << "Coverage:   " << GetName();
         err.AddNote() << "start_time: " << GetStartTime();
         err.AddNote() << "end_time:   " << GetEndTime();
         retval = false;
      }
   }

   return retval;
}

bool Coverage::InitializeMeasures()
{
   bool retval{true};
   for (auto& iter : mMeasures)
   {
      retval = retval && iter.second->Initialize(*this);
   }
   return retval;
}

bool Coverage::AssureFilterFileUniqueness() const
{
   std::set<std::string> files{};

   std::string name;
   for (auto& iter : mConstraints)
   {
      name = iter.GetOutputFileName();
      if (!name.empty())
      {
         if (files.find(name) == files.end())
         {
            files.emplace(name);
         }
         else
         {
            auto err = ut::log::error() << "Non-unique output file names.";
            err.AddNote() << "More than one interval_constraint is configured to output to '" << name << "'";
            return false;
         }
      }
   }

   return true;
}

bool Coverage::VerifyDevice(const WsfPlatform& aPlatform, const std::string& aDeviceName) const
{
   bool retval{true};
   if (aDeviceName != cNULL_COVERAGE_DEVICE_NAME)
   {
      retval = VerifyDeviceP(aPlatform, aDeviceName);
   }
   return retval;
}

bool Coverage::ResolvePlatformFreeAsset(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset)
{
   bool         retval{true};
   WsfPlatform* platformPtr = aSimulation.GetPlatformByName(aAsset.mName);
   if (platformPtr)
   {
      if (VerifyDevice(*platformPtr, aAsset.mDeviceName))
      {
         mFreeAssets.emplace(aAsset.mName, aAsset.mDeviceName);
      }
      else
      {
         auto msg = ut::log::error() << "Unable to resolve free asset.";
         msg.AddNote() << "No usable device on platform.";
         msg.AddNote() << "Name:          " << aAsset.mName;
         msg.AddNote() << "Device:        " << aAsset.mDeviceName;
         msg.AddNote() << "Coverage:      " << GetName();
         msg.AddNote() << "Coverage type: " << GetType();
         retval = false;
      }
   }
   else
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No such platform: " << aAsset.mName;
      retval = false;
   }
   return retval;
}

bool Coverage::ResolvePlatformTypeFreeAsset(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset)
{
   bool foundAny{false};
   bool foundAnyOfType{false};

   for (std::size_t i = 0; i < aSimulation.GetPlatformCount(); ++i)
   {
      const WsfPlatform* platPtr = aSimulation.GetPlatformEntry(i);
      if (aSimulation.GetPlatformTypeId(platPtr->GetIndex()) == aAsset.mName)
      {
         foundAnyOfType = true;
         if (VerifyDevice(*platPtr, aAsset.mDeviceName))
         {
            mFreeAssets.emplace(platPtr->GetName(), aAsset.mDeviceName);
            foundAny = true;
         }
      }
   }

   if (!foundAnyOfType)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No such platform_type: " << aAsset.mName;
   }
   else if (!foundAny)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No platforms of given type have specified device.";
      msg.AddNote() << "platform_type: " << aAsset.mName;
      msg.AddNote() << "Device:        " << aAsset.mDeviceName;
   }

   return foundAny;
}

bool Coverage::ResolveCategoryFreeAssets(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset)
{
   bool foundAny{false};
   bool foundAnyOfCategory{false};

   for (std::size_t i = 0; i < aSimulation.GetPlatformCount(); ++i)
   {
      const WsfPlatform* platPtr = aSimulation.GetPlatformEntry(i);
      if (platPtr->IsCategoryMember(aAsset.mName))
      {
         foundAnyOfCategory = true;
         if (VerifyDevice(*platPtr, aAsset.mDeviceName))
         {
            mFreeAssets.emplace(platPtr->GetName(), aAsset.mDeviceName);
            foundAny = true;
         }
      }
   }

   if (!foundAnyOfCategory)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No platforms in category: " << aAsset.mName;
   }
   else if (!foundAny)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No platforms in given category have specified device.";
      msg.AddNote() << "Category: " << aAsset.mName;
      msg.AddNote() << "Device:   " << aAsset.mDeviceName;
   }

   return foundAny;
}

bool Coverage::ResolveGroupFreeAssets(const WsfSimulation& aSimulation, const AssetSpecification::Asset& aAsset)
{
   bool foundAny{false};
   bool foundAnyOfGroup{false};

   for (std::size_t i = 0; i < aSimulation.GetPlatformCount(); ++i)
   {
      WsfPlatform* platPtr = aSimulation.GetPlatformEntry(i);
      if (platPtr->IsGroupMember(aAsset.mName))
      {
         foundAnyOfGroup = true;
         if (VerifyDevice(*platPtr, aAsset.mDeviceName))
         {
            mFreeAssets.emplace(platPtr->GetName(), aAsset.mDeviceName);
            foundAny = true;
         }
      }
   }

   if (!foundAnyOfGroup)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No platforms in group: " << aAsset.mName;
   }
   else if (!foundAny)
   {
      auto msg = ut::log::error() << "Unable to resolve free asset.";
      msg.AddNote() << "No platforms in given group have specified device.";
      msg.AddNote() << "Group:  " << aAsset.mName;
      msg.AddNote() << "Device: " << aAsset.mDeviceName;
   }

   return foundAny;
}

void Coverage::StartInterval(double aSimTime)
{
   for (auto& measure : mMeasures)
   {
      measure.second->CollectionStarting(*this, aSimTime);
   }
}

//! End all outstanding access intervals on this object.
//!
//! When the coverage interval ends, any open access intervals need to be closed.
//! The given @p aSimTime is used as the end time for any access interval that is
//! already open. This will notify MOEs about the end of the interval, and the
//! completion of the interval. This will also save any completed intervals in the
//! raw data store if the user has requested raw data output.
//!
//! The given @p aSimTime will either be the end of the coverage interval
//! or the end of simulation, whichever comes first.
//!
//! \param aSimTime - The simulation time at which the coverage interval is being ended.
void Coverage::CloseAllOpenIntervals(double aSimTime)
{
   for (auto gaIter : mData)
   {
      for (auto faIter : gaIter.second)
      {
         if (faIter.second.HasStart())
         {
            faIter.second.SetEnd(aSimTime);
            auto reducedInterval = mCoverageInterval.IntersectionWith(faIter.second);
            if (reducedInterval.IsValid())
            {
               if (reducedInterval.GetStart() != faIter.second.GetStart())
               {
                  // This occurs if the start of an access interval was before the start of the coverage
                  // interval, and was just clamped by the intersection. In this case, the interval's
                  // start has not been signalled to any subscribed MOEs.
                  mIntervalStart(gaIter.first, faIter.first, reducedInterval.GetStart());
               }
               mIntervalEnd(gaIter.first, faIter.first, reducedInterval.GetEnd());
               ProcessInterval(gaIter.first, faIter.first, reducedInterval);
               faIter.second = AccessInterval{};
            }
         }
      }
   }
}

void Coverage::ProcessInterval(const GridAsset& aGridAsset, const FreeAsset& aFreeAsset, const AccessInterval& aInterval)
{
   // Filter out intervals using the defined constraints.
   for (auto& iter : mConstraints)
   {
      if (iter.FilterInterval(aGridAsset, aFreeAsset, aInterval))
      {
         return;
      }
   }

   // The interval passed all filters, so it is processed.
   mIntervalComplete(aGridAsset, aFreeAsset, aInterval);

   // The interval is saveed to raw data if needed.
   if (mRawData)
   {
      mRawData->AddInterval(aGridAsset, aFreeAsset, aInterval);
   }
}

void Coverage::CategorizeAssets(const WsfSimulation& aSimulation)
{
   std::string category = GetFreeAssetCategory();
   std::string universal{cUNIVERSAL_FREE_ASSET_CATEGORY};
   for (auto& asset : mFreeAssets)
   {
      WsfPlatform* platPtr = aSimulation.GetPlatformByName(asset.GetPlatformName());
      if (platPtr)
      {
         platPtr->AddCategory(category);
         platPtr->AddCategory(universal);
      }
      else
      {
         throw std::runtime_error{"Free asset platform does not exist when it should."};
      }
   }
}

void Coverage::WriteOverlayData() const
{
   if (!mOverlayOutputFileName.empty())
   {
      if (mGridPtr->IsStructured())
      {
         OverlayOutput               output{mOverlayOutputFileName, mOutputDir};
         std::vector<const Measure*> measures{};
         for (auto& iter : mMeasures)
         {
            measures.push_back(iter.second.get());
         }
         if (!output.Write(GetName(), measures, mGridPtr))
         {
            auto err = ut::log::error() << "Unable to write coverage overlay data to file.";
            err.AddNote() << "Coverage: " << GetName();
            err.AddNote() << "Overlay output file name: " << mOverlayOutputFileName;
         }
      }
      else
      {
         auto warn = ut::log::warning() << "Grid type does not support coverage overlay data.";
         warn.AddNote() << "Coverage: " << GetName();
         warn.AddNote() << "Grid: " << mGridPtr->GetName();
         warn.AddNote() << "Grid type: " << mGridPtr->GetType();
      }
   }
}

} // namespace coverage
} // namespace wsf
