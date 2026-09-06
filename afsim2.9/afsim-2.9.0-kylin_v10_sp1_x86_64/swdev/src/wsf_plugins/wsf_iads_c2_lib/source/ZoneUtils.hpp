// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#pragma once
class WsfZoneDefinition;
class WsfSimulation;
#include "WsfGeoPoint.hpp"

namespace ZoneUtils
{
   // @return The distance in meters from the point to the zone center.
   // @param point    The point from which to calculate the distance.
   // @param zone     The Zone of interest.
   double GetDistanceFromPointToZoneCenter(WsfSimulation* sim_ptr, double wcsPoint[3], WsfZoneDefinition& zone);

   // @return true    If the calculation was successfully completed
   // @param point    The point from which to calculate the distance.
   // @param zone     The Zone of interest.
   // @param distance The distance in meters from the point to the nearest point on the zone. If the calculation
   //                 fails, will be set to -1.
   // @note This function assumes that the distance is along the radial from the point to the zone centroid.
   bool GetDistanceFromPointToZone(WsfSimulation* sim_ptr, double wcsPoint[3], WsfZoneDefinition& zone, double& distance);

   //@return Location of the centroid of zone.
   //@param zone The zone for which to calculate the centroid. Some of the Zone
   //            methods are non-const, so it is passed as non-const.
   WsfGeoPoint GetZoneCentroid(WsfSimulation* sim_ptr, WsfZoneDefinition& zone);

   // @return true       If any part of the line segment defined by the starting position and velocity lies within the zone.
   // @param zone        The zone of interest.
   // @param WcsPosition The beginning point of the segment.
   // @param WcsVelocity The velocity of the point of interest. This will be propagated forward to see if the
   //                    point ever lies in the zone.
   bool isProjectedInsideZonePosVel(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double wcsPosition[3], double wcsVelocity[3]);

   // @return true        If any part of the line segment defined by the two points lies within the zone.
   // @param zone         The zone of interest.
   // @param WcsPosition1 The beginning point of the segment.
   // @param WcsPosition2 The end point of the segment.
   // @note Only the line segment itself is checked for intersection with the zone. We do not attempt to
   //       extrapolate off either end of the segment.
   bool isProjectedInsideZone2Points(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double wcsPosition1[3], double wcsPosition2[3]);

   // @return true if any part of the line segment defined by the starting position and compass heading lies within the zone.
   // @param zone The zone of interest.
   // @param wcsPosition1 The beginning point of the segment.
   // @param headingRadians The compass heading (radians, 0 .. 2*PI)
   bool isProjectedInsideZonePointHeading(WsfSimulation* sim_ptr, WsfZoneDefinition& zone, double WcsPosition[3], double headingRadians);

   // @return true if the closest point was found.
   // @param zone The zone of interest.
   // @param wcsPoint The point from which to find the closest in-zone point.
   // @param wcsZonePoint The closest point in the zone. If the point cannot be found, this will not be overwritten.
   bool GetClosestPointInZone(WsfSimulation* sim_ptr, double wcsPoint[3], double wcsZonePoint[3], WsfZoneDefinition& zone);
}
