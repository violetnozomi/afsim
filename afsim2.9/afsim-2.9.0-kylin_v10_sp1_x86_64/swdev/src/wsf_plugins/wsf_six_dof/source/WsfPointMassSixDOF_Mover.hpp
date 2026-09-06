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

#ifndef WSFPOINTMASSSIXDOFMOVER_HPP
#define WSFPOINTMASSSIXDOFMOVER_HPP

#include "WsfPointMassSixDOF_AeroCoreObject.hpp"
#include "WsfPointMassSixDOF_Integrator.hpp"
#include "WsfPointMassSixDOF_MoverTestObject.hpp"
#include "WsfPointMassSixDOF_PilotManager.hpp"
#include "WsfPointMassSixDOF_PilotObject.hpp"
#include "WsfPointMassSixDOF_PropulsionSystem.hpp"
#include "WsfPointMassSixDOF_Utils.hpp"
#include "WsfSixDOF_Mover.hpp"

namespace wsf
{
namespace six_dof
{
//! This is a pseudo-6DOF (MRM) mover. It includes 6 degrees-of-freedom,
//! but includes a heavily simplified rotation kinematics model. This model
//! provides many 6DOF capabilities but is easier to use, easier to understand,
//! and executes faster than the rigid-body 6DOF model.
class WSF_SIX_DOF_EXPORT PointMassMover : public Mover
{
public:
   //! Constructor
   explicit PointMassMover(const WsfScenario& aScenario);

   PointMassMover& operator=(const PointMassMover& other) = delete;

   WsfMover* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   static int GetTypeId() { return WsfStringId::UnsafeGetNumber("WSF_POINT_MASS_SIX_DOF_MOVER"); }

   // ====================================================================================================

   // CommonController/autopilot control commands

   // Returns the position (deg) of the speed brakes (common position)
   double GetSpeedBrakePosition() const override;

   // Returns the position (deg) of the flaps (common position)
   double GetFlapsPosition() const override;

   // Returns the position (deg) of the spoilers (common position)
   double GetSpoilersPosition() const override;

   // These functions return the normalized control surface angle
   double GetNormalizedFlapLeft() const override { return GetFlapsPosition(); }
   double GetNormalizedFlapRight() const override { return GetFlapsPosition(); }
   double GetNormalizedSpoilerLeft() const override { return GetSpoilersPosition(); }
   double GetNormalizedSpoilerRight() const override { return GetSpoilersPosition(); }
   double GetNormalizedSpeedBrake() const override { return GetSpeedBrakePosition(); }

   // These functions return true if the data for that control surface is valid

   // Flaps, spoilers, and speedbrakes are always available, but may not have any associated aero data
   bool FlapLeftValid() const override { return true; };
   bool FlapRightValid() const override { return true; };
   bool SpoilerLeftValid() const override { return true; };
   bool SpoilerRightValid() const override { return true; };
   bool SpeedBrakeValid() const override { return true; };

   // Throttle is always available on a PM pilot, but may not be used
   bool ThrottleValid() const override { return GetActivePilotObject() != nullptr; }

   // ====================================================================================================

   // Functions to support scripting

   const char* GetScriptClassName() const override { return "WsfPointMassSixDOF_Mover"; }

   // Set the flaps position in normalized values ranging from 0 when flaps are retracted
   // to 1 when flaps are full deployed.
   void SetFlapsPosition(double aPosition) override;

   // Set the spoilers position in normalized values ranging from 0 when spoilers are retracted
   // to 1 when spoilers are full deployed.
   void SetSpoilersPosition(double aPosition) override;

   // This returns a pointer to the test object. It should only be used in testing.
   PointMassMoverTestObject* GetPointMassTestSupportObject() const { return mTestSupportObjectPtr.get(); }
   MoverTestObject*          GetTestSupportObject() const override { return mTestSupportObjectPtr.get(); }

   // Initialization functions

   void DeriveFrom(const PointMassMover& aSrc);

   // Note:  PointMassSixDOF_VehicleType uses ProcessInput(), but PointMassSixDOF_Vehicle does not

   // ====================================================================================================
   // Mass properties functions

   // This calculates the current mass properties, including those
   // from subobjects and other contributors such as fuel.
   void CalculateCurrentMassProperties() override;

   // This is used to modify the weight of a vehicle without adding fuel or subobjects
   // for testing and should only be called in testing. Additional weight is added at
   // the existing vehicle CG.
   void SetCurrentWeight_lbs(double aWeight_lbs);

   // ====================================================================================================

   // Integrator functions

   // This returns the integrator.
   const Integrator* GetIntegrator() const override;

   // ====================================================================================================

   size_t GetInputHandle(const std::string&) const override { return 0; }
   bool   GetBooleanInput(size_t) const override { return false; }
   bool   GetLastBooleanInput(size_t) const override { return false; }

   // ====================================================================================================

   // "Last" functions

   // This sets the mLastSimTime_nanosec to the specified sim time in seconds. If the
   // aIncludeSubobjects flag is true, all subobjects will also have their last time
   // values set.
   void SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects = false) override;

   // ====================================================================================================

   // Aerodynamics functions

   // This function is used to calculate the lift, drag, side force, and moments
   // using the current state. The reference point is typically (0,0,0), but
   // may be a non-origin reference due to subobjects. This function will include
   // aero contributions by subobjects, if appropriate.
   void CalculateAeroBodyForceAndMoments(UtVec3dX& aLiftBodyVector_lbs,
                                         UtVec3dX& aDragBodyVector_lbs,
                                         UtVec3dX& aSideForceBodyVector_lbs,
                                         UtVec3dX& aRotationAccelerationLimits_rps2,
                                         UtVec3dX& aStabilizingFrequency_rps) override;

   // This function is used to calculate the lift, drag, and side force using the
   // current state. Because moments are not included at this level of detail,
   // zero moments will be reported to any RB6 parent vehicles. Conversely,
   // RB6 vehicles cannot make use of omega-dot limits. This function will
   // include aero contributions by subobjects, if appropriate.
   void CalculateAeroBodyForceAndRotation(UtVec3dX& aLiftBodyVector_lbs,
                                          UtVec3dX& aDragBodyVector_lbs,
                                          UtVec3dX& aSideForceBodyVector_lbs,
                                          UtVec3dX& aMoment_ftlbs,
                                          UtVec3dX& aReferencePt_ft) override;

   // This uses CalculatePitchGLoadAtSpecifiedAlpha internally to calculate the
   // angle of attack (alpha) at the specified g-load using the current conditions.
   // If it cannot attain the specified g-load, it will return the *best* alpha.
   void CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg) const override;

   // This calculates the angle of sideslip (beta) at the specified g-load using the
   // current conditions. If it cannot attain the specified g-load, it will return
   // the *best* beta.
   void CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg) const override;

   // This sets the active aero object using the pointer
   void SetActiveAeroObject(PointMassAeroCoreObject* aObject);

   // This sets the active aero object using the name
   void SetActiveAeroObject(const std::string& aName) override;

   // This returns a pointer to the active aero object
   PointMassAeroCoreObject* GetActiveAeroObject() const { return mActiveAeroObjPtr; }

   // This returns the name of the active aero object
   std::string GetActiveAeroObjectName() const
   {
      return GetActiveAeroObject() ? GetActiveAeroObject()->GetModeName() : "";
   }

   // This returns the ref area (sq-ft) for the current aero mode
   double GetRefArea_sqft() const;

   // This returns the current center of gravity in feet from the reference point
   UtVec3dX GetCenterOfGravity_ft() const override { return mMassProperties.GetCmPosRelToRef_ft(); }

   // This uses the current state to calculate the maximum g-load maneuverability
   // based on the current mass, altitude, speed/mach and CLmax
   double MaxPotentialManeuverGLoad() const override;

   // This returns the fixed-aero object or null if none exists
   AeroCoreObject* GetAeroCoreObject() const override { return mAeroCoreObjPtr; }

   // ====================================================================================================

   // Subobject functions

   // PropulsionSystem functions

   // This calculates F&M for the full propulsion system (all engines) plus
   // the thrust from any subobjects
   void CalculatePropulsionFM(int64_t               aSimTime_nanosec,
                              double                aDeltaT_sec,
                              const KinematicState& aState,
                              UtVec3dX&             aInertialPropulsionVector_lbs,
                              UtVec3dX&             aMaximumRotationAcceleration_rps2);

   // This returns the propulsion system or null if no propulsion system exists
   PropulsionSystem* GetPropulsionSystem() const override { return mPropulsionObjPtr; }

   // This returns the propulsion system or null if no propulsion system exists
   PointMassPropulsionSystem* GetPointMassPropulsionSystem() { return mPropulsionObjPtr; }

   // This provides the maximum potential thrust available, if full throttle, including
   // afterburner (if available), is applied
   double GetMaximumPotentialThrust_lbs() const override;

   // This provides the minimum potential thrust available
   double GetMinimumPotentialThrust_lbs() const override;

   // This provides the current thrust magnitude produced by engines on the vehicle.
   // It includes thrust from any first-level/immediate subobjects
   double GetCurrentThrust_lbs() override;

   // This provides the current forward thrust produced by engines on the vehicle.
   // It includes thrust from any first-level/immediate subobjects
   double GetCurrentForwardThrust_lbs() override;

   // This returns the total fuel flow (lbs/hr) for all engines on the vehicle
   double GetTotalVehicleFuelFlow_pph() const override;

   // This returns the total fuel flow (lbs/hr) for the main engines on the vehicle.
   // It does not include any subobjects.
   double GetTotalFuelFlow_pph() const override;

   // ====================================================================================================

   // PilotObject and FlightControls functions

   // This returns the pilot manager object. Null is returned when
   // no pilot manager is present.
   PilotManager*          GetPilotManager() const override { return mPilotManagerPtr; }
   PointMassPilotManager* GetPointMassPilotManager() const { return mPilotManagerPtr; }

   // This returns the flight controls object. Null is returned when
   // no flight controls are present.
   PointMassFlightControlSystem* GetFlightControls() const { return mFlightControlsPtr; }

   // This returns true if the object includes flaps control surfaces
   bool HasFlaps() const;

   // These functions return the normalized control input (lever) value
   double GetStickRightInput() const override;
   double GetStickBackInput() const override;
   double GetRudderRightInput() const override;
   double GetThrottleInput() const override;
   double GetSpeedBrakeInput() const override;
   double GetSpoilerInput() const override;
   double GetLandingGearInput() const override;

   // ====================================================================================================

   // Testing functions

   // This sets kinematic state parameters to a zero condition for testing
   void ZeroKinematicsDuringTesting() override;

protected:
   explicit PointMassMover(const PointMassMover& aSrc);

   // This is used by all Initialize functions to complete "details"
   bool InitializeDetails(double aSimTime_sec) override;

   // Update functions
   void UpdateFlightControls(int64_t aSimTime_nanosec) override;

   void CalculateThrustVectorAndMomentUpdatePropulsion(double                  aSimTime_sec,
                                                       double                  aDeltaTime_sec,
                                                       const KinematicState&   aState,
                                                       ForceAndRotationObject& aForceAndMoment);

   void UpdatePropulsionFuelBurn(double aSimTime_sec, double aDeltaTime_sec, const KinematicState& aState) override;

   void CalculateObjectPropulsion(double                  aSimTime_sec,
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
                                  double&                 aFuelBurned_lbs) override;

   void CalculateObjectPropulsion(double                 aSimTime_sec,
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
                                  double&                aFuelBurned_lbs) override;

   void UpdateObjectPropulsion(double                  aSimTime_sec,
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
                               double&                 aFuelBurned_lbs);

   void ObjectPropulsionCalculations(double                  aSimTime_sec,
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
                                     bool                    aUpdateData);

   void CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
                                     double& aCurrentTotalFuel_lbs,
                                     double& aMaxTotalFuel_lbs) override;

   // ====================================================================================================

   std::unique_ptr<PointMassMoverTestObject> mTestSupportObjectPtr{nullptr};

   ut::CloneablePtr<PointMassAeroCoreObject> mAeroCoreObjPtr{nullptr};
   PointMassAeroCoreObject*                  mActiveAeroObjPtr = nullptr;

   ut::CloneablePtr<PointMassPropulsionSystem> mPropulsionObjPtr{nullptr};

   ut::CloneablePtr<PointMassIntegrator> mIntegratorPtr{nullptr};

   ut::CloneablePtr<PointMassFlightControlSystem> mFlightControlsPtr{nullptr};
   ut::CloneablePtr<PointMassPilotManager>        mPilotManagerPtr{nullptr};
};
} // namespace six_dof
} // namespace wsf


#endif
