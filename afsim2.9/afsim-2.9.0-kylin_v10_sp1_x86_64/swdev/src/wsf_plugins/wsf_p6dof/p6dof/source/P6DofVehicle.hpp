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

#ifndef P6DOFVEHICLE_H
#define P6DOFVEHICLE_H

#include "p6dof_export.h"

#include <cstdint>
#include <list>
#include <vector>

#include "P6DofAeroCoreObject.hpp"
#include "P6DofAeroMovableObject.hpp"
#include "P6DofFlightControlSystem.hpp"
#include "P6DofGraphicalSupport.hpp"
#include "P6DofIntegrator.hpp"
#include "P6DofKinematicState.hpp"
#include "P6DofMassProperties.hpp"
#include "P6DofObject.hpp"
#include "P6DofSizeFactor.hpp"
#include "P6DofUtils.hpp"
#include "P6DofVehicleData.hpp"
#include "UtCallback.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtQuaternion.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
class P6DofFlightControlSystem;
class P6DofForceAndMomentsObject;
class P6DofFuelTank;
#include "P6DofLandingGear.hpp"
class P6DofObject;
#include "P6DofPilotManager.hpp"
#include "P6DofPropulsionSystem.hpp"
class P6DofScenario;
#include "P6DofSequencer.hpp"
class P6DofThrustProducerObject;
class P6DofTypeManager;
class P6DofVehicleType;

class P6DOF_EXPORT P6DofVehicle : public P6DofObject
{
public:
   explicit P6DofVehicle(P6DofScenario* aScenario);

   ~P6DofVehicle() override = default;

   P6DofVehicle& operator=(const P6DofVehicle& other) = delete;

   P6DofVehicle* Clone() const;

   void SetParentVehicle(P6DofVehicle* aParentVehicle) override;

   // ====================================================================================================================

   // Initialization functions

   bool Initialize(double aSimTime_sec, const P6DofVehicleType* aObjectType, const std::string& aPrefixName);

   bool InitializeConditions(double aSimTime_sec);

   // Note:  P6DofVehicleType uses ProcessInput(), but P6DofVehicle does not

   // This "washes-in" (smooths) initial conditions, helping to eliminate "jumps"
   // as the vehicle reaches a stable state.
   void SetWashInInitConditions(bool aUseWashInInitConditions) { mUseWashInInitConditions = aUseWashInInitConditions; }

   // ====================================================================================================================

   // Update functions

   void Update(double aSimTime_sec);
   void Update(int64_t aSimTime_nanosec);
   void UpdateObject(int64_t aSimTime_nanosec);

   // ====================================================================================================================

   // Misc object functions

   // This returns true if the object has crashed
   bool Crashed() const { return mCrashed; }

   // This enables/disables the size factor. Size factor effects aerodynamics as
   // a size multiplier for reference area.
   void SetSizeFactorEnabled(bool aSetting) { mSizeFactor.SetSizeFactorEnabled(aSetting); }

   // ====================================================================================================================

   // Mass properties functions

   // This returns the mass properties.
   const P6DofMassProperties& GetMassProperties() const { return mMassProperties; }

   // This calculates the current mass properties, including those from subobjects and other contributors such as fuel.
   void CalculateCurrentMassProperties();

   double GetEmptyWeight_lbs() const { return mMassProperties.GetBaseMass_lbs(); }
   double GetCurrentWeight_lbs() const { return mMassProperties.GetMass_lbs(); }

   // This is used to modify the weight of a vehicle without adding fuel or subobjects for testing and should only be
   // called in testing. Additional weight is added at the existing vehicle CG.
   void SetCurrentWeight_lbs(double aWeight_lbs);

   // This is used to modify the CG of a vehicle without adding fuel or subobjects for testing and should only be called
   // through the test object. CG is shifted in X, Y, and/or Z with no modification to scalar mass or moments of inertia.
   void ShiftCurrentCg_ft(UtVec3dX aDeltaCg_ft);

   // ====================================================================================================================

   // Kinematics state functions

   // This returns the kinematics state.
   P6DofKinematicState& GetKinematicState() { return mKinematicState; }

   // ====================================================================================================================

   // Integrator functions

   // This returns the integrator.
   const P6DofIntegrator* GetIntegratorPtr() const { return mIntegratorPtr; }

   // ====================================================================================================================

   // Naming functions

   // This gets the base name
   std::string GetBaseName() const { return mBaseName; }

   // This gets the derived from object name
   std::string GetDerivedFromObjectName() const { return mDerivedFromObjectName; }

   // This is called to (possibly) add a prefix to the name of the object instance
   void SetNamePrefix(const std::string& aPrefixName);

   // This sets the basename
   void SetBaseName(const std::string& aName) { mBaseName = aName; }

   // ====================================================================================================================

   // Timing functions

   // This returns the last update time (in seconds) of the object
   double GetLastUpdateTime_sec() const { return P6DofUtils::TimeToTime(mLastSimTime_nanosec); }

   // This returns the total lifetime (in seconds) of the object, including time when the object was free or captive
   // to a parent
   double GetLifeTime_sec() const { return P6DofUtils::TimeToTime(mLifeTime_nanosec); }

   // This returns the time of flight (in seconds) of the object, when it was free from being captive to a parent
   double GetTimeOfFlight_sec() const { return P6DofUtils::TimeToTime(mTimeOfFlight_nanosec); }

   // This returns the total lifetime (in integer nanoseconds) of the object, including time when the object was free
   // or captive to a parent
   int64_t GetLifeTime_nanosec() const { return mLifeTime_nanosec; }

   // This returns the time of flight (in integer nanoseconds) of the object, when it was free from being captive to
   // a parent
   int64_t GetTimeOfFlight_nanosec() const { return mTimeOfFlight_nanosec; }

   // ====================================================================================================================

   // Position/location

   // This provides the lat/lon where the object was created
   void GetStartingLatLon(double& aStartingLat, double& aStartingLon) const;

   // This returns the current latitude in degrees
   double GetLat() const;

   // This returns the current longitude in degrees
   double GetLon() const;

   // This provides the current lat/lon (in degrees) and altitude (in meters)
   void GetLocationLLA(double& aLat, double& aLon, double& aAlt_m);

   // This returns the current altitude (in meters)
   double GetAlt_m() const;

   // This returns the current altitude (in feet above sea level)
   double GetAltitudeMSL_ft() const;

   // This returns the maximum altitude (in kilometers) reached by the object
   double GetApogee_km() const;

   // This returns the current great-circle range (in kilometers) from the starting location to the current location
   double GetRangeSinceStart_km() const;

   // This returns a vector for the current location in WCS
   UtVec3dX GetLocationWCS() const;

   // This provides x,y,z values for the current location in WCS
   void GetLocationWCS(double& aWcsPosX_m, double& aWcsPosY_m, double& aWcsPosZ_m) const;

   // This provides x,y,z values for the current location in spherical-earth coordinates
   void GetLocationSpherical(double& aSphericalPosX_m, double& aSphericalPosY_m, double& aSphericalPosZ_m) const;

   // This sets the location to the lat, lon, altitude specified
   void SetLocationLLA(double aLat, double aLon, double aAlt_m);

   // This sets the location to the x, y, z specified in WCS
   void SetLocationWCS(double aWcsPosX_m, double aWcsPosY_m, double aWcsPosZ_m);

   // This is used to move to the preposition location (starting location) at the specified altitude in meters.
   // This is often used in testing.
   void SetPrepositionAlt_m(double aAltitude_m);

   // The returns true is a spherical-earth model is being used instead of a WGS84 oblate-earth model (WCS).
   bool UsingSphericalEarth() const;

   // ====================================================================================================================

   // Velocity functions

   // This returns the indicated/calibrated airspeed in knots, based on the current altitude
   double GetKCAS() const;
   double GetKIAS() const;

   // This returns the true airspeed in knots
   double GetKTAS() const;

   // This returns the Mach, based on the current speed and altitude
   double GetMach() const;

   // This returns the vertical speed in ft/minutes
   double GetVertSpeed_fpm() const;

   // This returns the true speed (inertial, not airspeed) in ft/sec
   double GetSpeed_fps() const;

   // This provides the vx, vy, vz WCS velocity components in meters/sec
   void GetVelocityWCS(double& aWcsVelX_mps, double& aWcsVelY_mps, double& aWcsVelZ_mps) const;

   // This returns a vector of the WCS velocity in meters/sec
   UtVec3dX GetVelocityWCS() const;

   // This returns a vector of the NED velocity in meters/sec
   UtVec3dX GetVelocityNED_mps() const;

   // This sets the velocity using the vx, vy, vz WCS velocity components in meters/sec
   void SetVelocityWCS(double aWcsVelX_mps, double aWcsVelY_mps, double aWcsVelZ_mps);

   // This sets the velocity using the vx, vy, vz spherical-earth velocity components in meters/sec
   void SetVelocitySpherical(double aSphericalX_mps, double aSphericalY_mps, double aSphericalZ_mps);

   // This sets the velocity using the NED velocity components in meters/sec
   void SetVelocityNED(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps);

   // ====================================================================================================================

   // Acceleration functions

   // This returns the x-component of acceleration (in gees) in body coordinates
   double GetNx_g() const;

   // This returns the y-component of acceleration (in gees) in body coordinates
   double GetNy_g() const;

   // This returns the z-component of acceleration (in gees) in body coordinates
   double GetNz_g() const;

   // This returns a vector of the acceleration (in meters/sec^2) in NED coordinates
   UtVec3dX GetAccelNED_mps2() const;

   // ====================================================================================================================

   // Attitude functions

   // This returns the current heading in radians
   double GetHeading_rad() const;

   // This returns the current heading in degrees
   double GetHeading_deg() const;

   // This returns the current pitch in degrees
   double GetPitch_rad() const;

   // This returns the current pitch in degrees
   double GetPitch_deg() const;

   // This returns the current roll in degrees
   double GetRoll_rad() const;

   // This returns the current roll in degrees
   double GetRoll_deg() const;

   // This returns the current yaw rate in degrees/sec
   double GetYawRate_dps() const;

   // This returns the current pitch rate in degrees/sec
   double GetPitchRate_dps() const;

   // This returns the current roll rate in degrees/sec
   double GetRollRate_dps() const;

   // This returns the current alpha (angle of attack) in radians
   double GetAlpha_rad() const;

   // This returns the current alpha (angle of attack) in degrees
   double GetAlpha_deg() const;

   // This returns the current beta (angle of sideslip) in radians
   double GetBeta_rad() const;

   // This returns the current beta (angle of sideslip) in degrees
   double GetBeta_deg() const;

   // This returns the current angle of attack rate (alpha-dot) in radians/sec
   double GetAlphaDot_rps() const;

   // This returns the current angle of attack rate (alpha-dot) in degrees/sec
   double GetAlphaDot_dps() const;

   // This returns the current angle of sideslip rate (beta-dot) in radians/sec
   double GetBetaDot_rps() const;

   // This returns the current angle of sideslip rate (beta-dot) in degrees/sec
   double GetBetaDot_dps() const;

   // This returns the current center of gravity in meters
   UtVec3dX GetCg() const;

   // This returns the current gamma (flight path angle) in radians
   double GetFlightPathAngle_rad() const;

   // This provides the current WCS yaw, pitch, and roll in radians
   void GetOrientationWCS(double& aWcsYaw_rad, double& aWcsPitch_rad, double& aWcsRoll_rad) const;

   // This provides the current NED heading, pitch, and roll in radians
   void GetAttitudeNED(double& aHeading, double& aPitch, double& aRoll);

   // This sets the WCS yaw, pitch, and roll in radians
   void SetOrientationWCS(double aWcsYaw_rad, double aWcsPitch_rad, double aWcsRoll_rad);

   // This set the NED heading, pitch, and roll in radians
   void SetAttitudeNED(double aHeading_rad, double aPitch_rad, double aRoll_rad);

   // This provides the current body rates about each axis in radians/sec
   void GetBodyRates_rps(double& xAxis_rps, double& yAxis_rps, double& zAxis_rps) const;

   // Zeros body rates, used for autopilot tuning
   void ZeroBodyRates();

   // This provides the current body angular acceleration about each axis in radians/sec^2
   void GetBodyAngularAccel_rps2(double& xAxis_rps2, double& yAxis_rps2, double& zAxis_rps2) const;

   // This returns true if the positive-side stall warning is occurring
   bool StallWarningAlert() const;

   // ====================================================================================================================

   // "Last" functions

   // This sets the mLastSimTime_nanosec to the specified sim time in seconds. If the aIncludeSubobjects flag is true,
   // all subobjects will also have their last time values set.
   void SetLastSimTime(double aSimTime_sec, bool aIncludeSubobjects = false);

   // ====================================================================================================================

   // Graphical Support interface functions

   // This group of functions provide support for special P6DOF graphical interfaces.

   // This returns the "graphical" object type.
   unsigned short GetObjectGraphicalType() const;

   // This sets mCurrentObjectGraphicalType based on the aAltType argument:
   //   aAltType = 0 --> Sets type to mObjectGraphicalType (default type)
   //   aAltType = 1 --> Sets type to mObjectGraphicalAltType1 (alt type 1)
   //   aAltType = 2 --> Sets type to mObjectGraphicalAltType1 (alt type 2)
   //   aAltType = 3 --> Sets type to mObjectGraphicalAltType1 (alt type 3)
   void SetObjectGraphicalType(int aAltType);

   // TODO - Refactor this to avoid using "targets" in P6DOF
   // This allows up to ten tracks to be "injected".
   void SetPrimaryTargetData(const P6DOF::CurrentTargetData& aCurrentTarget,
                             int                             aNumTracks,
                             const P6DOF::TrackData          aTrackData[10]);

   // ====================================================================================================

   // Damage and Destruction

   // This function is called by WsfP6DOF_Mover when the mover is destroyed in flight. The aPitchInputModifier and
   // aRollInputModifier arguments vary between +/-1 and control the pitch and roll control when destroyed. The
   // aIsOnFire, aEngineIsSmoking, and aIsTrailingSmoke arguments control the appearance when destroyed.
   void SetDestroyed(double aPitchInputModifier,
                     double aRollInputModifier,
                     bool   aIsOnFire,
                     bool   aEngineIsSmoking,
                     bool   aIsTrailingSmoke);

   bool GetIsDestroyed();

   // ====================================================================================================================

   // Appearance functions

   // This group of functions provide support for appearance attributes, which are often used by visualization tools and
   // networked simulation, such as DIS. These functions are included in the P6DofVehicle class, since the vehicle is
   // best able to determine these appearances.

   void UpdateAppearance(){};

   // External appearance control. These functions allow the appearance to be set from an external source.
   bool AnEngineIsOperating() const;
   bool AnEngineIsSmoking() const;
   bool AnEngineHasAfterburnerOn() const;
   bool ContrailTrailingEffectPresent() const;
   bool RocketSmokeTrailingEffectPresent() const;
   bool DamageSmokeTrailingEffectPresent() const;
   bool FlamesArePresent() const;

   void MakeEnginesSmoke();
   void ActivateDamageSmokeTrailingEffect(bool aEffectActive);
   void ActivateFlamesEffect(bool aFlamesPresent);

   // This returns true if the specified altitude is within the contrailing band. If it is not within the band or
   // contrailing conditions do not exist, it returns false.
   bool WithinContrailAltitudeBand(double aAltitude_ft) const;

   // ====================================================================================================================

   // Aerodynamics functions

   // This function is used to calculate the lift, drag, side force, and moments using the current state. The reference
   // point is typically (0,0,0), but may be a non-origin reference due to subobjects. This function will include aero
   // contributions by subobjects, if appropriate.
   void CalculateAeroBodyFM(UtVec3dX& aLiftBodyVector_lbs,
                            UtVec3dX& aDragBodyVector_lbs,
                            UtVec3dX& aSideForceBodyVector_lbs,
                            UtVec3dX& aMoment_ftlbs,
                            UtVec3dX& aReferencePt_ft);

   // This uses CalculatePitchGLoadAtSpecifiedAlpha internally to calculate the angle of attack (alpha) at the specified
   // g-load using the current conditions. If it cannot attain the specified g-load, it will return the *best* alpha.
   void CalculateAlphaAtSpecifiedGLoad_deg(double aGLoad, double& aAlpha_deg);

   // This calculates the normalized stick back position to produce a net zero moment using the current conditions. If
   // it cannot produce a zero moment, it will do the best it can (full stick control).
   void CalculateStickBackForZeroMoment(double aAlpha_deg, double& aStickBack);

   // This calculates the normalized stick back position to produce a net zero moment using the specified Mach. If it
   // cannot produce a zero moment, it will do the best it can (full stick control).
   void CalculateStickBackForZeroMoment(double aAlpha_deg, double aMach, double& aStickBack);

   // This calculates the angle of sideslip (beta) at the specified g-load using the current conditions. If it cannot
   // attain the specified g-load, it will return the *best* beta.
   void CalculateBetaAtSpecifiedGLoad_deg(double aGLoad, double& aBeta_deg);

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
   void SetActiveAeroObject(P6DofAeroCoreObject* aObject);

   // This sets the active aero object using the name
   void SetActiveAeroObject(const std::string& aName);

   // This returns a pointer to the active aero object
   P6DofAeroCoreObject* GetActiveAeroObject() const { return mActiveAeroObjPtr; }

   // This returns the name of the active aero object
   std::string GetActiveAeroObjectName() const { return mActiveAeroModeName; }

   // This returns the wing area (sq-ft) for the current aero mode
   double GetWingArea_sqft() const;

   // This returns the ref area (sq-ft) for the current aero mode
   double GetRefArea_sqft() const;

   // This returns the lift in lbs
   double GetLift_lbs() const;

   // This returns the drag in lbs
   double GetDrag_lbs() const;

   // This returns the side force in lbs
   double GetSideForce_lbs() const;

   // This returns the thrust magnitude in lbs
   double GetThrustMagnitude_lbs() const;

   // Returns the yaw moment in ft*lbs
   double GetYawMoment_ftlbs() const;

   // Returns the pitch moment in ft*lbs
   double GetPitchMoment_ftlbs() const;

   // Returns the roll moment in ft*lbs
   double GetRollMoment_ftlbs() const;

   // This returns dynamic pressure in lbs/sq-ft
   double GetDynamicPressure_psf() const;

   // This returns pressure pressure in lbs/sq-ft
   double GetStaticPressure_psf() const;

   // This uses the current state to calculate the maximum g-load maneuverability based on the current mass, altitude,
   // speed/mach and CLmax
   double MaxPotentialManeuverGLoad() const;

   // ====================================================================================================================

   // Parent-object functions

   bool IsCaptive() const { return mIsCaptive; }
   void AdjustForParentInducedEffects();
   void ImpartSeparationEffects();
   void GetPositionRelativeToParent(UtVec3dX& aPosRelToParent_m, UtVec3dX& aAngRelToParentYPR_rad);

   void SetCaptiveState(bool aIsCaptive) { mIsCaptive = aIsCaptive; }

   void SetAngRelToParentYPR_rad(const UtVec3dX& aAngRelToParentYPR_rad)
   {
      mAngRelToParentYPR_rad = aAngRelToParentYPR_rad;
   }
   void SetYawRelToParent_rad(double aYawAngle_rad);
   void SetPitchRelToParent_rad(double aYawAngle_rad);
   void SetRollRelToParent_rad(double aYawAngle_rad);

   // ====================================================================================================================

   // Subobject functions

   // This adds the designated object as a subobject
   void AddSubObject(std::unique_ptr<P6DofVehicle> aSubObject);

   // This returns a pointer to the subobject with the specified name. It will search the direct subobjects and if no
   // match is found it will search children of subobjects.
   P6DofVehicle* GetSubobjectByName(const std::string& aSubobjectName) const;

   // This returns a reference to the list of subobjects
   const std::list<ut::CloneablePtr<P6DofVehicle>>& GetSubobjectList() const;

   size_t GetNumSubobjects() const { return mSubObjectList.size(); }

   bool JettisonSubobject(const std::string& aSubobjectName, int64_t aSimTime_nanosec);
   bool JettisonSubobject(P6DofVehicle* aSubObject, int64_t aSimTime_nanosec);

   // This will remove/jettison the specified subobject, transferring ownership to the caller if successful. Note that
   // this does not create a platform or any other action. It merely removes the subobject from the parent and removes
   // the captive flag.
   std::unique_ptr<P6DofVehicle> DirectJettisonOfSubobject(P6DofVehicle* aSubobject, double aSimTime_sec);

   // ====================================================================================================================

   // PropulsionSystem functions

   // This calculates F&M for the full propulsion system (all engines) plus the thrust from any subobjects
   void CalculatePropulsionFM(int64_t              aSimTime_nanosec,
                              double               aDeltaT_sec,
                              P6DofKinematicState& aState,
                              UtVec3dX&            aInertialPropulsionVector_lbs,
                              UtVec3dX&            aPropulsionMoment_ftlbs);

   // This updates the fuel/propellant consumption for propulsion system, but does not update burn for any subobjects,
   // which is handled by those objects
   void UpdateFuelBurn(int64_t aSimTime_nanosec, double aDeltaT_sec, P6DofKinematicState& aState);

   // This returns the propulsion system or null if no propulsion system exists
   P6DofPropulsionSystem* GetPropulsionSystem() const { return mPropulsionObjPtr; }

   // This returns true if afterburners are present
   bool HasAfterburner() const;

   // Returns the current fuel remaining (in percent)
   double GetRemainingFuelPercent() const;

   // This provides the maximum potential thrust available, if full throttle, including afterburner (if available),
   // is applied
   double GetMaximumPotentialThrust_lbs() const;

   // This provides the minimum potential thrust available
   double GetMinimumPotentialThrust_lbs() const;

   // This provides the current thrust magnitude produced by engines on the vehicle. It includes thrust from any
   // first-level/immediate subobjects
   double GetCurrentThrust_lbs();

   // This provides the current forward thrust produced by engines on the vehicle. It includes thrust from any
   // first-level/immediate subobjects
   double GetCurrentForwardThrust_lbs();

   // This provides the current normalized military-power thrust produced by engines on the vehicle. It includes
   // thrust from any first-level/immediate subobjects and may range from 0.0 to 1.0. Note -- This uses the first
   // engine on the vehicle (which may differ from other engines).
   double GetNormalizedMilitaryThrust() const;

   // This provides the current normalized afterburner thrust produced by engines on the vehicle. It includes
   // thrust from any first-level/immediate subobjects and may range from 0.0 to 1.0. Note -- This uses the first
   // engine on the vehicle (which may differ from other engines).
   double GetNormalizedAfterburnerThrust() const;

   // This provides the current normalized total thrust produced by engines on the vehicle. It includes thrust
   // from any first-level/immediate subobjects and may range from 0.0 to 2.0, where 0.0 is idle thrust, 1.0 is
   // full military power, and 2.0 is full afterburner. Note -- This uses the first engine on the vehicle (which
   // may differ from other engines).
   double GetNormalizedTotalThrust() const;

   // This returns the total fuel flow (lbs/hr) for all engines on the vehicle
   double GetTotalVehicleFuelFlow_pph() const;

   // This returns the total fuel flow (lbs/hr) for the main engines on the vehicle. It does not include any subobjects.
   double GetTotalFuelFlow_pph() const;

   // GetFuelTankByName returns the fuel source with the specified name
   P6DofFuelTank* GetFuelTankByName(const std::string& aName) const;

   // GetFuelTankByName returns the fuel source with the specified name
   P6DofFuelTank* GetFuelTankByName(const std::string& aName, const P6DofVehicle* aCallingObject, int aCallingLevel = 0) const;

   // This returns the maximum total capacity of fuel in tanks for the object and all of its subobjects. Note that
   // this is capacity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetTotalFuelTankCapacity_lbs() const;

   // This returns the current total amount of fuel in tanks for the object and all of its subobjects. Note that this
   // is quantity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets. This returns
   // the total amount of fuel in tanks for the object and all of its subobjects.
   double GetCurrentTotalFuelTankQuantity_lbs() const;

   // This returns the maximum total capacity of fuel in tanks for the object, but not including any of its subobjects.
   // Note that this is capacity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetInternalFuelTankCapacity_lbs() const;

   // This returns the current total amount of fuel in tanks for the object, but not including any of its subobjects.
   // Note that this is quantity of fuel in fuel tanks, and does not include integral fuel in solid propellant rockets.
   double GetCurrentInternalFuelTankQuantity_lbs() const;

   // This returns the maximum total capacity of fuel in external tanks for the object. It does this by summing the fuel
   // in the first level subobjects. Note that this is capacity of fuel in fuel tanks, and does not include integral
   // fuel in solid-propellant rockets.
   double GetExternalFuelTankCapacity_lbs() const;

   // This returns the current total amount of fuel in external tanks for the object. It does this by summing the fuel
   // in the first level subobjects. Note that this is quantity of fuel in fuel tanks, and does not include integral
   // fuel in solid-propellant rockets.
   double GetCurrentExternalFuelTankQuantity_lbs() const;

   // This adds fuel to the object in a simplistic manner. It does not use the "fill rate" limit of tanks -- hence all
   // of the fuel is put into the tank(s) at once. Later, we can add a "proper" fuel fill that is more realistic and
   // deals with a fill rate that will require a finite amount of time to transfer. This function will fill internal
   // tanks first and then any external/drop tanks and subobjects will be filled next. However, for simplicity, it
   // will make all non-internal tanks filled to the same percent full level, even if they already contained more
   // fuel -- in other words, it will redistribute non-internal fuel to keep the percent full consistent.
   void AddFuelQuantity_lbs(double aFuelToAdd_lbs);

   // This will set the fuel in a specified tank to the specified level, as long as the tank capacity can contain
   // it -- otherwise, the tank will be filled and the excess fuel will be "wasted". This function performs the
   // transfer instantly, regardless of the transfer rates supported by the tank and fuel system. It also ignores any
   // fuel that already may be in the tank.
   void SetFuelInTank_lbs(const std::string& aTankName, double aFuel_lbs);

   // This will instantly empty all internal tanks for the object
   void EmptyAllInternalTanks();

   // This will instantly empty all external tanks for the object
   void EmptyAllExternalTanks();

   // Returns the number of external fuel tanks (subobjects that can contain fuel) being carried.
   // Note -- This only checks the first level of subobjects.
   int GetNumberOfExternalTanks() const;

   // GetThrustProducerObjectByName returns the P6DofThrustProducerObject with the specified name
   P6DofThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName) const;

   // GetThrustProducerObjectByName returns the P6DofThrustProducerObject with the specified name
   P6DofThrustProducerObject* GetThrustProducerObjectByName(const std::string&  aName,
                                                            const P6DofVehicle* aCallingObject,
                                                            int                 aCallingLevel = 0) const;

   // Ignite all of the engines in the propulsion object
   void IgniteObject(int64_t aIgniteTimeInFrame_nanosec);

   // This terminates thrust from the propulsion object but not any subobjects aTerminateTime_sec is the time
   // termination occurs
   void ShutdownObject(int64_t aTerminateTime_nanosec = 0);

   // This terminates thrust from the propulsion object and all subobjects aTerminateTime_sec is the time
   // termination occurs
   void TerminateThrust(int64_t aTerminateTime_nanosec = 0);

   // This returns true is at least one thrust producer is producing thrust
   bool IsProducingThrust() const;

   // EnableThrustVectoring enables/disable thrust vectoring
   void EnableThrustVectoring(bool aEnableThrustVectoring);

   // This adds a new fuel transfer to the propulsion system. The fuel transfer name must be unique. It returns true
   // if the transfer is created.
   bool AddFuelTransfer(std::string aFuelTransferName, std::string aSourceTankName, std::string aTargetTankName);

   // This removes the fuel transfer with the specified name. It returns true if the removal is successful.
   bool RemoveFuelTransfer(std::string aFuelTransferName);

   // This specifies the fuel feed (input tank name) for the specified engine. It returns true if the assignment is
   // successful.
   bool SetFuelFeed(std::string aEngineName, std::string aFuelTankName);

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aFuelTankName);

   // ====================================================================================================================

   // PilotObject and FlightControls functions

   // This returns the pilot manager object. Null is returned when no pilot manager is present.
   P6DofPilotManager* GetPilotManager() const { return mPilotManagerPtr; }

   // This returns the pilot controller object. Null is returned when no pilot controller is present.
   P6DofPilotObject* GetPilotObject() const;

   // This makes the manual pilot active (if present)
   void MakeManualPilotActive();

   // This makes the manual pilot active (if present)
   void MakeSyntheticPilotActive();

   // This makes the manual pilot active (if present)
   void MakeHardwareAutopilotActive();

   // This makes the manual pilot active (if present)
   void MakeGuidanceAutopilotActive();

   // This returns the flight controls object. Null is returned when
   // no flight controls are present.
   P6DofFlightControlSystem* GetFlightControls() const { return mFlightControlsPtr; }

   // This returns true if the object includes flaps control surfaces
   bool HasFlaps() const;

   size_t GetInputHandle(const std::string& aInputName) const;
   bool   GetBooleanInput(size_t aHandle) const;
   bool   GetLastBooleanInput(size_t aHandle) const;

   // This is used to restore angles specified in the list to the control surfaces specified in the other list
   void RestoreControlSurfaceAngles(std::vector<P6DofFlightControlSystem::ControlSurfaceElement*>& aControlsList,
                                    std::vector<double>&                                           aAngleList);

   // This enables/disables controls. When controls are disabled, all control inputs are set to zero.
   void EnableControls(bool aEnabled);

   // This returns the controls enabled/disabled state.
   bool ControlsEnabled() const;

   // This returns true if an autopilot is present
   bool AutopilotAvailable() const;

   // If true, a simple yaw damper should be used. This is determined by the autopilot, so if no autopilot is present,
   // no yaw damper will be available. This "simple" yaw damper merely prevents any rotation about the body z-axis,
   // so it may not be desired, since it is simplified and not very realistic.
   bool UseSimpleYawDamper() const;

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
   double GetNormalizedAileronLeft() const;
   double GetNormalizedAileronRight() const;
   double GetNormalizedFlapLeft() const;
   double GetNormalizedFlapRight() const;
   double GetNormalizedSpoilerLeft() const;
   double GetNormalizedSpoilerRight() const;
   double GetNormalizedStabilizerLeft() const;
   double GetNormalizedStabilizerRight() const;
   double GetNormalizedElevator() const;
   double GetNormalizedRudderLeft() const;
   double GetNormalizedRudder() const;
   double GetNormalizedRudderRight() const;
   double GetNormalizedSpeedBrake() const;
   double GetNormalizedLandingGear() const;
   double GetNormalizedLandingGearNose() const;
   double GetNormalizedLandingGearMainLeft() const;
   double GetNormalizedLandingGearMainRight() const;

   // These functions return true if the data for that control surface is valid
   bool AileronLeftValid() const;
   bool AileronRightValid() const;
   bool FlapLeftValid() const;
   bool FlapRightValid() const;
   bool SpoilerLeftValid() const;
   bool SpoilerRightValid() const;
   bool StabilizerLeftValid() const;
   bool StabilizerRightValid() const;
   bool ElevatorValid() const;
   bool RudderLeftValid() const;
   bool RudderValid() const;
   bool RudderRightValid() const;
   bool SpeedBrakeValid() const;
   bool LandingGearValid() const;
   bool LandingGearNoseValid() const;
   bool LandingGearMainLeftValid() const;
   bool LandingGearMainRightValid() const;
   bool ThrottleValid() const;

   // These functions return the normalized control input (lever) value
   double GetStickRightInput() const;
   double GetStickBackInput() const;
   double GetRudderRightInput() const;
   double GetThrottleInput() const;
   double GetSpeedBrakeInput() const;
   double GetSpoilerInput() const;
   double GetLandingGearInput() const;

   // ====================================================================================================================

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
   P6DofLandingGear* GetLandingGear() const { return mLandingGearPtr; }

   // ====================================================================================================================

   // Sequencer functions

   P6DofSequencer* GetSequencerByName(const std::string& aName) const;

   // This will attempt to activate a sequencer with the specified name. Returns true if the sequencer was activated. It
   // will return false if the sequencer was already activated or cannot be activated.
   bool ActivateSequencer(const std::string& aSequencerName);

   // This will attempt to activate the specified sequencer on the specified subobject. If there is no first-level
   // subobjects with the specified name, the command will be passed to subobject, which will recurse the command to its
   // subobjects, and so on. Returns true if the sequencer was activated. It will return false if the sequencer was
   // already activated or cannot be activated.
   bool ActivateSequencer(const std::string& aSubobjectName, const std::string& aSequencerName);

   // This determines when the next time-based events (either sequencer event or a guidance and control event) will
   // occur, within a goal simulation time. It returns true if the event time is valid, else returns false, indicating
   // that there are no currently queued sequencer or guidance/control events.
   bool PendingEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This determines when the next time-based sequencer events will occur, within a goal simulation time. It returns true
   // if the event time is valid, else returns false, indicating that there are no currently queued sequencer or
   // guidance/control events.
   bool PendingSequencerEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This determines when the next time-based guidance or control event will occur, within a goal simulation time. It
   // returns true if the event time is valid, else returns false, indicating that there are no currently queued
   // sequencer or guidance/control events.
   bool PendingGuidanceControlEventTimes_nanosec(int64_t aSimTime_nanosec, std::set<int64_t>& aEventTime_nanosec);

   // This returns the last Nx_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNx_g() const;

   // This returns the last Ny_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNy_g() const;

   // This returns the last Nz_g. It is used in sequencers to detect if g-loads are increasing or decreasing.
   double GetLastNz_g() const;

   // This returns the last dynamic pressure (Q). It is used in sequencers to detect if Q is increasing or decreasing.
   double GetLastDynamicPressure_psf() const;

   // This returns the last static pressure (P). It is used in sequencers to detect if P is increasing or decreasing.
   double GetLastStaticPressure_psf() const;

   // This returns the last altitude in feet. It is used in sequencers to  detect if the altitude is increasing or
   // decreasing.
   double GetLastAltitudeMSL_ft() const;

   // This returns the last captive state (true means the object is captive to its parent). It is used in sequencers to
   // detect if the captive state has changed.
   bool GetLastIsCaptive() const { return mLastIsCaptive; }

   // ====================================================================================================================

   // Testing functions

   // This function is used to drive P6DOF in external programs, especially for performance calculations. It
   // automatically resets internal P6DOF data as well as setting the altitude, speed, pitch angle, stick and throttle
   // as specified. It also sets flags for freeze location, altitude, fuel burn, and no lag testing as well as
   // performing a single Update() function, automatically a synthetic sim time argument. Note: the stick back value may
   // be +/-1 while flaps are are 0-1. Throttle forward assumes that 1 is full (afterburner) and 0.5 is military power.
   void TestingUpdate(double aAltitude_ft,
                      double aSpeed_fps,
                      double aPitchAngle_deg,
                      double aStickBack,
                      double aThrottleForward,
                      double aFlapsDown);

   double GetNominalEstimateMaxMach() const { return mMaxMach; }
   double GetNominalEstimateMaxAlpha_deg() const { return mMaxAlpha_deg; }
   double GetNominalEstimateMinAlpha_deg() const { return mMinAlpha_deg; }
   double GetNominalEstimateMaxBeta_deg() const { return mMaxBeta_deg; }

   // This is used to prevent crashes when passing through the ground/sea surface or the artificial "floor" of sea
   // level. It is usually set to true when testing.
   void SetTestingIgnoreAllCrashes(bool aValue) { mTestingIgnoreAllCrashes = aValue; }

   // This returns true when crashes with the ground will be ignored
   bool GetTestingIgnoreAllCrashes() const { return mTestingIgnoreAllCrashes; }

   // This is used to prevent the vehicle from going below sea level at any time. The effectively limits the altitude
   // to zero. It is sometimes set to true when testing.
   void SetTestingClampToSealLevelMinAlt(bool aValue) { mTestingClampToSealLevelMinAlt = aValue; }

   // This returns true when the vehicle will be prevented from going below sea level at any time. The altitude will
   // effectively be limited to zero.
   bool GetTestingClampToSealLevelMinAlt() const { return mTestingClampToSealLevelMinAlt; }

   // This sets kinematic state parameters to a zero condition for testing
   void ZeroKinematicsDuringTesting();

   void ComputeWashInConditions(double aSimTime_sec);

   // ====================================================================================================================

   UtCallbackListN<void(double)> CrashedIntoGround;
   // First P6DofVehicle* is the Newly Jettisoned Object, and the second is the Parent object that released it
   UtCallbackListN<void(std::unique_ptr<P6DofVehicle>&)> SubobjectJettisoned;

protected:
   P6DofVehicle(const P6DofVehicle& aSrc);

   void CopyDataFromParent();

   void DeriveFromObjectType(const P6DofVehicleType* aObjectType, const std::string& aPrefixName);

   // This is used by all Initialize functions to complete "details"
   bool InitializeDetails(double aSimTime_sec);

   std::unique_ptr<P6DofVehicle> SeparateSubObject(P6DofVehicle* aSubObject);
   void                          SetSphericalEarthFlagOnSubobjects(bool aSetting);

   void SetParentVehicleForComponents();

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

   void CalculateThrustVectorAndMomentUpdatePropulsion(double                      aSimTime_sec,
                                                       double                      aDeltaTime_sec,
                                                       P6DofKinematicState&        aState,
                                                       P6DofForceAndMomentsObject& aForceAndMoment);

   void UpdatePropulsionFuelBurn(double aSimTime_sec, double aDeltaTime_sec, P6DofKinematicState& aState);

   void CalculateObjectPropulsion(double                      aSimTime_sec,
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
                                  double&                     aFuelBurned_lbs);

   void UpdateObjectPropulsion(double                      aSimTime_sec,
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
                               double&                     aFuelBurned_lbs);

   void ObjectPropulsionCalculations(double                      aSimTime_sec,
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
                                     bool                        aUpdateData);

   void CalculateSizeFactor(double dT_sec);

   void CalcTotalFuelFlowAndQuantity(double& aCurrentTotalFuelFlow_pph,
                                     double& aCurrentTotalFuel_lbs,
                                     double& aMaxTotalFuel_lbs);

   void HandleGroundContact(int64_t aSimTime_nanosec);

   void SetupStartingLatLon();
   void SetTheLastValues();

   void PerformCrash(int64_t aSimTime_nanosec);
   void ClampToSealLevelMinAlt();

   void CalculateWashIn(double aSimTime_sec);

   // Returns the angle (deg) of the surface with the specified handle
   double GetAngleSurface(size_t aHandle) const;

   // Returns the normalized angle of the surface with the specified handle
   double GetNormalizedSurfaceAngle(size_t aHandle) const;

   // Find the various parts -- landing gear, control surfaces, engines, etc.
   void FindParts();

   // Calculate the stall angle
   void CalcStallAngle();

   // ====================================================================================================================

   std::string mBaseName;
   std::string mDerivedFromObjectName;

   bool mIsCaptive;

   UtVec3dX mPosRelToParent_m;
   UtVec3dX mAngRelToParentYPR_rad;
   UtVec3dX mSeparationVel_fps;
   UtVec3dX mSeparationOmega_rps;

   std::list<ut::CloneablePtr<P6DofVehicle>> mSubObjectList;

   ut::CloneablePtr<P6DofAeroCoreObject>               mAeroCoreObjPtr;
   std::list<ut::CloneablePtr<P6DofAeroMovableObject>> mAeroPartList;

   std::string          mActiveAeroModeName;
   P6DofAeroCoreObject* mActiveAeroObjPtr;

   ut::CloneablePtr<P6DofPropulsionSystem> mPropulsionObjPtr;

   std::vector<ut::CloneablePtr<P6DofSequencer>> mSequencerList;

   int64_t mLastSimTime_nanosec;
   int64_t mStepSize_nanosec;

   // ====================================================================================================================

   P6DofMassProperties    mMassProperties;
   P6DofKinematicState    mKinematicState;
   const P6DofIntegrator* mIntegratorPtr;
   std::string            mIntegratorTypeName;

   // Graphical support
   P6DofGraphicalSupport mGraphicalSupport;

   bool mLastIsCaptive;

   bool mCrashed;
   bool mEngineOn;
   bool mAfterburnerOn;
   bool mSmoking;
   bool mOnFire;

   int64_t mLifeTime_nanosec;
   int64_t mTimeOfFlight_nanosec;

   P6DofSizeFactor mSizeFactor; // The size factor allows effects (esp aerodynamic)

   bool mFixedObject; // If set to true, the object will not move

   // These parameters are used to generate internal tabular data. Although the object can operate outside of these
   // values, the tabular data will be clamped to the highest value. For this reason, accurate estimates of the
   // operating envelope should be used.
   double mMaxMach;
   double mMaxAlpha_deg;
   double mMinAlpha_deg;
   double mMaxBeta_deg;

   UtVec3dX mCgForce_lbs;
   UtVec3dX mCgMoment_ftlbs;

   // TODO - Refactor this to avoid using "targets" in P6DOF
   UtVec3dX mTargetWCSLocation_m;
   bool     mTargetWCSLocationValid;

   double mInitialLat;
   double mInitialLon;
   double mInitialAlt_m;
   double mDownRangeDist_m;
   double mThrustTerminationTime_sec;
   bool   mThrustTerminationTimeValid;

   ut::CloneablePtr<P6DofFlightControlSystem> mFlightControlsPtr;
   ut::CloneablePtr<P6DofPilotManager>        mPilotManagerPtr;

   ut::CloneablePtr<P6DofLandingGear> mLandingGearPtr;

   UtQuaternion mQuaternion;
   bool         mInitSetupMode;
   bool         mUseWashInInitConditions;

   int64_t mTestingSimTime_nanosec;

   // These are used for tuning
   double mPrepositionLat;
   double mPrepositionLon;

   // TODO - Refactor this to avoid using "targets" in P6DOF
   P6DOF::CurrentTargetData mCurrentTarget;
   P6DOF::TrackData         mTrackData[10];
   int                      mNumTracks;

   bool mDamageSmokeTrailingEffect;
   bool mFlamesPresent;
   bool mIsDestroyed;

   bool mTestingIgnoreAllCrashes       = false;
   bool mTestingClampToSealLevelMinAlt = false;

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

   double mStallAngle               = 0.0;
   double mStallDetectionDeltaAngle = 0.0; // This is a delta angle where stall is detected
};

#endif
