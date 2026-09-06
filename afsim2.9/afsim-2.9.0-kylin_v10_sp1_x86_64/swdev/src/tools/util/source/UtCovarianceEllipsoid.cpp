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

#include "UtCovarianceEllipsoid.hpp"

#include "UtEntity.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"

UtCovarianceEllipsoid::UtCovarianceEllipsoid()
   : mHeadingNED(0.0)
   , mPitchNED(0.0)
   , mRollNED(0.0)
   , mSemiAxisForwardM(0.0)
   , mSemiAxisSideM(0.0)
   , mSemiAxisUpM(0.0)
{
}

// static
UtCovarianceEllipsoid UtCovarianceEllipsoid::CovarianceNED2Ellipsoid(const UtMatrixd& aPositionCovarianceNED)
{
   UtMatrixd copyP(3, 3);
   copyP.CopyFrom(aPositionCovarianceNED, 0, 0, 3, 3);
   UtMatrixd diagonalElements(3);
   UtMatrixd offDiagonalElements(3);
   return CovarianceNED2Ellipsoid(copyP, diagonalElements, offDiagonalElements);
}

// static
UtCovarianceEllipsoid UtCovarianceEllipsoid::CovarianceNED2Ellipsoid(UtMatrixd& aPositionCovarianceNED,
                                                                     UtMatrixd& aDiagonalElements,
                                                                     UtMatrixd& aOffDiagonalElements)
{
   // Just to make these facts clear.  The aOffDiagonalElements is just used internally
   //  and should end-up an empty set.
   UtMatrixd& eigenVectors = aPositionCovarianceNED;
   UtMatrixd& eigenValues  = aDiagonalElements;

   UtCovarianceEllipsoid data;

   // Capture this value in case of failure, and before
   // the reduction
   double X2 = aPositionCovarianceNED(0, 0);

   try
   {
      aPositionCovarianceNED.ReduceToTridiagonal(aDiagonalElements, aOffDiagonalElements);
      aPositionCovarianceNED.ReduceToEigenvectorsFromTridiagonal(aDiagonalElements, aOffDiagonalElements);
   }
   catch (UtMatrixd::NonConvergenceError&)
   {
      // Give it some dimension so the user can see something. And make it
      // a sphere, so it is conspicuous.
      data.mHeadingNED = 0.0;
      data.mPitchNED   = 0.0;
      data.mRollNED    = 0.0;
      // Make  it a sphere?
      data.mSemiAxisForwardM = 5 * X2;
      data.mSemiAxisSideM    = 5 * X2;
      data.mSemiAxisUpM      = 5 * X2;
      return data;
   }

   // The eigen vectors are in column order; it "is the" DCM rotation matrix,
   // and we can get the angles from it.

   double R3[3][3];

   R3[0][0] = eigenVectors(0, 0);
   R3[0][1] = eigenVectors(1, 0);
   R3[0][2] = eigenVectors(2, 0);
   R3[1][0] = eigenVectors(0, 1);
   R3[1][1] = eigenVectors(1, 1);
   R3[1][2] = eigenVectors(2, 1);
   R3[2][0] = eigenVectors(0, 2);
   R3[2][1] = eigenVectors(1, 2);
   R3[2][2] = eigenVectors(2, 2);

   if (UtMat3<double>::Determinant(R3) < 0.0)
   {
      R3[0][0] *= -1.0;
      R3[0][1] *= -1.0;
      R3[0][2] *= -1.0;
   }

   data.mSemiAxisForwardM = sqrt(std::abs(eigenValues(0)));
   data.mSemiAxisSideM    = sqrt(std::abs(eigenValues(1)));
   data.mSemiAxisUpM      = sqrt(std::abs(eigenValues(2)));

   UtEntity::ExtractEulerAngles(R3, data.mHeadingNED, data.mPitchNED, data.mRollNED);

   data.mHeadingNED = UtMath::NormalizeAngle0_TwoPi(data.mHeadingNED);
   data.mPitchNED   = UtMath::NormalizeAngleMinusPi_Pi(data.mPitchNED);
   data.mRollNED    = UtMath::NormalizeAngleMinusPi_Pi(data.mRollNED);

   return data;
}
