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

#include "P6DofVehicle.hpp"

#include <algorithm> // std::find
#include <fstream>
#include <limits>
#include <unordered_map>

#include "P6DofAeroCoreObject.hpp"
#include "P6DofAeroMovableObject.hpp"
#include "P6DofAtmosphere.hpp"
#include "P6DofFlightControlSystem.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofFreezeFlags.hpp"
#include "P6DofFuelTank.hpp"
#include "P6DofGravity.hpp"
#include "P6DofIntegrator.hpp"
#include "P6DofJetEngineObject.hpp"
#include "P6DofLandingGear.hpp"
#include "P6DofManualPilotSimpleControls.hpp"
#include "P6DofPilotManager.hpp"
#include "P6DofPilotObject.hpp"
#include "P6DofPropulsionSystem.hpp"
#include "P6DofRocketLiquidPropellantObject.hpp"
#include "P6DofRocketSolidPropellantObject.hpp"
#include "P6DofScenario.hpp"
#include "P6DofSequencer.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofThrustProducerObject.hpp"
#include "P6DofTypeManager.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicleType.hpp"
#include "P6DofWind.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringUtil.hpp"

const int64_t gNanosecStepSize = 10000000; // 10 msec (100 Hz)

P6DofVehicle::P6DofVehicle(P6DofScenario* aScenario)
   : P6DofObject(aScenario)
   , mIsCaptive(false)
   , mPosRelToParent_m(0., 0., 0.)
   , mAngRelToParentYPR_rad(0., 0., 0.)
   , mSeparationVel_fps(0., 0., 0.)
   , mSeparationOmega_rps(0., 0., 0.)
   , mAeroCoreObjPtr(nullptr)
   , mActiveAeroObjPtr(nullptr)
   , mPropulsionObjPtr(nullptr)
   , mLastSimTime_nanosec(0)
   , mStepSize_nanosec(gNanosecStepSize)
   , mMassProperties()
   , mKinematicState()
   , mIntegratorPtr(nullptr)
   , mGraphicalSupport()
   , mLastIsCaptive(true) // Regardless of the current captive state, we set this to true
   , mCrashed(false)
   , mEngineOn(false)
   , mAfterburnerOn(false)
   , mSmoking(false)
   , mOnFire(false)
   , mLifeTime_nanosec(0)
   , mTimeOfFlight_nanosec(0)
   , mSizeFactor()
   , mFixedObject(false)
   , mMaxMach(100.0)
   , mMaxAlpha_deg(180.0)
   , mMinAlpha_deg(-180.0)
   , mMaxBeta_deg(90.0)
   , mCgForce_lbs(0.0, 0.0, 0.0)
   , mCgMoment_ftlbs(0.0, 0.0, 0.0)
   , mTargetWCSLocation_m(0.0, 0.0, 0.0)
   , mTargetWCSLocationValid(false)
   , mInitialLat(0)
   , mInitialLon(0)
   , mInitialAlt_m(0)
   , mDownRangeDist_m(0)
   , mThrustTerminationTime_sec(0)
   , mThrustTerminationTimeValid(false)
   , mFlightControlsPtr(nullptr)
   , mPilotManagerPtr(nullptr)
   , mLandingGearPtr(nullptr)
   , mQuaternion(0.0, 0.0, 0.0)
   , mInitSetupMode(false)
   , mUseWashInInitConditions(false)
   , mTestingSimTime_nanosec(0)
   , mPrepositionLat(0)
   , mPrepositionLon(0)
   , mDamageSmokeTrailingEffect(false)
   , mFlamesPresent(false)
   , mIsDestroyed(false)
{
   mCurrentTarget.isValid                      = false;
   mCurrentTarget.targetName                   = "";
   mCurrentTarget.targetAziumth_deg            = 0;
   mCurrentTarget.targetElevation_deg          = 0;
   mCurrentTarget.targetSlantRange_nm          = 0;
   mCurrentTarget.guidanceYawAccelCommand_g    = 0;
   mCurrentTarget.guidancePitchAccelCommand_g  = 0;
   mCurrentTarget.guidanceGBias_g              = 0;
   mCurrentTarget.guidanceMaxG                 = 0;
   mCurrentTarget.guidanceFlightPathAngle_deg  = 0;
   mCurrentTarget.autopilotYawGLoadCommand_g   = 0;
   mCurrentTarget.autopilotPitchGLoadCommand_g = 0;
   mCurrentTarget.autopilotRollRateCommand_dps = 0;

   mNumTracks = 0;
   for (auto& i : mTrackData)
   {
      i.sensorName  = "";
      i.targetName  = "";
      i.targetLat   = 0;
      i.targetLon   = 0;
      i.targetAlt_m = 0;
   }
}

P6DofVehicle::P6DofVehicle(const P6DofVehicle& aSrc)
   : P6DofObject(aSrc)
   , mBaseName(aSrc.mBaseName)
   , mDerivedFromObjectName(aSrc.mDerivedFromObjectName)
   , mIsCaptive(false)
   , mPosRelToParent_m(aSrc.mPosRelToParent_m)
   , mAngRelToParentYPR_rad(aSrc.mAngRelToParentYPR_rad)
   , mSeparationVel_fps(aSrc.mSeparationVel_fps)
   , mSeparationOmega_rps(aSrc.mSeparationOmega_rps)
   , mSubObjectList(aSrc.mSubObjectList)
   , mAeroCoreObjPtr(aSrc.mAeroCoreObjPtr)
   , mAeroPartList(aSrc.mAeroPartList)
   , mActiveAeroModeName("")    // Set via SetActiveAeroObject()
   , mActiveAeroObjPtr(nullptr) // Set via SetActiveAeroObject()
   , mPropulsionObjPtr(aSrc.mPropulsionObjPtr)
   , mSequencerList(aSrc.mSequencerList)
   , mLastSimTime_nanosec(aSrc.mLastSimTime_nanosec)
   , mStepSize_nanosec(aSrc.mStepSize_nanosec)
   , mMassProperties(aSrc.mMassProperties)
   , mKinematicState(aSrc.mKinematicState)
   , mIntegratorPtr(aSrc.mIntegratorPtr)
   , mIntegratorTypeName(aSrc.mIntegratorTypeName)
   , mGraphicalSupport(aSrc.mGraphicalSupport)
   , mLastIsCaptive(aSrc.mLastIsCaptive)
   , mCrashed(aSrc.mCrashed)
   , mEngineOn(aSrc.mEngineOn)
   , mAfterburnerOn(aSrc.mAfterburnerOn)
   , mSmoking(aSrc.mSmoking)
   , mOnFire(aSrc.mOnFire)
   , mLifeTime_nanosec(aSrc.mLifeTime_nanosec)
   , mTimeOfFlight_nanosec(aSrc.mTimeOfFlight_nanosec)
   , mSizeFactor(aSrc.mSizeFactor)
   , mFixedObject(aSrc.mFixedObject)
   , mMaxMach(aSrc.mMaxMach)
   , mMaxAlpha_deg(aSrc.mMaxAlpha_deg)
   , mMinAlpha_deg(aSrc.mMinAlpha_deg)
   , mMaxBeta_deg(aSrc.mMaxBeta_deg)
   , mCgForce_lbs(aSrc.mCgForce_lbs)
   , mCgMoment_ftlbs(aSrc.mCgMoment_ftlbs)
   , mTargetWCSLocation_m(aSrc.mTargetWCSLocation_m)
   , mTargetWCSLocationValid(aSrc.mTargetWCSLocationValid)
   , mInitialLat(aSrc.mInitialLat)
   , mInitialLon(aSrc.mInitialLon)
   , mInitialAlt_m(aSrc.mInitialAlt_m)
   , mDownRangeDist_m(aSrc.mDownRangeDist_m)
   , mThrustTerminationTime_sec(aSrc.mThrustTerminationTime_sec)
   , mThrustTerminationTimeValid(aSrc.mThrustTerminationTimeValid)
   , mFlightControlsPtr(aSrc.mFlightControlsPtr)
   , mPilotManagerPtr(aSrc.mPilotManagerPtr)
   , mLandingGearPtr(aSrc.mLandingGearPtr)
   , mQuaternion(aSrc.mQuaternion)
   , mInitSetupMode(aSrc.mInitSetupMode)
   , mUseWashInInitConditions(aSrc.mUseWashInInitConditions)
   , mTestingSimTime_nanosec(aSrc.mTestingSimTime_nanosec)
   , mPrepositionLat(aSrc.mPrepositionLat)
   , mPrepositionLon(aSrc.mPrepositionLon)
   , mDamageSmokeTrailingEffect(aSrc.mDamageSmokeTrailingEffect)
   , mFlamesPresent(aSrc.mFlamesPresent)
   , mIsDestroyed(aSrc.mIsDestroyed)
   , mTestingIgnoreAllCrashes(aSrc.mTestingIgnoreAllCrashes)
   , mTestingClampToSealLevelMinAlt(aSrc.mTestingClampToSealLevelMinAlt)
   , mHandleAileronLeft(aSrc.mHandleAileronLeft)
   , mHandleAileronRight(aSrc.mHandleAileronRight)
   , mHandleFlapLeft(aSrc.mHandleFlapLeft)
   , mHandleFlapRight(aSrc.mHandleFlapRight)
   , mHandleSpoilerLeft(aSrc.mHandleSpoilerLeft)
   , mHandleSpoilerRight(aSrc.mHandleSpoilerRight)
   , mHandleStabilizerLeft(aSrc.mHandleStabilizerLeft)
   , mHandleStabilizerRight(aSrc.mHandleStabilizerRight)
   , mHandleElevator(aSrc.mHandleElevator)
   , mHandleRudderLeft(aSrc.mHandleRudderLeft)
   , mHandleRudder(aSrc.mHandleRudder)
   , mHandleRudderRight(aSrc.mHandleRudderRight)
   , mHandleSpeedBrake(aSrc.mHandleSpeedBrake)
   , mHandleLandingGear(aSrc.mHandleLandingGear)
   , mHandleLandingGearNose(aSrc.mHandleLandingGearNose)
   , mHandleLandingGearMainLeft(aSrc.mHandleLandingGearMainLeft)
   , mHandleLandingGearMainRight(aSrc.mHandleLandingGearMainRight)
   , mStallAngle(aSrc.mStallAngle)
   , mStallDetectionDeltaAngle(aSrc.mStallDetectionDeltaAngle)
{
   mCurrentTarget.isValid                      = aSrc.mCurrentTarget.isValid;
   mCurrentTarget.targetName                   = aSrc.mCurrentTarget.targetName;
   mCurrentTarget.targetAziumth_deg            = aSrc.mCurrentTarget.targetAziumth_deg;
   mCurrentTarget.targetElevation_deg          = aSrc.mCurrentTarget.targetElevation_deg;
   mCurrentTarget.targetSlantRange_nm          = aSrc.mCurrentTarget.targetSlantRange_nm;
   mCurrentTarget.guidanceYawAccelCommand_g    = aSrc.mCurrentTarget.guidanceYawAccelCommand_g;
   mCurrentTarget.guidancePitchAccelCommand_g  = aSrc.mCurrentTarget.guidancePitchAccelCommand_g;
   mCurrentTarget.guidanceGBias_g              = aSrc.mCurrentTarget.guidanceGBias_g;
   mCurrentTarget.guidanceMaxG                 = aSrc.mCurrentTarget.guidanceMaxG;
   mCurrentTarget.guidanceFlightPathAngle_deg  = aSrc.mCurrentTarget.guidanceFlightPathAngle_deg;
   mCurrentTarget.autopilotYawGLoadCommand_g   = aSrc.mCurrentTarget.autopilotYawGLoadCommand_g;
   mCurrentTarget.autopilotPitchGLoadCommand_g = aSrc.mCurrentTarget.autopilotPitchGLoadCommand_g;
   mCurrentTarget.autopilotRollRateCommand_dps = aSrc.mCurrentTarget.autopilotRollRateCommand_dps;

   mNumTracks = aSrc.mNumTracks;
   for (int i = 0; i < 10; ++i)
   {
      mTrackData[i].sensorName  = aSrc.mTrackData[i].sensorName;
      mTrackData[i].targetName  = aSrc.mTrackData[i].targetName;
      mTrackData[i].targetLat   = aSrc.mTrackData[i].targetLat;
      mTrackData[i].targetLon   = aSrc.mTrackData[i].targetLon;
      mTrackData[i].targetAlt_m = aSrc.mTrackData[i].targetAlt_m;
   }

   SetActiveAeroObject(mAeroCoreObjPtr);

   for (auto& objIter : mSubObjectList)
   {
      objIter->SetParentVehicle(this);
   }

   if (mFlightControlsPtr)
   {
      mFlightControlsPtr->SetParentVehicle(this);
   }

   if (mPilotManagerPtr)
   {
      mPilotManagerPtr->SetParentVehicle(this);
   }

   if (mLandingGearPtr)
   {
      mLandingGearPtr->SetParentVehicle(this);
   }

   if (mPropulsionObjPtr)
   {
      mPropulsionObjPtr->SetParentVehicle(this);
   }

   for (auto& sequencer : mSequencerList)
   {
      sequencer->SetParentVehicle(this);
   }
}

P6DofVehicle* P6DofVehicle::Clone() const
{
   return new P6DofVehicle(*this);
}

void P6DofVehicle::SetParentVehicle(P6DofVehicle* aParentVehicle)
{
   P6DofObject::SetParentVehicle(aParentVehicle);

   SetParentVehicleForComponents();
}

void P6DofVehicle::SetParentVehicleForComponents()
{
   for (auto& i : mSubObjectList)
   {
      i->SetParentVehicle(this);
   }

   for (auto& i : mSequencerList)
   {
      i->SetParentVehicle(this);
   }

   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->SetParentVehicle(this);
   }

   if (mLandingGearPtr != nullptr)
   {
      mLandingGearPtr->SetParentVehicle(this);
   }

   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->SetParentVehicle(this);
   }

   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->SetParentVehicle(this);
   }
}

void P6DofVehicle::SetNamePrefix(const std::string& aPrefixName)
{
   if (mBaseName.empty())
   {
      // No base name
      SetName(aPrefixName);
   }
   else
   {
      // Using base name
      SetName(aPrefixName + "_" + mBaseName);
   }
}

void P6DofVehicle::DeriveFromObjectType(const P6DofVehicleType* aObjectType, const std::string& aPrefixName)
{
   mDerivedFromObjectName = aObjectType->GetName();

   // Set the base name
   mBaseName = aObjectType->GetSubobjectBaseName();

   // Set the full name
   SetNamePrefix(aPrefixName);

   mPosRelToParent_m.Set(aObjectType->mPosRelToParent_m);
   mAngRelToParentYPR_rad.Set(aObjectType->mAngRelToParentYPR_rad);
   mSeparationVel_fps.Set(aObjectType->mSeparationVel_fps);
   mSeparationOmega_rps.Set(aObjectType->mSeparationOmega_rps);

   mSizeFactor   = aObjectType->mSizeFactor;
   mFixedObject  = aObjectType->mFixedObject;
   mMaxMach      = aObjectType->mMaxMach;
   mMaxAlpha_deg = aObjectType->mMaxAlpha_deg;
   mMinAlpha_deg = aObjectType->mMinAlpha_deg;
   mMaxBeta_deg  = aObjectType->mMaxBeta_deg;

   // TODO Iterates over aObjectType->mSubObjectList, but does nothing with the subObject in
   // aObjectType -- is this intended?
   auto objIter = aObjectType->mSubObjectList.begin();
   while (objIter != aObjectType->mSubObjectList.end())
   {
      // Create the instance for the subobject, deriving it from its type
      auto subObj = ut::make_unique<P6DofVehicle>(GetScenario());

      // TODO -- Need to verify that the DeriveFrom declared in P6DofObject should not
      // exist. Once confirmed, this should be removed.
      // subObj->DeriveFrom(*objIter);

      // Set the subobject's parent and captive state
      subObj->SetParentVehicle(this);
      subObj->SetCaptiveState(true);
      subObj->mLastIsCaptive = true;

      // Add the subobject to the list
      AddSubObject(std::move(subObj));
      ++objIter;
   }

   if (aObjectType->mAeroCoreObjPtr != nullptr)
   {
      mAeroCoreObjPtr = ut::clone(aObjectType->mAeroCoreObjPtr);
      SetActiveAeroObject(mAeroCoreObjPtr);
   }

   // ...........................................................

   for (auto& part : aObjectType->mAeroPartList)
   {
      mAeroPartList.emplace_back(ut::clone(part));
   }

   if (aObjectType->mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr = ut::clone(aObjectType->mPilotManagerPtr);
      mPilotManagerPtr->SetParentVehicle(this);
   }

   if (aObjectType->mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr = ut::clone(aObjectType->mFlightControlsPtr);
      mFlightControlsPtr->SetParentVehicle(this);
   }

   if (aObjectType->mLandingGearPtr != nullptr)
   {
      mLandingGearPtr = ut::clone(aObjectType->mLandingGearPtr);
      mLandingGearPtr->SetParentVehicle(this);
   }

   if (aObjectType->mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr = ut::clone(aObjectType->mPropulsionObjPtr);
      mPropulsionObjPtr->SetParentVehicle(this);
   }

   for (auto& sequencer : aObjectType->mSequencerList)
   {
      // Create the new sequencer
      auto newSequencer = ut::clone(sequencer);
      newSequencer->SetParentVehicle(this);

      // Adjust the naming in the sequencers to use full names
      newSequencer->AdjustNaming(GetName());

      // Add the new, copied sequencer to the list
      mSequencerList.emplace_back(std::move(newSequencer));
   }

   mMassProperties     = aObjectType->mMassProperties;
   mKinematicState     = aObjectType->mKinematicState;
   mIntegratorTypeName = aObjectType->mIntegratorTypeName;

   if (mIntegratorPtr == nullptr)
   {
      // Try to get an integrator with the desired type name
      mIntegratorPtr = GetScenario()->GetTypeManager()->GetIntegratorByName(mIntegratorTypeName);

      if (mIntegratorPtr == nullptr)
      {
         { // RAII block
            auto out = ut::log::warning() << "Integrator is null in P6DofVehicle::DeriveFromObjectType().";
            out.AddNote() << "Integrator: " << mIntegratorTypeName;
            out.AddNote() << "Vehicle: " << GetName();
         }

         // Get the default integrator
         mIntegratorPtr = GetScenario()->GetTypeManager()->GetIntegratorByName("DEFAULT");

         if (mIntegratorPtr == nullptr)
         {
            std::string exceptionStr = "No integrator available for object = " + GetName();
            throw(UtException(exceptionStr));
         }
      }
   }

   mGraphicalSupport = aObjectType->mGraphicalSupport;
}

void P6DofVehicle::SetActiveAeroObject(P6DofAeroCoreObject* aObject)
{
   if (aObject != nullptr)
   {
      mActiveAeroObjPtr   = aObject;
      mActiveAeroModeName = mActiveAeroObjPtr->GetModeName();
   }
}

void P6DofVehicle::SetActiveAeroObject(const std::string& aName)
{
   SetActiveAeroObject(mAeroCoreObjPtr->GetSubModeByName(aName));
}

bool P6DofVehicle::Initialize(double aSimTime_sec, const P6DofVehicleType* aObjectType, const std::string& aPrefixName)
{
   // Derive this object instance from an object type
   DeriveFromObjectType(aObjectType, aPrefixName);

   return InitializeDetails(aSimTime_sec);
}

bool P6DofVehicle::InitializeDetails(double aSimTime_sec)
{
   const P6DofVehicleType* objectType = GetScenario()->GetTypeManager()->GetObjectTypeByName(mDerivedFromObjectName);

   // Initialize sub-objects
   auto objTypeIter = objectType->mSubObjectList.begin();
   if (objTypeIter != objectType->mSubObjectList.end())
   {
      // Loop through subobject list
      auto iter = mSubObjectList.begin();
      while (iter != mSubObjectList.end())
      {
         if (objTypeIter == objectType->mSubObjectList.end())
         {
            // Ran out of sub-objects (this should never happen)
            auto out = ut::log::error() << "Insufficient sub-objects to initialize P6DofVehicle.";
            out.AddNote() << "Vehicle: " << GetName();
            break;
         }

         // Set the sub-object's parent and captive state
         (*iter)->SetCaptiveState(true);
         (*iter)->mLastIsCaptive = true;

         P6DofVehicleType* objType = (*objTypeIter);

         (*iter)->Initialize(aSimTime_sec, objType, GetName());

         // Increment the two iterators
         ++iter;
         ++objTypeIter;
      }
   }

   // Set spherical earth flag, if used (default is false, indicating use WGS84)
   SetSphericalEarthFlagOnSubobjects(mKinematicState.UseSphericalEarth());

   // Set the reference earth radius
   mKinematicState.SetRefEarthRadius();

   // Set the last time to the 'start time'
   mLastSimTime_nanosec    = P6DofUtils::TimeToTime(aSimTime_sec);
   int64_t simTime_nanosec = mLastSimTime_nanosec;

   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->Initialize(simTime_nanosec);
   }

   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->SetPilotManager(mPilotManagerPtr);
      if (mPilotManagerPtr != nullptr)
      {
         mFlightControlsPtr->SetPilotObject(mPilotManagerPtr->GetActivePilot());
      }
      mFlightControlsPtr->Initialize(simTime_nanosec);
   }

   if (mLandingGearPtr != nullptr)
   {
      mLandingGearPtr->Initialize(simTime_nanosec);
   }

   // Set aero reference areas for movable aero objects
   if (mAeroCoreObjPtr != nullptr)
   {
      mAeroCoreObjPtr->Initialize();

      double wingChord_ft  = mAeroCoreObjPtr->GetWingChord_ft();
      double wingSpan_ft   = mAeroCoreObjPtr->GetWingSpan_ft();
      double wingArea_sqft = mAeroCoreObjPtr->GetWingArea_sqft();
      double refArea_sqft  = mAeroCoreObjPtr->GetRefArea_sqft();

      for (auto& part : mAeroPartList)
      {
         if (mAeroCoreObjPtr->UsesRefArea())
         {
            part->SetReferenceArea(refArea_sqft);
         }
         else
         {
            part->SetReferenceAreas(wingChord_ft, wingSpan_ft, wingArea_sqft);
         }

         part->Initialize();

         if (mFlightControlsPtr != nullptr)
         {
            size_t handle = mFlightControlsPtr->GetControlSurfaceHandle(part->GetName());
            if (handle == 0)
            {
               auto out = ut::log::warning()
                          << "Unable to get flight controls handle for control surface in P6DofVehicle.";
               out.AddNote() << "Surface: " << part->GetName();
               out.AddNote() << "Vehicle: " << GetName();
            }
            part->SetControlSurfaceHandle(handle);
         }
      }
   }

   // We need to loop through all engines and get the controls handle
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->Initialize(simTime_nanosec);
      mPropulsionObjPtr->SetControlInputHandles(mFlightControlsPtr);
   }

   for (auto& sequencer : mSequencerList)
   {
      sequencer->Initialize(simTime_nanosec);
      sequencer->StartUp(simTime_nanosec);
   }

   // Set the default graphics object
   mGraphicalSupport.SetCurrentObjectGraphicalType(0);

   // Set the integrator
   mIntegratorPtr = GetScenario()->GetTypeManager()->GetIntegratorByName(mIntegratorTypeName);

   // Check for a null integrator
   if (mIntegratorPtr == nullptr)
   {
      { // RAII block
         auto out = ut::log::warning() << "Integrator is null in P6DofVehicle::InitializeDetails().";
         out.AddNote() << "Integrator: " << mIntegratorTypeName;
         out.AddNote() << "Vehicle: " << GetName();
      }

      // Get the default integrator
      mIntegratorPtr = GetScenario()->GetTypeManager()->GetIntegratorByName("DEFAULT");

      if (mIntegratorPtr == nullptr)
      {
         std::string exceptionStr = "No integrator available for object = " + GetName();
         throw(UtException(exceptionStr));
      }
   }

   mSizeFactor.InitializeValues();

   SetParentVehicleForComponents();

   // Determine the vehicle mass/inertia, including sub-objects
   CalculateCurrentMassProperties();

   FindParts();

   CalcStallAngle();

   return true;
}

bool P6DofVehicle::InitializeConditions(double aSimTime_sec)
{
   mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());

   ComputeWashInConditions(aSimTime_sec);

   // Remember the starting lat/lon (used with preposition support)
   mPrepositionLat = mKinematicState.GetLat();
   mPrepositionLon = mKinematicState.GetLon();

   return true;
}

void P6DofVehicle::CalculateWashIn(double aSimTime_sec)
{
   //// This section will handle "wash-in" initial conditions, to provide steady state conditions at start
   // if (mUseWashInInitConditions)
   //{
   //    // We only perform wash-in for objects that lack a parent, so ensure there is no parent
   //    if (mParentVehicle != nullptr)
   //    {
   //       // We have a parent, so we will not wash-in
   //       std::cout << "WARNING: Wash-in will not be performed for " << GetName() << " since it is not a parent
   //       object." << std::endl;
   //    }
   //    else // mParentVehicle is null (which is good in this case)
   //    {
   //  First, be sure to set the setup flag
   mInitSetupMode = true;

   P6DofFreezeFlags* flags = GetScenario()->GetFreezeFlags();

   // Remember the current freeze state
   bool fuelFreeze = flags->GetMasterFreezeFuelBurn();
   bool locFreeze  = flags->GetMasterFreezeLocation();
   bool altFreeze  = flags->GetMasterFreezeAltitude();
   bool spdFreeze  = flags->GetMasterFreezeSpeed();
   bool lagFreeze  = flags->GetMasterNoLagTesting();

   // Utilize freeze during wash-in for fuel burn and location
   flags->SetMasterFreezeFuelBurn(true);
   flags->SetMasterFreezeLocation(true);

   // Note that we do *NOT* use freeze for "no lag testing". This is because doing so would result
   // in "missing" engine lag and the ability to determine when the thrust is stable.

   // Determine the alpha needed for level flight (1g). However, keep in mind that the g-load in
   // level flight will likely not be 1g due to a non-zero pitch angle in level flight.
   double gLoad     = 1.0;
   double alpha_deg = 0;
   CalculateAlphaAtSpecifiedGLoad_deg(gLoad, alpha_deg);

   // Get the "target" conditions
   double tgtVel_mps     = mKinematicState.GetSpeed_mps();
   double tgtAlt_ft      = mKinematicState.GetAlt_ft();
   double tgtVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

   // Get controls data
   P6DofPilotObject* activePilot           = mPilotManagerPtr->GetActivePilot();
   size_t            stickHandle           = activePilot->GetStdStickBackHandle();
   size_t            throttleABHandle      = activePilot->GetStdThrottleABHandle();
   size_t            throttleMilHandle     = activePilot->GetStdThrottleMilHandle();
   double            stickBackControlInput = activePilot->ControlInputValue(stickHandle);
   double            throttleInputAB       = activePilot->ControlInputValue(throttleABHandle);
   double            throttleInputMil      = activePilot->ControlInputValue(throttleMilHandle);
   double            throttleLeverInput    = throttleInputMil + throttleInputAB;

   // These are used for "current conditions"
   double currentVel_mps     = mKinematicState.GetSpeed_mps();
   double currentAlt_ft      = mKinematicState.GetAlt_ft();
   double currentAlpha_deg   = mKinematicState.GetAlpha_deg();
   double currentVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

   // Remember that last conditions for the stick and throttle
   double lastStickBackControlInput = stickBackControlInput;
   double lastThrottleLeverInput    = throttleLeverInput;

   // Remember that last alpha condition
   double lastAlpha_deg = 0.0;

   // Get the initial attitude
   double hdg_rad   = 0;
   double pitch_rad = 0;
   double roll_rad  = 0;
   GetAttitudeNED(hdg_rad, pitch_rad, roll_rad);

   // Adjust pitch using the needed alpha
   pitch_rad += (alpha_deg * UtMath::cRAD_PER_DEG);
   SetAttitudeNED(hdg_rad, pitch_rad, roll_rad);

   // First, do a one second update
   double endTime_sec = aSimTime_sec + 1.0;
   for (double testSimTime_sec = aSimTime_sec; testSimTime_sec <= endTime_sec; testSimTime_sec += 0.01)
   {
      Update(P6DofUtils::TimeToTime(testSimTime_sec));
   }

   // Calc end time using a 120 sec maximum
   double newStartTime_sec = endTime_sec;
   endTime_sec             = aSimTime_sec + 120.0;

   // These are the target limits that are used to determine if conditions have settled sufficiently
   const double deltaAltLimit_ft      = 4.0;
   const double deltaVertSpdLimit_fpm = 0.1;
   const double deltaAlphaLimit_deg   = 0.01;
   const double deltaVelLimit_mps     = 0.05 * UtMath::cMPS_PER_NMPH;
   const double deltaTimeLimit_sec    = 5.0; // Must run for at least 5 seconds
   const double deltaStickLimit       = 0.00025;
   const double deltaThrottleLimit    = 0.015;

   double testSimTime_sec = 0;
   for (testSimTime_sec = newStartTime_sec; testSimTime_sec <= endTime_sec; testSimTime_sec += 0.01)
   {
      Update(P6DofUtils::TimeToTime(testSimTime_sec));

      // Update current conditions
      currentVel_mps     = mKinematicState.GetSpeed_mps();
      currentAlt_ft      = mKinematicState.GetAlt_ft();
      currentAlpha_deg   = mKinematicState.GetAlpha_deg();
      currentVertSpd_fpm = mKinematicState.GetVerticalSpeed_fpm();

      // Update controls data
      stickBackControlInput = activePilot->ControlInputValue(stickHandle);
      throttleInputAB       = activePilot->ControlInputValue(throttleABHandle);
      throttleInputMil      = activePilot->ControlInputValue(throttleMilHandle);
      throttleLeverInput    = throttleInputMil + throttleInputAB;

      // Determine the current "delta" values
      double deltaVel_mps     = fabs(tgtVel_mps - currentVel_mps);
      double deltaAlt_ft      = fabs(tgtAlt_ft - currentAlt_ft);
      double deltaVertSpd_fpm = fabs(tgtVertSpd_fpm - currentVertSpd_fpm);
      double deltaAlpha_deg   = fabs(currentAlpha_deg - lastAlpha_deg);
      double deltaStick       = fabs(stickBackControlInput - lastStickBackControlInput);
      double deltaThrottle    = fabs(throttleLeverInput - lastThrottleLeverInput);
      double deltaTime_sec    = testSimTime_sec - newStartTime_sec;

      if ((deltaAlt_ft < deltaAltLimit_ft) && (deltaVertSpd_fpm < deltaVertSpdLimit_fpm) &&
          (deltaAlpha_deg < deltaAlphaLimit_deg) && (deltaVel_mps < deltaVelLimit_mps) &&
          (deltaStick < deltaStickLimit) && (deltaThrottle < deltaThrottleLimit) && (deltaTime_sec > deltaTimeLimit_sec))
      {
         // Good enough, so break out
         break;
      }

      // Remember the last stick and throttle conditions
      lastStickBackControlInput = stickBackControlInput;
      lastThrottleLeverInput    = throttleLeverInput;

      // Remember the last alpha
      lastAlpha_deg = currentAlpha_deg;
   }

   // Uncomment this block for testing
   // std::cout << "  Wash-in completed in " << testSimTime_sec << " sec" << std::endl;
   // std::cout << "    Delta-Alt   = " << deltaAlt_ft << " ft (target = " << deltaAltLimit_ft << ")" << std::endl;
   // std::cout << "    Delta-VertSpeed = " << deltaVertSpd_fpm << " ft/min (target = " << deltaVertSpdLimit_fpm << ")"
   // << std::endl; std::cout << "    Delta-Alpha = " << deltaAlpha_deg << " deg (target = " << deltaAlphaLimit_deg <<
   // ")" << std::endl; std::cout << "    Delta-Speed = " << deltaVel_mps * UtMath::cNMPH_PER_MPS << " KTAS (target = "
   // << deltaVelLimit_mps * UtMath::cNMPH_PER_MPS << ")" << std::endl; std::cout << "    Stick Back  = " <<
   // stickBackControlInput << "  Delta-Stick = " << deltaStick << "  (target = " << deltaStickLimit << ")" <<
   // std::endl; std::cout << "    Throttle    = " << throttleLeverInput << "  Delta-Throttle = " << deltaThrottle << "
   // (target = " << deltaThrottleLimit << ")" << std::endl; std::cout << "    Thrust = " << currentThrust << " lbs" <<
   // std::endl;

   // Reset the sim time for this object and sub-objects
   SetLastSimTime(aSimTime_sec, true);

   // Restore freeze state
   flags->SetMasterFreezeFuelBurn(fuelFreeze);
   flags->SetMasterFreezeLocation(locFreeze);
   flags->SetMasterFreezeAltitude(altFreeze);
   flags->SetMasterFreezeSpeed(spdFreeze);
   flags->SetMasterNoLagTesting(lagFreeze);

   // Be sure the secondary data is ready
   mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());

   // Clear the setup flag
   mInitSetupMode = false;
}

void P6DofVehicle::CalculateMovableAero(double          aDynPress_lbsqft,
                                        double          aMach,
                                        double          aSpeed_fps,
                                        double          aAlpha_rad,
                                        double          aBeta_rad,
                                        const UtVec3dX& aAngularRates_rps,
                                        UtVec3dX&       aAeroMoment,
                                        double&         aLift,
                                        double&         aDrag,
                                        double&         aSide,
                                        double&         aLiftFactor)
{
   // Set default outputs
   aLift       = 0.0;
   aDrag       = 0.0;
   aSide       = 0.0;
   aLiftFactor = 1.0;

   // Loop through all aero parts and get contributions to F&M
   if (mFlightControlsPtr != nullptr)
   {
      for (auto& part : mAeroPartList)
      {
         // Get the angle
         double angle_deg = mFlightControlsPtr->GetControlSurfaceAngle_deg(part->GetControlSurfaceHandle());

         // Set the angle
         part->SetAngle_deg(angle_deg);

         UtVec3dX localAeroMoment;
         double   localLift       = 0.0;
         double   localDrag       = 0.0;
         double   localSide       = 0.0;
         double   localLiftFactor = 1.0;

         part->CalculateMovableAeroFM(aDynPress_lbsqft,
                                      aMach,
                                      aSpeed_fps,
                                      aAlpha_rad,
                                      aBeta_rad,
                                      aAngularRates_rps,
                                      localAeroMoment,
                                      localLift,
                                      localDrag,
                                      localSide,
                                      localLiftFactor,
                                      angle_deg);

         aLift += localLift;
         aDrag += localDrag;
         aSide += localSide;
         if (localLiftFactor < aLiftFactor)
         {
            aLiftFactor = localLiftFactor;
         }
         aAeroMoment = aAeroMoment + localAeroMoment;
      }
   }
}

void P6DofVehicle::Update(double aSimTime_sec)
{
   // Get the nanosecond version of time
   int64_t targetSimTime_nanosec = P6DofUtils::TimeToTime(aSimTime_sec);

   // Check for no appreciable advance of time -- if so, return
   int64_t deltaT_nanosec = targetSimTime_nanosec - mLastSimTime_nanosec;
   if (deltaT_nanosec < P6DofUtils::cEPSILON_SIMTIME_NANOSEC)
   {
      return;
   }

   // Are there events we need to be aware of between now and the requested time step? Keep going through each
   // event in sequence until we have no events between the current time and the end of the requested time step.
   std::set<int64_t> eventSimTimes_nano;
   int64_t           simTime_nano = mLastSimTime_nanosec;

   if (PendingEventTimes_nanosec(targetSimTime_nanosec, eventSimTimes_nano))
   {
      // Loop through the list of event times
      for (const int64_t& eventTime_nano : eventSimTimes_nano)
      {
         int64_t naturalTimeStepsToEvent = (eventTime_nano - mLastSimTime_nanosec) / mStepSize_nanosec;

         for (int64_t i = 0; i < naturalTimeStepsToEvent; ++i)
         {
            // Increment one time step and update
            simTime_nano += mStepSize_nanosec;
            Update(simTime_nano);
         }

         // To be safe, we avoid catch-up steps here for now. They should
         // be manageable here, but we're trading accuracy for safety.
      }
   }

   int64_t naturalTimeStepsRemaining = (targetSimTime_nanosec - simTime_nano) / mStepSize_nanosec;

   for (int64_t i = 0; i < naturalTimeStepsRemaining; ++i)
   {
      // Increment one time step and update
      simTime_nano += mStepSize_nanosec;
      Update(simTime_nano);
   }

   // Avoid regular catch-up steps - these can negatively affect auto-pilot loops.
}

void P6DofVehicle::Update(int64_t aSimTime_nanosec)
{
   UpdateObject(aSimTime_nanosec);

   if (mIsCaptive)
   {
      // Copy state from parent
      CopyDataFromParent();
      AdjustForParentInducedEffects();
   }

   // WARNING: While looping over subObject and calling Update() on them, the subObject may be removed from the
   // mSubObjectList due to being jettisoned. This will change the mSubObjectList container, invalidating any
   // iterator pointing to removed element.  The following code takes advantage of how list iterators work, the
   // code will not work properly if the container type is changed to vector or another standard type.
   auto subObjIter = mSubObjectList.begin();
   while (subObjIter != mSubObjectList.end())
   {
      P6DofVehicle* vehicle = (*subObjIter);
      ++subObjIter;
      vehicle->Update(aSimTime_nanosec);
   }
}

bool P6DofVehicle::PendingSequencerEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTimes_nanosec)
{
   aEventTimes_nanosec.clear();
   bool valid = false;

   // Add sequencer event times to the set
   for (auto& seqIter : mSequencerList)
   {
      std::set<int64_t> tempEventTimes_nanosec;
      seqIter->PendingTimeEvents(aSimTime_nanosec, tempEventTimes_nanosec);

      for (const int64_t& eventTime : tempEventTimes_nanosec)
      {
         aEventTimes_nanosec.insert(eventTime);
         valid = true;
      }
   }

   return valid;
}

bool P6DofVehicle::PendingGuidanceControlEventTimes_nanosec(int64_t /*aSimTime_nanosec*/,
                                                            std::set<int64_t>& aEventTimes_nanosec)
{
   // TODO -- Support guidance events (especially for ballistic missiles) when available.
   aEventTimes_nanosec.clear();
   return false;
}

bool P6DofVehicle::PendingEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTimes_nanosec)
{
   aEventTimes_nanosec.clear();
   bool valid = false;

   std::set<int64_t> sequencerEventTimes;
   if (PendingSequencerEventTimes_nanosec(aSimTime_nanosec, sequencerEventTimes))
   {
      for (const int64_t& eventTime : sequencerEventTimes)
      {
         aEventTimes_nanosec.insert(eventTime);
      }
      valid = true;
   }

   std::set<int64_t> guidanceControlEventTimes;
   if (PendingGuidanceControlEventTimes_nanosec(aSimTime_nanosec, sequencerEventTimes))
   {
      for (const int64_t& eventTime : guidanceControlEventTimes)
      {
         aEventTimes_nanosec.insert(eventTime);
      }
      valid = true;
   }

   return valid;
}

void P6DofVehicle::UpdateObject(int64_t aSimTime_nanosec)
{
   // Determine the starting LLA
   SetupStartingLatLon();

   // We currently do not use the sequencerTimeEvent, but it could be used to cause an intermediate update within
   // an update frame time./ Determine when the next sequencer time event will occur int64_t sequencerTimeEvent_nanosec
   // = -1; bool sequencerTimeEvent = NextSequencerEventTime_nanosec(aSimTime_nanosec, sequencerTimeEvent_nanosec);

   // Similar to the sequencerTimeEvent, we may need a means to determine when the next guidance-related event will occur.
   // This is important for ballistic missiles with very precise thrust cutoff timing needs. We currently do not handle this.

   // Process sequencers for this time step (if not in setup mode)
   if (!mInitSetupMode)
   {
      for (auto& sequencer : mSequencerList)
      {
         sequencer->Update(aSimTime_nanosec);
      }
   }

   // Return if the time is the same as it was during the last update
   if (aSimTime_nanosec == mLastSimTime_nanosec)
   {
      return;
   }

   // Delta T for target time in nanosec
   int64_t deltaT_nano = aSimTime_nanosec - mLastSimTime_nanosec;

   // dT (delta time) is typically 0.01 sec
   double deltaT_sec = P6DofUtils::TimeToTime(deltaT_nano);

   if ((deltaT_nano < 0) || (deltaT_sec < 0))
   {
      auto out = ut::log::error() << "Delta-T is negative.";
      out.AddNote() << "T = " << P6DofUtils::TimeToTime(aSimTime_nanosec);
      out.AddNote() << "Delta-T: " << deltaT_sec << " sec";
      out.AddNote() << "Vehicle: " << GetName();
      return;
   }

   // Increment the life time and flight times (if not setting up)
   if (!mInitSetupMode)
   {
      mLifeTime_nanosec += deltaT_nano;
      if (!mIsCaptive)
      {
         mTimeOfFlight_nanosec += deltaT_nano;
      }
   }

   // Update the flight controls (which will also update the control inputs)
   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->Update(aSimTime_nanosec, mKinematicState);
   }

   // Set 'last' values
   SetTheLastValues();

   // Set the 'last' sim time to the current time
   mLastSimTime_nanosec = aSimTime_nanosec;

   // Fixed objects do not move, so simply return
   if (mFixedObject)
   {
      return;
   }

   // Determine how the vehicle's size may be changing, especially for parachutes and balloons
   CalculateSizeFactor(deltaT_sec);

   // Call the integrator to update/integrate the state
   mIntegratorPtr->Update(this, aSimTime_nanosec, deltaT_sec);

   // Pass the delta angles to the PilotManager
   if (mPilotManagerPtr != nullptr)
   {
      double deltaYaw_rad, deltaPitch_rad, deltaRoll_rad;
      mKinematicState.GetAngleDeltas(deltaYaw_rad, deltaPitch_rad, deltaRoll_rad);
      mPilotManagerPtr->InputAngleDeltasToPilotObjects(deltaYaw_rad, deltaPitch_rad, deltaRoll_rad);
   }

   // This "handles" various ground contacts, such as crashing, hard landings, etc.
   HandleGroundContact(aSimTime_nanosec);
}

void P6DofVehicle::CalculateAeroBodyFM(UtVec3dX& aLiftBodyVector_lbs,
                                       UtVec3dX& aDragBodyVector_lbs,
                                       UtVec3dX& aSideForceBodyVector_lbs,
                                       UtVec3dX& aMoment_ftlbs,
                                       UtVec3dX& aReferencePt_ft)
{
   if (GetScenario()->GetAtmosphere() == nullptr)
   {
      // Cannot do aero without an atmosphere
      aLiftBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aDragBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aSideForceBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aMoment_ftlbs.Set(0.0, 0.0, 0.0);
      return;
   }

   if (mActiveAeroObjPtr == nullptr)
   {
      // Cannot do aero without a base aero object
      aLiftBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aDragBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aSideForceBodyVector_lbs.Set(0.0, 0.0, 0.0);
      aMoment_ftlbs.Set(0.0, 0.0, 0.0);
      return;
   }

   // Get primary aero vectors (lift, drag and side force)
   UtVec3dX liftBodyUnitVector, dragBodyUnitVector, sideBodyUnitVector;

   mKinematicState.GetBodyAeroVectors(mKinematicState.GetDCM(),
                                      mKinematicState.GetInertialVelocity(),
                                      liftBodyUnitVector,
                                      dragBodyUnitVector,
                                      sideBodyUnitVector);

   // Set the angular rates
   UtVec3dX angularRates_rps = mKinematicState.GetOmegaBody();

   double   lift = 0.0;
   double   drag = 0.0;
   double   side = 0.0;
   UtVec3dX aeroMoment_ftlbs(0., 0., 0.);
   if (mActiveAeroObjPtr != nullptr)
   {
      mActiveAeroObjPtr->CalculateCoreAeroFM(mKinematicState.GetDynamicPressure_psf(),
                                             mKinematicState.GetSpeed_Mach(),
                                             mKinematicState.GetSpeed_fps(),
                                             mKinematicState.GetAlpha_rad(),
                                             mKinematicState.GetBeta_rad(),
                                             mKinematicState.GetAlphaDot_rps(),
                                             mKinematicState.GetBetaDot_rps(),
                                             angularRates_rps,
                                             aeroMoment_ftlbs,
                                             lift,
                                             drag,
                                             side,
                                             mSizeFactor.GetSizeFactorRadius());
   }

   // Movable aero
   UtVec3dX additionalAeroMoment(0., 0., 0.);
   double   additionalLift = 0.0;
   double   additionalDrag = 0.0;
   double   additionalSide = 0.0;
   double   liftFactor     = 1.0;

   CalculateMovableAero(mKinematicState.GetDynamicPressure_psf(),
                        mKinematicState.GetSpeed_Mach(),
                        mKinematicState.GetSpeed_fps(),
                        mKinematicState.GetAlpha_rad(),
                        mKinematicState.GetBeta_rad(),
                        angularRates_rps,
                        additionalAeroMoment,
                        additionalLift,
                        additionalDrag,
                        additionalSide,
                        liftFactor);

   // Add the additional F&M
   aeroMoment_ftlbs += additionalAeroMoment;
   lift += additionalLift;
   drag += additionalDrag;
   side += additionalSide;

   // Adjust lift using factor
   lift *= liftFactor;

   // Calc aero force vectors using the normalized vector and the magnitude
   UtVec3dX liftBodyForce_lbs(liftBodyUnitVector);
   UtVec3dX dragBodyForce_lbs(dragBodyUnitVector);
   UtVec3dX sideBodyForce_lbs(sideBodyUnitVector);

   liftBodyForce_lbs *= lift;
   dragBodyForce_lbs *= drag;
   sideBodyForce_lbs *= side;

   if (!mSubObjectList.empty())
   {
      P6DofForceAndMomentsObject thisObjectFM;
      UtVec3dX                   zeroVector(0.0, 0.0, 0.0);
      UtVec3dX                   aeroTotalBodyForce_lbs = liftBodyForce_lbs + dragBodyForce_lbs + sideBodyForce_lbs;

      thisObjectFM.MoveRefPoint_ft(zeroVector);
      thisObjectFM.AddForceAndMomentAtReferencePoint(aeroTotalBodyForce_lbs, aeroMoment_ftlbs);

      // Now, get additional F&M from sub-objects
      for (auto& subObjIter : mSubObjectList)
      {
         UtVec3dX subObjectLiftBodyVector_lbs(0.0, 0.0, 0.0);
         UtVec3dX subObjectDragBodyVector_lbs(0.0, 0.0, 0.0);
         UtVec3dX subObjectSideBodyVector_lbs(0.0, 0.0, 0.0);
         UtVec3dX subObjectMoment_ftlbs(0.0, 0.0, 0.0);
         UtVec3dX subObjectReferencePt_ft(0.0, 0.0, 0.0);

         subObjIter->CalculateAeroBodyFM(subObjectLiftBodyVector_lbs,
                                         subObjectDragBodyVector_lbs,
                                         subObjectSideBodyVector_lbs,
                                         subObjectMoment_ftlbs,
                                         subObjectReferencePt_ft);

         UtVec3dX subObjectAeroTotalBodyForce_lbs =
            subObjectLiftBodyVector_lbs + subObjectDragBodyVector_lbs + subObjectSideBodyVector_lbs;
         P6DofForceAndMomentsObject subObjectFM;
         subObjectFM.MoveRefPoint_ft(subObjectReferencePt_ft);
         subObjectFM.AddForceAndMomentAtReferencePoint(subObjectAeroTotalBodyForce_lbs, subObjectMoment_ftlbs);

         // Add the subobject F&M to this object's (the parent's) F&M
         thisObjectFM += subObjectFM;
      }

      // Get the total F&M vectors
      UtVec3dX totalAeroTotalBodyForce_lbs;
      UtVec3dX totalAeroMoment_ftlbs;
      thisObjectFM.GetForceAndMomentAtCurrentRefPoint(totalAeroTotalBodyForce_lbs, totalAeroMoment_ftlbs);
      // We need to get the total aero force components

      // Get the total lift vector
      double   dotLift                = liftBodyUnitVector.Dot(totalAeroTotalBodyForce_lbs);
      UtVec3dX totalLiftBodyForce_lbs = liftBodyUnitVector * dotLift;

      // Get the total drag vector
      double   dotDrag                = dragBodyUnitVector.Dot(totalAeroTotalBodyForce_lbs);
      UtVec3dX totalDragBodyForce_lbs = dragBodyUnitVector * dotDrag;

      // Get the total side force vector
      double   dotSide                     = sideBodyUnitVector.Dot(totalAeroTotalBodyForce_lbs);
      UtVec3dX totalSideForceBodyForce_lbs = sideBodyUnitVector * dotSide;

      // Set the return values
      aLiftBodyVector_lbs      = totalLiftBodyForce_lbs;
      aDragBodyVector_lbs      = totalDragBodyForce_lbs;
      aSideForceBodyVector_lbs = totalSideForceBodyForce_lbs;
      aMoment_ftlbs            = totalAeroMoment_ftlbs;
   }
   else
   {
      // Set the return values
      aLiftBodyVector_lbs      = liftBodyForce_lbs;
      aDragBodyVector_lbs      = dragBodyForce_lbs;
      aSideForceBodyVector_lbs = sideBodyForce_lbs;
      aMoment_ftlbs            = aeroMoment_ftlbs;
   }

   if (mIsCaptive)
   {
      aReferencePt_ft = mPosRelToParent_m * UtMath::cFT_PER_M;
   }
   else
   {
      aReferencePt_ft.Set(0.0, 0.0, 0.0);
   }
}

void P6DofVehicle::CalculatePropulsionFM(int64_t              aSimTime_nanosec,
                                         double               aDeltaT_sec,
                                         P6DofKinematicState& aState,
                                         UtVec3dX&            aInertialPropulsionVector_lbs,
                                         UtVec3dX&            aPropulsionMoment_ftlbs)
{
   if (mPropulsionObjPtr == nullptr)
   {
      aInertialPropulsionVector_lbs.Set(0.0, 0.0, 0.0);
      aPropulsionMoment_ftlbs.Set(0.0, 0.0, 0.0);
      return;
   }

   // Calc the thrust vector by doing an 'update' on the propulsion object and a 'calculate' on propulsion objects of any children
   P6DofForceAndMomentsObject tempThrustFM;
   double                     simTime_sec = P6DofUtils::TimeToTime(aSimTime_nanosec);
   CalculateThrustVectorAndMomentUpdatePropulsion(simTime_sec, aDeltaT_sec, aState, tempThrustFM);

   UtVec3dX thrustVector_lbs(0., 0., 0.);
   UtVec3dX thrustMoment_ftlbs(0., 0., 0.);
   tempThrustFM.GetForceAndMomentAtCurrentRefPoint(thrustVector_lbs, thrustMoment_ftlbs);

   // The F&M above are body-rel - convert to inertial
   aInertialPropulsionVector_lbs = mKinematicState.GetDCM().InverseTransform(thrustVector_lbs);
   aPropulsionMoment_ftlbs       = thrustMoment_ftlbs;
}


void P6DofVehicle::UpdateFuelBurn(int64_t aSimTime_nanosec, double aDeltaT_sec, P6DofKinematicState& aState)
{
   if (mPropulsionObjPtr == nullptr)
   {
      return;
   }

   // Calc the thrust vector by doing an 'update' on the propulsion object and a 'calculate' on propulsion objects of any children
   double simTime_sec = P6DofUtils::TimeToTime(aSimTime_nanosec);
   UpdatePropulsionFuelBurn(simTime_sec, aDeltaT_sec, aState);
}

void P6DofVehicle::CalculateLandingGearFM(int64_t         aSimTime_nanosec,
                                          const UtVec3dX& aNonLandingGearTotalForceVecInertial_lbs,
                                          UtVec3dX&       aInertialTotalGearVector_lbs,
                                          UtVec3dX&       aTotalGearMoment_ftlbs)
{
   if (mLandingGearPtr == nullptr)
   {
      aInertialTotalGearVector_lbs.Set(0.0, 0.0, 0.0);
      aTotalGearMoment_ftlbs.Set(0.0, 0.0, 0.0);
      return;
   }

   // To start, we want to determine the surface normal for our current location (This is for the reference point of the
   // vehicle. Each landing gear will have its own surface normal which may differ slightly). We also get the speed of
   // the surface as well (such as for an aircraft carrier).

   UtVec3dX surfaceUpNEDVec(0., 0., -1.);
   UtVec3dX surfaceSpeedNEDVec_mps(0., 0., 0.);
   if (GetScenario()->GetTerrain() != nullptr)
   {
      double surfaceFrictionMult = 0.0;
      // Get the surface normal and speed of the ground at the vehicle location
      GetScenario()->GetTerrain()->HeightOfTerrain_m(mKinematicState.GetLat(),
                                                     mKinematicState.GetLon(),
                                                     surfaceUpNEDVec,
                                                     surfaceSpeedNEDVec_mps,
                                                     surfaceFrictionMult);
   }

   // We're going to use NED coordinates, since doing so makes things easier to understand and debug
   UtVec3dX nonLandingGearTotalForceVecNED_lbs;
   nonLandingGearTotalForceVecNED_lbs =
      mKinematicState.CalcNEDVecFromInertialVec(aNonLandingGearTotalForceVecInertial_lbs);

   // The friction direction opposes the net force in the surface plane
   UtVec3dX requiredOpposingForceInSurfaceVecNED_lbs = nonLandingGearTotalForceVecNED_lbs;

   // We negate the vector (to oppose the force with friction)
   requiredOpposingForceInSurfaceVecNED_lbs *= -1.0;

   // We have the force vector, but it is not yet in the surface plane. We now get the cross
   // product of the vector and the surface normal
   UtVec3dX vectorInSurfaceNED = requiredOpposingForceInSurfaceVecNED_lbs.Cross(surfaceUpNEDVec);

   // Next, we get the cross product of the surface normal and vectorInSurfaceNED which gives us our
   // desired friction vector in the surface plane.
   requiredOpposingForceInSurfaceVecNED_lbs = surfaceUpNEDVec.Cross(vectorInSurfaceNED);

   // We now know the desired direction of the frictional force in the surface plane, so let's update landing gear.
   mLandingGearPtr->Update(aSimTime_nanosec, requiredOpposingForceInSurfaceVecNED_lbs);

   UtVec3dX landingGearForce_lbs(0., 0., 0.);
   UtVec3dX landingGearMoment_ftlbs(0., 0., 0.);
   mLandingGearPtr->GetForceAndMoments(landingGearForce_lbs, landingGearMoment_ftlbs);

   // The F&M are body-rel
   aInertialTotalGearVector_lbs = mKinematicState.CalcInertialVecFromBodyVec(landingGearForce_lbs);
   aTotalGearMoment_ftlbs       = landingGearMoment_ftlbs;

   // Set crash flag if anything exceeded max compression
   if (mLandingGearPtr->SomethingExceededMaxCompression() && !mCrashed)
   {
      { // RAII block
         auto out = ut::log::info() << "Vehicle crashed. Landing gear exceeded compression limit.";
         out.AddNote() << "T = " << P6DofUtils::TimeToTime(aSimTime_nanosec);
         out.AddNote() << "Vehicle: " << GetName();
      }

      if (!mTestingIgnoreAllCrashes)
      {
         mCrashed = true;
         PerformCrash(aSimTime_nanosec);
      }
   }
}

void P6DofVehicle::PerformCrash(int64_t aSimTime_nanosec)
{
   // Zero the velocity
   double vel[3] = {0.0, 0.0, 0.0};
   mKinematicState.SetVelocitySpherical(vel);
   mKinematicState.SetVelocityWGS(vel);
   mKinematicState.SetOmegaBody(vel);

   // Clamp to seal level, as appropriate
   if (mTestingClampToSealLevelMinAlt)
   {
      ClampToSealLevelMinAlt();
   }

   // Re-calculate parameters that are useful to "outside" objects
   mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());

   // Inform objects that are listening with a callback
   CrashedIntoGround(P6DofUtils::TimeToTime(aSimTime_nanosec));
}

void P6DofVehicle::ClampToSealLevelMinAlt()
{
   // Set altitude to sea level
   double lat   = 0.0;
   double lon   = 0.0;
   double alt_m = 0.0;
   GetLocationLLA(lat, lon, alt_m);
   SetLocationLLA(lat, lon, 0.0);
}

void P6DofVehicle::HandleGroundContact(int64_t aSimTime_nanosec)
{
   // Check if already crashed
   if (mCrashed)
   {
      // Vehicle has already crashed, just return
      return;
   }

   // If we are ignoring all crashes, we just return
   if (mTestingIgnoreAllCrashes)
   {
      // Before we return, we will clamp to sea level, if appropriate
      if (mTestingClampToSealLevelMinAlt)
      {
         // Is the vehicle below sea level?
         if (mKinematicState.GetAlt_m() < 0)
         {
            ClampToSealLevelMinAlt();
            mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());
         }
      }

      return;
   }

   // Default ground level is sea level
   double terrainHgt_m = 0.0;

   // Get the terrain and use its height, if possible
   const P6DofTerrain* terrainPtr = GetScenario()->GetTerrain();
   if (terrainPtr != nullptr)
   {
      terrainHgt_m = terrainPtr->HeightOfTerrain_m(mKinematicState.GetLat(), mKinematicState.GetLon());
   }

   // Check for hit with ground

   if (mKinematicState.GetAlt_m() < terrainHgt_m) // Vehicle has hit the ground
   {
      // Crash!!! -- object has hit the ground
      mKinematicState.SetAlt(terrainHgt_m);

      if (mKinematicState.UseSphericalEarth())
      {
         // Set location with new alt
         UtVec3dX sphericalLocation;
         P6DofUtils::CalcSphericalPositionFromLatLonAlt(mKinematicState.GetLat(),
                                                        mKinematicState.GetLon(),
                                                        mKinematicState.GetAlt_m(),
                                                        sphericalLocation);
         mKinematicState.SetLocationSpherical(sphericalLocation);
      }
      else
      {
         // Set location with new alt
         mKinematicState.GetUtEntity().SetLocationLLA(mKinematicState.GetLat(),
                                                      mKinematicState.GetLon(),
                                                      mKinematicState.GetAlt_m());
         double locWCS[3];
         mKinematicState.GetUtEntity().GetLocationWCS(locWCS);
         mKinematicState.SetLocationWGS(UtVec3dX(locWCS));
      }

      // Zero the velocity
      mKinematicState.ZeroVelocity();

      // Set angular velocity to zero
      double vel[3] = {0.0, 0.0, 0.0};
      mKinematicState.SetOmegaBody(vel);

      // Set crashed flag
      if (!mTestingIgnoreAllCrashes)
      {
         mCrashed = true;
         PerformCrash(aSimTime_nanosec);
      }

   } // End hit ground
}

void P6DofVehicle::CalculateObjectPropulsion(double                      aSimTime_sec,
                                             double                      aDeltaT_sec,
                                             double                      aAlt_ft,
                                             double                      aDynPress_lbsqft,
                                             double                      aStatPress_lbssqft,
                                             double                      aSpeed_fps,
                                             double                      aMach,
                                             double                      aAlpha_rad,
                                             double                      aBeta_rad,
                                             P6DofForceAndMomentsObject& aForceAndMoment,
                                             double&                     aFuelBurnRate_pps,
                                             double&                     aFuelBurned_lbs)
{
   ObjectPropulsionCalculations(aSimTime_sec,
                                aDeltaT_sec,
                                aAlt_ft,
                                aDynPress_lbsqft,
                                aStatPress_lbssqft,
                                aSpeed_fps,
                                aMach,
                                aAlpha_rad,
                                aBeta_rad,
                                aForceAndMoment,
                                aFuelBurnRate_pps,
                                aFuelBurned_lbs,
                                false);
}

void P6DofVehicle::UpdateObjectPropulsion(double                      aSimTime_sec,
                                          double                      aDeltaT_sec,
                                          double                      aAlt_ft,
                                          double                      aDynPress_lbsqft,
                                          double                      aStatPress_lbssqft,
                                          double                      aSpeed_fps,
                                          double                      aMach,
                                          double                      aAlpha_rad,
                                          double                      aBeta_rad,
                                          P6DofForceAndMomentsObject& aForceAndMoment,
                                          double&                     aFuelBurnRate_pps,
                                          double&                     aFuelBurned_lbs)
{
   ObjectPropulsionCalculations(aSimTime_sec,
                                aDeltaT_sec,
                                aAlt_ft,
                                aDynPress_lbsqft,
                                aStatPress_lbssqft,
                                aSpeed_fps,
                                aMach,
                                aAlpha_rad,
                                aBeta_rad,
                                aForceAndMoment,
                                aFuelBurnRate_pps,
                                aFuelBurned_lbs,
                                true);
}

void P6DofVehicle::ObjectPropulsionCalculations(double                      aSimTime_sec,
                                                double                      aDeltaT_sec,
                                                double                      aAlt_ft,
                                                double                      aDynPress_lbsqft,
                                                double                      aStatPress_lbssqft,
                                                double                      aSpeed_fps,
                                                double                      aMach,
                                                double                      aAlpha_rad,
                                                double                      aBeta_rad,
                                                P6DofForceAndMomentsObject& aForceAndMoment,
                                                double&                     aFuelBurnRate_pps,
                                                double&                     aFuelBurned_lbs,
                                                bool                        aUpdateData)
{
   aForceAndMoment.ClearReferenceAndForceAndMoment();
   aFuelBurnRate_pps = 0.0;
   aFuelBurned_lbs   = 0.0;

   if (mPropulsionObjPtr != nullptr)
   {
      if (aUpdateData)
      {
         mPropulsionObjPtr->UpdatePropulsionFM(aDeltaT_sec,
                                               aAlt_ft,
                                               aDynPress_lbsqft,
                                               aStatPress_lbssqft,
                                               aSpeed_fps,
                                               aMach,
                                               aAlpha_rad,
                                               aBeta_rad,
                                               aForceAndMoment,
                                               aFuelBurnRate_pps,
                                               aFuelBurned_lbs);

         // This update is done primarily to update the fuel tanks
         mPropulsionObjPtr->Update(P6DofUtils::TimeToTime(aSimTime_sec));
      }
      else
      {
         mPropulsionObjPtr->CalculatePropulsionFM(aDeltaT_sec,
                                                  aAlt_ft,
                                                  aDynPress_lbsqft,
                                                  aStatPress_lbssqft,
                                                  aSpeed_fps,
                                                  aMach,
                                                  aAlpha_rad,
                                                  aBeta_rad,
                                                  aForceAndMoment,
                                                  aFuelBurnRate_pps,
                                                  aFuelBurned_lbs);
      }
   }

   // At this point, the ref pt is relative to the object. However, if the object is captive,
   // we need it to be relative to the parent object instead.
   if (mIsCaptive)
   {
      // This is captive so, we adjust for the relative position
      aForceAndMoment.RelocateForce_ft(mPosRelToParent_m * UtMath::cFT_PER_M, mAngRelToParentYPR_rad);
   }
}

void P6DofVehicle::AddSubObject(std::unique_ptr<P6DofVehicle> aSubObject)
{
   mSubObjectList.emplace_back(std::move(aSubObject));
}

P6DofVehicle* P6DofVehicle::GetSubobjectByName(const std::string& aSubobjectName) const
{
   for (auto& subObject : mSubObjectList)
   {
      if (subObject->GetName() == aSubobjectName)
      {
         // Match found
         return subObject;
      }
   }

   // No match found, try children
   for (auto& subObject : mSubObjectList)
   {
      P6DofVehicle* childObject = subObject->GetSubobjectByName(aSubobjectName);
      if (childObject != nullptr)
      {
         // Match found
         return childObject;
      }
   }

   return nullptr;
}

const std::list<ut::CloneablePtr<P6DofVehicle>>& P6DofVehicle::GetSubobjectList() const
{
   return mSubObjectList;
}

double P6DofVehicle::GetKCAS() const
{
   // Assume that KCAS is approximately KIAS
   return mKinematicState.GetSpeed_kias();
}

double P6DofVehicle::GetKIAS() const
{
   return mKinematicState.GetSpeed_kias();
}

double P6DofVehicle::GetKTAS() const
{
   return mKinematicState.GetSpeed_ktas();
}

double P6DofVehicle::GetAlt_m() const
{
   return mKinematicState.GetAlt_m();
}

double P6DofVehicle::GetHeading_rad() const
{
   return mKinematicState.GetLocalHeading_rad();
}

double P6DofVehicle::GetHeading_deg() const
{
   return mKinematicState.GetLocalHeading_deg();
}

double P6DofVehicle::GetPitch_rad() const
{
   return mKinematicState.GetLocalPitch_rad();
}

double P6DofVehicle::GetPitch_deg() const
{
   return mKinematicState.GetLocalPitch_deg();
}

double P6DofVehicle::GetRoll_rad() const
{
   return mKinematicState.GetLocalRoll_rad();
}

double P6DofVehicle::GetRoll_deg() const
{
   return mKinematicState.GetLocalRoll_deg();
}

double P6DofVehicle::GetLat() const
{
   return mKinematicState.GetLat();
}

double P6DofVehicle::GetLon() const
{
   return mKinematicState.GetLon();
}

double P6DofVehicle::GetAlpha_rad() const
{
   return mKinematicState.GetAlpha_rad();
}

double P6DofVehicle::GetAlpha_deg() const
{
   return mKinematicState.GetAlpha_deg();
}

double P6DofVehicle::GetBeta_rad() const
{
   return mKinematicState.GetBeta_rad();
}

double P6DofVehicle::GetBeta_deg() const
{
   return mKinematicState.GetBeta_deg();
}

double P6DofVehicle::GetAlphaDot_rps() const
{
   return mKinematicState.GetAlphaDot_rps();
}

double P6DofVehicle::GetAlphaDot_dps() const
{
   return mKinematicState.GetAlphaDot_dps();
}

double P6DofVehicle::GetBetaDot_rps() const
{
   return mKinematicState.GetBetaDot_rps();
}

double P6DofVehicle::GetBetaDot_dps() const
{
   return mKinematicState.GetBetaDot_dps();
}

UtVec3dX P6DofVehicle::GetCg() const
{
   return mMassProperties.GetCmPosRelToRef_ft();
}

double P6DofVehicle::GetMach() const
{
   return mKinematicState.GetSpeed_Mach();
}

double P6DofVehicle::GetVertSpeed_fpm() const
{
   return mKinematicState.GetVerticalSpeed_fpm();
}

double P6DofVehicle::GetYawRate_dps() const
{
   return mKinematicState.GetYawRate_dps();
}

double P6DofVehicle::GetPitchRate_dps() const
{
   return mKinematicState.GetPitchRate_dps();
}

double P6DofVehicle::GetRollRate_dps() const
{
   return mKinematicState.GetRollRate_dps();
}

double P6DofVehicle::GetFlightPathAngle_rad() const
{
   return mKinematicState.GetFlightPathAngle_rad();
}

void P6DofVehicle::CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
                                                double& aCurrentTotalFuel_lbs,
                                                double& aMaxTotalFuel_lbs)
{
   double localCurrentTotalFuelFlow_pph = 0.0;
   double localCurrentTotalFuel_lbs     = 0.0;
   double localMaxTotalFuel_lbs         = 0.0;

   if (mPropulsionObjPtr != nullptr)
   {
      for (const auto& i : mPropulsionObjPtr->GetFuelTanks())
      {
         localCurrentTotalFuel_lbs += i.second->GetCurrentFuelQuantity_lbs();
         localMaxTotalFuel_lbs += i.second->GetFuelCapacity_lbs();
      }

      // Loop through thrust producers
      for (auto& thrustProducer : mPropulsionObjPtr->GetThrustProducerList())
      {
         // Calc the fuel burn rate
         double fuelBurn_pph = thrustProducer->GetFuelBurnRate_pph();
         localCurrentTotalFuelFlow_pph += fuelBurn_pph;

         // Get propellant weight if we have a solid rocket
         P6DofRocketSolidPropellantObject* solidRocket = dynamic_cast<P6DofRocketSolidPropellantObject*>(thrustProducer);
         if (solidRocket != nullptr)
         {
            double currentTotalPropellant_lbs = solidRocket->GetCurrentPropellant_lbs();
            double maxTotalPropellant_lbs     = solidRocket->GetMaxPropellant_lbs();
            localCurrentTotalFuel_lbs += currentTotalPropellant_lbs;
            localMaxTotalFuel_lbs += maxTotalPropellant_lbs;
         }
      }
   }

   // Loop through sub-objects
   for (auto& subObject : mSubObjectList)
   {
      double subobjectCurrentTotalFuelFlow_pph = 0.0;
      double subobjectCurrentTotalFuel_lbs     = 0.0;
      double subobjectMaxTotalFuel_lbs         = 0.0;

      subObject->CalcTotalFuelFlowAndQuantity(subobjectCurrentTotalFuelFlow_pph,
                                              subobjectCurrentTotalFuel_lbs,
                                              subobjectMaxTotalFuel_lbs);

      localCurrentTotalFuelFlow_pph += subobjectCurrentTotalFuelFlow_pph;
      localCurrentTotalFuel_lbs += subobjectCurrentTotalFuel_lbs;
      localMaxTotalFuel_lbs += subobjectMaxTotalFuel_lbs;
   }

   // Return results
   aCurrentTotalFuelFlow_pph = localCurrentTotalFuelFlow_pph;
   aCurrentTotalFuel_lbs     = localCurrentTotalFuel_lbs;
   aMaxTotalFuel_lbs         = localMaxTotalFuel_lbs;
}

void P6DofVehicle::GetLocationWCS(double& aWcsPosX_m, double& aWcsPosY_m, double& aWcsPosZ_m) const
{
   if (mKinematicState.UseSphericalEarth())
   {
      double lat_deg = 0.0;
      double lon_deg = 0.0;
      double alt_m   = 0.0;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat_deg, lon_deg, alt_m);
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(lat_deg, lon_deg, alt_m);
      double locWCS[3];
      tempEntity.GetLocationWCS(locWCS);
      aWcsPosX_m = locWCS[0];
      aWcsPosY_m = locWCS[1];
      aWcsPosZ_m = locWCS[2];
   }
   else
   {
      mKinematicState.GetLocationWGS().Get(aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m);
   }
}

void P6DofVehicle::GetLocationSpherical(double& aSphericalPosX_m, double& aSphericalPosY_m, double& aSphericalPosZ_m) const
{
   if (mKinematicState.UseSphericalEarth())
   {
      mKinematicState.GetLocationSpherical().Get(aSphericalPosX_m, aSphericalPosY_m, aSphericalPosZ_m);
   }
   else
   {
      UtEntity tempEntity;
      double   locWCS[3];
      mKinematicState.GetLocationWGS().Get(locWCS);
      tempEntity.SetLocationWCS(locWCS);
      double lat, lon, alt;
      tempEntity.GetLocationLLA(lat, lon, alt);
      UtVec3dX sphericalPos_m;
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(lat, lon, alt, sphericalPos_m);
      double locSpherical[3];
      sphericalPos_m.Get(locSpherical);
      aSphericalPosX_m = locSpherical[0];
      aSphericalPosY_m = locSpherical[1];
      aSphericalPosZ_m = locSpherical[2];
   }
}

UtVec3dX P6DofVehicle::GetLocationWCS() const
{
   if (mKinematicState.UseSphericalEarth())
   {
      double lat_deg = 0.0;
      double lon_deg = 0.0;
      double alt_m   = 0.0;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat_deg, lon_deg, alt_m);
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(lat_deg, lon_deg, alt_m);
      double locWCS[3];
      tempEntity.GetLocationWCS(locWCS);
      UtVec3dX wgsVec(locWCS);
      return wgsVec;
   }
   else
   {
      return mKinematicState.GetLocationWGS();
   }
}

void P6DofVehicle::GetVelocityWCS(double& aWcsVelX_mps, double& aWcsVelY_mps, double& aWcsVelZ_mps) const
{
   if (mKinematicState.UseSphericalEarth())
   {
      UtVec3dX NEDVec;
      P6DofUtils::CalcSphericalVecToNED(mKinematicState.GetLocationSpherical(),
                                        mKinematicState.GetVelocitySpherical(),
                                        NEDVec);

      double lat_deg = 0.0;
      double lon_deg = 0.0;
      double alt_m   = 0.0;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat_deg, lon_deg, alt_m);
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(lat_deg, lon_deg, alt_m);
      double velNED[3];
      NEDVec.Get(velNED);
      tempEntity.SetVelocityNED(velNED);
      double velWCS[3];
      tempEntity.GetVelocityWCS(velWCS);
      aWcsVelX_mps = velWCS[0];
      aWcsVelY_mps = velWCS[1];
      aWcsVelZ_mps = velWCS[2];
   }
   else
   {
      mKinematicState.GetVelocityWGS().Get(aWcsVelX_mps, aWcsVelY_mps, aWcsVelZ_mps);
   }
}

UtVec3dX P6DofVehicle::GetVelocityWCS() const
{
   if (mKinematicState.UseSphericalEarth())
   {
      UtVec3dX NEDVec;
      P6DofUtils::CalcSphericalVecToNED(mKinematicState.GetLocationSpherical(),
                                        mKinematicState.GetVelocitySpherical(),
                                        NEDVec);

      double lat_deg = 0.0;
      double lon_deg = 0.0;
      double alt_m   = 0.0;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat_deg, lon_deg, alt_m);
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(lat_deg, lon_deg, alt_m);
      double velNED[3];
      NEDVec.Get(velNED);
      tempEntity.SetVelocityNED(velNED);
      double velWCS[3];
      tempEntity.GetVelocityWCS(velWCS);
      UtVec3dX tempVelWCS(velWCS);

      return tempVelWCS;
   }
   else
   {
      return mKinematicState.GetVelocityWGS();
   }
}

void P6DofVehicle::GetOrientationWCS(double& aWcsYaw_rad, double& aWcsPitch_rad, double& aWcsRoll_rad) const
{
   aWcsYaw_rad   = mKinematicState.GetYawWCS();
   aWcsPitch_rad = mKinematicState.GetPitchWCS();
   aWcsRoll_rad  = mKinematicState.GetRollWCS();
}

void P6DofVehicle::SetLocationWCS(double aWcsPosX_m, double aWcsPosY_m, double aWcsPosZ_m)
{
   if (mKinematicState.UseSphericalEarth())
   {
      // Set position
      UtEntity entity;
      double   wcsLoc[3] = {aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m};
      entity.SetLocationWCS(wcsLoc);
      double lat, lon, alt;
      entity.GetLocationLLA(lat, lon, alt);
      UtVec3dX sphericalPos;
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(lat, lon, alt, sphericalPos);
      mKinematicState.SetLocationSpherical(sphericalPos);
      mKinematicState.SetPositionLLA(lat, lon, alt);
   }
   else
   {
      // Set position
      mKinematicState.SetLocationWGS(UtVec3dX(aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m));

      // Set LLA
      UtEntity entity;
      double   wcsLoc[3] = {aWcsPosX_m, aWcsPosY_m, aWcsPosZ_m};
      entity.SetLocationWCS(wcsLoc);
      double lat, lon, alt;
      entity.GetLocationLLA(lat, lon, alt);
      mKinematicState.SetPositionLLA(lat, lon, alt);
   }
}

void P6DofVehicle::SetVelocityWCS(double aWcsVelX_mps, double aWcsVelY_mps, double aWcsVelZ_mps)
{
   if (mKinematicState.UseSphericalEarth())
   {
      double lat;
      double lon;
      double alt_m;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat, lon, alt_m);

      UtEntity entity;
      entity.SetLocationLLA(lat, lon, alt_m);
      double wcsVel[3] = {aWcsVelX_mps, aWcsVelY_mps, aWcsVelZ_mps};
      entity.SetVelocityWCS(wcsVel);

      double nedVel[3];
      entity.GetVelocityNED(nedVel);

      SetVelocityNED(nedVel[0], nedVel[1], nedVel[2]);
   }
   else
   {
      UtEntity entity;
      double   wcsLoc[3];
      mKinematicState.GetLocationWGS().Get(wcsLoc);
      entity.SetLocationWCS(wcsLoc);
      double wcsVel[3] = {aWcsVelX_mps, aWcsVelY_mps, aWcsVelZ_mps};
      entity.SetVelocityWCS(wcsVel);
      mKinematicState.SetVelocityWGS(UtVec3dX(aWcsVelX_mps, aWcsVelY_mps, aWcsVelZ_mps));

      double nedVel[3];
      entity.GetVelocityNED(nedVel);

      mKinematicState.SetVelocityNED(nedVel[0], nedVel[1], nedVel[2]);
   }
}

void P6DofVehicle::SetVelocitySpherical(double aSphericalX_mps, double aSphericalY_mps, double aSphericalZ_mps)
{
   mKinematicState.SetVelocitySpherical(UtVec3dX(aSphericalX_mps, aSphericalY_mps, aSphericalZ_mps));
}

void P6DofVehicle::SetVelocityNED(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   mKinematicState.SetVelocityNED(aVelNorth_mps, aVelEast_mps, aVelDown_mps);
}

void P6DofVehicle::SetOrientationWCS(double aWcsYaw_rad, double aWcsPitch_rad, double aWcsRoll_rad)
{
   if (mKinematicState.UseSphericalEarth())
   {
      double lat, lon, alt_m;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);

      UtEntity entity;
      entity.SetLocationLLA(lat, lon, alt_m);
      entity.SetOrientationWCS(aWcsYaw_rad, aWcsPitch_rad, aWcsRoll_rad);

      // Set WCS angles
      mKinematicState.SetOrientationWCS(aWcsYaw_rad, aWcsPitch_rad, aWcsRoll_rad);

      double yaw, pitch, roll;
      entity.GetOrientationNED(yaw, pitch, roll);

      // Set DCM
      UtDCM dcm;
      P6DofUtils::SetSphericalDCM(mKinematicState.GetLocationSpherical(), yaw, pitch, roll, dcm);
      mKinematicState.SetDCM(dcm);

      // Set local angles
      UtVec3dX unitVecX;
      UtVec3dX unitVecY;
      UtVec3dX unitVecZ;
      P6DofUtils::CalcUnitVectors(mKinematicState.GetDCM(), unitVecX, unitVecY, unitVecZ);
      UtVec3dX localNorth;
      UtVec3dX localEast;
      UtVec3dX localDown;
      P6DofUtils::CalcLocalNEDVectors(mKinematicState.GetLocationSpherical(), localNorth, localEast, localDown);
      double localHeading_rad, localPitch_rad, localRoll_rad;
      P6DofUtils::CalcLocalAngles(localNorth,
                                  localEast,
                                  localDown,
                                  unitVecX, // unitVecY,
                                  unitVecZ,
                                  localHeading_rad,
                                  localPitch_rad,
                                  localRoll_rad);

      mKinematicState.SetAttitude_rad(localHeading_rad, localPitch_rad, localRoll_rad);
   }
   else
   {
      UtEntity entity;
      double   wcsLoc[3];
      mKinematicState.GetLocationWGS().Get(wcsLoc);
      entity.SetLocationWCS(wcsLoc);

      // Set WCS angles
      mKinematicState.SetOrientationWCS(aWcsYaw_rad, aWcsPitch_rad, aWcsRoll_rad);

      // Set DCM
      mKinematicState.SetDCM(UtDCM(aWcsYaw_rad, aWcsPitch_rad, aWcsRoll_rad));

      // Set local angles
      entity.SetOrientationWCS(aWcsYaw_rad, aWcsPitch_rad, aWcsRoll_rad);
      double heading, pitch, roll;
      entity.GetOrientationNED(heading, pitch, roll);
      mKinematicState.SetAttitude_rad(heading, pitch, roll);
   }
}

void P6DofVehicle::CopyDataFromParent()
{
   mKinematicState = mParentVehicle->mKinematicState;
}

void P6DofVehicle::AdjustForParentInducedEffects()
{
   if (mKinematicState.UseSphericalEarth())
   {
      // Get adjusted position
      UtVec3dX posDeltaVec = mKinematicState.GetDCM().InverseTransform(mPosRelToParent_m);
      mKinematicState.SetLocationSpherical(mKinematicState.GetLocationSpherical() + posDeltaVec);

      // Get new LLA
      double lat, lon, alt_m;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);
   }
   else
   {
      // Get adjusted position
      UtEntity tempEntity;
      double   wcs[3];
      mKinematicState.GetLocationWGS().Get(wcs);
      tempEntity.SetLocationWCS(wcs);
      tempEntity.SetOrientationNED(mKinematicState.GetLocalHeading_rad(),
                                   mKinematicState.GetLocalPitch_rad(),
                                   mKinematicState.GetLocalRoll_rad());
      UtVec3dX posDeltaVec = mKinematicState.GetDCM().InverseTransform(mPosRelToParent_m);
      mKinematicState.SetLocationWGS(mKinematicState.GetLocationWGS() + posDeltaVec);

      // Get new LLA
      mKinematicState.GetLocationWGS().Get(wcs);
      tempEntity.SetLocationWCS(wcs);
      double lat, lon, alt_m;
      tempEntity.GetLocationLLA(lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);
   }

   // Adjust DCM for relative attitude
   double relYaw, relPitch, relRoll;
   mAngRelToParentYPR_rad.Get(relYaw, relPitch, relRoll);
   UtDCM yawDCM(relYaw, 0.0, 0.0);
   UtDCM pitchDCM(0.0, relPitch, 0.0);
   UtDCM rollDCM(0.0, 0.0, relRoll);
   mKinematicState.SetDCM(rollDCM * pitchDCM * yawDCM * mKinematicState.GetDCM());

   // Need to calculate the secondary parameters
   mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());
}

void P6DofVehicle::ImpartSeparationEffects()
{
   UtVec3dX separationVelVec = mSeparationVel_fps;
   UtVec3dX inertialVel(0.0, 0.0, 0.0);
   inertialVel = mKinematicState.GetDCM().InverseTransform(separationVelVec);

   mKinematicState.SetOmegaBody(mKinematicState.GetOmegaBody() + mSeparationOmega_rps);

   // Calc new vel
   if (mKinematicState.UseSphericalEarth())
   {
      mKinematicState.SetVelocitySpherical(mKinematicState.GetVelocitySpherical() + inertialVel);
   }
   else
   {
      mKinematicState.SetVelocityWGS(mKinematicState.GetVelocityWGS() + inertialVel);
   }
}

// TODO - Simplify this function, if possible
P6DofFuelTank* P6DofVehicle::GetFuelTankByName(const std::string&  aName,
                                               const P6DofVehicle* aCallingObject,
                                               int                 aCallingLevel) const
{
   if ((aCallingLevel < 0) || (aCallingLevel > 15))
   {
      auto out = ut::log::error() << "Calling limit reached while vehicle looking for fuel tank.";
      out.AddNote() << "Call Level: " << aCallingLevel;
      out.AddNote() << "Vehicle: " << GetName();
      out.AddNote() << "Looking For: " << aName;
      out.AddNote() << "Called From: " << aCallingObject->GetName();
      return nullptr;
   }

   if (mPropulsionObjPtr != nullptr)
   {
      P6DofFuelTank* tank = mPropulsionObjPtr->GetFuelTank(aName);
      if (tank != nullptr)
      {
         return tank;
      }
   }

   // Separate the SubObject's name and the FuelTanks name
   size_t tokenLocation = aName.find('.');
   if (tokenLocation != std::string::npos)
   {
      std::string subObjName   = aName.substr(0, tokenLocation);
      std::string fuelTankName = "";
      if (tokenLocation + 1 <= aName.length())
      {
         fuelTankName = aName.substr(tokenLocation + 1);
      }

      // Try the children
      for (auto& subObject : mSubObjectList)
      {
         if ((subObject != aCallingObject) && (subObject->GetBaseName() == subObjName))
         {
            // Increment calling level
            ++aCallingLevel;

            // Big change here -- now passing 'this' pointer instead of who called this
            P6DofFuelTank* tank = subObject->GetFuelTankByName(fuelTankName, this, aCallingLevel);

            if (tank != nullptr)
            {
               return tank;
            }

            // Decrement calling level
            aCallingLevel--;
         }
      }
   }

   // Try the parent
   if (aCallingLevel <= 0)
   {
      if ((mParentVehicle != nullptr) && (mParentVehicle != aCallingObject))
      {
         // Increment calling level
         ++aCallingLevel;

         P6DofFuelTank* tank = mParentVehicle->GetFuelTankByName(aName, aCallingObject, aCallingLevel);
         if (tank != nullptr)
         {
            return tank;
         }
      }
   }

   return nullptr;
}

P6DofFuelTank* P6DofVehicle::GetFuelTankByName(const std::string& aName) const
{
   return GetFuelTankByName(aName, this);
}

double P6DofVehicle::GetTotalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (mPropulsionObjPtr != nullptr)
   {
      fuelQuantity_lbs += mPropulsionObjPtr->GetInternalFuelTankCapacity_lbs();
   }

   for (auto& subObj : mSubObjectList)
   {
      fuelQuantity_lbs += subObj->GetTotalFuelTankCapacity_lbs();
   }

   return fuelQuantity_lbs;
}

double P6DofVehicle::GetCurrentTotalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (mPropulsionObjPtr != nullptr)
   {
      fuelQuantity_lbs += mPropulsionObjPtr->GetCurrentInternalFuelTankQuantity_lbs();
   }

   for (auto& subObj : mSubObjectList)
   {
      fuelQuantity_lbs += subObj->GetCurrentTotalFuelTankQuantity_lbs();
   }

   return fuelQuantity_lbs;
}

void P6DofVehicle::AddFuelQuantity_lbs(double aFuelToAdd_lbs)
{
   double fuelQuantityAdded_lbs = 0;

   // First, add fuel internally...
   if (mPropulsionObjPtr != nullptr)
   {
      fuelQuantityAdded_lbs = mPropulsionObjPtr->AddFuelQuantity_lbs(aFuelToAdd_lbs);
   }

   double fuelStillAvailableToAdd_lbs = aFuelToAdd_lbs - fuelQuantityAdded_lbs;

   if (fuelStillAvailableToAdd_lbs > 0)
   {
      // Determine the percent full

      double maxTotal_lbs        = GetTotalFuelTankCapacity_lbs();
      double currentTotal_lbs    = GetCurrentTotalFuelTankQuantity_lbs();
      double maxInternal_lbs     = GetInternalFuelTankCapacity_lbs();
      double currentInternal_lbs = GetCurrentInternalFuelTankQuantity_lbs();
      double maxExternal_lbs     = maxTotal_lbs - maxInternal_lbs;
      double currentExternal_lbs = currentTotal_lbs - currentInternal_lbs;

      if (maxExternal_lbs > 0)
      {
         double currentExternalAfterAdd_lbs = currentExternal_lbs + fuelStillAvailableToAdd_lbs;

         double percentFull = 100.0 * (currentExternalAfterAdd_lbs / maxExternal_lbs);

         if (percentFull < 0)
         {
            percentFull = 0.0;
         }
         if (percentFull > 100)
         {
            percentFull = 100.0;
         }

         double fuelAddedToExternal_lbs = 0;

         // Set percent full for sub-objects
         for (auto& subObjIter : mSubObjectList)
         {
            if (subObjIter->mPropulsionObjPtr != nullptr)
            {
               fuelAddedToExternal_lbs += subObjIter->mPropulsionObjPtr->FillAllTanks(percentFull);
            }
         }
      }
   }
}

void P6DofVehicle::SetFuelInTank_lbs(const std::string& aTankName, double aFuel_lbs)
{
   P6DofFuelTank* tank = GetFuelTankByName(aTankName);

   if (tank != nullptr)
   {
      double maxFuel_lbs = tank->GetFuelCapacity_lbs();

      double fuelToTranfer_lbs = aFuel_lbs;
      if (fuelToTranfer_lbs > maxFuel_lbs)
      {
         fuelToTranfer_lbs = maxFuel_lbs;
      }
      else if (fuelToTranfer_lbs <= 0)
      {
         fuelToTranfer_lbs = 0;
      }

      tank->SetCurrentFuelQuantity(fuelToTranfer_lbs);
   }
}

void P6DofVehicle::EmptyAllInternalTanks()
{
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->EmptyAllTanks();
   }
}

void P6DofVehicle::EmptyAllExternalTanks()
{
   for (auto& subObject : mSubObjectList)
   {
      subObject->EmptyAllInternalTanks();
   }
}

double P6DofVehicle::GetInternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (mPropulsionObjPtr != nullptr)
   {
      fuelQuantity_lbs += mPropulsionObjPtr->GetInternalFuelTankCapacity_lbs();
   }

   return fuelQuantity_lbs;
}

double P6DofVehicle::GetCurrentInternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   if (mPropulsionObjPtr != nullptr)
   {
      fuelQuantity_lbs += mPropulsionObjPtr->GetCurrentInternalFuelTankQuantity_lbs();
   }

   return fuelQuantity_lbs;
}

double P6DofVehicle::GetExternalFuelTankCapacity_lbs() const
{
   double fuelQuantity_lbs = 0;

   // It is important to only consider fuel tanks that lack any associated thrust producers.
   // Otherwise, this capacity might include liquid-propellant missiles, which would be incorrect.
   for (auto& subObj : mSubObjectList)
   {
      // Get the internal fuel of the subobject
      double tmpFuelQuantity_lbs = subObj->GetInternalFuelTankCapacity_lbs();

      if (tmpFuelQuantity_lbs > 0.0)
      {
         // Next, verify that it does *not* have an engine (thrust producer)
         P6DofPropulsionSystem* propSystem = subObj->GetPropulsionSystem();
         if (propSystem != nullptr)
         {
            if (propSystem->GetNumThrustProducers() == 0)
            {
               // This lacks an engine, so it is "just" a fuel tank, so increment total quantity
               fuelQuantity_lbs += tmpFuelQuantity_lbs;
            }
         }
         else
         {
            // This lacks an engine, so it is "just" a fuel tank, so increment total quantity
            fuelQuantity_lbs += tmpFuelQuantity_lbs;
         }
      }
   }

   return fuelQuantity_lbs;
}

double P6DofVehicle::GetCurrentExternalFuelTankQuantity_lbs() const
{
   double fuelQuantity_lbs = 0;

   // It is important to only consider fuel tanks that lack any associated thrust producers.
   // Otherwise, this capacity might include liquid-propellant missiles, which would be incorrect.
   for (auto& subObj : mSubObjectList)
   {
      // Get the internal fuel of the subobject
      double tmpFuelQuantity_lbs = subObj->GetCurrentInternalFuelTankQuantity_lbs();

      if (tmpFuelQuantity_lbs > 0.0)
      {
         // Next, verify that it does *not* have an engine (thrust producer)
         P6DofPropulsionSystem* propSystem = subObj->GetPropulsionSystem();
         if (propSystem != nullptr)
         {
            if (propSystem->GetNumThrustProducers() == 0)
            {
               // This lacks an engine, so it is "just" a fuel tank, so increment total quantity
               fuelQuantity_lbs += tmpFuelQuantity_lbs;
            }
         }
         else
         {
            // This lacks an engine, so it is "just" a fuel tank, so increment total quantity
            fuelQuantity_lbs += tmpFuelQuantity_lbs;
         }
      }
   }

   return fuelQuantity_lbs;
}

P6DofThrustProducerObject* P6DofVehicle::GetThrustProducerObjectByName(const std::string& aName) const
{
   return GetThrustProducerObjectByName(aName, this);
}

// TODO - Simplify this function, if possible
P6DofThrustProducerObject* P6DofVehicle::GetThrustProducerObjectByName(const std::string&  aName,
                                                                       const P6DofVehicle* aCallingObject,
                                                                       int                 aCallingLevel) const
{
   if ((aCallingLevel < 0) || (aCallingLevel > 15))
   {
      auto out = ut::log::error() << "Calling limit reached while vehicle looking for thrust producer.";
      out.AddNote() << "Call Level: " << aCallingLevel;
      out.AddNote() << "Vehicle: " << GetName();
      out.AddNote() << "Looking For: " << aName;
      out.AddNote() << "Called From: " << aCallingObject->GetName();
      return nullptr;
   }

   if (mPropulsionObjPtr != nullptr)
   {
      P6DofThrustProducerObject* thrustProducer = mPropulsionObjPtr->GetThrustProducerObjectByName(aName);
      if (thrustProducer != nullptr)
      {
         return thrustProducer;
      }
   }

   // Try the children
   for (auto& subObject : mSubObjectList)
   {
      if (subObject != aCallingObject)
      {
         // Increment calling level
         ++aCallingLevel;

         // Big change here -- now passing 'this' pointer instead of who called this
         P6DofThrustProducerObject* thrustProducer = subObject->GetThrustProducerObjectByName(aName, this, aCallingLevel);

         if (thrustProducer != nullptr)
         {
            return thrustProducer;
         }

         // Decrement calling level
         aCallingLevel--;
      }
   }

   // Try the parent
   if (aCallingLevel <= 0)
   {
      if ((mParentVehicle != nullptr) && (mParentVehicle != aCallingObject))
      {
         // Increment calling level
         ++aCallingLevel;

         P6DofThrustProducerObject* thrustProducer =
            mParentVehicle->GetThrustProducerObjectByName(aName, aCallingObject, aCallingLevel);
         if (thrustProducer != nullptr)
         {
            return thrustProducer;
         }
      }
   }

   return nullptr;
}

std::unique_ptr<P6DofVehicle> P6DofVehicle::SeparateSubObject(P6DofVehicle* aSubObject)
{
   // Confirm sub object is valid
   auto it =
      std::find_if(mSubObjectList.begin(),
                   mSubObjectList.end(),
                   [aSubObject](const ut::CloneablePtr<P6DofVehicle>& subObj) { return subObj.get() == aSubObject; });
   if (it != mSubObjectList.end())
   {
      // Copy state from parent
      aSubObject->CopyDataFromParent();
      aSubObject->AdjustForParentInducedEffects();

      // Include separation velocities
      aSubObject->ImpartSeparationEffects();

      // Change subobject from captive to free
      aSubObject->mIsCaptive = false;

      aSubObject->mKinematicState.CalculateSecondaryParameters(aSubObject->GetScenario()->GetAtmosphere());

      // Clear the subobject's parent
      aSubObject->SetParentVehicle(nullptr);

      // Remove it from the subobject list
      std::unique_ptr<P6DofVehicle> separatedSubObjectPtr = std::move(*it);
      mSubObjectList.erase(it);
      return separatedSubObjectPtr;
   }

   return nullptr;
}

bool P6DofVehicle::JettisonSubobject(const std::string& aSubobjectName, int64_t aSimTime_nanosec)
{
   return JettisonSubobject(GetSubobjectByName(aSubobjectName), aSimTime_nanosec);
}

bool P6DofVehicle::JettisonSubobject(P6DofVehicle* aSubObject, int64_t aSimTime_nanosec)
{
   // In order to jettison the object, it must be captive
   if (aSubObject != nullptr && aSubObject->IsCaptive())
   {
      auto separatedSubObject = SeparateSubObject(aSubObject);

      if (!mKinematicState.IgnoreJettisonObjects())
      {
         // Update the newly freed object
         aSubObject->Update(aSimTime_nanosec);

         // Callback to notify subscribers of the subobject jettisoned
         SubobjectJettisoned(separatedSubObject);
      }
      return true;
   }
   return false;
}

std::unique_ptr<P6DofVehicle> P6DofVehicle::DirectJettisonOfSubobject(P6DofVehicle* aSubobject, double aSimTime_sec)
{
   // In order to jettison the object, it must be captive
   if (aSubobject != nullptr && aSubobject->IsCaptive())
   {
      auto separatedSubObject = SeparateSubObject(aSubobject);

      if (!mKinematicState.IgnoreJettisonObjects())
      {
         // Get the nanosecond version of time
         int64_t targetSimTime_nanosec = P6DofUtils::TimeToTime(aSimTime_sec);

         // Update the newly freed object
         aSubobject->Update(targetSimTime_nanosec);
      }
      return separatedSubObject;
   }
   return nullptr;
}

void P6DofVehicle::GetPositionRelativeToParent(UtVec3dX& aPosRelToParent_m, UtVec3dX& aAngRelToParentYPR_rad)
{
   aPosRelToParent_m      = mPosRelToParent_m;
   aAngRelToParentYPR_rad = mAngRelToParentYPR_rad;
}

unsigned short P6DofVehicle::GetObjectGraphicalType() const
{
   return mGraphicalSupport.GetCurrentObjectGraphicalType();
}

void P6DofVehicle::SetObjectGraphicalType(int aAltType)
{
   mGraphicalSupport.SetCurrentObjectGraphicalType(aAltType);
}

double P6DofVehicle::GetNx_g() const
{
   return mKinematicState.GetNx_g();
}

double P6DofVehicle::GetNy_g() const
{
   return mKinematicState.GetNy_g();
}

double P6DofVehicle::GetNz_g() const
{
   return mKinematicState.GetNz_g();
}

double P6DofVehicle::GetDynamicPressure_psf() const
{
   return mKinematicState.GetDynamicPressure_psf();
}

double P6DofVehicle::GetStaticPressure_psf() const
{
   return mKinematicState.GetStaticPressure_psf();
}

double P6DofVehicle::MaxPotentialManeuverGLoad() const
{
   if (mPilotManagerPtr == nullptr)
   {
      return 0.0;
   }

   if (mActiveAeroObjPtr != nullptr)
   {
      const P6DofCommonController* controller = mPilotManagerPtr->GetSpecialCommonController();

      if (controller != nullptr)
      {
         double mach  = mKinematicState.GetSpeed_Mach();
         double clMax = 0;
         if (controller->GetCLMaxMach(mach, clMax))
         {
            double lift_lbs = 0.0;
            if (mActiveAeroObjPtr->UsesRefArea())
            {
               lift_lbs = mKinematicState.GetDynamicPressure_psf() * mActiveAeroObjPtr->GetRefArea_sqft() * clMax;
            }
            else
            {
               lift_lbs = mKinematicState.GetDynamicPressure_psf() * mActiveAeroObjPtr->GetWingArea_sqft() * clMax;
            }
            double mass_lbs = mMassProperties.GetMass_lbs();
            double gLoad    = lift_lbs / mass_lbs;
            return gLoad;
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetRemainingFuelPercent() const
{
   if (mPropulsionObjPtr != nullptr)
   {
      double currentTotalFuel_lbs = 0;
      double maxTotalFuel_lbs     = 0;

      for (const auto& i : mPropulsionObjPtr->GetFuelTanks())
      {
         currentTotalFuel_lbs += i.second->GetCurrentFuelQuantity_lbs();
         maxTotalFuel_lbs += i.second->GetFuelCapacity_lbs();
      }
   }
   return 0.0;
}

double P6DofVehicle::GetAltitudeMSL_ft() const
{
   return mKinematicState.GetAlt_ft();
}

double P6DofVehicle::GetRangeSinceStart_km() const
{
   return mKinematicState.GetRangeSinceStart_km();
}

double P6DofVehicle::GetApogee_km() const
{
   return mKinematicState.GetApogee_km();
}

double P6DofVehicle::GetLastNx_g() const
{
   return mKinematicState.GetLastNx_g();
}

double P6DofVehicle::GetLastNy_g() const
{
   return mKinematicState.GetLastNy_g();
}

double P6DofVehicle::GetLastNz_g() const
{
   return mKinematicState.GetLastNz_g();
}

double P6DofVehicle::GetLastDynamicPressure_psf() const
{
   return mKinematicState.GetLastDynamicPressure_psf();
}

double P6DofVehicle::GetLastStaticPressure_psf() const
{
   return mKinematicState.GetLastStaticPressure_psf();
}

double P6DofVehicle::GetLastAltitudeMSL_ft() const
{
   return mKinematicState.GetLastAltitudeMSL_ft();
}

void P6DofVehicle::IgniteObject(int64_t aIgniteTimeInFrame_nanosec)
{
   // Ignite all engines in propulsion object
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->Ignite(aIgniteTimeInFrame_nanosec);
   }

   // TODO -- we may want to consider ignite of sub-objects
}

void P6DofVehicle::ShutdownObject(int64_t aTerminateTime_nanosec)
{
   // Shutdown all engines in propulsion object
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->Shutdown(aTerminateTime_nanosec);
   }

   // TODO -- we may want to consider shutdown of sub-objects
}

void P6DofVehicle::TerminateThrust(int64_t aTerminateTime_nanosec)
{
   // Shutdown all engines in propulsion object
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->Shutdown(aTerminateTime_nanosec);
   }

   // Loop through all sub-objects and terminate thrust
   for (auto& subObject : mSubObjectList)
   {
      subObject->TerminateThrust(aTerminateTime_nanosec);
   }
}

bool P6DofVehicle::IsProducingThrust() const
{
   if (mPropulsionObjPtr == nullptr)
   {
      return false;
   }

   if (mPropulsionObjPtr->IsProducingThrust())
   {
      return true;
   }

   // Loop through all sub-objects
   for (auto& iter : mSubObjectList)
   {
      if (iter->IsProducingThrust())
      {
         return true;
      }
   }

   return false;
}

void P6DofVehicle::EnableThrustVectoring(bool aEnableThrustVectoring)
{
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->EnableThrustVectoring(aEnableThrustVectoring);
   }
}

bool P6DofVehicle::AddFuelTransfer(std::string aFuelTransferName, std::string aSourceTankName, std::string aTargetTankName)
{
   if (mPropulsionObjPtr != nullptr)
   {
      return mPropulsionObjPtr->AddFuelTransferAndConnect(aFuelTransferName, aSourceTankName, aTargetTankName);
   }
   return false;
}

bool P6DofVehicle::RemoveFuelTransfer(std::string aFuelTransferName)
{
   if (mPropulsionObjPtr != nullptr)
   {
      return mPropulsionObjPtr->RemoveFuelTransfer(aFuelTransferName);
   }
   return false;
}

bool P6DofVehicle::SetFuelFeed(std::string aEngineName, std::string aFuelTankName)
{
   if (mPropulsionObjPtr != nullptr)
   {
      return mPropulsionObjPtr->SetFuelFeed(aEngineName, aFuelTankName);
   }
   return false;
}

bool P6DofVehicle::SetFuelFeed(std::string aFuelTankName)
{
   if (mPropulsionObjPtr != nullptr)
   {
      return mPropulsionObjPtr->SetFuelFeed(aFuelTankName);
   }
   return false;
}

P6DofSequencer* P6DofVehicle::GetSequencerByName(const std::string& aName) const
{
   for (auto& sequencer : mSequencerList)
   {
      if (sequencer->GetName() == aName)
      {
         // Match found, return it
         return sequencer;
      }
   }
   return nullptr;
}

void P6DofVehicle::GetAttitudeNED(double& aHeading, double& aPitch, double& aRoll)
{
   if (mKinematicState.UseSphericalEarth())
   {
      P6DofUtils::GetSphericalNEDAngles(mKinematicState.GetDCM(), mKinematicState.GetLocationSpherical(), aHeading, aPitch, aRoll);
   }
   else
   {
      mKinematicState.GetUtEntity().GetOrientationNED(aHeading, aPitch, aRoll);
   }
}

void P6DofVehicle::SetAttitudeNED(double aHeading_rad, double aPitch_rad, double aRoll_rad)
{
   // Set local angles
   mKinematicState.SetAttitude_rad(aHeading_rad, aPitch_rad, aRoll_rad);

   // Ensure proper angle limits
   while (mKinematicState.GetLocalRoll_deg() > 180.0)
   {
      mKinematicState.SetLocalRoll_deg(mKinematicState.GetLocalRoll_deg() - 360.0);
   }
   while (mKinematicState.GetLocalRoll_deg() < -180.0)
   {
      mKinematicState.SetLocalRoll_deg(mKinematicState.GetLocalRoll_deg() + 360.0);
   }

   if (mKinematicState.GetLocalPitch_deg() > 90.0)
   {
      mKinematicState.SetLocalPitch_deg(90.0);
   }
   if (mKinematicState.GetLocalPitch_deg() < -90.0)
   {
      mKinematicState.SetLocalPitch_deg(-90.0);
   }

   while (mKinematicState.GetLocalHeading_deg() > 180.0)
   {
      mKinematicState.SetLocalHeading_deg(mKinematicState.GetLocalHeading_deg() - 360.0);
   }
   while (mKinematicState.GetLocalHeading_deg() < -180.0)
   {
      mKinematicState.SetLocalHeading_deg(mKinematicState.GetLocalHeading_deg() + 360.0);
   }

   if (mKinematicState.UseSphericalEarth())
   {
      // Set the DCM
      double lat, lon, alt_m;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);
      UtDCM dcm;
      //      P6DofUtils::SetSphericalDCM(lat, lon, alt_m, aHeading_rad, aPitch_rad, aRoll_rad, dcm);
      P6DofUtils::SetSphericalDCM(lat, lon, aHeading_rad, aPitch_rad, aRoll_rad, dcm);
      mKinematicState.SetDCM(dcm);

      // Set the WCS angles
      double yaw, pitch, roll;
      P6DofUtils::CalcSphericalWCSAngles(mKinematicState.GetDCM(), yaw, pitch, roll);
      mKinematicState.SetOrientationWCS(yaw, pitch, roll);
   }
   else
   {
      // Set the DCM
      mKinematicState.GetUtEntity().SetLocationLLA(mKinematicState.GetLat(),
                                                   mKinematicState.GetLon(),
                                                   mKinematicState.GetAlt_m());
      mKinematicState.GetUtEntity().SetOrientationNED(mKinematicState.GetLocalHeading_rad(),
                                                      mKinematicState.GetLocalPitch_rad(),
                                                      mKinematicState.GetLocalRoll_rad());
      double psi   = 0.0;
      double theta = 0.0;
      double phi   = 0.0;
      mKinematicState.GetUtEntity().GetOrientationWCS(psi, theta, phi);
      mKinematicState.SetDCM(UtDCM(psi, theta, phi));
      mKinematicState.SetOrientationWCS(psi, theta, phi);
   }
}

void P6DofVehicle::GetLocationLLA(double& aLat, double& aLon, double& aAlt_m)
{
   if (mKinematicState.UseSphericalEarth())
   {
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), aLat, aLon, aAlt_m);
   }
   else
   {
      mKinematicState.GetUtEntity().GetLocationLLA(aLat, aLon, aAlt_m);
   }
}

void P6DofVehicle::SetLocationLLA(double aLat, double aLon, double aAlt_m)
{
   // Set entity LLA orientation
   mKinematicState.SetPositionLLA(aLat, aLon, aAlt_m);

   if (mKinematicState.UseSphericalEarth())
   {
      // THIS IS NEW
      UtVec3dX sphericalPosition;
      P6DofUtils::CalcSphericalPositionFromLatLonAlt(aLat, aLon, aAlt_m, sphericalPosition);
      mKinematicState.SetLocationSpherical(sphericalPosition);

      // Set local orientation
      UtDCM dcm;
      P6DofUtils::SetSphericalDCM(aLat,
                                  aLon,
                                  mKinematicState.GetLocalHeading_rad(),
                                  mKinematicState.GetLocalPitch_rad(),
                                  mKinematicState.GetLocalRoll_rad(),
                                  dcm);
      mKinematicState.SetDCM(dcm);
   }
   else
   {
      // Set the WGS84 position
      mKinematicState.GetUtEntity().SetLocationLLA(aLat, aLon, aAlt_m);
      double vecWCS[3];
      mKinematicState.GetUtEntity().GetLocationWCS(vecWCS);
      mKinematicState.SetLocationWGS(UtVec3dX(vecWCS[0], vecWCS[1], vecWCS[2]));

      // Set local orientation
      mKinematicState.GetUtEntity().SetOrientationNED(mKinematicState.GetLocalHeading_rad(),
                                                      mKinematicState.GetLocalPitch_rad(),
                                                      mKinematicState.GetLocalRoll_rad());
      double psi, theta, phi;
      mKinematicState.GetUtEntity().GetOrientationWCS(psi, theta, phi);
      mKinematicState.SetDCM(UtDCM(psi, theta, phi));
      mKinematicState.SetOrientationWCS(psi, theta, phi);
   }
}

void P6DofVehicle::CalculateSizeFactor(double dT_sec)
{
   if (!mIsCaptive)
   {
      mSizeFactor.CalculateSizeFactor(dT_sec);
   }
}

bool P6DofVehicle::UsingSphericalEarth() const
{
   return mKinematicState.UseSphericalEarth();
}

void P6DofVehicle::SetSphericalEarthFlagOnSubobjects(bool aSetting)
{
   mKinematicState.SetUseSphericalEarth(aSetting);

   // Set sub-objects
   for (auto& iter : mSubObjectList)
   {
      iter->SetSphericalEarthFlagOnSubobjects(aSetting);
   }
}

void P6DofVehicle::CalculateThrustVectorAndMomentUpdatePropulsion(double                      aSimTime_sec,
                                                                  double                      aDeltaTime_sec,
                                                                  P6DofKinematicState&        aState,
                                                                  P6DofForceAndMomentsObject& aForceAndMoment)
{
   if (mPropulsionObjPtr == nullptr)
   {
      aForceAndMoment.ClearReferenceAndForceAndMoment();
      return;
   }

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = aState.GetAlt_ft();
   double speed_fps       = aState.GetSpeed_fps();
   double mach            = aState.GetSpeed_Mach();
   double alpha_rad       = aState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = aState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = aState.GetDynamicPressure_psf();

   P6DofForceAndMomentsObject thrustFM;
   double                     thrustFuelBurnRate_pps = 0.0;
   double                     thrustFuelBurned_lbs   = 0.0;

   // UpdateObjectPropulsion(aSimTime_sec,
   CalculateObjectPropulsion(aSimTime_sec,
                             aDeltaTime_sec,
                             alt_ft,
                             dynPressure_psf,
                             0.0,
                             speed_fps,
                             mach,
                             alpha_rad,
                             beta_rad,
                             thrustFM,
                             thrustFuelBurnRate_pps,
                             thrustFuelBurned_lbs);

   // TODO -- Review this to ensure that "double burn" does not occur. Investigate/test to determine if a double burn
   // might occur if the sub-objects burn here and on their own
   for (auto& subObject : mSubObjectList)
   {
      UtVec3dX                   tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      P6DofForceAndMomentsObject tempThrustFM;
      double                     tempThrustFuelBurnRate_pps = 0.0;
      double                     tempThrustFuelBurned_lbs   = 0.0;

      subObject->CalculateObjectPropulsion(aSimTime_sec,
                                           aDeltaTime_sec,
                                           alt_ft,
                                           dynPressure_psf,
                                           0.0,
                                           speed_fps,
                                           mach,
                                           alpha_rad,
                                           beta_rad,
                                           tempThrustFM,
                                           tempThrustFuelBurnRate_pps,
                                           tempThrustFuelBurned_lbs);
      // Add additional F&M
      thrustFM += tempThrustFM;
   }

   // Copy to output F&M
   aForceAndMoment = thrustFM;
}

void P6DofVehicle::UpdatePropulsionFuelBurn(double aSimTime_sec, double aDeltaTime_sec, P6DofKinematicState& aState)
{
   if (mPropulsionObjPtr == nullptr)
   {
      return;
   }

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = aState.GetAlt_ft();
   double speed_fps       = aState.GetSpeed_fps();
   double mach            = aState.GetSpeed_Mach();
   double alpha_rad       = aState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = aState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = aState.GetDynamicPressure_psf();

   P6DofForceAndMomentsObject thrustFM;
   double                     thrustFuelBurnRate_pps = 0.0;
   double                     thrustFuelBurned_lbs   = 0.0;

   UpdateObjectPropulsion(aSimTime_sec,
                          aDeltaTime_sec,
                          alt_ft,
                          dynPressure_psf,
                          0.0,
                          speed_fps,
                          mach,
                          alpha_rad,
                          beta_rad,
                          thrustFM,
                          thrustFuelBurnRate_pps,
                          thrustFuelBurned_lbs);
}

void P6DofVehicle::GetStartingLatLon(double& aStartingLat, double& aStartingLon) const
{
   mKinematicState.GetStartingLatLon(aStartingLat, aStartingLon);
};

void P6DofVehicle::GetBodyRates_rps(double& xAxis_rps, double& yAxis_rps, double& zAxis_rps) const
{
   mKinematicState.GetOmegaBody().Get(xAxis_rps, yAxis_rps, zAxis_rps);
}

void P6DofVehicle::ZeroBodyRates()
{
   mKinematicState.SetYawRate(0.0);
   mKinematicState.SetPitchRate(0.0);
   mKinematicState.SetRollRate(0.0);
}

void P6DofVehicle::GetBodyAngularAccel_rps2(double& xAxis_rps2, double& yAxis_rps2, double& zAxis_rps2) const
{
   mKinematicState.GetOmegaBodyDot().Get(xAxis_rps2, yAxis_rps2, zAxis_rps2);
}

bool P6DofVehicle::StallWarningAlert() const
{
   if (GetAlpha_rad() > (mStallAngle - mStallDetectionDeltaAngle))
   {
      return true;
   }
   else
   {
      return false;
   }
}

void P6DofVehicle::SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects)
{
   mLastSimTime_nanosec = P6DofUtils::TimeToTime(aSimTime_sec);

   if (aIncludeSubobjects)
   {
      for (auto& subObject : mSubObjectList)
      {
         subObject->SetLastSimTime(aSimTime_sec, true);
      }
   }

   // Set last sim time for components

   mKinematicState.SetLastUpdateTime(mLastSimTime_nanosec);

   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->SetLastSimTime(mLastSimTime_nanosec);
   }

   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->SetLastUpdateSimTime(mLastSimTime_nanosec);
   }

   if (mLandingGearPtr != nullptr)
   {
      mLandingGearPtr->SetLastUpdateSimTime(mLastSimTime_nanosec);
   }

   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->SetLastSimTime(mLastSimTime_nanosec); // This calls SetLastSimTime for all pilot objects
   }
}

void P6DofVehicle::SetupStartingLatLon()
{
   if (mKinematicState.StartingLocationInitialized())
   {
      return;
   }

   if (mIsCaptive)
   {
      // Get data based on parent and relative location
      CopyDataFromParent();
      AdjustForParentInducedEffects();
   }

   if (mKinematicState.UseSphericalEarth())
   {
      double lat, lon, alt_m;
      P6DofUtils::CalcSphericalLatLonAlt(mKinematicState.GetLocationSpherical(), lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);
   }
   else
   {
      double x, y, z;
      mKinematicState.GetLocationWGS().Get(x, y, z);

      double locWCS[3] = {x, y, z};
      mKinematicState.GetUtEntity().SetLocationWCS(locWCS);

      // Get LLA
      double lat, lon, alt_m;
      mKinematicState.GetUtEntity().GetLocationLLA(lat, lon, alt_m);
      mKinematicState.SetPositionLLA(lat, lon, alt_m);
   }

   mKinematicState.SetStartingLatLon(mKinematicState.GetLat(), mKinematicState.GetLon());
   mPrepositionLat = mKinematicState.GetLat();
   mPrepositionLon = mKinematicState.GetLon();
}

void P6DofVehicle::SetTheLastValues()
{
   mKinematicState.SetTheLastValues();
   mLastIsCaptive = mIsCaptive;
}

void P6DofVehicle::SetYawRelToParent_rad(double aYawAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(aYawAngle_rad, p, r);
}

void P6DofVehicle::SetPitchRelToParent_rad(double aPitchAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(y, aPitchAngle_rad, r);
}

void P6DofVehicle::SetRollRelToParent_rad(double aRollAngle_rad)
{
   double y, p, r;
   mAngRelToParentYPR_rad.Get(y, p, r);
   mAngRelToParentYPR_rad.Set(y, p, aRollAngle_rad);
}

UtVec3dX P6DofVehicle::GetVelocityNED_mps() const
{
   UtVec3dX NEDVel_mps;

   if (mKinematicState.UseSphericalEarth())
   {
      P6DofUtils::CalcSphericalVecToNED(mKinematicState.GetLocationSpherical(),
                                        mKinematicState.GetVelocitySpherical(),
                                        NEDVel_mps);
   }
   else
   {
      UtEntity entity;

      double wcsPos[3] = {mKinematicState.GetLocationWGS().X(),
                          mKinematicState.GetLocationWGS().Y(),
                          mKinematicState.GetLocationWGS().Z()};
      double wcsVel[3] = {mKinematicState.GetVelocityWGS().X(),
                          mKinematicState.GetVelocityWGS().Y(),
                          mKinematicState.GetVelocityWGS().Z()};

      entity.SetLocationWCS(wcsPos);
      entity.SetVelocityWCS(wcsVel);

      double velNED[3];
      entity.GetVelocityNED(velNED);
      NEDVel_mps.Set(velNED[0], velNED[1], velNED[2]);
   }

   return NEDVel_mps;
}

UtVec3dX P6DofVehicle::GetAccelNED_mps2() const
{
   UtVec3dX NEDAccel_mps2;

   if (mKinematicState.UseSphericalEarth())
   {
      P6DofUtils::CalcSphericalVecToNED(mKinematicState.GetLocationSpherical(),
                                        mKinematicState.GetInertialAccel(),
                                        NEDAccel_mps2);
   }
   else
   {
      UtEntity entity;

      double wcsPos[3]   = {mKinematicState.GetLocationWGS().X(),
                          mKinematicState.GetLocationWGS().Y(),
                          mKinematicState.GetLocationWGS().Z()};
      double wcsVel[3]   = {mKinematicState.GetVelocityWGS().X(),
                          mKinematicState.GetVelocityWGS().Y(),
                          mKinematicState.GetVelocityWGS().Z()};
      double wcsAccel[3] = {mKinematicState.GetInertialAccel().X(),
                            mKinematicState.GetInertialAccel().Y(),
                            mKinematicState.GetInertialAccel().Z()};

      entity.SetLocationWCS(wcsPos);
      entity.SetVelocityWCS(wcsVel);
      entity.SetAccelerationWCS(wcsAccel);

      double accelNED[3];
      entity.GetAccelerationNED(accelNED);
      NEDAccel_mps2.Set(accelNED[0], accelNED[1], accelNED[2]);
   }

   return NEDAccel_mps2;
}

P6DofPilotObject* P6DofVehicle::GetPilotObject() const
{
   if (mPilotManagerPtr != nullptr)
   {
      return mPilotManagerPtr->GetActivePilot();
   }
   return nullptr;
}

size_t P6DofVehicle::GetInputHandle(const std::string& aInputName) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetBooleanControlHandle(aInputName);
   }
   return 0;
}

bool P6DofVehicle::GetBooleanInput(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlBoolean(aHandle);
   }
   return false;
}

bool P6DofVehicle::GetLastBooleanInput(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlBooleanLastValue(aHandle);
   }
   return false;
}

bool P6DofVehicle::ActivateSequencer(const std::string& aSequencerName)
{
   P6DofSequencer* sequencer = GetSequencerByName(aSequencerName);
   if (sequencer == nullptr)
   {
      ut::log::error() << "Could not find sequencer.";
      return false;
   }
   return sequencer->ActivateExternally();
}

bool P6DofVehicle::ActivateSequencer(const std::string& aSubobjectName, const std::string& aSequencerName)
{
   for (auto& subobject : mSubObjectList)
   {
      if (subobject->GetBaseName() == aSubobjectName)
      {
         // We found the subobject, so try to activate the sequencer
         return subobject->ActivateSequencer(aSequencerName);
      }
   }

   // We did not locate a subobject, so loop on all sub-objects and pass the command
   for (auto& subobject : mSubObjectList)
   {
      bool activated = subobject->ActivateSequencer(aSubobjectName, aSequencerName);
      if (activated)
      {
         // The sequencer was activated, so return true
         return true;
      }
   }

   return false;
}

void P6DofVehicle::TestingUpdate(double aAltitude_ft,
                                 double aSpeed_fps,
                                 double aPitchAngle_deg,
                                 double aStickBack,
                                 double aThrottleForward,
                                 double aFlapsDown)
{
   // Set freeze flags
   GetScenario()->GetFreezeFlags()->SetMasterFreezeLocation(true);
   GetScenario()->GetFreezeFlags()->SetMasterFreezeAltitude(true);
   GetScenario()->GetFreezeFlags()->SetMasterFreezeFuelBurn(true);
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(true);

   P6DofPilotObject* pilot = nullptr;
   if (mPilotManagerPtr != nullptr)
   {
      pilot = mPilotManagerPtr->GetActivePilot();
   }

   if (pilot)
   {
      pilot->TakeTestControl();

      pilot->SetTestFlapsControllerPosition(aFlapsDown);
      pilot->SetTestStickBackControllerPosition(aStickBack);
      // The stick and rudder values are +/-1 while all others are 0-1. Throttle assumes that 0 is idle,
      // 1 is full power without augmentation/afterburner (military power), and 2 is full power with full
      // augmentation/afterburner (full afterburner)
      pilot->SetTestThrottleControllerPosition(aThrottleForward);
   }

   // Clear various state parameters
   UtVec3dX temp(0.0, 0.0, 0.0);
   mKinematicState.SetOmegaBody(temp);
   mKinematicState.SetOmegaBodyDot(temp);
   mKinematicState.SetAlphaBetaDotToZero();

   // Increment testing sim time
   mTestingSimTime_nanosec += 10000000; // 0.01 sec

   // Set testing state
   SetLocationLLA(0, 0, aAltitude_ft * UtMath::cM_PER_FT);
   SetAttitudeNED(0, aPitchAngle_deg * UtMath::cRAD_PER_DEG, 0);
   SetVelocityNED(aSpeed_fps * UtMath::cM_PER_FT, 0, 0);

   // Be sure to update/set secondary parameters such as alpha
   mKinematicState.CalculateSecondaryParameters(GetScenario()->GetAtmosphere());

   Update(mTestingSimTime_nanosec);
}

void P6DofVehicle::ZeroKinematicsDuringTesting()
{
   mKinematicState.ZeroKinematicsDuringTesting();

   P6DofPilotObject* pilot = GetPilotObject();
   if (pilot)
   {
      bool preservedUsingTestControl = pilot->UsingTestControl();
      pilot->TakeTestControl();

      pilot->SetTestStickBackControllerPosition(0.0);
      pilot->SetTestStickRightControllerPosition(0.0);
      pilot->SetTestRudderRightControllerPosition(0.0);

      if (!preservedUsingTestControl)
      {
         pilot->ReleaseTestControl();
      }
   }

   P6DofPropulsionSystem* propulsion = GetPropulsionSystem();
   if (propulsion)
   {
      propulsion->SetThrustVectoringPitch_deg(0.0);
      propulsion->SetThrustVectoringYaw_deg(0.0);
   }

   P6DofFlightControlSystem* fcs = GetFlightControls();
   if (fcs)
   {
      std::vector<double> controlSurfaceAngles, actuatorAngles;
      fcs->GetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngles, actuatorAngles);

      for (auto& controlSurfaceAngle : controlSurfaceAngles)
      {
         controlSurfaceAngle = 0.0;
      }
      for (auto& actuatorAngle : actuatorAngles)
      {
         actuatorAngle = 0.0;
      }
      fcs->SetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngles, actuatorAngles);
   }
}

void P6DofVehicle::ComputeWashInConditions(double aSimTime_sec)
{
   // This section will handle "wash-in" initial conditions, to provide steady state conditions at start
   if (mUseWashInInitConditions)
   {
      // We only perform wash-in for objects that lack a parent, so ensure there is no parent
      if (mParentVehicle != nullptr)
      {
         // We have a parent, so we will not wash-in
         auto out = ut::log::warning() << "Wash-in will not be performed for vehicle since it is not a parent object.";
         out.AddNote() << "Vehicle: " << GetName();
      }
      else // mParentVehicle is null (which is good in this case)
      {
         CalculateWashIn(aSimTime_sec);
      }
   }
}

// This provides the maximum potential thrust available, if full throttle, including
// afterburner (if available), is applied
double P6DofVehicle::GetMaximumPotentialThrust_lbs() const
{
   double maxPotentialThrust_lbs = 0;

   // Check all 'local' thrust producers
   if (mPropulsionObjPtr != nullptr)
   {
      for (auto& thrustProducer : mPropulsionObjPtr->GetThrustProducerList())
      {
         maxPotentialThrust_lbs +=
            thrustProducer->GetMaximumPotentialThrust_lbs(mKinematicState.GetAlt_ft(),
                                                          mKinematicState.GetDynamicPressure_psf(),
                                                          mKinematicState.GetStaticPressure_psf(),
                                                          mKinematicState.GetSpeed_fps(),
                                                          mKinematicState.GetSpeed_Mach(),
                                                          mKinematicState.GetAlpha_deg() * UtMath::cRAD_PER_DEG,
                                                          mKinematicState.GetBeta_deg() * UtMath::cRAD_PER_DEG);
      }
   }

   for (auto& subObject : mSubObjectList)
   {
      maxPotentialThrust_lbs += subObject->GetMaximumPotentialThrust_lbs();
   }

   return maxPotentialThrust_lbs;
}

// This provides the minimum potential thrust available
double P6DofVehicle::GetMinimumPotentialThrust_lbs() const
{
   double minPotentialThrust_lbs = 0;

   // Check all 'local' thrust producers
   if (mPropulsionObjPtr != nullptr)
   {
      for (auto& thrustProducer : mPropulsionObjPtr->GetThrustProducerList())
      {
         minPotentialThrust_lbs +=
            thrustProducer->GetMinimumPotentialThrust_lbs(mKinematicState.GetAlt_ft(),
                                                          mKinematicState.GetDynamicPressure_psf(),
                                                          mKinematicState.GetStaticPressure_psf(),
                                                          mKinematicState.GetSpeed_fps(),
                                                          mKinematicState.GetSpeed_Mach(),
                                                          mKinematicState.GetAlpha_deg() * UtMath::cRAD_PER_DEG,
                                                          mKinematicState.GetBeta_deg() * UtMath::cRAD_PER_DEG);
      }
   }

   // Check all sub-objects
   for (auto& subObject : mSubObjectList)
   {
      minPotentialThrust_lbs += subObject->GetMinimumPotentialThrust_lbs();
   }

   return minPotentialThrust_lbs;
}

double P6DofVehicle::GetCurrentThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const P6DofKinematicState& currentState = mKinematicState;

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   P6DofForceAndMomentsObject thrustFM;
   double                     thrustFuelBurnRate_pps = 0.0;
   double                     thrustFuelBurned_lbs   = 0.0;

   CalculateObjectPropulsion(simTime_sec,
                             deltaTime_sec,
                             alt_ft,
                             dynPressure_psf,
                             0.0,
                             speed_fps,
                             mach,
                             alpha_rad,
                             beta_rad,
                             thrustFM,
                             thrustFuelBurnRate_pps,
                             thrustFuelBurned_lbs);

   // TODO -- Review this to ensure that "double burn" does not occur
   // Investigate/test to determine if a double burn might occur if the sub-objects burn here and on their own

   // Loop through sub-objects
   for (auto& subObject : mSubObjectList)
   {
      UtVec3dX                   tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      P6DofForceAndMomentsObject tempThrustFM;
      double                     tempThrustFuelBurnRate_pps = 0.0;
      double                     tempThrustFuelBurned_lbs   = 0.0;

      subObject->CalculateObjectPropulsion(simTime_sec,
                                           deltaTime_sec,
                                           alt_ft,
                                           dynPressure_psf,
                                           0.0,
                                           speed_fps,
                                           mach,
                                           alpha_rad,
                                           beta_rad,
                                           tempThrustFM,
                                           tempThrustFuelBurnRate_pps,
                                           tempThrustFuelBurned_lbs);

      // Add additional F&M
      thrustFM += tempThrustFM;
   }

   return thrustFM.GetForceMagnitude_lbs();
}

double P6DofVehicle::GetCurrentForwardThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const P6DofKinematicState& currentState = mKinematicState;

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   P6DofForceAndMomentsObject thrustFM;
   double                     thrustFuelBurnRate_pps = 0.0;
   double                     thrustFuelBurned_lbs   = 0.0;

   CalculateObjectPropulsion(simTime_sec,
                             deltaTime_sec,
                             alt_ft,
                             dynPressure_psf,
                             0.0,
                             speed_fps,
                             mach,
                             alpha_rad,
                             beta_rad,
                             thrustFM,
                             thrustFuelBurnRate_pps,
                             thrustFuelBurned_lbs);

   // TODO -- Review this to ensure that "double burn" does not occur
   // Investigate/test to determine if a double burn might occur if the sub-objects burn here and on their own

   // Loop through sub-objects
   for (auto& subObject : mSubObjectList)
   {
      UtVec3dX                   tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX                   tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      P6DofForceAndMomentsObject tempThrustFM;
      double                     tempThrustFuelBurnRate_pps = 0.0;
      double                     tempThrustFuelBurned_lbs   = 0.0;

      subObject->CalculateObjectPropulsion(simTime_sec,
                                           deltaTime_sec,
                                           alt_ft,
                                           dynPressure_psf,
                                           0.0,
                                           speed_fps,
                                           mach,
                                           alpha_rad,
                                           beta_rad,
                                           tempThrustFM,
                                           tempThrustFuelBurnRate_pps,
                                           tempThrustFuelBurned_lbs);

      // Add additional F&M
      thrustFM += tempThrustFM;
   }

   return thrustFM.GetForceAtRefPoint_lbs().X();
}

double P6DofVehicle::GetNormalizedMilitaryThrust() const
{
   if (mPropulsionObjPtr != nullptr)
   {
      P6DofThrustProducerObject* engine = mPropulsionObjPtr->GetThrustProducerByIndex(0);
      if (engine != nullptr)
      {
         P6DofJetEngineObject* jetPtr = dynamic_cast<P6DofJetEngineObject*>(engine);
         if (jetPtr != nullptr)
         {
            // Military thrust is assumed proportional to the engine rpm value
            return jetPtr->GetEnginePercentRPM() * 0.01;
         }
      }
   }

   return 0.0;
}

double P6DofVehicle::GetNormalizedAfterburnerThrust() const
{
   if (mPropulsionObjPtr != nullptr)
   {
      P6DofThrustProducerObject* engine = mPropulsionObjPtr->GetThrustProducerByIndex(0);
      if (engine != nullptr)
      {
         P6DofJetEngineObject* jetPtr = dynamic_cast<P6DofJetEngineObject*>(engine);
         if (jetPtr != nullptr)
         {
            // Afterburner thrust is assumed proportional to the nozzle position
            return jetPtr->GetNozzlePosition();
         }
      }
   }

   return 0.0;
}

double P6DofVehicle::GetNormalizedTotalThrust() const
{
   if (mPropulsionObjPtr != nullptr)
   {
      double thrust = GetNormalizedMilitaryThrust() + GetNormalizedAfterburnerThrust();

      if (thrust < 0.0)
      {
         thrust = 0.0;
      }
      else if (thrust > 2.0)
      {
         thrust = 2.0;
      }

      return thrust;
   }

   return 0.0;
}

double P6DofVehicle::GetTotalVehicleFuelFlow_pph() const
{
   // Get fuel flow by this objects engines
   double fuelFlow_pph = 0.0;

   if (mPropulsionObjPtr != nullptr)
   {
      for (auto& thrustProducer : mPropulsionObjPtr->GetThrustProducerList())
      {
         fuelFlow_pph += thrustProducer->GetFuelBurnRate_pph();
      }
   }

   // Check all sub-objects
   for (auto& subObject : mSubObjectList)
   {
      fuelFlow_pph += subObject->GetTotalVehicleFuelFlow_pph();
   }

   return fuelFlow_pph;
}

double P6DofVehicle::GetTotalFuelFlow_pph() const
{
   double fuelFlow_pph = 0.0;

   if (mPropulsionObjPtr != nullptr)
   {
      for (auto& thrustProducer : mPropulsionObjPtr->GetThrustProducerList())
      {
         fuelFlow_pph += thrustProducer->GetFuelBurnRate_pph();
      }
   }

   return fuelFlow_pph;
}

double P6DofVehicle::GetLift_lbs() const
{
   return mKinematicState.GetLift();
}

double P6DofVehicle::GetDrag_lbs() const
{
   return mKinematicState.GetDrag();
}

double P6DofVehicle::GetSideForce_lbs() const
{
   return mKinematicState.GetSideForce();
}

double P6DofVehicle::GetThrustMagnitude_lbs() const
{
   return mKinematicState.GetThrust();
}

double P6DofVehicle::GetYawMoment_ftlbs() const
{
   return mKinematicState.GetYawMoment();
}

double P6DofVehicle::GetPitchMoment_ftlbs() const
{
   return mKinematicState.GetPitchMoment();
}

double P6DofVehicle::GetRollMoment_ftlbs() const
{
   return mKinematicState.GetRollMoment();
}

double P6DofVehicle::GetSpeed_fps() const
{
   return mKinematicState.GetSpeed_fps();
}

double P6DofVehicle::GetWingArea_sqft() const
{
   if (mActiveAeroObjPtr)
   {
      return mActiveAeroObjPtr->GetWingArea_sqft();
   }

   return 0.0;
}

double P6DofVehicle::GetRefArea_sqft() const
{
   if (mActiveAeroObjPtr)
   {
      return mActiveAeroObjPtr->GetRefArea_sqft();
   }

   return 0.0;
}

void P6DofVehicle::CalculateCmAreaForCoreAndControls(double  aMach,
                                                     double  aAlpha_rad,
                                                     double  aStickBack,
                                                     bool    aPowerOn,
                                                     double& aThrustProduced_lbs,
                                                     double& aCoreCm,
                                                     double& aControlsCm)
{
   // Preserve the original kinematic state altitude and velocity
   double preservedSpeed_fps  = mKinematicState.GetSpeed_fps();
   double preservedAltitude_m = mKinematicState.GetAlt_m();

   // This needs to consider various pure moments as well as induced moments

   P6DofForceAndMomentsObject coreForceMoments_CM;
   P6DofForceAndMomentsObject coreForceMoments_RP;
   P6DofForceAndMomentsObject controlsForceMoments_CM;
   P6DofForceAndMomentsObject controlsForceMoments_RP;

   UtVec3dX cmRef_ft = mMassProperties.GetCmPosRelToRef_ft();
   coreForceMoments_CM.MoveRefPoint_ft(cmRef_ft);
   controlsForceMoments_CM.MoveRefPoint_ft(cmRef_ft);

   UtVec3dX aeroCenter_ft = mActiveAeroObjPtr->GetAeroCenter_ft();
   coreForceMoments_RP.MoveRefPoint_ft(aeroCenter_ft);
   controlsForceMoments_RP.MoveRefPoint_ft(aeroCenter_ft);

   double coreCLArea = 0;
   double coreCdArea = 0;
   double coreCmArea = 0;

   mActiveAeroObjPtr->CalculateAeroCoefficientVectors(aMach, aAlpha_rad, coreCLArea, coreCdArea, coreCmArea);
   double controlsCLArea = 0;
   double controlsCdArea = 0;
   double controlsCmArea = 0;

   CalculateControlsAeroCoefficientVectors(aMach, aAlpha_rad, aStickBack, controlsCLArea, controlsCdArea, controlsCmArea);

   // Set an inertial-based velocity vector, based on coordinate reference
   UtVec3dX inertialVelVector = mKinematicState.GetInertialVelocity();

   if (inertialVelVector.Magnitude() < 0.001)
   {
      inertialVelVector.Set(100.0, 0.0, 0.0);
   }

   // Get primary aero vectors (lift, drag and side force)
   UtVec3dX coreLiftBodyVector;
   UtVec3dX coreDragBodyVector;
   UtVec3dX coreSideBodyVector;

   mKinematicState.GetBodyAeroVectors(mKinematicState.GetDCM(),
                                      inertialVelVector,
                                      coreLiftBodyVector,
                                      coreDragBodyVector,
                                      coreSideBodyVector);

   UtVec3dX controlsLiftBodyVector = coreLiftBodyVector;
   UtVec3dX controlsDragBodyVector = coreDragBodyVector;

   // Use the Mach value at sea level to calculate actual force and moment
   mKinematicState.SetAlt(0.0);
   mKinematicState.SetSpeed_Mach(aMach);

   const P6DofAtmosphere* atmosphere      = mKinematicState.GetAtmospherePtr();
   double                 speed_fps       = atmosphere->CalcFpsFromMach(0.0, std::max(aMach, 0.01));
   double                 dynPressure_psf = atmosphere->CalcDynamicPressure_lbft2(0.0, speed_fps);

   // Multiply by scalars
   coreLiftBodyVector *= dynPressure_psf * coreCLArea;
   coreDragBodyVector *= dynPressure_psf * coreCdArea;
   controlsLiftBodyVector *= dynPressure_psf * controlsCLArea;
   controlsDragBodyVector *= dynPressure_psf * controlsCdArea;

   UtVec3dX coreBodyVector     = coreLiftBodyVector + coreDragBodyVector;
   UtVec3dX controlsBodyVector = controlsLiftBodyVector + controlsDragBodyVector;

   UtVec3dX coreMoment(0.0, dynPressure_psf * coreCmArea, 0.0);
   UtVec3dX controlsMoment(0.0, dynPressure_psf * controlsCmArea, 0.0);

   // Now, we need to add to the F&M objects
   coreForceMoments_RP.AddForceAndMomentAtReferencePoint(coreBodyVector, coreMoment);
   controlsForceMoments_RP.AddForceAndMomentAtReferencePoint(controlsBodyVector, controlsMoment);

   coreForceMoments_CM += coreForceMoments_RP;
   controlsForceMoments_CM += controlsForceMoments_RP;

   if (aPowerOn)
   {
      // Preserve the original thrust vectoring state for all thrust producers
      std::vector<bool> preservedThrustVectoring;
      if (mPropulsionObjPtr)
      {
         preservedThrustVectoring.reserve(mPropulsionObjPtr->GetNumThrustProducers());
         for (auto& engine : mPropulsionObjPtr->GetThrustProducerList())
         {
            preservedThrustVectoring.push_back(engine->ThrustVectoringEnabled());
         }
      }

      // Observe thrust force & moment vectors both with thrust vectoring
      // turned on (core moments plus controls) and off (core moments only)
      mPropulsionObjPtr->Ignite(0);

      EnableThrustVectoring(false);
      P6DofForceAndMomentsObject propulsionCoreForceMoments_RP;
      CalculateThrustVectorAndMomentUpdatePropulsion(0.0, 2.0, mKinematicState, propulsionCoreForceMoments_RP);

      EnableThrustVectoring(true);
      P6DofForceAndMomentsObject propulsionControlsForceMoments_RP;
      CalculateThrustVectorAndMomentUpdatePropulsion(0.0, 2.0, mKinematicState, propulsionControlsForceMoments_RP);

      // Subtract core effects from thrust vectoring F&M to obtain control effect
      UtVec3dX propulsionCoreForce;
      UtVec3dX propulsionCoreMoment;
      propulsionCoreForceMoments_RP.GetForceAndMomentAtCurrentRefPoint(propulsionCoreForce, propulsionCoreMoment);
      propulsionControlsForceMoments_RP.AddForceAndMomentAtReferencePoint(-propulsionCoreForce, -propulsionCoreMoment);

      coreForceMoments_CM += propulsionCoreForceMoments_RP;
      controlsForceMoments_CM += propulsionControlsForceMoments_RP;

      // Restore the original thrust vectoring state for all thrust producers
      if (mPropulsionObjPtr)
      {
         for (size_t engineIdx = 0; engineIdx < mPropulsionObjPtr->GetNumThrustProducers(); engineIdx++)
         {
            bool singlePreservedThrustVectoring = preservedThrustVectoring.at(engineIdx);
            mPropulsionObjPtr->GetThrustProducerByIndex(engineIdx)->SetThrustVectoringEnabled(
               singlePreservedThrustVectoring);
         }
      }

      aThrustProduced_lbs = propulsionCoreForce.Magnitude();
   }
   else
   {
      aThrustProduced_lbs = 0.0;
   }

   UtVec3dX coreMomentAtCM     = coreForceMoments_CM.GetMomentAtRefPoint_ftlbs();
   UtVec3dX controlsMomentAtCM = controlsForceMoments_CM.GetMomentAtRefPoint_ftlbs();

   aCoreCm     = coreMomentAtCM.Y() / dynPressure_psf;
   aControlsCm = controlsMomentAtCM.Y() / dynPressure_psf;

   // Restore the kinematic state
   mKinematicState.SetAlt(preservedAltitude_m);
   mKinematicState.SetSpeed_fps(preservedSpeed_fps);
}

void P6DofVehicle::CalculateCLAreaForCoreAndControls(double  aMach,
                                                     double  aAlpha_rad,
                                                     double  aStickBack,
                                                     double& aCoreCL,
                                                     double& aControlsCL)
{
   aCoreCL     = mActiveAeroObjPtr->CalculateAeroCLArea(aMach, aAlpha_rad);
   aControlsCL = CalculateControlsCLArea(aMach, aAlpha_rad, aStickBack);
}

void P6DofVehicle::CalculateCdAreaForCoreAndControls(double  aMach,
                                                     double  aAlpha_rad,
                                                     double  aStickBack,
                                                     double& aCoreCd,
                                                     double& aControlsCd)
{
   aCoreCd     = mActiveAeroObjPtr->CalculateAeroCdArea(aMach, aAlpha_rad);
   aControlsCd = CalculateControlsCdArea(aMach, aAlpha_rad, aStickBack);
}

void P6DofVehicle::CalculateTotalCmAndCL(double aMach, double aAlpha_rad, double aStickBack, double& aTotalCm, double& aTotalCL)
{
   double coreCm           = 0;
   double controlsCm       = 0;
   double thrustDummyValue = 0.0;
   CalculateCmAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, false, thrustDummyValue, coreCm, controlsCm);

   double coreCL     = 0;
   double controlsCL = 0;
   CalculateCLAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, coreCL, controlsCL);

   P6DofAeroCoreObject* aeroCore = GetActiveAeroObject();
   if (aeroCore != nullptr)
   {
      double area_sqft = 0.0;

      if (aeroCore->UsesRefArea())
      {
         area_sqft = aeroCore->GetRefArea_sqft();
      }
      else
      {
         area_sqft = aeroCore->GetWingArea_sqft();
      }

      if (area_sqft > 0.0)
      {
         aTotalCm = (coreCm + controlsCm) / area_sqft;
         aTotalCL = (coreCL + controlsCL) / area_sqft;
         return;
      }
   }

   aTotalCm = 0.0;
   aTotalCL = 0.0;
}

void P6DofVehicle::CalculateControlsAeroCoefficientVectors(double  aMach,
                                                           double  aAlpha_rad,
                                                           double  aStickBack,
                                                           double& aCLArea,
                                                           double& aCdArea,
                                                           double& aCmArea)
{
   if (mPilotManagerPtr == nullptr)
   {
      aCLArea = 0;
      aCdArea = 0;
      aCmArea = 0;
      return;
   }

   // Get the pilot
   P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();

   if ((mFlightControlsPtr == nullptr) || (pilot == nullptr))
   {
      aCLArea = 0;
      aCdArea = 0;
      aCmArea = 0;
      return;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting();

   // Set no-lag testing
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(true);

   // Save the current control surface angles and actuator angles
   std::vector<double> controlSurfaceAngleList;
   std::vector<double> actuatorAngleList;
   mFlightControlsPtr->GetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);


   // Save the initial stick position
   double initialStickBack = pilot->GetStickBackControllerPosition();

   // We take "testing" control so that we can set the controls as desired
   pilot->TakeTestControl();

   // ---------------------------------------------------------------------------

   // Set the stick position
   pilot->SetTestStickBackControllerPosition(aStickBack);

   int64_t simTime_nanosec = 0;
   mFlightControlsPtr->Update(simTime_nanosec, mKinematicState);

   // ---------------------------------------------------------------------------

   // Loop through all aero parts and get contributions to Cm
   for (auto& tempAeroPart : mAeroPartList)
   {
      // Get the angle
      double surfaceAngle_deg = mFlightControlsPtr->GetControlSurfaceAngle_deg(tempAeroPart->GetControlSurfaceHandle());

      // Set the angle and calc Cm
      // prob do not need this --> tempAeroPart->SetAngle_deg(surfaceAngle_deg);
      aCLArea += tempAeroPart->CalculateMovableAeroCLArea(aMach, aAlpha_rad, surfaceAngle_deg);
      aCdArea += tempAeroPart->CalculateMovableAeroCdArea(aMach, aAlpha_rad, surfaceAngle_deg);
      aCmArea += tempAeroPart->CalculateMovableAeroCmArea(aMach, aAlpha_rad, surfaceAngle_deg);
   }

   // Restore the stick to its initial position and release test control
   pilot->SetTestStickBackControllerPosition(initialStickBack);
   pilot->ReleaseTestControl();

   // ---------------------------------------------------------------------------

   // Now, restore the control surfaces and actuators to their "original" positions
   mFlightControlsPtr->SetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);

   // Restore to non-testing conditions (lag, testing flags, etc)
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(testingFlag);
}

double P6DofVehicle::CalculateControlsCLArea(double aMach, double aAlpha_rad, double aStickBack)
{
   double CL = 0.0;
   if ((mPilotManagerPtr == nullptr) || (mFlightControlsPtr == nullptr))
   {
      return CL;
   }

   // Get the pilot
   P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return CL;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting();

   // Set no-lag testing
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(true);

   // Save the current control surface angles and actuator angles
   std::vector<double> controlSurfaceAngleList;
   std::vector<double> actuatorAngleList;
   mFlightControlsPtr->GetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);

   // Save the initial stick position
   double initialStickBack = pilot->GetStickBackControllerPosition();

   // We take "testing" control so that we can set the controls as desired
   pilot->TakeTestControl();

   // ---------------------------------------------------------------------------

   // Set the stick position
   pilot->SetTestStickBackControllerPosition(aStickBack);

   int64_t simTime_nanosec = 0;
   mFlightControlsPtr->Update(simTime_nanosec, mKinematicState);

   // ---------------------------------------------------------------------------

   // Loop through all aero parts and get their CL
   for (auto& tempAeroPart : mAeroPartList)
   {
      // Get the angle
      double surfaceAngle_deg = mFlightControlsPtr->GetControlSurfaceAngle_deg(tempAeroPart->GetControlSurfaceHandle());

      // Set the angle and calc Cm
      CL += tempAeroPart->CalculateMovableAeroCLArea(aMach, aAlpha_rad, surfaceAngle_deg);
   }

   // Now, restore the stick to its initial position and release test control
   pilot->SetTestStickBackControllerPosition(initialStickBack);
   pilot->ReleaseTestControl();

   // ---------------------------------------------------------------------------

   // Now, restore the control surfaces and actuators to their "original" positions
   mFlightControlsPtr->SetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);

   // Restore to non-testing conditions (lag, testing flags, etc)
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(testingFlag);

   return CL;
}

double P6DofVehicle::CalculateControlsCdArea(double aMach, double aAlpha_rad, double aStickBack)
{
   double Cd = 0.0;
   if ((mPilotManagerPtr == nullptr) || (mFlightControlsPtr == nullptr))
   {
      return Cd;
   }

   // Get the pilot
   P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return Cd;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = GetScenario()->GetFreezeFlags()->GetMasterNoLagTesting();

   // Set no-lag testing
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(true);

   // Save the current control surface angles and actuator angles
   std::vector<double> controlSurfaceAngleList;
   std::vector<double> actuatorAngleList;
   mFlightControlsPtr->GetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);

   // Save the initial stick position
   double initialStickBack = pilot->GetStickBackControllerPosition();

   // We take "testing" control so that we can set the controls as desired
   pilot->TakeTestControl();

   // ---------------------------------------------------------------------------

   // Set the stick position
   pilot->SetTestStickBackControllerPosition(aStickBack);

   int64_t simTime_nanosec = 0;
   mFlightControlsPtr->Update(simTime_nanosec, mKinematicState);

   // ---------------------------------------------------------------------------

   // Loop through all aero parts and get their Cd
   for (auto& tempAeroPart : mAeroPartList)
   {
      // Get the angle
      double surfaceAngle_deg = mFlightControlsPtr->GetControlSurfaceAngle_deg(tempAeroPart->GetControlSurfaceHandle());

      // Set the angle and calc Cm
      Cd += tempAeroPart->CalculateMovableAeroCdArea(aMach, aAlpha_rad, surfaceAngle_deg);
   }

   // Now, restore the stick to its initial position and release test control
   pilot->SetTestStickBackControllerPosition(initialStickBack);
   pilot->ReleaseTestControl();

   // ---------------------------------------------------------------------------

   // Now, restore the control surfaces and actuators to their "original" positions
   mFlightControlsPtr->SetAllControlSurfaceAndActuatorsAngles(controlSurfaceAngleList, actuatorAngleList);

   // Restore to non-testing conditions (lag, testing flags, etc)
   GetScenario()->GetFreezeFlags()->SetMasterNoLagTesting(testingFlag);

   return Cd;
}

void P6DofVehicle::CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg)
{
   aAlpha_deg = 0.0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const P6DofCommonController* controller = mPilotManagerPtr->GetSpecialCommonController();
   if (controller == nullptr)
   {
      return;
   }

   double mach        = mKinematicState.GetSpeed_Mach();
   double Q           = mKinematicState.GetDynamicPressure_psf();
   double wgt_lbs     = mMassProperties.GetMass_lbs();
   double loadWgt_lbs = wgt_lbs * aGLoad;
   double areaQ       = 0.0;

   if (GetActiveAeroObject()->UsesRefArea())
   {
      double refArea_sqft = GetActiveAeroObject()->GetRefArea_sqft();
      areaQ               = refArea_sqft * Q;
   }
   else
   {
      double wingArea_sqft = GetActiveAeroObject()->GetWingArea_sqft();
      areaQ                = wingArea_sqft * Q;
   }

   // Calculate the required CL
   double requiredCL = 0.0;
   if (areaQ > 0)
   {
      requiredCL = loadWgt_lbs / areaQ;
   }
   else
   {
      // Either Q or wingArea_sqft or both are zero
      if (aGLoad > 0.0)
      {
         requiredCL = 10.0;
      }
      else if (aGLoad < 0.0)
      {
         requiredCL = -10.0;
      }
   }

   controller->GetAlphaVsMachCL(mach, requiredCL, aAlpha_deg);
}

void P6DofVehicle::CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg)
{
   aBeta_deg = 0.0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const P6DofCommonController* controller = mPilotManagerPtr->GetSpecialCommonController();
   if (controller == nullptr)
   {
      return;
   }

   double mach        = mKinematicState.GetSpeed_Mach();
   double Q           = mKinematicState.GetDynamicPressure_psf();
   double wgt_lbs     = mMassProperties.GetMass_lbs();
   double loadWgt_lbs = wgt_lbs * aGLoad;
   double areaQ       = 0.0;

   if (GetActiveAeroObject()->UsesRefArea())
   {
      double refArea_sqft = GetActiveAeroObject()->GetRefArea_sqft();
      areaQ               = refArea_sqft * Q;
   }
   else
   {
      double wingArea_sqft = GetActiveAeroObject()->GetWingArea_sqft();
      areaQ                = wingArea_sqft * Q;
   }

   loadWgt_lbs *= -1.0; // Beta has opposite sign conventions than alpha

   // Calculate the required CL
   double requiredCL = 0.0;
   if (areaQ > 0)
   {
      requiredCL = loadWgt_lbs / areaQ;
   }
   else
   {
      // Either Q or area or both are zero
      if (aGLoad > 0.0)
      {
         requiredCL = 10.0;
      }
      else if (aGLoad < 0.0)
      {
         requiredCL = -10.0;
      }
      else
      {
         requiredCL = 0.0;
      }
   }

   controller->GetAlphaVsMachCL(mach, requiredCL, aBeta_deg);
}

double P6DofVehicle::CalculatePitchMomentAlphaStickBack_ftlbs(double aAlpha_deg, double aDesiredStickBack)
{
   if (mPilotManagerPtr == nullptr)
   {
      return 0.0;
   }

   // Get the pilot
   P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return 0.0;
   }

   if (!pilot->UsingTestControl())
   {
      // Error must use test control
      auto out =
         ut::log::error() << "TestControl not active in P6DofVehicle::CalculatePitchMomentAlphaStickBack_ftlbs().";
      out.AddNote() << "Vehicle: " << GetName();
      return 0.0;
   }

   // Set the stick position
   pilot->SetTestStickBackControllerPosition(aDesiredStickBack);

   if (mFlightControlsPtr != nullptr)
   {
      int64_t simTime_nanosec = 0;
      mFlightControlsPtr->Update(simTime_nanosec, mKinematicState);
   }

   // Perform aero calculations
   double   dynPress_lbsqft = mKinematicState.GetDynamicPressure_psf();
   double   mach            = mKinematicState.GetSpeed_Mach();
   double   speed_fps       = mKinematicState.GetSpeed_fps();
   double   alpha_rad       = aAlpha_deg * UtMath::cRAD_PER_DEG;
   double   beta_rad        = 0;
   double   alphaDot_rps    = 0.0;
   double   betaDot_rps     = 0.0;
   UtVec3dX angularRates_rps(0.0, 0.0, 0.0);
   double   liftFactor = 1;
   UtVec3dX baseMoment_ftlbs(0.0, 0.0, 0.0);
   double   baseLift_lbs      = 0;
   double   baseDrag_lbs      = 0;
   double   baseSideForce_lbs = 0;
   UtVec3dX controlsMoment_ftlbs(0.0, 0.0, 0.0);
   double   controlsLift_lbs      = 0;
   double   controlsDrag_lbs      = 0;
   double   controlsSideForce_lbs = 0;
   UtVec3dX totalMoment_ftlbs(0.0, 0.0, 0.0);

   // Get the main body aero
   mActiveAeroObjPtr->CalculateCoreAeroFM(dynPress_lbsqft,
                                          mach,
                                          speed_fps,
                                          alpha_rad,
                                          beta_rad,
                                          alphaDot_rps,
                                          betaDot_rps,
                                          angularRates_rps,
                                          baseMoment_ftlbs,
                                          baseLift_lbs,
                                          baseDrag_lbs,
                                          baseSideForce_lbs,
                                          liftFactor);

   // Get the "controls" aero
   CalculateMovableAero(dynPress_lbsqft,
                        mach,
                        speed_fps,
                        alpha_rad,
                        beta_rad,
                        angularRates_rps,
                        controlsMoment_ftlbs,
                        controlsLift_lbs,
                        controlsDrag_lbs,
                        controlsSideForce_lbs,
                        liftFactor);

   // Sum contributions
   totalMoment_ftlbs = baseMoment_ftlbs + controlsMoment_ftlbs;

   // Return the pitching moment
   return totalMoment_ftlbs.Y();
}

void P6DofVehicle::RestoreControlSurfaceAngles(std::vector<P6DofFlightControlSystem::ControlSurfaceElement*>& aControlsList,
                                               std::vector<double>& aAngleList)
{
   int index = 0;
   for (auto& iter : aControlsList)
   {
      iter->currentAngle_deg = aAngleList.at(index);
      ++index;
   }
}

void P6DofVehicle::EnableControls(bool aEnabled)
{
   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->EnableControls(aEnabled);
   }
}

bool P6DofVehicle::ControlsEnabled() const
{
   bool cntrlEnabled = false;
   if (mPilotManagerPtr != nullptr)
   {
      cntrlEnabled = mPilotManagerPtr->ControlsEnabled();
   }
   return cntrlEnabled;
}

void P6DofVehicle::CalculateStickBackForZeroMoment(double aAlpha_deg, double& aStickBack)
{
   aStickBack = 0;

   double mach = mKinematicState.GetSpeed_Mach();
   CalculateStickBackForZeroMoment(aAlpha_deg, mach, aStickBack);
}

void P6DofVehicle::CalculateStickBackForZeroMoment(double aAlpha_deg, double aMach, double& aStickBack)
{
   aStickBack = 0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return;
   }

   const P6DofCommonController* controller = pilot->GetCommonController();
   if (controller == nullptr)
   {
      return;
   }

   const P6DofPropulsionSystem* propulsion = GetPropulsionSystem();
   if (propulsion == nullptr)
   {
      return;
   }

   double totalThrust_lbs = 0;
   for (auto& thrustProducer : propulsion->GetThrustProducerList())
   {
      totalThrust_lbs += thrustProducer->GetThrust_lbs();
   }

   double alpha_rad = aAlpha_deg * UtMath::cRAD_PER_DEG;
   aStickBack       = 0;
   controller->GetStickForZeroMomentVsMachAlpha(aMach, alpha_rad, totalThrust_lbs, aStickBack);

   if (aStickBack < -1.0)
   {
      aStickBack = -1.0;
   }
   else if (aStickBack > 1.0)
   {
      aStickBack = 1.0;
   }
}

void P6DofVehicle::SetPrepositionAlt_m(double aAltitude_m)
{
   SetLocationLLA(mPrepositionLat, mPrepositionLon, aAltitude_m);
}

void P6DofVehicle::SetPrimaryTargetData(const P6DOF::CurrentTargetData& aCurrentTarget,
                                        int                             aNumTracks,
                                        const P6DOF::TrackData          aTrackData[10])
{
   mCurrentTarget = aCurrentTarget;
   mNumTracks     = aNumTracks;

   for (int i = 0; i < 10; ++i)
   {
      mTrackData[i] = aTrackData[i];
   }

   mCurrentTarget.autopilotYawGLoadCommand_g   = 0.0f;
   mCurrentTarget.autopilotPitchGLoadCommand_g = 0.0f;
   mCurrentTarget.autopilotRollRateCommand_dps = 0.0f;

   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot)
         {
            P6DofAutopilotAction* action = autopilot->GetCurrentActivity();
            if (action)
            {
               mCurrentTarget.autopilotYawGLoadCommand_g   = action->GetYawGLoad_g();
               mCurrentTarget.autopilotPitchGLoadCommand_g = action->GetPitchGLoad_g();
               mCurrentTarget.autopilotRollRateCommand_dps = action->GetRollRate_dps();
            }
         }
      }
   }
}

bool P6DofVehicle::HasFlaps() const
{
   bool hasFalps = false;
   for (const auto& part : mAeroPartList)
   {
      // Convert to uppercase
      std::string name = part->GetName();

      // Check if the name includes the word "Flap" in some form
      if ((name.find("flap") != std::string::npos) || (name.find("Flap") != std::string::npos) ||
          (name.find("FLAP") != std::string::npos))
      {
         hasFalps = true;
         break;
      }
   }
   return hasFalps;
}

bool P6DofVehicle::HasAfterburner() const
{
   bool hab = false;
   if (mPropulsionObjPtr != nullptr)
   {
      // Loop through engines, looking for at least one jet with an afterburner
      for (size_t i = 0; i < mPropulsionObjPtr->GetNumThrustProducers(); ++i)
      {
         // Get the current engine
         P6DofThrustProducerObject* engine = mPropulsionObjPtr->GetThrustProducerByIndex(i);

         // If engine is valid
         if ((engine != nullptr) && (engine->AfterburnerIsPresent()))
         {
            hab = true;
            break;
         }
      }
   }
   return hab;
}

void P6DofVehicle::SetLandingGearPositionAtStart(double aPosition)
{
   ForceLandingGearPositionInstantly("Landing_Gear_Extended", aPosition);
}

void P6DofVehicle::ForceLandingGearPositionInstantly(std::string aLandingGearControlSurfaceHandleName, double aPosition)
{
   // We need to do two things:
   //   1) Ensure the landing gear is down at start.
   //   2) Ensure that the "command" for gear is "down", so that it will not immediately retract.


   if ((mFlightControlsPtr == nullptr) || // If we lack flight controls or
       (mLandingGearPtr == nullptr) ||    // If we lack landing gear or
       (mPilotManagerPtr == nullptr))     // If we lack a pilot manager
   {
      // simply return
      return;
   }

   // Get the pilot
   P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return;
   } // If we lack a pilot controller, simply return

   // Get the gear handle
   size_t gearHandle = mFlightControlsPtr->GetControlSurfaceHandle(aLandingGearControlSurfaceHandleName);
   if (gearHandle == 0)
   {
      return;
   } // If we lack a gear handle, simply return

   // Set the current angle
   double gearAngle_deg = 90.0 * aPosition; // Gear down anglerolSurfaceAngle_deg(gearHandle);
   mFlightControlsPtr->SetControlSurfaceAngle_deg(gearHandle, gearAngle_deg);

   // The landing gear will now be down, but we need to be sure that there will not
   // be a command at the start to retract the gear.
   pilot->SetLandingGearControlPosition(aPosition);
}

void P6DofVehicle::ForceLandingGearUpInstantly(const std::string& aLandingGearControlSurfaceHandleName)
{
   ForceLandingGearPositionInstantly(aLandingGearControlSurfaceHandleName, 0.0);
}

void P6DofVehicle::ForceLandingGearDownInstantly(const std::string& aLandingGearControlSurfaceHandleName)
{
   ForceLandingGearPositionInstantly(aLandingGearControlSurfaceHandleName, 1.0);
}

void P6DofVehicle::CalculateCurrentMassProperties()
{
   // Begin by setting the "current" values to the "base" values
   mMassProperties.SetCurrentDataToBaseData();

   // Next, we'll add in mass properties for the propulsion system, if it exists
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->CalculateCurrentMassProperties();
      mMassProperties += mPropulsionObjPtr->GetMassProperties();
   }

   // Loop through the sub-objects
   for (auto& subobjectPtr : mSubObjectList)
   {
      if (subobjectPtr != nullptr)
      {
         subobjectPtr->CalculateCurrentMassProperties();

         // The mass properties from the subobject do not yet take into account the location
         // relative to the parent. Get the relative position and move the mass to that location.
         const P6DofMassProperties& subobjectMass = subobjectPtr->GetMassProperties();
         P6DofMassProperties        mass          = subobjectMass;
         UtVec3dX                   posRelToParent_m;
         UtVec3dX                   angRelToParentYPR_rad;
         subobjectPtr->GetPositionRelativeToParent(posRelToParent_m, angRelToParentYPR_rad);
         UtVec3dX posRelToParent_ft = posRelToParent_m * UtMath::cFT_PER_M;

         // Move to relative position
         mass.MoveToLocation(posRelToParent_ft);

         // Add-in the mass properties from the subobject
         mMassProperties += mass;
      }
   }
}

void P6DofVehicle::SetCurrentWeight_lbs(double aWeight_lbs)
{
   mMassProperties.SetBaseMassProperties(aWeight_lbs,
                                         mMassProperties.GetBaseIxx_slugft2(),
                                         mMassProperties.GetBaseIyy_slugft2(),
                                         mMassProperties.GetBaseIzz_slugft2(),
                                         mMassProperties.GetBaseCmPosRelToRef_ft());
}

void P6DofVehicle::ShiftCurrentCg_ft(UtVec3dX aDeltaCm_ft)
{
   UtVec3dX baseCm_ft    = mMassProperties.GetBaseCmPosRelToRef_ft();
   UtVec3dX updatedCm_ft = baseCm_ft + aDeltaCm_ft;

   mMassProperties.SetBaseMassProperties(mMassProperties.GetBaseMass_lbs(),
                                         mMassProperties.GetBaseIxx_slugft2(),
                                         mMassProperties.GetBaseIyy_slugft2(),
                                         mMassProperties.GetBaseIzz_slugft2(),
                                         updatedCm_ft);
}

bool P6DofVehicle::AutopilotAvailable() const
{
   bool ap_avail = false;
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      ap_avail                = ((pilot != nullptr) && (pilot->GetCommonController() != nullptr));
   }
   return ap_avail;
}

bool P6DofVehicle::UseSimpleYawDamper() const
{
   bool ok = false;
   if (mPilotManagerPtr != nullptr)
   {
      P6DofPilotObject* pilot = mPilotManagerPtr->GetActivePilot();
      if (pilot != nullptr)
      {
         P6DofCommonController* autopilot = pilot->GetCommonController();
         if (autopilot != nullptr)
         {
            ok = autopilot->UseSimpleYawDamper();
         }
         else if (pilot->GetPilotType() == "Manual-Simple") // Check for a simple manual pilot, which lacks an autopilot
         {
            P6DofManualPilotSimpleControls* manualPilot = dynamic_cast<P6DofManualPilotSimpleControls*>(pilot);
            ok                                          = manualPilot->UseSimpleYawDamper();
         }
      }
   }
   return ok;
}

double P6DofVehicle::GetAngleAileronLeft() const
{
   return GetAngleSurface(mHandleAileronLeft);
}

double P6DofVehicle::GetAngleAileronRight() const
{
   return GetAngleSurface(mHandleAileronRight);
}

double P6DofVehicle::GetAngleFlapLeft() const
{
   return GetAngleSurface(mHandleFlapLeft);
}

double P6DofVehicle::GetAngleFlapRight() const
{
   return GetAngleSurface(mHandleFlapRight);
}

double P6DofVehicle::GetAngleSpoilerLeft() const
{
   return GetAngleSurface(mHandleSpoilerLeft);
}

double P6DofVehicle::GetAngleSpoilerRight() const
{
   return GetAngleSurface(mHandleSpoilerRight);
}

double P6DofVehicle::GetAngleStabilizerLeft() const
{
   return GetAngleSurface(mHandleStabilizerLeft);
}

double P6DofVehicle::GetAngleStabilizerRight() const
{
   return GetAngleSurface(mHandleStabilizerRight);
}

double P6DofVehicle::GetAngleElevator() const
{
   return GetAngleSurface(mHandleElevator);
}

double P6DofVehicle::GetAngleRudderLeft() const
{
   return GetAngleSurface(mHandleRudderLeft);
}

double P6DofVehicle::GetAngleRudder() const
{
   return GetAngleSurface(mHandleRudder);
}

double P6DofVehicle::GetAngleRudderRight() const
{
   return GetAngleSurface(mHandleRudderRight);
}

double P6DofVehicle::GetAngleSpeedBrake() const
{
   return GetAngleSurface(mHandleSpeedBrake);
}

double P6DofVehicle::GetAngleLandingGear() const
{
   return GetAngleSurface(mHandleLandingGear);
}

double P6DofVehicle::GetAngleLandingGearNose() const
{
   return GetAngleSurface(mHandleLandingGearNose);
}

double P6DofVehicle::GetAngleLandingGearMainLeft() const
{
   return GetAngleSurface(mHandleLandingGearMainLeft);
}

double P6DofVehicle::GetAngleLandingGearMainRight() const
{
   return GetAngleSurface(mHandleLandingGearMainRight);
}

double P6DofVehicle::GetNormalizedAileronLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleAileronLeft);
}

double P6DofVehicle::GetNormalizedAileronRight() const
{
   return GetNormalizedSurfaceAngle(mHandleAileronRight);
}

double P6DofVehicle::GetNormalizedFlapLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleFlapLeft);
}

double P6DofVehicle::GetNormalizedFlapRight() const
{
   return GetNormalizedSurfaceAngle(mHandleFlapRight);
}

double P6DofVehicle::GetNormalizedSpoilerLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleSpoilerLeft);
}

double P6DofVehicle::GetNormalizedSpoilerRight() const
{
   return GetNormalizedSurfaceAngle(mHandleSpoilerRight);
}

double P6DofVehicle::GetNormalizedStabilizerLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleStabilizerLeft);
}

double P6DofVehicle::GetNormalizedStabilizerRight() const
{
   return GetNormalizedSurfaceAngle(mHandleStabilizerRight);
}

double P6DofVehicle::GetNormalizedElevator() const
{
   return GetNormalizedSurfaceAngle(mHandleElevator);
}

double P6DofVehicle::GetNormalizedRudderLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleRudderLeft);
}

double P6DofVehicle::GetNormalizedRudder() const
{
   return GetNormalizedSurfaceAngle(mHandleRudder);
}

double P6DofVehicle::GetNormalizedRudderRight() const
{
   return GetNormalizedSurfaceAngle(mHandleRudderRight);
}

double P6DofVehicle::GetNormalizedSpeedBrake() const
{
   return GetNormalizedSurfaceAngle(mHandleSpeedBrake);
}

double P6DofVehicle::GetNormalizedLandingGear() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGear);
}

double P6DofVehicle::GetNormalizedLandingGearNose() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearNose);
}

double P6DofVehicle::GetNormalizedLandingGearMainLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearMainLeft);
}

double P6DofVehicle::GetNormalizedLandingGearMainRight() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearMainRight);
}

bool P6DofVehicle::AileronLeftValid() const
{
   if (mHandleAileronLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::AileronRightValid() const
{
   if (mHandleAileronRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::FlapLeftValid() const
{
   if (mHandleFlapLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::FlapRightValid() const
{
   if (mHandleFlapRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::SpoilerLeftValid() const
{
   if (mHandleSpoilerLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::SpoilerRightValid() const
{
   if (mHandleSpoilerRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::StabilizerLeftValid() const
{
   if (mHandleStabilizerLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::StabilizerRightValid() const
{
   if (mHandleStabilizerRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::ElevatorValid() const
{
   if (mHandleElevator > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::RudderLeftValid() const
{
   if (mHandleRudderLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::RudderValid() const
{
   if (mHandleRudder > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::RudderRightValid() const
{
   if (mHandleRudderRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::SpeedBrakeValid() const
{
   if (mHandleSpeedBrake > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::LandingGearValid() const
{
   if (mHandleLandingGear > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::LandingGearNoseValid() const
{
   if (mHandleLandingGearNose > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::LandingGearMainLeftValid() const
{
   if (mHandleLandingGearMainLeft > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::LandingGearMainRightValid() const
{
   if (mHandleLandingGearMainRight > 0)
   {
      return true;
   }

   return false;
}

bool P6DofVehicle::ThrottleValid() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdThrottleMilHandle();
         if (handle > 0)
         {
            return true;
         }
      }
   }

   return false;
}

double P6DofVehicle::GetStickRightInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdStickRightHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetStickBackInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdStickBackHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetRudderRightInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdRudderRightHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetThrottleInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         double value  = 0.0;
         size_t handle = pilot->GetStdThrottleMilHandle();
         if (handle > 0)
         {
            value = pilot->ControlInputValue(handle);

            handle = pilot->GetStdThrottleABHandle();
            if (handle > 0)
            {
               double ab = pilot->ControlInputValue(handle);

               // If AB is valid, MIL should be 1.0
               if (value > 0.999) // Assume 0.999 is essentially 1.0
               {
                  // Ensure value is 1 plus ab
                  value = 1.0 + ab;
               }
               return value;
            }
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetSpeedBrakeInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdSpeedBrakesOutHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetSpoilerInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdSpoilersOutHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

double P6DofVehicle::GetLandingGearInput() const
{
   P6DofPilotManager* pilotMgr = mPilotManagerPtr;
   if (pilotMgr != nullptr)
   {
      P6DofPilotObject* pilot = pilotMgr->GetActivePilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdLandingGearDownHandle();
         if (handle > 0)
         {
            return pilot->ControlInputValue(handle);
         }
      }
   }
   return 0.0;
}

void P6DofVehicle::SetDestroyed(double aPitchInputModifier,
                                double aRollInputModifier,
                                bool   aIsOnFire,
                                bool   aEngineIsSmoking,
                                bool   aIsTrailingSmoke)
{
   mIsDestroyed = true;

   // Set various fire and smoke parameters, but only if true. We don't want to turn off smoke that already exists.
   if (aIsOnFire)
   {
      ActivateFlamesEffect(aIsOnFire);
   }
   if (aEngineIsSmoking)
   {
      MakeEnginesSmoke();
   }
   if (aIsTrailingSmoke)
   {
      ActivateDamageSmokeTrailingEffect(aIsTrailingSmoke);
   }

   // Tell the pilot manager that we're destroyed
   P6DofPilotManager* pilotMgr = GetPilotManager();
   if (pilotMgr != nullptr)
   {
      pilotMgr->SetDestroyed(aPitchInputModifier, aRollInputModifier);
   }
}

bool P6DofVehicle::GetIsDestroyed()
{
   return mIsDestroyed;
}

bool P6DofVehicle::AnEngineIsOperating() const
{
   bool operating = false;
   if (mPropulsionObjPtr != nullptr)
   {
      operating = mPropulsionObjPtr->AnEngineIsOperating();
   }
   return operating;
}

bool P6DofVehicle::AnEngineIsSmoking() const
{
   bool engSmoking = false;
   if (mPropulsionObjPtr != nullptr)
   {
      engSmoking = mPropulsionObjPtr->AnEngineIsSmoking();
   }
   return engSmoking;
}

bool P6DofVehicle::AnEngineHasAfterburnerOn() const
{
   bool eabOn = false;
   if (mPropulsionObjPtr != nullptr)
   {
      eabOn = mPropulsionObjPtr->AnEngineHasAfterburnerOn();
   }
   return eabOn;
}

void P6DofVehicle::MakeEnginesSmoke()
{
   if (mPropulsionObjPtr != nullptr)
   {
      return mPropulsionObjPtr->MakeAnEngineSmoke(0); // Zero, all engines
   }
}

bool P6DofVehicle::ContrailTrailingEffectPresent() const
{
   bool present = false;
   if (mPropulsionObjPtr != nullptr)
   {
      present = mPropulsionObjPtr->AnEngineIsContrailing();
   }
   return present;
}

bool P6DofVehicle::RocketSmokeTrailingEffectPresent() const
{
   bool present = false;
   if (mPropulsionObjPtr != nullptr)
   {
      present = mPropulsionObjPtr->AnEngineIsEmittingSmokeTrail();
   }
   return present;
}

bool P6DofVehicle::DamageSmokeTrailingEffectPresent() const
{
   return mDamageSmokeTrailingEffect;
}

void P6DofVehicle::ActivateDamageSmokeTrailingEffect(bool aEffectActive)
{
   mDamageSmokeTrailingEffect = aEffectActive;
}

bool P6DofVehicle::FlamesArePresent() const
{
   return mFlamesPresent;
}

void P6DofVehicle::ActivateFlamesEffect(bool aFlamesPresent)
{
   mFlamesPresent = aFlamesPresent;
}

bool P6DofVehicle::WithinContrailAltitudeBand(double aAltitude_ft) const
{
   bool                   wcab = false;
   const P6DofAtmosphere* atm  = mKinematicState.GetAtmospherePtr();
   if (atm != nullptr)
   {
      wcab = atm->WithinContrailAltitudeBand(aAltitude_ft);
   }
   return wcab;
}

double P6DofVehicle::GetAngleSurface(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlSurfaceAngle_deg(aHandle);
   }

   return 0.0;
}

double P6DofVehicle::GetNormalizedSurfaceAngle(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlSurfaceValueNormalized(aHandle);
   }

   return 0.0;
}

void P6DofVehicle::FindParts()
{
   if (mFlightControlsPtr != nullptr)
   {
      size_t handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Aileron", "Left");
      if (handle != 0)
      {
         mHandleAileronLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Aileron", "Lft");
         if (handle != 0)
         {
            mHandleAileronLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Aileron", "Right");
      if (handle != 0)
      {
         mHandleAileronRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Aileron", "Rgt");
         if (handle != 0)
         {
            mHandleAileronRight = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Flap", "Left");
      if (handle != 0)
      {
         mHandleFlapLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Flap", "Lft");
         if (handle != 0)
         {
            mHandleFlapLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Flap", "Right");
      if (handle != 0)
      {
         mHandleFlapRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Flap", "Rgt");
         if (handle != 0)
         {
            mHandleFlapRight = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Spoiler", "Left");
      if (handle != 0)
      {
         mHandleSpoilerLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Spoiler", "Lft");
         if (handle != 0)
         {
            mHandleSpoilerLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Spoiler", "Right");
      if (handle != 0)
      {
         mHandleSpoilerRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Spoiler", "Rgt");
         if (handle != 0)
         {
            mHandleSpoilerRight = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Stabilizer", "Left");
      if (handle != 0)
      {
         mHandleStabilizerLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Stabilizer", "Lft");
         if (handle != 0)
         {
            mHandleStabilizerLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Stabilizer", "Right");
      if (handle != 0)
      {
         mHandleStabilizerRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Stabilizer", "Rgt");
         if (handle != 0)
         {
            mHandleStabilizerRight = handle;
         }
      }

      // If we lack left and right stabs, we may have a single elevator
      if ((mHandleStabilizerLeft == 0) && (mHandleStabilizerRight == 0))
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("Elevator");
         if (handle != 0)
         {
            mHandleElevator = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Rudder", "Left");
      if (handle != 0)
      {
         mHandleRudderLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Rudder", "Lft");
         if (handle != 0)
         {
            mHandleRudderLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Rudder", "Right");
      if (handle != 0)
      {
         mHandleRudderRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Rudder", "Rgt");
         if (handle != 0)
         {
            mHandleRudderRight = handle;
         }
      }

      // If we lack left and right rudders, we may have a single rudder
      if ((mHandleRudderLeft == 0) && (mHandleRudderRight == 0))
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("Rudder");
         if (handle != 0)
         {
            mHandleRudder = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("SpeedBrake");
      if (handle != 0)
      {
         mHandleSpeedBrake = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("Speedbrake");
         if (handle != 0)
         {
            mHandleSpeedBrake = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("Gear");
      if (handle != 0)
      {
         mHandleLandingGear = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingString("gear");
         if (handle != 0)
         {
            mHandleLandingGear = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Gear", "Nose");
      if (handle != 0)
      {
         mHandleLandingGearNose = handle;
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Gear", "Left");
      if (handle != 0)
      {
         mHandleLandingGearMainLeft = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Gear", "Lft");
         if (handle != 0)
         {
            mHandleLandingGearMainLeft = handle;
         }
      }

      handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Gear", "Right");
      if (handle != 0)
      {
         mHandleLandingGearMainRight = handle;
      }
      else
      {
         handle = mFlightControlsPtr->GetControlSurfaceHandleContainingTwoStrings("Gear", "Rgt");
         if (handle != 0)
         {
            mHandleLandingGearMainRight = handle;
         }
      }
   }
}

void P6DofVehicle::CalcStallAngle()
{
   if (mAeroCoreObjPtr != nullptr)
   {
      // Use a slow speed, but non-zero
      double mach = 0.01;

      // First find transition point
      double step           = 0.2 * UtMath::cRAD_PER_DEG; // Step using 0.2 degrees
      double transition_ang = -100.0;                     // Use a hugely negative angle (radians)
      double lastCL         = -100.0;                     // Use a hugely negative CL
      for (double ang = 0.0; ang <= 1.57; ang += step)
      {
         double CL = mAeroCoreObjPtr->CalculateAeroCLArea(mach, ang);

         if (CL < lastCL)
         {
            // Transition point reached
            transition_ang = ang;
            break;
         }

         lastCL = CL;
      }

      if (transition_ang < 0.0)
      {
         // Stall point not discovered -- use 1.57 radians, approx 90 degrees
         mStallAngle = 1.57;

         // Detection angle is assumed to be 1 degree plus 10% of the stall angle
         mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

         return;
      }
      else
      {
         // Now, we check values with a much smaller step size
         // We use 3 degrees on either side
         double delta = 3.0 * UtMath::cRAD_PER_DEG;
         double start = transition_ang - delta;
         double end   = transition_ang + delta;

         step   = 0.2 * UtMath::cRAD_PER_DEG; // Step using only 0.01 degrees
         lastCL = -100.0;                     // Use a hugely negative CL

         for (double ang = start; ang <= end; ang += step)
         {
            double CL = mAeroCoreObjPtr->CalculateAeroCLArea(mach, ang);

            if (CL < lastCL)
            {
               // Transition point reached -- The stall angle is the preceding angle
               mStallAngle = ang - step;

               // Detection angle is assumed to be 1 degree plus 10% of the stall angle
               mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

               return;
            }

            lastCL = CL;
         }

         // We were unable to calculate the precise stall angle (which should not happen)

         // Simply use the transition_ang
         mStallAngle = transition_ang;

         // Detection angle is assumed to be 1 degree plus 10% of the stall angle
         mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;

         return;
      }
   }

   // No aero -- use 1.57 radians, approx 90 degrees
   mStallAngle = 1.57;

   // Detection angle is assumed to be 1 degree plus 10% of the stall angle
   mStallDetectionDeltaAngle = 1.0 * UtMath::cRAD_PER_DEG + 0.1 * mStallAngle;
}

int P6DofVehicle::GetNumberOfExternalTanks() const
{
   int num = 0;
   for (auto& subObject : mSubObjectList)
   {
      // First, determine if it has an internal tank capacity
      if (subObject->GetInternalFuelTankCapacity_lbs() > 0.0)
      {
         // Next, verify that it does *not* have an engine (thrust producer)
         P6DofPropulsionSystem* system = subObject->GetPropulsionSystem();
         if (system != nullptr)
         {
            if (system->GetNumThrustProducers() == 0)
            {
               // This lacks an engine, so it is "just" a fuel tank - increment count
               ++num;
            }
         }
      }
   }

   return num;
}
