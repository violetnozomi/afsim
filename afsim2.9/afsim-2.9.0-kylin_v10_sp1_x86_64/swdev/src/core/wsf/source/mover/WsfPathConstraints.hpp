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

#ifndef WSFPATHCONSTRAINTS_HPP
#define WSFPATHCONSTRAINTS_HPP

#include "wsf_export.h"

class UtInput;

//! Contains constraints used for computing a path in WsfPathComputer
struct WSF_EXPORT WsfPathConstraints
{
   static const double cDEFAULT_MAX_FLIGHT_ANGLE;
   static const double cDEFAULT_MAX_LINEAR_ACCEL;
   static const double cDEFAULT_MAX_RADIAL_ACCEL;
   static const double cLARGE_DOUBLE;
   WsfPathConstraints();

   bool ProcessInput(UtInput& aInput);

   //! For XIO (de)serialization.
   template<typename BUFF>
   void Serialize(BUFF& aBuff)
   {
      aBuff& mMaxSpeed& mMinSpeed& mMaxLinearAccel& mMaxClimbRate& mMaxFlightPathAngle& mMaxAltitude& mMinAltitude& mTurnRateLimit&
         mRollAngleLimit& mBodyVertLimit& mMaxRadialAccel& mRollRateLimit& mHeadingPursuitGain& mMaxTurnRate& mIsOnGround;
   }

   // Straight line or vertical Attributes:
   double mMaxSpeed;           // m/s
   double mMinSpeed;           // m/s
   double mMaxLinearAccel;     // m/s^2
   double mMaxClimbRate;       // m/s
   double mMaxFlightPathAngle; // radians
   double mMaxAltitude;        // meters
   double mMinAltitude;        // meters

   // Turn Attributes:
   double mTurnRateLimit;      // radians/s
   double mRollAngleLimit;     // radians
   double mBodyVertLimit;      // m/s^2
   double mMaxRadialAccel;     // m/s^2
   double mRollRateLimit;      // radians/s
   double mHeadingPursuitGain; // non-dim
   double mMaxTurnRate;        // this is same thing as 'mTurnRateLimit' above & used the same way...

   // Copied from the mover... indicates of the controlled object is constrained to the ground
   bool mIsOnGround;
};

#endif
