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

#ifndef WSFHYBRID_HPP
#define WSFHYBRID_HPP

#include "wsf_export.h"

#include "WsfMover.hpp"

class UtInput;
class WsfFollower;
class WsfWaypointMover;

//! A specialized mover that consolidates the functionality of the WsfFollower mover
//! and WsfWaypointMover mover.
class WSF_EXPORT WsfHybridMover : public WsfMover
{
public:
   enum MoverType
   {
      cINVALID,
      cFOLLOWER,
      cWAYPOINT
   };

   WsfHybridMover() = delete;
   WsfHybridMover(WsfScenario& aScenario);
   WsfHybridMover& operator=(const WsfHybridMover&) = delete;
   ~WsfHybridMover() override;

   //! @name Common framework methods
   //@{
   WsfMover*        Clone() const override;
   WsfSpatialDomain GetSpatialDomain() const override;
   bool             Initialize(double aSimTime) override;
   bool             ProcessInput(UtInput& aInput) override;
   void             Update(double aSimTime) override;
   //@}

   //! Sets the follower mover.
   //! Note: this must be set prior to the call to Initialize.
   void SetFollowerMover(WsfFollower* aFollowerMoverPtr) { mFollowerMoverPtr = aFollowerMoverPtr; }

   //! Sets the waypoint mover.
   //! Note: this must be set prior to the call to Initialize.
   void SetWaypointMover(WsfMover* aWaypointMoverPtr) { mWaypointMoverPtr = aWaypointMoverPtr; }

   void SetType(MoverType aMoverType);

   MoverType GetType() const { return mCurrentMoverType; }

   WsfMover* Get(MoverType aMoverType) const;

   WsfMover* GetCurrent() const;

   // The following methods implement the navigation interface in the base class.

   const WsfRoute* GetRoute() const override;

   const WsfRoute* GetDefaultRoute() const override;

   bool GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute) override;

   bool GoToClosestPoint(double aSimTime) override;

   bool GoToLabel(double aSimTime, WsfStringId aLabelId) override;

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;

   bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection) override;

   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange,
                              double                 aRadialAccel,
                              WsfPath::TurnDirection aTurnDirection) override;

   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;

private:
   WsfHybridMover(const WsfHybridMover& aSrc);

   MoverType    mCurrentMoverType;
   WsfFollower* mFollowerMoverPtr;
   WsfMover*    mWaypointMoverPtr;

   bool mAutoSwitch;
   bool mInitialized;
};

#endif
