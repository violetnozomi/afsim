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

#ifndef P6DOFKINEMATICSTATE_H
#define P6DOFKINEMATICSTATE_H

#include "p6dof_export.h"

#include <cstdint>

#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtLLAPos.hpp"
#include "UtVec3dX.hpp"

class P6DofAtmosphere;
class P6DofGravity;
class P6DofTerrain;
class P6DofWind;

// P6DofKinematicState provides a collection of state data for P6DofObjects.
class P6DOF_EXPORT P6DofKinematicState
{
public:
   P6DofKinematicState();

   // Call this to save the current values as the "last values"
   // for later reference/recall
   void SetTheLastValues();

   // This calculates the aero state, updating internal values
   void UpdateAeroState(const P6DofAtmosphere* aAtmosphere,
                        const P6DofWind*       aWind,
                        const int64_t          aSimTime_nanosec,
                        const double           aDeltaT_sec);

   // These are the last delta angles calculated during a state update.
   // They are needed by the PilotController object and the P6DofVehicle
   // will call this function to get them to pass the PilotController.
   void GetAngleDeltas(double& aDeltaYaw_rad, double& aDeltaPitch_rad, double& aDeltaRoll_rad) const;
   void SetAngleDeltas(double aDeltaYaw_rad, double aDeltaPitch_rad, double aDeltaRoll_rad);

   // This is used (in testing) to remove any alpha. It is used for tests
   // involving ballistic motion with automatic/perfect "weather-vane" effect.
   void RemoveAlphaForTesting(P6DofAtmosphere* aAtmospherePtr);

   // This returns the current angle of attack (alpha) in degrees
   double GetAlpha_deg() const;

   // This returns the current angle of attack (alpha) in radians
   double GetAlpha_rad() const;

   // This returns the current angle of sideslip (beta) in degrees
   double GetBeta_deg() const;

   // This returns the current angle of sideslip (beta) in radians
   double GetBeta_rad() const;

   // This returns the current angle of attack rate (alpha-dot) in degrees/sec
   double GetAlphaDot_dps() const;

   // This returns the current angle of attack rate (alpha-dot) in radians/sec
   double GetAlphaDot_rps() const;

   // This returns the current angle of sideslip rate (beta-dot) in degrees/sec
   double GetBetaDot_dps() const;

   // This returns the current angle of sideslip rate (beta-dot) in radians/sec
   double GetBetaDot_rps() const;

   // This sets alpha-dot and beta-dot value to zero (used in testing)
   void SetAlphaBetaDotToZero();

   // This returns the current DCM (direction cosine matrix)
   UtDCM GetDCM() const { return mDCM; }

   // This sets the current DCM
   void SetDCM(const UtDCM& aDCM) { mDCM = aDCM; }

   // This returns the current LLA (lat, lon, altitude) in degrees, degrees, and meters, respectively
   UtLLAPos GetCurrentPositionLLA() const;

   // This returns the current speed in ft/sec
   double GetSpeed_fps() const { return mSpeed_fps; }

   // This returns the current speed in m/sec
   double GetSpeed_mps() const;

   // This returns the current speed in KTAS (knots true airspeed)
   double GetSpeed_ktas() const { return mSpeed_ktas; }

   // This returns the current speed in KIAS/KCAS (knots indicated/calibrated airspeed)
   double GetSpeed_kias() const { return mSpeed_kias; }

   // This returns the current speed in Mach
   double GetSpeed_Mach() const { return mSpeed_Mach; }

   // This returns the current velocity (in m/sec) in the NED frame
   UtVec3dX GetVelocityNED_mps() const;

   // This returns the current local heading in degrees
   double GetLocalHeading_deg() const { return mLocalHeading_deg; }

   // This returns the current local heading in radians
   double GetLocalHeading_rad() const;

   // This returns the current local pitch angle in degrees
   double GetLocalPitch_deg() const { return mLocalPitch_deg; }

   // This returns the current local pitch angle in radians
   double GetLocalPitch_rad() const;

   // This returns the current local roll angle in degrees
   double GetLocalRoll_deg() const { return mLocalRoll_deg; }

   // This returns the current local roll angle in radians
   double GetLocalRoll_rad() const;

   // This sets the current local heading in degrees
   void SetLocalHeading_deg(double aHeading) { mLocalHeading_deg = aHeading; }

   // This sets the current local pitch angle in degrees
   void SetLocalPitch_deg(double aPitch) { mLocalPitch_deg = aPitch; }

   // This sets the current local roll angle in degrees
   void SetLocalRoll_deg(double aRoll) { mLocalRoll_deg = aRoll; }

   // This returns the current flight path angle in degrees
   double GetFlightPathAngle_rad() const { return mFlightPathAngle_rad; }

   // This returns the current flight path angle in radians
   double GetFlightPathAngle_deg() const;

   // This returns the current dynamic pressure in lbs/sq-ft
   double GetDynamicPressure_psf() const { return mDynamicPressure_psf; }

   // This returns the current static pressure in lbs/sq-ft
   double GetStaticPressure_psf() const { return mStaticPressure_psf; }

   // This returns the current air density in slugs/cubic-foot
   double GetAirDensity_slugs_per_ft3() const { return mRho_slugs_per_ft3; }

   // This returns the current latitude in degrees
   double GetLat() const;

   // This returns the current longitude in degrees
   double GetLon() const;

   // This returns the current altitude in meters
   double GetAlt_m() const;

   // This returns the current altitude in feet
   double GetAlt_ft() const;

   // This sets the altitude in meters
   void SetAlt(double aAlt_m) { mAlt_m = aAlt_m; }

   // This returns the body-relative acceleration (in gees) in the x (forward) direction
   double GetNx_g() const;

   // This returns the body-relative acceleration (in gees) in the y (right) direction
   double GetNy_g() const;

   // This returns the body-relative acceleration (in gees) in the z (down) direction
   double GetNz_g() const;

   // This returns the pitch angle (in radians) in the WCS frame
   double GetPitchWCS() const { return mWcsPitch_rad; }

   // This returns the roll angle (in radians) in the WCS frame
   double GetRollWCS() const { return mWcsRoll_rad; }

   // This returns the yaw angle (in radians) in the WCS frame
   double GetYawWCS() const { return mWcsYaw_rad; }

   // This sets the yaw, pitch, and roll angles (in radians) in the WCS frame
   void SetOrientationWCS(double aYaw_rad, double aPitch_rad, double aRoll_rad);

   // This returns the yaw rate in radians/sec
   double GetYawRate_rps() const;

   // This returns the pitch rate in radians/sec
   double GetPitchRate_rps() const;

   // This returns the roll rate in radians/sec
   double GetRollRate_rps() const;

   // This returns the yaw rate in degrees/sec
   double GetYawRate_dps() const;

   // This returns the pitch rate in degrees/sec
   double GetPitchRate_dps() const;

   // This returns the roll rate in degrees/sec
   double GetRollRate_dps() const;

   // This sets the yaw rate in degrees/sec
   void SetYawRate(double aYawRate_dps);

   // This sets the pitch rate in degrees/sec
   void SetPitchRate(double aPitchRate_dps);

   // This sets the roll rate in degrees/sec
   void SetRollRate(double aRollRate_dps);

   // This returns the vertical speed in ft/minute
   double GetVerticalSpeed_fpm() const { return mVertSpeed_fpm; }

   // This returns the last dynamic pressure in lbs/sq-ft
   double GetLastDynamicPressure_psf() const;

   // This returns the last static pressure in lbs/sq-ft
   double GetLastStaticPressure_psf() const;

   // This returns the last altitude in feet
   double GetLastAltitudeMSL_ft() const;


   // This provides the lift, drag, and side force vectors in body coordinates
   void GetBodyAeroVectors(const UtDCM&    aDCM,
                           const UtVec3dX& aInertialVelVector,
                           UtVec3dX&       aLiftBodyVector,
                           UtVec3dX&       aDragBodyVector,
                           UtVec3dX&       aSideForceBodyVector) const;

   // This returns true if a spherical earth model is being used
   bool UseSphericalEarth() const { return mUseSphericalEarth; }

   // This returns true if a rotating earth model is being used
   bool UseRotatingEarth() const { return mUseRotatingEarth; }

   // This returns true if the ignore jettisoned objects flag is active
   bool IgnoreJettisonObjects() const { return mIgnoreJettisonObjects; }

   // This sets the earth model to spherical
   void SetUseSphericalEarth(bool aValue) { mUseSphericalEarth = aValue; }

   // This sets the earth model to rotating
   void SetUseRotatingEarth(bool aValue) { mUseRotatingEarth = aValue; }

   // This sets the state of the ignore jettisoned objects flag
   void SetIgnoreJettisonObjects(bool aValue) { mIgnoreJettisonObjects = aValue; }

   // This sets the current LLA (lat, lon, altitude) in degrees, degrees, and meters, respectively
   void SetPositionLLA(double aLat, double aLon, double aAlt_m);

   // This sets the current attitude (heading, pitch, and roll) using radians
   void SetAttitude_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad);

   // This sets the current vertical speed in feet/minute
   void SetVerticalSpeed_fpm(double aVertSpeed_fpm);

   // This sets the current speed in feet/sec
   void SetSpeed_fps(double aSpeed_fps);

   // This sets the current speed in meters/sec
   void SetSpeed_mps(double aSpeed_mps);

   // This sets the current speed in mph
   void SetSpeed_mph(double aSpeed_mph);

   // This sets the current speed in KTAS (knots true airspeed)
   void SetSpeed_ktas(double aSpeed_ktas);

   // This sets the current speed in KIAS/KCAS (knots indicated/calibrated airspeed)
   void SetSpeed_kias(double aSpeed_kias);

   // This sets the current speed in Mach
   void SetSpeed_Mach(double aSpeed_Mach);

   // This sets the body-relative accelerations in gees
   void SetBodyAccel(double aNx_g, double aNy_g, double aNz_g);

   // This sets the lift, drag, side force, thrust, and weight
   void SetLiftDragSideForceThrustWeight(double aLift_lbs,
                                         double aDrag_lbs,
                                         double aSideForce_lbs,
                                         double aThrust_lbs,
                                         double aWgt_lbs);

   // This sets the moment at the CG
   void SetMomentAtCG(UtVec3dX aMoment_ftlbs);

   // This sets the velocity in the NED frame using meters/sec
   void SetVelocityNED(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps);

   // Calculate rates (especially alpha-dot and beta-dot) based on current state and last update time
   void CalculateRates(int64_t aSimTime_nanosec);

   // CalculateSecondaryParameters uses WGS/spherical data
   //
   //   mWGS84_pos, mWGS84_vel,
   //   mDCM,
   //   mOmega,
   //   mNx/mNy/mNz
   //
   // to calculate:
   //
   //   mLat,mLon,mAlt_m,mRadarAlt_m
   //   mVelNorth_mps,mVelEast_mps,mVelDown_mps,
   //   mVertSpeed_fpm,
   //   mLocalHeading_deg,mLocalPitch_deg,mLocalRoll_deg,
   //   mWcsYaw_rad,mWcsPitch_rad,mWcsRoll_rad
   //   mRollRate_dps,mPitchRate_dps,mYawRate_dps
   //   mAlpha_deg,mBeta_deg
   //   mRho,mDynamicPressure_psf,mStaticPressure_psf,mMach
   //   mSpeed_ktas,mSpeed_kias
   void CalculateSecondaryParameters(const P6DofAtmosphere* aAtmospherePtr);

   // This calculates the great circle distance from the point at which the object was created
   void CalcRangeSinceStart();

   // This returns the last body-relative acceleration (in gees) in the x-direction
   double GetLastNx_g() const;

   // This returns the last body-relative acceleration (in gees) in the y-direction
   double GetLastNy_g() const;

   // This returns the last body-relative acceleration (in gees) in the z-direction
   double GetLastNz_g() const;

   // -------------------------------------------------------------------------

   // The CalculateLLAofRelativePoint calculates the LLA of a relative point. It is
   // typically used for ground reaction points. It uses the location and attitude of
   // the P6DofVehicle (parent) along with the relative position of the point (which
   // is defined in body coordinates) to calculate the lat/lon/alt of the point.
   // It also calculates the compression and rolling vectors in NED coords and returns
   // terrain data at the point, including the normal of the terrain (in NED) and the
   // friction multiplier. It also provides the compression of the point "into" the terrain.
   // It returns true if data is valid.
   static bool CalculateLLAofRelativePoint(const P6DofTerrain*        aTerrainPtr,
                                           const P6DofKinematicState& aKinematicState,
                                           const UtVec3dX&            aBodyRelPos_m,
                                           const UtVec3dX&            aBodyRelCompressionVector,
                                           const UtVec3dX&            aBodyRelRollingVector,
                                           UtVec3dX&                  aGearVelNED_mps,
                                           double&                    aLat,
                                           double&                    aLon,
                                           double&                    aAlt_m,
                                           double&                    aHeightAboveTerrain_m,
                                           UtVec3dX&                  aCompressionVectorNED,
                                           UtVec3dX&                  aRollingVectorInSurfaceNED,
                                           UtVec3dX&                  aNormalVectorNED,
                                           UtVec3dX&                  aSurfaceSpeedVectorNED_mps,
                                           double&                    aFrictionMultiplier,
                                           double&                    aCompression_m);

   static UtVec3dX NormalizedGravitationalAccelVec(const P6DofGravity* aGravityPtr,
                                                   const double        aLat,
                                                   const double        aLon,
                                                   const double        aAlt_m,
                                                   const bool          aUseSphericalEarth);

   ////////////////////////////////////////////////////////////////////////////////

   // This collection of six functions utilize the current state of the object

   // Get NED vector from other reference frames

   // This returns a NED vector based on a vector in body coords
   UtVec3dX CalcNEDVecFromBodyVec(const UtVec3dX& aVecBody) const;

   // This returns a NED vector based on a vector in body coords
   UtVec3dX CalcNEDVecFromInertialVec(const UtVec3dX& aVecInertial) const;

   // Get inertial vector from other reference frames

   // This returns an inertial vector based on a vector in body coords
   UtVec3dX CalcInertialVecFromBodyVec(const UtVec3dX& aVecBody) const;

   // This returns an inertial vector based on a vector in NED coords
   UtVec3dX CalcInertialVecFromNEDVec(const UtVec3dX& aVecNED) const;

   // Get body vector from other reference frames

   // This returns a body vector based on a vector in NED coords
   UtVec3dX CalcBodyVecFromNEDVec(const UtVec3dX& aVecNED) const;

   // This returns a body vector based on a vector in inertial coords
   UtVec3dX CalcBodyVecFromInertialVec(const UtVec3dX& aVecInertial) const;

   ////////////////////////////////////////////////////////////////////////////////

   UtVec3dX GetInertialVelocity() const;

   // Returns rotational rate in radians/sec
   UtVec3dX GetOmegaBody() const { return mOmega; }

   // Sets rotational rate in radians/sec
   void SetOmegaBody(const UtVec3dX& aOmega) { mOmega = aOmega; }

   // Returns rotational acceleration (omega-dot) in radians/sec^2
   UtVec3dX GetOmegaBodyDot() const { return mOmegaDot; }

   // Sets rotational acceleration (omega-dot) in radians/sec^2
   void SetOmegaBodyDot(const UtVec3dX& aOmegaDot) { mOmegaDot = aOmegaDot; }

   UtVec3dX GetLocationWGS() const { return mWGS84_pos; }
   void     SetLocationWGS(const UtVec3dX& aWGS84_pos) { mWGS84_pos = aWGS84_pos; }

   UtVec3dX GetVelocityWGS() const { return mWGS84_vel; }
   void     SetVelocityWGS(const UtVec3dX& aWGS84_vel) { mWGS84_vel = aWGS84_vel; }

   UtVec3dX GetLocationSpherical() const { return mSpherical_pos; }
   void     SetLocationSpherical(const UtVec3dX& aSpherical_pos) { mSpherical_pos = aSpherical_pos; }

   UtVec3dX GetVelocitySpherical() const { return mSpherical_vel; }
   void     SetVelocitySpherical(const UtVec3dX& aSpherical_vel) { mSpherical_vel = aSpherical_vel; }

   void ZeroVelocity();

   // Returns rotational rate in deg/sec
   UtVec3dX GetOmegaBodyDPS() const;

   bool StartingLocationInitialized() const { return mStartingLocationInitialized; }
   void SetStartingLatLon(double aLat, double aLon);
   void GetStartingLatLon(double& aLat, double& aLon) const;

   double GetRangeSinceStart_km() const { return mRangeSinceStart_km; }
   double GetApogee_km() const { return mApogee_km; }

   void     SetInertialAccel(const UtVec3dX& aInertialAccel) { mInertialAccel = aInertialAccel; }
   UtVec3dX GetInertialAccel() const { return mInertialAccel; }

   void SetRefEarthRadius();

   UtEntity& GetUtEntity() { return mUtEntity; }

   const P6DofAtmosphere* GetAtmospherePtr() const { return mAtmospherePtr; }

   double   GetLift() const { return mLift_lbs; }                // Returns the lift in lbs
   double   GetDrag() const { return mDrag_lbs; }                // Returns the drag in lbs
   double   GetSideForce() const { return mSideForce_lbs; }      // Returns the side force in lbs
   UtVec3dX GetMoment() const { return mMoment_ftlbs; }          // Returns the moment vector in ft*lbs
   double   GetYawMoment() const { return mMoment_ftlbs.Z(); }   // Returns the yaw moment in ft*lbs
   double   GetPitchMoment() const { return mMoment_ftlbs.Y(); } // Returns the pitch moment in ft*lbs
   double   GetRollMoment() const { return mMoment_ftlbs.X(); }  // Returns the roll moment in ft*lbs
   double   GetThrust() const { return mThrust_lbs; }            // Returns the thrust in lbs
   double   GetWeight() const { return mWgt_lbs; }               // Returns the weight/mass in lbs

   void SetLastUpdateTime(int64_t aLastUpdate_nanosec) { mLastUpdate_nanosec = aLastUpdate_nanosec; }

   // This sets kinematic state parameters to a zero condition during testing
   void ZeroKinematicsDuringTesting();

protected:
   // This is used for the earth radius when mUseSphericalEarth is true
   // Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
   // Typical earth radius is 6371 km -- note the 4.3 km discrepancy
   // Distances from points on the surface to the center range from
   // 6,353 km to 6,384 km. Several different ways of modeling the
   // Earth as a sphere each yield a mean radius of 6,371 km.
   static const double gSphericalEarthRadius_m;

   // This flag is used to denote that a spherical earth (rather than elliptical
   // earth) model should be used in calculations. This is often done for simple
   // ballistic missile models.
   bool mUseSphericalEarth;

   // This flag is used to denote that a earth rotation should be considered in
   // calculations. This is often done for high-fidelity ballistic missile models.
   bool mUseRotatingEarth;

   // This flag is used to ignore any jettisoned objects. This prevents the
   // subobjects from becoming free -- they will be removed instead. This flag
   // is typically used for multi-stage rockets/missile where the trajectory
   // of spent stages is not needed.
   bool mIgnoreJettisonObjects;

   UtVec3dX mWGS84_pos;
   UtVec3dX mWGS84_vel;

   UtVec3dX mSpherical_pos;
   UtVec3dX mSpherical_vel;

   UtVec3dX mInertialAccel;

   UtDCM    mDCM;
   UtEntity mUtEntity;
   UtVec3dX mOmega;    // Body rates
   UtVec3dX mOmegaDot; // Body acceleration

   double   mNx_g;
   double   mNy_g;
   double   mNz_g;
   double   mLift_lbs;
   double   mThrust_lbs;
   double   mDrag_lbs;
   double   mSideForce_lbs;
   double   mWgt_lbs;
   UtVec3dX mMoment_ftlbs;

   double mRho_slugs_per_ft3; // [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ]
   double mDynamicPressure_psf;
   double mStaticPressure_psf;

   double mLat;
   double mLon;
   double mAlt_m;
   double mRadarAlt_m;

   double mVelNorth_mps;
   double mVelEast_mps;
   double mVelDown_mps;

   double mAlpha_deg;
   double mBeta_deg;
   double mAlphaDot_dps;
   double mBetaDot_dps;

   double mSpeed_fps;
   double mSpeed_ktas;
   double mSpeed_kias;
   double mSpeed_Mach;

   double mVertSpeed_fpm;
   double mYawRate_dps;
   double mPitchRate_dps;
   double mRollRate_dps;

   double mLocalHeading_deg;
   double mLocalPitch_deg;
   double mLocalRoll_deg;

   double mWcsYaw_rad;   // TODO - review the use of these
   double mWcsPitch_rad; // TODO - review the use of these
   double mWcsRoll_rad;  // TODO - review the use of these

   double mApogee_km;

   double mFlightPathAngle_rad;

   // This is used for range calculations. It is set during initialization,
   // based on the type of earth (WGS or spherical) used by this object.
   double mRefEarthRadius_m;

   double mRangeSinceStart_km;
   double mStartingLat;
   double mStartingLon;
   bool   mStartingLocationInitialized;

   double mLastNx;
   double mLastNy;
   double mLastNz;
   double mLastDynamicPressure_psf;
   double mLastStaticPressure_psf;
   double mLastAlt_m;

   const P6DofAtmosphere* mAtmospherePtr;

   // These are the last delta angles calculated during an update. They
   // are needed by the PilotController object and the P6DofVehicle will
   // call the GetAngleDeltas() function above to get them.
   double mDeltaYaw_rad;
   double mDeltaPitch_rad;
   double mDeltaRoll_rad;

   int64_t mLastUpdate_nanosec = -1;
   double  mLastAlpha_deg      = 0.0;
   double  mLastBeta_deg       = 0.0;
};

#endif
