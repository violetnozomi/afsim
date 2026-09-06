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

#ifndef UTINERTIATENSOR_HPP
#define UTINERTIATENSOR_HPP

#include "ut_export.h"
class UtInput;
#include "UtMatrix.hpp"
class UtVec3dX;
class UtDCM;

//! Utility inertia tensor class, used for 6-DOF attitude rate propagation.
//! Newtonian motion dictates that Angular momentum is conserved in the
//! inertial frame.  A UtInertiaTensor is the means to convert to/from body
//! angular rates (ECS) to a momentum vector.  Change in angular momentum
//! is identically equal to applied moment times delta time.
//! AngularMomentum = MassTensor * AngularVelocity, and so
//! AngularVelocity = Inverse(MassTensor) * AngularMomentum.

class UT_EXPORT UtInertiaTensor : public UtMatrixd
{
public:
   // ==== Static functions: ====

   static const UtInertiaTensor cNULL;

   static UtInertiaTensor
   Interpolate(double aXtarget, double aX0, double aX1, const UtInertiaTensor& aY0, const UtInertiaTensor& aY1);

   static UtInertiaTensor Interpolate(double aInterpolant, const UtInertiaTensor& aY0, const UtInertiaTensor& aY1);

   static void Interpolate(double                 aXtarget,
                           double                 aX0,
                           double                 aX1,
                           const UtInertiaTensor& aY0,
                           const UtInertiaTensor& aY1,
                           UtInertiaTensor&       aResult);

   static void Interpolate(double aInterpolant, const UtInertiaTensor& aY0, const UtInertiaTensor& aY1, UtInertiaTensor& aResult);

   // ===== Constructors =====

   UtInertiaTensor();
   UtInertiaTensor(const UtInertiaTensor& aSrc) = default;

   // ===== Stream Operations =====

   bool ProcessInput(UtInput& aInput);

   // ===== Operators =====
   // (For most accessors, use those in the parent UtMatrixd class.)
   double Get(unsigned int i, unsigned int j) const { return (*this)(i, j); }

   //! Angular momentum vector = UtInertiaTensor * body frame angular velocity vector,
   UtVec3dX operator*(const UtVec3dX& aVect) const;

   void Multiply(const UtDCM& aDCM, UtInertiaTensor& aResult) const;

   //! Parallel Axis Theorem (two functions following):
   //! A. Return the UtInertiaTensor that results when this object is caused to rotate about
   //!    another point removed from its principal axes origin.
   UtInertiaTensor ParallelAxisTheorem(double          aMassValue,
                                       const UtVec3dX& aOffsetFromCG); // in parent frame, and measured between cg and object.

   //! B. Request that this UtInertiaTensor to add its effect into a parent object.
   //! This function is likely to be used with aAggregatedTensor beginning as zero, with a loop
   //! repeatedly calling foo.AggregateTo_CG(offset, aAggregatedTensor) to build up the summed
   //! value, from a number of 'foo' parts.  Note:  The "this" object must be expressed in the
   //! same frame as the aAggregatedTensor object!
   void AggregateInto(double           aMassValue,
                      const UtVec3dX&  aOffsetFromCG, // in parent frame, and measured from cg to object.
                      UtInertiaTensor& aAggregatedTensor);

   // ===== Accessors and Utilities =====

   void            Null();
   void            Fill3By3(double aMat[3][3]) const;
   UtInertiaTensor Inverse() const;

   // =========================================================
   // *** Attributes are all contained in the Parent class. ***
   // =========================================================

}; // end class

#endif
