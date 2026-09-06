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

#ifndef WSFSIXDOFKINEMATICSTATE_HPP
#define WSFSIXDOFKINEMATICSTATE_HPP

#include "wsf_six_dof_export.h"

#include <cstdint>

#include "UtCentralBody.hpp"
#include "UtDCM.hpp"
#include "UtEntity.hpp"
#include "UtLLAPos.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Utils.hpp"

namespace wsf
{
namespace six_dof
{
class Environment;

// KinematicState provides a collection of state data for Objects.
class WSF_SIX_DOF_EXPORT KinematicState
{
public:
   explicit KinematicState(const Environment* aEnvironmentPtr);

   // Call this to save the current values as the "last values"
   // for later reference/recall
   void SetTheLastValues();

   // This calculates the aero state, updating internal values
   void UpdateAeroState(const int64_t aSimTime_nanosec);

   // These are the last delta angles calculated during a state update.
   // They are needed by the PilotController object and the Mover
   // will call this function to get them to pass the PilotController.
   void GetAngleDeltas(double& aDeltaYaw_rad, double& aDeltaPitch_rad, double& aDeltaRoll_rad) const;
   void SetAngleDeltas(double aDeltaYaw_rad, double aDeltaPitch_rad, double aDeltaRoll_rad);

   // This is used (in testing) to remove any alpha. It is used for tests
   // involving ballistic motion with automatic/perfect "weather-vane" effect.
   void RemoveAlphaForTesting();

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

   // This returns the current speed in m/sec
   double GetSpeed_mps() const;

   // This returns the current speed in ft/sec
   double GetSpeed_fps() const;

   // This returns the current speed in KTAS (knots true airspeed)
   double GetSpeed_ktas() const;

   // This returns the current speed in KIAS/KCAS (knots indicated/calibrated airspeed)
   double GetSpeed_kias() const;

   // This returns the current speed in Mach
   double GetSpeed_Mach() const;

   // This returns the current velocity (in m/sec) in the NED frame
   UtVec3dX GetVelocityNED_mps() const;

   // This returns the current local heading in degrees
   double GetLocalHeading_deg() const { return GetLocalHeading_rad() * UtMath::cDEG_PER_RAD; }

   // This returns the current local heading in radians
   double GetLocalHeading_rad() const;

   // This returns the current local pitch angle in degrees
   double GetLocalPitch_deg() const { return GetLocalPitch_rad() * UtMath::cDEG_PER_RAD; }

   // This returns the current local pitch angle in radians
   double GetLocalPitch_rad() const;

   // This returns the current local roll angle in degrees
   double GetLocalRoll_deg() const { return GetLocalRoll_rad() * UtMath::cDEG_PER_RAD; }

   // This returns the current local roll angle in radians
   double GetLocalRoll_rad() const;

   // This returns the current flight path angle in degrees
   double GetFlightPathAngle_rad() const;

   // This returns the current flight path angle in radians
   double GetFlightPathAngle_deg() const { return GetFlightPathAngle_rad() * UtMath::cDEG_PER_RAD; }

   // This returns the current flight path angle in degrees
   double GetBearing_rad() const;

   // This returns the current flight path angle in radians
   double GetBearing_deg() const { return GetBearing_rad() * UtMath::cDEG_PER_RAD; }

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
   void SetAlt(double aAlt_m) { mUtEntity.SetLocationLLA(GetLat(), GetLon(), aAlt_m); }

   // This returns the body-relative acceleration (in gees) in the x (forward) direction
   double GetNx_g() const;

   // This returns the body-relative acceleration (in gees) in the y (right) direction
   double GetNy_g() const;

   // This returns the body-relative acceleration (in gees) in the z (down) direction
   double GetNz_g() const;

   // This returns the pitch angle (in radians) in the WCS frame
   double GetPitchWCS() const
   {
      double psi;
      double theta;
      double phi;
      mUtEntity.GetOrientationWCS(psi, theta, phi);

      return theta;
   }

   // This returns the roll angle (in radians) in the WCS frame
   double GetRollWCS() const
   {
      double psi;
      double theta;
      double phi;
      mUtEntity.GetOrientationWCS(psi, theta, phi);

      return phi;
   }

   // This returns the yaw angle (in radians) in the WCS frame
   double GetYawWCS() const
   {
      double psi;
      double theta;
      double phi;
      mUtEntity.GetOrientationWCS(psi, theta, phi);

      return psi;
   }

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
   double GetVerticalSpeed_fpm() const
   {
      UtVec3dX velocityNED;
      mUtEntity.GetVelocityNED(velocityNED.GetData());

      // Set vertical speed
      return -velocityNED.Z() * UtMath::cFT_PER_M * 60.0;
   }

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
   void SetUseSphericalEarth(bool aValue);

   // This sets the earth model to rotating
   void SetUseRotatingEarth(bool aValue) { mUseRotatingEarth = aValue; }

   // This sets the state of the ignore jettisoned objects flag
   void SetIgnoreJettisonObjects(bool aValue) { mIgnoreJettisonObjects = aValue; }

   // This sets the current LLA (lat, lon, altitude) in degrees, degrees, and meters, respectively
   void SetPositionLLA(double aLat, double aLon, double aAlt_m);

   // This sets the current attitude (heading, pitch, and roll) using radians
   void SetAttitude_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad);

   // This sets the current speed in feet/sec
   void SetSpeed_fps(double aSpeed_fps, double aBearing_deg, double aFlightPathAngle_deg);

   // This sets the current speed in meters/sec
   void SetSpeed_mps(double aSpeed_mps, double aBearing_deg, double aFlightPathAngle_deg);

   // This sets the current speed in mph
   void SetSpeed_mph(double aSpeed_mph, double aBearing_deg, double aFlightPathAngle_deg);

   // This sets the current speed in KTAS (knots true airspeed)
   void SetSpeed_ktas(double aSpeed_ktas, double aBearing_deg, double aFlightPathAngle_deg);

   // This sets the current speed in KIAS/KCAS (knots indicated/calibrated airspeed)
   void SetSpeed_kias(double aSpeed_kias, double aBearing_deg, double aFlightPathAngle_deg);

   // This sets the current speed in Mach
   void SetSpeed_Mach(double aSpeed_Mach, double aBearing_deg, double aFlightPathAngle_deg);

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
   void CalculateSecondaryParameters();

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
   // the Mover (parent) along with the relative position of the point (which
   // is defined in body coordinates) to calculate the lat/lon/alt of the point.
   // It also calculates the compression and rolling vectors in NED coords and returns
   // terrain data at the point, including the normal of the terrain (in NED) and the
   // friction multiplier. It also provides the compression of the point "into" the terrain.
   // It returns true if data is valid.
   static bool CalculateLLAofRelativePoint(double                aTerrainElevation_m,
                                           const KinematicState* aKinematicState,
                                           const UtVec3dX&       aBodyRelPos_m,
                                           const UtVec3dX&       aBodyRelCompressionVector,
                                           const UtVec3dX&       aBodyRelRollingVector,
                                           UtVec3dX&             aGearVelNED_mps,
                                           double&               aLat,
                                           double&               aLon,
                                           double&               aAlt_m,
                                           double&               aHeightAboveTerrain_m,
                                           UtVec3dX&             aCompressionVectorNED,
                                           UtVec3dX&             aRollingVectorInSurfaceNED,
                                           UtVec3dX&             aNormalVectorNED,
                                           UtVec3dX&             aSurfaceSpeedVectorNED_mps,
                                           double&               aFrictionMultiplier,
                                           double&               aCompression_m);

   ////////////////////////////////////////////////////////////////////////////////

   // This collection of six functions utilize the current state of the object

   // Get NED vector from other reference frames

   // This returns a NED vector based on a vector in body coords
   UtVec3dX CalcNEDVecFromBodyVec(const UtVec3dX& aVecBody) const;

   // This returns a NED vector based on a vector in body coords
   UtVec3dX CalcNEDVecFromWCSVec(const UtVec3dX& aVecInertial) const;

   // Get inertial vector from other reference frames

   // This returns an inertial vector based on a vector in body coords
   UtVec3dX CalcWCSVecFromBodyVec(const UtVec3dX& aVecBody) const;

   // This returns an inertial vector based on a vector in NED coords
   UtVec3dX CalcWCSVecFromNEDVec(const UtVec3dX& aVecNED) const;

   // Get body vector from other reference frames

   // This returns a body vector based on a vector in NED coords
   UtVec3dX CalcBodyVecFromNEDVec(const UtVec3dX& aVecNED) const;

   // This returns a body vector based on a vector in inertial coords
   UtVec3dX CalcBodyVecFromWCSVec(const UtVec3dX& aVecInertial) const;

   ////////////////////////////////////////////////////////////////////////////////

   UtVec3dX GetVelocityECI() const
   {
      UtVec3dX vel;
      mUtEntity.GetVelocityECI(vel.GetData());
      return vel;
   }

   // Returns rotational rate in radians/sec
   UtVec3dX GetOmegaBody() const { return mOmega; }

   // Sets rotational rate in radians/sec
   void SetOmegaBody(const UtVec3dX& aOmega) { mOmega = aOmega; }

   // Returns rotational acceleration (omega-dot) in radians/sec^2
   UtVec3dX GetOmegaBodyDot() const { return mOmegaDot; }

   // Sets rotational acceleration (omega-dot) in radians/sec^2
   void SetOmegaBodyDot(const UtVec3dX& aOmegaDot) { mOmegaDot = aOmegaDot; }

   UtVec3dX GetLocationWCS() const
   {
      UtVec3dX pos;
      mUtEntity.GetLocationWCS(pos.GetData());
      return pos;
   }
   void SetLocationWCS(const UtVec3dX& aPosWCS) { mUtEntity.SetLocationWCS(aPosWCS.GetData()); }

   UtVec3dX GetVelocityWCS() const
   {
      UtVec3dX vel;
      mUtEntity.GetVelocityWCS(vel.GetData());
      return vel;
   }
   void SetVelocityWCS(const UtVec3dX& aVelWCS) { mUtEntity.SetVelocityWCS(aVelWCS.GetData()); }

   void ZeroVelocity();

   // Returns rotational rate in deg/sec
   UtVec3dX GetOmegaBodyDPS() const;

   bool StartingLocationInitialized() const { return mStartingLocationInitialized; }
   void SetStartingLatLon(double aLat, double aLon);
   void GetStartingLatLon(double& aLat, double& aLon) const;

   double GetRangeSinceStart_km() const { return mRangeSinceStart_km; }
   double GetApogee_km() const { return mApogee_km; }

   void     SetAccelerationWCS(const UtVec3dX& aAccelerationWCS) { mAccelerationWCS = aAccelerationWCS; }
   UtVec3dX GetAccelerationWCS() const { return mAccelerationWCS; }

   UtEntity& GetUtEntity() { return mUtEntity; }

   // Returns an acceleration vector normalized to the mean radius magnitude
   UtVec3dX NormalizedGravitationalAccelVec() const;

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
   // This flag is used to denote that a spherical earth (rather than elliptical
   // earth) model should be used in calculations. This is often done for simple
   // ballistic missile models.
   bool mUseSphericalEarth = false;

   // This flag is used to denote that a earth rotation should be considered in
   // calculations. This is often done for high-fidelity ballistic missile models.
   bool mUseRotatingEarth = false;

   // This flag is used to ignore any jettisoned objects. This prevents the
   // subobjects from becoming free -- they will be removed instead. This flag
   // is typically used for multi-stage rockets/missile where the trajectory
   // of spent stages is not needed.
   bool mIgnoreJettisonObjects = false;

   UtVec3dX mAccelerationWCS;

   UtDCM    mDCM;
   UtEntity mUtEntity;
   UtVec3dX mOmega;    // Body rates
   UtVec3dX mOmegaDot; // Body acceleration

   double   mNx_g          = 0.0;
   double   mNy_g          = 0.0;
   double   mNz_g          = 0.0;
   double   mLift_lbs      = 0.0;
   double   mThrust_lbs    = 0.0;
   double   mDrag_lbs      = 0.0;
   double   mSideForce_lbs = 0.0;
   double   mWgt_lbs       = 0.0;
   UtVec3dX mMoment_ftlbs;

   double mRho_slugs_per_ft3   = 0.0; // [ ((lbf*sec^2)/ft^4) or slugs/ft^3 ]
   double mDynamicPressure_psf = 0.0;
   double mStaticPressure_psf  = 0.0;

   double mRadarAlt_m = 0.0;

   double mAlpha_deg    = 0.0;
   double mBeta_deg     = 0.0;
   double mAlphaDot_dps = 0.0;
   double mBetaDot_dps  = 0.0;

   double mYawRate_dps   = 0.0;
   double mPitchRate_dps = 0.0;
   double mRollRate_dps  = 0.0;

   double mApogee_km = 0.0;

   double mRangeSinceStart_km          = 0.0;
   double mStartingLat                 = 0.0;
   double mStartingLon                 = 0.0;
   bool   mStartingLocationInitialized = false;

   double mLastNx                  = 0.0;
   double mLastNy                  = 0.0;
   double mLastNz                  = 0.0;
   double mLastDynamicPressure_psf = 0.0;
   double mLastStaticPressure_psf  = 0.0;
   double mLastAlt_m               = 0.0;

   const Environment* mEnvironment = nullptr;

   // These are the last delta angles calculated during an update. They
   // are needed by the PilotController object and the Mover object will
   // call the GetAngleDeltas() function above to get them.
   double mDeltaYaw_rad   = 0.0;
   double mDeltaPitch_rad = 0.0;
   double mDeltaRoll_rad  = 0.0;

   int64_t mLastUpdate_nanosec = -1;
   double  mLastAlpha_deg      = 0.0;
   double  mLastBeta_deg       = 0.0;
};

//! Definition for spherical Earth, based on WGS-84 ellipsoid.
class WSF_SIX_DOF_EXPORT SixDOF_EarthSphere : public ut::EarthEllipsoidWGS84
{
public:
   static constexpr const char* GetNameId() { return "earth_sixdofsphere"; }
   const char*                  GetName() const override { return GetNameId(); }
   CentralBodyEllipsoid*        Clone() const override { return new SixDOF_EarthSphere(*this); }
   double                       GetSemiMajorAxis() const override { return GetMeanRadius(); }
   double                       GetSemiMinorAxis() const override { return GetMeanRadius(); }
   double GetFlatteningReciprocal() const override { return ut::CentralBodyEllipsoid::GetFlatteningReciprocal(); }
   double GetFirstEccentricitySquared() const override { return 0.0; }
   double GetSecondEccentricitySquared() const override { return 0.0; }
};

class WSF_SIX_DOF_EXPORT SixDOF_SphericalEarth : public ut::EarthCentralBody
{
public:
   SixDOF_SphericalEarth()
      : EarthCentralBody(SixDOF_EarthSphere())
   {
   }

   CentralBody*                 Clone() const override { return new SixDOF_SphericalEarth(*this); }
   static constexpr const char* GetNameId() { return SixDOF_EarthSphere::GetNameId(); }
};
} // namespace six_dof
} // namespace wsf

#endif
