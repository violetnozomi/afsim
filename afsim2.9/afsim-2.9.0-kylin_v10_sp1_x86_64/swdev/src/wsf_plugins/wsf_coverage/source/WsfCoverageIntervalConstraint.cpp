// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCoverageIntervalConstraint.hpp"

#include <string>

#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "WsfCoverage.hpp"

namespace wsf
{
namespace coverage
{

bool IntervalConstraint::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "output_file")
   {
      std::string filename;
      aInput.ReadCommand(filename);
      mFilteredIntervalsPtr = ut::make_unique<RawDataOutput>(filename);
   }
   else
   {
      Filter filter;
      if (filter.ProcessInput(aInput))
      {
         mFilters.push_back(filter);
      }
      else
      {
         myCommand = false;
      }
   }
   return myCommand;
}

void IntervalConstraint::Initialize(const Coverage& aCoverage, const std::string& aOutputPath)
{
   mCoveragePtr = &aCoverage;
   if (mFilteredIntervalsPtr)
   {
      mFilteredIntervalsPtr->SetOutputDir(aOutputPath);
   }
}

void IntervalConstraint::Finalize(const UtCalendar& aSimStartEpoch) const
{
   if (mFilteredIntervalsPtr)
   {
      mFilteredIntervalsPtr->Write(aSimStartEpoch);
   }
}

bool IntervalConstraint::FilterInterval(const GridAsset&      aGridAsset,
                                        const FreeAsset&      aFreeAsset,
                                        const AccessInterval& aInterval) const
{
   bool shouldFilter{false};

   // Check interval against all defined filters
   for (auto& filter : mFilters)
   {
      if (filter.FilterInterval(*this, aGridAsset, aFreeAsset, aInterval))
      {
         shouldFilter = true;
         break;
      }
   }

   // If the interval is filtered, add it to the set of intervals that will be written out
   // if the user has selected to output those intervals filtered by this constraint.
   if (shouldFilter && mFilteredIntervalsPtr)
   {
      mFilteredIntervalsPtr->AddInterval(aGridAsset, aFreeAsset, aInterval);
   }

   return shouldFilter;
}

std::string IntervalConstraint::GetOutputFileName() const
{
   if (mFilteredIntervalsPtr)
   {
      return mFilteredIntervalsPtr->GetFileName();
   }
   return std::string{};
}

bool IntervalConstraint::Filter::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string filterType;
   aInput.GetCommand(filterType);
   if (filterType == "platform")
   {
      mType = ConstraintType::cPLATFORM_NAME;
      aInput.ReadCommand(mIdentifier);
      ReadLimits(aInput);
   }
   else if (filterType == "platform_type")
   {
      mType = ConstraintType::cPLATFORM_TYPE;
      aInput.ReadCommand(mIdentifier);
      ReadLimits(aInput);
   }
   else if (filterType == "device")
   {
      mType = ConstraintType::cDEVICE_NAME;
      aInput.ReadCommand(mIdentifier);
      if (mIdentifier == "none")
      {
         throw UtInput::BadValue(aInput, "Cannot constrain against device 'none'");
      }
      ReadLimits(aInput);
   }
   else if (filterType == "device_type")
   {
      mType = ConstraintType::cDEVICE_TYPE;
      aInput.ReadCommand(mIdentifier);
      ReadLimits(aInput);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool IntervalConstraint::Filter::FilterInterval(const IntervalConstraint& aConstraint,
                                                const GridAsset&          aGridAsset,
                                                const FreeAsset&          aFreeAsset,
                                                const AccessInterval&     aInterval) const
{
   bool retval{false};
   switch (mType)
   {
   case ConstraintType::cPLATFORM_NAME:
      if (aGridAsset.GetPlatformName() == mIdentifier || aFreeAsset.GetPlatformName() == mIdentifier)
      {
         retval = !MeetsConstraint(aInterval.GetDuration());
      }
      break;
   case ConstraintType::cPLATFORM_TYPE:
      if (aConstraint.IsPlatformOfType(aGridAsset.GetPlatformName(), mIdentifier) ||
          aConstraint.IsPlatformOfType(aFreeAsset.GetPlatformName(), mIdentifier))
      {
         retval = !MeetsConstraint(aInterval.GetDuration());
      }
      break;
   case ConstraintType::cDEVICE_NAME:
      if (aGridAsset.GetDeviceName() == mIdentifier || aFreeAsset.GetDeviceName() == mIdentifier)
      {
         retval = !MeetsConstraint(aInterval.GetDuration());
      }
      break;
   case ConstraintType::cDEVICE_TYPE:
      if (aConstraint.IsPlatformDeviceOfType(aGridAsset.GetPlatformName(), aGridAsset.GetDeviceName(), mIdentifier) ||
          aConstraint.IsPlatformDeviceOfType(aFreeAsset.GetPlatformName(), aFreeAsset.GetDeviceName(), mIdentifier))
      {
         retval = !MeetsConstraint(aInterval.GetDuration());
      }
      break;
   case ConstraintType::cUNKNOWN: // Intentional fall-through
   default:                       //
      // Not a known type, so no filtering should occur
      break;
   }
   return retval;
}

void IntervalConstraint::Filter::ReadLimits(UtInput& aInput)
{
   std::string type;
   aInput.ReadCommand(type);
   if (type == "minimum")
   {
      aInput.ReadValueOfType(mMinimum, UtInput::cTIME);
      aInput.ValueGreater(mMinimum, 0.0);
   }
   else if (type == "maximum")
   {
      aInput.ReadValueOfType(mMaximum, UtInput::cTIME);
      aInput.ValueGreater(mMaximum, 0.0);
   }
   else if (type == "interval")
   {
      aInput.ReadValueOfType(mMinimum, UtInput::cTIME);
      aInput.ValueGreater(mMinimum, 0.0);
      aInput.ReadValueOfType(mMaximum, UtInput::cTIME);
      aInput.ValueGreater(mMaximum, mMinimum);
   }
   else
   {
      throw UtInput::BadValue{aInput, "Unknown interval constraint type"};
   }
}

bool IntervalConstraint::Filter::MeetsConstraint(double aDuration) const
{
   if (mMinimum > 0.0 && aDuration < mMinimum)
   {
      return false;
   }
   if (mMaximum > 0.0 && aDuration > mMaximum)
   {
      return false;
   }
   return true;
}

bool IntervalConstraint::IsPlatformOfType(const std::string& aPlatformName, const std::string& aPlatformType) const
{
   bool retval{false};
   if (mCoveragePtr)
   {
      retval = mCoveragePtr->IsPlatformOfType(aPlatformName, aPlatformType);
   }
   return retval;
}

bool IntervalConstraint::IsPlatformDeviceOfType(const std::string& aPlatformName,
                                                const std::string& aDeviceName,
                                                const std::string& aDeviceType) const
{
   bool retval{false};
   if (mCoveragePtr)
   {
      retval = mCoveragePtr->IsPlatformDeviceOfType(aPlatformName, aDeviceName, aDeviceType);
   }
   return retval;
}

} // namespace coverage
} // namespace wsf
