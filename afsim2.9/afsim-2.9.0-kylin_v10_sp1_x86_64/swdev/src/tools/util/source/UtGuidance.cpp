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

#include "UtGuidance.hpp"

//#define ASSERT_TRANSFORMS 1

#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"

namespace
{
const double   cSIM_TIME_NOT_SET = -1.0;
const UtVec3dX cNED_ACCEL_OF_GRAVITY(0.0, 0.0, UtEarth::cACCEL_OF_GRAVITY);
double         Sign(double aValue)
{
   return (aValue >= 0.0) ? 1.0 : -1.0;
}
} // namespace

/** Default constructor */
UtGuidance::UtGuidance(bool /*aOpposeGravity*/, bool /*aInertialIsNED*/)
   : mPrevSimTime(cSIM_TIME_NOT_SET)
   , mProNavGain(40.0)
   , mVelPursuitGain(4.0)
   , mHorizontalLOSR_Bias(0.0)
   , mVerticalLOSR_Bias(0.0)
   , mPrevInertialUnitLOS()
   , mInertialUnitLOS()
{
}

bool UtGuidance::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "velocity_pursuit_gain")
   {
      aInput.ReadValue(mVelPursuitGain);
      aInput.ValueGreaterOrEqual(mVelPursuitGain, 0.0);
   }
   else if (command == "proportional_navigation_gain")
   {
      aInput.ReadValue(mProNavGain);
      aInput.ValueGreaterOrEqual(mProNavGain, 0.0);
   }
   else if (command == "vertical_line_of_sight_rate_bias")
   {
      aInput.ReadValueOfType(mVerticalLOSR_Bias, UtInput::cANGULAR_RATE);
   }
   else if (command == "horizontal_line_of_sight_rate_bias")
   {
      aInput.ReadValueOfType(mHorizontalLOSR_Bias, UtInput::cANGULAR_RATE);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

/*void UtGuidance::AccelerationVel(const double aEyePoint[3],
                                 const double aEyeVelocity[3],
                                 const double aTgtPoint[3],
                                 const double aTgtVelocity[3],
                                 const double aInertialToVel[3][3],
                                 double       aAccelVel[3])
{
   // Re-use the function below, but change the signature:
   UtVec3dX eyePoint(aEyePoint);
   UtVec3dX eyeVelocity(aEyeVelocity);
   UtVec3dX tgtPoint(aTgtPoint);
   UtVec3dX tgtVelocity(aTgtVelocity);
   UtDCM    inertialToVel(aInertialToVel);

   UtVec3dX accelVel = AccelerationVel(eyePoint, eyeVelocity,
                                       tgtPoint, tgtVelocity,
                                       inertialToVel);
   accelVel.Get(aAccelVel);
}*/

/*UtVec3dX UtGuidance::AccelerationVel(const UtVec3dX& aEyePoint,
                                     const UtVec3dX& aEyeVelocity,
                                     const UtVec3dX& aTgtPoint,
                                     const UtVec3dX& aTgtVelocity,
                                     const UtDCM&    aInertialToVel)
{
   bool isSingularity = true;

   // AccelerationVel LOS Vector (Inertial Frame)
   UtVec3dX inertialLOS = aTgtPoint - aEyePoint;
   double magR = inertialLOS.Magnitude();
   if (magR < 0.1)
   {
      // No solution, return zero.
      return UtVec3dX();
   }
   else
   {
      // ======= VELOCITY PURSUIT =======

      UtVec3dX tgtVectorVel = inertialLOS * aInertialToVel;
      tgtVectorVel.Normalize();
      double az = UtMath::NormalizeAngleMinusPi_Pi(tgtVectorVel.Azimuth());
      double el = UtMath::NormalizeAngleMinusPi_Pi(tgtVectorVel.Elevation());
      UtVec3dX vpAccelVel;
      vpAccelVel[1] =  az * mVelPursuitGain;
      vpAccelVel[2] = -el * mVelPursuitGain;

      // ======= PROPORTIONAL NAVIGATION =======

      // <<< PROGRAMMERS NOTE:  This PN alternative contains an unknown error, and is disabled until found !!! >>>

      // AccelerationVel relative velocity (Inertial Frame)
      UtVec3dX inertialRelVel = aTgtVelocity - aEyeVelocity;
      double magV = inertialRelVel.Magnitude();

      // R cross V = m * m/s => m2 / s
      UtVec3dX RcrossV = inertialLOS.Cross(inertialRelVel) / inertialLOS.Magnitude() / inertialRelVel.Magnitude();

      // RcrossV / magR / magR = m2 / s / m / m => 1/s
      UtVec3dX inertialLOSR = (1.0 / magR ) * RcrossV;

      // Accel = losr * vel * nondim= 1/s * m/s => m/s2
      UtVec3dX pnAccelVel = inertialLOSR * mProNavGain * aInertialToVel;

      // Ignore any "roll" rotational rate of LOS.
      pnAccelVel[0] = 0.0;

      UtVec3dX accelVel = vpAccelVel + pnAccelVel;

      return accelVel;
   }
}*/

void UtGuidance::Reset()
{
   mPrevSimTime = cSIM_TIME_NOT_SET;
   mPrevInertialUnitLOS.Null();
   mInertialUnitLOS.Null();
}

void UtGuidance::AccelerationVel(double       aSimTime,
                                 const double aEyePoint[3],
                                 const double aEyeVelocity[3],
                                 const double aTgtPoint[3],
                                 const double aInertialToVel[3][3],
                                 const double aInertialToECS[3][3],
                                 const double aInertialToNED[3][3],
                                 double       aAccelVel[3])
{
   // Re-use the function below, but change the signature:
   UtVec3dX eyePoint(aEyePoint);
   UtVec3dX eyeVelocity(aEyeVelocity);
   UtVec3dX tgtPoint(aTgtPoint);
   UtDCM    inertialToVel(aInertialToVel);
   UtDCM    inertialToECS(aInertialToECS);
   UtDCM    inertialToNED(aInertialToNED);
   UtVec3dX accelVel =
      AccelerationVel(aSimTime, eyePoint, eyeVelocity, tgtPoint, inertialToVel, inertialToECS, inertialToNED);
   accelVel.Get(aAccelVel);
}

UtVec3dX UtGuidance::AccelerationVel(double          aSimTime,
                                     const UtVec3dX& aEyePoint,
                                     const UtVec3dX& aEyeVelocity,
                                     const UtVec3dX& aTgtPoint,
                                     const UtDCM&    aInertialToVel,
                                     const UtDCM&    aInertialToECS,
                                     const UtDCM&    aInertialToNED)
{
   bool useVP   = mVelPursuitGain > 0.0;
   bool useLOSR = mProNavGain > 0.0;

   // Save the previous value of Inertial LOS (Normalized):
   mPrevInertialUnitLOS = mInertialUnitLOS;

   // Update the new value for Inertial LOS (Normalized):
   UtVec3dX inertialLOS = aTgtPoint - aEyePoint;

   double magR = inertialLOS.Magnitude();
   if (magR < 0.1)
   {
      useVP   = false;
      useLOSR = false;
   }

   mInertialUnitLOS = inertialLOS;
   mInertialUnitLOS.Normalize();

   if (mPrevSimTime == cSIM_TIME_NOT_SET)
   {
      useLOSR = false;
   }

   double dt = aSimTime - mPrevSimTime;
   if (dt < 0.0001)
   {
      useLOSR = false;
   }

   // The return value below will have up to four contributions summed into it.
   UtVec3dX accelVel; // return value;

   // ======= VELOCITY PURSUIT =======
   if (useVP) // && aInertialToVel.IsValid())
   {
      UtVec3dX tgtUnitVectorECS = mInertialUnitLOS * aInertialToECS;

#ifdef ASSERT_TRANSFORMS
      assert(aInertialToVel.IsValid());
      assert(aInertialToECS.IsValid());
      assert(aInertialToNED.IsValid());
#endif

      double az = tgtUnitVectorECS.Azimuth();
      if (fabs(az) > UtMath::cPI_OVER_2)
      {
         az = Sign(az) * UtMath::cPI_OVER_2;
      }
      double el = tgtUnitVectorECS.Elevation();
      // double azDeg = az * UtMath::cDEG_PER_RAD;
      // double elDeg = el * UtMath::cDEG_PER_RAD;

      UtVec3dX vpAccelECS;
      vpAccelECS[1] = az * mVelPursuitGain;
      vpAccelECS[2] = -el * mVelPursuitGain;

#ifdef ASSERT_TRANSFORMS
      assert(vpAccelECS[1] == vpAccelECS[1]);
      assert(vpAccelECS[2] == vpAccelECS[2]);
#endif

      accelVel += vpAccelECS * (aInertialToVel * aInertialToECS.Transpose());
   }

   if (useLOSR)
   {
      // ======= PROPORTIONAL NAVIGATION =======

      UtVec3dX inertialLosRate = (1.0 / dt) * (mInertialUnitLOS - mPrevInertialUnitLOS);

      if ((mVerticalLOSR_Bias != 0.0) || (mHorizontalLOSR_Bias != 0.0))
      {
         // Wash out a vertical LOS bias by cos(LOS pitch).
         UtVec3dX unitLineOfSightNED = mInertialUnitLOS * aInertialToNED;
         double   cosine =
            sqrt(unitLineOfSightNED[0] * unitLineOfSightNED[0] + unitLineOfSightNED[1] * unitLineOfSightNED[1]);
         UtVec3dX losrBiasNED(mHorizontalLOSR_Bias * unitLineOfSightNED[0],
                              mHorizontalLOSR_Bias * unitLineOfSightNED[1],
                              cosine * mVerticalLOSR_Bias);
         inertialLosRate += losrBiasNED * aInertialToNED.Transpose();
      }

      // LOSR = 1/s.  So desired accel = m/s2 = 1/s * m/s
      UtVec3dX pnAccelVel = aEyeVelocity.Magnitude() * inertialLosRate * mProNavGain * aInertialToVel;
      pnAccelVel[0]       = 0.0;

#ifdef ASSERT_TRANSFORMS
      assert(aInertialToVel.IsValid());
      assert(pnAccelVel[1] == pnAccelVel[1]);
      assert(pnAccelVel[2] == pnAccelVel[2]);
#endif

      accelVel += pnAccelVel;
   }

   mPrevSimTime = aSimTime;

   return accelVel;
}
