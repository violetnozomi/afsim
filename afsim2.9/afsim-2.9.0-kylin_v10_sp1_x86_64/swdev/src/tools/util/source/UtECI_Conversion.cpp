// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// References:
//
// 1) Fundamentals of Astrodynamics and Applications, 4th ed., David A. Vallado
// 2) USNO Circular #179: "The IAU Resolutions on Astronomical Reference Systems,
//    Time Scales, and Earth Rotation Models, Explanation and Implementation",
//    George H. Kaplan
// 3) "Rotation matrix from the mean dynamical equator and equinox to the ICRS",
//    J.L. Hilton and C.Y. Hohenkerk, Astronomy & Astrophysics 413, pp. 765-770 (2004).

#include "UtECI_Conversion.hpp"

#include "UtEarth.hpp"
#include "UtException.hpp"
#include "UtSun.hpp"
#include "assert.h"

namespace ECIConversionLocal
{
// ****************************************************************************
// Constant needed by the Frame Tie Routines
static constexpr double ASEC2RAD = 4.848136811095359935899141e-6;
// ****************************************************************************

// The following are adapted from NOVAS routines:
//! Compute the Frame Tie matrix, to convert J2000 to GCRF (ref. 4)
const double* GetFrameTieMatrix()
{
   static const double xi0  = -0.0166170;
   static const double eta0 = -0.0068192;
   static const double da0  = -0.01460;
   static double       frameTie[9] =
      {1.0, -da0 * ASEC2RAD, xi0 * ASEC2RAD, da0 * ASEC2RAD, 1.0, eta0 * ASEC2RAD, -xi0 * ASEC2RAD, -eta0 * ASEC2RAD, 1.0};
   static bool once = true;
   if (once)
   {
      frameTie[0] = 1.0 - 0.5 * (frameTie[1] * frameTie[1] + frameTie[2] * frameTie[2]);
      frameTie[4] = 1.0 - 0.5 * (frameTie[1] * frameTie[1] + frameTie[5] * frameTie[5]);
      frameTie[8] = 1.0 - 0.5 * (frameTie[5] * frameTie[5] + frameTie[2] * frameTie[2]);
      once        = false;
   }

   return frameTie;
}

//! Compute the Frame Tie matrix transpose, to convert GCRF to J2000.
const double* GetFrameTieMatrixTranspose()
{
   static const double xi0  = -0.0166170;
   static const double eta0 = -0.0068192;
   static const double da0  = -0.01460;
   static double       frameTieTranspose[9] =
      {1.0, da0 * ASEC2RAD, -xi0 * ASEC2RAD, -da0 * ASEC2RAD, 1.0, -eta0 * ASEC2RAD, xi0 * ASEC2RAD, eta0 * ASEC2RAD, 1.0};
   static bool once = true;
   if (once)
   {
      frameTieTranspose[0] =
         1.0 - 0.5 * (frameTieTranspose[3] * frameTieTranspose[3] + frameTieTranspose[6] * frameTieTranspose[6]);
      frameTieTranspose[4] =
         1.0 - 0.5 * (frameTieTranspose[3] * frameTieTranspose[3] + frameTieTranspose[7] * frameTieTranspose[7]);
      frameTieTranspose[8] =
         1.0 - 0.5 * (frameTieTranspose[7] * frameTieTranspose[7] + frameTieTranspose[6] * frameTieTranspose[6]);
      once = false;
   }

   return frameTieTranspose;
}

} // namespace ECIConversionLocal

// static
const UtMat3d UtECI_Conversion::mFrameTie(ECIConversionLocal::GetFrameTieMatrix());

const UtMat3d UtECI_Conversion::mFrameTieTranspose(ECIConversionLocal::GetFrameTieMatrixTranspose());

//! Standard constructor.
//! @param aEpoch The reference epoch.
//! @param aPolarOffsetAngleX The polar offset angle "x" value (default is zero).
//! @param aPolarOffsetAngleY The polar offset angle "y" value (default is zero).
UtECI_Conversion::UtECI_Conversion(const UtCalendar& aEpoch, const ut::CentralPoint& aCentralPoint)
   : mSetFlags(0)
   , mValidityFlags(0)
   , mInitialEpoch(aEpoch)
   , mEpoch(aEpoch)
{
   SetCentralPoint(aCentralPoint);
   if (aCentralPoint.UsingPolarMotion())
   {
      mPolarMotion          = aCentralPoint.ComputePolarMotion();
      mPolarMotionTranspose = mPolarMotion;
      mPolarMotionTranspose.Transpose();
   }
   SetTime(0.0);
}

void UtECI_Conversion::SetCentralPoint(const ut::CentralPoint& aCentralPoint)
{
   mCentralPointPtr = ut::clone(aCentralPoint);
   mOmegaPEF.Set(0.0, 0.0, mCentralPointPtr->GetRotationRate());
   mECI_TransformsValid = false;
   mN_TransformValid    = false;
   mUsingPolarMotion    = mCentralPointPtr->UsingPolarMotion();
}

//! Set the time since epoch, and recompute values needed to compute the sidereal motion, precession, and nutation matrices.
//! @param aTimeSinceEpoch The elapsed time since mEpoch.
void UtECI_Conversion::SetTime(double aTimeSinceEpoch)
{
   double dt = aTimeSinceEpoch - mTimeSinceEpoch;
   mEpoch.AdvanceTimeBy(dt);
   mTimeSinceEpoch = aTimeSinceEpoch;

   if (fabs(mTimeSinceEpoch - mNutationLastUpdateTime) >= mNutationUpdateInterval)
   {
      mNutationLastUpdateTime = mTimeSinceEpoch;
      mN_TransformValid       = false;
   }

   mECI_TransformsValid = false;
   mValidityFlags       = mSetFlags;
}

//! Set the reference epoch (automatically resets the time past epoch to zero).
void UtECI_Conversion::SetEpoch(const UtCalendar& aEpoch)
{
   if (mInitialEpoch != aEpoch)
   {
      mInitialEpoch           = aEpoch;
      mEpoch                  = aEpoch;
      mValidityFlags          = 0;
      mSetFlags               = 0;
      mTimeSinceEpoch         = 0.0;
      mNutationLastUpdateTime = -mNutationUpdateInterval;
      SetTime(0.0);
      mCentralPointPtr->Initialize(aEpoch);
   }
}

//! Return the epoch that was initially set in the constructor or in a call to SetEpoch.
const UtCalendar& UtECI_Conversion::GetEpoch() const
{
   return mInitialEpoch;
}

#undef GetCurrentTime // Avoid conflict with Windows macro

//! Return the "current time" / current epoch.
const UtCalendar& UtECI_Conversion::GetCurrentTime() const
{
   return mEpoch;
}

//! Deprecated; see SetNutationUpdateInterval.
void UtECI_Conversion::SetPrecessionNutationUpdateInterval(double aPrecessionNutationUpdateInterval)
{
   mNutationUpdateInterval = aPrecessionNutationUpdateInterval;
}

//! Sets the time interval between computations of the nutation matrix.
//! @param aNutationUpdateInterval The interval at which nutation updates are performed.
void UtECI_Conversion::SetNutationUpdateInterval(double aNutationUpdateInterval)
{
   mNutationUpdateInterval = aNutationUpdateInterval;
}

//! Set the polar motion matrix according to the small-angle approximation in Vallado, eq. 3.78.
//! @param aXp The x polar offset angle.
//! @param aYp The y polar offset angle.
void UtECI_Conversion::SetPolarOffsetAngles(double aXp, double aYp)
{
   auto& centralBody = mCentralPointPtr->GetAsCentralBody();
   centralBody.SetPolarOffsetAngles(aXp, aYp);
   mPolarMotion          = centralBody.ComputePolarMotion();
   mPolarMotionTranspose = mPolarMotion;
   mPolarMotionTranspose.Transpose();
   mUsingPolarMotion = true;
}

//! Update the ECI location from a valid WCS or TOD location.
void UtECI_Conversion::UpdateECI_Location() const
{
   if (!mECILocationIsValid)
   {
      assert(mWCSLocationIsValid || mTODLocationIsValid);
      UpdateTransforms();

      if (mTODLocationIsValid)
      {
         mLocationECI = mFPN * mLocationTOD;
      }
      else
      {
         mLocationECI = mWCS_ToECI_Transform * mLocationWCS;
      }
      mECILocationIsValid = true;
   }
}

//! Update the ECI velocity from a valid WCS or TOD velocity.
void UtECI_Conversion::UpdateECI_Velocity() const
{
   if (!mECIVelocityIsValid)
   {
      assert(mWCSVelocityIsValid || mTODVelocityIsValid);
      UpdateTransforms();

      if (mWCSVelocityIsValid)
      {
         if (mUsingPolarMotion)
         {
            mVelocityECI = mFPN * mS * (mPolarMotion * mVelocityWCS + GetOmegaCrossR());
         }
         else
         {
            mVelocityECI = mFPN * mS * (mVelocityWCS + GetOmegaCrossR());
         }
      }
      else // TOD Velocity is valid
      {
         mVelocityECI = mFPN * mVelocityTOD;
      }

      mECIVelocityIsValid = true;
   }
}

//! Update the WCS velocity from a valid ECI or TOD velocity.
void UtECI_Conversion::UpdateWCS_Velocity() const
{
   if (!mWCSVelocityIsValid)
   {
      assert(mECIVelocityIsValid || mTODVelocityIsValid);
      UpdateTransforms();

      if (mECIVelocityIsValid)
      {
         if (mUsingPolarMotion)
         {
            mVelocityWCS = mPolarMotionTranspose * (mSt * mNtPtFt * mVelocityECI - GetOmegaCrossR());
         }
         else
         {
            mVelocityWCS = mSt * mNtPtFt * mVelocityECI - GetOmegaCrossR();
         }
      }
      else // TOD valid
      {
         if (mUsingPolarMotion)
         {
            mVelocityWCS = mPolarMotionTranspose * (mSt * mVelocityTOD - GetOmegaCrossR());
         }
         else
         {
            mVelocityWCS = mSt * mVelocityTOD - GetOmegaCrossR();
         }
      }

      mWCSVelocityIsValid = true;
   }
}

//! Update the ECI acceleration from a valid TOD acceleration, or from
//! a valid WCS acceleration, velocity, and position.
void UtECI_Conversion::UpdateECI_Acceleration() const
{
   if (!mECIAccelerationIsValid)
   {
      assert(mWCSAccelerationIsValid || mTODAccelerationIsValid);
      UpdateTransforms();

      if (mTODAccelerationIsValid)
      {
         mAccelerationECI = mFPN * mAccelerationTOD;
      }
      else
      {
         if (mUsingPolarMotion)
         {
            mAccelerationECI =
               mFPN * mS * (mPolarMotion * mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
         }
         else
         {
            mAccelerationECI = mFPN * mS * (mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
         }
      }

      mECIAccelerationIsValid = true;
   }
}

//! Update the WCS acceleration from a valid ECI or TOD acceleration, and
//! a valid WCS velocity, and position.
void UtECI_Conversion::UpdateWCS_Acceleration() const
{
   if (!mWCSAccelerationIsValid)
   {
      assert(mECIAccelerationIsValid || mTODAccelerationIsValid);
      UpdateTransforms();

      if (mECIAccelerationIsValid)
      {
         if (mUsingPolarMotion)
         {
            mAccelerationWCS = mPolarMotionTranspose * (mSt * mNtPtFt * mAccelerationECI -
                                                        GetCentripetalAcceleration() - GetCoriolisAcceleration());
         }
         else
         {
            mAccelerationWCS = mSt * mNtPtFt * mAccelerationECI - GetCentripetalAcceleration() - GetCoriolisAcceleration();
         }
      }
      else // TOD valid
      {
         if (mUsingPolarMotion)
         {
            mAccelerationWCS = mPolarMotionTranspose *
                               (mSt * mAccelerationTOD - GetCentripetalAcceleration() - GetCoriolisAcceleration());
         }
         else
         {
            mAccelerationWCS = mSt * mAccelerationTOD - GetCentripetalAcceleration() - GetCoriolisAcceleration();
         }
      }

      mWCSAccelerationIsValid = true;
   }
}

//! Update the TOD acceleration from a valid ECI acceleration, or from
//! a valid WCS acceleration, velocity, and position.
void UtECI_Conversion::UpdateTOD_Acceleration() const
{
   if (!mTODAccelerationIsValid)
   {
      assert(mWCSAccelerationIsValid || mECIAccelerationIsValid);
      UpdateTransforms();

      if (mWCSAccelerationIsValid)
      {
         if (mUsingPolarMotion)
         {
            mAccelerationTOD =
               mS * (mPolarMotion * mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
         }
         else
         {
            mAccelerationTOD = mS * (mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
         }
      }
      else // ECI valid
      {
         mAccelerationTOD = mNtPtFt * mAccelerationECI;
      }

      mTODAccelerationIsValid = true;
   }
}

//! Update the PEF location from a valid WCS location.
void UtECI_Conversion::UpdatePEF_Location() const
{
   if (!mPEFLocationIsValid)
   {
      UpdateWCS_Location();
      if (mUsingPolarMotion)
      {
         mLocationPEF = mPolarMotion * GetLocationWCS();
      }
      else
      {
         mLocationPEF = GetLocationWCS();
      }
      mPEFLocationIsValid = true;
   }
}

//! Get the ECI location (legacy version).
void UtECI_Conversion::GetLocationECI(double aLocationECI[3]) const
{
   UpdateECI_Location();
   UtVec3d::Set(aLocationECI, mLocationECI.GetData());
}

//! Set the ECI location (legacy version).
void UtECI_Conversion::SetLocationECI(const double aLocationECI[3])
{
   mLocationECI.Set(aLocationECI);
   mECILocationIsSet = true;
   mWCSLocationIsSet = false;
   mTODLocationIsSet = false;

   mECILocationIsValid = true;
   mWCSLocationIsValid = false;
   mTODLocationIsValid = false;
   mPEFLocationIsValid = false;
}

//! Get the WCS location (legacy version).
void UtECI_Conversion::GetLocationWCS(double aLocationWCS[3]) const
{
   UpdateWCS_Location();
   UtVec3d::Set(aLocationWCS, mLocationWCS.GetData());
}

//! Update the TOD location, given a valid WCS location.
void UtECI_Conversion::UpdateTOD_Location() const
{
   if (!mTODLocationIsValid)
   {
      UpdateTransforms();
      UpdateWCS_Location();
      if (mUsingPolarMotion)
      {
         mLocationTOD = mS * mPolarMotion * mLocationWCS;
      }
      else
      {
         mLocationTOD = mS * mLocationWCS;
      }
      mTODLocationIsValid = true;
   }
}

//! Get the TOD location.
UtVec3d UtECI_Conversion::GetLocationTOD() const
{
   UpdateTOD_Location();
   return mLocationTOD;
}

//! Set the WCS location (legacy version).
void UtECI_Conversion::SetLocationWCS(const double aLocationWCS[3])
{
   mLocationWCS.Set(aLocationWCS);
   mWCSLocationIsSet = true;
   mECILocationIsSet = false;
   mTODLocationIsSet = false;

   mWCSLocationIsValid = true;
   mECILocationIsValid = false;
   mTODLocationIsValid = false;
   mPEFLocationIsValid = false;
}

//! Get the ECI velocity (legacy version).
void UtECI_Conversion::GetVelocityECI(double aVelocityECI[3]) const
{
   UpdateECI_Velocity();
   UtVec3d::Set(aVelocityECI, mVelocityECI.GetData());
}

//! Set the ECI velocity (legacy version).
void UtECI_Conversion::SetVelocityECI(const double aVelocityECI[3])
{
   mVelocityECI.Set(aVelocityECI);
   mECIVelocityIsSet = true;
   mWCSVelocityIsSet = false;
   mTODVelocityIsSet = false;

   mECIVelocityIsValid = true;
   mWCSVelocityIsValid = false;
   mTODVelocityIsValid = false;
}

//! Get the WCS velocity (legacy version).
void UtECI_Conversion::GetVelocityWCS(double aVelocityWCS[3]) const
{
   UpdateWCS_Velocity();
   UtVec3d::Set(aVelocityWCS, mVelocityWCS.GetData());
}

//! Set the WCS velocity (legacy version).
void UtECI_Conversion::SetVelocityWCS(const double aVelocityWCS[3])
{
   mVelocityWCS.Set(aVelocityWCS);
   mWCSVelocityIsSet = true;
   mECIVelocityIsSet = false;
   mTODVelocityIsSet = false;

   mWCSVelocityIsValid = true;
   mECIVelocityIsValid = false;
   mTODVelocityIsValid = false;
}

//! Get the WCS location.
UtVec3d UtECI_Conversion::GetLocationWCS() const
{
   UpdateWCS_Location();
   return mLocationWCS;
}

//! Set the ECI acceleration (legacy version).
void UtECI_Conversion::SetAccelerationECI(const double aAccelerationECI[3])
{
   mAccelerationECI.Set(aAccelerationECI);
   mECIAccelerationIsSet = true;
   mWCSAccelerationIsSet = false;
   mTODAccelerationIsSet = false;

   mECIAccelerationIsValid = true;
   mWCSAccelerationIsValid = false;
   mTODAccelerationIsValid = false;
}

//! Get the WCS acceleration (legacy version).
void UtECI_Conversion::GetAccelerationWCS(double aAccelerationWCS[3]) const
{
   UpdateWCS_Acceleration();
   UtVec3d::Set(aAccelerationWCS, mAccelerationWCS.GetData());
}

//! Get the ECI acceleration (legacy version).
void UtECI_Conversion::GetAccelerationECI(double aAccelerationECI[3]) const
{
   UpdateECI_Acceleration();
   UtVec3d::Set(aAccelerationECI, mAccelerationECI.GetData());
}

//! Set the WCS acceleration (legacy version).
void UtECI_Conversion::SetAccelerationWCS(const double aAccelerationWCS[3])
{
   mAccelerationWCS.Set(aAccelerationWCS);
   mWCSAccelerationIsSet = true;
   mECIAccelerationIsSet = false;
   mTODAccelerationIsSet = false;

   mWCSAccelerationIsValid = true;
   mECIAccelerationIsValid = false;
   mTODAccelerationIsValid = false;
}

//! Set the WCS location.
void UtECI_Conversion::SetLocationWCS(const UtVec3d& aLocationWCS)
{
   mLocationWCS      = aLocationWCS;
   mWCSLocationIsSet = true;
   mECILocationIsSet = false;
   mTODLocationIsSet = false;

   mWCSLocationIsValid = true;
   mECILocationIsValid = false;
   mTODLocationIsValid = false;
   mPEFLocationIsValid = false;
}

//! Set the ECI location.
void UtECI_Conversion::SetLocationECI(const UtVec3d& aLocationECI)
{
   mLocationECI      = aLocationECI;
   mECILocationIsSet = true;
   mWCSLocationIsSet = false;
   mTODLocationIsSet = false;

   mECILocationIsValid = true;
   mWCSLocationIsValid = false;
   mTODLocationIsValid = false;
   mPEFLocationIsValid = false;
}

//! Set the TOD location.
void UtECI_Conversion::SetLocationTOD(const UtVec3d& aLocationTOD)
{
   mLocationTOD      = aLocationTOD;
   mTODLocationIsSet = true;
   mECILocationIsSet = false;
   mWCSLocationIsSet = false;

   mTODLocationIsValid = true;
   mECILocationIsValid = false;
   mWCSLocationIsValid = false;
   mPEFLocationIsValid = false;
}

//! Set the J2000 location.
void UtECI_Conversion::SetLocationJ2000(const UtVec3d& aLocationJ2000)
{
   if (mCentralPointPtr->IsEarth())
   {
      SetLocationECI(mFrameTie * aLocationJ2000);
   }
   else
   {
      SetLocationECI(aLocationJ2000);
   }
}

//! Get the TEME location.
UtVec3d UtECI_Conversion::GetLocationTEME() const
{
   UpdateTransforms();
   return mTEME_ToPEF_Transform.TransposeMultiply(GetLocationPEF());
}

//! Set the TEME location.
void UtECI_Conversion::SetLocationTEME(const UtVec3d& aLocationTEME)
{
   mECILocationIsValid = false;
   mTODLocationIsValid = false;
   mPEFLocationIsValid = false;

   // Immediately convert TEME to WCS
   UpdateTransforms();
   if (mUsingPolarMotion)
   {
      mLocationWCS = mPolarMotionTranspose * mTEME_ToPEF_Transform * aLocationTEME;
   }
   else
   {
      mLocationWCS = mTEME_ToPEF_Transform * aLocationTEME;
   }

   mWCSLocationIsValid = true;

   mWCSLocationIsSet = true;
   mECILocationIsSet = false;
   mTODLocationIsSet = false;
}

//! Set the ECI velocity.
void UtECI_Conversion::SetVelocityECI(const UtVec3d& aVelocityECI)
{
   mVelocityECI      = aVelocityECI;
   mECIVelocityIsSet = true;
   mWCSVelocityIsSet = false;
   mTODVelocityIsSet = false;

   mECIVelocityIsValid = true;
   mWCSVelocityIsValid = false;
   mTODVelocityIsValid = false;
}

//! Set the WCS velocity.
void UtECI_Conversion::SetVelocityWCS(const UtVec3d& aVelocityWCS)
{
   mVelocityWCS      = aVelocityWCS;
   mWCSVelocityIsSet = true;
   mECIVelocityIsSet = false;
   mTODVelocityIsSet = false;

   mWCSVelocityIsValid = true;
   mECIVelocityIsValid = false;
   mTODVelocityIsValid = false;
}

//! Set the TOD velocity.
void UtECI_Conversion::SetVelocityTOD(const UtVec3d& aVelocityTOD)
{
   mVelocityTOD      = aVelocityTOD;
   mTODVelocityIsSet = true;
   mWCSVelocityIsSet = false;
   mECIVelocityIsSet = false;

   mTODVelocityIsValid = true;
   mWCSVelocityIsValid = false;
   mECIVelocityIsValid = false;
}

//! Set the J2000 velocity.
void UtECI_Conversion::SetVelocityJ2000(const UtVec3d& aVelocityJ2000)
{
   if (mCentralPointPtr->IsEarth())
   {
      SetVelocityECI(mFrameTie * aVelocityJ2000);
   }
   else
   {
      SetVelocityECI(aVelocityJ2000);
   }
}

//! Get the J2000 velocity.
UtVec3d UtECI_Conversion::GetVelocityJ2000() const
{
   if (mCentralPointPtr->IsEarth())
   {
      return mFrameTieTranspose * mVelocityECI;
   }
   else
   {
      return mVelocityECI;
   }
}

//! Get the TEME velocity.
UtVec3d UtECI_Conversion::GetVelocityTEME() const
{
   UtVec3d velocityTEME;
   UpdateWCS_Velocity();
   if (mUsingPolarMotion)
   {
      velocityTEME = mTEME_ToPEF_Transform.TransposeMultiply(mPolarMotion * (mVelocityWCS + GetOmegaCrossR()));
   }
   else
   {
      velocityTEME = mTEME_ToPEF_Transform.TransposeMultiply(mVelocityWCS + GetOmegaCrossR());
   }
   return velocityTEME;
}

//! Set the TEME velocity.
void UtECI_Conversion::SetVelocityTEME(const UtVec3d& aVelocityTEME)
{
   // Immediately convert to WCS velocity
   mECIVelocityIsValid = false;
   mTODVelocityIsValid = false;

   UpdateTransforms();
   if (mUsingPolarMotion)
   {
      mVelocityWCS = mPolarMotionTranspose * (mTEME_ToPEF_Transform * aVelocityTEME - GetOmegaCrossR());
   }
   else
   {
      mVelocityWCS = mTEME_ToPEF_Transform * aVelocityTEME - GetOmegaCrossR();
   }
   mWCSVelocityIsValid = true;

   mWCSVelocityIsSet = true;
   mECIVelocityIsSet = false;
   mTODVelocityIsSet = false;
}

//! Get the ECI location.
UtVec3d UtECI_Conversion::GetLocationECI() const
{
   UpdateECI_Location();
   return mLocationECI;
}

//! Update the TOD velocity given a valid ECI velocity or
//! valid WCS location and velocity.
void UtECI_Conversion::UpdateTOD_Velocity() const
{
   if (!mTODVelocityIsValid)
   {
      UpdateTransforms();
      assert(mWCSVelocityIsValid || mECIVelocityIsValid);

      if (mWCSVelocityIsValid)
      {
         if (mUsingPolarMotion)
         {
            mVelocityTOD = mS * (mPolarMotion * mVelocityWCS + GetOmegaCrossR());
         }
         else
         {
            mVelocityTOD = mS * (mVelocityWCS + GetOmegaCrossR());
         }
      }
      else // ECI is valid
      {
         mVelocityTOD = mNtPtFt * mVelocityECI;
      }

      mTODVelocityIsValid = true;
   }
}

//! Get the WCS velocity.
UtVec3d UtECI_Conversion::GetVelocityWCS() const
{
   UpdateWCS_Velocity();
   return mVelocityWCS;
}

//! Get the ECI velocity.
UtVec3d UtECI_Conversion::GetVelocityECI() const
{
   UpdateECI_Velocity();
   return mVelocityECI;
}

//! Get the TOD velocity.
UtVec3d UtECI_Conversion::GetVelocityTOD() const
{
   UpdateTOD_Velocity();
   return mVelocityTOD;
}

//! Get the WCS acceleration.
UtVec3d UtECI_Conversion::GetAccelerationWCS() const
{
   UpdateWCS_Acceleration();
   return mAccelerationWCS;
}

//! Get the ECI acceleration.
UtVec3d UtECI_Conversion::GetAccelerationECI() const
{
   UpdateECI_Acceleration();
   return mAccelerationECI;
}

//! Get the TOD acceleration.
UtVec3d UtECI_Conversion::GetAccelerationTOD() const
{
   UpdateTOD_Acceleration();
   return mAccelerationTOD;
}

//! Set the ECI acceleration.
void UtECI_Conversion::SetAccelerationECI(const UtVec3d& aAccelerationECI)
{
   mAccelerationECI      = aAccelerationECI;
   mECIAccelerationIsSet = true;
   mWCSAccelerationIsSet = false;
   mTODAccelerationIsSet = false;

   mECIAccelerationIsValid = true;
   mWCSAccelerationIsValid = false;
   mTODAccelerationIsValid = false;
}

//! Set the TOD acceleration.
void UtECI_Conversion::SetAccelerationTOD(const UtVec3d& aAccelerationTOD)
{
   mAccelerationTOD      = aAccelerationTOD;
   mTODAccelerationIsSet = true;
   mECIAccelerationIsSet = false;
   mWCSAccelerationIsSet = false;

   mTODAccelerationIsValid = true;
   mECIAccelerationIsValid = false;
   mWCSAccelerationIsValid = false;
}

//! Set the acceleration in the WCS coordinate frame.
void UtECI_Conversion::SetAccelerationWCS(const UtVec3d& aAccelerationWCS)
{
   mAccelerationWCS      = aAccelerationWCS;
   mWCSAccelerationIsSet = true;
   mECIAccelerationIsSet = false;
   mTODAccelerationIsSet = false;

   mWCSAccelerationIsValid = true;
   mECIAccelerationIsValid = false;
   mTODAccelerationIsValid = false;
}

//! Get the acceleration in the TEME coordinate frame.
//! @note This method could be optimized further, but it is assumed that it will be used primarily for testing.
UtVec3d UtECI_Conversion::GetAccelerationTEME() const
{
   UpdateTransforms();
   UpdateWCS_Acceleration();

   UtVec3d accelerationTEME;
   UtMat3d pef_ToTEME_Transform = mTEME_ToPEF_Transform;
   pef_ToTEME_Transform.Transpose();

   // Convert from WCS acceleration.
   if (mUsingPolarMotion)
   {
      accelerationTEME = pef_ToTEME_Transform *
                         (mPolarMotion * mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
   }
   else
   {
      accelerationTEME =
         pef_ToTEME_Transform * (mAccelerationWCS + GetCentripetalAcceleration() + GetCoriolisAcceleration());
   }

   return accelerationTEME;
}

//! Set the acceleration in the TEME coordinate frame.
void UtECI_Conversion::SetAccelerationTEME(const UtVec3d& aAccelerationTEME)
{
   mECIAccelerationIsValid = false;
   mECIAccelerationIsSet   = false;

   // Immediately convert to WCS acceleration
   UpdateTransforms();

   if (mUsingPolarMotion)
   {
      mAccelerationWCS = mPolarMotionTranspose * (mTEME_ToPEF_Transform * aAccelerationTEME -
                                                  GetCentripetalAcceleration() - GetCoriolisAcceleration());
   }
   else
   {
      mAccelerationWCS =
         mTEME_ToPEF_Transform * aAccelerationTEME - GetCentripetalAcceleration() - GetCoriolisAcceleration();
   }

   mWCSAccelerationIsValid = true;
   mWCSAccelerationIsSet   = true;
}

//! Return the WCS->ECI transform matrix.
const UtMat3d& UtECI_Conversion::GetWCS_ToECI_Transform() const
{
   UpdateTransforms();
   return mWCS_ToECI_Transform;
}

//! Return the ECI->WCS transform matrix.
const UtMat3d& UtECI_Conversion::GetECI_ToWCS_Transform() const
{
   UpdateTransforms();
   return mECI_ToWCS_Transform;
}

//! Return the WCS->ECI transform matrix (legacy format).
void UtECI_Conversion::GetWCS_ToECI_Transform(double aWCS_ToECITransform[3][3]) const
{
   UpdateTransforms();
   aWCS_ToECITransform[0][0] = mWCS_ToECI_Transform[0];
   aWCS_ToECITransform[0][1] = mWCS_ToECI_Transform[1];
   aWCS_ToECITransform[0][2] = mWCS_ToECI_Transform[2];
   aWCS_ToECITransform[1][0] = mWCS_ToECI_Transform[3];
   aWCS_ToECITransform[1][1] = mWCS_ToECI_Transform[4];
   aWCS_ToECITransform[1][2] = mWCS_ToECI_Transform[5];
   aWCS_ToECITransform[2][0] = mWCS_ToECI_Transform[6];
   aWCS_ToECITransform[2][1] = mWCS_ToECI_Transform[7];
   aWCS_ToECITransform[2][2] = mWCS_ToECI_Transform[8];
}

//! Return the ECI->WCS transform matrix (legacy format).
void UtECI_Conversion::GetECI_ToWCS_Transform(double aECI_ToWCSTransform[3][3]) const
{
   UpdateTransforms();
   aECI_ToWCSTransform[0][0] = mECI_ToWCS_Transform[0];
   aECI_ToWCSTransform[0][1] = mECI_ToWCS_Transform[1];
   aECI_ToWCSTransform[0][2] = mECI_ToWCS_Transform[2];
   aECI_ToWCSTransform[1][0] = mECI_ToWCS_Transform[3];
   aECI_ToWCSTransform[1][1] = mECI_ToWCS_Transform[4];
   aECI_ToWCSTransform[1][2] = mECI_ToWCS_Transform[5];
   aECI_ToWCSTransform[2][0] = mECI_ToWCS_Transform[6];
   aECI_ToWCSTransform[2][1] = mECI_ToWCS_Transform[7];
   aECI_ToWCSTransform[2][2] = mECI_ToWCS_Transform[8];
}

//! Given a location in WCS coordinates, convert to ECI.
UtVec3d UtECI_Conversion::ConvertWCS_ToECI(const UtVec3d& aLocationWCS) const
{
   UpdateTransforms();
   return mWCS_ToECI_Transform * aLocationWCS;
}

//! Given a location in WCS coordinates, convert to ECI (legacy format).
void UtECI_Conversion::ConvertWCS_ToECI(const double aLocationWCS[3], double aLocationECI[3]) const
{
   UpdateTransforms();
   UtVec3d locWCS(aLocationWCS);
   UtVec3d locECI = ConvertWCS_ToECI(locWCS);
   UtVec3d::Set(aLocationECI, locECI.GetData());
}

//! Given a location in ECI coordinates, convert to WCS.
UtVec3d UtECI_Conversion::ConvertECI_ToWCS(const UtVec3d& aLocationECI) const
{
   UpdateTransforms();
   return mECI_ToWCS_Transform * aLocationECI;
}

//! Given a location in ECI coordinates, convert to TOD.
UtVec3d UtECI_Conversion::ConvertECI_ToTOD(const UtVec3d& aLocationECI) const
{
   UpdateTransforms();
   return mNtPtFt * aLocationECI;
}

//! Given a velocity in ECI coordinates, convert to TOD (legacy format).
UtVec3d UtECI_Conversion::ConvertVelocityECI_ToTOD(const UtVec3d& aVelocityECI) const
{
   UpdateTransforms();
   return mNtPtFt * aVelocityECI;
}

//! Given a location in ECI coordinates, convert to TEME.
UtVec3d UtECI_Conversion::ConvertECI_ToTEME(const UtVec3d& aLocationECI) const
{
   UpdateTransforms();
   UtVec3d locPEF = mSt * mNtPtFt * aLocationECI;
   return mTEME_ToPEF_Transform.TransposeMultiply(locPEF);
}

//! Given a velocity in ECI coordinates, convert to TEME.
UtVec3d UtECI_Conversion::ConvertVelocityECI_ToTEME(const UtVec3d& aVelocityECI) const
{
   UpdateTransforms();
   UtVec3d velIntermediate = mSt * mNtPtFt * aVelocityECI;
   return mTEME_ToPEF_Transform.TransposeMultiply(velIntermediate);
}

//! Given a location in ECI coordinates, convert to WCS (legacy format).
void UtECI_Conversion::ConvertECI_ToWCS(const double aLocationECI[3], double aLocationWCS[3]) const
{
   UpdateTransforms();
   UtVec3d locECI(aLocationECI);
   UtVec3d locWCS = ConvertECI_ToWCS(locECI);
   UtVec3d::Set(aLocationWCS, locWCS.GetData());
}

//! Given a velocity in ECI coordinates, convert to WCS (legacy format).
UtVec3d UtECI_Conversion::ConvertVelocityECI_ToWCS(const UtVec3d& aLocationECI, const UtVec3d& aVelECI) const
{
   UpdateTransforms();

   UtVec3d locPEF = mSt * mNtPtFt * aLocationECI;
   UtVec3d omegaCrossR;
   omegaCrossR.CrossProduct(mOmegaPEF, locPEF);
   UtVec3d velWCS = mSt * mNtPtFt * aVelECI - omegaCrossR;

   if (mUsingPolarMotion)
   {
      velWCS = mPolarMotionTranspose * velWCS;
   }

   return velWCS;
}

//! Given a velocity in ECI coordinates, convert to WCS (legacy format).
void UtECI_Conversion::ConvertVelocityECI_ToWCS(const double aLocationECI[3], const double aVelECI[3], double aVelWCS[3]) const
{
   UtVec3d locECI(aLocationECI);
   UtVec3d velECI(aVelECI);
   UtVec3d velWCS = ConvertVelocityECI_ToWCS(locECI, velECI);
   UtVec3d::Set(aVelWCS, velWCS.GetData());
}

//! Given a velocity in WCS coordinates, convert to ECI.
//! @param aLocationWCS The location at which the velocity is computed.
//! @aVelWCS The WCS-referenced velocity.
//! @return the ECI-referenced velocity.
UtVec3d UtECI_Conversion::ConvertVelocityWCS_ToECI(const UtVec3d& aLocationWCS, const UtVec3d& aVelWCS) const
{
   UpdateTransforms();

   UtVec3d locPEF = aLocationWCS;
   if (mUsingPolarMotion)
   {
      locPEF = mPolarMotion * locPEF;
   }

   UtVec3d omegaCrossR;
   omegaCrossR.CrossProduct(mOmegaPEF, locPEF);
   UtVec3d velECI;

   if (mUsingPolarMotion)
   {
      velECI = mFPN * mS * (mPolarMotion * aVelWCS + omegaCrossR);
   }
   else
   {
      velECI = mFPN * mS * (aVelWCS + omegaCrossR);
   }

   return velECI;
}

//! Given a velocity in WCS coordinates, convert to ECI (legacy format).
void UtECI_Conversion::ConvertVelocityWCS_ToECI(const double aLocationWCS[3], const double aVelWCS[3], double aVelECI[3]) const
{
   UtVec3d locWCS(aLocationWCS);
   UtVec3d velWCS(aVelWCS);
   UtVec3d velECI = ConvertVelocityWCS_ToECI(locWCS, velWCS);
   UtVec3d::Set(aVelECI, velECI.GetData());
}

//! Given an acceleration in ECI coordinates, convert to WCS.
UtVec3d UtECI_Conversion::ConvertAccelerationECI_ToWCS(const UtVec3d& aLocationECI,
                                                       const UtVec3d& aVelECI,
                                                       const UtVec3d& aAccelECI) const
{
   UpdateTransforms();
   UtMat3d Teci_pef = mSt * mNtPtFt;
   UtVec3d locPEF   = Teci_pef * aLocationECI;

   // compute the pef velocity from eci
   UtVec3d omegaCrossR;
   omegaCrossR.CrossProduct(mOmegaPEF, locPEF);

   // The intermediate value (omega x r) is used to compute the PEF velocity:
   UtVec3d velPEF = Teci_pef * aVelECI - omegaCrossR;

   UtVec3d centripetalAcc;
   centripetalAcc.CrossProduct(mOmegaPEF, centripetalAcc);
   UtVec3d coriolisAcc;
   coriolisAcc.CrossProduct(2.0 * mOmegaPEF, velPEF);
   UtVec3d accWCS;
   if (mUsingPolarMotion)
   {
      accWCS = mPolarMotionTranspose * (Teci_pef * mAccelerationECI - centripetalAcc - coriolisAcc);
   }
   else
   {
      accWCS = Teci_pef * mAccelerationECI - centripetalAcc - coriolisAcc;
   }

   return accWCS;
}

//! Given an acceleration in ECI coordinates, convert to WCS.
UtVec3d UtECI_Conversion::ConvertAccelerationWCS_ToECI(const UtVec3d& aLocationWCS,
                                                       const UtVec3d& aVelWCS,
                                                       const UtVec3d& aAccelWCS) const
{
   UpdateTransforms();

   UtVec3d locPEF = aLocationWCS;
   UtVec3d velPEF = aVelWCS;
   if (mUsingPolarMotion)
   {
      locPEF = mPolarMotion * aLocationWCS;
      velPEF = mPolarMotion * aVelWCS;
   }

   // compute the pef velocity from eci
   UtVec3d omegaCrossR;
   omegaCrossR.CrossProduct(mOmegaPEF, locPEF);
   UtVec3d centripetalAcc;
   centripetalAcc.CrossProduct(mOmegaPEF, centripetalAcc);
   UtVec3d coriolisAcc;
   coriolisAcc.CrossProduct(2.0 * mOmegaPEF, velPEF);
   UtVec3d accECI;
   if (mUsingPolarMotion)
   {
      accECI = mFPN * mS * (mPolarMotion * mAccelerationWCS + centripetalAcc + coriolisAcc);
   }
   else
   {
      accECI = mFPN * mS * (mAccelerationWCS + centripetalAcc + coriolisAcc);
   }

   return accECI;
}

//! Given an acceleration in ECI coordinates, convert to WCS (legacy format).
void UtECI_Conversion::ConvertAccelerationECI_ToWCS(const double aLocationECI[3],
                                                    const double aVelECI[3],
                                                    const double aAccelECI[3],
                                                    double       aAccelWCS[3]) const
{
   UtVec3d locECI(aLocationECI);
   UtVec3d velECI(aVelECI);
   UtVec3d accelECI(aAccelECI);
   UtVec3d::Set(aAccelWCS, ConvertAccelerationECI_ToWCS(locECI, velECI, accelECI).GetData());
}

//! Given an acceleration in ECI coordinates, convert to WCS (legacy format).
void UtECI_Conversion::ConvertAccelerationWCS_ToECI(const double aLocationWCS[3],
                                                    const double aVelWCS[3],
                                                    const double aAccelWCS[3],
                                                    double       aAccelECI[3]) const
{
   UtVec3d locWCS(aLocationWCS);
   UtVec3d velWCS(aVelWCS);
   UtVec3d accelWCS(aAccelWCS);
   UtVec3d::Set(aAccelECI, ConvertAccelerationWCS_ToECI(locWCS, velWCS, accelWCS).GetData());
}

//! Get the PEF location.
UtVec3d UtECI_Conversion::GetLocationPEF() const
{
   UpdatePEF_Location();
   return mLocationPEF;
}

//! Get the ECI location.
UtVec3d UtECI_Conversion::GetLocationJ2000() const
{
   UpdateTransforms();
   UpdateECI_Location();
   if (mCentralPointPtr->IsEarth())
   {
      return mFrameTieTranspose * mLocationECI;
   }
   else
   {
      return mLocationECI;
   }
}

//! Get the location in the Mean of Date (MOD) frame (for test only).
UtVec3d UtECI_Conversion::GetLocationMOD() const
{
   UpdateTransforms();
   UpdateWCS_Location();
   UtVec3d locMOD = mCentralPointPtr->ComputeNutation(mEpoch) * mS * mPolarMotion * mLocationWCS;
   return locMOD;
}

//! Compute the sidereal motion matrices.
void UtECI_Conversion::ComputeSiderealMotion() const
{
   mS = mCentralPointPtr->ComputeSiderealMotion(mEpoch);

   if (mCentralPointPtr->IsEarth())
   {
      ComputeSiderealMotionTEME();
   }
}

//! Compute and return the Coriolis acceleration.
//! @note When using polar motion this calculation is approximated using the WCS (not PEF) velocity.
UtVec3d UtECI_Conversion::GetCoriolisAcceleration() const
{
   UtVec3d coriolisAcc;
   coriolisAcc.CrossProduct(2.0 * mOmegaPEF, GetVelocityWCS());
   return coriolisAcc;
}

//! Compute and return the centripetal acceleration.
UtVec3d UtECI_Conversion::GetCentripetalAcceleration() const
{
   UtVec3d omegaCrossR = GetOmegaCrossR();
   UtVec3d centripetalAcc;
   centripetalAcc.CrossProduct(mOmegaPEF, omegaCrossR);
   return centripetalAcc;
}

//! Compute and return the cross product of the rotation vector and PEF location,
//! using in non-inertial velocity and acceleration calculations.
UtVec3d UtECI_Conversion::GetOmegaCrossR() const
{
   UtVec3d omegaCrossR;
   omegaCrossR.CrossProduct(mOmegaPEF, GetLocationPEF());
   return omegaCrossR;
}

void UtECI_Conversion::UpdateTransforms() const
{
   if (!mECI_TransformsValid)
   {
      if (!mN_TransformValid)
      {
         mN  = mCentralPointPtr->ComputeNutation(mEpoch);
         mNt = mN;
         mNt.Transpose();
         mN_TransformValid = true;
      }

      UtMat3d precession = mCentralPointPtr->ComputePrecession(mEpoch);
      if (mCentralPointPtr->IsEarth())
      {
         mFPN = mFrameTie * precession * mN;
      }
      else
      {
         mFPN = precession * mN;
      }
      precession.Transpose();
      if (mCentralPointPtr->IsEarth())
      {
         mNtPtFt = mNt * precession * mFrameTieTranspose;
      }
      else
      {
         mNtPtFt = mNt * precession;
      }

      ComputeSiderealMotion(); // -> mS
      mSt = mS;
      mSt.Transpose();

      if (mUsingPolarMotion)
      {
         mWCS_ToECI_Transform = mFPN * mS * mPolarMotion;
      }
      else
      {
         mWCS_ToECI_Transform = mFPN * mS;
      }

      mECI_ToWCS_Transform = mWCS_ToECI_Transform;
      mECI_ToWCS_Transform.Transpose();

      mECI_TransformsValid = true;
   }
}

// private
void UtECI_Conversion::UpdateWCS_Location() const
{
   if (!mWCSLocationIsValid)
   {
      assert(mECILocationIsValid || mTODLocationIsValid);
      UpdateTransforms();

      if (mECILocationIsValid)
      {
         mLocationWCS = mECI_ToWCS_Transform * mLocationECI;
      }
      else
      {
         if (mUsingPolarMotion)
         {
            mLocationWCS = mPolarMotionTranspose * mSt * mLocationTOD;
         }
         else
         {
            mLocationWCS = mSt * mLocationTOD;
         }
      }
      mWCSLocationIsValid = true;
   }
}

void UtECI_Conversion::ComputeSiderealMotionTEME() const
{
   // The following is used for TEME conversions as defined by SPG4 and related propagators.
   double gmst1982          = mEpoch.GetGMST_1982() / UtCalendar::cSEC_IN_DAY * UtMath::cTWO_PI;
   double cosGMST           = cos(gmst1982);
   double sinGMST           = sin(gmst1982);
   mTEME_ToPEF_Transform[0] = cosGMST;
   mTEME_ToPEF_Transform[1] = sinGMST;
   mTEME_ToPEF_Transform[3] = -sinGMST;
   mTEME_ToPEF_Transform[4] = cosGMST;
}
