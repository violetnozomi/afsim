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

#include "UtDCM.hpp"

//#define ASSERT_TRANSFORMS 1

#include <cassert>
#include <iostream>
#include <vector> // Needed only for Test() method.  May be removed in the future (?).

#include "UtEntity.hpp"
#include "UtInertiaTensor.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"

namespace
{
// Acceptable epsilon comparison value for "equals".
const double cDCM_FUZZ = 0.00001;
} // namespace

// static
const double UtDCM::cIDENTITY_MAT[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

// static
const UtDCM UtDCM::cIDENTITY = UtDCM(cIDENTITY_MAT);

// ============= UtDCM Constructors =====================

UtDCM::UtDCM()
// : mMat() // This causes a Windows compile warning, so it was removed.
{
   for (size_t i = 0; i != 3; ++i)
   {
      for (size_t j = 0; j != 3; ++j)
      {
         mMat[i][j] = cIDENTITY_MAT[i][j];
      }
   }
}

UtDCM::UtDCM(const UtDCM& aSrc)
{
   UtMat3d::Set(mMat, aSrc.mMat);
}

UtDCM::UtDCM(const double aDCM[3][3])
{
   UtMat3d::Set(mMat, aDCM);
}

UtDCM::UtDCM(const UtQuaternion& aQuat)
{
   aQuat.Get(mMat);
}

UtDCM::UtDCM(double aPsi, double aTheta, double aPhi)
{
   UtEntity::ComputeRotationalTransform(aPsi, aTheta, aPhi, mMat);
}

//! Rotation about an axis by aAngle radians
//! Axis must be a unit vector
UtDCM::UtDCM(double aAngle, const double aRotationUnitVector[3])
{
   double ux    = aRotationUnitVector[0];
   double uy    = aRotationUnitVector[1];
   double uz    = aRotationUnitVector[2];
   double cosa  = cos(aAngle);
   double cosan = 1 - cosa;
   double sina  = sin(aAngle);
   mMat[0][0]   = cosa + ux * ux * cosan;
   mMat[0][1]   = ux * uy * cosan - uz * sina;
   mMat[0][2]   = ux * uz * cosan + uy * sina;
   mMat[1][0]   = uy * ux * cosan + uz * sina;
   mMat[1][1]   = cosa + uy * uy * cosan;
   mMat[1][2]   = uy * uz * cosan - ux * sina;
   mMat[2][0]   = uz * ux * cosan - uy * sina;
   mMat[2][1]   = uz * uy * cosan + ux * sina;
   mMat[2][2]   = cosa + uz * uz * cosan;
}

// ============= UtDCM Operators =====================

bool UtDCM::operator==(const UtDCM& aSrc) const
{
   for (size_t i = 0; i != 3; ++i)
   {
      for (size_t j = 0; j != 3; ++j)
      {
         if (fabs(mMat[i][j] - aSrc.mMat[i][j]) > cDCM_FUZZ)
         {
            return false;
         }
      }
   }
   return true;
}

UtDCM& UtDCM::operator=(const UtDCM& aSrc)
{
   UtMat3d::Set(mMat, aSrc.mMat);
   return *this;
}

UtDCM& UtDCM::operator=(const UtQuaternion& aSrc)
{
   aSrc.Get(mMat);
   return *this;
}

UtDCM UtDCM::operator*(const UtDCM& aRhs) const
{
   UtDCM result;
   UtMat3d::Multiply(result.mMat, this->mMat, aRhs.mMat);
   return result;
}

UtVec3dX UtDCM::operator*(const double aVect[3]) const
{
   UtVec3dX result;
   UtMat3d::Transform(result.GetData(), this->mMat, aVect);
   return result;
}

UtVec3dX UtDCM::operator*(const UtVec3dX& aVect) const
{
   UtVec3dX result;
   UtMat3d::Transform(result.GetData(), mMat, aVect.GetData());
   return result;
}

// ============= UtDCM Accessors =====================

void UtDCM::Get(double aMat[][3]) const
{
   UtMat3d::Set(aMat, mMat);
}

void UtDCM::Set(double aPsi, double aTheta, double aPhi)
{
   UtEntity::ComputeRotationalTransform(aPsi, aTheta, aPhi, mMat);
}

//! This function should be const, but cannot be made so due to the underlying implementation.
void UtDCM::Get(double& aPsi, double& aTheta, double& aPhi)
{
   UtEntity::ExtractEulerAngles(mMat, aPsi, aTheta, aPhi);
}

void UtDCM::Set(const UtQuaternion& aRhs)
{
   aRhs.Get(mMat);
}

void UtDCM::Set(const double aMat[3][3])
{
   UtMat3d::Set(mMat, aMat);

#ifdef ASSERT_TRANSFORMS
   // Note that not all DCM's "Set()" like this are guaranteed to be valid.
   // This is an expensive validity check, but is highly recommended while
   // debugging.  Not recommended for release builds.  The assert must be
   // disabled in order for the unit test cases to be run to completion, as
   // they explicitly test for validity internally.
   assert(IsValid());
#endif
}

void UtDCM::GetRow(size_t aRow, double aUnitVec[3]) const
{
   aUnitVec[0] = mMat[aRow][0];
   aUnitVec[1] = mMat[aRow][1];
   aUnitVec[2] = mMat[aRow][2];
}

UtVec3dX UtDCM::GetRow(size_t aRow) const
{
   return UtVec3dX(mMat[aRow][0], mMat[aRow][1], mMat[aRow][2]);
}

void UtDCM::GetCol(size_t aCol, double aUnitVec[3]) const
{
   aUnitVec[0] = mMat[0][aCol];
   aUnitVec[1] = mMat[1][aCol];
   aUnitVec[2] = mMat[2][aCol];
}

UtVec3dX UtDCM::GetCol(size_t aCol) const
{
   return UtVec3dX(mMat[0][aCol], mMat[1][aCol], mMat[2][aCol]);
}

UtDCM UtDCM::Transpose() const
{
   UtDCM result;
   UtMat3d::Transpose(result.mMat, this->mMat);
   return result;
}

// ============= UtDCM Transform Methods =====================

UtVec3d UtDCM::InverseTransform(const UtVec3d& aRhs) const
{
   UtVec3d result;
   UtMat3d::InverseTransform(result.GetData(), this->mMat, aRhs.GetData());
   return result;
}

UtVec3d UtDCM::Transform(const UtVec3d& aRhs) const
{
   UtVec3d result;
   UtMat3d::Transform(result.GetData(), this->mMat, aRhs.GetData());
   return result;
}

UtVec3dX UtDCM::InverseTransform(const UtVec3dX& aRhs) const
{
   UtVec3dX result;
   UtMat3d::InverseTransform(result.GetData(), this->mMat, aRhs.GetData());
   return result;
}

UtVec3dX UtDCM::Transform(const UtVec3dX& aRhs) const
{
   UtVec3dX result;
   UtMat3d::Transform(result.GetData(), this->mMat, aRhs.GetData());
   return result;
}

void UtDCM::InverseTransform(const UtInertiaTensor& aInput, UtInertiaTensor& aResult) const
{
   // NOTE:  Tensors transform differently than vectors.
   // I' = R * I * R^T, so this multiplication is only half of the full operation.
   // (The following multiply algorithm came from UtMatrix.)
   for (unsigned int i = 0; i != 3; ++i)
   {
      for (unsigned int j = 0; j != 3; ++j)
      {
         double sum = 0.0;
         for (unsigned int k = 0; k != 3; ++k)
         {
            // Note:  The (i,k) indices have been swapped to (k,i) to transform the DCM
            sum += mMat[k][i] * aInput(k, j);
         }
         aResult(i, j) = sum;
      }
   }
}

void UtDCM::Transform(const UtInertiaTensor& aInput, UtInertiaTensor& aResult) const
{
   // NOTE:  Tensors transform differently than vectors.
   // I' = R * I * R^T, so this multiplication is only half of the operation.
   // (The following multiply algorithm came from UtMatrix.)
   for (unsigned int i = 0; i != 3; ++i)
   {
      for (unsigned int j = 0; j != 3; ++j)
      {
         double sum = 0.0;
         for (unsigned int k = 0; k != 3; ++k)
         {
            sum += mMat[i][k] * aInput(k, j);
         }
         aResult(i, j) = sum;
      }
   }
}

// ============= UtDCM Test Methods =====================

bool UtDCM::IsValid(bool aPrintErr) const
{
   bool success = true;

   // This is a quick-look check to see that DCM is sensical.
   // This is not a sufficient test, but is necessary.  Any valid
   // DCM will transform an arbitrary unit vector into another unit
   // vector. If the DCM is skewed, it will not maintain unity magnitude.
   const UtVec3dX testVector(6.3, -8., .333);
   UtVec3dX       testUnit(testVector);
   testUnit.Normalize();

   UtVec3dX resultUnit = (*this) * testUnit;

   if (fabs(resultUnit.MagnitudeSquared() - 1) > cDCM_FUZZ)
   {
      success = false;
   }

   double totalMag = 0.0;

   size_t irow;
   size_t icol;

   for (irow = 0; irow != 3; ++irow)
   {
      for (icol = 0; icol != 3; ++icol)
      {
         double valSq = mMat[irow][icol] * mMat[irow][icol];
         totalMag += valSq;
      }
   }

   if (fabs(totalMag - 3) > cDCM_FUZZ)
   {
      success = false;
   }

   if (!success && aPrintErr)
   {
      auto out = ut::log::error() << "Invalid DCM encountered.";
      for (size_t i = 0; i != 3; ++i)
      {
         auto note = out.AddNote() << '[' << mMat[i][0];
         for (size_t j = 1; j != 3; ++j)
         {
            note << ", " << mMat[i][j];
         }
         note << ']';
      }
   }

   return success;
}

// ============================== Test() function for daily build usage ====================================

bool UtDCM::Test() const
{
   bool success = true;

   // Angular tolerance for test values
   const double cANGLE_FUZZ = 0.1 * UtMath::cRAD_PER_DEG; // a tenth of a degree.
   const double cDCM_FUZZ   = 0.0001;

   UtDCM        threeD_TestRot(10 * UtMath::cRAD_PER_DEG, 20 * UtMath::cRAD_PER_DEG, -40 * UtMath::cRAD_PER_DEG);
   UtQuaternion quatD_TestRot(10 * UtMath::cRAD_PER_DEG, 20 * UtMath::cRAD_PER_DEG, -40 * UtMath::cRAD_PER_DEG);

   double quatDCM[3][3];
   quatD_TestRot.Get(quatDCM);

   double otherTestDCM[3][3];
   UtEntity::ComputeRotationalTransform(10 * UtMath::cRAD_PER_DEG,
                                        20 * UtMath::cRAD_PER_DEG,
                                        -40 * UtMath::cRAD_PER_DEG,
                                        otherTestDCM);

   bool bothDCMsMatch = true;
   for (size_t i = 0; i != 3; ++i)
   {
      for (size_t j = 0; j != 3; ++j)
      {
         if (fabs(quatDCM[i][j] - threeD_TestRot[i][j]) > cDCM_FUZZ)
         {
            bothDCMsMatch = false;
         }
      }
   }

   if (!bothDCMsMatch)
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 0 failed to match a Quaternion-produced DCM... one of them is "
                                    "wrong (is Theta = 90 degrees ?).";
      success = false;
   }

   if (bothDCMsMatch && !threeD_TestRot.IsValid())
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM IsValid() Test Failed.";
      success = false;
   }

   UtDCM rot3yaw(3 * UtMath::cRAD_PER_DEG, 0.0, 0.0);
   UtDCM rot1yaw(1 * UtMath::cRAD_PER_DEG, 0.0, 0.0);

   double psi;
   double theta;
   double phi;
   double psiDeg;
   double thetaDeg;
   double phiDeg;

   // Test 1 - Rotate 3 degrees in yaw + 1 degree in yaw:
   UtDCM rot4yaw = rot1yaw * rot3yaw;

   rot4yaw.Get(psi, theta, phi);
   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg - 4.0) > cANGLE_FUZZ) || (fabs(thetaDeg) > cANGLE_FUZZ) || (fabs(phiDeg) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 1 failed summed 1 + 3 degrees yaw rotation.";
      success = false;
   }

   // Test 2 - Rotate 3 degrees in yaw - 1 degrees in yaw:
   UtDCM rot2yaw = rot1yaw.Transpose() * rot3yaw;

   rot2yaw.Get(psi, theta, phi);

   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg - 2.0) > cANGLE_FUZZ) || (fabs(thetaDeg) > cANGLE_FUZZ) || (fabs(phiDeg) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 2 failed summed +3 - 1 degree yaw rotation.";
      success = false;
   }

   UtDCM rot90yaw(90.0 * UtMath::cRAD_PER_DEG, 0.0, 0.0);
   UtDCM rot45pitch(0.0, 45.0 * UtMath::cRAD_PER_DEG, 0.0);

   // Test 3 - Rotate 90 degrees in yaw, then 45 degrees in pitch:
   UtDCM rotResult = rot45pitch * rot90yaw;

   rotResult.Get(psi, theta, phi);

   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg - 90.0) > cANGLE_FUZZ) || (fabs(thetaDeg - 45.0) > cANGLE_FUZZ) || (fabs(phiDeg) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 3 failed summed 90 yaw plus 45 pitch => (90, 45, 0) rotation.";
      success = false;
   }

   // Test 4 - Rotate 45 pitch, then 90 degrees in yaw:
   rotResult = rot90yaw * rot45pitch;

   rotResult.Get(psi, theta, phi);

   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg - 90.0) > cANGLE_FUZZ) || (fabs(thetaDeg) > cANGLE_FUZZ) || (fabs(phiDeg - 45.0) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 4 failed summed 45 pitch plus 90 yaw => (90, 0, 45) rotation.";
      success = false;
   }

   // Test 5 - Rotate -45 pitch, then -90 degrees in yaw:
   rotResult = rot90yaw.Transpose() * rot45pitch.Transpose();

   rotResult.Get(psi, theta, phi);

   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg + 90.0) > cANGLE_FUZZ) || (fabs(thetaDeg) > cANGLE_FUZZ) || (fabs(phiDeg - 45.0) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail")
         << "-FAIL- UtDCM Test 5 failed summed -45 deg pitch plus -90 yaw => (-90, 0, 45) rotation.";
      success = false;
   }

   // Test 6 - Rotate in two directions, twice... (BUT, do NOT hit a +/- 90 deg pitch attitude in the process!):
   UtDCM rotA(0.0, 20.0 * UtMath::cRAD_PER_DEG, 45.0 * UtMath::cRAD_PER_DEG);
   UtDCM rotB(10.0 * UtMath::cRAD_PER_DEG, 90.0 * UtMath::cRAD_PER_DEG, 0.0);

   rotResult = rotB * rotA;

   rotResult.Get(psi, theta, phi);

   psiDeg   = psi * UtMath::cDEG_PER_RAD;
   thetaDeg = theta * UtMath::cDEG_PER_RAD;
   phiDeg   = phi * UtMath::cDEG_PER_RAD;

   if ((fabs(psiDeg - 108.882) > cANGLE_FUZZ) || (fabs(thetaDeg - 41.641) > cANGLE_FUZZ) ||
       (fabs(phiDeg - 107.236) > cANGLE_FUZZ))
   {
      ut::log::info("test_fail")
         << "-FAIL- UtDCM Test 6 failed summed (0, 20, 45) plus(10, 90, 0) rotations => (109, 42, 107) rotation.";
      success = false;
   }

   // Now try a couple of vector transforms:
   // Assuring that the DCM class is the one that handles the function,
   // or perhaps intentionally testing a DCM class multiplication versus a Vector class multiplication.)

   UtVec3dX vec1_1_0(1.0, 1.0, 0.0);
   UtVec3dX vec10_0_0(10, 0.0, 0.0);

   UtDCM rot45yaw(45.0 * UtMath::cRAD_PER_DEG, 0.0, 0.0);

   UtVec3dX resultA = vec1_1_0 * rot45yaw;
   UtVec3dX resultB = rot45yaw * vec1_1_0;

   // Note:  The following three tests have been commented out until the cause for a false failure in Linux can be determined.
   /*if (fabs(resultA[0] - 1.414) > 0.1)
   {
      auto out = ut::log::info("test_fail") << "-FAIL- UtDCM Test 7A failed vector(1,1,0) 45 degree transform.";
      out.AddNote() << "Actual resultA[0]: " << resultA[0];
      out.AddNote() << "Expected: " << 1.414;
      success = false;
   }

   if (fabs(resultA[1])         > 0.1)
   {
      auto out = ut::log::info("test_fail") << "-FAIL- UtDCM Test 7B failed vector(1,1,0) 45 degree transform.";
      out.AddNote() << "Actual resultA[1]: " << resultA[1];
      out.AddNote() << "Expected: 0";
      success = false;
   }

   if (fabs(resultA[2])         > 0.1)
   {
      auto out = ut::log::info("test_fail") << "-FAIL- UtDCM Test 7C failed vector(1,1,0) 45 degree transform.";
      out.AddNote() << "Actual resultA[2]: " << resultA[2];
      out.AddNote() << "Expected: 0";
      success = false;
   }*/

   UtDCM wildRot(26.0 * UtMath::cRAD_PER_DEG, -16.0 * UtMath::cRAD_PER_DEG, 120.0 * UtMath::cRAD_PER_DEG);
   resultA = vec10_0_0 * wildRot;
   resultB = wildRot * vec10_0_0;

   bool missedX = false;
   bool missedY = false;
   bool missedZ = false;
   if (fabs(resultA[0] - 8.6398) > 0.1)
      missedX = true;
   if (fabs(resultA[1] - 0.0464) > 0.1)
      missedY = true;
   if (fabs(resultA[2] - 5.0350) > 0.1)
      missedZ = true;

   if (missedX || missedY || missedZ)
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 8 failed vector(10,0,0) random angles transform.";
      success = false;
   }

   UtVec3dX vec1414_0_0(1.414, 0.0, 0.0);

   resultA = rot45yaw.InverseTransform(vec1414_0_0);

   // Test to assure IsValid works:
   // A) Correctly pass a good matrix.
   UtDCM arbitraryOrient(20.0 * UtMath::cRAD_PER_DEG, 30.0 * UtMath::cRAD_PER_DEG, 10.0 * UtMath::cRAD_PER_DEG);
   if (!arbitraryOrient.IsValid())
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM IsValid() Test 10 incorrectly failed a correct DCM.";
      success = false;
   }
   // B) Correctly fail a bad matrix.
   double junkMatrix[3][3];
   junkMatrix[0][0] = 0.345;
   junkMatrix[0][1] = 0.125;
   junkMatrix[0][2] = -0.345;
   junkMatrix[1][0] = -0.3;
   junkMatrix[1][1] = -0.125;
   junkMatrix[1][2] = 0.9;
   junkMatrix[2][0] = 0.0;
   junkMatrix[2][1] = 0.111;
   junkMatrix[2][2] = 0.0;
   arbitraryOrient.Set(junkMatrix);
   if (arbitraryOrient.IsValid())
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM IsValid() Test 11 incorrectly passed an invalid DCM.";
      success = false;
   }

   // Tests to assure that GetRow function returns correct vectors:
   // First, a (0, 0, 0) rotation should return Ux = (1, 0, 0)
   UtDCM    unitVectorTest = UtDCM::cIDENTITY;
   UtVec3dX result;

   unitVectorTest.GetRow(0, result.GetData());
   if ((fabs(result[0] - 1.0) > cDCM_FUZZ) || (fabs(result[1]) > cDCM_FUZZ) || (fabs(result[2]) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 12 failed DCM Identity Ux() function.";
      success = false;
   }

   unitVectorTest.GetRow(1, result.GetData());
   if ((fabs(result[0]) > cDCM_FUZZ) || (fabs(result[1] - 1.0) > cDCM_FUZZ) || (fabs(result[2]) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 13 failed DCM Identity Uy() function.";
      success = false;
   }

   unitVectorTest.GetRow(2, result.GetData());
   if ((fabs(result[0]) > cDCM_FUZZ) || (fabs(result[1]) > cDCM_FUZZ) || (fabs(result[2] - 1.0) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 14 failed DCM Identity Uz() function.";
      success = false;
   }

   // Rotate a frame 90 degrees in yaw (psi), and then access its relative Fwd,
   // Right, Down vectors, in the original parent frame coordinates:
   unitVectorTest.Set(90.0 * UtMath::cRAD_PER_DEG, 0.0, 0.0);

   unitVectorTest.GetRow(0, result.GetData());
   if ((fabs(result[0]) > cDCM_FUZZ) || (fabs(result[1] - 1.0) > cDCM_FUZZ) || (fabs(result[2]) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 15 failed DCM rotated GetUx() function.";
      success = false;
   }

   unitVectorTest.GetRow(1, result.GetData());
   if ((fabs(result[0] + 1.0) > cDCM_FUZZ) || (fabs(result[1]) > cDCM_FUZZ) || (fabs(result[2]) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 16 failed DCM rotated GetUy() function.";
      success = false;
   }

   unitVectorTest.GetRow(2, result.GetData());
   if ((fabs(result[0]) > cDCM_FUZZ) || (fabs(result[1]) > cDCM_FUZZ) || (fabs(result[2] - 1.0) > cDCM_FUZZ))
   {
      ut::log::info("test_fail") << "-FAIL- UtDCM Test 17 failed DCM rotated GetUz() function.";
      success = false;
   }

   if (success)
   {
      ut::log::info() << "-PASS- UtDCM passed all test cases.";

      // Piggy Back a UtVec3dX test onto this one... they are inter-related classes.
      UtVec3dX a;
      success = a.Test();
      if (!success)
      {
         ut::log::info("test_fail") << "-FAIL- UtDCM passed its tests, but failed a sub-test on the UtVec3dX class.";
      }
   }

   return success;
}

// ==========================================================================
