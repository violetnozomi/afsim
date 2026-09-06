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

#ifndef WSFRIGIDBODYSIXDOFMOVER_HPP
#define WSFRIGIDBODYSIXDOFMOVER_HPP

#include "WsfRigidBodySixDOF_AeroCoreObject.hpp"
#include "WsfRigidBodySixDOF_AeroMovableObject.hpp"
#include "WsfRigidBodySixDOF_FlightControlSystem.hpp"
#include "WsfRigidBodySixDOF_Integrator.hpp"
#include "WsfRigidBodySixDOF_LandingGear.hpp"
#include "WsfRigidBodySixDOF_MoverTestObject.hpp"
#include "WsfRigidBodySixDOF_PilotManager.hpp"
#include "WsfRigidBodySixDOF_PilotObject.hpp"
#include "WsfRigidBodySixDOF_PropulsionSystem.hpp"
#include "WsfRigidBodySixDOF_ThrustProducerObject.hpp"
#include "WsfRigidBodySixDOF_Utils.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfSixDOF_Route.hpp"
#include "WsfSixDOF_Waypoint.hpp"

namespace wsf
{
namespace six_dof
{
//! This is an evolution of the P6DOF mover. It includes 6 degrees-of-freedom,
//! but uses only the diagonal elements of the moment-of-inertia tensor.
class WSF_SIX_DOF_EXPORT RigidBodyMover : public Mover
{
public:
   //! Constructor
   explicit RigidBodyMover(const WsfScenario& aScenario);

   RigidBodyMover& operator=(const RigidBodyMover& other) = delete;

   WsfMover* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;

   static int GetTypeId() { return WsfStringId::UnsafeGetNumber("WSF_RIGID_BODY_SIX_DOF_MOVER"); }

   // ====================================================================================================

   // CommonController/autopilot control commands

   // Returns the most recent yaw moment in newton*meters
   double GetYawMoment_nm() const;

   // Returns the most recent yaw moment in ft*lbs
   double GetYawMoment_ftlbs() const;

   // Returns the most recent pitch moment in newton*meters
   double GetPitchMoment_nm() const;

   // Returns the most recent pitch moment in ft*lbs
   double GetPitchMoment_ftlbs() const;

   // Returns the most recent roll moment in newton*meters
   double GetRollMoment_nm() const;

   // Returns the most recent roll moment in ft*lbs
   double GetRollMoment_ftlbs() const;

   // This returns a 'handle' to a control surface. If there is no control surface with the specified name, the function
   // returns 0;
   size_t GetControlSurfaceHandle(const std::string& aControlSurfaceName) const;

   // This returns a 'handle' to the first control surface that contains the specified string. If there is no control
   // surface that contains the name, the function returns 0;
   size_t GetControlSurfaceHandleContainingString(const std::string& aString) const;

   // This returns a 'handle' to the first control surface that contains the two specified strings. If there is no
   // control surface that contains the names, the function returns 0;
   size_t GetControlSurfaceHandleContainingTwoStrings(const std::string& aString1, const std::string& aString2) const;

   // Returns the position (deg) of the specified control surface, or returns zero if no surface exists
   double GetAngleOfControlSurface(std::string aControlSurfaceName) const;

   // Returns the position (deg) of the specified control surface (based on handle), or returns zero if no surface exists
   double GetAngleOfControlSurface(size_t aHandle) const;

   // Returns a normalized (either +/-1 or 0-1) angle of the specified control surface (based on handle),
   // or returns zero if no surface exists
   double GetNormalizedAngleOfControlSurface(size_t aHandle) const;

   // Returns a list(vector) containing the names of the control surfaces
   std::vector<std::string> GetListOfControlSurfaceNames() const;

   // Returns the position (deg) of the ailerons (common position)
   double GetAileronsBothPosition() const;

   // Returns the position (deg) of the left stabilator
   double GetStabilatorLeftPosition() const;

   // Returns the position (deg) of the right stabilator
   double GetStabilatorRightPosition() const;

   // Returns the position (deg) of the rudder(s) (common position)
   double GetRudderPosition() const;

   // Returns the position (deg) of the landing gear (common position)
   double GetLandingGearPosition() const;

   // Returns the position (rad) of the speed brakes (common position)
   double GetSpeedBrakePosition_rad() const;

   // Returns the position (deg) of the speed brakes (common position)
   double GetSpeedBrakePosition() const override;

   // Returns the position (deg) of the flaps (common position)
   double GetFlapsPosition() const override;

   // Returns the position (deg) of the spoilers (common position)
   double GetSpoilersPosition() const override;

   // Returns the position (deg) of the nose wheel steering angle
   double GetNoseWheelAngle() const;

   // These functions return the control surface angle in degrees
   double GetAngleAileronLeft() const;
   double GetAngleAileronRight() const;
   double GetAngleFlapLeft() const;
   double GetAngleFlapRight() const;
   double GetAngleSpoilerLeft() const;
   double GetAngleSpoilerRight() const;
   double GetAngleStabilizerLeft() const;
   double GetAngleStabilizerRight() const;
   double GetAngleElevator() const;
   double GetAngleRudderLeft() const;
   double GetAngleRudder() const;
   double GetAngleRudderRight() const;
   double GetAngleSpeedBrake() const;
   double GetAngleLandingGear() const;
   double GetAngleLandingGearNose() const;
   double GetAngleLandingGearMainLeft() const;
   double GetAngleLandingGearMainRight() const;

   // These functions return the normalized control surface angle
   double GetNormalizedAileronLeft() const override;
   double GetNormalizedAileronRight() const override;
   double GetNormalizedFlapLeft() const override;
   double GetNormalizedFlapRight() const override;
   double GetNormalizedSpoilerLeft() const override;
   double GetNormalizedSpoilerRight() const override;
   double GetNormalizedStabilizerLeft() const override;
   double GetNormalizedStabilizerRight() const override;
   double GetNormalizedElevator() const override;
   double GetNormalizedRudderLeft() const override;
   double GetNormalizedRudder() const override;
   double GetNormalizedRudderRight() const override;
   double GetNormalizedSpeedBrake() const override;
   double GetNormalizedLandingGear() const override;
   double GetNormalizedLandingGearNose() const override;
   double GetNormalizedLandingGearMainLeft() const override;
   double GetNormalizedLandingGearMainRight() const override;

   // These functions return true if the data for that control surface is valid
   bool AileronLeftValid() const override;
   bool AileronRightValid() const override;
   bool FlapLeftValid() const override;
   bool FlapRightValid() const override;
   bool SpoilerLeftValid() const override;
   bool SpoilerRightValid() const override;
   bool StabilizerLeftValid() const override;
   bool StabilizerRightValid() const override;
   bool ElevatorValid() const override;
   bool RudderLeftValid() const override;
   bool RudderValid() const override;
   bool RudderRightValid() const override;
   bool SpeedBrakeValid() const override;
   bool LandingGearValid() const override;
   bool LandingGearNoseValid() const override;
   bool LandingGearMainLeftValid() const override;
   bool LandingGearMainRightValid() const override;
   bool ThrottleValid() const override;

   // ====================================================================================================

   // Functions to support scripting

   const char* GetScriptClassName() const override { return "WsfRigidBodySixDOF_Mover"; }

   // This allows an external force (such as a ground tug) to apply a force to the nose gear. The angle is measured as
   // zero being forward and positive angles being to the right.
   void ApplyExternalForce(double aForceMagnitude_lbs, double aAngle_deg);

   // This removes any external forces on the nose gear.
   void RemoveExternalForce();

   // This should be called to enable direct braking commands in script.
   void EnableDirectBraking();

   // This sets the parking brake, if equipped.
   void SetParkingBrake();

   // This applies the left main gear wheel brake with a normalized value or 0 for no braking and 1.0 for full braking.
   void ApplyLeftGearBrake(double aValue);

   // This applies the right main gear wheel brake with a normalized value or 0 for no braking and 1.0 for full braking.
   void ApplyRightGearBrake(double aValue);

   // This releases both wheel brakes.
   void ReleaseWheelBrakes();

   // This should be called to disable any further direct braking commands in script.
   void ReleaseDirectBraking();

   // This retracts the landing gear
   void RetractLandingGear();

   // This lowers the landing gear
   void LowerLandingGear();

   // Set the flaps position in normalized values ranging from 0 when flaps are retractedto 1 when flaps are
   // fully deployed.
   void SetFlapsPosition(double aPosition) override;

   // Set the spoilers position in normalized values ranging from 0 when spoilers are retracted to 1 when spoilers are
   // fully deployed.
   void SetSpoilersPosition(double aPosition) override;

   void SetThrustReverserPosition(double aPosition);
   void SetEnableNWS(bool aNWS_Enabled);

   // Returns true if there is weight on any wheel
   bool GetWeightOnWheels() const override;

   // Returns true if there is weight on the nose wheel
   bool GetWeightOnNoseWheel() const;

   // Limits and settings functions

   // This returns the maximum ground taxi speed in ft/sec
   double GetTaxiSpeedMax();

   // This returns the maximum ground taxi yaw rate in deg/sec
   double GetTaxiYawRateMax();

   // This sets the maximum ground taxi speed in ft/sec
   void SetTaxiSpeedMax(double aSpeedMax);

   // This sets the maximum ground taxi yaw rate in deg/sec
   void SetTaxiYawRateMax(double aSpeedMax);

   // Autopilot commands

   // This enable/disables taxi mode
   void SetTaxiMode(bool aTaxiModeEnabled);

   // This sets the turn radius (in feet) used for taxiing
   void SetTaxiRadius(double aTaxiRadius_ft);

   // This returns a pointer to the test object. It should only be used in testing.
   RigidBodyMoverTestObject* GetRigidBodyTestSupportObject() const { return mTestSupportObjectPtr.get(); }
   MoverTestObject*          GetTestSupportObject() const override { return mTestSupportObjectPtr.get(); }

   // ====================================================================================================

   // Initialization functions

   void DeriveFrom(const RigidBodyMover& aSrc);

   // Note:  RigidBodySixDOF_VehicleType uses ProcessInput(), but RigidBodySixDOF_Vehicle does not

   // ====================================================================================================
   // Mass properties functions

   // This calculates the current mass properties, including those from subobjects and other contributors such as fuel.
   void CalculateCurrentMassProperties() override;

   // This is used to modify the weight of a vehicle without adding fuel or subobjects for testing
   // and should only be called in testing. Additional weight is added at the existing vehicle CG.
   void SetCurrentWeight_lbs(double aWeight_lbs);

   // This is used to modify the CG of a vehicle without adding fuel or subobjects for testing and should only be called
   // through the test object. CG is shifted in X, Y, and/or Z with no modification to scalar mass or moments of inertia.
   void ShiftCurrentCg_ft(UtVec3dX aDeltaCg_ft);

   // ====================================================================================================

   // Integrator functions

   // This returns the integrator.
   const Integrator* GetIntegrator() const override;

   // ====================================================================================================

   // "Last" functions

   // This sets the mLastSimTime_nanosec to the specified sim time in seconds. If the
   // aIncludeSubobjects flag is true, all subobjects will also have their last time
   // values set.
   void SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects = false) override;

   // ====================================================================================================================

   // Aerodynamics functions

   // This function is used to calculate the lift, drag, side force, and moments using the current state. The reference
   // point is typically (0,0,0), but may be a non-origin reference due to subobjects. This function will include aero
   // contributions by subobjects, if appropriate.
   void CalculateAeroBodyForceAndMoments(UtVec3dX& aLiftBodyVector_lbs,
                                         UtVec3dX& aDragBodyVector_lbs,
                                         UtVec3dX& aSideForceBodyVector_lbs,
                                         UtVec3dX& aMoment_ftlbs,
                                         UtVec3dX& aReferencePt_ft) override;

   // This function is used to calculate the lift, drag, side force, and otherdata for lower-fidelity vehicles using the
   // current state. This function will include aero contributions by subobjects, if appropriate.
   void CalculateAeroBodyForceAndRotation(UtVec3dX& aLiftBodyVector_lbs,
                                          UtVec3dX& aDragBodyVector_lbs,
                                          UtVec3dX& aSideForceBodyVector_lbs,
                                          UtVec3dX& aRotationAccelerationLimits_rps2,
                                          UtVec3dX& aStabilizingFrequency_rps) override;

   // This uses CalculatePitchGLoadAtSpecifiedAlpha internally to calculate the angle of attack (alpha) at the specified
   // g-load using the current conditions. If it cannot attain the specified g-load, it will return the *best* alpha.
   void CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg) const override;

   // This calculates the normalized stick back position to produce a net zero moment using the current conditions. If
   // it cannot produce a zero moment, it will do the best it can (full stick control).
   void CalculateStickBackForZeroMoment(double aAlpha_deg, double& aStickBack);

   // This calculates the normalized stick back position to produce a net zero moment using the specified Mach. If it
   // cannot produce a zero moment, it will do the best it can (full stick control).
   void CalculateStickBackForZeroMoment(double aAlpha_deg, double aMach, double& aStickBack);

   // This calculates the angle of sideslip (beta) at the specified g-load using the current conditions. If it cannot
   // attain the specified g-load, it will return the *best* beta.
   void CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg) const override;

   // This calculates the lift coefficient (CL), drag coefficient (Cd), and pitching moment coefficient (Cm),
   // multiplied by the appropriate reference area, produced by all aero movable objects. It uses the specified
   // stick back position.
   void CalculateControlsAeroCoefficientVectors(double  aMach,
                                                double  aAlpha_rad,
                                                double  aStickBack,
                                                double& aCLArea,
                                                double& aCdArea,
                                                double& aCmArea);

   // This calculates the lift coefficient (CL), multiplied by the appropriate reference area, produced by all aero
   // movable objects. It uses the specified stick back position.
   double CalculateControlsCLArea(double aMach, double aAlpha_rad, double aStickBack);

   // This calculates the drag coefficient (Cd), multiplied by the appropriate reference area, produced by all aero
   // movable objects. It uses the specified stick back position.
   double CalculateControlsCdArea(double aMach, double aAlpha_rad, double aStickBack);

   // This calculate the CmArea, including reference area effects, for the core aero and movable (controls) aero.
   void CalculateCmAreaForCoreAndControls(double  aMach,
                                          double  aAlpha_rad,
                                          double  aStickBack,
                                          bool    aPowerOn,
                                          double& aThrustProduced_lbs,
                                          double& aCoreCm,
                                          double& aControlsCm);

   // This calculate the CLArea, including reference area effects, for the core aero and movable (controls) aero.
   void CalculateCLAreaForCoreAndControls(double aMach, double aAlpha_rad, double aStickBack, double& aCoreCL, double& aControlsCL);

   // This calculate the CdArea, including reference area effects, for the core aero and movable (controls) aero.
   void CalculateCdAreaForCoreAndControls(double aMach, double aAlpha_rad, double aStickBack, double& aCoreCd, double& aControlsCd);

   // This calculates the total Cm and CL for the vehicle, including both core aero and movable (controls) aero,
   // returning the Cm and CL via reference.
   void CalculateTotalCmAndCL(double aMach, double aAlpha_rad, double aStickBack, double& aTotalCm, double& aTotalCL);

   // This sets the active aero object using the pointer
   void SetActiveAeroObject(RigidBodyAeroCoreObject* aObject);

   // This sets the active aero object using the name
   void SetActiveAeroObject(const std::string& aName) override;

   // This returns a pointer to the active aero object
   RigidBodyAeroCoreObject* GetActiveAeroObject() const { return mActiveAeroObjPtr; }

   // This returns the name of the active aero object
   std::string GetActiveAeroObjectName() const
   {
      return GetActiveAeroObject() ? GetActiveAeroObject()->GetModeName() : "";
   }

   // This returns the wing area (sq-ft) for the current aero mode
   double GetWingArea_sqft() const;

   // This returns the ref area (sq-ft) for the current aero mode
   double GetRefArea_sqft() const;

   // This returns the current center of gravity in feet from the reference point
   UtVec3dX GetCenterOfGravity_ft() const override { return mMassProperties.GetCmPosRelToRef_ft(); }

   // This uses the current state to calculate the maximum g-load maneuverability based on the current mass, altitude,
   // speed/mach and CLmax
   double MaxPotentialManeuverGLoad() const override;

   // This returns the fixed-aero object or null if none exists
   AeroCoreObject* GetAeroCoreObject() const override { return mAeroCoreObjPtr; }

   // ====================================================================================================

   // PropulsionSystem functions

   // This calculates F&M for the full propulsion system (all engines) plus the thrust from any subobjects
   void CalculatePropulsionFM(int64_t               aSimTime_nanosec,
                              double                aDeltaT_sec,
                              const KinematicState& aState,
                              UtVec3dX&             aInertialPropulsionVector_lbs,
                              UtVec3dX&             aPropulsionMoment_ftlbs);

   // This returns the propulsion system or null if no propulsion system exists
   PropulsionSystem* GetPropulsionSystem() const override { return mPropulsionObjPtr; }

   // This returns the propulsion system or null if no propulsion system exists
   RigidBodyPropulsionSystem* GetRigidBodyPropulsionSystem() { return mPropulsionObjPtr; }

   // This provides the maximum potential thrust available, if full throttle, including afterburner (if available),
   // is applied
   double GetMaximumPotentialThrust_lbs() const override;

   // This provides the minimum potential thrust available
   double GetMinimumPotentialThrust_lbs() const override;

   // This provides the current thrust magnitude produced by engines on the vehicle. It includes thrust from any
   // first-level/immediate subobjects
   double GetCurrentThrust_lbs() override;

   // This provides the current forward thrust produced by engines on the vehicle. It includes thrust from any
   // first-level/immediate subobjects
   double GetCurrentForwardThrust_lbs() override;

   // This returns the total fuel flow (lbs/hr) for all engines on the vehicle
   double GetTotalVehicleFuelFlow_pph() const override;

   // This returns the total fuel flow (lbs/hr) for the main engines on the vehicle. It does not include any subobjects.
   double GetTotalFuelFlow_pph() const override;

   // ====================================================================================================

   // PilotObject and FlightControls functions

   // This returns the pilot manager object. Null is returned when no pilot manager is present.
   PilotManager*          GetPilotManager() const override { return mPilotManagerPtr; }
   RigidBodyPilotManager* GetRigidBodyPilotManager() const { return mPilotManagerPtr; }

   // This returns the flight controls object. Null is returned when
   // no flight controls are present.
   RigidBodyFlightControlSystem* GetFlightControls() const { return mFlightControlsPtr; }

   // This returns true if the object includes flaps control surfaces
   bool HasFlaps() const;

   size_t GetInputHandle(const std::string& aInputName) const override;
   bool   GetBooleanInput(size_t aHandle) const override;
   bool   GetLastBooleanInput(size_t aHandle) const override;

   // This is used to restore angles specified in the list to the control surfaces specified in the other list
   void RestoreControlSurfaceAngles(std::vector<RigidBodyFlightControlSystem::ControlSurfaceElement*>& aControlsList,
                                    std::vector<double>&                                               aAngleList);

   // If true, a simple yaw damper should be used. This is determined by the autopilot, so if no autopilot is present,
   // no yaw damper will be available. This "simple" yaw damper merely prevents any rotation about the body z-axis,
   // so it may not be desired, since it is simplified and not very realistic.
   bool UseSimpleYawDamper() const;

   // These functions return the normalized control input (lever) value
   double GetStickRightInput() const override;
   double GetStickBackInput() const override;
   double GetRudderRightInput() const override;
   double GetThrottleInput() const override;
   double GetSpeedBrakeInput() const override;
   double GetSpoilerInput() const override;
   double GetLandingGearInput() const override;

   // ====================================================================================================

   // Landing gear functions

   void CalculateLandingGearFM(int64_t         aSimTime_nanosec,
                               const UtVec3dX& aNonLandingGearTotalForceVecInertial_lbs,
                               UtVec3dX&       aInertialTotalGearVector_lbs,
                               UtVec3dX&       aTotalGearMoment_ftlbs);

   // Use this to set landing gear position at start of simulation
   void SetLandingGearPositionAtStart(double aPosition);

   // This will force the gear to the specified position instantly. The typical name is "Landing_Gear_Extended", but it
   // may use a different name.
   void ForceLandingGearPositionInstantly(std::string aLandingGearControlSurfaceHandleName, double aPosition);

   // This will force the gear up instantly. The typical name is "Landing_Gear_Extended", but it may use a different name.
   void ForceLandingGearUpInstantly(const std::string& aLandingGearControlSurfaceHandleName);

   // This will force the gear down instantly. The typical name is "Landing_Gear_Extended", but it may use a different name.
   void ForceLandingGearDownInstantly(const std::string& aLandingGearControlSurfaceHandleName);

   // Returns true if the object has landing gear
   bool HasLandingGear() const { return (mLandingGearPtr != nullptr); }

   // This returns the landing gear object. Null is returned when no landing gear are present.
   RigidBodyLandingGear* GetLandingGear() const { return mLandingGearPtr; }

   // ====================================================================================================

   // Testing functions

   // This sets kinematic state parameters to a zero condition for testing
   void ZeroKinematicsDuringTesting() override;

protected:
   explicit RigidBodyMover(const RigidBodyMover& aSrc);

   // This is used by all Initialize functions to complete "details"
   bool InitializeDetails(double aSimTime_sec) override;

   // Update functions
   void UpdateFlightControls(int64_t aSimTime_nanosec) override;

   // This returns the pitching moment (ft-lbs) for the specified angle of attack (alpha) and stick back at the current
   // conditions. Note -- this assumes that "testing" controls are being used.
   double CalculatePitchMomentAlphaStickBack_ftlbs(double aAlpha_deg, double aDesiredStickBack);

   void CalculateMovableAero(double          aDynPress_lbsqft,
                             double          aMach,
                             double          aSpeed_fps,
                             double          aAlpha_rad,
                             double          aBeta_rad,
                             const UtVec3dX& aAngularRates_rps,
                             UtVec3dX&       aAeroMoment,
                             double&         aLift,
                             double&         aDrag,
                             double&         aSide,
                             double&         aLiftFactor);

   void CalculateThrustVectorAndMomentUpdatePropulsion(double                 aSimTime_sec,
                                                       double                 aDeltaTime_sec,
                                                       const KinematicState&  aState,
                                                       ForceAndMomentsObject& aForceAndMoment);

   void UpdatePropulsionFuelBurn(double aSimTime_sec, double aDeltaTime_sec, const KinematicState& aState) override;

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

   void CalculateObjectPropulsion(double                  aSimTime_sec,
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
                                  double&                 aFuelBurned_lbs) override;

   void UpdateObjectPropulsion(double                 aSimTime_sec,
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
                               double&                aFuelBurned_lbs);

   void ObjectPropulsionCalculations(double                 aSimTime_sec,
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
                                     bool                   aUpdateData);

   void CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
                                     double& aCurrentTotalFuel_lbs,
                                     double& aMaxTotalFuel_lbs) override;

   // Returns the angle (deg) of the surface with the specified handle
   double GetAngleSurface(size_t aHandle) const;

   // Returns the normalized angle of the surface with the specified handle
   double GetNormalizedSurfaceAngle(size_t aHandle) const;

   // Find the various parts -- landing gear, control surfaces, engines, etc.
   void FindParts();

   // ====================================================================================================

   std::unique_ptr<RigidBodyMoverTestObject> mTestSupportObjectPtr{nullptr};

   //// Components

   ut::CloneablePtr<RigidBodyAeroCoreObject>               mAeroCoreObjPtr{nullptr};
   std::list<ut::CloneablePtr<RigidBodyAeroMovableObject>> mAeroPartList;

   RigidBodyAeroCoreObject* mActiveAeroObjPtr = nullptr;

   ut::CloneablePtr<RigidBodyPropulsionSystem> mPropulsionObjPtr{nullptr};

   ut::CloneablePtr<RigidBodyIntegrator> mIntegratorPtr{nullptr};

   ut::CloneablePtr<RigidBodyFlightControlSystem> mFlightControlsPtr{nullptr};
   ut::CloneablePtr<RigidBodyPilotManager>        mPilotManagerPtr{nullptr};
   ut::CloneablePtr<RigidBodyLandingGear>         mLandingGearPtr{nullptr};

   bool mLandingGearDownAtStart = false;
   bool mParkingBrakeOnAtStart  = false;
   bool mTaxiModeEnabledAtStart = false;
   bool mNWSEnabledAtStart      = false;

   double mLastLandingGearLeverPosition       = 0.0;
   double mLastThrustReverserLeverPosition    = 0.0;
   double mLastThrustVectorYawRightPosition   = 0.0;
   double mLastThrustVectorPitchUpPosition    = 0.0;
   double mLastThrustVectorRollRightPosition  = 0.0;
   double mLastNoseWheelSteeringRightPosition = 0.0;
   double mLastWheelBrakeLeftPosition         = 0.0;
   double mLastWheelBrakeRightPosition        = 0.0;
   bool   mLastNWS_Enabled                    = false;

   size_t mHandleAileronLeft          = 0;
   size_t mHandleAileronRight         = 0;
   size_t mHandleFlapLeft             = 0;
   size_t mHandleFlapRight            = 0;
   size_t mHandleSpoilerLeft          = 0;
   size_t mHandleSpoilerRight         = 0;
   size_t mHandleStabilizerLeft       = 0;
   size_t mHandleStabilizerRight      = 0;
   size_t mHandleElevator             = 0;
   size_t mHandleRudderLeft           = 0;
   size_t mHandleRudder               = 0;
   size_t mHandleRudderRight          = 0;
   size_t mHandleSpeedBrake           = 0;
   size_t mHandleLandingGear          = 0;
   size_t mHandleLandingGearNose      = 0;
   size_t mHandleLandingGearMainLeft  = 0;
   size_t mHandleLandingGearMainRight = 0;
};
} // namespace six_dof
} // namespace wsf

#endif
