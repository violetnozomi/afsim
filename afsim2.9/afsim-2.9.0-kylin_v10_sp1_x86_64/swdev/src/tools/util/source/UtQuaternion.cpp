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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtQuaternion.hpp"

//#define ASSERT_TRANSFORMS 1

#include <cassert>

#include "UtDCM.hpp"
#include "UtMath.hpp"

namespace
{
constexpr double cALMOST_ONE = 0.999998;
}

// ============================================================================
// This is a non-class operation, but turns it into a previously-defined class operation...
UtQuaternion operator*(double aLHS, const UtQuaternion& aRHS)
{
   return aRHS * aLHS;
}

// ============================================================================
UtQuaternion::UtQuaternion()
   : mA(1.0)
   , mI(0.0)
   , mJ(0.0)
   , mK(0.0)
{
}

// ============================================================================
UtQuaternion::UtQuaternion(double aA, double aI, double aJ, double aK)
   : mA(aA)
   , mI(aI)
   , mJ(aJ)
   , mK(aK){};

// ============================================================================
//! Re-normalizes the quaternion.
//! A quaternion orientation should always have a magnitude of one, so after
//! constructing and/or extended time propagation, it should be re-normalized.
void UtQuaternion::Normalize()
{
   double imag = 1.0 / sqrt(mA * mA + mI * mI + mJ * mJ + mK * mK);
   mA *= imag;
   mI *= imag;
   mJ *= imag;
   mK *= imag;
}

// ============================================================================
//! Returns the conjugate of the quaternion
UtQuaternion UtQuaternion::Conjugate() const
{
   return UtQuaternion(mA, -mI, -mJ, -mK);
}

// ============================================================================
//! Takes the vector dot product of a quaternion with another quaternion
double UtQuaternion::Dot(const UtQuaternion& aOther) const
{
   return mA * aOther.mA + mI * aOther.mI + mJ * aOther.mJ + mK * aOther.mK;
}

// ============================================================================
// protected
//! Internal utility method used in constructing a quaternion from a DCM.
void UtQuaternion::CalcQi(size_t aKnown, size_t aUnknown, double aDestination[4], const double aDCM[3][3]) const
{
   double temp = 0;

   switch (aKnown)
   {
   case 0:
      switch (aUnknown)
      {
      case 1:
         temp = aDCM[1][2] - aDCM[2][1];
         break;
      case 2:
         temp = aDCM[2][0] - aDCM[0][2];
         break;
      case 3:
         temp = aDCM[0][1] - aDCM[1][0];
      }
      break;

   case 1:
      switch (aUnknown)
      {
      case 0:
         temp = aDCM[1][2] - aDCM[2][1];
         break;
      case 2:
         temp = aDCM[0][1] + aDCM[1][0];
         break;
      case 3:
         temp = aDCM[0][2] + aDCM[2][0];
      }
      break;

   case 2:
      switch (aUnknown)
      {
      case 0:
         temp = aDCM[2][0] - aDCM[0][2];
         break;
      case 1:
         temp = aDCM[0][1] + aDCM[1][0];
         break;
      case 3:
         temp = aDCM[1][2] + aDCM[2][1];
      }
      break;

   case 3:
      switch (aUnknown)
      {
      case 0:
         temp = aDCM[0][1] - aDCM[1][0];
         break;
      case 1:
         temp = aDCM[0][2] + aDCM[2][0];
         break;
      case 2:
         temp = aDCM[1][2] + aDCM[2][1];
      }
   }

   aDestination[aUnknown] = 0.25 * temp / aDestination[aKnown];
}

// ============================================================================
//! Construct a quaternion from initial Euler Angles.
//! (Psi is Yaw, Theta is Pitch, Phi is Roll, in that order.)
UtQuaternion::UtQuaternion(double aPsi, double aTheta, double aPhi)
{
   Set(aPsi, aTheta, aPhi);
}

// ============================================================================
//! Construct a quaternion from initial Euler Angles.
//! (Psi is Yaw, Theta is Pitch, Phi is Roll, in that order.)
UtQuaternion::UtQuaternion(const ut::coords::EulerAngles& aAngles)
   : UtQuaternion(aAngles.mPsi, aAngles.mTheta, aAngles.mPhi)
{
}

// ============================================================================
//! Construct a quaternion from an axis and angle (we assume the axis is normal)
UtQuaternion::UtQuaternion(const double aAngle, const double aVector[3])
{
   double unitVector[3];
   UtVec3d::Set(unitVector, aVector);
   UtVec3d::Normalize(unitVector);

   double sinHalfAng = sin(0.5 * aAngle);
   mA                = cos(0.5 * aAngle);
   mI                = unitVector[0] * sinHalfAng;
   mJ                = unitVector[1] * sinHalfAng;
   mK                = unitVector[2] * sinHalfAng;
}

// ============================================================================
//! Construct a quaternion from an axis and angle (we assume the axis is normal)
UtQuaternion::UtQuaternion(const double aAngle, const UtVec3dX& aVector)
{
   UtVec3dX tempUnit = aVector;
   tempUnit.Normalize();
   double sinHalfAng = sin(0.5 * aAngle);
   mA                = cos(0.5 * aAngle);
   mI                = tempUnit[0] * sinHalfAng;
   mJ                = tempUnit[1] * sinHalfAng;
   mK                = tempUnit[2] * sinHalfAng;
}

// ============================================================================
//! Overwrite the current orientation with a new one.
//! (Psi is Yaw, Theta is Pitch, Phi is Roll.)
void UtQuaternion::Set(double aPsi, double aTheta, double aPhi)
{
   double sx2 = sin(aPhi * 0.5);
   double cx2 = cos(aPhi * 0.5);
   double sy2 = sin(aTheta * 0.5);
   double cy2 = cos(aTheta * 0.5);
   double sz2 = sin(aPsi * 0.5);
   double cz2 = cos(aPsi * 0.5);

   mA = cx2 * cy2 * cz2 + sx2 * sy2 * sz2;
   mI = sx2 * cy2 * cz2 - cx2 * sy2 * sz2;
   mJ = cx2 * sy2 * cz2 + sx2 * cy2 * sz2;
   mK = cx2 * cy2 * sz2 - sx2 * sy2 * cz2;
}

// ============================================================================
//! Overwrite the current orientation with a new one.
//! (The DirectionCosineMatrix supplies the values needed.)
void UtQuaternion::Set(const UtDCM& aDCM)
{
   double tempDCM[3][3];
#ifdef ASSERT_TRANSFORMS
   assert(aDCM.IsValid());
#endif
   aDCM.Get(tempDCM);
   Set(tempDCM);
}

// ============================================================================
//! Overwrite the current orientation with a new one.
//! (The DirectionCosineMatrix supplies the values needed.)
void UtQuaternion::Set(const double aDCM[3][3])
{
   double temp[4]; // temporary array used twice... first to get largest of the
   // 4 quaternion values, then to hold newly calculated ones
   double tmax = 0.0;
   size_t sub  = 0;

   double d00 = aDCM[0][0];
   double d11 = aDCM[1][1];
   double d22 = aDCM[2][2];

   temp[0] = 1 + d00 + d11 + d22;
   temp[1] = 1 + d00 - d11 - d22;
   temp[2] = 1 - d00 + d11 - d22;
   temp[3] = 1 - d00 - d11 + d22;

   // Find the largest component of the temporary quaternion:
   for (size_t i = 0; i < 4; ++i)
   {
      if (temp[i] > tmax)
      {
         sub  = i;
         tmax = temp[i];
      }
   }

   // The largest component of the quaternion is calculated first.
   temp[sub] = sqrt(tmax / 4.);

   // Now, with some difficulty, calculate the other 3 values:
   for (size_t j = 0; j < 4; ++j)
   {
      if (j != sub)
      {
         CalcQi(sub, j, temp, aDCM);
      }
   }

   // Assign member values from the temporary array:
   mA = temp[0];
   mI = temp[1];
   mJ = temp[2];
   mK = temp[3];

   Normalize();
}

// ============================================================================
//! Construct a quaternion from a Direction Cosine Matrix (DCM).
UtQuaternion::UtQuaternion(const double aDCM[3][3])
{
   double temp[4]; // temporary array used twice... first to get largest of the
   // 4 quaternion values, then to hold newly calculated ones
   double tmax = 0.0;
   size_t sub  = 0;

   double d00 = aDCM[0][0];
   double d11 = aDCM[1][1];
   double d22 = aDCM[2][2];

   temp[0] = 1 + d00 + d11 + d22;
   temp[1] = 1 + d00 - d11 - d22;
   temp[2] = 1 - d00 + d11 - d22;
   temp[3] = 1 - d00 - d11 + d22;

   // Find the largest component of the temporary quaternion:
   for (size_t i = 0; i < 4; ++i)
   {
      if (temp[i] > tmax)
      {
         sub  = i;
         tmax = temp[i];
      }
   }

   // The largest component of the quaternion is calculated first.
   temp[sub] = sqrt(tmax / 4.);

   // Now, with some difficulty, calculate the other 3 values:
   for (size_t j = 0; j < 4; ++j)
   {
      if (j != sub)
      {
         CalcQi(sub, j, temp, aDCM);
      }
   }

   // Assign member values from the temporary array:
   mA = temp[0];
   mI = temp[1];
   mJ = temp[2];
   mK = temp[3];
}

// ============================================================================
//! Construct a quaternion from a UtDCM.
UtQuaternion::UtQuaternion(const UtDCM& aDCM)
{
   double dcm[3][3];
   aDCM.Get(dcm);

   double temp[4]; // temporary array used twice... first to get largest of the
   // 4 quaternion values, then to hold newly calculated ones
   double tmax = 0.0;
   size_t sub  = 0;

   double d00 = dcm[0][0];
   double d11 = dcm[1][1];
   double d22 = dcm[2][2];

   temp[0] = 1 + d00 + d11 + d22;
   temp[1] = 1 + d00 - d11 - d22;
   temp[2] = 1 - d00 + d11 - d22;
   temp[3] = 1 - d00 - d11 + d22;

   // Find the largest component of the temporary quaternion:
   for (size_t i = 0; i < 4; ++i)
   {
      if (temp[i] > tmax)
      {
         sub  = i;
         tmax = temp[i];
      }
   }

   // The largest component of the quaternion is calculated first.
   temp[sub] = sqrt(tmax / 4.);

   // Now, with some difficulty, calculate the other 3 values:
   for (size_t j = 0; j < 4; ++j)
   {
      if (j != sub)
      {
         CalcQi(sub, j, temp, dcm);
      }
   }

   // Assign member values from the temporary array:
   mA = temp[0];
   mI = temp[1];
   mJ = temp[2];
   mK = temp[3];
}

// ============================================================================
//! Equality operator.
bool UtQuaternion::operator==(const UtQuaternion& aSrc) const
{
   return (mA == aSrc.mA) && (mI == aSrc.mI) && (mJ == aSrc.mJ) && (mK == aSrc.mK);
}

// ============================================================================
//! Multiplication (Q*Q) operator.
UtQuaternion UtQuaternion::operator*(const UtQuaternion& aRHS) const
{
   return UtQuaternion(mA * aRHS.mA - mI * aRHS.mI - mJ * aRHS.mJ - mK * aRHS.mK,
                       mA * aRHS.mI + mI * aRHS.mA + mJ * aRHS.mK - mK * aRHS.mJ,
                       mA * aRHS.mJ - mI * aRHS.mK + mJ * aRHS.mA + mK * aRHS.mI,
                       mA * aRHS.mK + mI * aRHS.mJ - mJ * aRHS.mI + mK * aRHS.mA);
}

// ============================================================================
//! Multiplication (Q*k) operator.
UtQuaternion UtQuaternion::operator*(const double aRHS) const
{
   return UtQuaternion(mA * aRHS, mI * aRHS, mJ * aRHS, mK * aRHS);
}

// ============================================================================
//! Multiplication (Q*UtVec3dX) operator.
UtVec3dX UtQuaternion::operator*(const UtVec3dX& aVector) const
{
   return (*this).Transform(aVector);
}

// ============================================================================
//! Incremental summation (Q += dQ) operator.
UtQuaternion& UtQuaternion::operator+=(const UtQuaternion& aSrc)
{
   mA += aSrc.mA;
   mI += aSrc.mI;
   mJ += aSrc.mJ;
   mK += aSrc.mK;
   return *this;
}

// ============================================================================
//! Addition (Q + Q) operator.
UtQuaternion UtQuaternion::operator+(const UtQuaternion& aSrc) const
{
   return UtQuaternion(mA + aSrc.mA, mI + aSrc.mI, mJ + aSrc.mJ, mK + aSrc.mK);
}

// ============================================================================
//! Subtraction (Q - Q) operator.
UtQuaternion UtQuaternion::operator-(const UtQuaternion& aSrc) const
{
   return UtQuaternion(mA - aSrc.mA, mI - aSrc.mI, mJ - aSrc.mJ, mK - aSrc.mK);
}

// ============================================================================
//! Accessor to obtain the ith component of the Quaternion.
double UtQuaternion::operator[](int aIndex) const
{
   double val = 0.0;
   switch (aIndex)
   {
   case 0:
      val = mA;
      break;
   case 1:
      val = mI;
      break;
   case 2:
      val = mJ;
      break;
   case 3:
      val = mK;
   }
   return val;
}

// ============================================================================
//! Accessor to obtain the ith component of the Quaternion.
const double* UtQuaternion::operator()(int aIndex) const
{
   switch (aIndex)
   {
   case 0:
      return &mA;
   case 1:
      return &mI;
   case 2:
      return &mJ;
   case 3:
      return &mK;
   }
   return &mA; // should not get here, but compiler requires it.
}

// ============================================================================
//! Calculates a quaternion rate from local body rates.
//! The aPQR vector is (X,Y,Z) body-axis rates, in rad per sec.
//! Do not re-normalize this vector before use, as it is an
//! incremental change value, with non-unity magnitude.
UtQuaternion UtQuaternion::GetRate(const double aPQR[3]) const
{
   UtQuaternion rv;
   rv.SetRate(*this, aPQR);
   return rv;
}

// ============================================================================
//! Calculates a quaternion rate from local body rates.
//! The aPQR vector is (X,Y,Z) body-axis rates, in rad per sec.
//! Do not re-normalize this vector before use, as it is an
//! incremental change value, with non-unity magnitude.
UtQuaternion UtQuaternion::GetRate(const UtVec3dX& aPQR) const
{
   UtQuaternion rv;
   rv.SetRate(*this, aPQR.GetData());
   return rv;
}

// ============================================================================
//! Sets a Quaternion to a Rate, or Qdot, value, from an orientation and local body rates.
//! The aPQR vector is (X,Y,Z) body-axis rates, in rad per sec.
//! Do not re-normalize this vector before use, as it is an
//! incremental change value, with non-unity magnitude.
void UtQuaternion::SetRate(const UtQuaternion& aQuat, const double aPQR[3])
{
   // EQUATION:  qRate = 1/2 * omegaBody grassmann product
   mA = 0.5 * (-aPQR[0] * aQuat.mI - aPQR[1] * aQuat.mJ - aPQR[2] * aQuat.mK);
   mI = 0.5 * (aPQR[0] * aQuat.mA + aPQR[2] * aQuat.mJ - aPQR[1] * aQuat.mK);
   mJ = 0.5 * (aPQR[1] * aQuat.mA - aPQR[2] * aQuat.mI + aPQR[0] * aQuat.mK);
   mK = 0.5 * (aPQR[2] * aQuat.mA + aPQR[1] * aQuat.mI - aPQR[0] * aQuat.mJ);
}

// ============================================================================
//! Sets a Quaternion to a Rate, or Qdot, value, from an orientation and local body rates.
//! The aPQR vector is (X,Y,Z) body-axis rates, in rad per sec.
//! Do not re-normalize this vector before use, as it is an
//! incremental change value, with non-unity magnitude.
void UtQuaternion::SetRate(const UtQuaternion& aQuat, const UtVec3dX& aPQR)
{
   // EQUATION:  qRate = 1/2 * omegaBody grassmann product
   mA = 0.5 * (-aPQR[0] * aQuat.mI - aPQR[1] * aQuat.mJ - aPQR[2] * aQuat.mK);
   mI = 0.5 * (aPQR[0] * aQuat.mA + aPQR[2] * aQuat.mJ - aPQR[1] * aQuat.mK);
   mJ = 0.5 * (aPQR[1] * aQuat.mA - aPQR[2] * aQuat.mI + aPQR[0] * aQuat.mK);
   mK = 0.5 * (aPQR[2] * aQuat.mA + aPQR[1] * aQuat.mI - aPQR[0] * aQuat.mJ);
}

// ============================================================================
//! Propagates the body-relative rotation rates forward to a new orientation.
//! The aPQR vector is (X,Y,Z) body-axis rates, in rad per sec.
//! This function does not require that the aDeltaT value be kept small.
UtQuaternion UtQuaternion::Propagate(const double aPQR[3], double aDeltaT) const
{
   // Construct a quaternion from the rates vector.
   // (Note the reversing of the indices.)
   UtQuaternion qRate(aPQR[2], aPQR[1], aPQR[0]);

   // Rotate quaternion to a new future orientation.
   UtQuaternion result = *this * qRate.Power(aDeltaT);

   return result;
}

// ============================================================================
//! Propagates the body-relative rotation rates forward to a new orientation.  This function
//! degrades in accuracy at larger time steps with higher rotation rates; the context
//! is assumed to be digital forward integration of platform motion state in small steps.
//! If this is not the case, use the Propagate() function, which does not assume "small"
//! increments.
UtQuaternion UtQuaternion::Rotate(const double aPQR[3], double aDeltaT) const
{
   UtQuaternion qRate;
   qRate.SetRate(*this, aPQR);
   UtQuaternion result;
   result = *this + qRate * aDeltaT;
   return result;
}

// ============================================================================
//! Propagates the body-relative rotation rates forward to a new orientation.  This function
//! degrades in accuracy at larger time steps with higher rotation rates; the context
//! is assumed to be digital forward integration of platform motion state in small steps.
//! If this is not the case, use the Propagate() function, which does not assume "small"
//! increments.
void UtQuaternion::Update(const UtQuaternion& aQrate, double aDeltaT)
{
   *this += aQrate * aDeltaT;

   // The application should take responsibility for re-normalizing "occasionally",
   // and a normalization is not needed each time step, so to do this here is
   // computation-intensive.   BUT the benefit is that it makes the class more
   // "idiot proof", so for now, do it anyway!
   Normalize();
}

// ============================================================================
//! Obtain a Direction Cosine Matrix (DCM) from the quaternion.
//! Algorithms should prefer the use of the DCM if possible, over the
//! use of Euler Angles, for computational efficiency.
void UtQuaternion::Get(double aDCM[3][3]) const
{
   aDCM[0][0] = mA * mA + mI * mI - mJ * mJ - mK * mK;
   aDCM[0][1] = 2.0 * (mI * mJ + mA * mK);
   aDCM[0][2] = 2.0 * (mI * mK - mA * mJ);

   aDCM[1][0] = 2.0 * (mI * mJ - mA * mK);
   aDCM[1][1] = mA * mA - mI * mI + mJ * mJ - mK * mK;
   aDCM[1][2] = 2.0 * (mJ * mK + mA * mI);

   aDCM[2][0] = 2.0 * (mI * mK + mA * mJ);
   aDCM[2][1] = 2.0 * (mJ * mK - mA * mI);
   aDCM[2][2] = mA * mA - mI * mI - mJ * mJ + mK * mK;
}

// ============================================================================
//! Obtain a Direction Cosine Matrix (DCM) from the quaternion.
void UtQuaternion::Get(UtDCM& aDCM) const
{
   aDCM.SetVal(0, 0, mA * mA + mI * mI - mJ * mJ - mK * mK);
   aDCM.SetVal(0, 1, 2.0 * (mI * mJ + mA * mK));
   aDCM.SetVal(0, 2, 2.0 * (mI * mK - mA * mJ));

   aDCM.SetVal(1, 0, 2.0 * (mI * mJ - mA * mK));
   aDCM.SetVal(1, 1, mA * mA - mI * mI + mJ * mJ - mK * mK);
   aDCM.SetVal(1, 2, 2.0 * (mJ * mK + mA * mI));

   aDCM.SetVal(2, 0, 2.0 * (mI * mK + mA * mJ));
   aDCM.SetVal(2, 1, 2.0 * (mJ * mK - mA * mI));
   aDCM.SetVal(2, 2, mA * mA - mI * mI - mJ * mJ + mK * mK);

#ifdef ASSERT_TRANSFORMS
   assert(aDCM.IsValid(true));
#endif
}

// ============================================================================
//! Obtain a Direction Cosine Matrix (DCM) from the quaternion.
UtDCM UtQuaternion::GetDCM() const
{
   UtDCM out;
   Get(out);
   return out;
}

// ============================================================================
//! Obtain Euler Angles from the quaternion.  Note that algorithms should
//! prefer the use of the DCM if possible, over the use of Euler Angles,
//! for computational efficiency.  This function internally calculates a
//! DCM, so if one is available elsewhere, it will be better to obtain
//! Euler angles from it, instead.
void UtQuaternion::Get(double& aPsi, double& aTheta, double& aPhi) const
{
   double dcm[3][3];
   Get(dcm);
   if (dcm[0][2] > cALMOST_ONE)
   {
      aPhi   = 0;
      aTheta = -UtMath::cPI_OVER_2;
      aPsi   = 2 * atan2(mI, mA);
   }
   else if (dcm[0][2] < -cALMOST_ONE)
   {
      aPhi   = 0;
      aTheta = UtMath::cPI_OVER_2;
      aPsi   = -2 * atan2(mI, mA);
   }
   else
   {
      aPhi   = atan2(dcm[1][2], dcm[2][2]);
      aTheta = asin(-dcm[0][2]);
      aPsi   = atan2(dcm[0][1], dcm[0][0]);
   }
}

// ============================================================================
//! Obtain Euler Angles from the quaternion.  Note that algorithms should
//! prefer the use of the DCM if possible, over the use of Euler Angles,
//! for computational efficiency.  This function internally calculates a
//! DCM, so if one is available elsewhere, it will be better to obtain
//! Euler angles from it, instead.
ut::coords::EulerAngles UtQuaternion::Get() const
{
   ut::coords::EulerAngles out;
   Get(out.mPsi, out.mTheta, out.mPhi);
   return out;
}

// ============================================================================
//! Linearly interpolate an orientation between two values
// static
UtQuaternion UtQuaternion::Slerp(const UtQuaternion& aLHS, const UtQuaternion& aRHS, const float aFactor)
{
   float        qbMulter = 1;
   UtQuaternion qm;
   // Calculate angle between them.
   double cosHalfTheta = aLHS.mA * aRHS.mA + aLHS.mI * aRHS.mI + aLHS.mJ * aRHS.mJ + aLHS.mK * aRHS.mK;

   // if aLHS=aRHS or aLHS=-aRHS then theta = 0 and we can return aLHS
   if (fabs(cosHalfTheta) >= 1.0)
   {
      return aLHS;
   }
   if (cosHalfTheta < 0)
   {
      qbMulter     = -1;
      cosHalfTheta = -cosHalfTheta;
   }
   // Calculate temporary values.
   double halfTheta    = acos(cosHalfTheta);
   double sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
   // if theta = 180 degrees then result is not fully defined
   // we could rotate around any axis normal to aLHS or aRHS
   if (fabs(sinHalfTheta) < 0.001)
   {
      qm.mA = (aLHS.mA * 0.5 + qbMulter * aRHS.mA * 0.5);
      qm.mI = (aLHS.mI * 0.5 + qbMulter * aRHS.mI * 0.5);
      qm.mJ = (aLHS.mJ * 0.5 + qbMulter * aRHS.mJ * 0.5);
      qm.mK = (aLHS.mK * 0.5 + qbMulter * aRHS.mK * 0.5);
      return qm;
   }
   double ratioA = sin((1 - aFactor) * halfTheta) / sinHalfTheta;
   double ratioB = sin(aFactor * halfTheta) / sinHalfTheta;

   // Calculate the Quaternion.
   qm.mA = (aLHS.mA * ratioA + qbMulter * aRHS.mA * ratioB);
   qm.mI = (aLHS.mI * ratioA + qbMulter * aRHS.mI * ratioB);
   qm.mJ = (aLHS.mJ * ratioA + qbMulter * aRHS.mJ * ratioB);
   qm.mK = (aLHS.mK * ratioA + qbMulter * aRHS.mK * ratioB);

   qm.Normalize(); // TO-DO : This may not be necessary.

   return qm;
}

// ============================================================================
//! Rotate a vector by this quaternion.  The quaternion is assumed to be normalized.
void UtQuaternion::Rotate(const double aVecIn[3], double aVecOut[3]) const
{
   UtQuaternion qv(0.0, aVecIn[0], aVecIn[1], aVecIn[2]);
   UtQuaternion tmp = Conjugate() * qv * (*this);
   aVecOut[0]       = tmp.mI;
   aVecOut[1]       = tmp.mJ;
   aVecOut[2]       = tmp.mK;
}

// ============================================================================
//! Rotate a vector by the reverse of this quaternion.
//! The quaternion is assumed to be normalized.
void UtQuaternion::ReverseRotate(const double aVecIn[3], double aVecOut[3]) const
{
   UtQuaternion qv(0.0, aVecIn[0], aVecIn[1], aVecIn[2]);
   UtQuaternion tmp = *this * qv * Conjugate();
   aVecOut[0]       = tmp.mI;
   aVecOut[1]       = tmp.mJ;
   aVecOut[2]       = tmp.mK;
}

// ============================================================================
//! Transform() is equivalent to Rotate().
UtVec3dX UtQuaternion::Transform(const UtVec3dX& aVector) const
{
   UtQuaternion qv(0.0, aVector[0], aVector[1], aVector[2]);
   UtQuaternion tmp = Conjugate() * qv * (*this);
   return UtVec3dX(tmp.mI, tmp.mJ, tmp.mK);
}

// ============================================================================
//! InverseTransform() is equivalent to ReverseRotate()..
UtVec3dX UtQuaternion::InverseTransform(const UtVec3dX& aVector) const
{
   UtQuaternion qv(0.0, aVector[0], aVector[1], aVector[2]);
   UtQuaternion tmp = *this * qv * Conjugate();
   return UtVec3dX(tmp.mI, tmp.mJ, tmp.mK);
}

// ============================================================================
//! Obtain the quaternion raised to an exponent.
UtQuaternion UtQuaternion::Exp() const
{
   double EPSILON = 0.00000001;

   double theta     = sqrt(mI * mI + mJ * mJ + mK * mK);
   double sin_theta = sin(theta);

   UtQuaternion q;
   q.mA = cos(theta);
   if (fabs(sin_theta) > EPSILON)
   {
      q.mI = mI * sin_theta / theta;
      q.mJ = mJ * sin_theta / theta;
      q.mK = mK * sin_theta / theta;
   }
   else
   {
      q.mI = mI;
      q.mJ = mJ;
      q.mK = mK;
   }

   return q;
}

// ============================================================================
//! Obtain the quaternion raised to a power.
UtQuaternion UtQuaternion::Power(double t) const
{
   return UtQuaternion(Log() * t).Exp();
}

// ============================================================================
//! Obtain the logarithm of a quaternion.
UtQuaternion UtQuaternion::Log() const
{
   double EPSILON = 0.00000001;

   UtQuaternion q;
   q.mA             = 0;
   double theta     = acos(mA);
   double sin_theta = sin(theta);

   if (fabs(sin_theta) > EPSILON)
   {
      q.mI = mI / sin_theta * theta;
      q.mJ = mJ / sin_theta * theta;
      q.mK = mK / sin_theta * theta;
   }
   else
   {
      q.mI = mI;
      q.mJ = mJ;
      q.mK = mK;
   }

   return q;
}
