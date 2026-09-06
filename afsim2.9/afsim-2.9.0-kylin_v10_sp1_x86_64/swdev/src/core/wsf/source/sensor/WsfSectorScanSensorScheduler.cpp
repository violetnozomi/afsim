// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSectorScanSensorScheduler.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSimulation.hpp"

const double WsfSectorScanSensorScheduler::Sector::cUNINITIALIZED = -999.0;

namespace
{
// This constant is used instead of UtMath::cPI (positive value), so that when renormalized
// in UtMath::NormalizeAngleMinusPi_Pi, it does not become negative pi.
const double sReducedPI = std::nextafter(UtMath::cPI, 0.0);
} // namespace

// This class was developed in order to schedule targets to be searched based on their physical location
// relative to the sensor.

WsfSectorScanSensorScheduler::WsfSectorScanSensorScheduler()
   : WsfSensorScheduler()
   , mModeList()
   , mLastExplicitModeIndex(0)
   , mSearchModeIndex(0)
   , mSearchAllowed(true)
   , mSensorTargetQueue()
   , mSensorUpdateInterval(0.0)
   , mLastUpdateTime(0.0)
   , mSlewingToNextSector(false)
   , mIsContinuousAz(false)
   , mFrameBasedScheduling(true)
   , mSlewTransition()
   , mCurrentSectorPtr(nullptr)
   , mSectors()
   , mSectorIter()
   , mNextSectorIter()
   , mTargetsSelectedThisFrame()
{
   mSlewTransition.SetType(Sector::cAZEL);
}

WsfSectorScanSensorScheduler::WsfSectorScanSensorScheduler(const WsfSectorScanSensorScheduler& aSrc)
   : WsfSensorScheduler(aSrc)
   , mModeList()
   , mLastExplicitModeIndex(aSrc.mLastExplicitModeIndex)
   , mSearchModeIndex(aSrc.mSearchModeIndex)
   , mSearchAllowed(aSrc.mSearchAllowed)
   , mSensorTargetQueue()
   , mSensorUpdateInterval(aSrc.mSensorUpdateInterval)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mSlewingToNextSector(false)
   , mIsContinuousAz(aSrc.mIsContinuousAz)
   , mFrameBasedScheduling(aSrc.mFrameBasedScheduling)
   , mSlewTransition(aSrc.mSlewTransition)
   , mCurrentSectorPtr(nullptr)
   , mSectors(aSrc.mSectors)
   , mSectorIter(aSrc.mSectorIter)
   , mNextSectorIter(aSrc.mNextSectorIter)
   , mTargetsSelectedThisFrame()
{
}

//! Factory method for WsfSensorSchedulerTypes to determine if a scheduler
//! represented by this class is being requested.
// static
//  =================================================================================================
std::unique_ptr<WsfSensorScheduler> WsfSectorScanSensorScheduler::ObjectFactory(const std::string& aTypeName)
{
   if (aTypeName == "sector_scan")
   {
      return ut::make_unique<WsfSectorScanSensorScheduler>();
   }
   return nullptr;
}

// virtual
WsfSensorScheduler* WsfSectorScanSensorScheduler::Clone() const
{
   return new WsfSectorScanSensorScheduler(*this);
}

bool WsfSectorScanSensorScheduler::Sector::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "type")
   {
      aInput.ReadCommand(command);
      if (command == "azimuth")
      {
         mType = cAZ;
      }
      else if (command == "elevation")
      {
         mType = cEL;
      }
      else if (command == "azimuth_and_elevation")
      {
         mType = cAZEL;
      }
      else
      {
         myCommand = false;
      }
   }
   else if (command == "start_azimuth")
   {
      aInput.ReadValueOfType(mStartAz, UtInput::cANGLE);
   }
   else if (command == "end_azimuth")
   {
      aInput.ReadValueOfType(mEndAz, UtInput::cANGLE);
   }
   else if (command == "azimuth_scan_direction")
   {
      std::string direction;
      aInput.ReadValue(direction);
      if (direction == "positive")
      {
         mAzScanDir = WsfArticulatedPart::cPOSITIVE;
      }
      else if (direction == "negative")
      {
         mAzScanDir = WsfArticulatedPart::cNEGATIVE;
      }
      else
      {
         throw(UtInput::UnknownCommand(aInput));
      }
   }
   else if (command == "elevation")
   {
      aInput.ReadValueOfType(mEndEl, UtInput::cANGLE);
      mStartEl = mEndEl;
   }
   else if (command == "azimuth")
   {
      aInput.ReadValueOfType(mEndAz, UtInput::cANGLE);
      mStartAz = mEndAz;
   }
   else if (command == "start_elevation")
   {
      aInput.ReadValueOfType(mStartEl, UtInput::cANGLE);
      UtMath::Limit(mStartEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "end_elevation")
   {
      aInput.ReadValueOfType(mEndEl, UtInput::cANGLE);
      UtMath::Limit(mEndEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "azimuth_rate")
   {
      aInput.ReadValueOfType(mAzScanRate, UtInput::cANGULAR_RATE);
   }
   else if (command == "elevation_rate")
   {
      aInput.ReadValueOfType(mElScanRate, UtInput::cANGULAR_RATE);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfSectorScanSensorScheduler::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;
   if (command == "sector")
   {
      Sector sector;

      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         if (!sector.ProcessInput(block.GetInput()))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (!sector.Validate())
      {
         ut::log::warning() << "Insufficient data in sector definition.";
         throw(UtInput::EndOfData(aInput));
      }
      mSectors.push_back(sector);
   }
   else if (command == "frame_based_scheduling")
   {
      aInput.ReadValue(mFrameBasedScheduling);
   }
   else
   {
      myCommand = WsfSensorScheduler::ProcessInput(aInput);
   }

   return myCommand;
}

WsfSectorScanSensorScheduler::Sector::Sector()
   : mType(cUNDEFINED)
   , mAzScanDir(WsfArticulatedPart::cUNDEFINED)
   , mElScanDir(WsfArticulatedPart::cUNDEFINED)
   , mStartAz(cUNINITIALIZED)
   , mEndAz(cUNINITIALIZED)
   , mStartEl(cUNINITIALIZED)
   , mEndEl(cUNINITIALIZED)
   , mAzScanRate(cUNINITIALIZED)
   , mElScanRate(cUNINITIALIZED)
   , mScanTime(cUNINITIALIZED)
{
}

//! Initialize the sector.  It is assumed that data are already valid (Validate() returns true).
bool WsfSectorScanSensorScheduler::Sector::Initialize(WsfSensor& aSensor)
{
   bool   ok          = true;
   double azFrameTime = 0;
   double elFrameTime = 0;

   // Verify Limits
   ok &= aSensor.WithinSlewLimits(mEndAz,
                                  mEndEl); // always true because we define the fixed azimuth/elevation to be the end value.
   if (mType == cAZ)
   {
      ok &= aSensor.WithinSlewLimits(mStartAz, mEndEl); // Test the start azimuth
   }
   else if (mType == cEL)
   {
      ok &= aSensor.WithinSlewLimits(mEndAz, mStartEl); // Test the start elevation
   }
   else // azimuth-elevation
   {
      ok &= aSensor.WithinSlewLimits(mStartAz, mStartEl); // Test both start azimuth and elevation
   }

   if (ok)
   {
      // Initialize direction and rate based on type.
      if ((mType == cAZ) || (mType == cAZEL))
      {
         if (mAzScanDir == WsfArticulatedPart::cUNDEFINED)
         {
            // Assume scan from start azimuth to end azimuth, not going through 180 degrees
            if (mEndAz > mStartAz)
            {
               mAzScanDir = WsfArticulatedPart::cPOSITIVE;
            }
            else
            {
               mAzScanDir = WsfArticulatedPart::cNEGATIVE;
            }
         }

         if (mAzScanRate == cUNINITIALIZED)
         {
            mAzScanRate = aSensor.GetMaxAzSlewRate();
            if (mAzScanRate >= WsfArticulatedPart::cINFINITE_SLEW_RATE)
            {
               ut::log::error() << "You must define an 'azimuth_rate' or the sensor's 'azimuth_slew_rate'.";
               ok = false;
            }
         }
         azFrameTime = fabs(mEndAz - mStartAz) / mAzScanRate;
      }
      else // type is cEL
      {
         mAzScanRate = 0.0;
      }

      if ((mType == cEL) || (mType == cAZEL))
      {
         if (mEndEl > mStartEl)
         {
            mElScanDir = WsfArticulatedPart::cPOSITIVE;
         }
         else
         {
            mElScanDir = WsfArticulatedPart::cNEGATIVE;
         }
         if (mElScanRate == cUNINITIALIZED)
         {
            mElScanRate = aSensor.GetMaxElSlewRate();
            if (mElScanRate >= WsfArticulatedPart::cINFINITE_SLEW_RATE)
            {
               ut::log::error() << "You must define an 'elevation_rate' or the sensor's 'elevation_slew_rate'.";
               ok = false;
            }
         }
         elFrameTime = fabs(mEndEl - mStartEl) / mElScanRate;
      }
      else // type is cAz
      {
         mElScanRate = 0.0;
      }
   }

   if (ok)
   {
      if (mType == cAZ)
      {
         mScanTime = azFrameTime;
      }
      else if (mType == cEL)
      {
         mScanTime = elFrameTime;
      }
      else // cAZ_EL
      {
         mScanTime = std::max(azFrameTime, elFrameTime);
      }
   }

   // Adjust and normalize the azimuth values now that the scan rates have been calculated
   if (mStartAz == UtMath::cPI)
   {
      mStartAz = sReducedPI;
   }
   if (mEndAz == UtMath::cPI)
   {
      mEndAz = sReducedPI;
   }

   mStartAz = UtMath::NormalizeAngleMinusPi_Pi(mStartAz);
   mEndAz   = UtMath::NormalizeAngleMinusPi_Pi(mEndAz);

   return ok;
}

// virtual
bool WsfSectorScanSensorScheduler::Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr)
{
   bool ok = WsfSensorScheduler::Initialize(aSimTime, aSensorPtr, aTrackerPtr);

   if (ok)
   {
      if (aSensorPtr->GetSlewMode() != WsfArticulatedPart::cSLEW_AZ_EL)
      {
         ut::log::error()
            << "Slew mode must be set to 'azimuth_and_elevation' or 'both' to use the sector_scan scheduler.";
         ok = false;
      }
   }

   if (ok)
   {
      Sectors::iterator iter = mSectors.begin();
      while (iter != mSectors.end())
      {
         Sector& sector = *iter;
         ok &= sector.Initialize(*aSensorPtr);
         if (!ok)
         {
            break;
         }
         ++iter;
      }

      if (ok)
      {
         // If this is an imaging sensor, try and detect targets every update interval;
         // otherwise only provide one detection chance every complete scan set.
         mFrameBasedScheduling &= !mSensorPtr->IsClassImaging();

         if (aSensorPtr->GetModeList() != nullptr)
         {
            // Reduce future dynamic casting by extracting derived class mode pointers.
            aSensorPtr->GetModeList()->GetDerivedModeList(mModeList);
            mLastExplicitModeIndex = static_cast<unsigned int>(mModeList.size());

            for (auto& modePtr : mModeList)
            {
               // The frame time might be used by other objects.
               // Set the frame time to be the update time.
               modePtr->SetFrameTime(aSensorPtr->GetUpdateInterval());
            }

            mSearchAllowed = (!mSensorPtr->GetCurrentMode()->DisablesSearch());
         }

         mSensorUpdateInterval = aSensorPtr->GetUpdateInterval();
         if (mSensorUpdateInterval == 0.0)
         {
            auto out = ut::log::error() << "'update_interval' is required for 'sector_scan' scheduler.";
            out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
            out.AddNote() << "Sensor: " << mSensorPtr->GetName();
            return false;
         }

         // Determine if this is sensor can potentially scan continuously in azimuth
         if ((mSensorPtr->GetMinAzSlew() == -UtMath::cPI) && (mSensorPtr->GetMaxAzSlew() == UtMath::cPI))
         {
            mIsContinuousAz = true;
         }

         // Note: Scanning is initialized druing the call to TurnOn().
      }
   }
   return ok;
}

//! Transition to the next scanning sector and begin scanning.
//! @param aSimTime The current simulation time.
void WsfSectorScanSensorScheduler::TransitionSector(double aSimTime)
{
   ++mSectorIter;
   ++mNextSectorIter;
   if (mSectorIter == mSectors.end())
   {
      mSectorIter = mSectors.begin();

      if (mFrameBasedScheduling)
      {
         mTargetsSelectedThisFrame.clear();
      }
   }
   if (mNextSectorIter == mSectors.end())
   {
      mNextSectorIter = mSectors.begin();
   }

   BeginSlew(aSimTime, *mSectorIter);
}

//! Transition the scan to the provided sector.
//! @param aSimTime The current simulation time.
//! @param aSector The next sector to scan.
void WsfSectorScanSensorScheduler::BeginSlew(double aSimTime, Sector& aSector)
{
   mCurrentSectorPtr = &aSector;
   mSensorPtr->SetAzimuthSlewDirection(mCurrentSectorPtr->GetAzSlewDirection());

   // Set the slew rate, rather than the cue rate, so that scripted cues will perform properly.
   mSensorPtr->SetSlewRates(mCurrentSectorPtr->GetAzScanRate(), mCurrentSectorPtr->GetElScanRate());

   // Set the cued orientation to begin slewing to the end of the sector.
   // This can be overridden by scripted cueing.
   mSensorPtr->SetCuedOrientation(mCurrentSectorPtr->GetEndAz(), mCurrentSectorPtr->GetEndEl());
}

//! Determine whether to transition the scan to the next sector.
//! @param aAz The current scan azimuth.
//! @param aEl The current scan elevation.
bool WsfSectorScanSensorScheduler::Sector::CheckTransition(double aAz, double aEl)
{
   bool doTransition = true;

   if ((mType == cAZ) || (mType == cAZEL))
   {
      doTransition &= UtMath::NearlyEqual(GetEndAz(), aAz, 1.0e-12);
   }
   if ((mType == cEL) || (mType == cAZEL))
   {
      doTransition &= (aEl == GetEndEl());
   }

   return doTransition;
}

//! Validate that all initial inputs were provided such that this sector is completely defined.
//! Initialize must also be called prior to use.
bool WsfSectorScanSensorScheduler::Sector::Validate() const
{
   bool isValid = (mType != cUNDEFINED);
   if (mType == cAZ)
   {
      isValid &= (mStartAz != cUNINITIALIZED) && (mEndAz != cUNINITIALIZED) && (mEndEl != cUNINITIALIZED);
   }
   else if (mType == cEL)
   {
      isValid &= (mStartEl != cUNINITIALIZED) && (mEndEl != cUNINITIALIZED) && (mEndAz != cUNINITIALIZED);
   }
   else if (mType == cAZEL)
   {
      isValid &= (mStartAz != cUNINITIALIZED) && (mEndAz != cUNINITIALIZED) && (mStartEl != cUNINITIALIZED) &&
                 (mEndEl != cUNINITIALIZED);
   }
   return isValid;
}

//! Advance the scan to the given time.
//! @param aSimTime The time to which the scan should be advanced.
void WsfSectorScanSensorScheduler::AdvanceScan(double aSimTime)
{
   mSensorPtr->UpdatePosition(aSimTime);

   // check for at cue limits.
   double az, el;
   mSensorPtr->GetCurrentCuedOrientation(az, el);

   bool doTransition = mCurrentSectorPtr->CheckTransition(az, el);
   if (doTransition)
   {
      if (mSlewingToNextSector)
      {
         mSlewingToNextSector = false;
         TransitionSector(aSimTime);
      }
      else
      {
         SlewToSector(aSimTime, *mNextSectorIter);
      }
   }
}

//! From the current cued orientation, slew to the start of the provided sector.
//! @param aSimTime The current simulation time.
//! @param aSector The sector to which we are slewing.
void WsfSectorScanSensorScheduler::SlewToSector(double aSimTime, Sector& aSector)
{
   mSlewingToNextSector = true;
   if (!mSensorTargetQueue.empty())
   {
      // Empty the target queue.
      SensorTargetQueue swapQueue;
      mSensorTargetQueue.swap(swapQueue);
   }
   double az, el;
   mSensorPtr->GetCurrentCuedOrientation(az, el);
   mSlewTransition.SetStartAz(az);
   mSlewTransition.SetEndAz(aSector.GetStartAz());
   mSlewTransition.SetStartEl(el);
   mSlewTransition.SetEndEl(aSector.GetStartEl());
   mSlewTransition.SetAzScanRate(mSensorPtr->GetMaxAzSlewRate());
   mSlewTransition.SetElScanRate(mSensorPtr->GetMaxElSlewRate());
   mSlewTransition.Initialize(*mSensorPtr);

   if (mIsContinuousAz) // Check to see if it's a faster/shorter path to go through 180 degrees
   {
      double deltaAz = aSector.GetStartAz() - az;
      if (fabs(deltaAz) > UtMath::cPI)
      {
         if (deltaAz > 0.0)
         {
            mSlewTransition.SetAzSlewDirection(WsfArticulatedPart::cNEGATIVE);
         }
         else
         {
            mSlewTransition.SetAzSlewDirection(WsfArticulatedPart::cPOSITIVE);
         }
      }
      else
      {
         if (deltaAz > 0.0)
         {
            mSlewTransition.SetAzSlewDirection(WsfArticulatedPart::cPOSITIVE);
         }
         else
         {
            mSlewTransition.SetAzSlewDirection(WsfArticulatedPart::cNEGATIVE);
         }
      }
   }

   BeginSlew(aSimTime, mSlewTransition);
}

//! Given a target, provide an approximate time that the sensor boresight will be as close as possible in
//! angle to it.
//!@param aSimTime The current simulation time.
//!@param aTarget The prospective target platform.
//!@return The estimated optimal detection time.
double WsfSectorScanSensorScheduler::EstimateDetectTime(double aSimTime, WsfPlatform& aTarget)
{
   // compute the az/el of the target wrt the sensor
   UtVec3d sensorLocWCS;
   mSensorPtr->GetLocationWCS(sensorLocWCS.GetData());
   UtVec3d targetLocWCS;
   aTarget.GetLocationWCS(targetLocWCS.GetData());
   UtVec3d relativeLocation;
   relativeLocation.Subtract(targetLocWCS, sensorLocWCS);
   double deltaAz, deltaEl;
   mSensorPtr->ComputeAspect(relativeLocation.GetData(), deltaAz, deltaEl);
   double detectTime = aSimTime;

   double deltaT_Az = 0.0;
   double deltaT_El = 0.0;

   if ((mCurrentSectorPtr->GetType() == Sector::cAZ) || (mCurrentSectorPtr->GetType() == Sector::cAZEL))
   {
      if (mCurrentSectorPtr->GetAzSlewDirection() == WsfArticulatedPart::cNEGATIVE)
      {
         deltaAz *= -1.0;
      }

      // Only provide a later time if the target is ahead in the scan; otherwise it might not get a detection chance.
      if (deltaAz > 0.0)
      {
         double cuedAz, cuedEl;
         mSensorPtr->GetCurrentCuedOrientation(cuedAz, cuedEl);
         deltaAz   = std::min(deltaAz, fabs(mCurrentSectorPtr->GetEndAz() - cuedAz));
         deltaT_Az = deltaAz / mCurrentSectorPtr->GetAzScanRate();
      }
   }
   if ((mCurrentSectorPtr->GetType() == Sector::cEL) || (mCurrentSectorPtr->GetType() == Sector::cAZEL))
   {
      if (mCurrentSectorPtr->GetElSlewDirection() == WsfArticulatedPart::cNEGATIVE)
      {
         deltaEl *= -1.0;
      }

      // Only provide a later time if the target is ahead in the scan; otherwise it might not get a detection chance.
      if (deltaEl > 0.0)
      {
         double cuedAz, cuedEl;
         mSensorPtr->GetCurrentCuedOrientation(cuedAz, cuedEl);
         deltaEl   = std::min(deltaEl, fabs(mCurrentSectorPtr->GetEndEl() - cuedEl));
         deltaT_El = deltaEl / mCurrentSectorPtr->GetElScanRate();
      }
   }
   if (mCurrentSectorPtr->GetType() == Sector::cAZ)
   {
      detectTime += deltaT_Az;
   }
   else if (mCurrentSectorPtr->GetType() == Sector::cEL)
   {
      detectTime += deltaT_El;
   }
   else // az-el; use the least of the two times (otherwise, as with the individual cases, it might not get a detection chance).
   {
      detectTime += std::min(deltaT_Az, deltaT_El);
   }

   return detectTime;
}

void WsfSectorScanSensorScheduler::InitializeScan(double aSimTime)
{
   // Transition to the first sector in the list, which is the *next* sector.
   mSectorIter = mSectors.end();
   --mSectorIter;
   mNextSectorIter = mSectors.begin();

   // Begin by slewing to the first sector.
   SlewToSector(aSimTime, *mNextSectorIter);
}

// virtual
void WsfSectorScanSensorScheduler::ModeDeselected(double aSimTime, WsfStringId aModeNameId)
{
   size_t modeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   if (mLastExplicitModeIndex == modeIndex)
   {
      mLastExplicitModeIndex = mModeList.size();
   }
}

// virtual
void WsfSectorScanSensorScheduler::ModeSelected(double aSimTime, WsfStringId aModeNameId)
{
   mLastExplicitModeIndex = mSensorPtr->GetModeList()->GetModeByName(aModeNameId);
   mSearchAllowed         = (!mModeList[mLastExplicitModeIndex]->DisablesSearch());
}

// virtual
void WsfSectorScanSensorScheduler::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Put the platform on the back of the queue.  The entries are processed by their physical location
   // in relation to the sensor not where they are in the queue.
   mSearchList.push_back(aPlatformPtr->GetIndex());
}

// virtual
void WsfSectorScanSensorScheduler::RemoveTarget(double aSimTime, size_t aTargetIndex)
{
   auto sli = find(mSearchList.begin(), mSearchList.end(), aTargetIndex);
   if (sli != mSearchList.end())
   {
      mSearchList.erase(sli);
   }
}

// virtual
bool WsfSectorScanSensorScheduler::SelectTarget(double               aSimTime,
                                                double&              aNextSimTime,
                                                size_t&              aTargetIndex,
                                                WsfTrackId&          aRequestId,
                                                WsfSensor::Settings& aSettings)
{
   aRequestId.Null();
   aTargetIndex = 0;

   // Check to see if the cueing was recently cleared.  If so,
   // there was likely a scripted cue; start the scan pattern over.
   if (!mSensorPtr->IsCued())
   {
      InitializeScan(aSimTime);
   }

   AdvanceScan(aSimTime);

   double deltaT = aSimTime - mLastUpdateTime;

   // Only select targets on an update interval and if we are not in the process of slewing to the next sector.
   if ((deltaT >= mSensorUpdateInterval) && (!mSlewingToNextSector))
   {
      // Go through the list of allowed platforms and see which are in this slice of the FOV
      if (!mSearchList.empty() && mSearchAllowed)
      {
         // Loop through list of platforms to search
         for (size_t searchIndex : mSearchList)
         {
            // If this target falls within the FOV.
            WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(searchIndex);
            if (targetPtr != nullptr)
            {
               // For frame-based detections, only return each target once per frame.
               if (!mFrameBasedScheduling ||
                   (mTargetsSelectedThisFrame.find(targetPtr->GetIndex()) == mTargetsSelectedThisFrame.end()))
               {
                  // Select and store targets that are within the field of view.
                  targetPtr->Update(aSimTime);
                  if (mSensorPtr->WithinFieldOfView(aSimTime, targetPtr))
                  {
                     SensorTarget sensorTarget(searchIndex, aSettings.mModeIndex, aSimTime);

                     if (mFrameBasedScheduling)
                     {
                        sensorTarget.mDetectTime = EstimateDetectTime(aSimTime, *targetPtr);
                        mTargetsSelectedThisFrame.insert(targetPtr->GetIndex());
                     }
                     else
                     {
                        sensorTarget.mDetectTime = aSimTime;
                     }

                     mSensorTargetQueue.push(sensorTarget);
                  }
               }
            }
         }
      }
      mLastUpdateTime = aSimTime;
   } // Done searching

   // Return next target in list if queue is not empty and it's time to return it.
   if (!mSensorTargetQueue.empty())
   {
      const SensorTarget& sensorTarget = mSensorTargetQueue.top();
      if (aSimTime >= sensorTarget.mDetectTime)
      {
         aTargetIndex         = sensorTarget.mTargetIndex;
         aSettings.mModeIndex = sensorTarget.mModeIndex;
         mSensorTargetQueue.pop();
      }
   }
   if (!mSensorTargetQueue.empty())
   {
      // Provide the next target at the specified detect time.
      aNextSimTime = mSensorTargetQueue.top().mDetectTime;
   }
   else
   {
      // Search for targets again at the next update interval.
      aNextSimTime = aSimTime + mSensorUpdateInterval;
   }

   return (aTargetIndex != 0);
}

// virtual
void WsfSectorScanSensorScheduler::TurnOff(double aSimTime)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Scheduler turned off.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
   }

   // When the sensor is turned off the platform search list is cleared to save memory.
   ResetSearchList();

   // Also any selection chances this frame are cleared.
   mTargetsSelectedThisFrame.clear();
}

// virtual
void WsfSectorScanSensorScheduler::TurnOn(double aSimTime)
{
   InitializeScan(aSimTime);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Scheduler turned on.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << mSensorPtr->GetPlatform()->GetName();
      out.AddNote() << "Sensor: " << mSensorPtr->GetName();
   }

   // When the sensor is turned off the platform search queue is cleared to save memory.
   // The queue must be rebuilt when the sensor is turned back on.

   ResetSearchList(); // This should be a no-op...
   WsfSimulation& sim           = *GetSimulation();
   size_t         platformCount = sim.GetPlatformCount();
   for (size_t platformEntry = 0; platformEntry < platformCount; ++platformEntry)
   {
      WsfPlatform* targetPtr = sim.GetPlatformEntry(platformEntry);

      if (targetPtr != mSensorPtr->GetPlatform())
      {
         mSensorPtr->PlatformAdded(aSimTime, targetPtr);
      }
   }
}

// private
void WsfSectorScanSensorScheduler::ResetSearchList()
{
   // Delete the scan chances.
   mSearchList.clear();
}
