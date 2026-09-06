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

#include "UtVec3dX.hpp"

#include <cassert>
#include <iostream>

#include "UtDCM.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"

// static
const UtVec3dX UtVec3dX::cNULL         = UtVec3dX(0.0, 0.0, 0.0);
const UtVec3dX UtVec3dX::cUNIT_FORWARD = UtVec3dX(1.0, 0.0, 0.0);
const UtVec3dX UtVec3dX::cUNIT_RIGHT   = UtVec3dX(0.0, 1.0, 0.0);
const UtVec3dX UtVec3dX::cUNIT_DOWN    = UtVec3dX(0.0, 0.0, 1.0);

namespace
{
const double cVEC3DX_FUZZ = 0.0000001;
bool         Equal(double a, double b, double aFuzz)
{
   return fabs(a - b) < aFuzz;
}
} // namespace

// ==========================================================================
UtVec3dX::UtVec3dX()
{
   Null();
}

// ==========================================================================
UtVec3dX::UtVec3dX(const double aVect[3])
{
   Set(aVect[0], aVect[1], aVect[2]);
}

// ==========================================================================
UtVec3dX::UtVec3dX(double aX, double aY, double aZ)
{
   Set(aX, aY, aZ);
}

// ==========================================================================
// This is a "spherical" constructor.
UtVec3dX::UtVec3dX(double aAzimuth, double aElevation, bool aIsNED)
{
   ConstructSpherical(aAzimuth, aElevation, 1.0, aIsNED);
}

// ==========================================================================
// This is a "spherical" constructor.  The bool is used to specify a NED or ENU
// coordinate frame
UtVec3dX::UtVec3dX(double aAzimuth, double aElevation, double aMagnitude, bool aIsNED)
{
   ConstructSpherical(aAzimuth, aElevation, aMagnitude, aIsNED);
}

// ==========================================================================
void UtVec3dX::Null()
{
   Set(0.0, 0.0, 0.0);
}

// ==========================================================================
double UtVec3dX::Azimuth() const
{
   if (Equal(Get(0), 0.0, cVEC3DX_FUZZ) && Equal(Get(1), 0.0, cVEC3DX_FUZZ))
   {
      return 0.0;
   }
   return atan2(Get(1), Get(0));
}

// ==========================================================================
double UtVec3dX::Elevation() const
{
   if (Equal(Get(0), 0.0, cVEC3DX_FUZZ) && Equal(Get(1), 0.0, cVEC3DX_FUZZ) && Equal(Get(2), 0.0, cVEC3DX_FUZZ))
   {
      return 0.0;
   }
   return -asin(Get(2) / Magnitude());
}

// ==========================================================================
//! Measured positive clockwise about X, with zero at twelve o'clock high.
double UtVec3dX::ClockAngle() const
{
   if (Equal(Get(1), 0.0, cVEC3DX_FUZZ) && Equal(Get(2), 0.0, cVEC3DX_FUZZ))
   {
      return 0.0;
   }
   return atan2(Get(1), -Get(2));
}

// ==========================================================================
// static
double UtVec3dX::HalfConeAngle(double aA, double aN)
{
   if (aN < cVEC3DX_FUZZ)
   {
      if (aA >= 0.0)
      {
         return 0.0;
      }
      else
      {
         return UtMath::cPI;
      }
   }
   return atan2(aN, aA);
}

// ==========================================================================
double UtVec3dX::HalfConeFromX() const
{
   return HalfConeAngle(Get(0), MagYZ());
}

// ==========================================================================
double UtVec3dX::HalfConeFromY() const
{
   return HalfConeAngle(Get(1), MagXZ());
}

// ==========================================================================
double UtVec3dX::HalfConeFromZ() const
{
   return HalfConeAngle(Get(2), MagXY());
}

// ==========================================================================
double UtVec3dX::MagXY() const
{
   return sqrt(Get(0) * Get(0) + Get(1) * Get(1));
}

// ==========================================================================

double UtVec3dX::MagYZ() const
{
   return sqrt(Get(1) * Get(1) + Get(2) * Get(2));
}

// ==========================================================================
double UtVec3dX::MagXZ() const
{
   return sqrt(Get(0) * Get(0) + Get(2) * Get(2));
}

// ==========================================================================
//! Specialized accessor for aerodynamic angles, given a body-relative wind:
void UtVec3dX::GetAeroAngles(double& aAlpha, double& aBeta) const
{
   // Note that wind sense is backwards, i.e. wind toward tail (-X) is "normal".
   // Convention:  Alpha ranges from -Pi to +Pi, Beta from -Pi/2 to +Pi/2;
   //              Wind approaching from below is positive alpha.
   //              Wind approaching from right ear is positive beta.

   // Wind magnitude (squared) in the XZ (alpha) plane:
   double xzMagSq = Get(0) * Get(0) + Get(2) * Get(2);
   if (xzMagSq > cVEC3DX_FUZZ)
   {
      aAlpha = UtMath::NormalizeAngleMinusPi_Pi(atan2(-Get(2), -Get(0)));
   }
   else
   {
      // If relative wind is in the Z direction (down), alpha is negative:
      if (Get(2) > 0.0)
      {
         aAlpha = -UtMath::cPI_OVER_2;
      }
      else
      {
         aAlpha = UtMath::cPI_OVER_2;
      }
   }

   // Wind magnitude (squared) in the XY (beta) plane:
   double xyMagSq = Get(0) * Get(0) + Get(1) * Get(1);
   if (xyMagSq > cVEC3DX_FUZZ)
   {
      // Beta is positive with wind in right ear...
      // Constrained to +/- 90 degrees
      aBeta = asin(-Get(1) / sqrt(xyMagSq));
   }
   else
   {
      aBeta = 0.0;
   }
}

// ==========================================================================
bool UtVec3dX::operator==(const UtVec3dX& aVect) const
{
   return (!((Get(0) != aVect.Get(0)) || (Get(1) != aVect.Get(1)) || (Get(2) != aVect.Get(2))));
}

// ==========================================================================
bool UtVec3dX::ApproxEqual(const UtVec3dX& aVect, double aEpsilon) const
{
   assert(aEpsilon >= 0);
   return Equal(Get(0), aVect.Get(0), aEpsilon) && Equal(Get(1), aVect.Get(1), aEpsilon) &&
          Equal(Get(2), aVect.Get(2), aEpsilon);
}

// ==========================================================================
// Removed here, and made an inline call in the header.
// bool UtVec3dX::operator!=(const UtVec3dX& aVect) const
// {
//    return (! (*this == aVect));
// }

// ==========================================================================
UtVec3dX& UtVec3dX::operator=(const UtVec3dX& aVect)
{
   GetData()[0] = aVect[0];
   GetData()[1] = aVect[1];
   GetData()[2] = aVect[2];
   return *this;
}

// ==========================================================================
UtVec3dX& UtVec3dX::operator+=(const UtVec3dX& aVect)
{
   GetData()[0] += aVect[0];
   GetData()[1] += aVect[1];
   GetData()[2] += aVect[2];
   return *this;
}

// ==========================================================================
UtVec3dX& UtVec3dX::operator-=(const UtVec3dX& aVect)
{
   GetData()[0] -= aVect[0];
   GetData()[1] -= aVect[1];
   GetData()[2] -= aVect[2];
   return *this;
}

// ==========================================================================
UtVec3dX UtVec3dX::operator*(double aValue) const
{
   return UtVec3dX(aValue * Get(0), aValue * Get(1), aValue * Get(2));
}

// ==========================================================================
UtVec3dX UtVec3dX::operator/(double aValue) const
{
   return UtVec3dX(Get(0) / aValue, Get(1) / aValue, Get(2) / aValue);
}

// ==========================================================================
UtVec3dX UtVec3dX::operator*(const UtDCM& aDCM) const
{
   UtVec3dX result;
   Multiply(aDCM, result);
   return result;
}

// ==========================================================================
UtVec3dX UtVec3dX::operator*(const UtQuaternion& aQuat) const
{
   UtVec3dX result;
   Multiply(aQuat, result);
   return result;
}

// ==========================================================================
UtVec3dX UtVec3dX::operator+(const UtVec3dX& aVect) const
{
   return UtVec3dX(Get(0) + aVect[0], Get(1) + aVect[1], Get(2) + aVect[2]);
}

// ==========================================================================
UtVec3dX UtVec3dX::operator-(const UtVec3dX& aVect) const
{
   return UtVec3dX(Get(0) - aVect[0], Get(1) - aVect[1], Get(2) - aVect[2]);
}

// ==========================================================================
UtVec3dX UtVec3dX::operator-() const
{
   return UtVec3dX(-Get(0), -Get(1), -Get(2));
}

// ==========================================================================
UtVec3dX UtVec3dX::Cross(const UtVec3dX& aVect) const
{
   return UtVec3dX(Get(1) * aVect[2] - Get(2) * aVect[1],
                   Get(2) * aVect[0] - Get(0) * aVect[2],
                   Get(0) * aVect[1] - Get(1) * aVect[0]);
}

// ==========================================================================
double UtVec3dX::Dot(const UtVec3dX& aVect) const
{
   return Get(0) * aVect[0] + Get(1) * aVect[1] + Get(2) * aVect[2];
}

// ==========================================================================
double UtVec3dX::AngleBetween(const UtVec3dX& aVect) const
{
   double magMag = Magnitude() * aVect.Magnitude();
   double dot    = this->Dot(aVect);

   if (magMag < cVEC3DX_FUZZ)
   {
      return 0.0;
   }
   double cosine = dot / magMag;
   if (fabs(cosine) < 1.0)
   {
      return acos(cosine);
   }
   if (cosine > 0.0)
   {
      return 0.0;
   }
   return UtMath::cPI;
}

// ==========================================================================
void UtVec3dX::Multiply(const UtDCM& aDCM, UtVec3dX& aResult) const
{
   aResult[0] = mVec[0] * aDCM[0][0] + mVec[1] * aDCM[0][1] + mVec[2] * aDCM[0][2];
   aResult[1] = mVec[0] * aDCM[1][0] + mVec[1] * aDCM[1][1] + mVec[2] * aDCM[1][2];
   aResult[2] = mVec[0] * aDCM[2][0] + mVec[1] * aDCM[2][1] + mVec[2] * aDCM[2][2];
}

// ==========================================================================
void UtVec3dX::MultiplyTranspose(const UtDCM& aDCM, UtVec3dX& aResult) const
{
   aResult[0] = mVec[0] * aDCM[0][0] + mVec[1] * aDCM[1][0] + mVec[2] * aDCM[2][0];
   aResult[1] = mVec[0] * aDCM[0][1] + mVec[1] * aDCM[1][1] + mVec[2] * aDCM[2][1];
   aResult[2] = mVec[0] * aDCM[0][2] + mVec[1] * aDCM[1][2] + mVec[2] * aDCM[2][2];
}

// ==========================================================================
void UtVec3dX::Multiply(const UtQuaternion& aQuat, UtVec3dX& aResult) const
{
   aQuat.Rotate(GetData(), aResult.GetData());
}

// ==========================================================================
void UtVec3dX::MultiplyReverse(const UtQuaternion& aQuat, UtVec3dX& aResult) const
{
   aQuat.ReverseRotate(GetData(), aResult.GetData());
}

// ==========================================================================
// friend
UtVec3dX operator*(double aValue, const UtVec3dX& aVect)
{
   return UtVec3dX(aVect[0] * aValue, aVect[1] * aValue, aVect[2] * aValue);
}

// friend
std::ostream& operator<<(std::ostream& aStream, const UtVec3dX& aVec)
{
   aStream << "(" << aVec.X() << ", " << aVec.Y() << ", " << aVec.Z() << ")";
   return aStream;
}

// ==========================================================================
// static
UtVec3dX UtVec3dX::Interpolate(double aXtarget, double aX0, double aX1, const UtVec3dX& aY0, const UtVec3dX& aY1)
{
   double interpolant = (aXtarget - aX0) / (aX1 - aX0);
   return Interpolate(interpolant, aY0, aY1);
}

// ==========================================================================
// static
UtVec3dX UtVec3dX::Interpolate(double aInterpolant, const UtVec3dX& aY0, const UtVec3dX& aY1)
{
   return UtVec3dX(aY0[0] + aInterpolant * (aY1[0] - aY0[0]),
                   aY0[1] + aInterpolant * (aY1[1] - aY0[1]),
                   aY0[2] + aInterpolant * (aY1[2] - aY0[2]));
}

// ==========================================================================
// static
// Historical Approximation Method:
// static
UtVec3dX UtVec3dX::LineOfSightRates(const UtVec3dX& aPrevEyeLoc,
                                    const UtVec3dX& aCurrEyeLoc,
                                    const UtVec3dX& aPrevTgtLoc,
                                    const UtVec3dX& aCurrTgtLoc,
                                    double          aDeltaT)
{
   // Algorithm:
   // 1. Take previous LOS Unit vector, Cross() current LOS Unit vector, yielding LOSR Direction Vector.
   // 2. Dot() the two LOS vectors, and acos to get the angle between them.
   // 3. Angle / dt, yields angular rate.  Multiply by (normalized) LOSR Direction Vector.

   if (aDeltaT <= 0.0)
      return UtVec3dX::cNULL;
   UtVec3dX prevULOS = aPrevTgtLoc - aPrevEyeLoc;
   prevULOS.Normalize();
   UtVec3dX thisULOS = aCurrTgtLoc - aCurrEyeLoc;
   thisULOS.Normalize();
   UtVec3dX losDir = prevULOS.Cross(thisULOS);
   losDir.Normalize();
   double   angle = prevULOS.AngleBetween(thisULOS);
   double   omega = angle / aDeltaT;
   UtVec3dX rv    = losDir * omega;
   return rv;
}

// ==========================================================================
// static
// Instantaneous Vector Cross Method:
// static
UtVec3dX UtVec3dX::LineOfSightRates(const UtVec3dX& aEyeLoc,
                                    const UtVec3dX& aEyeVel,
                                    const UtVec3dX& aTgtLoc,
                                    const UtVec3dX& aTgtVel)
{
   // Dimensional analysis:  R X V = m X m/s.
   // Need 1/s, so must divide by mag(R) twice.
   UtVec3dX relativeVel  = aTgtVel - aEyeVel;
   UtVec3dX offsetR      = aTgtLoc - aEyeLoc;
   UtVec3dX omega        = offsetR.Cross(relativeVel);
   double   timeRateIzer = offsetR.Magnitude() * offsetR.Magnitude();
   return omega / timeRateIzer;
}

// ==========================================================================

bool UtVec3dX::Test() const
{
   bool success = true;

   // ==================== Operator Tests ========================
   UtVec3dX A(1.5, 2.5, 3.5);
   UtVec3dX B(A);

   if (!(A == B))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 0 failed copy constructor.";
      success = false;
   }

   B = A;
   if (B != A)
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 1 failed UtVec3dX assignment test.";
      success = false;
   }

   B += A;
   if ((B[0] != 3.0) || (B[1] != 5.0) || (B[2] != 7.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 2 failed UtVec3dX operator+=  test.";
      success = false;
   }

   B -= A;
   if (B != A)
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 3 failed UtVec3dX operator-= test.";
      success = false;
   }

   B = -A;
   if ((B[0] != -A[0]) || (B[1] != -A[1]) || (B[2] != -A[2]))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 4 failed (unary) UtVec3dX operator- test.";
      success = false;
   }

   B = 2 * A;
   if ((B[0] != 3.0) || (B[1] != 5.0) || (B[2] != 7.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 5 failed (scalar * UtVec3dX) test.";
      success = false;
   }

   B = A * 2;
   if ((B[0] != 3.0) || (B[1] != 5.0) || (B[2] != 7.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 6 failed (UtVec3dX * scalar) test.";
      success = false;
   }

   B = A / 0.5;
   if ((B[0] != 3.0) || (B[1] != 5.0) || (B[2] != 7.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 7 failed (UtVec3dX / scalar) test.";
      success = false;
   }

   B = A;
   UtVec3dX C;
   C = A + B;
   if ((C[0] != 3.0) || (C[1] != 5.0) || (C[2] != 7.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 8 failed (UtVec3dX + UtVec3dX) test.";
      success = false;
   }

   C = A - B;
   if ((C[0] != 0.0) || (C[1] != 0.0) || (C[2] != 0.0))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 9 failed (UtVec3dX - UtVec3dX) test.";
      success = false;
   }

   // ==================== Interpolation Test ========================

   double interpolant = 0.25;
   B.Set(1.0, 1.0, 1.0);
   C = Interpolate(interpolant, A, B);

   if ((C[0] != A[0] + interpolant * (B[0] - A[0])) || (C[1] != A[1] + interpolant * (B[1] - A[1])) ||
       (C[2] != A[2] + interpolant * (B[2] - A[2])))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 10 failed Interpolate(scalar, UtVec3dX, UtVec3dX) test.";
      success = false;
   }

   // ==================== Cross Product Test ========================

   // Tests to be added....
   UtVec3dX D(1.0, 2.0, 3.0);
   UtVec3dX E(-1.0, -1.0, -1.0);

   UtVec3dX F = D.Cross(E);
   double   compareF[3];
   UtVec3d::CrossProduct(compareF, D.GetData(), E.GetData());

   if ((F[0] != compareF[0]) || (F[1] != compareF[1]) || (F[2] != compareF[2]))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 11 failed Cross() product test.";
      success = false;
   }

   // ==================== Dot Product Test ========================

   double dot        = D.Dot(E);
   double compareDot = UtVec3d::DotProduct(D.GetData(), E.GetData());

   if (dot != compareDot)
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 12 failed Dot() product test.";
      success = false;
   }

   // =================== Spherical Constructor Test ===============
   UtVec3dX spherical(30.0 * UtMath::cRAD_PER_DEG, 60.0 * UtMath::cRAD_PER_DEG, 10.0, true);
   UtVec3dX cartesian(2.5 * sqrt(3.0), 2.5, -5.0 * sqrt(3.0));

   if (!spherical.ApproxEqual(cartesian, cVEC3DX_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtVec3dX: Test 13 failed spherical constructor test.";
      success = false;
   }

   return success;
}

void UtVec3dX::ConstructSpherical(double aAzimuth, double aElevation, double aMagnitude, bool aIsNED)
{
   double signZ = (aIsNED ? -1.0 : 1.0);
   double signY = -signZ;
   Set(aMagnitude * cos(aElevation) * cos(aAzimuth),
       aMagnitude * cos(aElevation) * signY * sin(aAzimuth),
       aMagnitude * signZ * sin(aElevation));
}
