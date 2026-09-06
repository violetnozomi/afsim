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

#include "P6DofKinematicState.hpp"

#include "P6DofAtmosphere.hpp"
#include "P6DofGravity.hpp"
#include "P6DofTerrain.hpp"
#include "P6DofUtils.hpp"
#include "P6DofWind.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

// This is used for the earth radius when mUseSphericalEarth is true
// Note: The AFSIM cEARTH_RADIUS = 6366707.0194937074958 (6366.7 km)
// Typical earth radius is 6371 km -- note the 4.3 km discrepancy
// Distances from points on the surface to the center range from
// 6,353 km to 6,384 km. Several different ways of modeling the
// Earth as a sphere each yield a mean radius of 6,371 km.
const double P6DofKinematicState::gSphericalEarthRadius_m = 6371000.0;

P6DofKinematicState::P6DofKinematicState()
   : mUseSphericalEarth(false)
   , mUseRotatingEarth(false)
   , mIgnoreJettisonObjects(false)
   , mWGS84_pos(0., 0., 0.)
   , mWGS84_vel(0., 0., 0.)
   , mSpherical_pos(0., 0., 0.)
   , mSpherical_vel(0., 0., 0.)
   , mInertialAccel(0., 0., 0.)
   , mDCM(0.0, 0.0, 0.0)
   , mOmega(0., 0., 0.)
   , mOmegaDot(0., 0., 0.)
   , mNx_g(0)
   , mNy_g(0)
   , mNz_g(0)
   , mLift_lbs(0)
   , mThrust_lbs(0)
   , mDrag_lbs(0)
   , mSideForce_lbs(0)
   , mWgt_lbs(0)
   , mMoment_ftlbs(0., 0., 0.)
   , mRho_slugs_per_ft3(0)
   , mDynamicPressure_psf(0)
   , mStaticPressure_psf(0)
   , mLat(0)
   , mLon(0)
   , mAlt_m(0)
   , mRadarAlt_m(0)
   , mVelNorth_mps(0)
   , mVelEast_mps(0)
   , mVelDown_mps(0)
   , mAlpha_deg(0)
   , mBeta_deg(0)
   , mAlphaDot_dps(0)
   , mBetaDot_dps(0)
   , mSpeed_fps(0)
   , mSpeed_ktas(0)
   , mSpeed_kias(0)
   , mSpeed_Mach(0)
   , mVertSpeed_fpm(0)
   , mYawRate_dps(0)
   , mPitchRate_dps(0)
   , mRollRate_dps(0)
   , mLocalHeading_deg(0)
   , mLocalPitch_deg(0)
   , mLocalRoll_deg(0)
   , mWcsYaw_rad(0)
   , mWcsPitch_rad(0)
   , mWcsRoll_rad(0)
   , mApogee_km(0)
   , mFlightPathAngle_rad(0)
   , mRefEarthRadius_m(0)
   , mRangeSinceStart_km(0)
   , mStartingLat(0)
   , mStartingLon(0)
   , mStartingLocationInitialized(false)
   , mLastNx(0)
   , mLastNy(0)
   , mLastNz(0)
   , mLastDynamicPressure_psf(0)
   , mLastStaticPressure_psf(0)
   , mLastAlt_m(0)
   , mAtmospherePtr(nullptr)
   , mDeltaYaw_rad(0)
   , mDeltaPitch_rad(0)
   , mDeltaRoll_rad(0)
{
}

void P6DofKinematicState::SetPositionLLA(double aLat, double aLon, double aAlt_m)
{
   mLat   = aLat;
   mLon   = aLon;
   mAlt_m = aAlt_m;
}

void P6DofKinematicState::SetAttitude_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad)
{
   mLocalHeading_deg = aHeading_rad * UtMath::cDEG_PER_RAD;
   mLocalPitch_deg   = aPitch_rad * UtMath::cDEG_PER_RAD;
   mLocalRoll_deg    = aRoll_rad * UtMath::cDEG_PER_RAD;
}

void P6DofKinematicState::SetVerticalSpeed_fpm(double aVertSpeed_fpm)
{
   mVertSpeed_fpm = aVertSpeed_fpm;
}

void P6DofKinematicState::SetSpeed_fps(double aSpeed_fps)
{
   mSpeed_fps = aSpeed_fps;

   if (mAtmospherePtr == nullptr)
   {
      mSpeed_ktas = mSpeed_fps * (3600.0 / UtMath::cNM_PER_FT);
      mSpeed_kias = mSpeed_ktas;
      mSpeed_Mach = mSpeed_fps / 1000.0;
   }
   else
   {
      mSpeed_ktas       = mAtmospherePtr->CalcKtasFromFps(mSpeed_fps);
      double tempAlt_ft = GetAlt_ft();
      mSpeed_kias       = mAtmospherePtr->CalcKcasFromFps(tempAlt_ft, mSpeed_fps);
      mSpeed_Mach       = mAtmospherePtr->CalcMachFromFps(tempAlt_ft, mSpeed_fps);
   }
}

void P6DofKinematicState::SetSpeed_mps(double aSpeed_mps)
{
   SetSpeed_fps(aSpeed_mps * UtMath::cFT_PER_M);
}

void P6DofKinematicState::SetSpeed_mph(double aSpeed_mph)
{
   SetSpeed_fps(aSpeed_mph * UtMath::cFT_PER_MI / 3600.0);
}

void P6DofKinematicState::SetSpeed_ktas(double aSpeed_ktas)
{
   mSpeed_ktas = aSpeed_ktas;

   if (mAtmospherePtr == nullptr)
   {
      mSpeed_fps  = mSpeed_ktas * (UtMath::cNM_PER_FT / 3600.0);
      mSpeed_kias = mSpeed_ktas;
      mSpeed_Mach = mSpeed_fps / 1000.0;
   }
   else
   {
      mSpeed_fps        = mAtmospherePtr->CalcFpsFromKtas(mSpeed_ktas);
      double tempAlt_ft = GetAlt_ft();
      mSpeed_kias       = mAtmospherePtr->CalcKcasFromKtas(tempAlt_ft, mSpeed_ktas);
      mSpeed_Mach       = mAtmospherePtr->CalcMachFromKtas(tempAlt_ft, mSpeed_ktas);
   }
}

void P6DofKinematicState::SetSpeed_kias(double aSpeed_kias)
{
   mSpeed_kias = aSpeed_kias;

   if (mAtmospherePtr == nullptr)
   {
      mSpeed_fps  = mSpeed_kias * (UtMath::cNM_PER_FT / 3600.0);
      mSpeed_ktas = mSpeed_kias;
      mSpeed_Mach = mSpeed_fps / 1000.0;
   }
   else
   {
      double tempAlt_ft = GetAlt_ft();
      mSpeed_fps        = mAtmospherePtr->CalcFpsFromKcas(tempAlt_ft, mSpeed_kias);
      mSpeed_ktas       = mAtmospherePtr->CalcKtasFromKcas(tempAlt_ft, mSpeed_kias);
      mSpeed_Mach       = mAtmospherePtr->CalcMachFromKcas(tempAlt_ft, mSpeed_kias);
   }
}

void P6DofKinematicState::SetSpeed_Mach(double aSpeed_Mach)
{
   mSpeed_Mach = aSpeed_Mach;

   if (mAtmospherePtr == nullptr)
   {
      mSpeed_fps  = mSpeed_Mach / 1000.0;
      mSpeed_ktas = mSpeed_fps * (3600.0 / UtMath::cNM_PER_FT);
      mSpeed_kias = mSpeed_ktas;
   }
   else
   {
      double tempAlt_ft = GetAlt_ft();
      mSpeed_fps        = mAtmospherePtr->CalcFpsFromMach(tempAlt_ft, mSpeed_Mach);
      mSpeed_ktas       = mAtmospherePtr->CalcKtasFromMach(tempAlt_ft, mSpeed_Mach);
      mSpeed_kias       = mAtmospherePtr->CalcKcasFromMach(tempAlt_ft, mSpeed_Mach);
   }
}

void P6DofKinematicState::SetBodyAccel(double aNx_g, double aNy_g, double aNz_g)
{
   mNx_g = aNx_g;
   mNy_g = aNy_g;
   mNz_g = aNz_g;
}

void P6DofKinematicState::SetLiftDragSideForceThrustWeight(double aLift_lbs,
                                                           double aDrag_lbs,
                                                           double aSideForce_lbs,
                                                           double aThrust_lbs,
                                                           double aWgt_lbs)
{
   mLift_lbs      = aLift_lbs;
   mDrag_lbs      = aDrag_lbs;
   mSideForce_lbs = aSideForce_lbs;
   mThrust_lbs    = aThrust_lbs;
   mWgt_lbs       = aWgt_lbs;
}

void P6DofKinematicState::SetMomentAtCG(UtVec3dX aMoment_ftlbs)
{
   mMoment_ftlbs = aMoment_ftlbs;
}

void P6DofKinematicState::CalculateRates(int64_t aSimTime_nanosec)
{
   if (mLastUpdate_nanosec < 0)
   {
      // First time, just remember the time
      mLastUpdate_nanosec = aSimTime_nanosec;
      return;
   }
   else
   {
      int64_t deltaT_nanosec = aSimTime_nanosec - mLastUpdate_nanosec;

      if (deltaT_nanosec > 0)
      {
         double dT_sec = P6DofUtils::TimeToTime(deltaT_nanosec);

         if (dT_sec > std::numeric_limits<double>::epsilon())
         {
            double deltaAlpha_deg = mAlpha_deg - mLastAlpha_deg;
            double deltaBeta_deg  = mBeta_deg - mLastBeta_deg;

            mAlphaDot_dps = deltaAlpha_deg / dT_sec;
            mBetaDot_dps  = deltaBeta_deg / dT_sec;

            // Remember the time and values
            mLastUpdate_nanosec = aSimTime_nanosec;
            mLastAlpha_deg      = mAlpha_deg;
            mLastBeta_deg       = mBeta_deg;
         }
      }
      else
      {
         // Delta-t is zero, so do nothing
         return;
      }
   }
}

void P6DofKinematicState::CalculateSecondaryParameters(const P6DofAtmosphere* aAtmospherePtr)
{
   // Use WGS or spherical data (mWGS84_pos, mWGS84_vel, mDCM, mOmega and mNx/mNy/mNz) to calculate:
   //   mLat,mLon,mAlt_m,mRadarAlt_m
   //   mVelNorth_mps,mVelEast_mps,mVelDown_mps,
   //   mVertSpeed_fpm,
   //   mLocalHeading_deg,mLocalPitch_deg,mLocalRoll_deg,
   //   mWcsYaw_rad,mWcsPitch_rad,mWcsRoll_rad
   //   mRollRate_dps,mPitchRate_dps,mYawRate_dps
   //   mAlpha_deg,mBeta_deg
   //   mRho,mDynamicPressure_psf,mStaticPressure_psf,mMach
   //   mSpeed_ktas,mSpeed_kias

   // Set the atmosphere if uninitialized
   if (mAtmospherePtr == nullptr)
   {
      mAtmospherePtr = aAtmospherePtr;
   }

   double tempSpeed_fps = 0.0;

   if (UseSphericalEarth())
   {
      // Set LLA
      P6DofUtils::CalcSphericalLatLonAlt(mSpherical_pos, mLat, mLon, mAlt_m);

      // Set radar alt (should be based on ground altitude)
      mRadarAlt_m = mAlt_m;

      // See if we have exceeded the previous apogee
      if ((mAlt_m * 0.001) > mApogee_km)
      {
         mApogee_km = mAlt_m * 0.001;
      }

      // Set NED velocity
      UtVec3dX NEDVec;
      P6DofUtils::CalcSphericalVecToNED(mSpherical_pos, mSpherical_vel, NEDVec);
      double nedVel[3];
      NEDVec.Get(nedVel);
      mVelNorth_mps = nedVel[0];
      mVelEast_mps  = nedVel[1];
      mVelDown_mps  = nedVel[2];

      // Set vertical speed
      mVertSpeed_fpm = -nedVel[2] * UtMath::cFT_PER_M * 60.0;

      // Set gamma
      double vNE = sqrt(mVelNorth_mps * mVelNorth_mps + mVelEast_mps * mVelEast_mps);
      if (fabs(vNE) < 0.00001)
      {
         if (fabs(mVelDown_mps) < 0.00001)
         {
            mFlightPathAngle_rad = 0.0;
         }
         if (mVelDown_mps <= 0.0)
         {
            mFlightPathAngle_rad = UtMath::cPI_OVER_2;
         }
         else
         {
            mFlightPathAngle_rad = -UtMath::cPI_OVER_2;
         }
      }
      else
      {
         mFlightPathAngle_rad = atan2(-mVelDown_mps, vNE);
      }

      // Set the local YPR
      UtVec3dX unitVecX;
      UtVec3dX unitVecY;
      UtVec3dX unitVecZ;
      P6DofUtils::CalcUnitVectors(mDCM, unitVecX, unitVecY, unitVecZ);
      UtVec3dX localNorth;
      UtVec3dX localEast;
      UtVec3dX localDown;
      P6DofUtils::CalcLocalNEDVectors(mSpherical_pos, localNorth, localEast, localDown);
      double localHeading_rad, localPitch_rad, localRoll_rad;
      P6DofUtils::CalcLocalAngles(localNorth, localEast, localDown, unitVecX, unitVecZ, localHeading_rad, localPitch_rad, localRoll_rad);

      mLocalHeading_deg = UtMath::cDEG_PER_RAD * localHeading_rad;
      mLocalPitch_deg   = UtMath::cDEG_PER_RAD * localPitch_rad;
      mLocalRoll_deg    = UtMath::cDEG_PER_RAD * localRoll_rad;

      // Limit yaw
      while (mLocalHeading_deg > 180.0)
      {
         mLocalHeading_deg -= 360.0;
      }
      while (mLocalHeading_deg < -180.0)
      {
         mLocalHeading_deg += 360.0;
      }

      // Limit pitch
      while (mLocalPitch_deg > 90.0)
      {
         mLocalPitch_deg = 90.0;
      }
      while (mLocalPitch_deg < -90.0)
      {
         mLocalPitch_deg = -90.0;
      }

      // Limit roll
      while (mLocalRoll_deg > 180.0)
      {
         mLocalRoll_deg -= 360.0;
      }
      while (mLocalRoll_deg < -180.0)
      {
         mLocalRoll_deg += 360.0;
      }

      // Get the WCS orientation
      P6DofUtils::CalcSphericalWCSAngles(mDCM, mWcsYaw_rad, mWcsPitch_rad, mWcsRoll_rad);

      // Get alpha, beta, and speed
      P6DofUtils::AlphaBetaFromInertialVel(mDCM, mSpherical_vel, tempSpeed_fps, mAlpha_deg, mBeta_deg);
   }
   else
   {
      // Set entity WCS location
      double x, y, z;
      mWGS84_pos.Get(x, y, z);
      double locWCS[3] = {x, y, z};
      mUtEntity.SetLocationWCS(locWCS);

      // Set LLA
      mUtEntity.GetLocationLLA(mLat, mLon, mAlt_m);

      // Set radar alt (should be based on ground altitude)
      mRadarAlt_m = mAlt_m;

      // See if we have exceeded the previous apogee
      if ((mAlt_m * 0.001) > mApogee_km)
      {
         mApogee_km = mAlt_m * 0.001;
      }

      double wcsVel[3];
      mWGS84_vel.Get(wcsVel);
      mUtEntity.SetVelocityWCS(wcsVel);

      double nedVel[3];
      mUtEntity.GetVelocityNED(nedVel);

      // Set NED velocity
      mVelNorth_mps = nedVel[0];
      mVelEast_mps  = nedVel[1];
      mVelDown_mps  = nedVel[2];

      // Set vertical speed
      mVertSpeed_fpm = -nedVel[2] * UtMath::cFT_PER_M * 60.0;

      // Set gamma
      double vNE = sqrt(mVelNorth_mps * mVelNorth_mps + mVelEast_mps * mVelEast_mps);
      if (fabs(vNE) < 0.00001)
      {
         if (fabs(mVelDown_mps) < 0.00001)
         {
            mFlightPathAngle_rad = 0.0;
         }
         if (mVelDown_mps <= 0.0)
         {
            mFlightPathAngle_rad = UtMath::cPI_OVER_2;
         }
         else
         {
            mFlightPathAngle_rad = -UtMath::cPI_OVER_2;
         }
      }
      else
      {
         mFlightPathAngle_rad = atan2(-mVelDown_mps, vNE);
      }

      // Get WCS psi,theta,phi from DCM
      double psi   = 0.0;
      double theta = 0.0;
      double phi   = 0.0;
      mDCM.Get(psi, theta, phi);

      // Set entity orientation
      mUtEntity.SetOrientationWCS(psi, theta, phi);

      // Get the NED orientation
      mUtEntity.GetOrientationNED(mLocalHeading_deg, mLocalPitch_deg, mLocalRoll_deg);
      mLocalHeading_deg *= UtMath::cDEG_PER_RAD;
      mLocalPitch_deg *= UtMath::cDEG_PER_RAD;
      mLocalRoll_deg *= UtMath::cDEG_PER_RAD;

      // Limit yaw
      while (mLocalHeading_deg > 180.0)
      {
         mLocalHeading_deg -= 360.0;
      }
      while (mLocalHeading_deg < -180.0)
      {
         mLocalHeading_deg += 360.0;
      }

      // Limit pitch
      while (mLocalPitch_deg > 90.0)
      {
         mLocalPitch_deg = 90.0;
      }
      while (mLocalPitch_deg < -90.0)
      {
         mLocalPitch_deg = -90.0;
      }

      // Limit roll
      while (mLocalRoll_deg > 180.0)
      {
         mLocalRoll_deg -= 360.0;
      }
      while (mLocalRoll_deg < -180.0)
      {
         mLocalRoll_deg += 360.0;
      }

      // Get the WCS orientation
      mUtEntity.GetOrientationWCS(mWcsYaw_rad, mWcsPitch_rad, mWcsRoll_rad);

      // Get alpha, beta, and speed
      P6DofUtils::AlphaBetaFromInertialVel(mDCM, mWGS84_vel, tempSpeed_fps, mAlpha_deg, mBeta_deg);
   }

   // ............................................................................................

   // Common calculations

   // Get ypr rates
   mOmega.Get(mRollRate_dps, mPitchRate_dps, mYawRate_dps);
   mRollRate_dps *= UtMath::cDEG_PER_RAD;
   mPitchRate_dps *= UtMath::cDEG_PER_RAD;
   mYawRate_dps *= UtMath::cDEG_PER_RAD;

   mSpeed_fps = tempSpeed_fps;

   // Set rho, dynamic/static pressure and mach
   if (aAtmospherePtr != nullptr)
   {
      double tempAlt_ft = GetAlt_ft();

      aAtmospherePtr->GetAeroParametersAtAltitude(tempAlt_ft,
                                                  mSpeed_fps,
                                                  mRho_slugs_per_ft3,
                                                  mDynamicPressure_psf,
                                                  mStaticPressure_psf,
                                                  mSpeed_Mach);

      mSpeed_ktas = aAtmospherePtr->CalcKtasFromFps(mSpeed_fps);
      if (mSpeed_ktas < 0.0)
      {
         mSpeed_ktas = 0.0;
      }

      mSpeed_kias = aAtmospherePtr->CalcKcasFromFps(tempAlt_ft, mSpeed_fps);
      if (mSpeed_kias < 0.0)
      {
         mSpeed_kias = 0.0;
      }
   }
   else
   {
      mRho_slugs_per_ft3   = 0;
      mDynamicPressure_psf = 0;
      mStaticPressure_psf  = 0;
      mSpeed_fps           = 0;
      mSpeed_Mach          = 0;
      mSpeed_ktas          = 0;
      mSpeed_kias          = 0;
   }

   // This supports data needed by GetRangeSinceStart_km
   CalcRangeSinceStart();
}

void P6DofKinematicState::CalcRangeSinceStart()
{
   if (mRefEarthRadius_m < 1.0)
   {
      ut::log::error() << "mRefEarthRadius_m uninitialized in P6DofKinematicState::CalcRangeSinceStart().";
      mRangeSinceStart_km = 0.0;
   }

   double aTargetHeading_r;
   double aTargetDistance_m;
   P6DofUtils::SphericalGreatCircleHeadingAndDistance(mStartingLat, mStartingLon, mLat, mLon, aTargetHeading_r, aTargetDistance_m);
   mRangeSinceStart_km = aTargetDistance_m * 0.001;
}

void P6DofKinematicState::SetRefEarthRadius()
{
   // Set the reference earth radius
   if (UseSphericalEarth())
   {
      mRefEarthRadius_m = gSphericalEarthRadius_m;
   }
   else
   {
      UtEntity startEntity;
      startEntity.SetLocationLLA(mStartingLat, mStartingLon, 0.0);
      double startWCS[3];
      startEntity.GetLocationWCS(startWCS);
      mRefEarthRadius_m = sqrt(startWCS[0] * startWCS[0] + startWCS[1] * startWCS[1] + startWCS[2] * startWCS[2]);
   }
}

UtVec3dX P6DofKinematicState::CalcNEDVecFromBodyVec(const UtVec3dX& aVecBody) const
{
   UtVec3dX vecNED(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      UtVec3dX vecInertial;
      vecInertial = CalcInertialVecFromBodyVec(aVecBody);
      vecNED      = CalcNEDVecFromInertialVec(vecInertial);
   }
   else
   {
      UtVec3dX vecInertial;
      vecInertial = CalcInertialVecFromBodyVec(aVecBody);
      vecNED      = CalcNEDVecFromInertialVec(vecInertial);
   }

   return vecNED;
}

UtVec3dX P6DofKinematicState::CalcNEDVecFromInertialVec(const UtVec3dX& aVecInertial) const
{
   UtVec3dX vecNED(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      P6DofUtils::CalcSphericalVecToNED(mSpherical_pos, aVecInertial, vecNED);
   }
   else
   {
      return P6DofUtils::CalcWGS84VecToNED(mWGS84_pos, aVecInertial);
   }

   return vecNED;
}

UtVec3dX P6DofKinematicState::CalcInertialVecFromBodyVec(const UtVec3dX& aVecBody) const
{
   UtVec3dX vecInertial(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      vecInertial = mDCM.InverseTransform(aVecBody);
   }
   else
   {
      vecInertial = mDCM.InverseTransform(aVecBody);
   }

   return vecInertial;
}

UtVec3dX P6DofKinematicState::CalcInertialVecFromNEDVec(const UtVec3dX& aVecNED) const
{
   UtVec3dX vecInertial(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      UtVec3dX vecBody;
      vecBody     = CalcBodyVecFromNEDVec(aVecNED);
      vecInertial = CalcInertialVecFromBodyVec(vecBody);
   }
   else
   {
      UtVec3dX vecBody;
      vecBody     = CalcBodyVecFromNEDVec(aVecNED);
      vecInertial = CalcInertialVecFromBodyVec(vecBody);
   }

   return vecInertial;
}

UtVec3dX P6DofKinematicState::CalcBodyVecFromNEDVec(const UtVec3dX& aVecNED) const
{
   UtVec3dX vecBody(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      double heading_rad;
      double pitch_rad;
      double roll_rad;
      P6DofUtils::GetSphericalNEDAngles(mDCM, mSpherical_pos, heading_rad, pitch_rad, roll_rad);

      UtDCM tempDCM(heading_rad, pitch_rad, roll_rad);

      // Get body unit vectors in NED
      UtVec3dX unitVecNEDX(1.0, 0.0, 0.0);
      unitVecNEDX = tempDCM.InverseTransform(unitVecNEDX);
      UtVec3dX unitVecNEDY(0.0, 1.0, 0.0);
      unitVecNEDY = tempDCM.InverseTransform(unitVecNEDY);
      UtVec3dX unitVecNEDZ(0.0, 0.0, 1.0);
      unitVecNEDZ = tempDCM.InverseTransform(unitVecNEDZ);

      double xComponent = unitVecNEDX.Dot(aVecNED);
      double yComponent = unitVecNEDY.Dot(aVecNED);
      double zComponent = unitVecNEDZ.Dot(aVecNED);

      vecBody.Set(xComponent, yComponent, zComponent);
   }
   else
   {
      UtEntity tempEntity;
      double   posVec[3];
      mWGS84_pos.Get(posVec);
      tempEntity.SetLocationWCS(posVec);
      tempEntity.SetOrientationNED(mLocalHeading_deg * UtMath::cRAD_PER_DEG,
                                   mLocalPitch_deg * UtMath::cRAD_PER_DEG,
                                   mLocalRoll_deg * UtMath::cRAD_PER_DEG);
      double vecNED[3];
      aVecNED.Get(vecNED);
      tempEntity.SetVelocityNED(vecNED);
      double vecBodyTemp[3];
      tempEntity.GetVelocityECS(vecBodyTemp);

      vecBody.Set(vecBodyTemp);
   }

   return vecBody;
}

UtVec3dX P6DofKinematicState::CalcBodyVecFromInertialVec(const UtVec3dX& aVecInertial) const
{
   UtVec3dX vecBody(0., 0., 0.);

   if (mUseSphericalEarth)
   {
      vecBody = mDCM.Transform(aVecInertial);
   }
   else
   {
      vecBody = mDCM.Transform(aVecInertial);
   }

   return vecBody;
}

void P6DofKinematicState::SetTheLastValues()
{
   mLastNx                  = mNx_g;
   mLastNy                  = mNy_g;
   mLastNz                  = mNz_g;
   mLastDynamicPressure_psf = mDynamicPressure_psf;
   mLastStaticPressure_psf  = mStaticPressure_psf;
   mLastAlt_m               = mAlt_m;
}

double P6DofKinematicState::GetAlpha_deg() const
{
   return mAlpha_deg;
}

double P6DofKinematicState::GetAlpha_rad() const
{
   return mAlpha_deg * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetBeta_rad() const
{
   return mBeta_deg * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetBeta_deg() const
{
   return mBeta_deg;
}

double P6DofKinematicState::GetAlphaDot_dps() const
{
   return mAlphaDot_dps;
}

double P6DofKinematicState::GetAlphaDot_rps() const
{
   return mAlphaDot_dps * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetBetaDot_dps() const
{
   return mBetaDot_dps;
}

double P6DofKinematicState::GetBetaDot_rps() const
{
   return mBetaDot_dps * UtMath::cRAD_PER_DEG;
}

void P6DofKinematicState::SetAlphaBetaDotToZero()
{
   mAlphaDot_dps  = 0.0;
   mLastAlpha_deg = mAlpha_deg;

   mBetaDot_dps  = 0.0;
   mLastBeta_deg = mBeta_deg;
}

UtLLAPos P6DofKinematicState::GetCurrentPositionLLA() const
{
   UtLLAPos posLLA(mLat, mLon, mAlt_m);
   return posLLA;
}

double P6DofKinematicState::GetSpeed_mps() const
{
   return mSpeed_fps * UtMath::cM_PER_FT;
}

double P6DofKinematicState::GetLocalHeading_rad() const
{
   return mLocalHeading_deg * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetLocalPitch_rad() const
{
   return mLocalPitch_deg * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetLocalRoll_rad() const
{
   return mLocalRoll_deg * UtMath::cRAD_PER_DEG;
}

double P6DofKinematicState::GetFlightPathAngle_deg() const
{
   return mFlightPathAngle_rad * UtMath::cDEG_PER_RAD;
}

double P6DofKinematicState::GetLastNx_g() const
{
   return mLastNx;
}

double P6DofKinematicState::GetLastNy_g() const
{
   return mLastNy;
}

double P6DofKinematicState::GetLastNz_g() const
{
   return mLastNz;
}

double P6DofKinematicState::GetLat() const
{
   return mLat;
}

double P6DofKinematicState::GetLon() const
{
   return mLon;
}

double P6DofKinematicState::GetAlt_m() const
{
   return mAlt_m;
}

double P6DofKinematicState::GetAlt_ft() const
{
   return mAlt_m * UtMath::cFT_PER_M;
}

double P6DofKinematicState::GetNx_g() const
{
   return mNx_g;
}

double P6DofKinematicState::GetNy_g() const
{
   return mNy_g;
}

double P6DofKinematicState::GetNz_g() const
{
   return mNz_g;
}

void P6DofKinematicState::SetOrientationWCS(double aYaw_rad, double aPitch_rad, double aRoll_rad)
{
   mWcsYaw_rad   = aYaw_rad;
   mWcsPitch_rad = aPitch_rad;
   mWcsRoll_rad  = aRoll_rad;
}

double P6DofKinematicState::GetYawRate_rps() const
{
   return mOmega.Z();
}

double P6DofKinematicState::GetPitchRate_rps() const
{
   return mOmega.Y();
}

double P6DofKinematicState::GetRollRate_rps() const
{
   return mOmega.X();
}

double P6DofKinematicState::GetYawRate_dps() const
{
   return mOmega.Z() * UtMath::cDEG_PER_RAD;
}

double P6DofKinematicState::GetPitchRate_dps() const
{
   return mOmega.Y() * UtMath::cDEG_PER_RAD;
}

double P6DofKinematicState::GetRollRate_dps() const
{
   return mOmega.X() * UtMath::cDEG_PER_RAD;
}

void P6DofKinematicState::SetYawRate(double aYawRate_dps)
{
   mOmega.Set(mOmega.X(), mOmega.Y(), aYawRate_dps * UtMath::cDEG_PER_RAD);
}

void P6DofKinematicState::SetPitchRate(double aPitchRate_dps)
{
   mOmega.Set(mOmega.X(), aPitchRate_dps * UtMath::cDEG_PER_RAD, mOmega.Z());
}

void P6DofKinematicState::SetRollRate(double aRollRate_dps)
{
   mOmega.Set(aRollRate_dps * UtMath::cDEG_PER_RAD, mOmega.Y(), mOmega.Z());
}

double P6DofKinematicState::GetLastDynamicPressure_psf() const
{
   return mLastDynamicPressure_psf;
}

double P6DofKinematicState::GetLastStaticPressure_psf() const
{
   return mLastStaticPressure_psf;
}

double P6DofKinematicState::GetLastAltitudeMSL_ft() const
{
   return mLastAlt_m;
}

bool P6DofKinematicState::CalculateLLAofRelativePoint(const P6DofTerrain*        aTerrainPtr,
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
                                                      double&                    aCompression_m)
{
   // Clear and/or set default values
   aCompressionVectorNED.Set(0.0, 0.0, 0.0);
   aRollingVectorInSurfaceNED.Set(0.0, 0.0, 0.0);
   aNormalVectorNED.Set(0.0, 0.0, -1.0);
   aGearVelNED_mps.Set(0.0, 0.0, 0.0);
   aSurfaceSpeedVectorNED_mps.Set(0.0, 0.0, 0.0);
   aFrictionMultiplier = 1.0;
   aCompression_m      = 0.0;

   // If we lack terrain, just return
   if (aTerrainPtr == nullptr)
   {
      return false;
   }

   if (aKinematicState.UseSphericalEarth())
   {
      // Calc LLA of relative point
      UtVec3dX pointSpherical = aKinematicState.mSpherical_pos + aKinematicState.mDCM.InverseTransform(aBodyRelPos_m);
      P6DofUtils::CalcSphericalLatLonAlt(pointSpherical, aLat, aLon, aAlt_m);

      // Get terrain parameters
      double terrainHgt_m =
         aTerrainPtr->HeightOfTerrain_m(aLat, aLon, aNormalVectorNED, aSurfaceSpeedVectorNED_mps, aFrictionMultiplier);
      aHeightAboveTerrain_m = aAlt_m - terrainHgt_m;

      aCompression_m = terrainHgt_m - aAlt_m;
      if (aCompression_m < 0.0)
      {
         aCompression_m = 0.0;
      }

      // CalcNEDVecFromBodyVec(aBodyRelCompressionVector,aCompressionVectorNED);
      aCompressionVectorNED = aKinematicState.CalcNEDVecFromBodyVec(aBodyRelCompressionVector);
      aCompressionVectorNED.Normalize();

      UtVec3dX rollingVectorNED;
      rollingVectorNED = aKinematicState.CalcNEDVecFromBodyVec(aBodyRelRollingVector);
      rollingVectorNED.Normalize();

      // Note - At this point, the rolling vector has been transformed from body coords
      // to NED coords. However, the NED may contain some Z component (for example, if
      // the aircraft was pitched). Yet, for a rolling vector in NED, we do not want
      // any 'Z' to be present, unless the ground normal is (0,0,-1). We need to get a
      // rolling vector that is perpendicular to the surface normal. To get this, we
      // take the cross product of the rolling vec and the normal (to get a vec to the
      // side) and then use that vector and cross the normal with it to get the rolling
      // vector in the surface plane.
      UtVec3dX tempSideVec       = rollingVectorNED.Cross(aNormalVectorNED);
      aRollingVectorInSurfaceNED = aNormalVectorNED.Cross(tempSideVec);
      aRollingVectorInSurfaceNED.Normalize();

      // Calc induced vel for gear
      UtVec3dX inducedVelBodyVec_mps     = aKinematicState.mOmega.Cross(aBodyRelPos_m);
      UtVec3dX inducedVelInertialVec_mps = aKinematicState.mDCM.InverseTransform(inducedVelBodyVec_mps);
      UtVec3dX totalGearVelInertial_mps  = aKinematicState.mSpherical_vel + inducedVelInertialVec_mps;

      // Get gear vel in NED
      aGearVelNED_mps = aKinematicState.CalcNEDVecFromInertialVec(totalGearVelInertial_mps);
   }
   else
   {
      UtVec3dX pointWCS = aKinematicState.mWGS84_pos + aKinematicState.mDCM.InverseTransform(aBodyRelPos_m);

      // Set entity WCS location
      double locWCS[3];
      pointWCS.Get(locWCS);
      UtEntity tempEntity;
      tempEntity.SetLocationWCS(locWCS);

      // Get LLA
      tempEntity.GetLocationLLA(aLat, aLon, aAlt_m);

      // Get terrain parameters
      double terrainHgt_m =
         aTerrainPtr->HeightOfTerrain_m(aLat, aLon, aNormalVectorNED, aSurfaceSpeedVectorNED_mps, aFrictionMultiplier);
      aHeightAboveTerrain_m = aAlt_m - terrainHgt_m;

      aCompression_m = terrainHgt_m - aAlt_m;
      if (aCompression_m < 0.0)
      {
         aCompression_m = 0.0;
      }

      aCompressionVectorNED = aKinematicState.CalcNEDVecFromBodyVec(aBodyRelCompressionVector);
      aCompressionVectorNED.Normalize();

      UtVec3dX rollingVectorNED;
      rollingVectorNED = aKinematicState.CalcNEDVecFromBodyVec(aBodyRelRollingVector);
      rollingVectorNED.Normalize();

      // Note - At this point, the rolling vector has been transformed from body coords
      // to NED coords. However, the NED may contain some Z component (for example, if
      // the aircraft was pitched). Yet, for a rolling vector in NED, we do not want
      // any 'Z' to be present, unless the ground normal is (0,0,-1). We need to get a
      // rolling vector that is perpendicular to the surface normal. To get this, we
      // take the cross product of the rolling vec and the normal (to get a vec to the
      // side) and then use that vector and cross the normal with it to get the rolling
      // vector in the surface plane.
      UtVec3dX tempSideVec       = rollingVectorNED.Cross(aNormalVectorNED);
      aRollingVectorInSurfaceNED = aNormalVectorNED.Cross(tempSideVec);
      aRollingVectorInSurfaceNED.Normalize();

      // Calc induced vel for gear
      UtVec3dX inducedVelBodyVec_mps     = aKinematicState.mOmega.Cross(aBodyRelPos_m);
      UtVec3dX inducedVelInertialVec_mps = aKinematicState.mDCM.InverseTransform(inducedVelBodyVec_mps);
      UtVec3dX totalGearVelInertial_mps  = aKinematicState.mWGS84_vel + inducedVelInertialVec_mps;

      aGearVelNED_mps = aKinematicState.CalcNEDVecFromInertialVec(totalGearVelInertial_mps);
   }

   return true;
}

UtVec3dX P6DofKinematicState::NormalizedGravitationalAccelVec(const P6DofGravity* aGravityPtr,
                                                              const double        aLat,
                                                              const double        aLon,
                                                              const double        aAlt_m,
                                                              const bool          aUseSphericalEarth)
{
   UtVec3dX gravityVec(0.0, 0.0, 0.0);

   if (aGravityPtr != nullptr)
   {
      if (aUseSphericalEarth)
      {
         gravityVec = aGravityPtr->NormalizedGravitationalAccel(aLat, aLon, aAlt_m, true);
      }
      else
      {
         gravityVec = aGravityPtr->NormalizedGravitationalAccel(aLat, aLon, aAlt_m, false);
      }
   }

   return gravityVec;
}

void P6DofKinematicState::UpdateAeroState(const P6DofAtmosphere* aAtmospherePtr,
                                          const P6DofWind*, // aWindPtr not used yet
                                          const int64_t,    // aSimTime_nanosec not used yet
                                          const double aDeltaT_sec)
{
   mAtmospherePtr = aAtmospherePtr;

   double tempSpeed_fps = 0;
   double tempAlpha_deg = 0;
   double tempBeta_deg  = 0;
   double tempAlt_ft    = GetAlt_ft();

   // Set the temporary velocity vector, based on coordinate reference
   UtVec3dX tempInertialVelVector;
   if (mUseSphericalEarth)
   {
      tempInertialVelVector = mSpherical_vel;
   }
   else
   {
      tempInertialVelVector = mWGS84_vel;
   }

   P6DofUtils::AlphaBetaFromInertialVel(mDCM, tempInertialVelVector, tempSpeed_fps, tempAlpha_deg, tempBeta_deg);

   // Remember the last values
   double lastAlpha_deg = mAlpha_deg;
   double lastBeta_deg  = mBeta_deg;

   // Update to the new values
   mAlpha_deg = tempAlpha_deg;
   mBeta_deg  = tempBeta_deg;

   // Calc alpha dot and beta dot values based on aDeltaT_sec
   if (aDeltaT_sec < P6DofUtils::cEPSILON_SIMTIME_SEC)
   {
      // Leave the mAlphaDot_dps and mBetaDot_dps values untouched
   }
   else
   {
      mAlphaDot_dps = (mAlpha_deg - lastAlpha_deg) / aDeltaT_sec;
      mBetaDot_dps  = (mBeta_deg - lastBeta_deg) / aDeltaT_sec;
   }

   if (mAtmospherePtr != nullptr)
   {
      mAtmospherePtr->GetAeroParametersAtAltitude(tempAlt_ft,
                                                  tempSpeed_fps,
                                                  mRho_slugs_per_ft3,
                                                  mDynamicPressure_psf,
                                                  mStaticPressure_psf,
                                                  mSpeed_Mach);
   }
   else
   {
      mRho_slugs_per_ft3   = 0;
      mDynamicPressure_psf = 0;
      mStaticPressure_psf  = 0;
      mSpeed_Mach          = 0;
   }

   mSpeed_fps  = tempSpeed_fps;
   mSpeed_ktas = aAtmospherePtr->CalcKtasFromFps(tempSpeed_fps);
   mSpeed_kias = aAtmospherePtr->CalcKcasFromFps(tempAlt_ft, tempSpeed_fps);
}

void P6DofKinematicState::GetAngleDeltas(double& aDeltaYaw_rad, double& aDeltaPitch_rad, double& aDeltaRoll_rad) const
{
   aDeltaYaw_rad   = mDeltaYaw_rad;
   aDeltaPitch_rad = mDeltaPitch_rad;
   aDeltaRoll_rad  = mDeltaRoll_rad;
}

void P6DofKinematicState::SetAngleDeltas(double aDeltaYaw_rad, double aDeltaPitch_rad, double aDeltaRoll_rad)
{
   mDeltaYaw_rad   = aDeltaYaw_rad;
   mDeltaPitch_rad = aDeltaPitch_rad;
   mDeltaRoll_rad  = aDeltaRoll_rad;
}

void P6DofKinematicState::RemoveAlphaForTesting(P6DofAtmosphere* aAtmospherePtr)
{
   // Adjust DCM with ypr deltas
   // UtDCM yawDCM(deltaYaw_rad,0.0,0.0);
   // UtDCM pitchDCM(0.0,deltaPitch_rad,0.0);
   // UtDCM rollDCM(0.0,0.0,deltaRoll_rad);
   // mDCM = rollDCM * (pitchDCM * (yawDCM * mDCM));

   UtDCM pitchDCM(0.0, -GetAlpha_rad(), 0.0);
   mDCM = pitchDCM * mDCM;

   CalculateSecondaryParameters(aAtmospherePtr);
}

UtVec3dX P6DofKinematicState::GetInertialVelocity() const
{
   if (UseSphericalEarth())
   {
      return mSpherical_vel;
   }
   else
   {
      return mWGS84_vel;
   }
}

void P6DofKinematicState::SetVelocityNED(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   if (mUseSphericalEarth)
   {
      double   nedVec[3] = {aVelNorth_mps, aVelEast_mps, aVelDown_mps};
      UtVec3dX NEDVec(nedVec);
      P6DofUtils::CalcSphericalNEDVelToSphericalVel(mSpherical_pos, NEDVec, mSpherical_vel);

      mVelNorth_mps = aVelNorth_mps;
      mVelEast_mps  = aVelEast_mps;
      mVelDown_mps  = aVelDown_mps;
   }
   else
   {
      UtEntity entity;
      double   wcsLoc[3];
      mWGS84_pos.Get(wcsLoc);
      entity.SetLocationWCS(wcsLoc);
      double nedVel[3] = {aVelNorth_mps, aVelEast_mps, aVelDown_mps};
      entity.SetVelocityNED(nedVel);

      double wcsVel[3];
      entity.GetVelocityWCS(wcsVel);
      mWGS84_vel.Set(wcsVel);

      mVelNorth_mps = aVelNorth_mps;
      mVelEast_mps  = aVelEast_mps;
      mVelDown_mps  = aVelDown_mps;
   }
}

UtVec3dX P6DofKinematicState::GetVelocityNED_mps() const
{
   UtVec3dX NEDVel_mps;

   if (mUseSphericalEarth)
   {
      P6DofUtils::CalcSphericalVecToNED(mSpherical_pos, mSpherical_vel, NEDVel_mps);
   }
   else
   {
      UtEntity entity;

      double wcsPos[3] = {mWGS84_pos.X(), mWGS84_pos.Y(), mWGS84_pos.Z()};
      double wcsVel[3] = {mWGS84_vel.X(), mWGS84_vel.Y(), mWGS84_vel.Z()};

      entity.SetLocationWCS(wcsPos);
      entity.SetVelocityWCS(wcsVel);

      double velNED[3];
      entity.GetVelocityNED(velNED);
      NEDVel_mps.Set(velNED[0], velNED[1], velNED[2]);
   }

   return NEDVel_mps;
}

void P6DofKinematicState::GetBodyAeroVectors(const UtDCM&    aDCM,
                                             const UtVec3dX& aInertialVelVector,
                                             UtVec3dX&       aLiftBodyVector,
                                             UtVec3dX&       aDragBodyVector,
                                             UtVec3dX&       aSideForceBodyVector) const
{
   UtVec3dX velBody = aDCM.Transform(aInertialVelVector);
   aDragBodyVector  = -velBody;
   aDragBodyVector.Normalize();

   UtVec3dX yVector(0.0, 1.0, 0.0);

   aLiftBodyVector = yVector.Cross(velBody);
   aLiftBodyVector.Normalize();

   aSideForceBodyVector = aLiftBodyVector.Cross(aDragBodyVector);
   aSideForceBodyVector.Normalize();
}

void P6DofKinematicState::ZeroVelocity()
{
   double vel[3] = {0.0, 0.0, 0.0};
   if (UseSphericalEarth())
   {
      mSpherical_vel.Set(vel);
   }
   else
   {
      mWGS84_vel.Set(vel);
   }
}

UtVec3dX P6DofKinematicState::GetOmegaBodyDPS() const
{
   return mOmega * UtMath::cDEG_PER_RAD;
}

void P6DofKinematicState::SetStartingLatLon(double aLat, double aLon)
{
   mStartingLocationInitialized = true;
   mStartingLat                 = aLat;
   mStartingLon                 = aLon;
}

void P6DofKinematicState::GetStartingLatLon(double& aLat, double& aLon) const
{
   aLat = mStartingLat;
   aLon = mStartingLon;
}

void P6DofKinematicState::ZeroKinematicsDuringTesting()
{
   mInertialAccel.Set(0.0, 0.0, 0.0);
   mOmega.Set(0.0, 0.0, 0.0);
   mOmegaDot.Set(0.0, 0.0, 0.0);

   ZeroVelocity();
   SetSpeed_fps(0.0);

   mNx_g = 0.0;
   mNy_g = 0.0;
   mNz_g = 0.0;

   mAlpha_deg    = 0.0;
   mBeta_deg     = 0.0;
   mAlphaDot_dps = 0.0;
   mBetaDot_dps  = 0.0;

   mLastAlpha_deg = 0.0;
   mLastBeta_deg  = 0.0;
   mLastNx        = 0.0;
   mLastNy        = 0.0;
   mLastNz        = 0.0;

   mYawRate_dps      = 0.0;
   mPitchRate_dps    = 0.0;
   mRollRate_dps     = 0.0;
   mDeltaYaw_rad     = 0.0;
   mDeltaPitch_rad   = 0.0;
   mDeltaRoll_rad    = 0.0;
   mLocalHeading_deg = 0.0;
   mLocalPitch_deg   = 0.0;
   mLocalRoll_deg    = 0.0;

   mVertSpeed_fpm       = 0.0;
   mFlightPathAngle_rad = 0.0;

   mLift_lbs      = 0.0;
   mSideForce_lbs = 0.0;
   mDrag_lbs      = 0.0;
   mThrust_lbs    = 0.0;
   mMoment_ftlbs.Set(0.0, 0.0, 0.0);
}
