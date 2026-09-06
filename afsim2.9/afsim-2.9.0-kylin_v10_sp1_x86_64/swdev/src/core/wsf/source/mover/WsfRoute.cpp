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

#include "WsfRoute.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>

#include "UtAngle.hpp"
#include "UtCast.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputBuffer.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfUtil.hpp"

// =================================================================================================
//! Constructor for a named, but empty, route
//! @param aName A string containing the name of the route.
WsfRoute::WsfRoute(const std::string& aName)
   : WsfObject()
{
   SetName(aName);
}

// =================================================================================================
//! Copy constructor (for Clone())
WsfRoute::WsfRoute(const WsfRoute& aSrc)
   : WsfObject(aSrc)
   , WsfAuxDataEnabled(aSrc)
   , mRouteId(aSrc.mRouteId)
   , mWaypoints(aSrc.mWaypoints)
   , mOffsetRouteValid(aSrc.mOffsetRouteValid)
   , mLabelId(aSrc.mLabelId)
   , mTotalLength(aSrc.mTotalLength)
   , mWaypointReferenceCountPtr(nullptr)
{
}

// =================================================================================================
WsfRoute::~WsfRoute()
{
   ClearWaypointReference();
   UtReferenceTracked::FreeReference();
}

// =================================================================================================
//! Set one route equal to another.
//! @param aRhs The route to which the left-hand-side route is equated.
WsfRoute& WsfRoute::operator=(const WsfRoute& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::        operator=(aRhs);
      WsfAuxDataEnabled::operator=(aRhs);
      mRouteId                   = aRhs.mRouteId;
      mWaypoints                 = aRhs.mWaypoints;
      mOffsetRouteValid          = aRhs.mOffsetRouteValid;
      mLabelId                   = aRhs.mLabelId;
      mTotalLength               = aRhs.mTotalLength;
   }
   return *this;
}

// =================================================================================================
//! Return a reference to the waypoint in the route specified by the given index.
//! @param aIndex The waypoint number to be referenced.
//! @returns A const reference to the waypoint element specified by 'aIndex'
const WsfWaypoint& WsfRoute::operator[](size_t aIndex) const
{
   return GetWaypointAt(aIndex);
}

// =================================================================================================
//! Return a reference to the waypoint in the route specified by the given index.
//! @param aIndex The waypoint number to be referenced.
//! @returns A non-const reference to the waypoint element specified by 'aIndex.'
WsfWaypoint& WsfRoute::operator[](size_t aIndex)
{
   return GetWaypointAt(aIndex);
}

// =================================================================================================
void WsfRoute::Clear()
{
   mWaypoints.clear();
   mTotalLength = 0.0;
   ClearWaypointReference();
}

// =================================================================================================
//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
WsfRoute* WsfRoute::Clone() const
{
   return new WsfRoute(*this);
}

// =================================================================================================
//! Perform initialization of the route.
//! @returns 'true' if the initialization succeeded; 'false' otherwise.
bool WsfRoute::Initialize(double /*aSimTime*/)
{
   SetNetworkRouteId(mRouteId);
   bool ok = (GetNameId() != 0);
   return ok;
}

// =================================================================================================
//! Add the given point to the end of the waypoint vector.
void WsfRoute::Append(const WsfWaypoint& aWaypoint)
{
#if defined(__linux__)
   assert(!std::isnan(aWaypoint.GetLat()));
   assert(!std::isnan(aWaypoint.GetLon()));
#endif
   size_t capacityBefore = mWaypoints.capacity();
   auto   index          = ut::safe_cast<unsigned int, size_t>(mWaypoints.size());
   mWaypoints.push_back(aWaypoint);
   mWaypoints[index].SetPositionInRoute(index);

   // Compute the heading from the next-to-last point to the last point and update the total
   // length of the route.
   if (index == 0)
   {
      mTotalLength = 0.0;
      mWaypoints[index].SetDistanceAlongRoute(0.0);
   }
   else
   {
      auto   beforeIndex = index - 1U;
      auto   afterIndex  = index;
      double distance    = ComputeDistance(beforeIndex, afterIndex);
      if (distance >= 0.0)
      {
         mWaypoints[index].SetDistanceAlongRoute(mWaypoints[beforeIndex].GetDistanceAlongRoute() + distance);
         mTotalLength += distance;
      }
   }

   if (capacityBefore != mWaypoints.size())
   {
      ClearWaypointReference();
   }
}

// =================================================================================================
//! Add all the waypoints of the given route to the end of the current route.
void WsfRoute::Append(const WsfRoute& aRoute)
{
   // skip duplicate ending point
   unsigned int startingPosition = 0U;
   if ((!aRoute.mWaypoints.empty()) && (!mWaypoints.empty()) && (aRoute[0] == mWaypoints[mWaypoints.size() - 1U]))
   {
      startingPosition = 1U;
   }

   for (auto index = startingPosition; index < aRoute.GetSize(); ++index)
   {
      Append(aRoute[index]);
   }
}

// =================================================================================================
void WsfRoute::Insert(int aIndex, const WsfWaypoint& aWaypoint)
{
   if (aIndex >= 0 && aIndex <= (int)mWaypoints.size())
   {
      Insert(mWaypoints.begin() + aIndex, aWaypoint);
   }
}

// =================================================================================================
void WsfRoute::Insert(WayptVecIter aIter, const WsfWaypoint& aWaypoint)
{
#if defined(__linux__)
   assert(!std::isnan(aWaypoint.GetLat()));
   assert(!std::isnan(aWaypoint.GetLon()));
#endif
   auto insertOffset = ut::safe_cast<unsigned int, ptrdiff_t>(aIter - mWaypoints.begin());
   if (insertOffset == mWaypoints.size())
   {
      Append(aWaypoint);
      return;
   }

   mWaypoints.insert(aIter, aWaypoint);
   WsfWaypoint& newWaypt = mWaypoints[insertOffset];
   newWaypt.SetDistanceAlongRoute(0.0);
   newWaypt.SetPositionInRoute(insertOffset);
   newWaypt.SetRouteId(mRouteId);

   // Cast is required for safety in avoiding wrapping of max value insertOffset when
   // incremented by one
   for (auto i = static_cast<size_t>(insertOffset) + 1U; i < mWaypoints.size(); ++i)
   {
      mWaypoints[i].SetPositionInRoute(ut::safe_cast<unsigned int, size_t>(i));
   }

   double additionalLength = 0.0;
   auto   beforeIndex      = insertOffset;
   auto   atIndex          = beforeIndex;
   bool   validBeforeIndex = false;
   if (beforeIndex != 0U)
   {
      beforeIndex--;
      validBeforeIndex = true;
   }

   // Compute new geometry before the point
   if (validBeforeIndex)
   {
      double distance = ComputeDistance(beforeIndex, atIndex);
      if (distance >= 0.0)
      {
         additionalLength = distance;
         newWaypt.SetDistanceAlongRoute(mWaypoints[beforeIndex].GetDistanceAlongRoute() + distance);
      }
   }

   auto afterIndex = atIndex + 1U;

   if ((atIndex == insertOffset) && (static_cast<size_t>(atIndex) + 1U < mWaypoints.size()))
   {
      double distance = ComputeDistance(atIndex, afterIndex);
      if ((static_cast<size_t>(afterIndex) < mWaypoints.size()) && (distance >= 0.0))
      {
         double distancePrior = mWaypoints[afterIndex].GetDistanceAlongRoute();
         double newDistance   = newWaypt.GetDistanceAlongRoute() + distance;
         mWaypoints[afterIndex].SetDistanceAlongRoute(newDistance);
         additionalLength = newDistance - distancePrior;
      }
   }
   for (auto i = static_cast<size_t>(afterIndex) + 1U; i < mWaypoints.size(); ++i)
   {
      double dist = mWaypoints[i].GetDistanceAlongRoute() + additionalLength;
      mWaypoints[i].SetDistanceAlongRoute(dist);
   }
   mTotalLength = mWaypoints.back().GetDistanceAlongRoute();

   ClearWaypointReference();
}

// =================================================================================================
void WsfRoute::Remove(unsigned int aWaypointIndex)
{
   if (aWaypointIndex >= 0U && aWaypointIndex < ut::safe_cast<unsigned int, size_t>(mWaypoints.size()))
   {
      mWaypoints.erase(mWaypoints.begin() + aWaypointIndex);
      for (auto i = aWaypointIndex; i < ut::safe_cast<unsigned int, size_t>(mWaypoints.size()); ++i)
      {
         mWaypoints[i].SetPositionInRoute(i);
      }

      auto beforeIndex = aWaypointIndex - 1U;
      auto afterIndex  = aWaypointIndex;
      auto distance    = ComputeDistance(beforeIndex, afterIndex);

      // Update any non-location points' distance along route
      auto beforeDistance = (aWaypointIndex != 0) ? mWaypoints[beforeIndex].GetDistanceAlongRoute() : 0.0;
      for (auto i = aWaypointIndex; i < afterIndex; ++i)
      {
         mWaypoints[i].SetDistanceAlongRoute(beforeDistance);
      }
      auto removedDistance = 0.0;
      if (afterIndex < ut::safe_cast<unsigned int, size_t>(mWaypoints.size()))
      {
         if (distance < 0.0)
         {
            distance = 0.0;
         }
         auto newDistance = beforeDistance + distance;
         removedDistance  = mWaypoints[afterIndex].GetDistanceAlongRoute() - newDistance;
         mWaypoints[afterIndex].SetDistanceAlongRoute(newDistance);
      }
      for (size_t i = static_cast<size_t>(afterIndex) + 1U; i < mWaypoints.size(); ++i)
      {
         mWaypoints[i].SetDistanceAlongRoute(mWaypoints[i].GetDistanceAlongRoute() - removedDistance);
      }
   }
   if (!mWaypoints.empty())
   {
      mTotalLength = mWaypoints.back().GetDistanceAlongRoute();
   }
   ClearWaypointReference();
}

// =================================================================================================
//! Given a 'start' waypoint and an 'end' waypoint, both of which are contained within
//! the referenced route, return the sub-route that begins and ends with those waypoints.
//! @param aFromWaypoint The first waypoint in the sub-route.
//! @param aToWaypoint The last waypoint in the sub-route.
//! @param aRoute [output] The desired sub-route.
void WsfRoute::GetSubroute(const WsfWaypoint& aFromWaypoint, const WsfWaypoint& aToWaypoint, WsfRoute& aRoute) const
{
   int indexOfFirst      = 0;
   int indexOfLast       = 0;
   int numberOfWaypoints = static_cast<int>(mWaypoints.size());

   bool firstFound = false;
   bool lastFound  = false;
   for (int ptIndex = 0; ptIndex < numberOfWaypoints; ++ptIndex)
   {
      if (mWaypoints[ptIndex] == aFromWaypoint)
      {
         indexOfFirst = ptIndex;
         firstFound   = true;
      }

      if (mWaypoints[ptIndex] == aToWaypoint)
      {
         indexOfLast = ptIndex;
         lastFound   = true;
      }

      if (firstFound && lastFound)
      {
         break;
      }
   }

   GetSubroute(indexOfFirst, indexOfLast, aRoute);
}

// =================================================================================================
//! Given a 'start' route index and an 'end' route index, both of which represent waypoints that
//! are contained within the referenced route, return the sub-route that begins and ends
//! with those waypoints.
//! @param aStartIndex The first waypoint index in the sub-route.
//! @param aEndIndex   The last waypoint index in the sub-route.
//! @param aRoute [output] The desired sub-route.
void WsfRoute::GetSubroute(int aStartIndex, int aEndIndex, WsfRoute& aRoute) const
{
   aRoute.Clear();
   AppendSubroute(aStartIndex, aEndIndex, aRoute);
}

// =================================================================================================
//! Given a 'start' route index and an 'end' route index, both of which represent waypoints that
//! are contained within the referenced route, append the sub-route that begins and ends
//! with those waypoints.
//! @param aStartIndex The first waypoint index in the sub-route.
//! @param aEndIndex   The last waypoint index in the sub-route.
//! @param aRoute      The route to be appended with the sub-route.
void WsfRoute::AppendSubroute(int aStartIndex, int aEndIndex, WsfRoute& aRoute) const
{
   if (aStartIndex < aEndIndex)
   {
      aStartIndex = std::max(aStartIndex, 0);
      aEndIndex   = std::min(aEndIndex, int(mWaypoints.size()) - 1);
      for (int ptIndex = aStartIndex; ptIndex <= aEndIndex; ++ptIndex)
      {
         aRoute.Append(mWaypoints[ptIndex]);
      }
   }
   else
   {
      aEndIndex   = std::max(aEndIndex, 0);
      aStartIndex = std::min(aStartIndex, int(mWaypoints.size()) - 1);
      for (int ptIndex = aStartIndex; ptIndex >= aEndIndex; --ptIndex)
      {
         aRoute.Append(mWaypoints[ptIndex]);
      }
   }
}

// =================================================================================================
//! Loop through the waypoints and return a vector containing
//! only those points whose intersection identifier is non-zero.
//! @param aWaypointPtrVector [output] A vector of waypoint pointers whose
//! intersection identifier is non-zero.
//! @note In the context of a route a "node" is defined to be either the first or last waypoint in
//! the route or a waypoint where two or more routes intersect. This method returns all waypoints
//! that are nodes.
void WsfRoute::GetAllNodes(WaypointPtrVector& aWaypointPtrVector)
{
   aWaypointPtrVector.clear();
   for (auto& wp : mWaypoints)
   {
      if (!wp.GetNodeId().IsNull())
      {
         aWaypointPtrVector.push_back(&wp);
      }
   }
}

// =================================================================================================
int WsfRoute::FindWaypointIndex(const WsfWaypoint& aWpt)
{
   if ((aWpt.GetPositionInRoute() < mWaypoints.size()) && (aWpt == mWaypoints[aWpt.GetPositionInRoute()]))
   {
      return aWpt.GetPositionInRoute();
   }
   return FindWaypointIndex(aWpt.GetLat(), aWpt.GetLon(), aWpt.GetAlt());
}

// =================================================================================================
int WsfRoute::FindWaypointIndex(double aLat, double aLon, double aAlt)
{
   int index = -1;
   for (size_t i = 0; i < mWaypoints.size(); ++i)
   {
      WsfWaypoint& wpt = mWaypoints[i];
      if ((wpt.GetLat() == aLat) && (wpt.GetLon() == aLon) && (wpt.GetAlt() == aAlt))
      {
         index = (int)i;
         break;
      }
   }
   return index;
}

// =================================================================================================
//! Determine which nodes are adjacent to a node given its index.
//! @param aWaypointIndex The waypoint index around which to find adjacent nodes.
//! @param aAdjacentNodeIndices [output] The (0, 1, or 2) adjacent node indices in the route.
//! @returns The number of adjacent nodes (0, 1, or 2)
int WsfRoute::GetAdjacentNodeIndices(int aWaypointIndex, int* aAdjacentNodeIndices) const
{
   int adjacentCount = 0;

   // look at points before this point
   for (int index = aWaypointIndex; index >= 0; --index)
   {
      if (!mWaypoints[index].GetNodeId().IsNull())
      {
         aAdjacentNodeIndices[adjacentCount] = index;
         ++adjacentCount;
         break;
      }
   }

   if (mWaypoints[aWaypointIndex].GetNodeId().IsNull())
   {
      int numberOfWaypoints = static_cast<int>(mWaypoints.size());
      // look at points after this point
      for (int index = aWaypointIndex + 1; index < numberOfWaypoints; ++index)
      {
         if (!mWaypoints[index].GetNodeId().IsNull())
         {
            aAdjacentNodeIndices[adjacentCount] = index;
            ++adjacentCount;
            break;
         }
      }
   }
   return adjacentCount;
}

// =================================================================================================
//! Determine if a waypoint is between two other waypoints.
//! @param aPointLL              The point of interest along the route.
//! @param aFirstWaypointIndex   The first route waypoint index.
//! @param aSecondWaypointIndex  The second route waypoint index.
//! @param aTolerancePercentage  The percentage of firstSecondDistance that
//!                                   aPointLL can be off the path and still
//!                                   be considered to be "between"
//! @returns 'true' if the given point is between the two waypoints; 'false' otherwise.
bool WsfRoute::IsBetweenWaypoints(const double       aPointLL[2],
                                  const unsigned int aFirstWaypointIndex,
                                  const unsigned int aSecondWaypointIndex,
                                  const double       aTolerancePercentage) const
{
   double headingDeg;
   double firstSecondDistance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[aFirstWaypointIndex].GetLat(),
                                                   mWaypoints[aFirstWaypointIndex].GetLon(),
                                                   mWaypoints[aSecondWaypointIndex].GetLat(),
                                                   mWaypoints[aSecondWaypointIndex].GetLon(),
                                                   headingDeg,
                                                   firstSecondDistance);
   double firstWpDistance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[aFirstWaypointIndex].GetLat(),
                                                   mWaypoints[aFirstWaypointIndex].GetLon(),
                                                   aPointLL[0],
                                                   aPointLL[1],
                                                   headingDeg,
                                                   firstWpDistance);
   double wpSecondDistance;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aPointLL[0],
                                                   aPointLL[1],
                                                   mWaypoints[aSecondWaypointIndex].GetLat(),
                                                   mWaypoints[aSecondWaypointIndex].GetLon(),
                                                   headingDeg,
                                                   wpSecondDistance);

   // if (firstWpDistance == 0 || wpSecondDistance == 0)
   //{
   //    return true;
   // }
   // else
   //{
   //    double diff = fabs(firstWpDistance + wpSecondDistance - firstSecondDistance);

   //   if (diff < aTolerancePercentage * firstSecondDistance)
   //   {
   //      return true;
   //   }
   //   else
   //   {
   //      return false;
   //   }
   //   /*
   //         double s = (firstSecondDistance + firstWpDistance + wpSecondDistance) / 2;
   //         double temp = s * (s - firstSecondDistance) *
   //                        (s - firstWpDistance) * (s - wpSecondDistance);
   //         if (temp < 0)
   //         {
   //            return true;
   //         }
   //         else
   //         {
   //            double h = 2 / firstSecondDistance * sqrt(temp);
   //            if (h < 5)
   //            {
   //               return true;
   //            }
   //            else
   //            {
   //               return false;
   //            }
   //         }
   //   */
   //}

   return ((firstWpDistance == 0) || (wpSecondDistance == 0)) ||
          (fabs(firstWpDistance + wpSecondDistance - firstSecondDistance) < (aTolerancePercentage * firstSecondDistance));
}

// =================================================================================================
//! Get the time to travel the route given the speed of each segment
double WsfRoute::GetTravelTime() const
{
   double time  = 0.0;
   double speed = 0.0;
   for (unsigned int index = 0; index < mWaypoints.size() - 1.; ++index)
   {
      double tempHeading;
      double segmentDistance;
      mWaypoints[index].GreatCircleHeadingAndDistance(mWaypoints[index + 1], tempHeading, segmentDistance);
      if (mWaypoints[index + 1].GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
      {
         speed = mWaypoints[index + 1].GetSpeed();
      }

      if (speed <= 0)
      {
         return std::numeric_limits<double>::max();
      }
      time += (segmentDistance / speed);
   }
   return time;
}
double WsfRoute::GetTravelTimeToPosition(const double aPosition[2]) const
{
   double time  = 0.0;
   double speed = 0.0;
   // Find the closest waypoint to the provided point but not
   // behind the position along the route.
   unsigned int closestIndex   = 0;
   double       distance       = std::numeric_limits<double>::max();
   double       currentHeading = std::numeric_limits<double>::max();
   for (unsigned int index = 0; index < mWaypoints.size() - 1; ++index)
   {
      double waypointLat, waypointLon, waypointAlt;
      double heading, waypointDistance;
      mWaypoints[index].GetLocationLLA(waypointLat, waypointLon, waypointAlt);
      UtSphericalEarth::GreatCircleHeadingAndDistance(aPosition[0], aPosition[1], waypointLat, waypointLon, heading, waypointDistance);
      // record the direction of travel and distance from the first waypoint
      if (currentHeading > 360.)
      {
         currentHeading = heading;
         distance       = waypointDistance;
      }

      // Only allow the closest waypoint to be behind the current waypoint
      if (waypointDistance < distance && fabs(heading - currentHeading) > 90.)
      {
         closestIndex = index;
         distance     = waypointDistance;
      }
   }

   // Calculate travel time to the waypoint closest to the specified position
   for (unsigned int index = 0; index < closestIndex && index < mWaypoints.size(); ++index)
   {
      double tempHeading;
      double segmentDistance;
      mWaypoints[index].GreatCircleHeadingAndDistance(mWaypoints[index + 1], tempHeading, segmentDistance);
      if (mWaypoints[index + 1].GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
      {
         speed = mWaypoints[index + 1].GetSpeed();
      }

      if (speed <= 0)
      {
         return std::numeric_limits<double>::max();
      }
      time += (segmentDistance / speed);
   }

   // Calculate the travel time from the closest waypoint to the specified location using speed info from the closest waypoint
   double tempHeading;
   double waypointLat, waypointLon, waypointAlt;
   mWaypoints[closestIndex].GetLocationLLA(waypointLat, waypointLon, waypointAlt);
   UtSphericalEarth::GreatCircleHeadingAndDistance(aPosition[0], aPosition[1], waypointLat, waypointLon, tempHeading, distance);
   speed = 0;
   if (mWaypoints[closestIndex].GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
   {
      speed = mWaypoints[closestIndex].GetSpeed();
   }

   if (speed <= 0)
   {
      time = std::numeric_limits<double>::max();
   }
   else
   {
      time += (distance / speed);
   }

   return time;
}

// =================================================================================================
//! Get the distance along the route between two waypoints.
//! @param aFromWaypointIndex The beginning waypoint index for which the distance is calculated.
//! @param aToWaypointIndex The ending waypoint index for which the distance is calculated.
//! @returns The distance, along a great circle route, between 'from' and 'to' waypoints
double WsfRoute::GetDistance(int aFromWaypointIndex, int aToWaypointIndex) const
{
   double distance = 0.0;
   if ((aFromWaypointIndex < (int)mWaypoints.size()) && (aToWaypointIndex < (int)mWaypoints.size()))
   {
      distance = fabs(mWaypoints[aToWaypointIndex].GetDistanceAlongRoute() -
                      mWaypoints[aFromWaypointIndex].GetDistanceAlongRoute());
   }
   return distance;
}

// =================================================================================================
//! Get the distance and offset along the route between two waypoints.
//! @param aFromLL The beginning point for which the distance is calculated.
//! @param aToLL The ending point for which the distance is calculated.
//! @param aPointLL The point for which the distances are calculated.
//! @param aOffsetDist the dist (aLat, aLon) is off the route between the waypoints
//! @returns The distance, along a great circle route, between the 'from' and 'to' input points.
double WsfRoute::GetDistanceOffset(const double aFromLL[2],
                                   const double aToLL[2],
                                   const double aPointLL[2],
                                   double&      aOffsetDist) const
{
   double headingFromTo;
   double distanceFromTo;
   double headingFromPt;
   double distanceFromPt;
   double headingToPt;
   double distanceToPt;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aFromLL[0], aFromLL[1], aPointLL[0], aPointLL[1], headingFromTo, distanceFromTo);
   UtSphericalEarth::GreatCircleHeadingAndDistance(aFromLL[0], aFromLL[1], aPointLL[0], aPointLL[1], headingFromPt, distanceFromPt);
   UtSphericalEarth::GreatCircleHeadingAndDistance(aToLL[0], aToLL[1], aPointLL[0], aPointLL[1], headingToPt, distanceToPt);

   // Convert heading degrees to radians
   headingFromTo /= UtMath::cDEG_PER_RAD;
   headingFromPt /= UtMath::cDEG_PER_RAD;
   // headingToPt /= UtMath::cDEG_PER_RAD;

   // Calculate needed angles
   double angleToFromPt = fabs(headingFromTo - headingFromPt);
   // double angelFromToPt = UtMath::cPI_OVER_2 - fabs(headingFromTo - headingToPt);

   // Use Euclidean geometry as a close approximation
   // specifically Heron's Formula
   double distance;
   double s;
   double area;
   s           = (distanceFromTo + distanceFromPt + distanceToPt) / 2.;
   area        = sqrt(s * (s - distanceFromTo) * (s - distanceFromPt) * (s - distanceToPt));
   aOffsetDist = 2 / distanceFromTo * area;
   distance    = sqrt(pow(distanceFromPt, 2) - pow(aOffsetDist, 2));
   if (angleToFromPt > (UtMath::cPI / 2.0) && angleToFromPt < (3.0 * UtMath::cPI / 2.0))
   {
      // (lat, lon) behind
      distance *= -1;
   }

   double signedAngle = headingFromPt - headingFromTo;

   // Since we're using a signed angle with 0 being no difference, we need to
   // compensate if the magnitude of the difference is more than PI/2.
   if (signedAngle < -UtMath::cPI)
   {
      signedAngle += UtMath::cPI * 2.0;
   }

   if (signedAngle > UtMath::cPI)
   {
      signedAngle -= UtMath::cPI * 2.0;
   }

   if (signedAngle < 0 || signedAngle > UtMath::cPI)
   {
      aOffsetDist *= -1;
   }

   return distance;
}

// =================================================================================================
//! Get the distance along the route between ANY two waypoints along the route.
//! @param aFromLL The beginning point for which the distance is calculated.
//! @param aToLL The ending point for which the distance is calculated.
//! @returns The distance, along a great circle route, between the 'from' and 'to' input waypoints.
//!      negative - 'to' appears in route prior to 'from'
//!      positive - 'from' appears in route prior to 'to'
//!      std::numeric_limits<int>::max() if either 'from' or 'to' are not along the route.
//!
//! @note This method differs from GetDistance in that the 'from' and 'to' waypoints are NOT required
//! to be a waypoints of the route.
double WsfRoute::GetDistanceAlongRoute(const double aFromLL[2], const double aToLL[2]) const
{
   const double* fromToWaypoints[2]       = {aFromLL, aToLL};
   int           fromToClosestWaypoint[2] = {-1, -1};
   int           fromToIndex = 0; // Required to define this variable here for compatibility with Linux and VS6.
   for (fromToIndex = 0; fromToIndex < 2; ++fromToIndex)
   {
      for (unsigned int waypointIndex = 0; waypointIndex < mWaypoints.size() - 1; ++waypointIndex)
      {
         if (IsBetweenWaypoints(fromToWaypoints[fromToIndex], waypointIndex, waypointIndex + 1))
         {
            fromToClosestWaypoint[fromToIndex] = waypointIndex;
            break;
         }
      }
   }
   for (fromToIndex = 0; fromToIndex < 2; ++fromToIndex)
   {
      if (fromToClosestWaypoint[fromToIndex] == -1)
      {
         return std::numeric_limits<int>::max();
      }
   }
   double distance = 0;
   double heading  = 0;
   if (fromToClosestWaypoint[0] == fromToClosestWaypoint[1])
   {
      // 'to' and 'from' between the same waypoints
      UtSphericalEarth::GreatCircleHeadingAndDistance(fromToWaypoints[0][0],
                                                      fromToWaypoints[0][1],
                                                      fromToWaypoints[1][0],
                                                      fromToWaypoints[1][1],
                                                      heading,
                                                      distance);
      double segmentDistance = 0;
      double segmentHeading  = 0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[fromToClosestWaypoint[0]].GetLat(),
                                                      mWaypoints[fromToClosestWaypoint[0]].GetLon(),
                                                      mWaypoints[fromToClosestWaypoint[0] + 1].GetLat(),
                                                      mWaypoints[fromToClosestWaypoint[0] + 1].GetLon(),
                                                      segmentHeading,
                                                      segmentDistance);
      if (fabs(heading - segmentHeading) > .1)
      {
         // 'to' is behind 'from'
         distance *= -1;
      }
   }
   else
   {
      if (fromToClosestWaypoint[0] < fromToClosestWaypoint[1])
      {
         // 'from' appears prior to 'to' in route
         fromToClosestWaypoint[0] += 1;
      }
      else
      {
         // 'to' appears prior to 'from' in route
         fromToClosestWaypoint[1] += 1;
      }
      for (fromToIndex = 0; fromToIndex < 2; ++fromToIndex)
      {
         double startEndDistance = 0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(fromToWaypoints[fromToIndex][0],
                                                         fromToWaypoints[fromToIndex][1],
                                                         mWaypoints[fromToClosestWaypoint[fromToIndex]].GetLat(),
                                                         mWaypoints[fromToClosestWaypoint[fromToIndex]].GetLon(),
                                                         heading,
                                                         startEndDistance);
         distance += startEndDistance;
      }
      distance += GetDistance(fromToClosestWaypoint[0], fromToClosestWaypoint[1]);
   }
   return distance;
}

// =================================================================================================
//! Get the distance along the route between ANY waypoint on the route and
//! an arbitrary (lat, lon) along with the perpendicular offset distance of
//! (lat, lon) to the route.
//! @param aFromLL The beginning point for which the distance is calculated.
//! @param aToLL   The ending point for which the distances are calculated.
//! @param aOffsetDist [output] The perpendicular distance from the specified point to the point on the route.
//! @returns The distance, along a great circle route, between the 'from' and 'to' input waypoints.
//!          negative - 'to' appears in route prior to 'from'
//!          positive - 'from' appears in route prior to 'to'
//! @note This method returns the distance to the first segment of route ahead of aFromLL which aToLL's
//! perpendicular intercepts. Caution: there maybe smaller offsets distance further along route.
double WsfRoute::GetDistanceAlongRoute(const double aFromLL[2], const double aToLL[2], double& aOffsetDist) const

{
   int fromIndex = -1;
   int toIndex   = -1;
   for (unsigned int waypointIndex = 0; waypointIndex < mWaypoints.size() - 1; ++waypointIndex)
   {
      if (IsBetweenWaypoints(aFromLL, waypointIndex, waypointIndex + 1))
      {
         fromIndex = waypointIndex;
         break;
      }
   }
   for (unsigned int waypointIndex = 0; waypointIndex < mWaypoints.size() - 1; ++waypointIndex)
   {
      double fromLL[2] = {mWaypoints[waypointIndex].GetLat(), mWaypoints[waypointIndex].GetLon()};
      double toLL[2]   = {mWaypoints[waypointIndex + 1].GetLat(), mWaypoints[waypointIndex + 1].GetLon()};
      double distance  = GetDistanceOffset(fromLL, toLL, aToLL, aOffsetDist);
      double segmentDistance;
      double segmentHeading;
      mWaypoints[waypointIndex].GreatCircleHeadingAndDistance(mWaypoints[waypointIndex + 1], segmentHeading, segmentDistance);
      if (distance > 0 && distance < segmentDistance)
      {
         toIndex = waypointIndex;
         break;
      }
   }
   if (fromIndex == -1 || toIndex == -1)
   {
      return std::numeric_limits<int>::max();
   }
   double distance = 0;
   if (fromIndex == toIndex)
   {
      // 'to' and 'from' between the same waypoints
      double wptLL[2] = {mWaypoints[fromIndex + 1].GetLat(), mWaypoints[fromIndex + 1].GetLon()};
      distance        = GetDistanceOffset(aFromLL, wptLL, aToLL, aOffsetDist);
   }
   else
   {
      if (fromIndex < toIndex)
      {
         // 'from' appears prior to 'to' in route
         fromIndex += 1;
      }
      else
      {
         // 'to' appears prior to 'from' in route
         toIndex += 1;
      }
      double tempDist = 0;
      double heading  = 0;
      UtSphericalEarth::GreatCircleHeadingAndDistance(aFromLL[0],
                                                      aFromLL[1],
                                                      mWaypoints[fromIndex].GetLat(),
                                                      mWaypoints[fromIndex].GetLon(),
                                                      heading,
                                                      distance);

      /* Get the great circle distance between aLat/aLon and its closest waypoint. */
      UtSphericalEarth::GreatCircleHeadingAndDistance(aToLL[0],
                                                      aToLL[1],
                                                      mWaypoints[toIndex].GetLat(),
                                                      mWaypoints[toIndex].GetLon(),
                                                      heading,
                                                      tempDist);

      distance += tempDist;
      distance += GetDistance(fromIndex, toIndex);
   }
   return distance;
}

// =================================================================================================
namespace
{
// Private function for using std::lower_bound
inline double WaypointCompareDistance(const WsfWaypoint& aA, const WsfWaypoint& aB)
{
   return aA.GetDistanceAlongRoute() < aB.GetDistanceAlongRoute();
}
} // namespace

// =================================================================================================
//! @param aDistance      A distance along the route.
//! @param aSegmentIndex  Set to the index of the waypoint which comes before the specified distance.
//!                       aSegmentIndex will never be the last waypoint.
bool WsfRoute::GetEndPointsAtDistance(double aDistance, int& aSegmentIndex) const
{
   if (mWaypoints.size() < 2)
   {
      return false;
   }

   bool        found = false;
   WsfWaypoint tmp;
   tmp.SetDistanceAlongRoute(aDistance);
   auto iter = std::lower_bound(mWaypoints.begin(), mWaypoints.end(), tmp, &WaypointCompareDistance);
   // If aDistance is equal to mTotalLength, allow it.
   if (iter == mWaypoints.end() && aDistance <= mTotalLength)
   {
      iter = mWaypoints.end() - 1;
   }

   if (iter != mWaypoints.end())
   {
      found = true;
      if (iter == mWaypoints.begin())
      {
         aSegmentIndex = 0;
      }
      else
      {
         aSegmentIndex = static_cast<int>(iter - mWaypoints.begin());
         --aSegmentIndex;
      }
   }
   return found;
}

// =================================================================================================
//! Projects the given point onto the provided segment.
//!
//! @param aPointLL The point to project.
//! @param aSegmentStartLL The segment start point
//! @param aSegmentEndLL   The segment end point
//! @param aProjectedLL    Set to the location of the point projected onto the segment
//! @return Returns true if the projection was possible.
// static
bool WsfRoute::ProjectOntoRoute(const double aPointLL[2],
                                const double aSegmentStartLL[2],
                                const double aSegmentEndLL[2],
                                double       aProjectedLL[2])
{
   bool projected = false;

   // Convert the provided location's lat/lon into geocentric coordinates.
   UtVec3d pt;
   UtSphericalEarth::ConvertLLAToECEF(aPointLL[0], aPointLL[1], 0.0, pt.GetData());

   // Convert the lat/lon of the first waypoint in the segment.
   UtVec3d ptCurr;
   UtSphericalEarth::ConvertLLAToECEF(aSegmentStartLL[0], aSegmentStartLL[1], 0.0, ptCurr.GetData());

   // Calculate the vector from the first waypoint in the segment to the provided location.
   UtVec3d vec(pt - ptCurr);

   // Convert the lat/lon of the second waypoint in the segment.
   UtVec3d ptNext;
   UtSphericalEarth::ConvertLLAToECEF(aSegmentEndLL[0], aSegmentEndLL[1], 0.0, ptNext.GetData());

   // Calculate the vector between the first and second waypoint in the segment.
   UtVec3d vecNext(ptNext - ptCurr);

   // If vecNext points in the same direction of vec, it will project onto the segment.
   double dot = vec.DotProduct(vecNext);
   if (dot >= 0)
   {
      double cosTheta = dot / vecNext.Magnitude();

      UtVec3d vecProj = vecNext;
      vecProj.Normalize();
      vecProj *= cosTheta;
      UtVec3d ptProj(ptCurr + vecProj);

      // Convert back to LLA
      double alt;
      UtSphericalEarth::ConvertECEFToLLA(ptProj.GetData(), aProjectedLL[0], aProjectedLL[1], alt);
      projected = true;
   }
   return projected;
}

// =================================================================================================
//! Projects the given point onto the closest segment in the route.
//!
//! @param aPointLL The point to project.
//! @param aSegmentIndex Set to the segment index used to project
//! @param aProjectedLL  Set to the location of the point projected onto the segment
//! @param aDistance     Set to the distance from the segment
//! @return Returns true if the projection was possible.
// static
bool WsfRoute::ProjectOntoRoute(const double aPointLL[2], int& aSegmentIndex, double aProjectedLL[2], double& aDistance) const
{
   bool projected = false;
   aDistance      = std::numeric_limits<double>::max();
   for (unsigned int index = 1; index < mWaypoints.size(); ++index)
   {
      const WsfWaypoint& start = mWaypoints[index - 1];
      UtVec2d            startLL(start.GetLat(), start.GetLon());
      const WsfWaypoint& end = mWaypoints[index];
      UtVec2d            endLL(end.GetLat(), end.GetLon());
      UtVec2d            projectedLL;
      if (ProjectOntoRoute(aPointLL, startLL.GetData(), endLL.GetData(), projectedLL.GetData()))
      {
         if (IsBetweenWaypoints(projectedLL.GetData(), index - 1, index))
         {
            double distance;
            double heading;
            UtSphericalEarth::GreatCircleHeadingAndDistance(aPointLL[0],
                                                            aPointLL[1],
                                                            projectedLL[0],
                                                            projectedLL[1],
                                                            heading,
                                                            distance);
            if (distance < aDistance)
            {
               aDistance = distance;
               projectedLL.Get(aProjectedLL);
               aSegmentIndex = index - 1;
               projected     = true;
            }
         }
      }
   }
   return projected;
}

// =================================================================================================
//! Projects a point onto the closest segment in the route.
//! @param aPointLL The point to project Latitude/Longitude
//! @param aDistanceFromRoute   Set to the distance the given point is from the projected point.
//! @param aDistanceAlongRoute  Set to the distance along the route at which the point projected point occurs
//! returns True if successful
bool WsfRoute::ProjectOntoRoute(const double aPointLL[2], double& aDistanceFromRoute, double& aDistanceAlongRoute) const
{
   int    segmentIndex;
   double projectedLL[2];
   bool   projected = ProjectOntoRoute(aPointLL, segmentIndex, projectedLL, aDistanceFromRoute);
   if (projected)
   {
      double heading;
      double distance;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[segmentIndex].GetLat(),
                                                      mWaypoints[segmentIndex].GetLon(),
                                                      projectedLL[0],
                                                      projectedLL[1],
                                                      heading,
                                                      distance);
      aDistanceAlongRoute = mWaypoints[segmentIndex].GetDistanceAlongRoute() + distance;
   }
   return projected;
}

// =================================================================================================
//! Projects a point onto the closest segment in the route.
//! @param aPointLL The point to project Latitude/Longitude
//! @param aDistanceFromRoute   Set to the distance the given point is from the projected point.
//! @param aDistanceAlongRoute  Set to the distance along the route at which the point projected point occurs
//! returns Index of closest segment, or -1 if failure
int WsfRoute::ProjectOntoRouteFast(const double aPointLL[2], double& aDistanceFromRoute, double& aDistanceAlongRoute) const
{
   if (mWaypoints.size() < 2)
   {
      return -1;
   }

   // This starts with a rough computation based on lat/lon differences
   // We assume that all points in the route are lat/lon points.
   double  bestDistance2   = std::numeric_limits<double>::max();
   int     bestSegment     = 0;
   int     bestSegmentType = 0;
   UtVec2d pt(aPointLL);
   for (size_t i = 1; i < mWaypoints.size(); ++i)
   {
      const WsfWaypoint& a = mWaypoints[i - 1];
      UtVec2d            A(a.GetLat(), a.GetLon());
      const WsfWaypoint& b = mWaypoints[i];
      UtVec2d            B(b.GetLat(), b.GetLon());

      // if a change of more than 180 degrees is detected,
      // take the small angle instead
      if (fabs(B[1] - A[1]) > 180.0)
      {
         if (B[1] - A[1] > 180.0)
         {
            B[1] -= 360.0;
         }
         else
         {
            B[1] += 360.0;
         }
      }

      UtVec2d direction(B - A);
      UtVec2d fromA(pt - A);
      double  dot = direction.DotProduct(fromA);
      double  dist2;
      int     segType;
      if (dot < 0)
      {
         // point is behind segment
         // closest point is straight line to A
         dist2   = fromA.MagnitudeSquared();
         segType = 0;
      }
      else
      {
         UtVec2d toB(B - pt);
         double  dot2 = direction.DotProduct(toB);
         if (dot2 < 0)
         {
            // point is in front of segment, closest point is straight line to B
            dist2   = toB.MagnitudeSquared();
            segType = 1;
         }
         else
         {
            // point is in-between segment
            double dMag = direction.Magnitude();
            if (dMag > 0)
            {
               direction /= dMag;
               dist2 = direction.CrossProduct(fromA);
               dist2 *= dist2;
               segType = 2;
            }
            else
            {
               // Waypoints are on top of each other, revert to distance from waypoint.
               dist2   = toB.MagnitudeSquared();
               segType = 1;
            }
            direction.Normalize();
         }
      }
      if (dist2 < bestDistance2)
      {
         bestDistance2   = dist2;
         bestSegment     = static_cast<int>(i) - 1;
         bestSegmentType = segType;
      }
   }

   // Now use GetDistanceOffset to get an accurate distance
   if (bestSegmentType == 2)
   {
      UtVec3d a, b, c;
      double  hdgAtoB, distAtoB;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[bestSegment].GetLat(),
                                                      mWaypoints[bestSegment].GetLon(),
                                                      mWaypoints[bestSegment + 1].GetLat(),
                                                      mWaypoints[bestSegment + 1].GetLon(),
                                                      hdgAtoB,
                                                      distAtoB);
      double hdgAtoC, distAtoC;
      UtSphericalEarth::GreatCircleHeadingAndDistance(mWaypoints[bestSegment].GetLat(),
                                                      mWaypoints[bestSegment].GetLon(),
                                                      aPointLL[0],
                                                      aPointLL[1],
                                                      hdgAtoC,
                                                      distAtoC);
      double angle        = (hdgAtoC - hdgAtoB) * UtMath::cRAD_PER_DEG;
      aDistanceFromRoute  = fabs(sin(angle) * distAtoC);
      aDistanceAlongRoute = cos(angle) * distAtoC + mWaypoints[bestSegment].GetDistanceAlongRoute();
   }
   else
   {
      const WsfWaypoint& wpt = mWaypoints[bestSegment + bestSegmentType];
      double             heading;
      UtSphericalEarth::GreatCircleHeadingAndDistance(wpt.GetLat(),
                                                      wpt.GetLon(),
                                                      aPointLL[0],
                                                      aPointLL[1],
                                                      heading,
                                                      aDistanceFromRoute);
      aDistanceAlongRoute = wpt.GetDistanceAlongRoute();
   }
   return bestSegment;
}

// =================================================================================================
//! Computes a location along a route given a distance from the start of the route.
//! @param aDistanceFromStart  The distance from the start of the route
//! @param aPointAlongRoute    Set to the point on the route at the specified distance from start.
//! @param aSegmentIndex       Set to the segment index at which the point occurs
//! @returns True if successful
bool WsfRoute::PointAlongRoute(double aDistanceFromStart, double aPointAlongRoute[2], int& aSegmentIndex)
{
   bool pointOnRoute = false;
   if (GetEndPointsAtDistance(aDistanceFromStart, aSegmentIndex))
   {
      WsfWaypoint& a               = mWaypoints[aSegmentIndex];
      WsfWaypoint& b               = mWaypoints[aSegmentIndex + 1];
      double       d1              = a.GetDistanceAlongRoute();
      double       d2              = b.GetDistanceAlongRoute();
      double       distanceBetween = d2 - d1;
      double       f2              = 0.0;
      // Quietly prevent divide by zero
      if (distanceBetween > 0)
      {
         f2 = (aDistanceFromStart - d1) / (distanceBetween);
      }
      double f1           = 1.0 - f2;
      aPointAlongRoute[0] = a.GetLat() * f1 + b.GetLat() * f2;
      aPointAlongRoute[1] = a.GetLon() * f1 + b.GetLon() * f2;
      pointOnRoute        = true;
   }
   return pointOnRoute;
}

// =================================================================================================
//! Projects the given latitude/longitude onto a segment where the provided
//! closest waypoint is one of the endpoints of the segment.
//!
//! @param aLat The latitude to project.
//! @param aLon The longitude to project.
//! @param aClosestWaypointIndex The closest waypoint to the provided latitude/longitude.
//! @param aSegmentIndex The index of the waypoint at segment start
//! @param aProjectedLL Set to the projected point.
//! @return Returns true if the projection was possible.

bool WsfRoute::ProjectOntoRouteGivenClosestWaypoint(double aLat,
                                                    double aLon,
                                                    int    aClosestWaypointIndex,
                                                    int&   aSegmentIndex,
                                                    double aProjectedLL[2])
{
   bool projected = false;
   if (aClosestWaypointIndex < (int)mWaypoints.size())
   {
      double             pointLL[2]   = {aLat, aLon};
      const WsfWaypoint& current      = mWaypoints[aClosestWaypointIndex];
      double             currentLL[2] = {current.GetLat(), current.GetLon()};

      // First try the next waypoint.
      if (aClosestWaypointIndex + 1 < (int)mWaypoints.size())
      {
         const WsfWaypoint& b         = mWaypoints[aClosestWaypointIndex + 1];
         double             segEnd[2] = {b.GetLat(), b.GetLon()};
         aSegmentIndex                = aClosestWaypointIndex;
         projected                    = ProjectOntoRoute(pointLL, currentLL, segEnd, aProjectedLL);
      }

      if (!projected && (aClosestWaypointIndex - 1 < (int)mWaypoints.size()) && (aClosestWaypointIndex - 1 >= 0))
      {
         const WsfWaypoint& b         = mWaypoints[aClosestWaypointIndex - 1];
         double             segEnd[2] = {b.GetLat(), b.GetLon()};
         aSegmentIndex                = aClosestWaypointIndex - 1;
         projected                    = ProjectOntoRoute(pointLL, segEnd, currentLL, aProjectedLL);
      }
   }
   return projected;
}

// =================================================================================================
//! Returns a list of intersections between two routes.
//! Assumes that both routes contain only latitude-longitude waypoints.
int WsfRoute::Intersect(WsfRoute& aOtherRoute, std::vector<RouteIntersection>& aIntersections)
{
   int intersectionCount = 0;

   // Add a segment for each waypoint segment
   SegmentList segments;
   for (size_t i = 0; i < mWaypoints.size() - 1; ++i)
   {
      segments.emplace_back(mWaypoints[i].GetLat(),
                            mWaypoints[i].GetLon(),
                            mWaypoints[i + 1].GetLat(),
                            mWaypoints[i + 1].GetLon());
   }
   for (size_t i = 0; i < aOtherRoute.mWaypoints.size() - 1; ++i)
   {
      segments.emplace_back(aOtherRoute.mWaypoints[i].GetLat(),
                            aOtherRoute.mWaypoints[i].GetLon(),
                            aOtherRoute.mWaypoints[i + 1].GetLat(),
                            aOtherRoute.mWaypoints[i + 1].GetLon());
   }

   // Compute intersections
   IntersectionList intersections;
   IntersectSegments(segments, intersections);

   for (Intersection& inter : intersections)
   {
      if (inter.mSegmentIndices[0] > inter.mSegmentIndices[1])
      {
         std::swap(inter.mSegmentIndices[0], inter.mSegmentIndices[1]);
      }

      ++intersectionCount;
      RouteIntersection routeIntersection;
      routeIntersection.mWaypointIndex1 = inter.mSegmentIndices[0];
      routeIntersection.mWaypointIndex2 = inter.mSegmentIndices[1] - static_cast<int>(mWaypoints.size()) + 1;

      routeIntersection.mPoint = inter.mPoint;
      aIntersections.push_back(routeIntersection);
   }

   return intersectionCount;
}

// =================================================================================================
bool WsfRoute::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "navigation")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         ProcessNavigationInput(inputBlock.GetInput());
      }
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
   }
   else if (ProcessNavigationInput(aInput)) // allow for legacy inline input of waypoints
   {
      // TODO - no deprecation warning for now until a final decision on how to deal with waypoints in general is
      // decided. ut::log::warning() << "route input without navigation ... end_navigation block is deprecated\n"
      //          << aInput.GetCurrentInput()->RealGetLocation();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Used to print out a route for debugging purposes
void WsfRoute::Print(ut::log::MessageStream& aStream) const
{
   if (!GetName().empty())
   {
      aStream.AddNote() << "Name: " << GetName();
   }
   if (mWaypoints.empty())
   {
      aStream.AddNote() << "There are no waypoints in this route.";
   }
   for (const auto& waypoint : mWaypoints)
   {
      auto waypointNote = aStream.AddNote() << "Waypoint:";
      waypoint.Print(waypointNote);
   }
}

// =================================================================================================
// Read in and process all configuration data up to the given ending string.
// public static
void WsfRoute::ProcessInputBlock(UtInputBlock& aInputBlock, WsfRoute* aRoutePtr, bool aAllowName)
{
   while (aInputBlock.ReadCommand())
   {
      UtInput& input = aInputBlock.GetInput();
      if (aRoutePtr->ProcessInput(input))
      {
      }
      else if (aAllowName && (input.GetCommand() == "name"))
      {
         std::string temp;
         input.ReadCommand(temp);
         WsfStringId nameId;
         input.ReadValue(nameId);
         aRoutePtr->SetName(nameId);
      }
      else
      {
         throw UtInput::UnknownCommand(aInputBlock.GetInput());
      }
   }
}

// =================================================================================================
// private
bool WsfRoute::ProcessNavigationInput(UtInput& aInput)
{
   bool myCommand = true;
   bool processed = false;

   double      absoluteHeading = 0.0;
   std::string command;
   aInput.GetCommand(command);
   while (myCommand)
   {
      if (command == "label")
      {
         aInput.ReadValue(mLabelId);
         processed = true;
      }
      else if ((command == "goto") || (command == "go_to"))
      {
         WsfStringId label;
         aInput.ReadValue(label);
         if (mWaypoints.empty())
         {
            throw UtInput::BadValue(aInput, "No active point for 'goto'");
         }
         mWaypoints.back().SetGotoId(label);
         processed = true;
      }
      else if (command == "position")
      {
         WsfWaypoint newWaypoint;
         double      lat, lon;
         aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
         aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
         newWaypoint.SetLat(lat);
         newWaypoint.SetLon(lon);
         newWaypoint.SetPointType(WsfWaypoint::cLATITUDE_AND_LONGITUDE);
         ProcessWaypoint(aInput, newWaypoint);
         processed = true;
      }
      else if (command == "mgrs_coordinate")
      {
         std::string mgrsString;
         aInput.ReadValue(mgrsString);
         double      lat, lon;
         bool        success = UtEllipsoidalEarth::ConvertMGRSToLL(mgrsString, lat, lon);
         WsfWaypoint newWaypoint;
         newWaypoint.SetLat(lat);
         newWaypoint.SetLon(lon);

         if (success)
         {
            newWaypoint.SetPointType(WsfWaypoint::cLATITUDE_AND_LONGITUDE);
            ProcessWaypoint(aInput, newWaypoint);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unable to convert MGRS location: " + mgrsString);
         }
         processed = true;
      }
      else if (command == "offset")
      {
         std::string units;
         double      lat, lon;
         aInput.ReadValue(lat);
         aInput.ReadValue(lon);
         aInput.ReadValue(units);
         double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
         lat *= multiplier;
         lon *= multiplier;
         WsfWaypoint newWaypoint;
         newWaypoint.SetPointType(WsfWaypoint::cRELATIVE_OFFSET);
         newWaypoint.SetLat(lat);
         newWaypoint.SetLon(lon);
         ProcessWaypoint(aInput, newWaypoint);
         processed = true;
      }
      else if (command == "turn_right")
      {
         double heading;
         aInput.ReadValueOfType(heading, UtInput::cANGLE);
         WsfWaypoint newWaypoint;
         newWaypoint.SetHeading(heading);
         newWaypoint.SetPointType(WsfWaypoint::cRELATIVE_HEADING);
         ProcessWaypoint(aInput, newWaypoint);
         absoluteHeading = ProcessTurnWaypoint(newWaypoint, heading, absoluteHeading);
         processed       = true;
      }
      else if (command == "turn_left")
      {
         double heading;
         aInput.ReadValueOfType(heading, UtInput::cANGLE);
         WsfWaypoint newWaypoint;
         newWaypoint.SetPointType(WsfWaypoint::cRELATIVE_HEADING);
         newWaypoint.SetHeading(-heading);
         ProcessWaypoint(aInput, newWaypoint);
         absoluteHeading = ProcessTurnWaypoint(newWaypoint, heading, absoluteHeading);
         processed       = true;
      }
      else if (command == "turn_to_heading")
      {
         double heading;
         aInput.ReadValueOfType(heading, UtInput::cANGLE);
         WsfWaypoint newWaypoint;
         newWaypoint.SetPointType(WsfWaypoint::cABSOLUTE_HEADING);
         newWaypoint.SetHeading(heading);
         ProcessWaypoint(aInput, newWaypoint);
         if (IsGlobalRoute())
         {
            double range    = CalculateTurnWaypointDistance(newWaypoint);
            size_t index    = GetSize() - 1;
            absoluteHeading = heading;
            double lat      = 0.0;
            double lon      = 0.0;
            if (GetSize() > 1)
            {
               auto prevPoint = GetWaypointAt(index - 1);
               lat            = prevPoint.GetLat();
               lon            = prevPoint.GetLon();
            }
            UtSphericalEarth::ExtrapolateGreatCirclePosition(lat, lon, absoluteHeading * UtMath::cDEG_PER_RAD, range, lat, lon);
            mWaypoints[index].SetLat(lat);
            mWaypoints[index].SetLon(lon);
         }
         processed = true;
      }
      else if (command == "transform_route" || command == "insert_route" || command == "insert_offset_route")
      {
         if (command == "transform_route")
         {
            auto out = ut::log::warning()
                       << "transform_route has been deprecated and will be removed, use insert_route instead.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
         std::string routeName;
         aInput.ReadValue(routeName);

         WsfScenario::FromInput(aInput).GetDeferredInput().Requires("route", routeName);
         WsfRoute* xformRoutePtr = dynamic_cast<WsfRoute*>(WsfScenario::FromInput(aInput).FindType("route", routeName));
         if (xformRoutePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "Route does not exist: " + routeName);
         }
         xformRoutePtr->mOffsetRouteValid = (command == "insert_offset_route");

         // Determine the base position and heading for the transformed route.
         // If the current route is empty then the values must follow in the command stream.
         // If the current route is not empty then the values are derived from the last point in the route.

         double lat     = 0.0;
         double lon     = 0.0;
         double heading = 0.0;
         if (GetSize() == 0)
         {
            aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
            aInput.ReadValueOfType(heading, UtInput::cANGLE);
         }
         else
         {
            unsigned int       lastIndex = GetSize() - 1;
            const WsfWaypoint& lastPoint = mWaypoints[lastIndex];
            if ((lastPoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) == 0)
            {
               throw UtInput::BadValue(aInput, "transform_route cannot append to a relative path");
            }
            lat     = lastPoint.GetLat();
            lon     = lastPoint.GetLon();
            heading = lastPoint.GetHeading();

            std::string referenceHeading;
            aInput.ReadValue(referenceHeading);
            if (referenceHeading == "reference_heading")
            {
               aInput.ReadValueOfType(heading, UtInput::cANGLE);
            }
            else if (lastIndex != 0)
            {
               aInput.PushBack(referenceHeading);
               // If two or more points in the route then attempt to derive the heading from the last two points.
               const WsfWaypoint& prevPoint = mWaypoints[lastIndex - 1];
               if (prevPoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
               {
                  // Get the heading in the reverse direction and then take the reciprocal heading. This doesn't
                  // make much difference for points that are close, but it does with longer distances where the
                  // headings change along a great circle.
                  double distance;
                  double tempHeading;
                  lastPoint.GreatCircleHeadingAndDistance(prevPoint, tempHeading, distance);
                  tempHeading *= UtMath::cRAD_PER_DEG; // Return was in degrees
                  heading = UtMath::NormalizeAngle0_TwoPi(tempHeading + UtMath::cPI);
               }
            }
            else
            {
               aInput.PushBack(referenceHeading);
            }
         }

         // Create a temporary route that will be transformed, transform it and then append to the current route.
         WsfRoute xformRoute(*xformRoutePtr);
         xformRoute.Transform(lat, lon, heading * UtMath::cDEG_PER_RAD);
         Append(xformRoute);
      }
      else if (command == "transform_absolute_route")
      {
         std::string routeName;
         aInput.ReadValue(routeName);
#if !WSF_WITHOUT_SIMULATION
         WsfScenario::FromInput(aInput).GetDeferredInput().Requires("route", routeName);
         WsfRoute* xformRoutePtr = dynamic_cast<WsfRoute*>(WsfScenario::FromInput(aInput).FindType("route", routeName));
#else
         WsfRoute* xformRoutePtr = nullptr;
#endif
         if (xformRoutePtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "Route does not exist: " + routeName);
         }
         WsfStringId nameId = GetNameId();
         *this              = *xformRoutePtr;
         SetName(nameId);
         double north;
         double east;
         double down;
         aInput.ReadValueOfType(north, UtInput::cLENGTH);
         aInput.ReadValueOfType(east, UtInput::cLENGTH);
         aInput.ReadValueOfType(down, UtInput::cLENGTH);
         TransformNED(north, east, down);
      }
      else
      {
         myCommand = false;
      }

      if (myCommand)
      {
         aInput.ReadCommand(command);
      }
      else
      {
         aInput.PushBack(command);
      }
   }
   return processed;
}

// =================================================================================================
// private
void WsfRoute::ProcessWaypoint(UtInput& aInput, WsfWaypoint& aWaypoint)
{
   // Assign the current label to this waypoint and reset it.
   aWaypoint.SetLabelId(mLabelId);
   mLabelId = nullptr;

   // Process the remaining input
   bool processed = aWaypoint.ProcessAllInput(aInput);

   // Add the waypoint to the route
   if (processed && (!mWaypoints.empty()))
   {
      const WsfWaypoint& lastWaypoint = mWaypoints.back();
      if ((aWaypoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) &&
          (lastWaypoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) &&
          (aWaypoint.GetLat() == lastWaypoint.GetLat()) && (aWaypoint.GetLon() == lastWaypoint.GetLon()))
      {
         throw UtInput::BadValue(aInput, "Coincident waypoints!");
      }
   }
   Append(aWaypoint);
}

// =================================================================================================
//! Computes the distance between two adjacent waypoints given their indices.
//! If either of these waypoints are not lat/lon points, or distance points, aFirstWptIndex and
//! aSecondWptIndex will be incremented/decremented to set the to the nearest points on which distance
//! can be computed.
//! @returns Distance between the two points, or negative if it cannot be computed.
double WsfRoute::ComputeDistance(unsigned int& aFirstWptIndex, unsigned int& aSecondWptIndex)
{
   // move up aSecondWptIndex until we find a lat/lon or distance point.
   const int cDISTANCE_MASK = WsfWaypoint::cLATITUDE_AND_LONGITUDE | WsfWaypoint::cDISTANCE;
   while ((static_cast<size_t>(aSecondWptIndex) < mWaypoints.size()) &&
          (0 == (mWaypoints[aSecondWptIndex].GetPointType() & cDISTANCE_MASK)))
   {
      ++aSecondWptIndex;
   }

   double distance = -1.0;

   if (static_cast<size_t>(aSecondWptIndex) < mWaypoints.size())
   {
      WsfWaypoint& b = mWaypoints[aSecondWptIndex];
      if (b.GetPointType() & WsfWaypoint::cDISTANCE)
      {
         distance = b.GetDistanceOrTime();
      }
      else
      {
         // back up aFirstWptIndex until we find a lat/lon point
         bool found = false;
         for (unsigned int i = aFirstWptIndex + 1U; i-- > 0;)
         {
            if ((0 != (mWaypoints[i].GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)))
            {
               aFirstWptIndex = i;
               found          = true;
               break;
            }
         }

         if (!found)
         {
            aFirstWptIndex = 0U;
         }
         else
         {
            WsfWaypoint& a = mWaypoints[aFirstWptIndex];
            if ((a.GetPointType() & b.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE) != 0)
            {
               // 2 lat/lon's. Compute distance, and update heading to point.
               double heading;
               a.GreatCircleHeadingAndDistance(b, heading, distance);
               // If the waypoint does not specify a heading, compute one
               if (a.GetHeading() == WsfPath::cDOUBLE_NOT_SET)
               {
                  // this method is used when inserting waypoints, update the heading when possible
                  if (mWaypoints[aFirstWptIndex + 1].GetPointType() & WsfWaypoint::cRELATIVE_HEADING)
                  {
                     a.SetHeading(0.0);
                     // When it comes to relative heading waypoints, having a clear and consistent idea of the previous
                     // waypoint's heading is important, and it shouldn't depend on the location of the following waypoint.
                  }
                  else
                  {
                     a.SetHeading(heading * UtMath::cRAD_PER_DEG);
                  }
               }
            }
         }
      }
   }
   return distance;
}

// =================================================================================================
//! Return a reference to the waypoint in the route specified by the given index.
//! @param aIndex The waypoint number to be referenced.
//! @returns A non-const reference to the waypoint element specified by 'aIndex.'
WsfWaypoint& WsfRoute::GetWaypointAt(size_t aIndex)
{
   assert(aIndex < mWaypoints.size());
   return mWaypoints[aIndex];
}

// =================================================================================================
//! Return a reference to the waypoint in the route specified by the given index.
//! @param aIndex The waypoint number to be referenced.
//! @returns A const reference to the waypoint element specified by 'aIndex.'
const WsfWaypoint& WsfRoute::GetWaypointAt(size_t aIndex) const
{
   assert(aIndex < mWaypoints.size());
   return mWaypoints[aIndex];
}

// =================================================================================================
//! Return a index to the waypoint in the route specified by the given label.
//! @param aLabelId The waypoint label.
//!  @returns The index of the first waypoint with the given label or the value of Size()
//!           if a waypoint with the given label could not be found.
unsigned int WsfRoute::GetWaypointIndexAt(WsfStringId aLabelId) const
{
   unsigned int numberOfWaypoints = static_cast<unsigned int>(mWaypoints.size());
   unsigned int index             = 0;
   while (index < numberOfWaypoints)
   {
      if (mWaypoints[index].GetLabelId() == aLabelId)
      {
         break;
      }
      ++index;
   }
   return index;
}

bool WsfRoute::IsGlobalRoute() const
{
   return (!GetName().empty());
}

// =================================================================================================
//! Assign a route with a particular route Id.
//! @param aRouteId The route id to associate with the route.
void WsfRoute::SetNetworkRouteId(unsigned int aRouteId)
{
   mRouteId              = aRouteId;
   int lastWaypointIndex = static_cast<int>(mWaypoints.size()) - 1;
   for (int index = 0; index <= lastWaypointIndex; ++index)
   {
      mWaypoints[index].SetRouteId(aRouteId);
   }
}

// =================================================================================================
//! Set the speed of all waypoints in the route
//! @param aSpeed The speed to which all waypoints in the route will be set.
void WsfRoute::SetSpeed(double aSpeed)
{
   size_t numberOfWaypoints = mWaypoints.size();
   for (size_t index = 0; index < numberOfWaypoints; ++index)
   {
      mWaypoints[index].SetSpeed(aSpeed);
   }
}

// =================================================================================================
//! Transform a relative route to an absolute route by applying a rotation and a translation.
//! This method is useful for transforming a pattern constructed of relative
//! positions into an absolute route that can be used by a mover.
//! rotation and a translation.  Waypoints of type cRELATIVE_OFFSET are
//! rotated and translated to become points of cLATITUDE_AND_LONGITUDE.
//!
//! @param aRefLat The latitude coordinate (degrees) of the local coordinate system's origin.
//! @param aRefLon The longitude coordinate (degrees) of the local coordinate system's origin.
//! @param aHeading The heading (degrees) of the X axis from north.
//!
//! @note In an unrotated frame +X is North and +Y is East.
void WsfRoute::Transform(double aRefLat, double aRefLon, double aHeading)
{
   double xyzNED[3];
   double refECEF[3];
   double transECEF[3][3];

   // Determine the ECEF->Local NED transformation.
   UtEllipsoidalEarth::ComputeNEDTransform(aRefLat, aRefLon, 0.0, transECEF, refECEF);

   // Determine the components of the rotation matrix. Heading is in degrees.
   double cosHeading = cos(aHeading * UtMath::cRAD_PER_DEG);
   double sinHeading = sin(aHeading * UtMath::cRAD_PER_DEG);

   for (auto& waypoint : mWaypoints)
   {
      if (waypoint.GetPointType() & WsfWaypoint::cRELATIVE_OFFSET)
      {
         // Extract the local NED coordinates of the point...

         double x = waypoint.GetLat(); // mLat is really the local X
         double y = waypoint.GetLon(); // mLon is really the local Y

         // Rotate by the desired heading...

         xyzNED[0] = x * cosHeading - y * sinHeading;
         xyzNED[1] = x * sinHeading + y * cosHeading;
         xyzNED[2] = 0.0;

         // Convert the local NED coordinate to LLA
         double lat, lon, alt;
         UtEllipsoidalEarth::ConvertLocalToLLA(refECEF, transECEF, xyzNED, lat, lon, alt);
         waypoint.SetLat(lat);
         waypoint.SetLon(lon);
         waypoint.SetPointType((waypoint.GetPointType() & ~WsfWaypoint::cLOCATION_MASK) |
                               WsfWaypoint::cLATITUDE_AND_LONGITUDE);
      }

      if (mOffsetRouteValid)
      {
         // Determine the ECEF->Local NED transformation for the next waypoint
         UtEllipsoidalEarth::ComputeNEDTransform(waypoint.GetLat(), waypoint.GetLon(), 0.0, transECEF, refECEF);
      }
   }
}

// =================================================================================================
//! Transform an absolute route by a given offset.
//! Only waypoints of type cLATITUDE_AND_LONGITUDE are translated.
//!
//! @param aNorth The distance the waypoint will be translated in the north direction (meters).
//! @param aEast The distance the waypoint will be translated in the east direction (meters).
//! @param aDown The distance the waypoint will be translated in the down direction (meters).
void WsfRoute::TransformNED(double aNorth, double aEast, double aDown)
{
   double wptNED[3];
   double refECEF[3];
   double transECEF[3][3];

   for (auto& waypoint : mWaypoints)
   {
      if (waypoint.GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         // Determine the ECEF->Local NED transformation.
         UtEllipsoidalEarth::ComputeNEDTransform(waypoint.GetLat(), waypoint.GetLon(), waypoint.GetAlt(), transECEF, refECEF);

         // Convert the waypoint's LLA to NED.
         UtEllipsoidalEarth::ConvertLLAToLocal(refECEF, transECEF, waypoint.GetLat(), waypoint.GetLon(), waypoint.GetAlt(), wptNED);

         // Add the NED offsets to the waypoint's NED components.
         wptNED[0] += aNorth;
         wptNED[1] += aEast;
         wptNED[2] += aDown;

         // Convert the waypoint's new NED components back to LLA.
         double lat, lon, alt;
         UtEllipsoidalEarth::ConvertLocalToLLA(refECEF, transECEF, wptNED, lat, lon, alt);
         waypoint.SetLat(lat);
         waypoint.SetLon(lon);
         waypoint.SetAlt(alt);
      }
   }
}

const char* WsfRoute::GetScriptClassName() const
{
   return "WsfRoute";
}

double WsfRoute::CalculateTurnWaypointDistance(const WsfWaypoint& aWaypoint) const
{
   double speed = aWaypoint.GetSpeed();
   size_t index = GetSize() - 1;
   if (speed == WsfPath::cDOUBLE_NOT_SET)
   {
      for (size_t i = index; i < std::numeric_limits<size_t>::max(); --i)
      {
         if (GetWaypointAt(i).GetSpeed() != WsfPath::cDOUBLE_NOT_SET)
         {
            speed = GetWaypointAt(i).GetSpeed();
            break;
         }
      }
   }
   if (aWaypoint.GetPointType() & WsfWaypoint::cTIME)
   {
      // require speed and time
      return speed * aWaypoint.GetDistanceOrTime();
   }
   else
   {
      // use distance
      return aWaypoint.GetDistanceOrTime();
   }
}

double WsfRoute::ProcessTurnWaypoint(const WsfWaypoint& aWaypoint, double aNewHeading, double aCurrentHeading)
{
   if (IsGlobalRoute())
   {
      double range = CalculateTurnWaypointDistance(aWaypoint);
      size_t index = GetSize() - 1;
      // relative heading
      auto prevPoint = GetWaypointAt(index - 1);
      if (prevPoint.GetHeading() == WsfPath::cDOUBLE_NOT_SET || prevPoint.GetPointType() & WsfWaypoint::cRELATIVE_HEADING)
      {
         if (index > 1)
         {
            auto twoPointsBack = GetWaypointAt(index - 2);
            // We don't want to use the distance, only the heading
            double _;
            UtSphericalEarth::GreatCircleHeadingAndDistance(twoPointsBack.GetLat(),
                                                            twoPointsBack.GetLon(),
                                                            prevPoint.GetLat(),
                                                            prevPoint.GetLon(),
                                                            aCurrentHeading,
                                                            _);
            aCurrentHeading *= UtMath::cRAD_PER_DEG;
         }
         // else use the heading calculated last waypoint since it must have also been realative heading.
      }
      else
      {
         aCurrentHeading = prevPoint.GetHeading();
      }
      aCurrentHeading += aNewHeading;
      double lat, lon;
      UtSphericalEarth::ExtrapolateGreatCirclePosition(prevPoint.GetLat(),
                                                       prevPoint.GetLon(),
                                                       aCurrentHeading * UtMath::cDEG_PER_RAD,
                                                       range,
                                                       lat,
                                                       lon);
      mWaypoints[index].SetLat(lat);
      mWaypoints[index].SetLon(lon);
   }
   return aCurrentHeading;
}

//! Finds all intersections between segments in aSegmentList. Segments are defined
//! as between every two pairs points.
void WsfRoute::IntersectSegments(const SegmentList& aSegmentList, IntersectionList& aIntersections)
{
   // A simplified version of the sweep-line algorithm for intersecting segments
   // An intersection test is done between every pair of lines which overlap
   // in the latitude-coordinate (this is scanning over "y" instead of the traditional "x").

   using Edge     = std::pair<double, int>;
   using EdgeList = std::list<Edge>;
   EdgeList starts;
   // The "stops" structure denotes a set of temporary stops, or endpoints of line segments of the sweep.
   EdgeList stops;

   // For each line segment, store the minimum y value of the two points as the start value.
   for (size_t i = 0; i < aSegmentList.size(); i += 1)
   {
      double beginLat = aSegmentList[i].mBegin.GetLat();
      double endLat   = aSegmentList[i].mEnd.GetLat();
      if (beginLat < endLat)
      {
         starts.emplace_back(beginLat, static_cast<int>(i));
         stops.emplace_back(endLat, static_cast<int>(i));
      }
      else
      {
         starts.emplace_back(endLat, static_cast<int>(i));
         stops.emplace_back(beginLat, static_cast<int>(i));
      }
   }

   starts.sort();
   stops.sort();

   // latSweep is the current location in the sweep.
   double latSweep = -1.0E30;
   for (auto& start : starts)
   {
      // No need to re-evaluate the same point
      if (latSweep == start.first)
      {
         continue;
      }
      latSweep = start.first;

      auto stop         = stops.begin();
      int  segmentIndex = stop->second;
      stops.pop_front();

      const Segment& segment1 = aSegmentList[segmentIndex];

      // Iterate over the remaining stops and see if we have an intersection.
      // for (size_t i = 0; i < stops.size(); ++i)
      for (stop = stops.begin(); stop != stops.end(); ++stop)
      {
         // Identify the index of the other segment so we can test against it.
         int            otherSegmentIndex = stop->second;
         const Segment& segment2          = aSegmentList[otherSegmentIndex];

         // Do not consider segments beyond the lat sweep
         if ((segment2.mBegin.GetLat() > latSweep) && (segment2.mEnd.GetLat() > latSweep))
         {
            continue;
         }

         // Do not consider intersections at begin or endpoints.
         if ((segment1.mEnd.GetLat() == segment2.mBegin.GetLat()) || (segment1.mBegin.GetLat() == segment2.mEnd.GetLat()))
         {
            continue;
         }

         UtLLPos intersectPosition; // The returned intersection location, if any.
         if (GreatCircleIntersect(segment1, segment2, intersectPosition))
         {
            Intersection intersection;
            intersection.mSegmentIndices[0] = segmentIndex;
            intersection.mSegmentIndices[1] = otherSegmentIndex;
            intersection.mSegments[0]       = segment1;
            intersection.mSegments[1]       = segment2;
            intersection.mPoint             = intersectPosition;
            aIntersections.push_back(intersection);
         }
      }
   }
}

// static
bool WsfRoute::GreatCircleIntersect(const Segment& aSegment1, const Segment& aSegment2, UtLLPos& aIntersectPosition)
{
   UtLLPos intersect2;
   UtSphericalEarth::GreatCircleIntersection(aIntersectPosition,
                                             intersect2,
                                             aSegment1.mBegin,
                                             aSegment1.mEnd,
                                             aSegment2.mBegin,
                                             aSegment2.mEnd);
   bool intersected = aIntersectPosition.GetLat() != std::numeric_limits<double>::max();
   if (!intersected)
   {
      intersected = (intersect2.GetLat() != std::numeric_limits<double>::max());
      if (intersected)
      {
         aIntersectPosition = intersect2;
      }
   }
   return intersected;
}
