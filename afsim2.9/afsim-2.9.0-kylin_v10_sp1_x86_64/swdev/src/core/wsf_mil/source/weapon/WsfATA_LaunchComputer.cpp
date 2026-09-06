// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfATA_LaunchComputer.hpp"

#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"

WsfATA_LaunchComputer::WsfATA_LaunchComputer()
   : WsfLaunchComputer()
   , mTableShotAspectIndex(0)
   , mTargetAzIndex(0)
   , mTargetElIndex(0)
   , mRangeRateIndex(0)
   , mRangeIndex(0)
   , mIterIndex(0)
   , mTableShotAspects()
   , mTableTargetAzimuths()
   , mTableTargetElevations()
   , mTableRangeRates()
   , mTableRanges()
{
}

WsfATA_LaunchComputer::WsfATA_LaunchComputer(const WsfATA_LaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mTableShotAspectIndex(aSrc.mTableShotAspectIndex)
   , mTargetAzIndex(aSrc.mTargetAzIndex)
   , mTargetElIndex(aSrc.mTargetElIndex)
   , mRangeRateIndex(aSrc.mRangeRateIndex)
   , mRangeIndex(aSrc.mRangeIndex)
   , mIterIndex(aSrc.mIterIndex)
   , mTableShotAspects(aSrc.mTableShotAspects)
   , mTableTargetAzimuths(aSrc.mTableTargetAzimuths)
   , mTableTargetElevations(aSrc.mTableTargetElevations)
   , mTableRangeRates(aSrc.mTableRangeRates)
   , mTableRanges(aSrc.mTableRanges)
   , mTOFResults(aSrc.mTOFResults)
{
}

WsfATA_LaunchComputer::~WsfATA_LaunchComputer() {}

WsfLaunchComputer* WsfATA_LaunchComputer::Clone() const
{
   return new WsfATA_LaunchComputer(*this);
}

// virtual
bool WsfATA_LaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool result = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   return result && CheckRanges();
}

// Base method is not needed, so re-implement a trivial one.
// protected virtual
bool WsfATA_LaunchComputer::InitializeTTIData()
{
   return true;
}

// virtual
bool WsfATA_LaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "intercept_results")
   {
      UtInputBlock inputBlock(aInput);
      ProcessInterceptBlock(inputBlock);
   }
   else if (command == "generate_rectangular_results")
   {
      UtInputBlock inputBlock(aInput);
      ProcessGenerateBlock(inputBlock);
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }

   return myCommand;
}

//! Routine processes reading in the resulting Time Of Flight values
//! given between the "begin" and "end" block qualifiers.
// protected
void WsfATA_LaunchComputer::ProcessInterceptBlock(UtInputBlock& aInputBlock)
{
   while (aInputBlock.ReadCommand())
   {
      UtInput&    aInput = aInputBlock.GetInput();
      std::string command(aInput.GetCommand());

      if (command == "number_of_aspect_angle_bins")
      {
         // Aspect angle has to cover zero to 180 degrees approach angle,
         // and results are assumed symmetric about Left and Right,
         // so the only question is how many bins (sets the resolution).
         int newNumBins = 0;
         aInput.ReadValue(newNumBins);
         aInput.ValueGreater<double>(newNumBins, 1.5);
         mTableShotAspects.SetNumBins(newNumBins);
         mTableShotAspects.SetMin(0.0);
         // (newNumBins is known to be > 0 because of ValueGreater check above)
         // cppcheck-suppress zerodiv
         mTableShotAspects.SetDelta(UtMath::cPI / newNumBins);
      }
      else if (command == "azimuth_bin_count_and_limit")
      {
         int    count;
         double limit;
         aInput.ReadValue(count);
         aInput.ValueGreater<double>(count, 1.5);
         aInput.ReadValueOfType(limit, UtInput::cANGLE);
         aInput.ValueGreater(limit, 0.0);
         mTableTargetAzimuths.SetNumBins(count);
         mTableTargetAzimuths.SetMin(0.0);
         mTableTargetAzimuths.SetDelta(limit / count);
      }
      else if (command == "elevation_bin_count_min_and_max_limits")
      {
         int    count;
         double lowerLimit;
         double upperLimit;
         aInput.ReadValue(count);
         aInput.ValueGreater<double>(count, 1.5);
         aInput.ReadValueOfType(lowerLimit, UtInput::cANGLE);
         aInput.ReadValueOfType(upperLimit, UtInput::cANGLE);
         aInput.ValueGreater(upperLimit, lowerLimit);
         mTableTargetElevations.SetNumBins(count);
         mTableTargetElevations.SetMin(lowerLimit);
         double delta = (upperLimit - lowerLimit) / count;
         mTableTargetElevations.SetDelta(delta);
      }
      else if (command == "range_rate_bin_count_min_and_max_limits")
      {
         int    count;
         double lowerLimit;
         double upperLimit;
         aInput.ReadValue(count);
         aInput.ValueGreater<double>(count, 1.5);
         aInput.ReadValueOfType(lowerLimit, UtInput::cSPEED);
         aInput.ReadValueOfType(upperLimit, UtInput::cSPEED);
         aInput.ValueGreater(upperLimit, lowerLimit);
         mTableRangeRates.SetNumBins(count);
         mTableRangeRates.SetMin(lowerLimit);
         double delta = (upperLimit - lowerLimit) / count;
         mTableRangeRates.SetDelta(delta);
      }
      else if (command == "firing_range_bin_count_and_values")
      {
         int newNumBins = 0;
         aInput.ReadValue(newNumBins);
         aInput.ValueGreater<double>(newNumBins, 1.5);
         mTableRanges.SetNumBins(newNumBins);
         double       value;
         unsigned int i = 0;
         for (; i < (mTableRanges.GetNumBins() + 1); ++i)
         {
            aInput.ReadValueOfType(value, UtInput::cLENGTH);
            if (!mTableRanges.SetValue(i, value))
            {
               throw UtInput::BadValue(aInput, "Inappropriate Table Range Value.");
            }
         }
      }
      else if (command == "time_of_flight_values")
      {
         bool finalValueIsRead = false;
         ResetAllIndices();
         AllocateResults(SizeOfResults());
         UtInputBlock inputBlock(aInput);
         while (inputBlock.ReadCommand())
         {
            UtInput& aInput2 = aInputBlock.GetInput();
            if (finalValueIsRead)
            {
               throw UtInput::BadValue(aInput2, "Too many boolean table values specified.");
            }
            // I'm not sure why, but both the following lines seem to be required.
            std::string newCommand(aInput2.GetCommand());
            aInput2.PushBack(newCommand);
            double result;
            aInput2.ReadValue(result);
            SetResult(result);
            if (!IncrementRange())
            {
               finalValueIsRead = true;
            }
         }
         if (!finalValueIsRead)
         {
            throw UtInput::BadValue(inputBlock.GetInput(), "Too few boolean values in results input block.");
         }
      }
   }
}

//! Routine processes reading in the engagement condition values
//! given between the "begin" and "end" block qualifiers.
// protected
void WsfATA_LaunchComputer::ProcessGenerateBlock(UtInputBlock& aInputBlock)
{
   int aspectBinCount = 4;

   int    azimuthBinCount = 4;
   double azimuthLimit    = 90.0 * UtMath::cRAD_PER_DEG;

   int    elevationBinCount   = 4;
   double elevationUpperLimit = 45.0 * UtMath::cRAD_PER_DEG;
   double elevationLowerLimit = -45.0 * UtMath::cRAD_PER_DEG;

   int    rangeRateBinCount   = 4;
   double rangeRateUpperLimit = 1000.0 * UtMath::cM_PER_FT;  // 1000 fps approx 1.0 Mach
   double rangeRateLowerLimit = -1000.0 * UtMath::cM_PER_FT; // 1000 fps approx 1.0 Mach

   int    rangeBinCount   = 4;
   double rangeUpperLimit = 40.0 * UtMath::cM_PER_NM;
   double rangeLowerLimit = 0.0;

   while (aInputBlock.ReadCommand())
   {
      UtInput&    input = aInputBlock.GetInput();
      std::string command(input.GetCommand());

      if (command == "aspect_angle_bin_count")
      {
         // Aspect angle has to cover zero to 180 degrees approach angle,
         // and results are assumed symmetric about Left and Right,
         // so the only question is how many bins (sets the resolution).
         input.ReadValue(aspectBinCount);
      }
      else if (command == "azimuth_bin_count")
      {
         input.ReadValue(azimuthBinCount);
      }
      else if (command == "azimuth_limit")
      {
         input.ReadValueOfType(azimuthLimit, UtInput::cANGLE);
      }
      else if (command == "elevation_bin_count")
      {
         input.ReadValue(elevationBinCount);
      }
      else if (command == "elevation_lower_limit")
      {
         input.ReadValueOfType(elevationLowerLimit, UtInput::cANGLE);
      }
      else if (command == "elevation_upper_limit")
      {
         input.ReadValueOfType(elevationUpperLimit, UtInput::cANGLE);
      }
      else if (command == "range_rate_bin_count")
      {
         input.ReadValue(rangeRateBinCount);
      }
      else if (command == "range_rate_lower_limit")
      {
         input.ReadValueOfType(rangeRateLowerLimit, UtInput::cSPEED);
      }
      else if (command == "range_rate_upper_limit")
      {
         input.ReadValueOfType(rangeRateUpperLimit, UtInput::cSPEED);
      }
      else if (command == "range_bin_count")
      {
         input.ReadValue(rangeBinCount);
      }
      else if (command == "minimum_ground_range")
      {
         input.ReadValueOfType(rangeLowerLimit, UtInput::cLENGTH);
      }
      else if (command == "maximum_ground_range")
      {
         input.ReadValueOfType(rangeUpperLimit, UtInput::cLENGTH);
      }
   }

   bool isError = false;

   if (aspectBinCount < 3)
   {
      auto out = ut::log::error() << "'aspect_angle_bin_count' must be 3 or greater.";
      out.AddNote() << "Current: " << aspectBinCount;
      isError = true;
   }

   if (azimuthBinCount < 3)
   {
      auto out = ut::log::error() << "'azimuth_bin_count' must be 3 or greater.";
      out.AddNote() << "Current: " << azimuthBinCount;
      isError = true;
   }

   if (elevationBinCount < 3)
   {
      auto out = ut::log::error() << "'elevation_bin_count' must be 3 or greater.";
      out.AddNote() << "Current: " << elevationBinCount;
      isError = true;
   }

   if (rangeRateBinCount < 3)
   {
      auto out = ut::log::error() << "'range_rate_bin_count' must be 3 or greater.";
      out.AddNote() << "Current: " << rangeRateBinCount;
      isError = true;
   }

   if (rangeBinCount < 3)
   {
      auto out = ut::log::error() << "'range_bin_count' must be 3 or greater.";
      out.AddNote() << "Current: " << rangeBinCount;
      isError = true;
   }

   if (azimuthLimit <= 0.0)
   {
      auto out = ut::log::error() << "'azimuth_limit' must be greater than zero.";
      out.AddNote() << "Current: " << azimuthLimit;
      isError = true;
   }

   if (elevationUpperLimit <= elevationLowerLimit)
   {
      auto out = ut::log::error() << "'elevation_upper_limit' must be set greater than 'elevation_lower_limit'.";
      out.AddNote() << "Upper Limit: " << elevationUpperLimit;
      out.AddNote() << "Lower Limit: " << elevationLowerLimit;
      isError = true;
   }

   if (rangeUpperLimit <= rangeLowerLimit)
   {
      auto out = ut::log::error() << "'range_upper_limit' must be set greater than 'range_lower_limit'.";
      out.AddNote() << "Upper Limit: " << rangeUpperLimit;
      out.AddNote() << "Lower Limit: " << rangeLowerLimit;
      isError = true;
   }

   if (rangeRateUpperLimit <= rangeRateLowerLimit)
   {
      auto out = ut::log::error() << "'range_rate_upper_limit' must be set greater than 'range_rate_lower_limit'.";
      out.AddNote() << "Upper Limit: " << rangeRateUpperLimit;
      out.AddNote() << "Lower Limit: " << rangeRateLowerLimit;
      isError = true;
   }

   if (isError)
   {
      throw UtInput::BadValue(aInputBlock.GetInput(), "Check all inputs for consistency.");
   }

   mTableShotAspects.SetNumBins(aspectBinCount);
   mTableShotAspects.SetMin(0.0);
   mTableShotAspects.SetDelta(UtMath::cPI / aspectBinCount);

   mTableTargetAzimuths.SetNumBins(azimuthBinCount);
   mTableTargetAzimuths.SetMin(0.0);
   mTableTargetAzimuths.SetDelta(azimuthLimit / azimuthBinCount);

   mTableTargetElevations.SetNumBins(elevationBinCount);
   mTableTargetElevations.SetMin(elevationLowerLimit);
   double delta = (elevationUpperLimit - elevationLowerLimit) / elevationBinCount;
   mTableTargetElevations.SetDelta(delta);

   mTableRangeRates.SetMin(rangeRateLowerLimit);
   delta = (rangeRateUpperLimit - rangeRateLowerLimit) / rangeRateBinCount;
   mTableRangeRates.SetDelta(delta);
   mTableRangeRates.SetNumBins(rangeRateBinCount);

   // Here we set an EVEN-INTERVAL set of ranges, for simplicity
   // (The model does not require that the range intervals be even...)
   delta = (rangeUpperLimit - rangeLowerLimit) / rangeBinCount;
   mTableRanges.SetNumBins(rangeBinCount);
   unsigned int i = 0;
   for (; i < (mTableRanges.GetNumBins() + 1); ++i)
   {
      mTableRanges.SetValue(i, rangeLowerLimit + i * delta);
   }
}

// Implementation of base class method.
// virtual
double WsfATA_LaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   InitializeResults(aSimTime, aTrack);

   // Let's assume that two iterations are required to converge on
   // an estimated time to intercept.  Make an assumption to begin
   // with, of ten seconds.  Then get a new TTI, and re-check constraints.
   double estimatedTTI = 10.0; // We've got to start with SOME assumption!

   // FIRST ITERATION ONLY:
   bool   inConstraints = WithinConstraintsAt(aSimTime, aSimTime + aLaunchDelayTime + estimatedTTI, aTrack);
   double range         = ExpectedFlightDistance();
   double rangeRate     = CurrentOpeningSpeed();
   bool   done          = false;
   int    loopCount     = 0;

   while (!done)
   {
      ++loopCount;
      if (loopCount > 2)
         done = true;
      if (aTrack.VelocityValid())
      {
         double futureTgtLocWCS[3];
         if (!aTrack.GetExtrapolatedLocationWCS(aSimTime + aLaunchDelayTime + estimatedTTI, futureTgtLocWCS))
         {
            return estimatedTTI;
         }

         double trackLatDeg;
         double trackLonDeg;
         double trackAlt;
         UtEntity::ConvertWCSToLLA(futureTgtLocWCS, trackLatDeg, trackLonDeg, trackAlt);

         double launchLatDeg;
         double launchLonDeg;
         double launchAlt;
         GetPlatform()->GetLocationLLA(launchLatDeg, launchLonDeg, launchAlt);
         double shotHeadingDegrees;
         double shotDistance;
         UtSphericalEarth::GreatCircleHeadingAndDistance(launchLatDeg,
                                                         launchLonDeg, // inputs
                                                         trackLatDeg,
                                                         trackLonDeg, // inputs
                                                         shotHeadingDegrees,
                                                         shotDistance); // outputs
         double targetVelWCS[3];
         aTrack.GetVelocityWCS(targetVelWCS);
         double targetVelNED[3];
         GetPlatform()->ConvertWCSVectorToNED(targetVelNED, targetVelWCS);
         double targetHeading = atan2(targetVelNED[1], targetVelNED[0]);

         // We assume results are symmetric regardless of left or right aspect angles.
         double shotAspect = fabs(
            UtMath::NormalizeAngleMinusPi_Pi(shotHeadingDegrees * UtMath::cRAD_PER_DEG + UtMath::cPI - targetHeading));

         double heading;
         double pitch;
         double roll;
         GetPlatform()->GetOrientationNED(heading, pitch, roll);
         // We also assume results are symmetric regardless of left or right azimuth angles.
         double tgtAzimuth = fabs(UtMath::NormalizeAngleMinusPi_Pi(shotHeadingDegrees * UtMath::cRAD_PER_DEG - heading));

         double nedOffset[3] = {0.0};
         GetPlatform()->ConvertWCSToNED(futureTgtLocWCS, nedOffset);
         double tgtElevation = -asin(nedOffset[0] / range);
         estimatedTTI        = GetTOF(shotAspect, tgtAzimuth, tgtElevation, rangeRate, range);

         auto out = ut::log::debug();
         if (estimatedTTI == 0.0)
         {
            if (DebugEnabled())
            {
               out << "Estimated TTI == 0.0, so failed test.";
            }
         }
         else
         {
            inConstraints = WithinConstraintsAt(aSimTime, aSimTime + aLaunchDelayTime + estimatedTTI, aTrack);
            if (DebugEnabled())
            {
               if (inConstraints)
               {
                  out << "Estimated TTI is non-zero, and passed constraints.";
               }
               else
               {
                  out << "Estimated TTI is non-zero, but failed constraints.";
               }
            }
         }
      }
   }

   if ((estimatedTTI == 0.0) || (!inConstraints))
   {
      // Results not valid, so indicate an infinite TTI:
      estimatedTTI = cFOREVER;
   }
   UpdateResults(aSimTime, estimatedTTI, aTrack);

   return estimatedTTI;
}

//! For the given engagement conditions, do a table lookup, and get the expected Time Of Flight (TOF)
//! for the weapon. A zero return indicates the engagement is kinematically impossible for the weapon.
//!    @return Expected Time Of Flight.  Zero indicates no intercept is possible.
double WsfATA_LaunchComputer::GetTOF(double aAspect, double aAzimuth, double aElevation, double aRangeRate, double aRange)
{
   double result = 0.0;
   bool   ok     = true;
   ok &= mTableShotAspects.GetIndex(aAspect, mTableShotAspectIndex);
   ok &= mTableTargetAzimuths.GetIndex(aAzimuth, mTargetAzIndex);
   ok &= mTableTargetElevations.GetIndex(aElevation, mTargetElIndex);
   ok &= mTableRangeRates.GetIndex(aRangeRate, mRangeRateIndex);
   ok &= mTableRanges.GetIndex(aRange, mRangeIndex);
   auto out = ut::log::debug();
   if (ok)
   {
      SetIterIndex();
      result = mTOFResults[mIterIndex];
      if (DebugEnabled())
      {
         if (result == 0.0)
         {
            out << "GetTOF table lookup result was zero.";
         }
         else
         {
            out << "Success: GetTOF table lookup result:";
            out.AddNote() << "Result: " << result << " sec.";
         }
      }
   }
   else if (DebugEnabled())
   {
      out << "Table Lookup failed because it was outside of table bounds.";
   }
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
unsigned int WsfATA_LaunchComputer::SizeOfAspectsSet() const
{
   return mTableShotAspects.GetNumBins() * SizeOfAzimuthsSet();
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
unsigned int WsfATA_LaunchComputer::SizeOfAzimuthsSet() const
{
   return mTableTargetAzimuths.GetNumBins() * SizeOfElevationsSet();
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
unsigned int WsfATA_LaunchComputer::SizeOfElevationsSet() const
{
   return mTableTargetElevations.GetNumBins() * SizeOfRangeRatesSet();
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
unsigned int WsfATA_LaunchComputer::SizeOfRangeRatesSet() const
{
   return mTableRangeRates.GetNumBins() * mTableRanges.GetNumBins();
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
unsigned int WsfATA_LaunchComputer::SizeOfRangeBinsSet() const
{
   return mTableRanges.GetNumBins();
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
double WsfATA_LaunchComputer::TableTargetRange() const
{
   double result = 0.0;
   mTableRanges.BinCenter(mRangeIndex, result);
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
double WsfATA_LaunchComputer::TableTargetRangeRate() const
{
   double result = 0.0;
   mTableRangeRates.BinCenter(mRangeRateIndex, result);
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
double WsfATA_LaunchComputer::TableTargetAspect() const
{
   double result = 0.0;
   mTableShotAspects.BinCenter(mTableShotAspectIndex, result);
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
double WsfATA_LaunchComputer::TableTargetAzimuth() const
{
   double result = 0.0;
   mTableTargetAzimuths.BinCenter(mTargetAzIndex, result);
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
double WsfATA_LaunchComputer::TableTargetElevation() const
{
   double result = 0.0;
   mTableTargetElevations.BinCenter(mTargetElIndex, result);
   return result;
}

//! Properly size vector<double> to hold the values for Times Of Flight (TOF) of the weapon
//! and initializes each to zero.
//! Used by the Weapon_Tool during the Launch Computer Generation process.
void WsfATA_LaunchComputer::AllocateResults(unsigned int aSize)
{
   assert(aSize > 0);
   mTOFResults.assign(aSize, 0.0);
}

//! Assures that ranges are monotonically increasing.
//!   @return True if increasing.  False if not.
bool WsfATA_LaunchComputer::CheckRanges()
{
   bool goodValues = true;
   for (unsigned int i = 0; i != mTableRanges.GetNumBins(); ++i)
   {
      double lower = 0.0;
      double upper = 0.0;
      goodValues &= mTableRanges.BinLower(i, lower);
      if (goodValues)
      {
         goodValues &= mTableRanges.BinUpper(i, upper);
      }
      if (lower >= upper)
      {
         goodValues = false;
         break;
      }
   }
   if (!goodValues)
   {
      auto out = ut::log::error() << "Launch computer Range values must be continuously increasing.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   return goodValues;
}

//! Set the index to enter the Times Of Flight array, and retrieve a value.
//! Used by the Weapon_Tool during the Launch Computer Generation process.
void WsfATA_LaunchComputer::SetIterIndex()
{
   mIterIndex = mTableShotAspectIndex * SizeOfAzimuthsSet() + mTargetAzIndex * SizeOfElevationsSet() +
                mTargetElIndex * SizeOfRangeRatesSet() + mRangeRateIndex * mTableRanges.GetNumBins() + mRangeIndex;
}

//! Set the actual resulting Time Of Flight (TOF) for the current TOF index value.
//! Used by the Weapon_Tool during the Launch Computer Generation process.
void WsfATA_LaunchComputer::SetResult(double aValue)
{
   mTOFResults[mIterIndex] = aValue;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::IncrementRange()
{
   bool result = true;
   ++mRangeIndex;
   if (mRangeIndex == mTableRanges.GetNumBins())
   {
      mRangeIndex = 0;
      result      = IncrementRangeRate();
   }
   assert(mRangeIndex < mTableRanges.GetNumBins());
   SetIterIndex();
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::IncrementRangeRate()
{
   bool result = true;
   ++mRangeRateIndex;
   if (mRangeRateIndex == mTableRangeRates.GetNumBins())
   {
      mRangeRateIndex = 0;
      result          = IncrementTargetElevation();
   }
   assert(mRangeRateIndex < mTableRangeRates.GetNumBins());
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::IncrementTargetElevation()
{
   bool result = true;
   ++mTargetElIndex;
   if (mTargetElIndex == mTableTargetElevations.GetNumBins())
   {
      mTargetElIndex = 0;
      result         = IncrementTargetAzimuth();
   }
   assert(mTargetElIndex < mTableTargetElevations.GetNumBins());
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::IncrementTargetAzimuth()
{
   bool result = true;
   ++mTargetAzIndex;
   if (mTargetAzIndex == mTableTargetAzimuths.GetNumBins())
   {
      mTargetAzIndex = 0;
      result         = IncrementShotAspect();
   }
   assert(mTargetAzIndex < mTableTargetAzimuths.GetNumBins());
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::IncrementShotAspect()
{
   bool result = true;
   ++mTableShotAspectIndex;
   if (mTableShotAspectIndex == mTableShotAspects.GetNumBins())
   {
      mTableShotAspectIndex = 0;
      result                = false;
   }
   assert(mTableShotAspectIndex < mTableShotAspects.GetNumBins());
   return result;
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::TableTargetAzimuth(unsigned int aIndex, double& aValue) const
{
   return mTableTargetAzimuths.BinCenter(aIndex, aValue);
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::TableTargetElevation(unsigned int aIndex, double& aValue) const
{
   return mTableTargetElevations.BinCenter(aIndex, aValue);
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::TableTargetRangeRate(unsigned int aIndex, double& aValue) const
{
   return mTableRangeRates.BinCenter(aIndex, aValue);
}

//! Used by the Weapon_Tool during the Launch Computer Generation process.
bool WsfATA_LaunchComputer::TableTargetRelHeading(unsigned int aIndex, double& aValue) const
{
   return mTableShotAspects.BinCenter(aIndex, aValue);
}

//! Reset index values to zero.  Used prior to a full traversal of all engagement conditions.
//! Used by the Weapon_Tool during the Launch Computer Generation process.
void WsfATA_LaunchComputer::ResetAllIndices()
{
   mTableShotAspectIndex = 0;
   mTargetAzIndex        = 0;
   mTargetElIndex        = 0;
   mRangeRateIndex       = 0;
   mRangeIndex           = 0;
}

//! For use by the weapon_tools.  Assures that selected ranges are monotonically increasing,
//! which is required for proper table lookups.
bool WsfATA_LaunchComputer::ToolOnlyInitialize(double aSimTime)
{
   return CheckRanges();
}

// ********************************************************************************

//! Get the floating point value that is half way between the upper and lower bound of this "bin" index.
bool WsfATA_LaunchComputer::EvenlySpaced::BinCenter(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex <= (mBins - 1))
   {
      aValue = mMin + aIndex * mDelta + mDelta / 2.0;
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the floating point value that is the lower bound of this "bin" index.
bool WsfATA_LaunchComputer::EvenlySpaced::BinLower(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex <= (mBins - 1))
   {
      aValue = mMin + aIndex * mDelta;
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the floating point value that is the upper bound of this "bin" index.
bool WsfATA_LaunchComputer::EvenlySpaced::BinUpper(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex <= (mBins - 1))
   {
      aValue = mMin + (aIndex + 1) * mDelta;
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the "bin" index for a floating point value within its range.
//!  @return False if outside the valid range of floating point values.
bool WsfATA_LaunchComputer::EvenlySpaced::GetIndex(double aValue, unsigned int& aIndex) const
{
   bool success = true;
   if (aValue < mMin)
   {
      success = false;
   }
   else if (aValue > Max())
   {
      success = false;
   }
   else
   {
      aIndex = 0;
      while (aValue > (mMin + (aIndex + 1) * mDelta))
      {
         ++aIndex;
      }
   }
   return success;
}

// ********************************************************************************

//! Get the "bin" index for a floating point value within its range.
//!  @return False if outside the valid range of floating point values.
bool WsfATA_LaunchComputer::NotEvenlySpaced::GetIndex(double aValue, unsigned int& aIndex) const
{
   bool success = true;
   if (aValue < mValues[0])
   {
      success = false;
   }
   else if (aValue > mValues[mValues.size() - 1])
   {
      success = false;
   }
   else
   {
      aIndex = 0;
      while (aValue > (mValues[aIndex + 1]))
      {
         ++aIndex;
      }
   }
   return success;
}

//! Set the floating point value that is the lower bound of indicated "bin" index.
bool WsfATA_LaunchComputer::NotEvenlySpaced::SetValue(unsigned int aIndex, double aValue)
{
   bool success = true;
   if (aIndex < mValues.size())
   {
      mValues[aIndex] = aValue;
      // In the future, may want to do a validity check for monotonically increasing.
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the floating point value that is the difference of the Upper minus the Lower bound of this bin.
bool WsfATA_LaunchComputer::NotEvenlySpaced::Delta(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex < (mValues.size() - 1))
   {
      aValue = mValues[aIndex + 1] - mValues[aIndex];
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the floating point value that is half way between the upper and lower bound of this "bin" index.
bool WsfATA_LaunchComputer::NotEvenlySpaced::BinCenter(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex < (mValues.size() - 1))
   {
      aValue = mValues[aIndex] + (mValues[aIndex + 1] - mValues[aIndex]) / 2.0;
   }
   else
   {
      success = false;
   }
   return success;
}

//! Get the floating point value that is the lower bound of this "bin" index.
bool WsfATA_LaunchComputer::NotEvenlySpaced::BinLower(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (mValues.size() <= aIndex)
   {
      success = false;
   }
   else
   {
      aValue = mValues[aIndex];
   }
   return success;
}

//! Get the floating point value that is the upper bound of this "bin" index.
bool WsfATA_LaunchComputer::NotEvenlySpaced::BinUpper(unsigned int aIndex, double& aValue) const
{
   bool success = true;
   if (aIndex < (mValues.size() - 1))
   {
      aValue = mValues[aIndex + 1];
   }
   else
   {
      success = false;
   }
   return success;
}
