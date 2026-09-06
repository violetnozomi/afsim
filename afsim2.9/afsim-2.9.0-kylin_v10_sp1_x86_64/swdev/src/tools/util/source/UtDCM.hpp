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

#ifndef UT_DCM_HPP
#define UT_DCM_HPP

#include "ut_export.h"

class UtInertiaTensor;
class UtQuaternion;
#include "UtVec3dX.hpp"

//! Utility Direction Cosine Matrix (DCM) class.
//! A DCM is the means to transform a 3-D vector from one coordinate frame into
//! another frame, rotated relative to the first.   Use of operator overloads
//! will likely yield the most readable code, but equivalent return void
//! procedures will be more computationally efficient, by eliminating copy
//! construction of the returned value.  Angle values are in radians.
//! If an equivalent UtQuaternion is available, Euler angles should be
//! obtained from it, since math singularities near 90 degrees pitch are
//! not a problem there.  Two successive rotations are combined as follows:
//! Rtotal = R2 * R1 (Note the order, since the vector being rotated is
//! multiplied from the right).

class UT_EXPORT UtDCM
{
public:
   static const double cIDENTITY_MAT[3][3];
   static const UtDCM  cIDENTITY;

   // ===== Constructors =====

   UtDCM();
   UtDCM(const UtDCM& aSrc);
   UtDCM(const double aDCM[3][3]);
   UtDCM(const UtQuaternion& aQuat);
   UtDCM(double aAngle, const double aRotationUnitVector[3]);

   //! Usage reminder for this constructor:  Yaw, Pitch, Roll, in that
   //! order, which is contrary to typical (x,y,z) vector order.
   //! (Rotation angles are _not_ vectors.)
   UtDCM(double aPsi, double aTheta, double aPhi);

   // ===== Operators =====

   UtDCM& operator=(const UtDCM& aRhs);
   UtDCM& operator=(const UtQuaternion& aRhs);

   bool operator==(const UtDCM& aSrc) const;
   bool operator!=(const UtDCM& aSrc) const { return !(*this == aSrc); }

   //! Direction Cosine multiplication is not commutative (DcmA * DcmB != DcmB * DdcmA), because
   //! the order in which rotations are accomplished is significant.  Multiplication (and thus
   //! rotations proceed from the right-most factor in the expression (first rotation) to the
   //! left factor (second rotation), etc.
   UtDCM operator*(const UtDCM& aRhs) const;

   UtVec3dX operator*(const UtVec3dX& aVect) const;

   UtVec3dX operator*(const double aVect[3]) const;

   UtVec3d InverseTransform(const UtVec3d& aRhs) const;
   UtVec3d Transform(const UtVec3d& aRhs) const;

   UtVec3dX InverseTransform(const UtVec3dX& aRhs) const;
   UtVec3dX Transform(const UtVec3dX& aRhs) const;

   void InverseTransform(const UtInertiaTensor& aInput, UtInertiaTensor& aResult) const;
   void Transform(const UtInertiaTensor& aInput, UtInertiaTensor& aResult) const;

   // ===== Accessors =====

   double Get(size_t aRow, size_t aCol) const { return mMat[aRow][aCol]; }

   void Get(double aMat[3][3]) const;

   //! Set the rotation angles in order:  RZ/Yaw/Psi, RY/Pitch/Theta, RX/Roll/Phi
   void Set(double aPsi, double aTheta, double aPhi);

   //! Get the rotation angles in order:  RZ/Yaw/Psi, RY/Pitch/Theta, RX/Roll/Phi
   void Get(double& aPsi, double& aTheta, double& aPhi);

   void Set(const UtQuaternion& aRhs);

   void Set(const double aMat[3][3]);

   void GetRow(size_t aRow, double aUnitVec[3]) const;

   UtVec3dX GetRow(size_t aRow) const;

   void GetCol(size_t aCol, double aUnitVec[3]) const;

   UtVec3dX GetCol(size_t aCol) const;

   const double* operator[](size_t aIndex) const { return &mMat[aIndex][0]; }

   // ===== Miscellaneous =====

   UtDCM Transpose() const;

   //! If a "suspect" DCM is received from another simulation, this tests the DCM for validity.
   //! Recommended for debug use only, as routine run-time use on all DCMs is probably too time intensive
   bool IsValid(bool aPrintErr = false) const;

   //! (Temporary) automated build testing function, prior to fully implemented scriptability of the class.
   bool Test() const;

   //! Use this method carefully , as it breaks encapsulation.
   void SetVal(size_t aRow, size_t aCol, double aVal) { mMat[aRow][aCol] = aVal; }

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      for (int i = 0; i < 3; ++i)
      {
         for (int j = 0; j < 3; ++j)
         {
            aBuff& mMat[i][j];
         }
      }
   }

protected:
   double mMat[3][3];
};

#endif
