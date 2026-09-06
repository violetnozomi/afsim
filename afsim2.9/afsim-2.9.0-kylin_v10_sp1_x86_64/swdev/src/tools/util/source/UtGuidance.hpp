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

#ifndef UT_GUIDANCE_HPP
#define UT_GUIDANCE_HPP

#include "ut_export.h"

#include "UtInput.hpp"

/**
   This is a utility to compute inertial line-of-sight angular rates, and
   velocity pursuit and proportional navigation desired accelerations.  In
   theory, there are two possible ways of computing line of sight rates;
   A) Use a previous time-stamped value of target vector, and compute the
   rate of change from past to present, or B) using the inertial positions
   and velocities of the observer and target, and a vector cross-product
   to compute instantaneous angular rates.  Method B does not require a
   past value or time stamp.  (NOTE:  Method B was tested and not proven
   to function properly, so has been commented out below, until repaired.)
   The former method keeps past state, and so must be a class function.
   The output is a desired acceleration, which may not be kinematically
   achievable in all cases, and should be constrained in the calling
   application.  Functions are available to formulate the arguments
   as double vector/matrices, or vector class objects, but the underlying
   algorithms are identical in either case.  All input vectors must be in
   the inertial frame.

   NOTE:  Note the use use of the _VELOCITY_ frame for all acceleration
   calculations, not ECS frame.

   An available option is to set bias levels for horizontal and vertical line-
   of-sight rates which cause the guidance to take on a lateral or vertical
   curvature.  (Some platforms are often vertical-biased to preserve energy in
   the vertical plane, and some radar sensor models are able to improve SNRs
   if a lateral line-of-sight rate is flown to the target intercept.)  This
   option will not have any effect unless pro-nav is in use.  A positive
   vertical bias causes a depressed trajectory, pulling up into the target
   from below, and a positive horizontal bias causes the trajectory to bulge
   to the left, with a continuous right turn into the target.  These biases
   must be small, on the order of fractions of a degree per second!

   This algorithm does not consider any acceleration adjustment to oppose
   gravity sag, beyond a possible vertical LOSR bias.  If needed, this must
   be imposed by the owning application.
*/

#include "UtVec3dX.hpp"
class UtInput;
#include "UtDCM.hpp"

class UT_EXPORT UtGuidance
{
public:
   UtGuidance(bool aOpposeGravity = false, bool aInertialIsNED = false);

   /*void AccelerationVel(const double aEyePoint[3],
                        const double aEyeVelocity[3],
                        const double aTgtPoint[3],
                        const double aTgtVelocity[3],
                        const double aInertialToVel[3][3],
                        double       aAccelVel[3]);

   UtVec3dX AccelerationVel(const UtVec3dX& aEyePoint,
                            const UtVec3dX& aEyeVelocity,
                            const UtVec3dX& aTgtPoint,
                            const UtVec3dX& aTgtVelocity,
                            const UtDCM&    aInertialToVel);*/

   void AccelerationVel(double       aSimTime,
                        const double aEyePoint[3],
                        const double aEyeVelocity[3],
                        const double aTgtPoint[3],
                        const double aInertialToVel[3][3],
                        const double aInertialToECS[3][3],
                        const double aInertialToNED[3][3],
                        double       aAccelVel[3]);

   UtVec3dX AccelerationVel(double          aSimTime,
                            const UtVec3dX& aEyePoint,
                            const UtVec3dX& aEyeVelocity,
                            const UtVec3dX& aTgtPoint,
                            const UtDCM&    aInertialToVel,
                            const UtDCM&    aInertialToECS,
                            const UtDCM&    aInertialToNED);

   void Reset();

   bool ProcessInput(UtInput& aInput);

   void SetProportionalNavigationGain(double aValue) { mProNavGain = fabs(aValue); }
   void SetVelocityPursuitGain(double aValue) { mVelPursuitGain = fabs(aValue); }

   void SetHorizontalLOSR_Bias(double aValue) { mHorizontalLOSR_Bias = aValue; }
   void SetVerticalLOSR_Bias(double aValue) { mVerticalLOSR_Bias = aValue; }

protected:
   double   mPrevSimTime;
   double   mProNavGain;
   double   mVelPursuitGain;
   double   mHorizontalLOSR_Bias; // positive right
   double   mVerticalLOSR_Bias;   // positive up (often called Gee-Bias)
   UtVec3dX mPrevInertialUnitLOS;
   UtVec3dX mInertialUnitLOS;
};

#endif
