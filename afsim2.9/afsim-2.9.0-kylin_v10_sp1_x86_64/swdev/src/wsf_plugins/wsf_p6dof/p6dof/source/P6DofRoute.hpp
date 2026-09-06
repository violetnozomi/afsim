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

#ifndef P6DofRoute_H
#define P6DofRoute_H

#include "p6dof_export.h"

#include <map>
#include <memory>
#include <vector>

#include "P6DofCommonController.hpp"
#include "P6DofWaypoint.hpp"
#include "UtLLAPos.hpp"
#include "UtVec2.hpp"

class P6DOF_EXPORT P6DofRoute
{
public:
   // Default constructor
   P6DofRoute();

   // Return a deep copy of this route
   P6DofRoute* Clone() const;

   // Destructor
   ~P6DofRoute();

   // Geometric data computed once based on a route from one waypoint to another
   struct sRouteSegment
   {
      double  trackDistance_m;   // Distance from prev wpt to curr wpt
      double  trackEndHdg_rad;   // Heading when current wpt is reached
      double  trackStartHdg_rad; // Heading initially taken from prev wpt to curr wpt
      double  slantRange_m;      // 3D distance from prev to curr meters
      double  slope_rad;         // Angle from prev wpt to curr wpt
      double  rangeTrack_m;      // distance from current position to curr wpt
      double  earthNED[3][3];    // Transform matrix for earth NED coordinates
      UtVec3d currWptVector;     // LLA Position of the current waypoint vector
      UtVec3d prevWptVector;     // LLA Position of the previous waypoint vector
      UtVec3d trackVector;       // Vector from previous to current waypoint
   };

   // Calculate heading and bank angle for roll-only waypoint navigation
   static void CalcAimHeadingAndBankAngle(const P6DofWaypoint* aPrevWpt,        // Pointer to prev wpt
                                          const P6DofWaypoint* aCurrWpt,        // Pointer to curr wpt
                                          const P6DofWaypoint* aNextWpt,        // Pointer to next wpt
                                          const sRouteSegment* aCurrSegment,    // Pointer to curr route segment
                                          const sRouteSegment* aNextSegment,    // Pointer to next route segment
                                          const UtLLAPos&      aCurrentPos_lla, // Current position
                                          const UtVec2d&       aCurrentVel_mps, // Current velocity
                                          P6DofCommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
                                          double aTurnRollInMultiplier,         // Roll-in multiplier for autopilot
                                          double aRouteAllowableAngleError_rad, // Allowable angular error for route heading
                                          double aHeading_rad,                  // Current heading
                                          double aSpeed_mps,                    // Current speed
                                          double aMaxBankAngle_rad,             // Max allowed bank angle
                                          double aMaxBankRate_rad_s,            // Max allowed bank rate
                                          double aMaxG_g,                       // Max allowed turn g
                                          double aDeltaT_sec,
                                          bool&  aAchievedWaypoint);

   // Calculate heading angle for yaw-only navigation waypoint
   static void CalcYawAimHeadingAngle(const P6DofWaypoint* aPrevWpt,                    // Pointer to prev wpt
                                      const P6DofWaypoint* aCurrWpt,                    // Pointer to curr wpt
                                      const P6DofWaypoint* aNextWpt,                    // Pointer to next wpt
                                      const sRouteSegment* aCurrSegment,                // Pointer to curr route segment
                                      const sRouteSegment* aNextSegment,                // Pointer to next route segment
                                      const UtLLAPos&      aCurrentPos_lla,             // Current position
                                      const UtVec2d&       aCurrentVel_mps,             // Current velocity
                                      P6DofCommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
                                      double aRouteAllowableAngleError_rad, // Allowable angular error for route heading
                                      double aHeading_rad,                  // Current heading
                                      double aSpeed_mps,                    // Current speed
                                      double aMaxG_g,                       // Max allowed turn g
                                      double aDeltaT_sec,
                                      bool&  aAchievedWaypoint);

   // Calculate heading angle for taxi navigation waypoint
   static void CalcTaxiAimHeadingAngle(const P6DofWaypoint* aPrevWpt,        // Pointer to prev wpt
                                       const P6DofWaypoint* aCurrWpt,        // Pointer to curr wpt
                                       const P6DofWaypoint* aNextWpt,        // Pointer to next wpt
                                       const sRouteSegment* aCurrSegment,    // Pointer to curr route segment
                                       const sRouteSegment* aNextSegment,    // Pointer to next route segment
                                       const UtLLAPos&      aCurrentPos_lla, // Current position
                                       const UtVec2d&       aCurrentVel_mps, // Current velocity
                                       P6DofCommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
                                       float                                   aHeading_rad,   // Current heading
                                       double                                  aTurnRadius_ft, // Desired turn radius
                                       double                                  aDeltaT_sec,
                                       bool&                                   aAchievedWaypoint);

   // Calculate vertical rate data for waypoint navigation
   static void CalcVerticalSpeed(P6DofCommonController::WaypointNavData& aNavData);

   // Get the start heading from one waypoint to another
   static double GetInitialHeading_rad(const UtLLAPos& aLLAStart, const UtLLAPos& aLLAEnd);
   // Get the radius of a turn
   static double GetTurnRadius_m(double aSpeed_mps, double aBankAngle_rad);

   // Get the radius of a turn from the lateral G
   static double GetTurnRadiusFromLateralG_m(double aSpeed_mps, double aLateralG_g);

   // Get the distance at which point the vehicle needs to start turning based on turn geometry
   static double GetTurnLeadDistance_m(double aTurnAngle_rad, double aTurnRadius_m);

   // Get the start heading, end heading, and distance between two LLA positions
   static double GetDistanceBetweenWaypoints_m(const UtLLAPos& aStartLLA,
                                               const UtLLAPos& aEndLLA,
                                               double&         aStartHdg_rad,
                                               double&         aEndHdg_rad);

   // Perform one time computations for the geometry of a curve between 3 waypoints
   static sRouteSegment* CalcSegmentGeometry(const P6DofWaypoint* aPrevWpt, const P6DofWaypoint* aCurrWpt);

   // Return the first element in the mRoute vector. Using this and GetNextWaypoint, the entire
   // route can be iterated across
   const P6DofWaypoint* GetFirstElement() const;

   // Given a waypoint, return the next waypoint in the route or 0 if it is the last waypoint.
   const P6DofWaypoint* GetNextWaypoint(const P6DofWaypoint* aWaypoint) const;

   // Return the waypoint at the specified index
   const P6DofWaypoint* GetWaypointAtIndex(size_t aIndex) const;

   // Return the index of the given waypoint in the route
   size_t GetWaypointIndex(const P6DofWaypoint* aWaypoint) const;

   // Determine whether the waypoint was passed
   static bool PassedWaypoint(float aDT, const P6DofCommonController::WaypointNavData& aNavData);

   // Determine whether the waypoint was achieved, either by passing or approaching
   static bool AchievedWaypoint(float                                         aDT,
                                const P6DofCommonController::WaypointNavData& aNavData,
                                const P6DofWaypoint*                          aWaypoint,
                                const P6DofWaypoint*                          aNextWaypoint);

   // Add a waypoint to the route if the waypoint is valid and not present in the route.
   // Note: Traveling to the same waypoint multiple times can be accomplished using the
   // "GoTo"
   void AddWaypointToRouteEnd(P6DofWaypoint* aWaypoint);

   // Add a waypoint to the start of the route
   void AddWaypointToRouteStart(P6DofWaypoint* aWaypoint);

   // Return the size of the mRoute vector
   size_t GetNumberOfWaypoints() const;

   // Return an iterator to the beginning of the route
   std::vector<P6DofWaypoint*>::const_iterator Begin() const;

   // Return an iterator to the end of the route
   std::vector<P6DofWaypoint*>::const_iterator End() const;

   // Populate the segment map
   void ComputeSegmentMap();

   // Add a segment to the segment map.
   void AddSegment(P6DofWaypoint* aWaypoint, sRouteSegment* aSegment);

   // Return a route segment corresponding to a waypoint.
   // Note: Given a segment from point A to point B, the segment is mapped using
   // point A as the key.
   sRouteSegment* GetRouteSegment(const P6DofWaypoint* aWaypoint) const;

private:
   P6DofRoute& operator=(const P6DofRoute& aSrc) = delete;

   // Copy constructor
   P6DofRoute(const P6DofRoute& aSrc);

   // Vector of waypoint pointers that defines a navigation route
   std::vector<P6DofWaypoint*> mRoute;

   // Map of route segments
   // Note: Given a segment from point A to point B, the segment is mapped using
   // point A as the key.
   std::map<P6DofWaypoint*, sRouteSegment*> mSegments;
};

#endif // P6DofRoute_H
