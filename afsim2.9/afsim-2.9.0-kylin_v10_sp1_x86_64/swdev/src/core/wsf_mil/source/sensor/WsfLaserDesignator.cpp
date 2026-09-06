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

#include "WsfLaserDesignator.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "UtAzElLookup.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfDefaultSensorTracker.hpp"
#include "WsfDraw.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
WsfLaserDesignator::WsfLaserDesignator(WsfScenario& aScenario)
   : WsfSensor(aScenario)
   , mLaserDesignationsPtr(nullptr)
   , mLD_ModeList()
   , mLaserCode(0)
   , mUpdateCount(0)
   , mEmitterLocWCS()
   , mUnitLOS_WCS()
   , mSpotLocationWCS()
   , mFrequency(0.0)
   , mPower(0.0)
   , mMaximumRange(10000.0)
   , mTerrainFloatDistance(1.5)
   , mPrivateDebug(false)
   , mIsEmitting(false)
   , mSpotIsPlaced(false)
   , mSpotIsExtinguished(true)
   , mAssociation()
   , mLaserSpot()
   , mTerrain(nullptr)
{
   SetClass(cACTIVE | cVISUAL);

   // Create the mode list with the sensor-specific mode template.
   SetModeList(ut::make_unique<WsfSensorModeList>(new LaserDesignationMode()));

   // Assign the default sensor scheduler and tracker
   SetScheduler(ut::make_unique<WsfDefaultSensorScheduler>());
   SetTracker(ut::make_unique<WsfDefaultSensorTracker>(aScenario));
}

// =================================================================================================
WsfLaserDesignator::WsfLaserDesignator(const WsfLaserDesignator& aSrc)
   : WsfSensor(aSrc)
   , mLaserDesignationsPtr(nullptr)
   , mLD_ModeList()
   , mLaserCode(aSrc.mLaserCode)
   , mUpdateCount(aSrc.mUpdateCount)
   , mEmitterLocWCS(aSrc.mEmitterLocWCS)
   , mUnitLOS_WCS(aSrc.mUnitLOS_WCS)
   , mSpotLocationWCS(aSrc.mSpotLocationWCS)
   , mFrequency(aSrc.mFrequency)
   , mPower(aSrc.mPower)
   , mMaximumRange(aSrc.mMaximumRange)
   , mTerrainFloatDistance(aSrc.mTerrainFloatDistance)
   , mPrivateDebug(aSrc.mPrivateDebug)
   , mIsEmitting(aSrc.mIsEmitting)
   , mSpotIsPlaced(aSrc.mSpotIsPlaced)
   , mSpotIsExtinguished(aSrc.mSpotIsExtinguished)
   , mAssociation()
   , mLaserSpot(aSrc.mLaserSpot)
   , mTerrain(nullptr)
{
}

WsfLaserDesignator::~WsfLaserDesignator()
{
   delete mTerrain;
}

// =================================================================================================
// virtual
WsfSensor* WsfLaserDesignator::Clone() const
{
   return new WsfLaserDesignator(*this);
}

// =================================================================================================
// virtual
bool WsfLaserDesignator::Initialize(double aSimTime)
{
   bool ok               = true;
   mLaserDesignationsPtr = static_cast<WsfLaserDesignations*>(GetSimulation()->FindExtension("laser_designations"));
   if (mLaserDesignationsPtr == nullptr)
   {
      ut::log::error() << "Extension 'laser_designations' not available.  Cannot initialize WSF_LASER_DESIGNATOR.";
      ok = false;
   }
   mTerrain = new wsf::Terrain(GetSimulation()->GetTerrainInterface());
   ok       = WsfSensor::Initialize(aSimTime) && ok;

   // Reduce future dynamic casting by extracting derived class mode pointers.
   mModeListPtr->GetDerivedModeList(mLD_ModeList);

   return ok;
}

// =================================================================================================
// virtual
void WsfLaserDesignator::TurnOff(double aSimTime)
{
   if (CanBeTurnedOff())
   {
      Extinguish(aSimTime);
      WsfSensor::TurnOff(aSimTime);
   }
}

// =================================================================================================
// virtual
void WsfLaserDesignator::TurnOn(double aSimTime)
{
   if (CanBeTurnedOn())
   {
      WsfSensor::TurnOn(aSimTime);
   }
}

// =================================================================================================
size_t WsfLaserDesignator::GetDesignatedEntityIndex() const
{
   return mAssociation.GetIndex();
}

// =================================================================================================
// virtual
bool WsfLaserDesignator::ProcessInput(UtInput& aInput)
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
   else if (command == "maximum_target_association_distance")
   {
      double dist;
      aInput.ReadValueOfType(dist, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(dist, 0.0);
      mAssociation.mNearestLimitSquared = dist * dist;
   }
   else if (command == "terrain_float_distance")
   {
      aInput.ReadValueOfType(mTerrainFloatDistance, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mTerrainFloatDistance, 0.0);
   }
   else if (command == "target_association_staleout_interval")
   {
      aInput.ReadValueOfType(mAssociation.mTimeoutInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mAssociation.mTimeoutInterval, 0.0);
   }
   else if (command == "maximum_transmission_range")
   {
      aInput.ReadValueOfType(mMaximumRange, UtInput::cLENGTH);
      aInput.ValueGreater(mMaximumRange, 0.0);
   }
   else
   {
      myCommand = WsfSensor::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
void WsfLaserDesignator::PerformScheduledDetections(double aSimTime)
{
   // Make sure that a scheduler and tracker are present.
   // This function gets gutted in order to simply determine what the associated platform is
   // chosen as the lased entity.
   assert(mModeListPtr != nullptr);
   assert(mSchedulerPtr != nullptr);
   assert(mTrackerPtr != nullptr);

   WsfTrackId                 requestId;
   Settings                   settings;
   WsfSensorResult            result;
   WsfSensorTracker::Settings stSettings;
   size_t                     targetIndex = 0;

   while (mSchedulerPtr->SelectTarget(aSimTime, mNextUpdateTime, targetIndex, requestId, settings))
   {
      // Perform the sensing chance if the target still exists.
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(targetIndex);
      if (targetPtr != nullptr && targetPtr != GetPlatform())
      {
         if (mAssociation.Consider(aSimTime, mEmitterLocWCS, mUnitLOS_WCS, mSpotLocationWCS, targetPtr))
         {
            size_t newIndex = mAssociation.GetIndex();
            mLaserSpot.SetDesignatedIndex(*GetSimulation(), newIndex);
            if (mPrivateDebug)
            {
               auto out = ut::log::debug() << "Laser is selecting target index as designated entity.";
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Laser Designator: " << GetName();
               out.AddNote() << "Target Index: " << newIndex;
            }
         }
      }
      else if (mTrackerPtr->TargetDeleted(aSimTime, stSettings, requestId, targetIndex))
      {
         // Target no longer physically exists and is not being tracked.
         mSchedulerPtr->RemoveTarget(aSimTime, targetIndex);
      }
      WsfArticulatedPart::ClearTransientCue(); // Release any transient cue created by the scheduler.
   }                                           // while (mSchedulerPtr->SelectTarget())

   // Set the update interval so the schedule is called at the desired time.
   double updateInterval = std::max(mNextUpdateTime - aSimTime, 1.0E-5);
   SetUpdateInterval(updateInterval);
}

// =================================================================================================
// virtual
void WsfLaserDesignator::Update(double aSimTime)
{
   ++mUpdateCount;

   // Bypass updates if not time for an update.  This avoids unnecessary device updates.
   // (A little slop is allowed to make sure event-driven chances occur as scheduled)
   if (mNextUpdateTime <= (aSimTime + 1.0E-5))
   {
      WsfSensor::Update(aSimTime);

      // Place or Update the laser spot at the terminal end of our emitted beam:
      if (mIsEmitting)
      {
         // Place a spot along LOS, first at maximum range,
         // then shorten for clouds and terrain impact.....

         // My frustrum location (eyepoint), WCS
         GetLocationWCS(mEmitterLocWCS.GetData());

         // My unit vector along LOS, WCS.
         GetLOS_UnitVectorWCS(mUnitLOS_WCS.GetData());
         // Assure it IS a unit vector:
         mUnitLOS_WCS.Normalize();

         double beamLength = mMaximumRange;

         // Set spot at end of max range...
         mSpotLocationWCS = mEmitterLocWCS + mUnitLOS_WCS * beamLength;

         // Now either of the two calls below may foreshorten the beam, moving mSpotLocationWCS.
         if (GetScenario().GetEnvironment().LOS_AdjustedClearOfClouds(mEmitterLocWCS.GetData(), mSpotLocationWCS.GetData()))
         {
            // But if we hit clouds need to shorten the beam length
            beamLength       = (mSpotLocationWCS - mEmitterLocWCS).Magnitude();
            mSpotLocationWCS = mEmitterLocWCS + beamLength * mUnitLOS_WCS;
         }
         if (mTerrain->TerrainHitAlongLOS(mEmitterLocWCS.GetData(), mSpotLocationWCS.GetData()))
         {
            // But if we hit terrain, we need to shorten the beam length by
            // some distance to suspend the spot slightly _above_ terrain, or
            // the spot will get occulted by terrain.
            beamLength = (mSpotLocationWCS - mEmitterLocWCS).Magnitude();
            beamLength -= mTerrainFloatDistance;
            mSpotLocationWCS = mEmitterLocWCS + beamLength * mUnitLOS_WCS;
         }

         mLaserSpot.SetLocationWCS(mSpotLocationWCS.GetData());

         if (beamLength >= mMaximumRange)
         {
            beamLength = mMaximumRange;
            // Max range beam, set power to zero.
            mLaserSpot.SetPower(0.0);
         }
         else
         {
            mLaserSpot.SetPower(mPower);
         }

         // Refresh the time stamp so I do not get extinguished in the environment.
         mLaserSpot.SetSimTimeStamp(aSimTime);
         mLaserDesignationsPtr->AddOrUpdate(mLaserSpot, mEmitterLocWCS.GetData());

         if (mPrivateDebug)
         {
            WsfDraw draw(*GetSimulation());
            draw.BeginLines();
            draw.SetColor(255.0, 255.0, 0.0, 0.65); // yellow
            draw.SetDuration(2.5);
            draw.SetLineSize(3);
            double locWCS[3];
            mLaserSpot.GetLocationWCS(locWCS);
            double lat;
            double lon;
            double alt;
            UtEntity::ConvertWCSToLLA(locWCS, lat, lon, alt);
            draw.VertexLLA(lat, lon, alt + 30);
            draw.VertexLLA(lat, lon, alt);
            draw.End();
         }

         // Now find the platform closest to our beam, and associate it as the designated entity.
         PerformScheduledDetections(aSimTime); // Perform any required detection attempts
      }
      else if (!mLaserSpot.IsNulled())
      {
         if (mPrivateDebug)
         {
            auto out = ut::log::debug() << "Extinguishing a laser spot.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Laser Designator: " << GetName();
         }
         Extinguish(aSimTime);
      }
   }
}

// =================================================================================================
// virtual
void WsfLaserDesignator::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   size_t platIndex = aPlatformPtr->GetIndex();
   if (mLaserSpot.GetDesignatedIndex() == platIndex)
   {
      mLaserSpot.ClearDesignatedPlatform();
   }
   mAssociation.Remove(platIndex);
   WsfSensor::PlatformDeleted(aPlatformPtr);
}

// =================================================================================================
void WsfLaserDesignator::Lase(double aSimTime, double aFrequency, double aPower)
{
   assert(mLaserSpot.IsNulled());
   assert(!mIsEmitting);

   mPower     = aPower;
   mFrequency = aFrequency;

   mLaserSpot.SetIsLocal(true);
   mLaserSpot.SetPower(mPower);
   mLaserSpot.SetFrequency(mFrequency);
   mLaserSpot.SetCode(mLaserCode);
   mLaserSpot.SetDesignatingIndex(GetPlatform()->GetIndex());

   mIsEmitting = (mPower > 0.0);
}

// =================================================================================================
void WsfLaserDesignator::Extinguish(double aSimTime)
{
   mAssociation.Clear();
   if (!mLaserSpot.IsNulled())
   {
      // Remove will force power to zero anyway.
      mLaserSpot.NullPower();
      mLaserSpot.ClearDesignatedPlatform();
      if (!mLaserDesignationsPtr->Remove(mLaserSpot))
      {
         if (mPrivateDebug)
         {
            auto out = ut::log::debug() << "Attempted removal failed. The spot does not exist.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Laser Designator: " << GetName();
         }
      }
   }
}

// =================================================================================================
// virtual
size_t WsfLaserDesignator::GetEM_XmtrCount() const
{
   // Returns 0 prior to initialization
   return mLD_ModeList.empty() ? 0U : 1U;
}

// =================================================================================================
// virtual
WsfEM_Xmtr& WsfLaserDesignator::GetEM_Xmtr(size_t aIndex) const
{
   return mLD_ModeList[mModeListPtr->GetCurrentMode()]->mXmtr;
}

// =================================================================================================
// Definition for the nested class that implements the mode of the sensor.
// =================================================================================================

// =================================================================================================
WsfLaserDesignator::LaserDesignationMode::LaserDesignationMode()
   : WsfSensorMode()
   , mLD_Ptr(nullptr)
   , mAntenna()
   , mXmtr(WsfEM_Xmtr::cXF_SENSOR, &mAntenna)
{
   mXmtr.SetEarthRadiusMultiplier(1.0); // The default is 4/3
}

// =================================================================================================
WsfLaserDesignator::LaserDesignationMode::LaserDesignationMode(const LaserDesignationMode& aSrc)
   : WsfSensorMode(aSrc)
   , mLD_Ptr(nullptr)
   , mAntenna(aSrc.mAntenna)
   , mXmtr(aSrc.mXmtr, &mAntenna)
{
}

// =================================================================================================
// virtual
WsfMode* WsfLaserDesignator::LaserDesignationMode::Clone() const
{
   return new LaserDesignationMode(*this);
}

// =================================================================================================
// virtual
bool WsfLaserDesignator::LaserDesignationMode::Initialize(double aSimTime)
{
   bool ok = WsfSensorMode::Initialize(aSimTime);

   mLD_Ptr = dynamic_cast<WsfLaserDesignator*>(mSensorPtr);
   if (mLD_Ptr == nullptr)
   {
      ut::log::error() << "WsfLaserDesignator LaserDesignationMode not provided with proper sensor type.";
      ok = false;
   }

   ok &= mAntenna.Initialize(mSensorPtr);
   ok &= mXmtr.Initialize(*mSensorPtr->GetSimulation());

   // Set the debug flag
   mXmtr.SetDebugEnabled(mSensorPtr->DebugEnabled());

   return ok;
}

// =================================================================================================
// virtual
bool WsfLaserDesignator::LaserDesignationMode::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   // We check for the 'transmitter' block ourself because we have additional
   // commands that we wish to include in those contexts
   if (command == "transmitter")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (mXmtr.ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
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
bool WsfLaserDesignator::LaserDesignationMode::AttemptToDetect(double           aSimTime,
                                                               WsfPlatform*     aTargetPtr,
                                                               Settings&        aSettings,
                                                               WsfSensorResult& aResult)
{
   return mLD_Ptr->IsEmitting() && (mLD_Ptr->GetDesignatedEntityIndex() == aTargetPtr->GetIndex());
}

// =================================================================================================
// virtual
void WsfLaserDesignator::LaserDesignationMode::Deselect(double aSimTime)
{
   mXmtr.Deactivate();
   mLD_Ptr->Extinguish(aSimTime);
}

// =================================================================================================
// virtual
void WsfLaserDesignator::LaserDesignationMode::Select(double aSimTime)
{
   // Activate the sensor, ands set the laser spot power and frequency.
   mXmtr.Activate();
   mLD_Ptr->Lase(aSimTime, mXmtr.GetFrequency(), mXmtr.GetPower());
}


// =================================================================================================
bool WsfLaserDesignator::TargetAssociation::Consider(double          aSimTime,
                                                     const UtVec3dX& aEmitterLocWCS,
                                                     const UtVec3dX& aLOS_UnitVectorWCS,
                                                     const UtVec3dX& aLaserSpotLocWCS,
                                                     WsfPlatform*    aTargetPtr)
{
   // If this platform is nearest to the laser beam, and is "close enough",
   // then consider it the intended target being lased.  Use R squared
   // to prevent using the sqrt() operation any more than necessary.

   UtVec3dX tgtLocWCS;
   aTargetPtr->Update(aSimTime);
   aTargetPtr->GetLocationWCS(tgtLocWCS.GetData());
   UtVec3dX diffWCS            = tgtLocWCS - aEmitterLocWCS;
   double   normalDist2BeamSqd = (diffWCS - (diffWCS.Dot(aLOS_UnitVectorWCS) * aLOS_UnitVectorWCS)).MagnitudeSquared();

   if (normalDist2BeamSqd <= mNearestLimitSquared)
   {
      // This is a contender for an associated target, update its info.
      // (If we do not know this contender, create a new struct for it.)
      Consideration* ptr  = GetPtr(aTargetPtr->GetIndex());
      ptr->mLastCloseTime = aSimTime;
      ptr->mDist2BeamSqd  = normalDist2BeamSqd;
      ptr->mDist2SpotSqd  = (aLaserSpotLocWCS - tgtLocWCS).MagnitudeSquared();
   }

   return Update(aSimTime);
}

// =================================================================================================
WsfLaserDesignator::Consideration* WsfLaserDesignator::TargetAssociation::GetPtr(size_t aIndex)
{
   for (size_t i = 0; i != mConsiderations.size(); ++i)
   {
      if (mConsiderations[i]->mIndex == aIndex)
      {
         return mConsiderations[i];
      }
   }

   // It did not exist, so create and return it.
   Consideration* ptr = new Consideration(aIndex);
   mConsiderations.push_back(ptr);
   return ptr;
}

// =================================================================================================
bool WsfLaserDesignator::TargetAssociation::Update(double aSimTime)
{
   double                                smallestRsquared = DBL_MAX;
   size_t                                closestIndex     = 0;
   std::vector<Consideration*>::iterator iter             = mConsiderations.begin();
   while (iter != mConsiderations.end())
   {
      if (aSimTime > (*iter)->mLastCloseTime + mTimeoutInterval)
      {
         // Stale.  Remove it.
         Consideration* cPtr = *iter;
         // increments iterator
         iter = mConsiderations.erase(iter);
         delete cPtr;
      }
      else
      {
         if ((*iter)->mDist2BeamSqd < smallestRsquared)
         {
            smallestRsquared = (*iter)->mDist2BeamSqd;
            closestIndex     = (*iter)->mIndex;
         }

         ++iter;
      }
   }

   bool changed      = closestIndex != mCurrentPlatIndex;
   mCurrentPlatIndex = closestIndex;
   return changed;
}

// =================================================================================================
WsfLaserDesignator::TargetAssociation::TargetAssociation()
   : mCurrentPlatIndex(0)
   , mTimeoutInterval(60.0)
   , mNearestLimitSquared(1000.0)
   , mConsiderations()
{
}

// =================================================================================================
WsfLaserDesignator::TargetAssociation::~TargetAssociation()
{
   Clear();
}

// =================================================================================================
void WsfLaserDesignator::TargetAssociation::Clear()
{
   std::vector<Consideration*>::iterator iter = mConsiderations.begin();
   while (iter != mConsiderations.end())
   {
      Consideration* cPtr = *iter;
      iter                = mConsiderations.erase(iter);
      delete cPtr;
   }
}

// =================================================================================================
void WsfLaserDesignator::TargetAssociation::Remove(size_t aIndex)
{
   std::vector<Consideration*>::iterator iter = mConsiderations.begin();
   while (iter != mConsiderations.end())
   {
      if ((*iter)->mIndex == aIndex)
      {
         Consideration* cPtr = *iter;
         iter                = mConsiderations.erase(iter);
         delete cPtr;
         break;
      }
      ++iter;
   }
}
