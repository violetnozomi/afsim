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

#ifndef UTVEC3DX_HPP
#define UTVEC3DX_HPP

#include "ut_export.h"

#include <iostream>

#include "UtVec3.hpp"

class UtDCM;
class UtQuaternion;

//! UtVec3dX inherits from UtVec3d, but provides additional operator overloads.
class UT_EXPORT UtVec3dX : public UtVec3d
{
public:
   static const UtVec3dX cNULL;
   static const UtVec3dX cUNIT_FORWARD;
   static const UtVec3dX cUNIT_RIGHT;
   static const UtVec3dX cUNIT_DOWN;

   UtVec3dX();
   UtVec3dX(const double aVect[3]);
   UtVec3dX(double aX, double aY, double aZ);
   //! Creates a unit vector with the given azimuth and elevation values.
   UtVec3dX(double aAzimuth, double aElevation, bool aIsNED);
   //! Creates a spherical vector of magnitude aMagnitude with the given azimuth and elevation values.
   UtVec3dX(double aAzimuth, double aElevation, double aMagnitude, bool aIsNED);

   double X() const { return Get(0); }
   double Y() const { return Get(1); }
   double Z() const { return Get(2); }

   double Azimuth() const;
   double Elevation() const;
   double ClockAngle() const;

   // Utility function.
   static double HalfConeAngle(double aA, double aN);

   double HalfConeFromX() const;
   double HalfConeFromY() const;
   double HalfConeFromZ() const;

   double MagXY() const;
   double MagYZ() const;
   double MagXZ() const;

   void GetAeroAngles(double& aAlpha, double& aBeta) const;

   void Null();

   bool ApproxEqual(const UtVec3dX& aVect, double aEpsilon) const;

   bool operator==(const UtVec3dX& aVect) const;
   bool operator!=(const UtVec3dX& aVect) const { return !(*this == aVect); }

   UtVec3dX& operator=(const UtVec3dX& aVect);
   UtVec3dX& operator+=(const UtVec3dX& aVect);
   UtVec3dX& operator-=(const UtVec3dX& aVect);
   UtVec3dX  operator-() const;

   UtVec3dX operator*(double aValue) const;
   UtVec3dX operator/(double aValue) const;
   UtVec3dX operator+(const UtVec3dX& aVect) const;
   UtVec3dX operator-(const UtVec3dX& aVect) const;
   UtVec3dX operator*(const UtDCM& aDCM) const;
   UtVec3dX operator*(const UtQuaternion& aQuat) const;

   friend UT_EXPORT UtVec3dX operator*(double aValue, const UtVec3dX& aVect);

   friend UT_EXPORT std::ostream& operator<<(std::ostream& aStream, const UtVec3dX& aVec);

   UtVec3dX Cross(const UtVec3dX& aVect) const;
   double   Dot(const UtVec3dX& aVect) const;

   double AngleBetween(const UtVec3dX& aVector) const;

   void Multiply(const UtDCM& aDCM, UtVec3dX& aResult) const;

   void MultiplyTranspose(const UtDCM& aDCM, UtVec3dX& aResult) const;

   void Multiply(const UtQuaternion& aQuat, UtVec3dX& aResult) const;

   void MultiplyReverse(const UtQuaternion& aQuat, UtVec3dX& aResult) const;

   static UtVec3dX Interpolate(double aXtarget, double aX0, double aX1, const UtVec3dX& aY0, const UtVec3dX& aY1);

   static UtVec3dX Interpolate(double aInterpolant, const UtVec3dX& aY0, const UtVec3dX& aY1);

   // === Utility functions for Line-Of-Sight Rates (LOSR) ===
   // There are two primary ways to compute LOSR, historically, or instantaneously.
   // (All vectors must be expressed in the same non-rotating base frame!)

   // Historical Approximation Method:
   static UtVec3dX LineOfSightRates(const UtVec3dX& aPrevEyeLoc,
                                    const UtVec3dX& aCurrEyeLoc,
                                    const UtVec3dX& aPrevTgtLoc,
                                    const UtVec3dX& aCurrTgtLoc,
                                    double          aDeltaT);

   // Instantaneous Vector Cross Method:
   static UtVec3dX
   LineOfSightRates(const UtVec3dX& aEyeLoc, const UtVec3dX& aEyeVel, const UtVec3dX& aTgtLoc, const UtVec3dX& aTgtVel);

   //! (Temporary) automated build testing function, prior to fully implemented scriptability of the class.
   bool Test() const;

private:
   void ConstructSpherical(double aAzimuth, double aElevation, double aMagnitude, bool aIsNED);
};

#endif
