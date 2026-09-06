// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFROUTE_HPP
#define WSFSIXDOFROUTE_HPP

#include "wsf_six_dof_export.h"

#include <map>
#include <memory>
#include <vector>

#include "UtLLAPos.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"
#include "WsfSixDOF_CommonController.hpp"
#include "WsfSixDOF_Waypoint.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT Route
{
public:
   // Default constructor
   Route() = default;

   // Return a deep copy of this route
   Route* Clone() const;

   // Destructor
   ~Route() = default;

   // Geometric data computed once based on a route from one waypoint to another
   struct sRouteSegment
   {
      double  trackDistance_m   = 0.0; // Distance from prev wpt to curr wpt
      double  trackEndHdg_rad   = 0.0; // Heading when current wpt is reached
      double  trackStartHdg_rad = 0.0; // Heading initially taken from prev wpt to curr wpt
      double  slantRange_m      = 0.0; // 3D distance from prev to curr meters
      double  slope_rad         = 0.0; // Angle from prev wpt to curr wpt
      double  rangeTrack_m      = 0.0; // distance from current position to curr wpt
      double  earthNED[3][3]    = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // Transform matrix for earth NED coordinates
      UtVec3d currWptVector;                                         // LLA Position of the current waypoint vector
      UtVec3d prevWptVector;                                         // LLA Position of the previous waypoint vector
      UtVec3d trackVector;                                           // Vector from previous to current waypoint
   };

   // Calculate heading and bank angle for roll-only waypoint navigation
   static void CalcAimHeadingAndBankAngle(const Waypoint*      aPrevWpt,               // Pointer to prev wpt
                                          const Waypoint*      aCurrWpt,               // Pointer to curr wpt
                                          const Waypoint*      aNextWpt,               // Pointer to next wpt
                                          const sRouteSegment* aCurrSegment,           // Pointer to curr route segment
                                          const sRouteSegment* aNextSegment,           // Pointer to next route segment
                                          const UtLLAPos&      aCurrentPos_lla,        // Current position
                                          const UtVec2d&       aCurrentVel_mps,        // Current velocity
                                          CommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
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
   static void CalcYawAimHeadingAngle(const Waypoint*                    aPrevWpt,     // Pointer to prev wpt
                                      const Waypoint*                    aCurrWpt,     // Pointer to curr wpt
                                      const Waypoint*                    aNextWpt,     // Pointer to next wpt
                                      const sRouteSegment*               aCurrSegment, // Pointer to curr route segment
                                      const sRouteSegment*               aNextSegment, // Pointer to next route segment
                                      const UtLLAPos&                    aCurrentPos_lla, // Current position
                                      const UtVec2d&                     aCurrentVel_mps, // Current velocity
                                      CommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
                                      double aRouteAllowableAngleError_rad, // Allowable angular error for route heading
                                      double aHeading_rad,                  // Current heading
                                      double aSpeed_mps,                    // Current speed
                                      double aMaxG_g,                       // Max allowed turn g
                                      double aDeltaT_sec,
                                      bool&  aAchievedWaypoint);

   // Calculate heading angle for taxi navigation waypoint
   static void CalcTaxiAimHeadingAngle(const Waypoint*                    aPrevWpt,     // Pointer to prev wpt
                                       const Waypoint*                    aCurrWpt,     // Pointer to curr wpt
                                       const Waypoint*                    aNextWpt,     // Pointer to next wpt
                                       const sRouteSegment*               aCurrSegment, // Pointer to curr route segment
                                       const sRouteSegment*               aNextSegment, // Pointer to next route segment
                                       const UtLLAPos&                    aCurrentPos_lla, // Current position
                                       const UtVec2d&                     aCurrentVel_mps, // Current velocity
                                       CommonController::WaypointNavData& aNavData, // Output of nav data for autopilot
                                       float                              aHeading_rad,   // Current heading
                                       double                             aTurnRadius_ft, // Desired turn radius
                                       double                             aDeltaT_sec,
                                       bool&                              aAchievedWaypoint);

   // Calculate vertical rate data for waypoint navigation
   static void CalcVerticalSpeed(CommonController::WaypointNavData& aNavData);

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
   static std::unique_ptr<sRouteSegment> CalcSegmentGeometry(const Waypoint* aPrevWpt, const Waypoint* aCurrWpt);

   // Return the first element in the mRoute vector. Using this and GetNextWaypoint, the entire
   // route can be iterated across
   const Waypoint* GetFirstElement() const;

   // Given a waypoint, return the next waypoint in the route or 0 if it is the last waypoint.
   const Waypoint* GetNextWaypoint(const Waypoint* aWaypoint) const;

   // Return the waypoint at the specified index
   const Waypoint* GetWaypointAtIndex(size_t aIndex) const;

   // Return the index of the given waypoint in the route
   size_t GetWaypointIndex(const Waypoint* aWaypoint) const;

   // Determine whether the waypoint was passed
   static bool PassedWaypoint(float aDT, const CommonController::WaypointNavData& aNavData);

   // Determine whether the waypoint was achieved, either by passing or approaching
   static bool AchievedWaypoint(float                                    aDT,
                                const CommonController::WaypointNavData& aNavData,
                                const Waypoint*                          aWaypoint,
                                const Waypoint*                          aNextWaypoint);

   // Add a waypoint to the route if the waypoint is valid and not present in the route.
   // Note: Traveling to the same waypoint multiple times can be accomplished using the
   // "GoTo"
   void AddWaypointToRouteEnd(std::unique_ptr<Waypoint>& aWaypoint);

   // Add a waypoint to the start of the route
   void AddWaypointToRouteStart(std::unique_ptr<Waypoint>& aWaypoint);

   // Return the size of the mRoute vector
   size_t GetNumberOfWaypoints() const;

   // Return a const pointer to the route
   const std::vector<std::unique_ptr<Waypoint>>& GetRoute() const { return mRoute; }

   // Populate the segment map
   void ComputeSegmentMap();

   // Add a segment to the segment map.
   void AddSegment(Waypoint* aWaypoint, std::unique_ptr<sRouteSegment>& aSegment);

   // Return a route segment corresponding to a waypoint.
   // Note: Given a segment from point A to point B, the segment is mapped using
   // point A as the key.
   sRouteSegment* GetRouteSegment(const Waypoint* aWaypoint) const;

private:
   Route& operator=(const Route& aSrc) = delete;

   // Copy constructor
   Route(const Route& aSrc);

   // Vector of waypoint pointers that defines a navigation route
   std::vector<std::unique_ptr<Waypoint>> mRoute;

   // Map of route segments
   // Note: Given a segment from point A to point B, the segment is mapped using
   // point A as the key.
   std::map<Waypoint*, std::unique_ptr<sRouteSegment>> mSegments;
};
} // namespace six_dof
} // namespace wsf

#endif // WSFSIXDOFROUTE_H
