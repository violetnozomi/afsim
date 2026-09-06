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

#ifndef P6DOFWAYPOINT_HPP
#define P6DOFWAYPOINT_HPP

#include "p6dof_export.h"

#include <iostream>
#include <string>

#include "UtLLAPos.hpp"

class P6DOF_EXPORT P6DofWaypoint
{
public:
   // This enum specifies the speed "units"
   enum eSpeedType
   {
      SPD_TYPE_MACH,      // Mach
      SPD_TYPE_TAS_KNOTS, // Knots True Air Speed (KTAS)
      SPD_TYPE_CAS_KNOTS, // Knots Calibrated Air Speed (KCAS/KIAS)
      SPD_TYPE_FPS,       // Feet/second (fps)
      SPD_TYPE_MPH,       // Miles/hour (mph)
      SPD_TYPE_MPS        // Meters/second (mps)
   };

   // This enum specifies the type of g-load specified for a turn.
   enum eTurnGType
   {
      TURN_G_TYPE_LATERAL, // G-load in the lateral plane
      TURN_G_TYPE_PILOT
   };

   // Structure containing a speed type and value
   struct sSpeed
   {
      eSpeedType type;
      double     val;
   };

   // Structure containing a turn G type and value
   struct sTurnG
   {
      eTurnGType type;
      double     val;
   };

   // Default constructor
   P6DofWaypoint();

   // Constructor given a lat/lon/alt
   P6DofWaypoint(double aLat, double aLon, double aAlt);

   // Remove the assignment operator
   P6DofWaypoint& operator=(const P6DofWaypoint& other) = delete;

   // Destructor
   ~P6DofWaypoint() = default;

   // Return a deep copy of this waypoint
   P6DofWaypoint* Clone();

   // Set Methods

   // Functions to set current waypoint parameters
   void SetLLA(const UtLLAPos& aPosLLA);

   // Set whether or not the horizontal track should be followed
   void SetFollowHorizontalTrack(bool aCommand);

   // Set whether or not the vertical track should be followed
   void SetFollowVerticalTrack(bool aCommand);

   // Set whether or not the waypoint is achieved on passing
   void SetWaypointOnPassing(bool aCommand);

   // Set the speed given an sSpeed struct
   void SetSpeed(sSpeed aSpeed);

   // Set the speed given a type and a value
   void SetSpeed(eSpeedType aType, double aValue);

   // Set the maximum turn g
   void SetMaxTurnG(eTurnGType type, double aTurnG);

   // Set the waypoint label
   void SetLabel(const std::string& aLabel);

   // Set a GoTo to another waypoint
   void SetGoTo(const std::string& aLabel);

   // Numerical id value
   void SetId(int aId);

   // Get Methods
   // Return the lat/lon/alt position of this waypoint
   UtLLAPos GetLLA() const;

   // Return the speed at this waypoint
   sSpeed GetSpeed() const;

   // Return the maximum turn G at this waypoint
   sTurnG GetMaxTurnG() const;

   // Return the label of this waypoint
   std::string GetLabel() const;

   // Return the GoTo of this waypoint
   std::string GetGoTo() const;

   // Return whether or not the horizontal track is to be followed
   bool FollowHorizontalTrack() const;

   // Return whether or not the vertical track is to be followed
   bool FollowVerticalTrack() const;

   // Return whether or not the waypoint is achieved on passing
   bool WaypointOnPassing() const;

   int GetId() const;

private:
   // Copy constructor
   P6DofWaypoint(const P6DofWaypoint& aSrc) = default;

   UtLLAPos    mPosition_lla;          // Position in lat, lon, alt_m
   sSpeed      mSpeed;                 // Speed type/value
   sTurnG      mMaxG_g;                // Maximum radial G to use when turning
   std::string mLabel;                 // Waypoint label
   std::string mGoTo;                  // Label of waypoint to travel to
   bool        mFollowHorizontalTrack; // Follow the horizontal track (or just head straight to waypoint)
   bool        mFollowVerticalTrack;   // Follow the vertical track (or use max vertical speed to climb to altitude)
   bool        mWaypointOnPassing;     // Waypoint is achieved on passing (or on approach)
   int         mId;
};

#endif
