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

#include "WsfPointMassSixDOF_Mover.hpp"

#include <iomanip>
#include <string>

#include "UtDCM.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLLAPos.hpp"
#include "UtLLPos.hpp"
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
#include "WsfPointMassSixDOF_FlightControlSystem.hpp"
#include "WsfPointMassSixDOF_ManualPilotSimpleControls.hpp"
#include "WsfPointMassSixDOF_ThrustProducerObject.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_ForceAndMomentsObject.hpp"
#include "WsfSixDOF_ForceAndRotationObject.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_ObjectManager.hpp"
#include "WsfSixDOF_PilotObject.hpp"
#include "WsfSixDOF_RocketSolidPropellantEngine.hpp"
#include "WsfSixDOF_Sequencer.hpp"
#include "WsfSixDOF_Terrain.hpp"
#include "WsfSixDOF_TypeManager.hpp"
#include "WsfTrackManager.hpp"
#include "WsfVisualPart.hpp"
#include "WsfWaypoint.hpp"
#include "WsfWeapon.hpp"

const int64_t gPointMassNanosecStepSize = 50000000; // 50 msec (20 Hz)

wsf::six_dof::PointMassMover::PointMassMover(const WsfScenario& aScenario)
   : Mover(aScenario)
   , mTestSupportObjectPtr(new PointMassMoverTestObject(this))
   , mPropulsionObjPtr(new PointMassPropulsionSystem())
{
   auto typeManager    = static_cast<TypeManager*>(aScenario.FindExtension("wsf_six_dof"));
   mIntegratorTypeName = typeManager->GetDefaultPointMassSixDOF_IntegratorName();
   mStepSize_nanosec   = gPointMassNanosecStepSize;
}

// =================================================================
wsf::six_dof::PointMassMover::PointMassMover(const PointMassMover& aSrc)
   : Mover(aSrc)
   , mTestSupportObjectPtr(new PointMassMoverTestObject(this))
   , mAeroCoreObjPtr(aSrc.mAeroCoreObjPtr)
   , mPropulsionObjPtr(aSrc.mPropulsionObjPtr)
   , mIntegratorPtr(aSrc.mIntegratorPtr)
   , mFlightControlsPtr(aSrc.mFlightControlsPtr)
   , mPilotManagerPtr(aSrc.mPilotManagerPtr)
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

   if (mPropulsionObjPtr)
   {
      mPropulsionObjPtr->SetParentVehicle(this);
   }

   if (mIntegratorPtr)
   {
      mIntegratorPtr->SetParentVehicle(this);
   }
}

// =================================================================
WsfMover* wsf::six_dof::PointMassMover::Clone() const
{
   return new PointMassMover(*this);
}

// =================================================================
bool wsf::six_dof::PointMassMover::ProcessInput(UtInput& aInput)
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
         throw UtInput::BadValue(aInput, "PointMassMover was not defined with a PointMassPilotObject, can not used route!");
      }

      mWsfRoutePtr = routePtr.release();

      pilot = GetActivePilotObject();
      if (pilot != nullptr)
      {
         pilot->SetPlannedRoute(ConvertWsfRouteToSixDOF_Route(mWsfRoutePtr));
      }
   }
   else if (command == "point_mass_vehicle_type")
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
            auto out = ut::log::error() << "WsfPointMassSixDOF_MoverType is trying to derive from an unknown type in "
                                           "WsfPointMassSixDOF_MoverType::ProcessInput.";
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
         auto out = ut::log::error() << "PointMassMover is using a duplicate name in PointMassMover::ProcessInput.";
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
         auto         derivedFromPointMassSixDOF_ = dynamic_cast<const PointMassMover*>(derivedFromObject);
         if (derivedFromPointMassSixDOF_ != nullptr)
         {
            DeriveFrom(*derivedFromPointMassSixDOF_);
         }
         else
         {
            // Source vehicle is of a different type, which we don't support for now
            auto out =
               ut::log::error()
               << "PointMassMover is attempting to derive from a different type in PointMassMover::ProcessInput.";
            out.AddNote() << "Name: " << typeName;
            out.AddNote() << "Deriving from: " << baseType;
            throw UtInput::UnknownCommand(aInput);
         }
      }

      // Previously, we registered the object with the manager here, but now handle this
      // outside of WsfPointMassSixDOF_MoverType::ProcessInput
      // mObjectTypeManagerPtr->RegisterObjectType(this);

      UtInputBlock block(aInput, "end_point_mass_vehicle_type");
      while (block.ReadCommand())
      {
         if (Mover::ProcessVehicleInput(aInput))
         {
         }
         else if (command == "aero_data")
         {
            if (mAeroCoreObjPtr == nullptr)
            {
               mAeroCoreObjPtr = new PointMassAeroCoreObject();
            }
            mAeroCoreObjPtr->ProcessInput(aInput);
         }
         else if (command == "flight_controls")
         {
            if (mFlightControlsPtr == nullptr)
            {
               mFlightControlsPtr = new PointMassFlightControlSystem();
            }
            mFlightControlsPtr->ProcessInput(aInput);
         }
         else if (command == "pilot_manager")
         {
            if (mPilotManagerPtr == nullptr)
            {
               mPilotManagerPtr = new PointMassPilotManager();
            }
            mPilotManagerPtr->ProcessInput(aInput);
         }
         else if (command == "propulsion_data")
         {
            mPropulsionObjPtr->ProcessInput(aInput, typeManager);
         }
         else
         {
            auto out = ut::log::error() << "Unrecognized command within WsfPointMassSixDOF_MoverType::ProcessInput().";
            out.AddNote() << "Command: " << command;
            out.AddNote() << "Location: " << aInput.GetLocation();
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "vehicle_type")
   {
      std::string typeName;
      aInput.ReadValue(typeName);

      auto typeManager = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));

      const Mover* tempObjType = typeManager->GetObjectTypeByName(typeName);

      auto tempPointMassSixDOF_Type = dynamic_cast<const PointMassMover*>(tempObjType);
      if (tempPointMassSixDOF_Type == nullptr)
      {
         throw UtInput::BadValue(aInput, "Could not find requested point-mass vehicle type!");
      }
      DeriveFrom(*tempPointMassSixDOF_Type);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

void wsf::six_dof::PointMassMover::DeriveFrom(const PointMassMover& aSrc)
{
   Mover::DeriveFrom(aSrc);

   if (aSrc.mAeroCoreObjPtr)
   {
      mAeroCoreObjPtr = aSrc.mAeroCoreObjPtr;
      SetActiveAeroObject(mAeroCoreObjPtr);
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
}

// =================================================================

void wsf::six_dof::PointMassMover::SetActiveAeroObject(PointMassAeroCoreObject* aObject)
{
   if (aObject != nullptr)
   {
      mActiveAeroObjPtr = aObject;
   }
}

void wsf::six_dof::PointMassMover::SetActiveAeroObject(const std::string& aName)
{
   SetActiveAeroObject(mAeroCoreObjPtr->GetSubModeByName(aName));
}

bool wsf::six_dof::PointMassMover::InitializeDetails(double aSimTime_sec)
{
   int64_t simTime_nanosec = utils::TimeToTime(aSimTime_sec);

   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->SetParentVehicle(this);
      mPilotManagerPtr->Initialize(simTime_nanosec);
   }

   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->SetParentVehicle(this);
      mFlightControlsPtr->SetPilotManager(mPilotManagerPtr);
      if (mPilotManagerPtr != nullptr)
      {
         mFlightControlsPtr->SetPilotObject(mPilotManagerPtr->GetActivePointMassPilot());
      }
      mFlightControlsPtr->Initialize(simTime_nanosec);
   }

   // Set aero reference areas for movable aero objects
   if (mAeroCoreObjPtr != nullptr)
   {
      mAeroCoreObjPtr->Initialize();
      SetActiveAeroObject(mAeroCoreObjPtr);
   }

   // We need to loop through all engines and get the controls handle
   if (mPropulsionObjPtr != nullptr)
   {
      mPropulsionObjPtr->SetParentVehicle(this);
      mPropulsionObjPtr->Initialize(simTime_nanosec);
      mPropulsionObjPtr->LinkControls(mFlightControlsPtr);
   }

   // Set the integrator
   auto                 typeManager         = static_cast<TypeManager*>(GetScenario().FindExtension("wsf_six_dof"));
   Integrator*          integrator          = typeManager->GetIntegratorByName(mIntegratorTypeName);
   PointMassIntegrator* pointMassIntegrator = dynamic_cast<PointMassIntegrator*>(integrator);

   if (pointMassIntegrator != nullptr)
   {
      double defaultStepSize_sec     = utils::TimeToTime(gPointMassNanosecStepSize);
      double moverUpdateInterval_sec = GetUpdateInterval();

      if (moverUpdateInterval_sec > utils::cEPSILON_SIMTIME_SEC && moverUpdateInterval_sec < defaultStepSize_sec)
      {
         mStepSize_nanosec = utils::TimeToTime(GetUpdateInterval());
      }

      mIntegratorPtr = pointMassIntegrator->Clone();
      mIntegratorPtr->SetParentVehicle(this);
   }
   else
   {
      std::string exceptionStr = "No integrator available for object = " + GetName();
      throw(UtException(exceptionStr));
   }

   return true;
}

void wsf::six_dof::PointMassMover::CalculateAeroBodyForceAndRotation(UtVec3dX& aLiftBodyVector_lbs,
                                                                     UtVec3dX& aDragBodyVector_lbs,
                                                                     UtVec3dX& aSideForceBodyVector_lbs,
                                                                     UtVec3dX& aRotationAccelerationLimits_rps2,
                                                                     UtVec3dX& aStabilizingFrequency_rps)
{
   if (mEnvironment == nullptr)
   {
      // Cannot do aero without an atmosphere
      aLiftBodyVector_lbs.Set(0.0);
      aDragBodyVector_lbs.Set(0.0);
      aSideForceBodyVector_lbs.Set(0.0);
      aRotationAccelerationLimits_rps2.Set(0.0);
      return;
   }

   if (mActiveAeroObjPtr == nullptr)
   {
      // Cannot do aero without a base aero object
      aLiftBodyVector_lbs.Set(0.0);
      aDragBodyVector_lbs.Set(0.0);
      aSideForceBodyVector_lbs.Set(0.0);
      aRotationAccelerationLimits_rps2.Set(0.0);
      return;
   }

   // Get primary aero vectors (lift, drag and side force)
   UtVec3dX liftBodyUnitVector, dragBodyUnitVector, sideBodyUnitVector;

   mKinematicState.GetBodyAeroVectors(mKinematicState.GetDCM(),
                                      mKinematicState.GetVelocityWCS(),
                                      liftBodyUnitVector,
                                      dragBodyUnitVector,
                                      sideBodyUnitVector);

   double lift                     = 0.0;
   double drag                     = 0.0;
   double side                     = 0.0;
   double maximumRollAcceleration  = 0.0;
   double maximumPitchAcceleration = 0.0;
   double maximumYawAcceleration   = 0.0;
   double rollFrequency            = 0.0;
   double alphaFrequency           = 0.0;
   double betaFrequency            = 0.0;

   if (mActiveAeroObjPtr != nullptr)
   {
      double flapsPosition      = 0.0;
      double spoilersPosition   = 0.0;
      double speedbrakePosition = 0.0;

      PointMassFlightControlSystem* fcs = GetFlightControls();
      if (fcs != nullptr)
      {
         speedbrakePosition = fcs->GetSpeedbrakesPosition();
         spoilersPosition   = fcs->GetSpoilersPosition();
         flapsPosition      = fcs->GetFlapsPosition();
      }

      mActiveAeroObjPtr->CalculateCoreAeroFM(mKinematicState.GetDynamicPressure_psf(),
                                             mKinematicState.GetSpeed_Mach(),
                                             mKinematicState.GetAlpha_rad(),
                                             mKinematicState.GetBeta_rad(),
                                             speedbrakePosition,
                                             flapsPosition,
                                             spoilersPosition,
                                             lift,
                                             drag,
                                             side,
                                             maximumRollAcceleration,
                                             maximumPitchAcceleration,
                                             maximumYawAcceleration,
                                             alphaFrequency,
                                             betaFrequency,
                                             rollFrequency,
                                             mSizeFactor.GetSizeFactorRadius());

      double altitude_m     = mKinematicState.GetAlt_m();
      double altitudeFactor = mEnvironment->CalcDensityRatio(altitude_m);

      maximumRollAcceleration *= altitudeFactor;
      maximumPitchAcceleration *= altitudeFactor;
      maximumYawAcceleration *= altitudeFactor;

      alphaFrequency *= altitudeFactor;
      betaFrequency *= altitudeFactor;
      rollFrequency *= altitudeFactor;
   }

   aRotationAccelerationLimits_rps2.Set(maximumRollAcceleration, maximumPitchAcceleration, maximumYawAcceleration);
   aStabilizingFrequency_rps.Set(rollFrequency, alphaFrequency, betaFrequency);

   // Calc aero force vectors using the normalized vector and the magnitude
   UtVec3dX liftBodyForce_lbs(liftBodyUnitVector);
   UtVec3dX dragBodyForce_lbs(dragBodyUnitVector);
   UtVec3dX sideBodyForce_lbs(sideBodyUnitVector);

   liftBodyForce_lbs *= lift;
   dragBodyForce_lbs *= drag;
   sideBodyForce_lbs *= side;

   if (!mSubObjectList.empty())
   {
      ForceAndRotationObject thisObjectFM;
      UtVec3dX               aeroTotalBodyForce_lbs = liftBodyForce_lbs + dragBodyForce_lbs + sideBodyForce_lbs;

      thisObjectFM.AddForce(aeroTotalBodyForce_lbs);

      // Now, get additional F&M from sub-objects
      for (auto& subObjIter : mSubObjectList)
      {
         UtVec3dX subObjectLiftBodyVector_lbs;
         UtVec3dX subObjectDragBodyVector_lbs;
         UtVec3dX subObjectSideBodyVector_lbs;
         UtVec3dX subObjectRotationAccelerationLimits_rps2; // Discarded for subobjects
         UtVec3dX subObjectStabilizingFrequency_rps2;       // Discarded for subobjects

         subObjIter->CalculateAeroBodyForceAndRotation(subObjectLiftBodyVector_lbs,
                                                       subObjectDragBodyVector_lbs,
                                                       subObjectSideBodyVector_lbs,
                                                       subObjectRotationAccelerationLimits_rps2,
                                                       subObjectStabilizingFrequency_rps2);

         UtVec3dX subObjectAeroTotalBodyForce_lbs =
            subObjectLiftBodyVector_lbs + subObjectDragBodyVector_lbs + subObjectSideBodyVector_lbs;
         ForceAndRotationObject subObjectFM;
         subObjectFM.AddForce(subObjectAeroTotalBodyForce_lbs);

         // Add the subobject F&M to this object's (the parent's) F&M
         thisObjectFM += subObjectFM;
      }

      // Get the total F&M vectors
      UtVec3dX totalAeroTotalBodyForce_lbs = thisObjectFM.GetForce_lbs();
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
   }
   else
   {
      // Set the return values
      aLiftBodyVector_lbs      = liftBodyForce_lbs;
      aDragBodyVector_lbs      = dragBodyForce_lbs;
      aSideForceBodyVector_lbs = sideBodyForce_lbs;
   }
}

void wsf::six_dof::PointMassMover::CalculateAeroBodyForceAndMoments(UtVec3dX& aLiftBodyVector_lbs,
                                                                    UtVec3dX& aDragBodyVector_lbs,
                                                                    UtVec3dX& aSideForceBodyVector_lbs,
                                                                    UtVec3dX& aMoment_ftlbs,
                                                                    UtVec3dX& /*aReferencePt_ft*/)
{
   aMoment_ftlbs.Set(0.0);

   UtVec3dX rotationAccelerationLimit_rps2;
   UtVec3dX stabilizingFrequency_rps;
   CalculateAeroBodyForceAndMoments(aLiftBodyVector_lbs,
                                    aDragBodyVector_lbs,
                                    aSideForceBodyVector_lbs,
                                    rotationAccelerationLimit_rps2,
                                    stabilizingFrequency_rps);
}

void wsf::six_dof::PointMassMover::CalculatePropulsionFM(int64_t               aSimTime_nanosec,
                                                         double                aDeltaT_sec,
                                                         const KinematicState& aState,
                                                         UtVec3dX&             aInertialPropulsionVector_lbs,
                                                         UtVec3dX&             aMaximumRotationAcceleration_rps2)
{
   if (mPropulsionObjPtr == nullptr)
   {
      aInertialPropulsionVector_lbs.Set(0.0);
      return;
   }

   // Calc the thrust vector by doing an 'update' on the propulsion object and a 'calculate' on propulsion objects of any children
   ForceAndRotationObject tempThrustFM;
   double                 simTime_sec = utils::TimeToTime(aSimTime_nanosec);
   CalculateThrustVectorAndMomentUpdatePropulsion(simTime_sec, aDeltaT_sec, aState, tempThrustFM);

   UtVec3dX thrustVector_lbs    = tempThrustFM.GetForce_lbs();
   UtVec3dX thrustOmegaDot_rps2 = tempThrustFM.GetMaximumAngularAcceleration_rps2();

   // The F&M above are body-rel - convert to inertial
   aInertialPropulsionVector_lbs     = mKinematicState.GetDCM().InverseTransform(thrustVector_lbs);
   aMaximumRotationAcceleration_rps2 = thrustOmegaDot_rps2;
}

void wsf::six_dof::PointMassMover::CalculateObjectPropulsion(double                  aSimTime_sec,
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
   ObjectPropulsionCalculations(aSimTime_sec,
                                aDeltaT_sec,
                                aAlt_ft,
                                aDynPress_lbsqft,
                                aStatPress_lbssqft,
                                aSpeed_fps,
                                aMach,
                                aAlpha_rad,
                                aBeta_rad,
                                aForceAndRotation,
                                aFuelBurnRate_pps,
                                aFuelBurned_lbs,
                                false);
}

void wsf::six_dof::PointMassMover::CalculateObjectPropulsion(double                 aSimTime_sec,
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
   ForceAndRotationObject objectForceAndRotation;

   CalculateObjectPropulsion(aSimTime_sec,
                             aDeltaT_sec,
                             aAlt_ft,
                             aDynPress_lbsqft,
                             aStatPress_lbssqft,
                             aSpeed_fps,
                             aMach,
                             aAlpha_rad,
                             aBeta_rad,
                             objectForceAndRotation,
                             aFuelBurnRate_pps,
                             aFuelBurned_lbs);

   aForceAndMoment.ClearForcesAndMoments();
   aForceAndMoment.AddForceAtReferencePoint(objectForceAndRotation.GetForce_lbs());
}

void wsf::six_dof::PointMassMover::UpdateObjectPropulsion(double                  aSimTime_sec,
                                                          double                  aDeltaT_sec,
                                                          double                  aAlt_ft,
                                                          double                  aDynPress_lbsqft,
                                                          double                  aStatPress_lbssqft,
                                                          double                  aSpeed_fps,
                                                          double                  aMach,
                                                          double                  aAlpha_rad,
                                                          double                  aBeta_rad,
                                                          ForceAndRotationObject& aForceAndMoment,
                                                          double&                 aFuelBurnRate_pps,
                                                          double&                 aFuelBurned_lbs)
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

void wsf::six_dof::PointMassMover::ObjectPropulsionCalculations(double                  aSimTime_sec,
                                                                double                  aDeltaT_sec,
                                                                double                  aAlt_ft,
                                                                double                  aDynPress_lbsqft,
                                                                double                  aStatPress_lbssqft,
                                                                double                  aSpeed_fps,
                                                                double                  aMach,
                                                                double                  aAlpha_rad,
                                                                double                  aBeta_rad,
                                                                ForceAndRotationObject& aForceAndMoment,
                                                                double&                 aFuelBurnRate_pps,
                                                                double&                 aFuelBurned_lbs,
                                                                bool                    aUpdateData)
{
   aForceAndMoment.ClearForcesAndRotation();
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
      // This is captive, so we adjust for the relative position
      aForceAndMoment.ReorientForce_ft(mAngRelToParentYPR_rad);
   }
}

void wsf::six_dof::PointMassMover::CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
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

void wsf::six_dof::PointMassMover::UpdateFlightControls(int64_t aSimTime_nanosec)
{
   if (mFlightControlsPtr != nullptr)
   {
      mFlightControlsPtr->Update(aSimTime_nanosec);
   }
}

double wsf::six_dof::PointMassMover::MaxPotentialManeuverGLoad() const
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
            double lift_lbs = mKinematicState.GetDynamicPressure_psf() * mActiveAeroObjPtr->GetRefArea_sqft() * clMax;
            double mass_lbs = mMassProperties.GetMass_lbs();
            double gLoad    = lift_lbs / mass_lbs;
            return gLoad;
         }
      }
   }
   return 0.0;
}

void wsf::six_dof::PointMassMover::CalculateThrustVectorAndMomentUpdatePropulsion(double                aSimTime_sec,
                                                                                  double                aDeltaTime_sec,
                                                                                  const KinematicState& aState,
                                                                                  ForceAndRotationObject& aForceAndMoment)
{
   if (mPropulsionObjPtr == nullptr)
   {
      aForceAndMoment.ClearForcesAndRotation();
      return;
   }

   double alt_ft          = aState.GetAlt_ft();
   double speed_fps       = aState.GetSpeed_fps();
   double mach            = aState.GetSpeed_Mach();
   double alpha_rad       = aState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = aState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = aState.GetDynamicPressure_psf();

   ForceAndRotationObject thrustFM;
   double                 thrustFuelBurnRate_pps = 0.0;
   double                 thrustFuelBurned_lbs   = 0.0;

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

   // TODO -- Review this to ensure that "double burn" does not occur
   // Loop through sub-objects
   // Investigate/test to determine if a double burn might occur if the sub-objects burn here and on their own
   for (auto& subObject : mSubObjectList)
   {
      UtVec3dX               tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndRotationObject tempThrustFM;
      double                 tempThrustFuelBurnRate_pps = 0.0;
      double                 tempThrustFuelBurned_lbs   = 0.0;

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

void wsf::six_dof::PointMassMover::UpdatePropulsionFuelBurn(double                aSimTime_sec,
                                                            double                aDeltaTime_sec,
                                                            const KinematicState& aState)
{
   if (GetPropulsionSystem() == nullptr)
   {
      return;
   }

   double alt_ft          = aState.GetAlt_ft();
   double speed_fps       = aState.GetSpeed_fps();
   double mach            = aState.GetSpeed_Mach();
   double alpha_rad       = aState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = aState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = aState.GetDynamicPressure_psf();

   ForceAndRotationObject thrustFM;
   double                 thrustFuelBurnRate_pps = 0.0;
   double                 thrustFuelBurned_lbs   = 0.0;

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

void wsf::six_dof::PointMassMover::SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects)
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

   if (mPilotManagerPtr != nullptr)
   {
      mPilotManagerPtr->SetLastSimTime(mLastSimTime_nanosec); // This calls SetLastSimTime for all pilot objects
   }
}

void wsf::six_dof::PointMassMover::ZeroKinematicsDuringTesting()
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

      pilot->SetTestFlapsControllerPosition(0.0);
      pilot->SetTestSpoilersControllerPosition(0.0);
      pilot->SetTestSpeedBrakesControllerPosition(0.0);

      if (!preservedUsingTestControl)
      {
         pilot->ReleaseTestControl();
      }
   }
}

// This provides the maximum potential thrust available, if full throttle, including
// afterburner (if available), is applied
double wsf::six_dof::PointMassMover::GetMaximumPotentialThrust_lbs() const
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
double wsf::six_dof::PointMassMover::GetMinimumPotentialThrust_lbs() const
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

double wsf::six_dof::PointMassMover::GetCurrentThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const KinematicState& currentState = mKinematicState;

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   ForceAndRotationObject thrustFM;
   double                 thrustFuelBurnRate_pps = 0.0;
   double                 thrustFuelBurned_lbs   = 0.0;

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
      UtVec3dX               tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndRotationObject tempThrustFM;
      double                 tempThrustFuelBurnRate_pps = 0.0;
      double                 tempThrustFuelBurned_lbs   = 0.0;

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

double wsf::six_dof::PointMassMover::GetCurrentForwardThrust_lbs()
{
   if (mPropulsionObjPtr == nullptr)
   {
      return 0.0;
   }

   const KinematicState& currentState = mKinematicState;

   double alt_ft          = currentState.GetAlt_ft();
   double speed_fps       = currentState.GetSpeed_fps();
   double mach            = currentState.GetSpeed_Mach();
   double alpha_rad       = currentState.GetAlpha_deg() * UtMath::cRAD_PER_DEG;
   double beta_rad        = currentState.GetBeta_deg() * UtMath::cRAD_PER_DEG;
   double dynPressure_psf = currentState.GetDynamicPressure_psf();
   double simTime_sec     = 0.0;
   double deltaTime_sec   = 0.01;

   ForceAndRotationObject thrustFM;
   double                 thrustFuelBurnRate_pps = 0.0;
   double                 thrustFuelBurned_lbs   = 0.0;

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
      UtVec3dX               tempThrustForce_lbs(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustLocation_ft(0.0, 0.0, 0.0);
      UtVec3dX               tempThrustMoment_ftlbs(0.0, 0.0, 0.0);
      ForceAndRotationObject tempThrustFM;
      double                 tempThrustFuelBurnRate_pps = 0.0;
      double                 tempThrustFuelBurned_lbs   = 0.0;

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

   return thrustFM.GetForce_lbs().X();
}

double wsf::six_dof::PointMassMover::GetTotalVehicleFuelFlow_pph() const
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

double wsf::six_dof::PointMassMover::GetTotalFuelFlow_pph() const
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

double wsf::six_dof::PointMassMover::GetRefArea_sqft() const
{
   if (mActiveAeroObjPtr)
   {
      return mActiveAeroObjPtr->GetRefArea_sqft();
   }

   return 0.0;
}

void wsf::six_dof::PointMassMover::CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg) const
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

   double refArea_sqft = GetActiveAeroObject()->GetRefArea_sqft();
   areaQ               = refArea_sqft * Q;

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

void wsf::six_dof::PointMassMover::CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg) const
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

   double refArea_sqft = GetActiveAeroObject()->GetRefArea_sqft();
   areaQ               = refArea_sqft * Q;

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

bool wsf::six_dof::PointMassMover::HasFlaps() const
{
   bool hasFlaps = false;
   return hasFlaps;
}

double wsf::six_dof::PointMassMover::GetStickRightInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetStickRightControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetStickBackInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetStickBackControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetRudderRightInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetRudderRightControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetThrottleInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetThrottleControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetSpeedBrakeInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetSpeedBrakesControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetSpoilerInput() const
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         return pilot->GetSpoilersControllerPosition();
      }
   }
   return 0.0;
}

double wsf::six_dof::PointMassMover::GetLandingGearInput() const
{
   return 0.0;
}

void wsf::six_dof::PointMassMover::CalculateCurrentMassProperties()
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

         // Add-in the mass properties from the subobject
         mMassProperties += subobjectMass;
      }
   }
}

void wsf::six_dof::PointMassMover::SetCurrentWeight_lbs(double aWeight_lbs)
{
   mMassProperties.SetBaseMassProperties(aWeight_lbs, mMassProperties.GetBaseCmPosRelToRef_ft());
}

const wsf::six_dof::Integrator* wsf::six_dof::PointMassMover::GetIntegrator() const
{
   return mIntegratorPtr.get();
}

void wsf::six_dof::PointMassMover::SetFlapsPosition(double aPosition)
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         pilot->SetFlapsControlPosition(aPosition);
      }
   }
}

void wsf::six_dof::PointMassMover::SetSpoilersPosition(double aPosition)
{
   PointMassPilotManager* pilotMgr = GetPointMassPilotManager();
   if (pilotMgr != nullptr)
   {
      PointMassPilotObject* pilot = pilotMgr->GetActivePointMassPilot();
      if (pilot != nullptr)
      {
         pilot->SetSpoilersControlPosition(aPosition);
      }
   }
}

double wsf::six_dof::PointMassMover::GetSpeedBrakePosition() const
{
   return 0.0; // GetAngleOfControlSurface("Speed_Brake_Extended");
}

double wsf::six_dof::PointMassMover::GetFlapsPosition() const
{
   return 0.0; // GetAngleOfControlSurface("Flaps_TE_Down");
}

double wsf::six_dof::PointMassMover::GetSpoilersPosition() const
{
   return 0.0; // GetAngleOfControlSurface("Spoilers_Extended");
}
