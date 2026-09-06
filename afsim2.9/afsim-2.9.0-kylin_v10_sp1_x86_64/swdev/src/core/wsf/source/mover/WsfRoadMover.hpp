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

#ifndef WSFROADMOVER_HPP
#define WSFROADMOVER_HPP

#include "wsf_export.h"

#include "WsfGroundMover.hpp"
class WsfRoute;
class WsfTrack;

//! WsfRoadMover is a specialization of WsfGroundMover that moves on a road network.
//! It calculates the shortest path between a start and end point and uses it as its
//! waypoint path. A pause time can be set to offset the start time of the mover.
//! As well, a flag can be set (mUseClosestWaypoint) that causes the mover to plot
//! a shortest path based on the closest waypoints to the user specified start and
//! end positions.

class WSF_EXPORT WsfRoadMover : public WsfGroundMover
{
public:
   // using WsfGroundMover::CreateRoute;
   WsfRoadMover() = delete;
   WsfRoadMover(WsfScenario& aScenario);
   WsfRoadMover& operator=(const WsfRoadMover&) = delete;
   ~WsfRoadMover() override                     = default;

   //! @name Framework methods.
   //!@{
   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   //@}

   WsfRoadMover(const WsfGroundMover& aSrc);

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_LAND; }

   bool GoToLocation(double aSimTime,
                     double aLatitude,
                     double aLongitude,
                     double aAltitude = 0.0) override; // Normally don't care about altitude.

   virtual bool GoToTrack(double aSimTime, WsfTrack* aTrackPtr);

   virtual double GetTravelTime(double aSimTime, WsfTrack* aTrackPtr);

   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;

   void        SetRoadNetworkId(WsfStringId aId) { mRoadNetworkId = aId; }
   WsfStringId GetRoadNetworkId() const { return mRoadNetworkId; }
   void        SetStartLat(double aStartLat)
   {
      mStartLat          = aStartLat;
      mHaveStartPosition = true;
   }
   double GetStartLat() const { return mStartLat; }
   void   SetStartLon(double aStartLon)
   {
      mStartLon          = aStartLon;
      mHaveStartPosition = true;
   }
   double GetStartLon() const { return mStartLon; }
   void   SetEndLat(double aEndLat)
   {
      mEndLat          = aEndLat;
      mHaveEndPosition = true;
   }
   double GetEndLat() const { return mEndLat; }
   void   SetEndLon(double aEndLon)
   {
      mEndLon          = aEndLon;
      mHaveEndPosition = true;
   }
   double GetEndLon() const { return mEndLon; }
   void   SetSpeed(double aSpeed) { mSpeed = aSpeed; }
   double GetSpeed() const { return mSpeed; }
   void   SetPauseTime(double aPauseTime) { mPauseTime = aPauseTime; }
   double GetPauseTime() const { return mPauseTime; }
   void   SetUseClosestWaypoint(bool aFlag) { mUseClosestWaypoint = aFlag; }
   bool   GetUseClosestWaypoint() const { return mUseClosestWaypoint; }

protected:
   WsfRoadMover(const WsfRoadMover& aSrc);

   bool SetOrUpdateRoute(double aSimTime);

   bool CreateRoute(double aSimTime, double aStartLat, double aStartLon, double aEndLat, double aEndLon, WsfRoute& aRoute);

   bool CreateRoute(double aSimTime, const WsfWaypoint& aWp1, const WsfWaypoint& aWp2, WsfRoute& aRoute);


   //! Create route between the given locations
   bool CreateRoute(double    aFromLatitude,
                    double    aFromLongitude,
                    double    aFromAltitude,
                    double    aToLatitude,
                    double    aToLongitude,
                    double    aToAltitude,
                    WsfRoute& aRoute) const;

   void MergeWaypoints(const WsfWaypoint& aWpFrom, WsfWaypoint& aWpTo);

private:
   bool AppendRoute(double aSimTime, const WsfWaypoint& aWp1, const WsfWaypoint& aWp2, WsfRoute& aRoute);

   double GetTravelTime(double aSimTime, WsfTrack* aTrackPtr, WsfRoute& aRoute);

   WsfStringId mRoadNetworkId;
   bool        mConsiderOffRoadShortcut;
   double      mStartLat;
   double      mStartLon;
   double      mEndLat;
   double      mEndLon;
   double      mSpeed;
   double      mOffRoadSpeed;
   double      mPauseTime;
   bool        mUseClosestWaypoint;
   bool        mHaveStartPosition;
   bool        mHaveEndPosition;
   double      mLinearAccel;
};

#endif
