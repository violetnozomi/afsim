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

#ifndef UTCOVARIANCEELLIPSOID_HPP
#define UTCOVARIANCEELLIPSOID_HPP

#include "ut_export.h"

#include "UtMatrix.hpp"

//! A utility class that converts a covariance matrix into an ellipsoid.  The
//! ellipsoid is oriented in the NED coordinate frame. The input is also in NED
//! (to which a UtCovariance can be converted from WCS).
class UT_EXPORT UtCovarianceEllipsoid
{
public:
   UtCovarianceEllipsoid();
   virtual ~UtCovarianceEllipsoid() = default;

   //! Factory functions, which try to convert the covariance into an NEU ellipsoid.
   //! Note: It is based on Eigen vectors which can fail (non-convergence).
   //! @argument aPositionCovarianceNED - the covariance to convert
   //! @return - the NED ellipsoid
   static UtCovarianceEllipsoid CovarianceNED2Ellipsoid(const UtMatrixd& aPositionCovarianceNED);

   static UtCovarianceEllipsoid CovarianceNED2Ellipsoid(UtMatrixd& aPositionCovarianceNED,
                                                        UtMatrixd& aDiagonalElements,
                                                        UtMatrixd& aOffDiagonalElements);

   // Accessors
   double GetOrientationHeading() { return mHeadingNED; }
   double GetOrientationPitch() { return mPitchNED; }
   double GetOrientationRoll() { return mRollNED; }
   double GetSemiAxisForward() { return mSemiAxisForwardM; }
   double GetSemiAxisSide() { return mSemiAxisSideM; }
   double GetSemiAxisUp() { return mSemiAxisUpM; }
   void   SetOrientationHeadingDeg(double aHeadingNED) { mHeadingNED = aHeadingNED; }
   void   SetOrientationPitchDeg(double aPitchNED) { mPitchNED = aPitchNED; }
   void   SetOrientationRollDeg(double aRollNED) { mRollNED = aRollNED; }
   void   SetSemiAxisForward(double aSemiAxisForwardM) { mSemiAxisForwardM = aSemiAxisForwardM; }
   void   SetSemiAxisSide(double aSemiAxisSideM) { mSemiAxisSideM = aSemiAxisSideM; }
   void   SetSemiAxisUp(double aSemiAxisUpM) { mSemiAxisUpM = aSemiAxisUpM; }

protected:
   double mHeadingNED;
   double mPitchNED;
   double mRollNED;
   double mSemiAxisForwardM;
   double mSemiAxisSideM;
   double mSemiAxisUpM;
};
#endif
