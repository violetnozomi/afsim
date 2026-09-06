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

#ifndef WSFITERATIVEROUTEMOVER_HPP
#define WSFITERATIVEROUTEMOVER_HPP

#include "wsf_export.h"

#include "WsfRouteMover.hpp"
class WsfPathGuidance;

//! Base class for route movers that use iterative computation.
class WSF_EXPORT WsfIterativeRouteMover : public WsfRouteMover
{
public:
   WsfIterativeRouteMover() = delete;
   WsfIterativeRouteMover(WsfScenario& aScenario);
   WsfIterativeRouteMover(const WsfIterativeRouteMover& aSrc);
   WsfIterativeRouteMover& operator=(const WsfIterativeRouteMover&) = delete;
   ~WsfIterativeRouteMover() override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void Pause(double aSimTime, double aUnpauseTime) override;

   void SetGuidanceFrom(const WsfWaypoint& aPreviousWaypoint) override;
   void SetGuidanceTo(const WsfWaypoint& aPreviousWaypoint) override;

   void AdvanceToNextWaypoint(double aSimTime, const WsfWaypoint& aTarget, const WsfWaypoint* aNextPointPtr) override;

   void UpdatePosition(double aSimTime) override;

   double GetTurnLeadDistance(double aCurrentSpeed, double aCurrentHeading);

   bool GetConverging() const { return mConverging[2]; }

   bool GoToAltitude(double aSimTime, double aAltitude, double aClimbDiveRate, bool aKeepRoute) override;
   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;
   bool GoToSpeed(double aSimTime, double aSpeed, double aLinearAccel, bool aKeepRoute) override;
   bool TurnToHeading(double aSimTime, double aHeading, double aRadialAccel, WsfPath::TurnDirection aTurnDirection) override;
   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange,
                              double                 aRadialAccel,
                              WsfPath::TurnDirection aTurnDirection) override;

   WsfPathGuidance* GetGuidance() const { return mGuidancePtr; }

protected:
   virtual void LocalUpdate(double aSimTime, bool& aMoved, bool& aAdvanceToNextWaypoint) = 0;
   virtual void PostMoveUpdate(double aSimTime);
   void         AdvanceTime(double aSimTime, bool& aAdvanceToNextWaypoint);
   void         SetGuidance(WsfPathGuidance* aGuidancePtr) { mGuidancePtr = aGuidancePtr; }
   void         SetConverging(bool aValue);
   void         ClearConverging();
   void         ResetWPSwitching();
   double       SegmentDistRemaining() const;
   double       SegmentTimeRemaining() const;
   double       CumSegmentDist() const { return mCumSegmentDist; }
   double       CumSegmentTime() const { return mCumSegmentTime; }
   //! Return the current value of minimum turn radius.
   double GetMinTurnRadius() const { return mMinTurnRadius; }

   //! Return an indication of whether this platform is currently sitting on terrain.
   bool GetIsOnGround() const { return mIsOnGround; }
   void SetIsOnGround(bool aValue) { mIsOnGround = aValue; }

   //! Return an indication of whether this platform is currently in air.
   bool GetIsInAir() const { return !mIsOnGround; }

   void IncrSegmentDist(double aIncrement) { mCumSegmentDist += aIncrement; }

   void IncrSegmentTime(double aIncrement) { mCumSegmentTime += aIncrement; }
   void SetMinTurnRadius(double aValue) { mMinTurnRadius = aValue; }

   virtual bool IsWaypointSwitchTime(double aSimTime);

   void   SetGroundAttitude(double aLatitude, double aLongitude);
   void   CalcAltitudesAndAttitude(double /*aSimTime*/);
   double GetHeightOfTerrain() const { return mHeightOfTerrain; }

private:
   void InitializeTurnOnApproach(const WsfWaypoint& aTarget, const WsfWaypoint* aNextTarget);

   bool mIsOnGround;
   bool mIsOnRoad;
   bool mPitchEnable;

   bool mConverging[3]; // Keep three past values as a history buffer

   int mRecursionLevel;

   double mHeightOfTerrain;
   double mCumSegmentDist; // Cumulative distance in waypoint segment.
   double mCumSegmentTime; // Cumulative time in waypoint segment.
   double mMinTurnRadius;  // Min Turn Radius (provided by implementation)
   double mSwitchRadius;   // North-East distance to switch point

   //! The value of the 'forward offset' of the target waypoint during the last waypoint switch evaluation.
   double mLastForwardOffset;

   //! The heading from the target waypoint to the following waypoint. (Used for cTURN_ON_APPROACH)
   double mNextSegHeading;
   //! The radial acceleration to be used for cTURN_ON_APPROACH.
   double mNextSegRadialAccel;

   WsfPathGuidance* mGuidancePtr;
};

#endif
