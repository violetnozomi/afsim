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

#ifndef WSFMATH3D_MOVER_HPP
#define WSFMATH3D_MOVER_HPP

#include "wsf_export.h"

class UtInput;
#include "UtMath3D_Motion.hpp"
#include "WsfPathGuidance.hpp"
#include "WsfRouteMover.hpp"

/**
   Pure mathematics-based 3-D Motion model.  This class was designed to present
   smooth three dimensional motion, with nearly zero knowledge of the underlying
   physics.  No mass properties, no aerodynamics; the only imposed constraints
   are limits on lateral and longitudinal acceleration.  The velocity vector is
   continuously accelerated to attain a target speed, and then "directed" by
   velocity pursuit and proportional navigation gains toward a target point in
   3-D space.  A singularity occurs when the 3-D target point is reached, so the
   containing application should ALWAYS set a new target point when within one
   turn radius of the old target point.  After a turn to a new waypoint, the
   flight path to the destination will be a straight line, and will NOT consider
   any earth curvature, so all waypoints should be less than 10 nmi distance
   apart, or earth contact may be a problem.
*/

class WSF_EXPORT WsfMath3D_Mover : public WsfRouteMover
{
public:
   WsfMath3D_Mover() = delete;
   WsfMath3D_Mover(WsfScenario& aScenario);
   WsfMath3D_Mover(const WsfMath3D_Mover& aRhs);
   WsfMath3D_Mover& operator=(const WsfMath3D_Mover&) = delete;
   ~WsfMath3D_Mover() override;

   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   UtMath3D_Motion& Motion() { return mMotion; }

   // Base methods that must be implemented by this class,
   // per the documentation provided by WsfRouteMover:
   double TurnRadius();                       // TODO
   double TurnRadius(double aSpeed) override; // TODO
   void   SetGuidanceTo(const WsfWaypoint& aNextWaypoint) override;
   void   SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint) override;
   void   Pause(double aSimTime, double aUnpauseTime) override;
   bool   Unpause(double aSimTime, bool aInterrupt) override;
   void   ResetPosition(double aSimTime) override;
   void   AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr) override;
   void   UpdatePosition(double aSimTime) override;
   bool   GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute) override;
   bool   GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;
   bool   TurnToRelativeHeading(double                 aSimTime,
                                double                 aHeadingChange,
                                double                 aRadialAccel,
                                WsfPath::TurnDirection aTurnDirection) override;
   bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection) override;

protected:
   void SetExternalMotionState();

   // ========== BEGIN ATTRIBUTES =============
   double           mTurnRadius;
   double           mPrepauseVelocityNED[3];
   UtMath3D_Motion  mMotion;
   WsfPathGuidance* mGuidancePtr;
   double           mLocalUpdateTime;
};

#endif
