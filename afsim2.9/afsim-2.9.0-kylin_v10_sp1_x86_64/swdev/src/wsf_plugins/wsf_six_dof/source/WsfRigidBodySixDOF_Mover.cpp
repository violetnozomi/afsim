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

#include "WsfRigidBodySixDOF_Mover.hpp"

#include <iomanip>
#include <string>

#include "UtDCM.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLLAPos.hpp"
#include "UtLLPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMatrix.hpp"
#include "UtMemory.hpp"
#include "UtRandom.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfCallback.hpp"
#include "WsfDateTime.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMoverGuidance.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfPlatformPartObserver.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_ForceAndMomentsObject.hpp"
#include "WsfSixDOF_ForceAndRotationObject.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "WsfTrackManager.hpp"
#include "WsfVisualPart.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"
#include "maneuvers/WsfSixDOF_Maneuver.hpp"
#include "maneuvers/WsfSixDOF_ManeuverSequence.hpp"

const int64_t gRigidBodyNanosecStepSize = 10000000; // 10 msec (100 Hz)

wsf::six_dof::RigidBodyMover::RigidBodyMover(const WsfScenario& aScenario)
   : Mover(aScenario)
   , mTestSupportObjectPtr(ut::make_unique<RigidBodyMoverTestObject>(this))
   , mPropulsionObjPtr(ut::make_unique<RigidBodyPropulsionSystem>())
{
   auto typeManager    = static_cast<TypeManager*>(aScenario.FindExtension("wsf_six_dof"));
   mIntegratorTypeName = typeManager->GetDefaultRigidBodySixDOF_IntegratorName();
   mStepSize_nanosec   = gRigidBodyNanosecStepSize;

   // Set the default message rates to off.
   mEventPipeManager["MsgSixDOF_ControlSurfaces"] = std::pair<double, int>(0.0, 0);
}

// =================================================================
wsf::six_dof::RigidBodyMover::RigidBodyMover(const RigidBodyMover& aSrc)
   : Mover(aSrc)
   , mTestSupportObjectPtr(ut::make_unique<RigidBodyMoverTestObject>(this))
   , mAeroCoreObjPtr(aSrc.mAeroCoreObjPtr)
   , mAeroPartList(aSrc.mAeroPartList)
   , mPropulsionObjPtr(aSrc.mPropulsionObjPtr)
   , mIntegratorPtr(aSrc.mIntegratorPtr)
   , mFlightControlsPtr(aSrc.mFlightControlsPtr)
   , mPilotManagerPtr(aSrc.mPilotManagerPtr)
   , mLandingGearPtr(aSrc.mLandingGearPtr)
   , mLandingGearDownAtStart(aSrc.mLandingGearDownAtStart)
   , mParkingBrakeOnAtStart(aSrc.mParkingBrakeOnAtStart)
   , mTaxiModeEnabledAtStart(aSrc.mTaxiModeEnabledAtStart)
   , mNWSEnabledAtStart(aSrc.mNWSEnabledAtStart)
   , mLastLandingGearLeverPosition(aSrc.mLastLandingGearLeverPosition)
   , mLastThrustReverserLeverPosition(aSrc.mLastThrustReverserLeverPosition)
   , mLastThrustVectorYawRightPosition(aSrc.mLastThrustVectorYawRightPosition)
   , mLastThrustVectorPitchUpPosition(aSrc.mLastThrustVectorPitchUpPosition)
   , mLastThrustVectorRollRightPosition(aSrc.mLastThrustVectorRollRightPosition)
   , mLastNoseWheelSteeringRightPosition(aSrc.mLastNoseWheelSteeringRightPosition)
   , mLastWheelBrakeLeftPosition(aSrc.mLastWheelBrakeLeftPosition)
   , mLastWheelBrakeRightPosition(aSrc.mLastWheelBrakeRightPosition)
   , mLastNWS_Enabled(aSrc.mLastNWS_Enabled)
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
{
   SetActiveAeroObject(mAeroCoreObjPtr);

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

   if (aSrc.mIntegratorPtr)
   {
      mIntegratorPtr->SetParentVehicle(this);
   }
}

// =================================================================
WsfMover* wsf::six_dof::RigidBodyMover::Clone() const
{
   return new RigidBodyMover(*this);
}

// =================================================================
bool wsf::six_dof::RigidBodyMover::ProcessInput(UtInput& aInput)
{
   bool               myCommand = true;
   const std::string& command   = aInput.GetCommand();

   std::unique_ptr<WsfRoute> routePtr(nullptr);
   if (Mover::ProcessInput(aInput))
   {
   }
   else if (WsfRouteTypes::Get(WsfScenario::FromInput(aInput)).LoadInstance(aInput, routePtr) && routePtr != nullptr)
   {
      PilotObject* pilot = GetActivePilotObject();
      if (pilot == nullptr)
      {
         throw UtInput::BadValue(aInput, "RigidBodyMover was not defined with a RigidBodyPilotObject, can not use route!");
      }

      mWsfRoutePtr = routePtr.release();

      pilot = GetActivePilotObject();
      if (pilot != nullptr)
      {
         pilot->SetPlannedRoute(ConvertWsfRouteToSixDOF_Route(mWsfRoutePtr));
      }
   }
   else if (command == "rigid_body_vehicle_type")
   {
      std::string typeName;
      std::string baseType;
      aInput.ReadValue(typeName);
      aInput.ReadValue(baseType);
      SetName(typeName);

      auto typeManager = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));

      bool derivedFromBase = true;
      if (baseType != "BASE_TYPE")
      {
         derivedFromBase = false;
         // This is not derived from the base type
         // Determine if this base type exists
         if (!typeManager->ObjectTypeExists(baseType))
         {
            // Unknown type -- error since we can't derive from an unknown type
            auto out = ut::log::error() << "WsfRigidBodySixDOF_MoverType is trying to derive from an unknown type in "
                                           "WsfRigidBodySixDOF_MoverType::ProcessInput.";
            out.AddNote() << "Base Type: " << baseType;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // We have a known type from which to derive
      // Determine if the name is unique
      if (!typeManager->ObjectTypeNameIsUnique(typeName))
      {
         // Non-unique type name -- error since we can't have duplicate names
         auto out =
            ut::log::error()
            << "WsfRigidBodySixDOF_MoverType is using a duplicate name in WsfRigidBodySixDOF_MoverType::ProcessInput.";
         out.AddNote() << "Name: " << typeName;
         out.AddNote() << "Location: " << aInput.GetLocation();
         throw UtInput::UnknownCommand(aInput);
      }

      // Set data
      mDerivedFromName = baseType;

      // If derived, clone the data
      if (!derivedFromBase)
      {
         const Mover* derivedFromObject           = typeManager->GetObjectTypeByName(baseType);
         auto         derivedFromRigidBodySixDOF_ = dynamic_cast<const RigidBodyMover*>(derivedFromObject);
         if (derivedFromRigidBodySixDOF_ != nullptr)
         {
            DeriveFrom(*derivedFromRigidBodySixDOF_);
         }
         else
         {
            // Source vehicle is of a different type, which we don't support for now
            auto out = ut::log::error() << "RigidBodyMover is attempting to derive from a different type in "
                                           "PointMassSixDOF_Vehicle::ProcessInput.";
            out.AddNote() << "Name: " << typeName;
            out.AddNote() << "Deriving from: " << baseType;
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Previously, we registered the object with the manager here, but now handle this
      // outside of WsfRigidBodySixDOF_MoverType::ProcessInput
      // mObjectTypeManagerPtr->RegisterObjectType(this);

      UtInputBlock block(aInput, "end_rigid_body_vehicle_type");
      while (block.ReadCommand())
      {
         if (Mover::ProcessVehicleInput(aInput))
         {
         }
         else if (command == "aero_data")
         {
            if (mAeroCoreObjPtr == nullptr)
            {
               mAeroCoreObjPtr = ut::make_unique<RigidBodyAeroCoreObject>();
            }
            mAeroCoreObjPtr->ProcessInput(aInput);
         }
         else if (command == "aero_component")
         {
            // Add this object to the list
            auto tempObj = ut::make_unique<RigidBodyAeroMovableObject>();
            tempObj->ProcessInput(aInput);
            mAeroPartList.push_back(std::move(tempObj));
         }
         else if (command == "flight_controls")
         {
            if (mFlightControlsPtr == nullptr)
            {
               mFlightControlsPtr = ut::make_unique<RigidBodyFlightControlSystem>();
            }
            mFlightControlsPtr->ProcessInput(aInput);
         }
         else if (command == "pilot_manager")
         {
            if (mPilotManagerPtr == nullptr)
            {
               mPilotManagerPtr = ut::make_unique<RigidBodyPilotManager>();
            }
            mPilotManagerPtr->ProcessInput(aInput);
         }
         else if (command == "landing_gear")
         {
            if (mLandingGearPtr == nullptr)
            {
               mLandingGearPtr = ut::make_unique<RigidBodyLandingGear>();
            }
            mLandingGearPtr->ProcessInput(aInput);
         }
         else if (command == "propulsion_data")
         {
            mPropulsionObjPtr->ProcessInput(aInput, typeManager);
         }
      }
   }
   else if (command == "vehicle_type")
   {
      std::string typeName;
      aInput.ReadValue(typeName);

      auto         typeManager = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));
      const Mover* tempObjType = typeManager->GetObjectTypeByName(typeName);

      auto tempRigidBodySixDOF_Type = dynamic_cast<const RigidBodyMover*>(tempObjType);
      if (tempRigidBodySixDOF_Type == nullptr)
      {
         throw UtInput::BadValue(aInput, "Could not find requested rigid-body vehicle type!");
      }
      DeriveFrom(*tempRigidBodySixDOF_Type);
   }
   else if (command == "landing_gear_down")
   {
      bool gearDown = false;
      aInput.ReadValue(gearDown);
      mLandingGearDownAtStart = gearDown;
   }
   else if (command == "parking_brake_on")
   {
      bool brakeSet = false;
      aInput.ReadValue(brakeSet);
      mParkingBrakeOnAtStart = brakeSet;
   }
   else if (command == "taxi_mode_enabled")
   {
      bool taxiMode = false;
      aInput.ReadValue(taxiMode);
      mTaxiModeEnabledAtStart = taxiMode;
   }
   else if (command == "nws_enabled")
   {
      bool nwsEnabled = false;
      aInput.ReadValue(nwsEnabled);
      mNWSEnabledAtStart = nwsEnabled;
   }
   else if (command == "event_control_surfaces_data")
   {
      double dataRate;
      CheckForFrequencyUnits(dataRate, aInput);
      mEventPipeManager["MsgSixDOF_ControlSurfaces"] = std::pair<double, int>(dataRate, 0);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void wsf::six_dof::RigidBodyMover::DeriveFrom(const RigidBodyMover& aSrc)
{
   Mover::DeriveFrom(aSrc);

   if (aSrc.mAeroCoreObjPtr)
   {
      mAeroCoreObjPtr = aSrc.mAeroCoreObjPtr;
      SetActiveAeroObject(mAeroCoreObjPtr);
   }

   mAeroPartList.clear();
   for (auto& part : aSrc.mAeroPartList)
   {
      mAeroPartList.push_back(part);
   }

   if (aSrc.mPropulsionObjPtr)
   {
      mPropulsionObjPtr = aSrc.mPropulsionObjPtr;
      mPropulsionObjPtr->SetParentVehicle(this);
   }

   if (aSrc.mPilotManagerPtr)
   {
      mPilotManagerPtr = aSrc.mPilotManagerPtr;
      mPilotManagerPtr->SetParentVehicle(this);
   }

   if (aSrc.mFlightControlsPtr)
   {
      mFlightControlsPtr = aSrc.mFlightControlsPtr;
      mFlightControlsPtr->SetParentVehicle(this);
   }

   if (aSrc.mLandingGearPtr)
   {
      mLandingGearPtr = aSrc.mLandingGearPtr;
      mLandingGearPtr->SetParentVehicle(this);
   }
}

// =================================================================
bool wsf::six_dof::RigidBodyMover::Initialize(double aSimTime)
{
   if (!Mover::Initialize(aSimTime))
   {
      return false;
   }

   if (mLandingGearDownAtStart)
   {
      SetLandingGearPositionAtStart(1.0);
   }
   else
   {
      SetLandingGearPositionAtStart(0.0);
   }

   if (mParkingBrakeOnAtStart)
   {
      EnableDirectBraking();
      SetParkingBrake();
   }

   if (mTaxiModeEnabledAtStart)
   {
      SetTaxiMode(true);
   }

   if (mNWSEnabledAtStart)
   {
      SetEnableNWS(true);
   }

   return true;
}

bool wsf::six_dof::RigidBodyMover::InitializeDetails(double aSimTime_sec)
{
   int64_t simTime_nanosec = utils::TimeToTime(aSimTime_sec);

   if (mPilotManagerPtr)
   {
      mPilotManagerPtr->SetParentVehicle(this);
      mPilotManagerPtr->Initialize(simTime_nanosec);
   }

   if (mFlightControlsPtr)
   {
      mFlightControlsPtr->SetParentVehicle(this);
      mFlightControlsPtr->SetPilotManager(mPilotManagerPtr);
      if (mPilotManagerPtr != nullptr)
      {
         mFlightControlsPtr->SetPilotObject(mPilotManagerPtr->GetActiveRigidBodyPilot());
      }
      mFlightControlsPtr->Initialize(simTime_nanosec);
   }

   if (mLandingGearPtr)
   {
      mLandingGearPtr->SetParentVehicle(this);
      mLandingGearPtr->Initialize(simTime_nanosec);
   }

   // Set aero reference areas for movable aero objects
   if (mAeroCoreObjPtr)
   {
      mAeroCoreObjPtr->Initialize();
      SetActiveAeroObject(mAeroCoreObjPtr);

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
                          << "Unable to get flight controls handle for control surface in RigidBodyMover.";
               out.AddNote() << "Surface: " << part->GetName();
               out.AddNote() << "Vehicle: " << GetName();
            }
            part->SetControlSurfaceHandle(handle);
         }
      }
   }

   // We need to loop through all engines and get the controls handle
   if (mPropulsionObjPtr)
   {
      mPropulsionObjPtr->SetParentVehicle(this);
      mPropulsionObjPtr->Initialize(simTime_nanosec);
      mPropulsionObjPtr->SetControlInputHandles(mFlightControlsPtr);
   }

   // Set the integrator
   auto                 typeManager         = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));
   Integrator*          integrator          = typeManager->GetIntegratorByName(mIntegratorTypeName);
   RigidBodyIntegrator* rigidBodyIntegrator = dynamic_cast<RigidBodyIntegrator*>(integrator);

   if (rigidBodyIntegrator != nullptr)
   {
      // Take a smaller step size if one is requested
      double defaultStepSize_sec     = utils::TimeToTime(gRigidBodyNanosecStepSize);
      double moverUpdateInterval_sec = GetUpdateInterval();

      if (moverUpdateInterval_sec > utils::cEPSILON_SIMTIME_SEC && moverUpdateInterval_sec < defaultStepSize_sec)
      {
         mStepSize_nanosec = utils::TimeToTime(GetUpdateInterval());
      }

      mIntegratorPtr = rigidBodyIntegrator->Clone();
      mIntegratorPtr->SetParentVehicle(this);
   }
   else
   {
      std::string exceptionStr = "No integrator available for object = " + GetName();
      throw(UtException(exceptionStr));
   }

   FindParts();

   return true;
}

void wsf::six_dof::RigidBodyMover::UpdateFlightControls(int64_t aSimTime_nanosec)
{
   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->Update(aSimTime_nanosec);
   }
}

void wsf::six_dof::RigidBodyMover::CalculateMovableAero(double          aDynPress_lbsqft,
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

void wsf::six_dof::RigidBodyMover::CalculateAeroBodyForceAndMoments(UtVec3dX& aLiftBodyVector_lbs,
                                                                    UtVec3dX& aDragBodyVector_lbs,
                                                                    UtVec3dX& aSideForceBodyVector_lbs,
                                                                    UtVec3dX& aMoment_ftlbs,
                                                                    UtVec3dX& aReferencePt_ft)
{
   if (mEnvironment == nullptr)
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
                                      mKinematicState.GetVelocityWCS(),
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
      ForceAndMomentsObject thisObjectFM;
      UtVec3dX              zeroVector(0.0, 0.0, 0.0);
      UtVec3dX              aeroTotalBodyForce_lbs = liftBodyForce_lbs + dragBodyForce_lbs + sideBodyForce_lbs;

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

         subObjIter->CalculateAeroBodyForceAndMoments(subObjectLiftBodyVector_lbs,
                                                      subObjectDragBodyVector_lbs,
                                                      subObjectSideBodyVector_lbs,
                                                      subObjectMoment_ftlbs,
                                                      subObjectReferencePt_ft);

         UtVec3dX subObjectAeroTotalBodyForce_lbs =
            subObjectLiftBodyVector_lbs + subObjectDragBodyVector_lbs + subObjectSideBodyVector_lbs;
         ForceAndMomentsObject subObjectFM;
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
      aReferencePt_ft.Set(0.0);
   }
}

void wsf::six_dof::RigidBodyMover::CalculateAeroBodyForceAndRotation(UtVec3dX& aLiftBodyVector_lbs,
                                                                     UtVec3dX& aDragBodyVector_lbs,
                                                                     UtVec3dX& aSideForceBodyVector_lbs,
                                                                     UtVec3dX& aRotationAccelerationLimits_rps2,
                                                                     UtVec3dX& aStabilizingFrequency_rps)
{
   aRotationAccelerationLimits_rps2 = UtVec3dX();
   aStabilizingFrequency_rps        = UtVec3dX();

   UtVec3dX moment_ftlbs;
   UtVec3dX referencePt_ft;
   CalculateAeroBodyForceAndRotation(aLiftBodyVector_lbs,
                                     aDragBodyVector_lbs,
                                     aSideForceBodyVector_lbs,
                                     moment_ftlbs,
                                     referencePt_ft);
}

void wsf::six_dof::RigidBodyMover::CalculatePropulsionFM(int64_t               aSimTime_nanosec,
                                                         double                aDeltaT_sec,
                                                         const KinematicState& aState,
                                                         UtVec3dX&             aInertialPropulsionVector_lbs,
                                                         UtVec3dX&             aPropulsionMoment_ftlbs)
{
   if (mPropulsionObjPtr == nullptr)
   {
      aInertialPropulsionVector_lbs.Set(0.0);
      aPropulsionMoment_ftlbs.Set(0.0);
      return;
   }

   // Calc the thrust vector by doing an 'update' on the propulsion object and a 'calculate' on propulsion objects of any children
   ForceAndMomentsObject tempThrustFM;
   double                simTime_sec = utils::TimeToTime(aSimTime_nanosec);
   CalculateThrustVectorAndMomentUpdatePropulsion(simTime_sec, aDeltaT_sec, aState, tempThrustFM);

   UtVec3dX thrustVector_lbs;
   UtVec3dX thrustMoment_ftlbs;
   tempThrustFM.GetForceAndMomentAtCurrentRefPoint(thrustVector_lbs, thrustMoment_ftlbs);

   // The F&M above are body-rel - convert to inertial
   aInertialPropulsionVector_lbs = mKinematicState.GetDCM().InverseTransform(thrustVector_lbs);
   aPropulsionMoment_ftlbs       = thrustMoment_ftlbs;
}

void wsf::six_dof::RigidBodyMover::CalculateLandingGearFM(int64_t         aSimTime_nanosec,
                                                          const UtVec3dX& aNonLandingGearTotalForceVecInertial_lbs,
                                                          UtVec3dX&       aInertialTotalGearVector_lbs,
                                                          UtVec3dX&       aTotalGearMoment_ftlbs)
{
   if (mLandingGearPtr == nullptr)
   {
      aInertialTotalGearVector_lbs.Set(0.0);
      aTotalGearMoment_ftlbs.Set(0.0);
      return;
   }

   // To start, we want to determine the surface normal for our current location (This is for the reference point of the
   // vehicle. Each landing gear will have its own surface normal which may differ slightly). We also get the speed of
   // the surface as well (such as for an aircraft carrier).

   UtVec3dX surfaceUpNEDVec(0., 0., -1.);
   UtVec3dX surfaceSpeedNEDVec_mps;
   double   surfaceFrictionMult = 0.0;
   // Get the surface normal and speed of the ground at the vehicle location
   mEnvironment->GetTerrainDataElevationAndData(mKinematicState.GetLat(),
                                                mKinematicState.GetLon(),
                                                surfaceUpNEDVec,
                                                surfaceSpeedNEDVec_mps,
                                                surfaceFrictionMult);

   // We're going to use NED coordinates, since doing so makes things easier to understand and debug
   UtVec3dX nonLandingGearTotalForceVecNED_lbs;
   nonLandingGearTotalForceVecNED_lbs = mKinematicState.CalcNEDVecFromWCSVec(aNonLandingGearTotalForceVecInertial_lbs);

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

   UtVec3dX landingGearForce_lbs;
   UtVec3dX landingGearMoment_ftlbs;
   mLandingGearPtr->GetForceAndMoments(landingGearForce_lbs, landingGearMoment_ftlbs);

   // The F&M are body-rel
   aInertialTotalGearVector_lbs = mKinematicState.CalcWCSVecFromBodyVec(landingGearForce_lbs);
   aTotalGearMoment_ftlbs       = landingGearMoment_ftlbs;

   // Set crash flag if anything exceeded max compression
   if (mLandingGearPtr->SomethingExceededMaxCompression() && !mCrashed)
   {
      {
         // RAII block
         auto out = ut::log::info() << "Vehicle crashed. Landing gear exceeded compression limit.";
         out.AddNote() << "T = " << utils::TimeToTime(aSimTime_nanosec);
         out.AddNote() << "Vehicle: " << GetName();
      }

      if (!mTestingIgnoreAllCrashes)
      {
         mCrashed = true;
         PerformCrash(aSimTime_nanosec);
      }
   }
}

void wsf::six_dof::RigidBodyMover::CalculateObjectPropulsion(double                 aSimTime_sec,
                                                             double                 aDeltaT_sec,
                                                             double                 aAlt_ft,
                                                             double                 aDynPress_lbsqft,
                                                             double                 aStatPress_lbssqft,
                                                             double                 aSpeed_fps,
                                                             double                 aMach,
                                                             double                 aAlpha_rad,
                                                             double                 aBeta_rad,
                                                             ForceAndMomentsObject& aForceAndMoment,
                                                             double&                aFuelBurnRate_pps,
                                                             double&                aFuelBurned_lbs)
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

void wsf::six_dof::RigidBodyMover::CalculateObjectPropulsion(double                  aSimTime_sec,
                                                             double                  aDeltaT_sec,
                                                             double                  aAlt_ft,
                                                             double                  aDynPress_lbsqft,
                                                             double                  aStatPress_lbssqft,
                                                             double                  aSpeed_fps,
                                                             double                  aMach,
                                                             double                  aAlpha_rad,
                                                             double                  aBeta_rad,
                                                             ForceAndRotationObject& aForceAndRotation,
                                                             double&                 aFuelBurnRate_pps,
                                                             double&                 aFuelBurned_lbs)
{
   ForceAndMomentsObject objectForceAndMoment;

   ObjectPropulsionCalculations(aSimTime_sec,
                                aDeltaT_sec,
                                aAlt_ft,
                                aDynPress_lbsqft,
                                aStatPress_lbssqft,
                                aSpeed_fps,
                                aMach,
                                aAlpha_rad,
                                aBeta_rad,
                                objectForceAndMoment,
                                aFuelBurnRate_pps,
                                aFuelBurned_lbs,
                                false);

   aForceAndRotation.ClearForcesAndRotation();
   aForceAndRotation.AddForce(objectForceAndMoment.GetForceAtRefPoint_lbs());
}

void wsf::six_dof::RigidBodyMover::UpdateObjectPropulsion(double                 aSimTime_sec,
                                                          double                 aDeltaT_sec,
                                                          double                 aAlt_ft,
                                                          double                 aDynPress_lbsqft,
                                                          double                 aStatPress_lbssqft,
                                                          double                 aSpeed_fps,
                                                          double                 aMach,
                                                          double                 aAlpha_rad,
                                                          double                 aBeta_rad,
                                                          ForceAndMomentsObject& aForceAndMoment,
                                                          double&                aFuelBurnRate_pps,
                                                          double&                aFuelBurned_lbs)
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

void wsf::six_dof::RigidBodyMover::ObjectPropulsionCalculations(double                 aSimTime_sec,
                                                                double                 aDeltaT_sec,
                                                                double                 aAlt_ft,
                                                                double                 aDynPress_lbsqft,
                                                                double                 aStatPress_lbssqft,
                                                                double                 aSpeed_fps,
                                                                double                 aMach,
                                                                double                 aAlpha_rad,
                                                                double                 aBeta_rad,
                                                                ForceAndMomentsObject& aForceAndMoment,
                                                                double&                aFuelBurnRate_pps,
                                                                double&                aFuelBurned_lbs,
                                                                bool                   aUpdateData)
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
         mPropulsionObjPtr->Update(utils::TimeToTime(aSimTime_sec));
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

void wsf::six_dof::RigidBodyMover::CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
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

double wsf::six_dof::RigidBodyMover::MaxPotentialManeuverGLoad() const
{
   if (mPilotManagerPtr == nullptr)
   {
      return 0.0;
   }

   if (mActiveAeroObjPtr != nullptr)
   {
      const CommonController* controller = GetPilotManager()->GetSpecialCommonController();

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

void wsf::six_dof::RigidBodyMover::CalculateThrustVectorAndMomentUpdatePropulsion(double                 aSimTime_sec,
                                                                                  double                 aDeltaTime_sec,
                                                                                  const KinematicState&  aState,
                                                                                  ForceAndMomentsObject& aForceAndMoment)
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

   ForceAndMomentsObject thrustFM;
   double                thrustFuelBurnRate_pps = 0.0;
   double                thrustFuelBurned_lbs   = 0.0;

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
      UtVec3dX              tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndMomentsObject tempThrustFM;
      double                tempThrustFuelBurnRate_pps = 0.0;
      double                tempThrustFuelBurned_lbs   = 0.0;

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

void wsf::six_dof::RigidBodyMover::UpdatePropulsionFuelBurn(double                aSimTime_sec,
                                                            double                aDeltaTime_sec,
                                                            const KinematicState& aState)
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

   ForceAndMomentsObject thrustFM;
   double                thrustFuelBurnRate_pps = 0.0;
   double                thrustFuelBurned_lbs   = 0.0;

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

void wsf::six_dof::RigidBodyMover::SetActiveAeroObject(RigidBodyAeroCoreObject* aObject)
{
   if (aObject != nullptr)
   {
      mActiveAeroObjPtr = aObject;
   }
}

void wsf::six_dof::RigidBodyMover::SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects)
{
   mLastSimTime_nanosec = utils::TimeToTime(aSimTime_sec);

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

size_t wsf::six_dof::RigidBodyMover::GetInputHandle(const std::string& aInputName) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetBooleanControlHandle(aInputName);
   }
   return 0;
}

bool wsf::six_dof::RigidBodyMover::GetBooleanInput(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlBoolean(aHandle);
   }
   return false;
}

bool wsf::six_dof::RigidBodyMover::GetLastBooleanInput(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlBooleanLastValue(aHandle);
   }
   return false;
}

void wsf::six_dof::RigidBodyMover::ZeroKinematicsDuringTesting()
{
   mKinematicState.ZeroKinematicsDuringTesting();

   PilotObject* pilot = GetActivePilotObject();
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

   RigidBodyPropulsionSystem* propulsion = GetRigidBodyPropulsionSystem();
   if (propulsion)
   {
      propulsion->SetThrustVectoringPitch_deg(0.0);
      propulsion->SetThrustVectoringYaw_deg(0.0);
   }

   RigidBodyFlightControlSystem* fcs = GetFlightControls();
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

// This provides the maximum potential thrust available, if full throttle, including
// afterburner (if available), is applied
double wsf::six_dof::RigidBodyMover::GetMaximumPotentialThrust_lbs() const
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
double wsf::six_dof::RigidBodyMover::GetMinimumPotentialThrust_lbs() const
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

double wsf::six_dof::RigidBodyMover::GetCurrentThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const KinematicState& currentState = mKinematicState;

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   ForceAndMomentsObject thrustFM;
   double                thrustFuelBurnRate_pps = 0.0;
   double                thrustFuelBurned_lbs   = 0.0;

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
      UtVec3dX              tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndMomentsObject tempThrustFM;
      double                tempThrustFuelBurnRate_pps = 0.0;
      double                tempThrustFuelBurned_lbs   = 0.0;

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

double wsf::six_dof::RigidBodyMover::GetCurrentForwardThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const KinematicState& currentState = mKinematicState;

   mPropulsionObjPtr->SetControlInputValues(mFlightControlsPtr);

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   ForceAndMomentsObject thrustFM;
   double                thrustFuelBurnRate_pps = 0.0;
   double                thrustFuelBurned_lbs   = 0.0;

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
      UtVec3dX              tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX              tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndMomentsObject tempThrustFM;
      double                tempThrustFuelBurnRate_pps = 0.0;
      double                tempThrustFuelBurned_lbs   = 0.0;

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

double wsf::six_dof::RigidBodyMover::GetTotalVehicleFuelFlow_pph() const
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

double wsf::six_dof::RigidBodyMover::GetTotalFuelFlow_pph() const
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

double wsf::six_dof::RigidBodyMover::GetYawMoment_ftlbs() const
{
   return mKinematicState.GetYawMoment();
}

double wsf::six_dof::RigidBodyMover::GetPitchMoment_ftlbs() const
{
   return mKinematicState.GetPitchMoment();
}

double wsf::six_dof::RigidBodyMover::GetRollMoment_ftlbs() const
{
   return mKinematicState.GetRollMoment();
}

double wsf::six_dof::RigidBodyMover::GetWingArea_sqft() const
{
   if (mActiveAeroObjPtr)
   {
      return mActiveAeroObjPtr->GetWingArea_sqft();
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyMover::GetRefArea_sqft() const
{
   if (mActiveAeroObjPtr)
   {
      return mActiveAeroObjPtr->GetRefArea_sqft();
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyMover::CalculateCmAreaForCoreAndControls(double  aMach,
                                                                     double  aAlpha_rad,
                                                                     double  aStickBack,
                                                                     bool    aPowerOn,
                                                                     double& aThrustProduced_lbs,
                                                                     double& aCoreCm,
                                                                     double& aControlsCm)
{
   // Preserve the original kinematic state altitude and velocity
   UtVec3dX preservedSpeed_fps  = mKinematicState.GetVelocityNED_mps();
   double   preservedAltitude_m = mKinematicState.GetAlt_m();

   // This needs to consider various pure moments as well as induced moments

   ForceAndMomentsObject coreForceMoments_CM;
   ForceAndMomentsObject coreForceMoments_RP;
   ForceAndMomentsObject controlsForceMoments_CM;
   ForceAndMomentsObject controlsForceMoments_RP;

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
   UtVec3dX inertialVelVector = mKinematicState.GetVelocityWCS();

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
   mKinematicState.SetSpeed_Mach(aMach, 0.0, 0.0);

   double dynPressure_psf =
      mEnvironment->CalcDynamicPressure_psf(mKinematicState.GetAlt_m(), mKinematicState.GetSpeed_fps());

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
      ForceAndMomentsObject propulsionCoreForceMoments_RP;
      CalculateThrustVectorAndMomentUpdatePropulsion(0.0, 2.0, mKinematicState, propulsionCoreForceMoments_RP);

      EnableThrustVectoring(true);
      ForceAndMomentsObject propulsionControlsForceMoments_RP;
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
   mKinematicState.SetVelocityNED(preservedSpeed_fps.X(), preservedSpeed_fps.Y(), preservedSpeed_fps.Z());
}

void wsf::six_dof::RigidBodyMover::CalculateCLAreaForCoreAndControls(double  aMach,
                                                                     double  aAlpha_rad,
                                                                     double  aStickBack,
                                                                     double& aCoreCL,
                                                                     double& aControlsCL)
{
   aCoreCL     = mActiveAeroObjPtr->CalculateAeroCLArea(aMach, aAlpha_rad);
   aControlsCL = CalculateControlsCLArea(aMach, aAlpha_rad, aStickBack);
}

void wsf::six_dof::RigidBodyMover::CalculateCdAreaForCoreAndControls(double  aMach,
                                                                     double  aAlpha_rad,
                                                                     double  aStickBack,
                                                                     double& aCoreCd,
                                                                     double& aControlsCd)
{
   aCoreCd     = mActiveAeroObjPtr->CalculateAeroCdArea(aMach, aAlpha_rad);
   aControlsCd = CalculateControlsCdArea(aMach, aAlpha_rad, aStickBack);
}

void wsf::six_dof::RigidBodyMover::CalculateTotalCmAndCL(double  aMach,
                                                         double  aAlpha_rad,
                                                         double  aStickBack,
                                                         double& aTotalCm,
                                                         double& aTotalCL)
{
   double coreCm           = 0;
   double controlsCm       = 0;
   double thrustDummyValue = 0.0;
   CalculateCmAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, false, thrustDummyValue, coreCm, controlsCm);

   double coreCL     = 0;
   double controlsCL = 0;
   CalculateCLAreaForCoreAndControls(aMach, aAlpha_rad, aStickBack, coreCL, controlsCL);

   RigidBodyAeroCoreObject* aeroCore = GetActiveAeroObject();
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

void wsf::six_dof::RigidBodyMover::CalculateControlsAeroCoefficientVectors(double  aMach,
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
   PilotObject* pilot = mPilotManagerPtr->GetActivePilot();

   if ((mFlightControlsPtr == nullptr) || (pilot == nullptr))
   {
      aCLArea = 0;
      aCdArea = 0;
      aCmArea = 0;
      return;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = mFreezeFlags.testingNoLag;

   // Set no-lag testing
   mFreezeFlags.testingNoLag = true;

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
   mFlightControlsPtr->Update(simTime_nanosec);

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
   mFreezeFlags.testingNoLag = testingFlag;
}

double wsf::six_dof::RigidBodyMover::CalculateControlsCLArea(double aMach, double aAlpha_rad, double aStickBack)
{
   double CL = 0.0;
   if ((mPilotManagerPtr == nullptr) || (mFlightControlsPtr == nullptr))
   {
      return CL;
   }

   // Get the pilot
   PilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return CL;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = mFreezeFlags.testingNoLag;

   // Set no-lag testing
   mFreezeFlags.testingNoLag = true;

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
   mFlightControlsPtr->Update(simTime_nanosec);

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
   mFreezeFlags.testingNoLag = testingFlag;

   return CL;
}

double wsf::six_dof::RigidBodyMover::CalculateControlsCdArea(double aMach, double aAlpha_rad, double aStickBack)
{
   double Cd = 0.0;
   if ((mPilotManagerPtr == nullptr) || (mFlightControlsPtr == nullptr))
   {
      return Cd;
   }

   // Get the pilot
   PilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return Cd;
   }

   // ---------------------------------------------------------------------------

   // Remember the current testing conditions
   bool testingFlag = mFreezeFlags.testingNoLag;

   // Set no-lag testing
   mFreezeFlags.testingNoLag = true;

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
   mFlightControlsPtr->Update(simTime_nanosec);

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
   mFreezeFlags.testingNoLag = testingFlag;

   return Cd;
}

void wsf::six_dof::RigidBodyMover::CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg) const
{
   aAlpha_deg = 0.0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const CommonController* controller = GetPilotManager()->GetSpecialCommonController();
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

void wsf::six_dof::RigidBodyMover::CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg) const
{
   aBeta_deg = 0.0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const CommonController* controller = GetPilotManager()->GetSpecialCommonController();
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

double wsf::six_dof::RigidBodyMover::CalculatePitchMomentAlphaStickBack_ftlbs(double aAlpha_deg, double aDesiredStickBack)
{
   if (mPilotManagerPtr == nullptr)
   {
      return 0.0;
   }

   // Get the pilot
   PilotObject* pilot = mPilotManagerPtr->GetActivePilot();
   if (pilot == nullptr)
   {
      return 0.0;
   }

   if (!pilot->UsingTestControl())
   {
      // Error must use test control
      auto out =
         ut::log::error() << "TestControl not active in RigidBodyMover::CalculatePitchMomentAlphaStickBack_ftlbs().";
      out.AddNote() << "Vehicle: " << GetName();
      return 0.0;
   }

   // Set the stick position
   pilot->SetTestStickBackControllerPosition(aDesiredStickBack);

   if (mFlightControlsPtr != nullptr)
   {
      int64_t simTime_nanosec = 0;
      mFlightControlsPtr->Update(simTime_nanosec);
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

void wsf::six_dof::RigidBodyMover::RestoreControlSurfaceAngles(
   std::vector<RigidBodyFlightControlSystem::ControlSurfaceElement*>& aControlsList,
   std::vector<double>&                                               aAngleList)
{
   int index = 0;
   for (auto& iter : aControlsList)
   {
      iter->currentAngle_deg = aAngleList.at(index);
      ++index;
   }
}

void wsf::six_dof::RigidBodyMover::CalculateStickBackForZeroMoment(double aAlpha_deg, double& aStickBack)
{
   aStickBack = 0;

   double mach = mKinematicState.GetSpeed_Mach();
   CalculateStickBackForZeroMoment(aAlpha_deg, mach, aStickBack);
}

void wsf::six_dof::RigidBodyMover::CalculateStickBackForZeroMoment(double aAlpha_deg, double aMach, double& aStickBack)
{
   aStickBack = 0;

   if (mPilotManagerPtr == nullptr)
   {
      return;
   }

   const RigidBodyPilotObject* pilot = mPilotManagerPtr->GetActiveRigidBodyPilot();
   if (pilot == nullptr)
   {
      return;
   }

   const RigidBodyCommonController* controller = pilot->GetRigidBodyCommonController();
   if (controller == nullptr)
   {
      return;
   }

   const RigidBodyPropulsionSystem* propulsion = GetRigidBodyPropulsionSystem();
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

bool wsf::six_dof::RigidBodyMover::HasFlaps() const
{
   bool hasFlaps = false;
   for (const auto& part : mAeroPartList)
   {
      // Convert to uppercase
      std::string name = part->GetName();

      // Check if the name includes the word "Flap" in some form
      if ((name.find("flap") != std::string::npos) || (name.find("Flap") != std::string::npos) ||
          (name.find("FLAP") != std::string::npos))
      {
         hasFlaps = true;
         break;
      }
   }
   return hasFlaps;
}

void wsf::six_dof::RigidBodyMover::SetLandingGearPositionAtStart(double aPosition)
{
   ForceLandingGearPositionInstantly("Landing_Gear_Extended", aPosition);
}

void wsf::six_dof::RigidBodyMover::ForceLandingGearPositionInstantly(std::string /* aLandingGearControlSurfaceHandleName */,
                                                                     double aPosition)
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
   RigidBodyPilotObject* pilot = mPilotManagerPtr->GetActiveRigidBodyPilot();
   if (pilot == nullptr)
   {
      return;
   } // If we lack a pilot controller, simply return

   // Set the current angle
   double gearAngle_deg = 90.0 * aPosition; // Gear down anglerolSurfaceAngle_deg(gearHandle);
   mFlightControlsPtr->SetControlSurfaceAngle_deg(mHandleLandingGear, gearAngle_deg);
   mFlightControlsPtr->SetControlSurfaceAngle_deg(mHandleLandingGearNose, gearAngle_deg);
   mFlightControlsPtr->SetControlSurfaceAngle_deg(mHandleLandingGearMainRight, gearAngle_deg);
   mFlightControlsPtr->SetControlSurfaceAngle_deg(mHandleLandingGearMainLeft, gearAngle_deg);

   // The landing gear will now be down, but we need to be sure that there will not
   // be a command at the start to retract the gear.
   pilot->SetLandingGearControlPosition(aPosition);
}

void wsf::six_dof::RigidBodyMover::ForceLandingGearUpInstantly(const std::string& aLandingGearControlSurfaceHandleName)
{
   ForceLandingGearPositionInstantly(aLandingGearControlSurfaceHandleName, 0.0);
}

void wsf::six_dof::RigidBodyMover::ForceLandingGearDownInstantly(const std::string& aLandingGearControlSurfaceHandleName)
{
   ForceLandingGearPositionInstantly(aLandingGearControlSurfaceHandleName, 1.0);
}

void wsf::six_dof::RigidBodyMover::CalculateCurrentMassProperties()
{
   // Begin by setting the "current" values to the "base" values
   mMassProperties.SetCurrentDataToBaseData();

   // Next, we'll add in mass properties for the propulsion system, if it exists
   if (mPropulsionObjPtr != nullptr)
   {
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
         MassProperties subobjectMass = subobjectPtr->GetMassProperties();
         UtVec3dX       posRelToParent_m;
         UtVec3dX       angRelToParentYPR_rad;
         subobjectPtr->GetPositionRelativeToParent(posRelToParent_m, angRelToParentYPR_rad);
         UtVec3dX posRelToParent_ft = posRelToParent_m * UtMath::cFT_PER_M;

         // Move to relative position
         subobjectMass.MoveToLocation(posRelToParent_ft);

         // Add-in the mass properties from the subobject
         mMassProperties += subobjectMass;
      }
   }
}

void wsf::six_dof::RigidBodyMover::SetCurrentWeight_lbs(double aWeight_lbs)
{
   mMassProperties.SetBaseMassProperties(aWeight_lbs,
                                         mMassProperties.GetBaseIxx_slugft2(),
                                         mMassProperties.GetBaseIyy_slugft2(),
                                         mMassProperties.GetBaseIzz_slugft2(),
                                         mMassProperties.GetBaseCmPosRelToRef_ft());
}

void wsf::six_dof::RigidBodyMover::ShiftCurrentCg_ft(UtVec3dX aDeltaCm_ft)
{
   UtVec3dX baseCm_ft    = mMassProperties.GetBaseCmPosRelToRef_ft();
   UtVec3dX updatedCm_ft = baseCm_ft + aDeltaCm_ft;

   mMassProperties.SetBaseMassProperties(mMassProperties.GetBaseMass_lbs(),
                                         mMassProperties.GetBaseIxx_slugft2(),
                                         mMassProperties.GetBaseIyy_slugft2(),
                                         mMassProperties.GetBaseIzz_slugft2(),
                                         updatedCm_ft);
}

const wsf::six_dof::Integrator* wsf::six_dof::RigidBodyMover::GetIntegrator() const
{
   return mIntegratorPtr.get();
}

bool wsf::six_dof::RigidBodyMover::UseSimpleYawDamper() const
{
   bool ok = false;
   if (mPilotManagerPtr != nullptr)
   {
      RigidBodyPilotObject* pilot = mPilotManagerPtr->GetActiveRigidBodyPilot();
      if (pilot != nullptr)
      {
         RigidBodyCommonController* autopilot = pilot->GetRigidBodyCommonController();
         if (autopilot != nullptr)
         {
            ok = autopilot->UseSimpleYawDamper();
         }
         else if (pilot->GetPilotType() == "Manual-Simple") // Check for a simple manual pilot, which lacks an autopilot
         {
            RigidBodyManualPilotSimpleControls* manualPilot = dynamic_cast<RigidBodyManualPilotSimpleControls*>(pilot);
            ok                                              = manualPilot->UseSimpleYawDamper();
         }
      }
   }
   return ok;
}

double wsf::six_dof::RigidBodyMover::GetAngleAileronLeft() const
{
   return GetAngleSurface(mHandleAileronLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleAileronRight() const
{
   return GetAngleSurface(mHandleAileronRight);
}

double wsf::six_dof::RigidBodyMover::GetAngleFlapLeft() const
{
   return GetAngleSurface(mHandleFlapLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleFlapRight() const
{
   return GetAngleSurface(mHandleFlapRight);
}

double wsf::six_dof::RigidBodyMover::GetAngleSpoilerLeft() const
{
   return GetAngleSurface(mHandleSpoilerLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleSpoilerRight() const
{
   return GetAngleSurface(mHandleSpoilerRight);
}

double wsf::six_dof::RigidBodyMover::GetAngleStabilizerLeft() const
{
   return GetAngleSurface(mHandleStabilizerLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleStabilizerRight() const
{
   return GetAngleSurface(mHandleStabilizerRight);
}

double wsf::six_dof::RigidBodyMover::GetAngleElevator() const
{
   return GetAngleSurface(mHandleElevator);
}

double wsf::six_dof::RigidBodyMover::GetAngleRudderLeft() const
{
   return GetAngleSurface(mHandleRudderLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleRudder() const
{
   return GetAngleSurface(mHandleRudder);
}

double wsf::six_dof::RigidBodyMover::GetAngleRudderRight() const
{
   return GetAngleSurface(mHandleRudderRight);
}

double wsf::six_dof::RigidBodyMover::GetAngleSpeedBrake() const
{
   return GetAngleSurface(mHandleSpeedBrake);
}

double wsf::six_dof::RigidBodyMover::GetAngleLandingGear() const
{
   return GetAngleSurface(mHandleLandingGear);
}

double wsf::six_dof::RigidBodyMover::GetAngleLandingGearNose() const
{
   return GetAngleSurface(mHandleLandingGearNose);
}

double wsf::six_dof::RigidBodyMover::GetAngleLandingGearMainLeft() const
{
   return GetAngleSurface(mHandleLandingGearMainLeft);
}

double wsf::six_dof::RigidBodyMover::GetAngleLandingGearMainRight() const
{
   return GetAngleSurface(mHandleLandingGearMainRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedAileronLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleAileronLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedAileronRight() const
{
   return GetNormalizedSurfaceAngle(mHandleAileronRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedFlapLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleFlapLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedFlapRight() const
{
   return GetNormalizedSurfaceAngle(mHandleFlapRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedSpoilerLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleSpoilerLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedSpoilerRight() const
{
   return GetNormalizedSurfaceAngle(mHandleSpoilerRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedStabilizerLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleStabilizerLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedStabilizerRight() const
{
   return GetNormalizedSurfaceAngle(mHandleStabilizerRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedElevator() const
{
   return GetNormalizedSurfaceAngle(mHandleElevator);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedRudderLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleRudderLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedRudder() const
{
   return GetNormalizedSurfaceAngle(mHandleRudder);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedRudderRight() const
{
   return GetNormalizedSurfaceAngle(mHandleRudderRight);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedSpeedBrake() const
{
   return GetNormalizedSurfaceAngle(mHandleSpeedBrake);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedLandingGear() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGear);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedLandingGearNose() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearNose);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedLandingGearMainLeft() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearMainLeft);
}

double wsf::six_dof::RigidBodyMover::GetNormalizedLandingGearMainRight() const
{
   return GetNormalizedSurfaceAngle(mHandleLandingGearMainRight);
}

bool wsf::six_dof::RigidBodyMover::AileronLeftValid() const
{
   if (mHandleAileronLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::AileronRightValid() const
{
   if (mHandleAileronRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::FlapLeftValid() const
{
   if (mHandleFlapLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::FlapRightValid() const
{
   if (mHandleFlapRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::SpoilerLeftValid() const
{
   if (mHandleSpoilerLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::SpoilerRightValid() const
{
   if (mHandleSpoilerRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::StabilizerLeftValid() const
{
   if (mHandleStabilizerLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::StabilizerRightValid() const
{
   if (mHandleStabilizerRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::ElevatorValid() const
{
   if (mHandleElevator > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::RudderLeftValid() const
{
   if (mHandleRudderLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::RudderValid() const
{
   if (mHandleRudder > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::RudderRightValid() const
{
   if (mHandleRudderRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::SpeedBrakeValid() const
{
   if (mHandleSpeedBrake > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::LandingGearValid() const
{
   if (mHandleLandingGear > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::LandingGearNoseValid() const
{
   if (mHandleLandingGearNose > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::LandingGearMainLeftValid() const
{
   if (mHandleLandingGearMainLeft > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::LandingGearMainRightValid() const
{
   if (mHandleLandingGearMainRight > 0)
   {
      return true;
   }

   return false;
}

bool wsf::six_dof::RigidBodyMover::ThrottleValid() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetStickRightInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetStickBackInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetRudderRightInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetThrottleInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
      if (pilot != nullptr)
      {
         size_t handle = pilot->GetStdThrottleMilHandle();
         if (handle > 0)
         {
            double value = pilot->ControlInputValue(handle);

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

double wsf::six_dof::RigidBodyMover::GetSpeedBrakeInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetSpoilerInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetLandingGearInput() const
{
   RigidBodyPilotManager* pilotMgr = GetRigidBodyPilotManager();
   if (pilotMgr != nullptr)
   {
      RigidBodyPilotObject* pilot = pilotMgr->GetActiveRigidBodyPilot();
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

double wsf::six_dof::RigidBodyMover::GetAngleSurface(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlSurfaceAngle_deg(aHandle);
   }

   return 0.0;
}

double wsf::six_dof::RigidBodyMover::GetNormalizedSurfaceAngle(size_t aHandle) const
{
   if (mFlightControlsPtr != nullptr)
   {
      return mFlightControlsPtr->GetControlSurfaceValueNormalized(aHandle);
   }

   return 0.0;
}

void wsf::six_dof::RigidBodyMover::FindParts()
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

void wsf::six_dof::RigidBodyMover::SetActiveAeroObject(const std::string& aName)
{
   SetActiveAeroObject(mAeroCoreObjPtr->GetSubModeByName(aName));
}

void wsf::six_dof::RigidBodyMover::ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_deg)
{
   RigidBodyLandingGear* gear = GetLandingGear();
   if (gear == nullptr)
   {
      auto out = ut::log::warning() << "Cannot apply force to RigidBodySixDOF mover. No landing gear defined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   else
   {
      gear->ApplyExternalForce(aForceMagnitude_lbs, aAngle_deg * UtMath::cRAD_PER_DEG);
   }
}

void wsf::six_dof::RigidBodyMover::RemoveExternalForce()
{
   RigidBodyLandingGear* gear = GetLandingGear();
   if (gear == nullptr)
   {
      auto out = ut::log::warning() << "Cannot remove force from RigidBodySixDOF mover. No landing gear defined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   else
   {
      gear->RemoveExternalForce();
   }
}

void wsf::six_dof::RigidBodyMover::EnableDirectBraking()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->EnableDirectBraking();
   }
   else
   {
      auto out = ut::log::warning() << "No pilot in EnableDirectBraking.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
}

void wsf::six_dof::RigidBodyMover::ReleaseDirectBraking()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->ReleaseDirectBraking();
   }
}

void wsf::six_dof::RigidBodyMover::SetParkingBrake()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->SetParkingBrake();
   }
   else
   {
      auto out = ut::log::warning() << "No pilot in SetParkingBrake.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
}

void wsf::six_dof::RigidBodyMover::ApplyLeftGearBrake(double aValue)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->ApplyLeftGearBrake(aValue);
   }
}

void wsf::six_dof::RigidBodyMover::ApplyRightGearBrake(double aValue)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->ApplyRightGearBrake(aValue);
   }
}

void wsf::six_dof::RigidBodyMover::ReleaseWheelBrakes()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (pilot != nullptr)
   {
      pilot->ReleaseParkingBrake();
   }
}

void wsf::six_dof::RigidBodyMover::RetractLandingGear()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   // The landing gear will now be down, but we need to be sure that there will not
   // be a command at the start to retract the gear.
   pilot->SetLandingGearControlPosition(0.0);
}

void wsf::six_dof::RigidBodyMover::LowerLandingGear()
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   // The landing gear will now be down, but we need to be sure that there will not
   // be a command at the start to retract the gear.
   pilot->SetLandingGearControlPosition(1.0);
}

void wsf::six_dof::RigidBodyMover::SetFlapsPosition(double aPosition)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   pilot->SetFlapsControlPosition(aPosition);
}

void wsf::six_dof::RigidBodyMover::SetSpoilersPosition(double aPosition)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   pilot->SetSpoilersControlPosition(aPosition);
}

void wsf::six_dof::RigidBodyMover::SetThrustReverserPosition(double aPosition)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   pilot->SetThrustReverserControlPosition(aPosition);
}

void wsf::six_dof::RigidBodyMover::SetEnableNWS(bool aNWS_Enabled)
{
   RigidBodyPilotObject* pilot = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();

   // If we lack a pilot controller, simply return
   if (pilot == nullptr)
   {
      return;
   }

   pilot->SetEnableNWS(aNWS_Enabled);
}

bool wsf::six_dof::RigidBodyMover::GetWeightOnWheels() const
{
   RigidBodyLandingGear* gear = GetLandingGear();
   if (gear != nullptr)
   {
      return gear->WeightOnWheels();
   }
   return false;
}

bool wsf::six_dof::RigidBodyMover::GetWeightOnNoseWheel() const
{
   RigidBodyLandingGear* gear = GetLandingGear();
   if (gear == nullptr)
   {
      auto out =
         ut::log::warning() << "Cannot process weight on nose wheel in RigidBodySixDOF Mover. No landing gear defined.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   else
   {
      return gear->WeightOnNoseWheel();
   }
   return false;
}

double wsf::six_dof::RigidBodyMover::GetTaxiSpeedMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.taxiSpeed_Max);
      }
   }
   return 0.0;
}

double wsf::six_dof::RigidBodyMover::GetTaxiYawRateMax()
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         const AutopilotLimitsAndSettings& apControls = autopilot->GetCurrentLimitsAndSettings();
         return static_cast<double>(apControls.taxiYawRate_Max);
      }
   }
   return 0.0;
}

void wsf::six_dof::RigidBodyMover::SetTaxiSpeedMax(double aSpeedMax)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxTaxiSpeedLimit(aSpeedMax);
      }
   }
}

void wsf::six_dof::RigidBodyMover::SetTaxiYawRateMax(double aSpeedMax)
{
   PilotObject* pilot = GetActivePilotObject();
   if (pilot != nullptr)
   {
      CommonController* autopilot = pilot->GetCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetCurrentMaxTaxiYawRateLimit(aSpeedMax);
      }
   }
}

void wsf::six_dof::RigidBodyMover::SetTaxiMode(bool aTaxiModeEnabled)
{
   RigidBodyPilotObject* controller = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (controller != nullptr)
   {
      RigidBodyCommonController* autopilot = controller->GetRigidBodyCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetTaxiMode(aTaxiModeEnabled);
      }
   }
}

void wsf::six_dof::RigidBodyMover::SetTaxiRadius(double aTaxiRadius_ft)
{
   RigidBodyPilotObject* controller = GetRigidBodyPilotManager()->GetActiveRigidBodyPilot();
   if (controller != nullptr)
   {
      RigidBodyCommonController* autopilot = controller->GetRigidBodyCommonController();
      if (autopilot != nullptr)
      {
         autopilot->SetDesiredTaxiRadius(aTaxiRadius_ft);
      }
   }
}

double wsf::six_dof::RigidBodyMover::GetYawMoment_nm() const
{
   return GetYawMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

double wsf::six_dof::RigidBodyMover::GetPitchMoment_nm() const
{
   return GetPitchMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

double wsf::six_dof::RigidBodyMover::GetRollMoment_nm() const
{
   return GetRollMoment_ftlbs() * UtMath::cNM_PER_FTLB;
}

size_t wsf::six_dof::RigidBodyMover::GetControlSurfaceHandle(const std::string& aControlSurfaceName) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      return fltControls->GetControlSurfaceHandle(aControlSurfaceName);
   }
   return 0;
}

size_t wsf::six_dof::RigidBodyMover::GetControlSurfaceHandleContainingString(const std::string& aString) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      return fltControls->GetControlSurfaceHandleContainingString(aString);
   }
   return 0;
}

size_t wsf::six_dof::RigidBodyMover::GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1,
                                                                                 const std::string& aString2) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      return fltControls->GetControlSurfaceHandleContainingTwoStrings(aString1, aString2);
   }
   return 0;
}

double wsf::six_dof::RigidBodyMover::GetAngleOfControlSurface(std::string aControlSurfaceName) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      size_t handle = fltControls->GetControlSurfaceHandle(aControlSurfaceName);

      if (handle > 0)
      {
         return fltControls->GetControlSurfaceAngle_deg(handle);
      }
   }

   return 0.0;
}

// Returns the position (deg) of the specified control surface (based on handle),
// or returns zero if no surface exists
double wsf::six_dof::RigidBodyMover::GetAngleOfControlSurface(size_t aHandle) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      if (aHandle > 0)
      {
         return fltControls->GetControlSurfaceAngle_deg(aHandle);
      }
   }

   return 0.0;
}

// Returns a normalized (either +/-1 or 0-1) angle of the specified control surface (based on handle),
// or returns zero if no surface exists
double wsf::six_dof::RigidBodyMover::GetNormalizedAngleOfControlSurface(size_t aHandle) const
{
   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      if (aHandle > 0)
      {
         return fltControls->GetControlSurfaceValueNormalized(aHandle);
      }
   }

   return 0.0;
}


std::vector<std::string> wsf::six_dof::RigidBodyMover::GetListOfControlSurfaceNames() const
{
   std::vector<std::string> surfaceNameList;

   RigidBodyFlightControlSystem* fltControls = GetFlightControls();
   if (fltControls != nullptr)
   {
      fltControls->GetControlSurfaceNames(surfaceNameList);
   }

   return surfaceNameList;
}

double wsf::six_dof::RigidBodyMover::GetAileronsBothPosition() const
{
   return GetAngleAileronRight();
}

double wsf::six_dof::RigidBodyMover::GetStabilatorLeftPosition() const
{
   return GetAngleStabilizerLeft();
}

double wsf::six_dof::RigidBodyMover::GetStabilatorRightPosition() const
{
   return GetAngleStabilizerRight();
}

double wsf::six_dof::RigidBodyMover::GetRudderPosition() const
{
   return GetAngleRudder();
}

double wsf::six_dof::RigidBodyMover::GetLandingGearPosition() const
{
   return GetAngleLandingGear();
}

double wsf::six_dof::RigidBodyMover::GetSpeedBrakePosition_rad() const
{
   return GetSpeedBrakePosition() * UtMath::cRAD_PER_DEG;
}

double wsf::six_dof::RigidBodyMover::GetSpeedBrakePosition() const
{
   return GetAngleSpeedBrake();
}

double wsf::six_dof::RigidBodyMover::GetFlapsPosition() const
{
   return GetAngleFlapLeft();
}

double wsf::six_dof::RigidBodyMover::GetSpoilersPosition() const
{
   return GetAngleSpoilerLeft();
}

double wsf::six_dof::RigidBodyMover::GetNoseWheelAngle() const
{
   return GetAngleOfControlSurface("Nose_Wheel_Angle_Rgt");
}
