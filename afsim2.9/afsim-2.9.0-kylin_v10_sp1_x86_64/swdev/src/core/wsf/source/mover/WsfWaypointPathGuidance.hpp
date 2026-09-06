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

#ifndef WSFWAYPOINTPATHGUIDANCE_HPP
#define WSFWAYPOINTPATHGUIDANCE_HPP

#include "wsf_export.h"

#include "WsfPathGuidance.hpp"
class WsfWaypoint;

//! A derived guidance class used by WsfMovers that follow WsfWaypoints.
class WSF_EXPORT WsfWaypointPathGuidance : public WsfPathGuidance
{
public:
   WsfWaypointPathGuidance();
   // virtual ~WsfWaypointPathGuidance();
   WsfPathGuidance* Clone() const override;
   // virtual bool Initialize(double aSimTime, WsfMover* aMoverPtr);
   bool ProcessInput(UtInput& aInput) override;

   double GetAltitude() const override;
   double GetSpeed() const override;

   double CalcLateralAccelLimit(double aCurrentSpeed);

   double TurnRateLimit() const { return mTurnRateLimit; }
   double MaxLinearAccel() const { return mMaxLinearAccel; }
   double MaxRadialAccel() const { return mMaxRadialAccel; }

   void   SetRollRateLimit(double aRollRateLimit) { mRollRateLimit = aRollRateLimit; }
   double RollRateLimit() const { return mRollRateLimit; }

   void   SetHeadingPursuitGain(double aHeadingPursuitGain) { mHeadingPursuitGain = aHeadingPursuitGain; }
   double HeadingPursuitGain() const { return mHeadingPursuitGain; }

   double MaxClimbRate() const { return mMaxClimbRate; }
   double MaxFlightPathAngle() const { return mMaxFlightPathAngle; }
   double DefaultRadialAccel() const { return MaxRadialAccel(); } // Provided for backward compatibility.

   void GetWPTargetInfo(double  aVelNED[3],
                        double& aPlatformHeading,
                        double& aPlatformPitch,
                        double& aPlatformRoll,
                        double& aHeadingChangeMag,
                        double& aHdgChgDirection,
                        double& aGroundSpeed,
                        double& aTotalSpeed,
                        double& aGroundDistToTgt,
                        double& aTotalDistToTgt);

   double GetRadialAccelAtPoint(const WsfWaypoint& aPoint) const override;

private:
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
};

#endif
