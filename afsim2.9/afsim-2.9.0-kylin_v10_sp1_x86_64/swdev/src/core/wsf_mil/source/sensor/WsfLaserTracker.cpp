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

#include "WsfLaserTracker.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtAzElLookup.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfDraw.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEnvironment.hpp"
#include "WsfLaserDesignations.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfLaserTracker::WsfLaserTracker(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mLaserDesignationsPtr(nullptr)
   , mTrackModeList()
   , mLaserCode(0)
   , mCurrentLT_ModePtr(nullptr)
   , mTrackModeIndex(0)
   , mUpdateCount(0)
   , mCorrelatedToTgtIndex(0)
   , mLastSpotCount(0)
   , mWavelength(0.0)
   , mMaximumDetectionRangeSquared(DBL_MAX)
   , mPrivateDebug(false)
   , mIsTracking(false)
   , mLocalTrackId()
   , mLocalResult()
   , mTerrain(nullptr)
   , mLastLossReason(cNO_SPOTS)
   , mLossReason(cNO_SPOTS)
{
   SetClass(cACTIVE | cVISUAL);

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new LaserTrackMode()));

   // Assign a Null sensor scheduler, but a non-null tracker.
   SetScheduler(nullptr); // new WsfDefaultSensorScheduler());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfLaserTracker::WsfLaserTracker(const WsfLaserTracker& aSrc)
   : WsfSensor(aSrc)
   , mLaserDesignationsPtr(nullptr)
   , mTrackModeList()
   , mLaserCode(aSrc.mLaserCode)
   , mCurrentLT_ModePtr(nullptr)
   , mTrackModeIndex(aSrc.mTrackModeIndex)
   , mUpdateCount(aSrc.mUpdateCount)
   , mCorrelatedToTgtIndex(aSrc.mCorrelatedToTgtIndex)
   , mLastSpotCount(aSrc.mLastSpotCount)
   , mWavelength(aSrc.mWavelength)
   , mMaximumDetectionRangeSquared(aSrc.mMaximumDetectionRangeSquared)
   , mPrivateDebug(aSrc.mPrivateDebug)
   , mIsTracking(aSrc.mIsTracking)
   , mLocalTrackId(aSrc.mLocalTrackId)
   , mLocalResult(aSrc.mLocalResult)
   , mTerrain(nullptr)
   , mLastLossReason(aSrc.mLastLossReason)
   , mLossReason(aSrc.mLossReason)
{
}

// =================================================================================================
WsfLaserTracker::~WsfLaserTracker()
{
   /*if (mPrivateDebug)
   {
      cout << " At t = " << WsfSimulation::GetInstance().GetSimTime() << " ***** WsfLaserTracker DESTRUCTING. *****" << endl;
   }*/
}

// =================================================================================================
// virtual
void WsfLaserTracker::TurnOn(double aSimTime)
{
   WsfSensor::TurnOn(aSimTime);
   if (IsTurnedOn())
   {
      SetCuedOrientation(0.0, 0.0);
   }
}

// =================================================================================================
// virtual
void WsfLaserTracker::TurnOff(double aSimTime)
{
   WsfSensor::TurnOff(aSimTime);
   if (!IsTurnedOn())
   {
      SetCuedOrientation(0.0, 0.0);
   }
}

// =================================================================================================
std::string WsfLaserTracker::GetReason()
{
   if (mLossReason == cNO_SPOTS)
   {
      return "NO_SPOTS";
   }
   else if (mLossReason == cZERO_POWER)
   {
      return "ZERO_POWER";
   }
   else if (mLossReason == cOUT_OF_RANGE)
   {
      return "OUT_OF_RANGE";
   }
   else if (mLossReason == cCLOUD_OCCULTED)
   {
      return "CLOUD_OCCULTED";
   }
   else if (mLossReason == cOUT_OF_FOV)
   {
      return "OUT_OF_FOV";
   }
   else if (mLossReason == cTERRAIN_OCCULTED)
   {
      return "TERRAIN_OCCULTED";
   }
   else if (mLossReason == cDETECTED)
   {
      return "DETECTED";
   }
   else
   {
      return "UNKNOWN";
   }
}

// =================================================================================================
// virtual
WsfSensor* WsfLaserTracker::Clone() const
{
   return new WsfLaserTracker(*this);
}

// =================================================================================================
// virtual
bool WsfLaserTracker::Initialize(double aSimTime)
{
   bool ok  = WsfSensor::Initialize(aSimTime);
   mTerrain = new wsf::Terrain(GetSimulation()->GetTerrainInterface());

   mLaserDesignationsPtr = static_cast<WsfLaserDesignations*>(GetSimulation()->FindExtension("laser_designations"));
   if (mLaserDesignationsPtr == nullptr)
   {
      ut::log::error() << "Could not find 'laser_designations'. Required for WSF_LASER_TRACKER.";
      ok = false;
   }

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mTrackModeList);

   // Stare straight ahead until something better comes along.
   SetCuedOrientation(0.0, 0.0);
   mLocalTrackId.SetOwningPlatformId(GetPlatform()->GetNameId());
   mLocalTrackId.SetLocalTrackNumber(1); // DRB TO-DO

   for (size_t i = 0; i != mTrackModeList.size(); ++i)
   {
      if (!mTrackModeList[i]->Initialize(aSimTime))
      {
         ok = false;
      }
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfLaserTracker::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "laser_code")
   {
      aInput.ReadValue(mLaserCode);
      aInput.ValueGreater(mLaserCode, 0);
   }
   else if (command == "private_debug")
   {
      mPrivateDebug = true;
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfLaserTracker::Update(double aSimTime)
{
   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      ++mUpdateCount;
      bool detected = false;

      WsfSensorTracker::Settings stSettings;
      double                     sensorLocWCS[3];
      double                     sensorLosWCS[3];

      WsfSensor::Update(aSimTime);
      GetLocationWCS(sensorLocWCS);
      GetLOS_UnitVectorWCS(sensorLosWCS);

      // Get all the spots that are currently in the simulation which match my Frequency and Code.
      std::vector<const WsfLaserDesignations::Spot*> spotPtrs;
      mLaserDesignationsPtr->GetSpecified(mLaserCode, mWavelength, spotPtrs);

      mLastLossReason = mLossReason;

      unsigned int spotCount = (unsigned int)spotPtrs.size();
      if (mPrivateDebug && mLastSpotCount != spotCount)
      {
         auto out = ut::log::debug() << "Spot count changed.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Laser Tracker: " << GetName();
         out.AddNote() << "Last Spot Count: " << mLastSpotCount;
         out.AddNote() << "New Spot Count: " << spotCount;
      }
      mLastSpotCount = spotCount;
      if (mPrivateDebug && mUpdateCount % 111 == 0)
      {
         auto out = ut::log::debug() << "WsfLaserTracker - Laser spot count:";
         out.AddNote() << "Count: " << spotCount;
      }
      if (spotCount == 0)
      {
         mLossReason = cNO_SPOTS;
      }

      // Traverse all the spots that match:
      // A) My Frequency (done).
      // B) My Code (done).
      // C) Power greater than zero.
      // D) Within detection Range.
      // E) Does not pass through a cloud layer.
      // F) Within Field Of View.
      // G) Is not occulted by terrain.
      //    (Terrain test is done last, as it is CPU intensive.)

      double spotDistSqrd;
      double spotDist;
      double spotLocWCS[3];
      double unitSpotOffsetWCS[3];
      double unitSpotVectPCS[3];
      double thisToTgtAz;
      double thisToTgtEl;

      double latA = 0.0;
      double lonA = 0.0;
      double altA = 0.0;
      double latB = 0.0;
      double lonB = 0.0;
      double altB = 0.0;

      for (unsigned int iSpot = 0; iSpot != spotPtrs.size(); ++iSpot)
      {
         detected = true;

         const WsfLaserDesignations::Spot& spot = *spotPtrs[iSpot];

         if (spot.GetPower() <= 0.0)
         {
            // Fail test C.
            mLossReason = cZERO_POWER;
            detected    = false;
         }

         if (detected)
         {
            spot.GetTargetAdjustedLocationWCS(spotLocWCS, mLaserDesignationsPtr);
            UtVec3d::Subtract(unitSpotOffsetWCS, spotLocWCS, sensorLocWCS);
            spotDist     = UtVec3d::Normalize(unitSpotOffsetWCS);
            spotDistSqrd = spotDist * spotDist;
            if (spotDistSqrd >= mMaximumDetectionRangeSquared)
            {
               // Failed test D.
               mLossReason = cOUT_OF_RANGE;
               detected    = false;
            }
         }

         if (detected)
         {
            if (!GetScenario().GetEnvironment().LOS_ClearOfClouds(sensorLocWCS, spotLocWCS))
            {
               // Failed test E.
               mLossReason = cCLOUD_OCCULTED;
               detected    = false;
            }
         }

         if (detected)
         {
            // Now check test F...  Field Of View
            // (mAntenna is a GemometricSensor class attribute):

            ConvertWCSVectorToPCS(unitSpotVectPCS, unitSpotOffsetWCS);
            thisToTgtAz = atan2(-unitSpotVectPCS[1], unitSpotVectPCS[0]);
            thisToTgtEl = asin(unitSpotVectPCS[2]);

            if (!mCurrentLT_ModePtr->mAntenna.WithinFieldOfView(thisToTgtAz, thisToTgtEl))
            {
               mLossReason = cOUT_OF_FOV;
               detected    = false;
            }
         }

         if (detected)
         {
            // Now check test G...    Terrain Occulting:
            // Check terrain masking only if all other checks succeed
            UtEntity::ConvertWCSToLLA(sensorLocWCS, latA, lonA, altA);
            UtEntity::ConvertWCSToLLA(spotLocWCS, latB, lonB, altB);
            if (mTerrain->MaskedByTerrain(latA, lonA, altA, latB, lonB, altB, spotDist))
            {
               // Failed test G.
               mLossReason = cTERRAIN_OCCULTED;
               detected    = false;
            }
         }

         if (detected)
         {
            if (!mIsTracking)
            {
               // Just beginning a new track.  Cue to LOS.
               SelectMode(aSimTime, "TRACK");

               if (mPrivateDebug)
               {
                  auto out = ut::log::debug() << "Started tracking.";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Laser Tracker: " << GetName();
                  out.AddNote() << "Reason: " << GetReason();
               }
            }

            mIsTracking         = true;
            mLossReason         = cDETECTED;
            WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(spot.GetDesignatedIndex());

            if (tgtPtr == nullptr)
            {
               mCorrelatedToTgtIndex = 0;
            }
            else
            {
               mCorrelatedToTgtIndex = tgtPtr->GetIndex();
            }

            SetCuedLocationWCS(spotLocWCS);
            mLocalResult.mMeasurement.SetLocationWCS(spotLocWCS);

            // Do not apply any measurement errors, this is assumed a perfect track.
            if (tgtPtr != nullptr)
            {
               mTrackerPtr->TargetDetected(aSimTime, stSettings, mLocalTrackId, mCorrelatedToTgtIndex, tgtPtr, mLocalResult);
            }

            break;
         }
      } // end for loop over all the spots

      if (detected)
      {
         if (mPrivateDebug && mLastLossReason != mLossReason)
         {
            auto out = ut::log::debug() << "Track reason changed.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Laser Tracker: " << GetName();
            out.AddNote() << "Reason: " << GetReason();
         }
      }
      else
      {
         if (mIsTracking && mCorrelatedToTgtIndex != 0)
         {
            WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(mCorrelatedToTgtIndex);
            if (tgtPtr != nullptr)
            {
               mTrackerPtr->TargetUndetected(aSimTime, stSettings, mLocalTrackId, mCorrelatedToTgtIndex, tgtPtr, mLocalResult);
            }
            mCorrelatedToTgtIndex = 0;
         }

         if (mPrivateDebug && mIsTracking)
         {
            auto out = ut::log::debug() << "Lost track.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Laser Tracker: " << GetName();
            out.AddNote() << "Reason: " << GetReason();
         }

         if (mPrivateDebug && mLastLossReason != mLossReason)
         {
            auto out = ut::log::debug() << "Track loss reason changed.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Laser Tracker: " << GetName();
            out.AddNote() << "Reason: " << GetReason();
         }

         mIsTracking = false;
      }

      if (mPrivateDebug)
      {
         WsfDraw draw(*GetSimulation());
         draw.BeginLines();
         if (mIsTracking)
         {
            // Blue
            draw.SetColor(0.0, 0.0, 1.0, 0.25);
         }
         else
         {
            // Red
            draw.SetColor(1.0, 0.0, 0.0, 0.25);
         }

         draw.SetDuration(0.5);
         draw.VertexLLA(latA, lonA, altA);
         draw.VertexLLA(latB, lonB, altB);
         draw.End();
      }

      // Some of the sensor classes do something fancy here, but not sure why.  Ignore.
      mNextUpdateTime = aSimTime + GetUpdateInterval();
   }
}

// =================================================================================================
// virtual
size_t WsfLaserTracker::GetEM_RcvrCount() const
{
   // Returns 0 prior to initialization
   return mTrackModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Rcvr& WsfLaserTracker::GetEM_Rcvr(size_t aIndex) const
{
   return mTrackModeList[mModeListPtr->GetCurrentMode()]->mRcvr;
}

// =================================================================================================
// Definition for the nested class that implements the mode of the sensor.
// =================================================================================================

// =================================================================================================
WsfLaserTracker::LaserTrackMode::LaserTrackMode()
   : WsfSensorMode()
   , mAntenna()
   , mRcvr(WsfEM_Rcvr::cRF_SENSOR, &mAntenna)
   , mLT_Ptr(nullptr)
{
}

// =================================================================================================
WsfLaserTracker::LaserTrackMode::LaserTrackMode(const LaserTrackMode& aSrc)
   : WsfSensorMode(aSrc)
   , mAntenna(aSrc.mAntenna)
   , mRcvr(aSrc.mRcvr, &mAntenna)
   , mLT_Ptr(nullptr)
{
}

// =================================================================================================
// virtual
WsfMode* WsfLaserTracker::LaserTrackMode::Clone() const
{
   return new LaserTrackMode(*this);
}

// =================================================================================================
void WsfLaserTracker::SetCurrentLT_Mode(LaserTrackMode* aModePtr)
{
   mCurrentLT_ModePtr = aModePtr;
   assert(mCurrentLT_ModePtr != nullptr);
   double freq                   = mCurrentLT_ModePtr->mRcvr.GetFrequency();
   mWavelength                   = UtMath::cLIGHT_SPEED / freq;
   double range                  = mCurrentLT_ModePtr->GetMaximumRange();
   mMaximumDetectionRangeSquared = range * range;
}

// =================================================================================================
// virtual
bool WsfLaserTracker::LaserTrackMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);

   mLT_Ptr = dynamic_cast<WsfLaserTracker*>(mSensorPtr);
   assert(mLT_Ptr != nullptr);

   ok &= mAntenna.Initialize(mSensorPtr);
   ok &= mRcvr.Initialize(*GetSimulation());

   // Set the debug flag
   mRcvr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// =================================================================================================
// virtual
bool WsfLaserTracker::LaserTrackMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // We check for the 'receiver' block ourself because we have additional
   // commands that we wish to include in those contexts
   if (command == "receiver")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (mRcvr.ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "maximum_range")
   {
      // mMaximumRange is a base class attribute.
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaximumRange, 0.0);
   }
   else if (mAntenna.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfSensorMode::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool WsfLaserTracker::LaserTrackMode::AttemptToDetect(double           aSimTime,
                                                      WsfPlatform*     aTargetPtr,
                                                      Settings&        aSettings,
                                                      WsfSensorResult& aResult)
{
   return false;

   /*   // Unlike most other sensors, this class does not detect "platforms" per se,
      // it detects laser spots, which, coincidentally MAY be associated with a
      // target platform or not.  So on this sensor chance, if the laser spot we
      // are <already> tracking happens to be associated with the platform we are
      // doing a sensor chance on, return true:
      bool detected = mLT_Ptr->IsTracking() &&
         (aTargetPtr->GetIndex() == mLT_Ptr->TrackedSpot().GetDesignatedIndex());

      aResult.Reset(aSettings);
      aResult.SetCategory(GetSensor()->GetZoneAttenuationModifier());
      GetSensor()->UpdatePosition(aSimTime);        // Ensure my position is current
      aTargetPtr->Update(aSimTime);                 // Ensure the target position is current
      if (GetSensor()->DebugEnabled())
      {
         cout << "T=" << aSimTime << ' ' << GetPlatform()->GetName() << '.' << GetSensor()->GetName()
              << " attempting to detect " << aTargetPtr->GetName() << '\n';
      }

      // Compute the target-to-receiver aspect angles from the unit vectors
      // (Not specifically required, but it is printed out by debug and sensor detection events).
      aTargetPtr->ComputeAspect(aResult.mTgtToRcvr.mTrueUnitVecWCS, aResult.mTgtToRcvr.mTrueAz, aResult.mTgtToRcvr.mTrueEl);
      aTargetPtr->ComputeAspect(aResult.mTgtToRcvr.mUnitVecWCS, aResult.mTgtToRcvr.mAz, aResult.mTgtToRcvr.mEl);

      if ((aResult.mFailedStatus == 0) &&
          (aResult.BeginOneWayInteraction(&mRcvr, aTargetPtr) == 0))
      {
         // Set the position of the antenna beam(s).
         aResult.SetReceiverBeamPosition();

         // DO NOT - Determine the target optical cross section.
         // DO NOT - Determine the target reflectivity.
         // DO NOT - Compute the two-way atmospheric transmission factor.
         // DO NOT - Use the LADAR range equation to compute the number of photons received
         // DO NOT - Compute the energy of the pulse from the laser (J)
         // DO NOT - Propagate through the transmit optics
         // DO NOT - Convert back to Watts for interaction output
         // DO NOT - Account for atmospheric losses from the transmitter to the target.
         // DO NOT - Compute the effective beamwidth (theta = 2 * K * M * wavelength / D_t)
         // DO NOT - Compute the energy per unit area at the target plane (J/m^2)
         // DO NOT - Compute effective area
         // DO NOT - Compute the reflected energy is energy per unit area times the effective cross section.
         // DO NOT - Account for atmospheric losses from target to receiver.
         // DO NOT - Compute the energy per unit area at the receiver (J/m^2)
         // DO NOT - Compute the energy intercepted by the receiver (J)
         // DO NOT - Propagate through the receive optics
         // DO NOT - Account for other factors.
         // DO NOT - Account for structural masking.
         // DO NOT - Convert back to Watts for interaction output
         // DO NOT - Compute the number of photons.
         // DO NOT - Determine if the Pd exceeds the detection threshold.
      }

      if (GetSensor()->DebugEnabled())
      {
         aResult.Print();
      }

      GetSensor()->NotifySensorDetectionAttempted(aSimTime, aTargetPtr, aResult);

      return detected;*/
}

// =================================================================================================
// virtual
void WsfLaserTracker::LaserTrackMode::Deselect(double aSimTime)
{
   mRcvr.Deactivate();
}

// =================================================================================================
// virtual
void WsfLaserTracker::LaserTrackMode::Select(double aSimTime)
{
   mRcvr.Activate();
   mLT_Ptr->SetCurrentLT_Mode(this);
}
