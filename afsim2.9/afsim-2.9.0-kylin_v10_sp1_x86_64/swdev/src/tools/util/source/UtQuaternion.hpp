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

#ifndef UT_QUATERNION_HPP
#define UT_QUATERNION_HPP

class UtDCM;
#include "ut_export.h"

#include "UtCoords.hpp"
#include "UtVec3dX.hpp"

//! A quaternion representation of 3-D orientation and orientation rates.
//!
//! Abstract:  A quaternion is a four-valued mathematical construct, an
//! alternative to 3 Euler Rotation Angles or a Direction Cosine Matrix
//! (DCM) as a way to represent the orientation of a solid body in 3-D
//! space.  It has the advantage of not producing a mathematical
//! singularity at pitch angles of +/- 90 degrees.  It will allow
//! computationally efficient propagation (integration) of body angular
//! rates forward in time to a new orientation.  A quaternion orientation
//! is to be maintained near RMS magnitude of one by re-Normalization
//! periodically (see below).  A "rate" quaternion, however, may be of
//! any magnitude, including zero.  The Qrate is linear, and may be
//! summed to another Qrate, i.e. during an RK4 integration scheme.
//!
//! A quaternion is better at transforming vectors with fewer math
//! operations, less trigonometry, and less roundoff error than a DCM.
//! Euler rotation angles are consistent with common use, but not quite
//! intuitive, so as a reminder:
//!   Psi   = Yaw Rotation(+Z)
//!   Theta = Pitch Rotation(+Y)
//!   Phi   = Roll Rotation(+X)      specifically in that order.
//!
//! Use of operator overloads will probably yield the most readable code,
//! but equivalent return void methods will be more computationally
//! efficient by eliminating the return by value copy constructor.
//!
//! Usage Notes:  Updating to a new orientation is a two-step process:
//!
//!  1) Qrate = f(Qcurrent, PQR[3]);  Where the PQR vector is the
//!       (X, Y, Z) body frame angular rates in radians per second.
//!  2) Qnew = Qcurrent + Qrate * deltaT;
//!     ...or the more computationally efficient (and readable) alternative:
//!     Qcurrent += Qrate * deltaT.
//!
//! When the time propagation functions are used iteratively in an
//! integration loop, the quaternion may drift out of unity, and
//! should be re-normalized periodically, on some appropriate modulus
//! count.  Ideally this should be up to the implementation to do, based
//! upon the magnitude and rates of the rotations.  But in practice,
//! this caused difficulties if Normalize() was not called frequently
//! enough by the application, so the Normalize() call was placed in the
//! Update() and Slerp() calls directly, so the application need not do
//! so.  This is at the expense of computational speed, so may be
//! changed later.
//!
//! Reference:   Aircraft Control and Simulation, Brian L. Stevens and
//! Frank L. Lewis, 1992, ISBN 0-471-61397-5.  For a more compact synopsis,
//! see also http://www.mare.ee/indrek/varphi/vardyn.pdf

class UT_EXPORT UtQuaternion
{
public:
   // ===== Constructors =====
   UtQuaternion();

   //! Construct a quaternion directly from four values
   UtQuaternion(double aA, double aI, double aJ, double aK);

   UtQuaternion(double aPsi, double aTheta, double aPhi);

   UtQuaternion(const ut::coords::EulerAngles& aAngles);

   UtQuaternion(const double aDCM[3][3]);
   UtQuaternion(const UtDCM& aDCM);

   UtQuaternion(const double aAngle, const double aVector[3]);
   UtQuaternion(const double aAngle, const UtVec3dX& aVector);

   //! Copy constructor.
   UtQuaternion(const UtQuaternion& aSrc) = default;

   // ===== Operators =====

   bool operator==(const UtQuaternion& aSrc) const;
   bool operator!=(const UtQuaternion& aSrc) const { return !(*this == aSrc); }

   UtQuaternion& operator=(const UtQuaternion& aSrc) = default;
   UtQuaternion& operator+=(const UtQuaternion& aSrc);

   UtQuaternion operator+(const UtQuaternion& aSrc) const;
   UtQuaternion operator-(const UtQuaternion& aSrc) const;

   UtQuaternion operator*(const UtQuaternion& aRHS) const;
   UtQuaternion operator*(const double aRHS) const;

   UtVec3dX operator*(const UtVec3dX& aVector) const;

   // ===== Accessors =====

   double        operator[](int aIndex) const;
   const double* operator()(int aIndex) const;

   void  Get(double aDCM[3][3]) const;
   void  Get(UtDCM& aDCM) const;
   UtDCM GetDCM() const;

   void                    Get(double& aPsi, double& aTheta, double& aPhi) const;
   ut::coords::EulerAngles Get() const;

   UtQuaternion GetRate(const UtVec3dX& aPQR) const;
   UtQuaternion GetRate(const double aPQR[3]) const;

   void Set(const UtDCM& aDCM);
   void Set(const double aDCM[3][3]);

   void Set(double aPsi, double aTheta, double aPhi);

   void SetRate(const UtQuaternion& aQuat, const UtVec3dX& aPQR);

   void SetRate(const UtQuaternion& aQuat, const double aPQR[3]);

   // ===== Other Procedures and Functions =====

   UtQuaternion Conjugate() const;

   double Dot(const UtQuaternion& aOther) const;

   void Normalize();

   UtQuaternion Propagate(const double aPQR[3], double aDeltaT) const;

   void GetPropagated(const double aPQR[3], double aDeltaT, UtQuaternion& aNewQuat) const;

   UtQuaternion Rotate(const double aPQR[3], double aDeltaT) const;
   UtQuaternion Rotate(const UtVec3dX& aPQR, double aDeltaT) const;
   UtQuaternion Rotate(const UtQuaternion& aRate, double aDeltaT) const;

   void Update(const double aPQR[3], const UtQuaternion& aRate, double aDeltaT);
   void Update(const UtVec3dX& aPQR, const UtQuaternion& aRate, double aDeltaT);
   void Update(const UtQuaternion& aRate, double aDeltaT);

   static UtQuaternion Slerp(const UtQuaternion& aLHS, const UtQuaternion& aRHS, float aFactor);

   void Rotate(const double aVecIn[3], double aVecOut[3]) const;

   void ReverseRotate(const double aVecIn[3], double aVecOut[3]) const;

   // Transform() is equivalent to Rotate()
   UtVec3dX Transform(const UtVec3dX& aVector) const;

   // InverseTransform() is equivalent to ReverseRotate()
   UtVec3dX InverseTransform(const UtVec3dX& aVector) const;

   UtQuaternion Exp() const;
   UtQuaternion Power(double t) const;
   UtQuaternion Log() const;

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      aBuff& mA& mI& mJ& mK;
   }

protected:
   void CalcQi(size_t aKnown, size_t aUnknown, double aDestination[4], const double aDCM[3][3]) const;

   double mA;
   double mI;
   double mJ;
   double mK;

}; // end class

UtQuaternion operator*(double aLHS, const UtQuaternion& aRHS);

#endif
