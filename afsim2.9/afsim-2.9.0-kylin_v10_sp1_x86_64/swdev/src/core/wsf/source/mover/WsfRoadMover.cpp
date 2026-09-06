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

#include "WsfRoadMover.hpp"

#include <cassert>
#include <limits>
#include <string>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPathFinder.hpp"
#include "WsfPlatform.hpp"
#include "WsfRouteNetwork.hpp"
#include "WsfRouteNetworkTypes.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"

// =================================================================================================
WsfRoadMover::WsfRoadMover(WsfScenario& aScenario)
   : WsfGroundMover(aScenario)
   , mRoadNetworkId()
   , mConsiderOffRoadShortcut(false)
   , mStartLat(0.0)
   , mStartLon(0.0)
   , mEndLat(0.0)
   , mEndLon(0.0)
   , mSpeed(0.0)
   , mOffRoadSpeed(0.0)
   , mPauseTime(0.0)
   , mUseClosestWaypoint(false)
   , mHaveStartPosition(false)
   , mHaveEndPosition(false)
   , mLinearAccel(12.0)
{
}

// =================================================================================================
/** Copy constructor (for Clone()) */
WsfRoadMover::WsfRoadMover(const WsfRoadMover& aSrc)
   : WsfGroundMover(aSrc)
   , mRoadNetworkId(aSrc.mRoadNetworkId)
   , mConsiderOffRoadShortcut(aSrc.mConsiderOffRoadShortcut)
   , mStartLat(aSrc.mStartLat)
   , mStartLon(aSrc.mStartLon)
   , mEndLat(aSrc.mEndLat)
   , mEndLon(aSrc.mEndLon)
   , mSpeed(aSrc.mSpeed)
   , mOffRoadSpeed(aSrc.mOffRoadSpeed)
   , mPauseTime(aSrc.mPauseTime)
   , mUseClosestWaypoint(aSrc.mUseClosestWaypoint)
   , mHaveStartPosition(aSrc.mHaveStartPosition)
   , mHaveEndPosition(aSrc.mHaveEndPosition)
   , mLinearAccel(aSrc.mLinearAccel)
{
}

// =================================================================================================
WsfRoadMover::WsfRoadMover(const WsfGroundMover& aSrc)
   : WsfGroundMover(aSrc)
   , mRoadNetworkId()
   , mConsiderOffRoadShortcut(false)
   , mStartLat(0.0)
   , mStartLon(0.0)
   , mEndLat(0.0)
   , mEndLon(0.0)
   , mSpeed(0.0)
   , mOffRoadSpeed(0.0)
   , mPauseTime(0.0)
   , mUseClosestWaypoint(false)
   , mHaveStartPosition(false)
   , mHaveEndPosition(false)
   , mLinearAccel(12.0)
{
}

// =================================================================================================
// virtual
bool WsfRoadMover::GoToLocation(double aSimTime,
                                double aLatitude,
                                double aLongitude,
                                double /*aAltitude*/) //= 0.0; Normally don't care about altitude.
{
   // Get Current lat/lon from platform
   double alt;
   GetPlatform()->GetLocationLLA(mStartLat, mStartLon, alt);
   mEndLat = aLatitude;
   mEndLon = aLongitude;
   WsfRoute route;
   bool     retValue = false;
   if (CreateRoute(aSimTime, mStartLat, mStartLon, mEndLat, mEndLon, route))
   {
      retValue = SetRoute(aSimTime, route);
   }
   return retValue;
}

// =================================================================================================
// virtual
bool WsfRoadMover::GoToTrack(double aSimTime, WsfTrack* aTrackPtr)
{
   bool retValue = false;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform commanded to go to track.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Track ID: " << aTrackPtr->GetTrackId();
      out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
   }

   WsfRoute route;

   // Intercept Collision
   // if (GetTravelTime(aSimTime, aTrackPtr, route) < std::numeric_limits<double>::max())
   //{
   //    retValue = UpdateRoute(aSimTime, route);
   //}
   // else
   //{
   //    ut::log::info() << "-------Unable to find Route";
   //}

   // Chase Pursuit
   double startLat, startLon, alt;
   GetPlatform()->GetLocationLLA(startLat, startLon, alt);
   double otherLocWCS[3];
   double lat, lon;
   aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, otherLocWCS);
   UtEntity::ConvertWCSToLLA(otherLocWCS, lat, lon, alt);
   if (CreateRoute(aSimTime, startLat, startLon, lat, lon, route))
   {
      retValue = SetRoute(aSimTime, route);
   }
   else
   {
      auto out = ut::log::error() << "Platform was unable to find route.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   return retValue;
}

// =================================================================================================
// virtual
double WsfRoadMover::GetTravelTime(double aSimTime, WsfTrack* aTrackPtr)
{
   double travelTime = std::numeric_limits<double>::max();

   WsfRoute route;

   // Lead Intercept
   // travelTime = GetTravelTime(aSimTime, aTrackPtr, route);

   // Pursuit Intercept
   double lat, lon, startLat, startLon, alt;
   GetPlatform()->GetLocationLLA(startLat, startLon, alt);
   double otherLocWCS[3];
   aTrackPtr->GetExtrapolatedLocationWCS(aSimTime, otherLocWCS);
   UtEntity::ConvertWCSToLLA(otherLocWCS, lat, lon, alt);
   if (CreateRoute(aSimTime, startLat, startLon, lat, lon, route))
   {
      travelTime = route.GetTravelTime();
   }
   else
   {
      auto out = ut::log::error() << "Platform was unable to find route.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
   }
   return travelTime;
}

// =================================================================================================
bool WsfRoadMover::UpdateRoute(double aSimTime, const WsfRoute& aRoute)
{
   unsigned int routeSize = aRoute.GetSize();

   // Quick return
   if (routeSize == 0)
   {
      return true;
   }

   // Get Current lat/lon from platform
   double latitude, longitude, altitude;
   GetPlatform()->GetLocationLLA(latitude, longitude, altitude);

   WsfWaypoint wp1(latitude, longitude, altitude);

   unsigned int index = 0;
   WsfWaypoint  wp2   = aRoute.GetWaypointAt(index);

   WsfRoute route;
   bool     foundPath = CreateRoute(aSimTime, wp1, wp2, route);

   wp1 = wp2;
   // Create the intermediate segment routes and append them together
   while (index < routeSize - 1)
   {
      ++index;
      wp2 = aRoute.GetWaypointAt(index);

      if (!foundPath)
      {
         break;
      }
      foundPath &= AppendRoute(aSimTime, wp1, wp2, route);
      wp1 = wp2;
   }

   if (foundPath)
   {
      return WsfGroundMover::UpdateRoute(aSimTime, route);
   }

   auto out = ut::log::error() << "Platform could not generate shortest path from initial position to destination.";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Initial Lat: " << wp1.GetLat();
   out.AddNote() << "Initial Lon: " << wp1.GetLon();
   out.AddNote() << "Dest Lat: " << wp2.GetLat();
   out.AddNote() << "Dest Lon: " << wp2.GetLon();

   return false;
}

// =================================================================================================
// private
bool WsfRoadMover::AppendRoute(double aSimTime, const WsfWaypoint& aWp1, const WsfWaypoint& aWp2, WsfRoute& aRoute)
{
   bool     routeAppended = false;
   WsfRoute aRouteToAppend;
   if (CreateRoute(aSimTime, aWp1, aWp2, aRouteToAppend))
   {
      aRoute.Append(aRouteToAppend);
      routeAppended = true;
   }
   return routeAppended;
}


// =================================================================================================
// private
double WsfRoadMover::GetTravelTime(double aSimTime, WsfTrack* aTrackPtr, WsfRoute& aRoute)
{
   static const double cInterval    = 20;
   static const double cMinLeadTime = 30;
   static const double cMaxLeadTime = 300;
   // Iterate until travel time within tolerance or delta time increases
   double myTravelTime    = std::numeric_limits<double>::max();
   double timeOfIntercept = -cInterval;
   double deltaTime       = std::numeric_limits<double>::max();
   double startLat, startLon, alt; // alt not used
   GetPlatform()->GetLocationLLA(startLat, startLon, alt);
   int interationCount = 0;
   do
   {
      ++interationCount;
      double previousDeltaTime = deltaTime;
      timeOfIntercept += cInterval;
      double otherLocWCS[3];
      double lat, lon, alt_2;
      double newTravelTime = std::numeric_limits<double>::max();
      aTrackPtr->GetExtrapolatedLocationWCS(timeOfIntercept + aSimTime, otherLocWCS);
      UtEntity::ConvertWCSToLLA(otherLocWCS, lat, lon, alt_2);
      if (CreateRoute(aSimTime, startLat, startLon, lat, lon, aRoute))
      {
         newTravelTime = aRoute.GetTravelTime();
      }
      deltaTime    = newTravelTime - myTravelTime;
      myTravelTime = newTravelTime;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Iteration: " << interationCount;
         out.AddNote() << "Delta Time: " << deltaTime;
         out.AddNote() << "Previous Delta Time: " << previousDeltaTime;
         out.AddNote() << "My Travel Time: " << myTravelTime;
         out.AddNote() << "Time Of Intercept: " << timeOfIntercept;
      }
   } while ((timeOfIntercept < 1200) && (deltaTime < cInterval) &&
            (((timeOfIntercept - myTravelTime) < cMinLeadTime) || ((timeOfIntercept - myTravelTime) > cMaxLeadTime)));

   if ((deltaTime < cInterval) && DebugEnabled())
   {
      auto out = ut::log::debug() << "Calculated route.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Target: " << aTrackPtr->GetTargetName();
      out.AddNote() << "Iterations: " << interationCount;
      out.AddNote() << "Heading: " << aTrackPtr->GetBearing();
      out.AddNote() << "Velocity: " << aTrackPtr->GetSpeed();
   }

   if ((timeOfIntercept >= 1200.) || (deltaTime >= cInterval))
   {
      timeOfIntercept = std::numeric_limits<double>::max();
   }
   return timeOfIntercept;
}

// =================================================================================================
// virtual
WsfMover* WsfRoadMover::Clone() const
{
   return new WsfRoadMover(*this);
}

// =================================================================================================
// private
bool WsfRoadMover::CreateRoute(double aSimTime, const WsfWaypoint& aWp1, const WsfWaypoint& aWp2, WsfRoute& aRoute)
{
   WsfRoute route;
   double   startLat, startLon, startAlt;
   double   endLat, endLon, endAlt;
   aWp1.GetLocationLLA(startLat, startLon, startAlt);
   aWp2.GetLocationLLA(endLat, endLon, endAlt);
   bool success = CreateRoute(aSimTime, startLat, startLon, endLat, endLon, route);
   if (success)
   {
      aRoute.Clear();
      aRoute.Append(aWp1);
      MergeWaypoints(route[0], aRoute[0]);
      for (unsigned int i = 1; i < route.GetSize() - 1; ++i)
      {
         aRoute.Append(route[i]);
         // affect speed and linearAccel if wp1 set
      }
      aRoute.Append(aWp2);
      MergeWaypoints(route[route.GetSize() - 1], aRoute[aRoute.GetSize() - 1]);
   }
   return success;
}

// =================================================================================================
// private
bool WsfRoadMover::CreateRoute(double aSimTime, double aStartLat, double aStartLon, double aEndLat, double aEndLon, WsfRoute& aRoute)
{
   bool foundPath = false;

   // Get a pointer to our current road network.
   WsfRouteNetwork* roadNetworkPtr = WsfRouteNetworkTypes::Get(GetScenario()).Find(mRoadNetworkId);
   if (roadNetworkPtr != nullptr)
   {
      // Calculate the shortest path between our start and end positions.
      if (mUseClosestWaypoint)
      {
         // Starts and ends on waypoints that are on the road network
         foundPath = roadNetworkPtr->GeneratePathBetweenClosestWaypoints(aStartLat, aStartLon, aEndLat, aEndLon, aRoute);
      }
      else
      {
         // Don't include start and ending points; they will be checked and added below
         foundPath =
            roadNetworkPtr->GeneratePathBetweenClosestSegments(aStartLat, aStartLon, aEndLat, aEndLon, aRoute, false);
      }
      double offRoadSpeed = mOffRoadSpeed;
      double linearAccel  = mLinearAccel;
      if (foundPath)
      {
         // Set the speed for the entire route
         for (unsigned int ii = 0; ii < aRoute.GetSize(); ++ii)
         {
            if (ii == 0)
            {
               aRoute[ii].SetPauseTime(mPauseTime);
            }
            aRoute[ii].SetSpeed(mSpeed);
         }

         // Find the ground distance between the starting point and the first point
         // in the calculated path and only consider the mover "off-road" if this distance is
         // greater than one meter. One meter was selected because this is what the WsfRouteNetwork uses for a
         // distance tolerance check. The ending point is checked in the same manner.

         WsfGeoPoint gp1(aStartLat, aStartLon, 0.);
         WsfGeoPoint gp2(aRoute[0].GetLat(), aRoute[0].GetLon(), 0.);
         double      groundRange = gp1.GroundRangeTo(gp2);
         if ((groundRange > 1.) && !mUseClosestWaypoint)
         {
            // Start point is off the route
            WsfWaypoint beginPt;
            beginPt.SetLat(aStartLat);
            beginPt.SetLon(aStartLon);
            aRoute.Insert(0, beginPt);

            // Determine the off-road speed
            double targetSpeed = GetPlatform()->GetSpeed();
            if (GetTarget().mTargetType & WsfPathTarget::cSPEED)
            {
               targetSpeed = GetTarget().mSpeed;
            }

            // If current speed or target waypoint speed
            // is greater than the offroad speed
            if (targetSpeed > mOffRoadSpeed)
            {
               aRoute[0].SetSpeed(targetSpeed);
            }
            else
            {
               aRoute[0].SetSpeed(offRoadSpeed);
               aRoute[1].SetSpeed(offRoadSpeed);
            }
            aRoute[0].SetLinearAccel(linearAccel);
         }

         unsigned int size = aRoute.GetSize();
         gp1.SetLocationLLA(aEndLat, aEndLon, 0.);
         gp2.SetLocationLLA(aRoute[size - 1].GetLat(), aRoute[size - 1].GetLon(), 0.);
         groundRange = gp1.GroundRangeTo(gp2);
         if ((groundRange > 1.) && !mUseClosestWaypoint)
         {
            // End point is off the route
            // so set this speed to the off-road value
            WsfWaypoint endPt;
            endPt.SetLat(aEndLat);
            endPt.SetLon(aEndLon);
            aRoute.Append(endPt);
            size = aRoute.GetSize();

            aRoute[size - 1].SetSpeed(offRoadSpeed);
            aRoute[size - 2].SetLinearAccel(linearAccel);
         }
      }

      // Create a route that is a direct path between the start and end point. No attempt
      // is made to follow the road. (This is like cutting across the back yard instead of
      // taking the street around the block).

      WsfRoute offRoadShortcutRoute;
      CreateRoute(aStartLat, aStartLon, 0.0, aEndLat, aEndLon, 0.0, offRoadShortcutRoute);
      offRoadShortcutRoute[0].SetPauseTime(mPauseTime);
      offRoadShortcutRoute[0].SetSpeed(offRoadSpeed);
      offRoadShortcutRoute[0].SetLinearAccel(linearAccel);
      offRoadShortcutRoute[1].SetSpeed(offRoadSpeed);
      offRoadShortcutRoute[1].SetLinearAccel(linearAccel);

      if (foundPath)
      {
         // If 'allow_off_road_shortcut_route' is true and the off-road shortcut route takes less time, use it.
         if (mConsiderOffRoadShortcut && (aRoute.GetTravelTime() > offRoadShortcutRoute.GetTravelTime()))
         {
            aRoute = offRoadShortcutRoute;
         }
      }
      else
      {
         // No path found... use the direct path.
         aRoute    = offRoadShortcutRoute;
         foundPath = true;
      }
   }
   else if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Road mover unable to find road network.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Mover: " << GetName();
      out.AddNote() << "Road Network: " << mRoadNetworkId.GetString();
   }

   return foundPath;
}

// =================================================================================================
//! Create route between the given locations
bool WsfRoadMover::CreateRoute(double    aFromLatitude,
                               double    aFromLongitude,
                               double    aFromAltitude,
                               double    aToLatitude,
                               double    aToLongitude,
                               double    aToAltitude,
                               WsfRoute& aRoute) const
{
   bool created = false;
   if ((GetRouteComputer() == nullptr) || !GetRouteComputer()->GetEnabled())
   {
      aRoute.Append(WsfWaypoint(aFromLatitude, aFromLongitude, 0.0, 0.0));
      aRoute.Append(WsfWaypoint(aToLatitude, aToLongitude, 0.0, 0.0));
      created = true;
   }
   else
   {
      const WsfGeoPoint startPoint(aFromLatitude, aFromLongitude, aFromAltitude);
      WsfGeoPoint       endPoint(aToLatitude, aToLongitude, aToAltitude);
      double            cost = 0.0;
      created = static_cast<WsfPathFinder*>(GetRouteComputer())->FindPath(startPoint, endPoint, aRoute, cost);
   }
   return created;
}

// =================================================================================================
// private
bool WsfRoadMover::SetOrUpdateRoute(double aSimTime)
{
   const WsfRoute* curRoutePtr = GetRoute();
   unsigned int    routeSize   = curRoutePtr->GetSize();

   // The mover will handle its single position
   if (routeSize <= 1)
   {
      // No start AND end position have been inputed.
      // No intermediate positions have been inputed.
      // Therefore the normal "route" processing will occur, and the platform will sit in one position
      return true;
   }

   unsigned int index = 0;
   WsfWaypoint  wp1   = curRoutePtr->GetWaypointAt(index);
   ++index;
   WsfWaypoint wp2 = curRoutePtr->GetWaypointAt(index);

   WsfRoute route;
   bool     foundPath = CreateRoute(aSimTime, wp1, wp2, route);

   wp1 = wp2;
   // Create the intermediate segment routes and append them together
   while (index < routeSize - 1)
   {
      ++index;
      wp2 = curRoutePtr->GetWaypointAt(index);

      if (!foundPath)
      {
         break;
      }
      foundPath &= AppendRoute(aSimTime, wp1, wp2, route);
      wp1 = wp2;
   }

   if (foundPath)
   {
      return SetRoute(aSimTime, route);
   }

   auto out = ut::log::error() << "Platform could not generate shortest path from initial position to destination.";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Initial Lat: " << wp1.GetLat();
   out.AddNote() << "Initial Lon: " << wp1.GetLon();
   out.AddNote() << "Dest Lat: " << wp2.GetLat();
   out.AddNote() << "Dest Lon: " << wp2.GetLon();

   return false;
}

// =================================================================================================
//! Generate a path between the provided start and end positions and
//! initialize the mover to use it as its waypoint route.
// virtual
bool WsfRoadMover::Initialize(double aSimTime)
{
   bool ok = GetPlatform() != nullptr;

   if (mHaveStartPosition != mHaveEndPosition)
   {
      auto out =
         ut::log::error() << "If either 'start_position' or 'end_position' is specified, then both must be specified.";
      out.AddNote() << "Road Mover: " << GetName();
      out.AddNote() << "Type: " << GetType();
      ok = false;
   }

   if (ok && (mSpeed == 0.0))
   {
      auto out = ut::log::error() << "Road Mover's 'speed' must specified.";
      out.AddNote() << "Road Mover: " << GetName();
      out.AddNote() << "Type: " << GetType();
      ok = false;
   }

   if (ok)
   {
      // Use the on-road speed for the off-road speed if the off-road speed wasn't specified.
      if (mOffRoadSpeed == 0.0)
      {
         mOffRoadSpeed = mSpeed;
      }

      // If the user provides a route, it will be used as follows:
      //
      // 1) If the user provided a start_/end_position, the route will be the concatenation of the successively
      //    generated road-following segments between [start_position, wp1], [wp1, wp2], ... [wpn-1, wpn] and
      //    [wpn, end_position].
      //
      // 2) If the user did not provide a start_/end_position, the route will be the concatenation of the successively
      //    generated road-following segments between [wp1, wp2], ... [wpn-1, wpn].
      if ((GetRoute() != nullptr) && (GetRoute()->GetSize() > 0))
      {
         if (mHaveStartPosition && mHaveEndPosition)
         {
            WsfRoute route;
            route.Append(WsfWaypoint(mStartLat, mStartLon, 0.0, 0.0));
            route.Append(*GetRoute());
            route.Append(WsfWaypoint(mEndLat, mEndLon, 0.0, 0.0));
            ok &= SetRoute(aSimTime, route);
         }
         ok &= SetOrUpdateRoute(aSimTime);
      }
      else if (mHaveStartPosition && mHaveEndPosition)
      {
         // Only the start_position and end_position were provided (no route). Simply generate the route between those points.
         WsfRoute route;
         ok &= CreateRoute(aSimTime, mStartLat, mStartLon, mEndLat, mEndLon, route);
         if (ok)
         {
            ok &= SetRoute(aSimTime, route);
         }
      }
      ok &= WsfGroundMover::Initialize(aSimTime);
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfRoadMover::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "road_network")
   {
      std::string roadNetwork;
      aInput.ReadValue(roadNetwork);
      mRoadNetworkId = roadNetwork;
   }
   else if (command == "consider_off_road_shortcut")
   {
      aInput.ReadValue(mConsiderOffRoadShortcut);
   }
   else if (command == "ignore_off_road_path")
   {
      // Deprecated - retained for backward compatibility
      bool ignoreOffRoadPath;
      aInput.ReadValue(ignoreOffRoadPath);
      mConsiderOffRoadShortcut = (!ignoreOffRoadPath);
   }
   else if (command == "start_position")
   {
      mHaveStartPosition = true;
      aInput.ReadValueOfType(mStartLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mStartLon, UtInput::cLONGITUDE);
   }
   else if (command == "end_position")
   {
      mHaveEndPosition = true;
      aInput.ReadValueOfType(mEndLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mEndLon, UtInput::cLONGITUDE);
   }
   else if (command == "speed")
   {
      double speed;
      aInput.ReadValueOfType(speed, UtInput::cSPEED);
      aInput.ValueGreater(speed, 0.0);
      mSpeed = speed;
   }
   else if (command == "pause_time")
   {
      double startTime;
      aInput.ReadValueOfType(startTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(startTime, 0.0);
      mPauseTime = startTime;
   }
   else if (command == "use_closest_waypoint")
   {
      mUseClosestWaypoint = true;
   }
   else if (command == "off_road_speed")
   {
      double speed;
      aInput.ReadValueOfType(speed, UtInput::cSPEED);
      aInput.ValueGreater(speed, 0.0);
      mOffRoadSpeed = speed;
   }
   else if (command == "linear_acceleration")
   {
      double accel;
      aInput.ReadValueOfType(accel, UtInput::cACCELERATION);
      aInput.ValueGreater(accel, 0.0);
      mLinearAccel = accel;
   }
   else
   {
      myCommand = WsfGroundMover::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
void WsfRoadMover::MergeWaypoints(const WsfWaypoint& aWpFrom, WsfWaypoint& aWpTo)
{
   if (aWpFrom.GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      aWpTo.SetSpeed(aWpFrom.GetSpeed());
   }

   if (aWpFrom.GetLinearAccel() != WsfPath::cDOUBLE_NOT_SET)
   {
      aWpTo.SetLinearAccel(aWpFrom.GetLinearAccel());
   }
}
