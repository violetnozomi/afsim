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

#include "WsfSixDOF_KinematicState.hpp"

#include "UtEllipsoidalCentralBody.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_Environment.hpp"
#include "WsfSixDOF_Terrain.hpp"

wsf::six_dof::KinematicState::KinematicState(const Environment* aEnvironmentPtr)
   : mEnvironment(aEnvironmentPtr)
{
}

void wsf::six_dof::KinematicState::SetUseSphericalEarth(bool aValue)
{
   mUseSphericalEarth = aValue;

   if (aValue)
   {
      mUtEntity.SetCentralPoint(SixDOF_SphericalEarth());
   }
   else
   {
      mUtEntity.SetCentralPoint(ut::EarthWGS84());
   }
}

void wsf::six_dof::KinematicState::SetPositionLLA(double aLat, double aLon, double aAlt_m)
{
   mUtEntity.SetLocationLLA(aLat, aLon, aAlt_m);
}

void wsf::six_dof::KinematicState::SetAttitude_rad(double aHeading_rad, double aPitch_rad, double aRoll_rad)
{
   mUtEntity.SetOrientationNED(aHeading_rad, aPitch_rad, aRoll_rad);
}

void wsf::six_dof::KinematicState::SetSpeed_fps(double aSpeed_fps, double aBearing_deg, double aFlightPathAngle_deg)
{
   SetSpeed_mps(aSpeed_fps * UtMath::cM_PER_FT, aBearing_deg, aFlightPathAngle_deg);
}

void wsf::six_dof::KinematicState::SetSpeed_mps(double aSpeed_mps, double aBearing_deg, double aFlightPathAngle_deg)
{
   double flightPathAngle_rad = aFlightPathAngle_deg * UtMath::cRAD_PER_DEG;
   double bearing_rad         = aBearing_deg * UtMath::cRAD_PER_DEG;

   UtVec3dX velocityNED = UtVec3dX(bearing_rad, flightPathAngle_rad, aSpeed_mps, false);

   mUtEntity.SetVelocityNED(velocityNED.GetData());
}

void wsf::six_dof::KinematicState::SetSpeed_mph(double aSpeed_mph, double aBearing_deg, double aFlightPathAngle_deg)
{
   SetSpeed_mps(aSpeed_mph * UtMath::cMPS_PER_MPH, aBearing_deg, aFlightPathAngle_deg);
}

void wsf::six_dof::KinematicState::SetSpeed_ktas(double aSpeed_ktas, double aBearing_deg, double aFlightPathAngle_deg)
{
   if (mEnvironment == nullptr)
   {
      return;
   }

   double speed_fps = UtMath::cFPS_PER_NMPH * aSpeed_ktas;
   SetSpeed_fps(speed_fps, aBearing_deg, aFlightPathAngle_deg);
}

void wsf::six_dof::KinematicState::SetSpeed_kias(double aSpeed_kias, double aBearing_deg, double aFlightPathAngle_deg)
{
   if (mEnvironment == nullptr)
   {
      return;
   }

   double speed_fps = mEnvironment->CalcFpsFromKcas(GetAlt_m(), aSpeed_kias);
   SetSpeed_fps(speed_fps, aBearing_deg, aFlightPathAngle_deg);
}

void wsf::six_dof::KinematicState::SetSpeed_Mach(double aSpeed_Mach, double aBearing_deg, double aFlightPathAngle_deg)
{
   if (mEnvironment == nullptr)
   {
      return;
   }

   double speed_fps = mEnvironment->CalcFpsFromMach(GetAlt_m(), aSpeed_Mach);
   SetSpeed_fps(speed_fps, aBearing_deg, aFlightPathAngle_deg);
}

void wsf::six_dof::KinematicState::SetBodyAccel(double aNx_g, double aNy_g, double aNz_g)
{
   mNx_g = aNx_g;
   mNy_g = aNy_g;
   mNz_g = aNz_g;
}

void wsf::six_dof::KinematicState::SetLiftDragSideForceThrustWeight(double aLift_lbs,
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

void wsf::six_dof::KinematicState::SetMomentAtCG(UtVec3dX aMoment_ftlbs)
{
   mMoment_ftlbs = aMoment_ftlbs;
}

void wsf::six_dof::KinematicState::CalculateSecondaryParameters()
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

   double tempSpeed_fps = 0.0;

   // Set radar alt (should be based on ground altitude)
   double altitude_m = GetAlt_m();
   mRadarAlt_m       = altitude_m;

   // See if we have exceeded the previous apogee
   if ((altitude_m * 0.001) > mApogee_km)
   {
      mApogee_km = altitude_m * 0.001;
   }

   // Get WCS psi,theta,phi from DCM
   double psi   = 0.0;
   double theta = 0.0;
   double phi   = 0.0;
   mDCM.Get(psi, theta, phi);

   // Set entity orientation
   mUtEntity.SetOrientationWCS(psi, theta, phi);

   // Get alpha, beta, and speed
   utils::AlphaBetaFromInertialVel(mDCM, GetVelocityWCS(), tempSpeed_fps, mAlpha_deg, mBeta_deg);

   // ............................................................................................

   // Common calculations

   // Get ypr rates
   mOmega.Get(mRollRate_dps, mPitchRate_dps, mYawRate_dps);
   mRollRate_dps *= UtMath::cDEG_PER_RAD;
   mPitchRate_dps *= UtMath::cDEG_PER_RAD;
   mYawRate_dps *= UtMath::cDEG_PER_RAD;

   // This supports data needed by GetRangeSinceStart_km
   CalcRangeSinceStart();

   if (mEnvironment == nullptr)
   {
      return;
   }

   // Set rho, dynamic/static pressure and mach
   mEnvironment->GetAeroParameters(GetAlt_ft(), GetSpeed_fps(), mRho_slugs_per_ft3, mDynamicPressure_psf, mStaticPressure_psf);
}

void wsf::six_dof::KinematicState::CalcRangeSinceStart()
{
   double initialHeading_r;
   double finalHeading_r;
   double distance_m;

   distance_m = ut::EllipsoidalCentralBody::GetVincentyDistance(mStartingLat,
                                                                mStartingLon,
                                                                GetLat(),
                                                                GetLon(),
                                                                mUtEntity.GetCentralBody().GetEllipsoid(),
                                                                initialHeading_r,
                                                                finalHeading_r);

   mRangeSinceStart_km = distance_m * 0.001;
}

UtVec3dX wsf::six_dof::KinematicState::CalcNEDVecFromBodyVec(const UtVec3dX& aVecBody) const
{
   UtVec3dX vecInertial = CalcWCSVecFromBodyVec(aVecBody);
   UtVec3dX vecNED      = CalcNEDVecFromWCSVec(vecInertial);

   return vecNED;
}

UtVec3dX wsf::six_dof::KinematicState::CalcNEDVecFromWCSVec(const UtVec3dX& aVecInertial) const
{
   UtVec3dX vecNED(0., 0., 0.);
   mUtEntity.ConvertWCSVectorToNED(vecNED.GetData(), aVecInertial.GetData());
   return vecNED;
}

UtVec3dX wsf::six_dof::KinematicState::CalcWCSVecFromBodyVec(const UtVec3dX& aVecBody) const
{
   return mDCM.InverseTransform(aVecBody);
}

UtVec3dX wsf::six_dof::KinematicState::CalcWCSVecFromNEDVec(const UtVec3dX& aVecNED) const
{
   UtVec3dX vecInertial(0., 0., 0.);
   mUtEntity.ConvertNEDVectorToWCS(vecInertial.GetData(), aVecNED.GetData());
   return vecInertial;
}

UtVec3dX wsf::six_dof::KinematicState::CalcBodyVecFromNEDVec(const UtVec3dX& aVecNED) const
{
   UtVec3dX vecBody(0., 0., 0.);
   double   vecInertial[3];
   mUtEntity.ConvertNEDVectorToWCS(vecInertial, aVecNED.GetData());
   mUtEntity.ConvertWCSVectorToECS(vecBody.GetData(), vecInertial);
   return vecBody;
}

UtVec3dX wsf::six_dof::KinematicState::CalcBodyVecFromWCSVec(const UtVec3dX& aVecInertial) const
{
   return mDCM.Transform(aVecInertial);
}

void wsf::six_dof::KinematicState::SetTheLastValues()
{
   mLastNx                  = mNx_g;
   mLastNy                  = mNy_g;
   mLastNz                  = mNz_g;
   mLastDynamicPressure_psf = mDynamicPressure_psf;
   mLastStaticPressure_psf  = mStaticPressure_psf;
   mLastAlt_m               = GetAlt_m();
}

// This returns the current speed in m/sec
double wsf::six_dof::KinematicState::GetSpeed_mps() const
{
   return GetVelocityWCS().Magnitude();
}

// This returns the current speed in ft/sec
double wsf::six_dof::KinematicState::GetSpeed_fps() const
{
   return GetSpeed_mps() * UtMath::cFT_PER_M;
}

// This returns the current speed in KTAS (knots true airspeed)
double wsf::six_dof::KinematicState::GetSpeed_ktas() const
{
   if (mEnvironment == nullptr)
   {
      return 0.0;
   }

   return UtMath::cNMPH_PER_FPS * GetSpeed_fps();
}

// This returns the current speed in KIAS/KCAS (knots indicated/calibrated airspeed)
double wsf::six_dof::KinematicState::GetSpeed_kias() const
{
   if (mEnvironment == nullptr)
   {
      return 0.0;
   }

   return mEnvironment->CalcKcasFromFps(GetAlt_m(), GetSpeed_fps());
}

// This returns the current speed in Mach
double wsf::six_dof::KinematicState::GetSpeed_Mach() const
{
   if (mEnvironment == nullptr)
   {
      return 0.0;
   }

   return mEnvironment->CalcMachFromFps(GetAlt_m(), GetSpeed_fps());
}

double wsf::six_dof::KinematicState::GetAlpha_deg() const
{
   return mAlpha_deg;
}

double wsf::six_dof::KinematicState::GetAlpha_rad() const
{
   return mAlpha_deg * UtMath::cRAD_PER_DEG;
}

double wsf::six_dof::KinematicState::GetBeta_rad() const
{
   return mBeta_deg * UtMath::cRAD_PER_DEG;
}

double wsf::six_dof::KinematicState::GetBeta_deg() const
{
   return mBeta_deg;
}

double wsf::six_dof::KinematicState::GetAlphaDot_dps() const
{
   return mAlphaDot_dps;
}

double wsf::six_dof::KinematicState::GetAlphaDot_rps() const
{
   return mAlphaDot_dps * UtMath::cRAD_PER_DEG;
}

double wsf::six_dof::KinematicState::GetBetaDot_dps() const
{
   return mBetaDot_dps;
}

double wsf::six_dof::KinematicState::GetBetaDot_rps() const
{
   return mBetaDot_dps * UtMath::cRAD_PER_DEG;
}

void wsf::six_dof::KinematicState::SetAlphaBetaDotToZero()
{
   mAlphaDot_dps  = 0.0;
   mLastAlpha_deg = mAlpha_deg;

   mBetaDot_dps  = 0.0;
   mLastBeta_deg = mBeta_deg;
}

UtLLAPos wsf::six_dof::KinematicState::GetCurrentPositionLLA() const
{
   double lat;
   double lon;
   double alt;
   mUtEntity.GetLocationLLA(lat, lon, alt);

   UtLLAPos posLLA(lat, lon, alt);
   return posLLA;
}

double wsf::six_dof::KinematicState::GetLocalHeading_rad() const
{
   double heading_rad;
   double pitch_rad;
   double roll_rad;

   mUtEntity.GetOrientationNED(heading_rad, pitch_rad, roll_rad);

   return heading_rad;
}

double wsf::six_dof::KinematicState::GetLocalPitch_rad() const
{
   double heading_rad;
   double pitch_rad;
   double roll_rad;

   mUtEntity.GetOrientationNED(heading_rad, pitch_rad, roll_rad);

   return pitch_rad;
}

double wsf::six_dof::KinematicState::GetLocalRoll_rad() const
{
   double heading_rad;
   double pitch_rad;
   double roll_rad;

   mUtEntity.GetOrientationNED(heading_rad, pitch_rad, roll_rad);

   return roll_rad;
}

double wsf::six_dof::KinematicState::GetFlightPathAngle_rad() const
{
   double flightPathAngle_rad;

   double nedVel[3];
   mUtEntity.GetVelocityNED(nedVel);

   double velocityNorth = nedVel[0];
   double velocityEast  = nedVel[1];
   double velocityDown  = nedVel[2];

   // Set gamma
   double velocityHorizontal = sqrt(velocityNorth * velocityNorth + velocityEast * velocityEast);
   if (UtMath::NearlyZero(velocityHorizontal))
   {
      if (UtMath::NearlyZero(velocityDown))
      {
         flightPathAngle_rad = 0.0;
      }
      else if (velocityDown <= 0.0)
      {
         flightPathAngle_rad = UtMath::cPI_OVER_2;
      }
      else
      {
         flightPathAngle_rad = -UtMath::cPI_OVER_2;
      }
   }
   else
   {
      flightPathAngle_rad = atan2(-velocityDown, velocityHorizontal);
   }

   return flightPathAngle_rad;
}

double wsf::six_dof::KinematicState::GetBearing_rad() const
{
   double nedVel[3];
   mUtEntity.GetVelocityNED(nedVel);

   double velocityNorth = nedVel[0];
   double velocityEast  = nedVel[1];

   return atan2(velocityEast, velocityNorth);
}

double wsf::six_dof::KinematicState::GetLastNx_g() const
{
   return mLastNx;
}

double wsf::six_dof::KinematicState::GetLastNy_g() const
{
   return mLastNy;
}

double wsf::six_dof::KinematicState::GetLastNz_g() const
{
   return mLastNz;
}

double wsf::six_dof::KinematicState::GetLat() const
{
   double lat = 0.0;
   double lon = 0.0;
   double alt = 0.0;
   mUtEntity.GetLocationLLA(lat, lon, alt);

   return lat;
}

double wsf::six_dof::KinematicState::GetLon() const
{
   double lat = 0.0;
   double lon = 0.0;
   double alt = 0.0;
   mUtEntity.GetLocationLLA(lat, lon, alt);

   return lon;
}

double wsf::six_dof::KinematicState::GetAlt_m() const
{
   return mUtEntity.GetAltitude();
}

double wsf::six_dof::KinematicState::GetAlt_ft() const
{
   return GetAlt_m() * UtMath::cFT_PER_M;
}

double wsf::six_dof::KinematicState::GetNx_g() const
{
   return mNx_g;
}

double wsf::six_dof::KinematicState::GetNy_g() const
{
   return mNy_g;
}

double wsf::six_dof::KinematicState::GetNz_g() const
{
   return mNz_g;
}

void wsf::six_dof::KinematicState::SetOrientationWCS(double aPsi_rad, double aTheta_rad, double aPhi_rad)
{
   mUtEntity.SetOrientationWCS(aPsi_rad, aTheta_rad, aPhi_rad);
}

double wsf::six_dof::KinematicState::GetYawRate_rps() const
{
   return mOmega.Z();
}

double wsf::six_dof::KinematicState::GetPitchRate_rps() const
{
   return mOmega.Y();
}

double wsf::six_dof::KinematicState::GetRollRate_rps() const
{
   return mOmega.X();
}

double wsf::six_dof::KinematicState::GetYawRate_dps() const
{
   return mOmega.Z() * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::KinematicState::GetPitchRate_dps() const
{
   return mOmega.Y() * UtMath::cDEG_PER_RAD;
}

double wsf::six_dof::KinematicState::GetRollRate_dps() const
{
   return mOmega.X() * UtMath::cDEG_PER_RAD;
}

void wsf::six_dof::KinematicState::SetYawRate(double aYawRate_dps)
{
   mOmega.Set(mOmega.X(), mOmega.Y(), aYawRate_dps * UtMath::cDEG_PER_RAD);
}

void wsf::six_dof::KinematicState::SetPitchRate(double aPitchRate_dps)
{
   mOmega.Set(mOmega.X(), aPitchRate_dps * UtMath::cDEG_PER_RAD, mOmega.Z());
}

void wsf::six_dof::KinematicState::SetRollRate(double aRollRate_dps)
{
   mOmega.Set(aRollRate_dps * UtMath::cDEG_PER_RAD, mOmega.Y(), mOmega.Z());
}

double wsf::six_dof::KinematicState::GetLastDynamicPressure_psf() const
{
   return mLastDynamicPressure_psf;
}

double wsf::six_dof::KinematicState::GetLastStaticPressure_psf() const
{
   return mLastStaticPressure_psf;
}

double wsf::six_dof::KinematicState::GetLastAltitudeMSL_ft() const
{
   return mLastAlt_m;
}

bool wsf::six_dof::KinematicState::CalculateLLAofRelativePoint(double                aTerrainElevation_m,
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
                                                               double&               aCompression_m)
{
   // Clear and/or set default values
   aCompressionVectorNED.Set(0.0, 0.0, 0.0);
   aRollingVectorInSurfaceNED.Set(0.0, 0.0, 0.0);
   aNormalVectorNED.Set(0.0, 0.0, -1.0);
   aGearVelNED_mps.Set(0.0, 0.0, 0.0);
   aSurfaceSpeedVectorNED_mps.Set(0.0, 0.0, 0.0);
   aFrictionMultiplier = 1.0;
   aCompression_m      = 0.0;

   UtVec3dX pointWCS = aKinematicState->GetLocationWCS() + aKinematicState->mDCM.InverseTransform(aBodyRelPos_m);

   // Set entity WCS location
   double locWCS[3];
   pointWCS.Get(locWCS);
   UtEntity tempEntity;
   tempEntity.SetLocationWCS(locWCS);

   // Get LLA
   tempEntity.GetLocationLLA(aLat, aLon, aAlt_m);

   aHeightAboveTerrain_m = aAlt_m - aTerrainElevation_m;

   aCompression_m = aTerrainElevation_m - aAlt_m;
   if (aCompression_m < 0.0)
   {
      aCompression_m = 0.0;
   }

   aCompressionVectorNED = aKinematicState->CalcNEDVecFromBodyVec(aBodyRelCompressionVector);
   aCompressionVectorNED.Normalize();

   UtVec3dX rollingVectorNED;
   rollingVectorNED = aKinematicState->CalcNEDVecFromBodyVec(aBodyRelRollingVector);
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
   UtVec3dX inducedVelBodyVec_mps     = aKinematicState->mOmega.Cross(aBodyRelPos_m);
   UtVec3dX inducedVelInertialVec_mps = aKinematicState->mDCM.InverseTransform(inducedVelBodyVec_mps);
   UtVec3dX totalGearVelInertial_mps  = aKinematicState->GetVelocityWCS() + inducedVelInertialVec_mps;

   aGearVelNED_mps = aKinematicState->CalcNEDVecFromWCSVec(totalGearVelInertial_mps);

   return true;
}

UtVec3dX wsf::six_dof::KinematicState::NormalizedGravitationalAccelVec() const
{
   // This will be the direction of gravity
   UtVec3dX unitGravityVec(0., 0., 0.);

   UtVec3d gravityAccel = mUtEntity.GetCentralBody().GetAccelerationInertial(GetLocationWCS());
   unitGravityVec.Set(gravityAccel);

   // We need a unit vector
   unitGravityVec.Normalize();

   // Get the gravity at the reference altitude
   double earthRadius_m = mUtEntity.GetCentralBody().GetMeanRadius();

   double re2 = earthRadius_m * earthRadius_m;
   double r   = earthRadius_m + GetAlt_m();

   // At altitude, gravity becomes gFactor = (re^2/((re+alt)^2)
   double normalizedGravityMagnitude = (re2 / (r * r));

   // Scale the unit vector
   unitGravityVec = normalizedGravityMagnitude * unitGravityVec;

   return unitGravityVec;
}

void wsf::six_dof::KinematicState::UpdateAeroState(const int64_t aSimTime_nanosec)
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

      if (deltaT_nanosec > utils::cEPSILON_SIMTIME_NANOSEC)
      {
         double dT_sec = utils::TimeToTime(deltaT_nanosec);

         if (mEnvironment == nullptr)
         {
            return;
         }

         // Remember the last values
         double mLastAlpha_deg = mAlpha_deg;
         double mLastBeta_deg  = mBeta_deg;

         UtVec3dX windSpeedNED_mps = mEnvironment->GetWindNED_mps(GetLat(), GetLon(), GetAlt_m());

         UtVec3dX windWCS;
         mUtEntity.ConvertNEDVectorToWCS(windWCS.GetData(), windSpeedNED_mps.GetData());
         UtVec3dX airspeed_mps = GetVelocityWCS() - windWCS;

         double speed_fps;
         utils::AlphaBetaFromInertialVel(mDCM, airspeed_mps, speed_fps, mAlpha_deg, mBeta_deg);

         mEnvironment->GetAeroParameters(GetAlt_ft(), speed_fps, mRho_slugs_per_ft3, mDynamicPressure_psf, mStaticPressure_psf);

         double deltaAlpha_deg = mAlpha_deg - mLastAlpha_deg;
         double deltaBeta_deg  = mBeta_deg - mLastBeta_deg;

         mAlphaDot_dps = deltaAlpha_deg / dT_sec;
         mBetaDot_dps  = deltaBeta_deg / dT_sec;

         // Update the time
         mLastUpdate_nanosec = aSimTime_nanosec;
      }
      else
      {
         // Delta-t is negligible, so do nothing
         return;
      }
   }
}

void wsf::six_dof::KinematicState::GetAngleDeltas(double& aDeltaYaw_rad, double& aDeltaPitch_rad, double& aDeltaRoll_rad) const
{
   aDeltaYaw_rad   = mDeltaYaw_rad;
   aDeltaPitch_rad = mDeltaPitch_rad;
   aDeltaRoll_rad  = mDeltaRoll_rad;
}

void wsf::six_dof::KinematicState::SetAngleDeltas(double aDeltaYaw_rad, double aDeltaPitch_rad, double aDeltaRoll_rad)
{
   mDeltaYaw_rad   = aDeltaYaw_rad;
   mDeltaPitch_rad = aDeltaPitch_rad;
   mDeltaRoll_rad  = aDeltaRoll_rad;
}

void wsf::six_dof::KinematicState::RemoveAlphaForTesting()
{
   // Adjust DCM with ypr deltas
   // UtDCM yawDCM(deltaYaw_rad,0.0,0.0);
   // UtDCM pitchDCM(0.0,deltaPitch_rad,0.0);
   // UtDCM rollDCM(0.0,0.0,deltaRoll_rad);
   // mDCM = rollDCM * (pitchDCM * (yawDCM * mDCM));

   UtDCM pitchDCM(0.0, -GetAlpha_rad(), 0.0);
   mDCM = pitchDCM * mDCM;

   CalculateSecondaryParameters();
}

void wsf::six_dof::KinematicState::SetVelocityNED(double aVelNorth_mps, double aVelEast_mps, double aVelDown_mps)
{
   double nedVel[3] = {aVelNorth_mps, aVelEast_mps, aVelDown_mps};
   mUtEntity.SetVelocityNED(nedVel);
}

UtVec3dX wsf::six_dof::KinematicState::GetVelocityNED_mps() const
{
   UtVec3dX NEDVel_mps;
   mUtEntity.GetVelocityNED(NEDVel_mps.GetData());
   return NEDVel_mps;
}

void wsf::six_dof::KinematicState::GetBodyAeroVectors(const UtDCM&    aDCM,
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

void wsf::six_dof::KinematicState::ZeroVelocity()
{
   SetVelocityWCS(UtVec3dX(0.0, 0.0, 0.0));
}

UtVec3dX wsf::six_dof::KinematicState::GetOmegaBodyDPS() const
{
   return mOmega * UtMath::cDEG_PER_RAD;
}

void wsf::six_dof::KinematicState::SetStartingLatLon(double aLat, double aLon)
{
   mStartingLocationInitialized = true;
   mStartingLat                 = aLat;
   mStartingLon                 = aLon;
}

void wsf::six_dof::KinematicState::GetStartingLatLon(double& aLat, double& aLon) const
{
   aLat = mStartingLat;
   aLon = mStartingLon;
}

void wsf::six_dof::KinematicState::ZeroKinematicsDuringTesting()
{
   mAccelerationWCS.Set(0.0, 0.0, 0.0);
   mOmega.Set(0.0, 0.0, 0.0);
   mOmegaDot.Set(0.0, 0.0, 0.0);

   ZeroVelocity();

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

   mYawRate_dps    = 0.0;
   mPitchRate_dps  = 0.0;
   mRollRate_dps   = 0.0;
   mDeltaYaw_rad   = 0.0;
   mDeltaPitch_rad = 0.0;
   mDeltaRoll_rad  = 0.0;

   mLift_lbs      = 0.0;
   mSideForce_lbs = 0.0;
   mDrag_lbs      = 0.0;
   mThrust_lbs    = 0.0;
   mMoment_ftlbs.Set(0.0, 0.0, 0.0);
}
