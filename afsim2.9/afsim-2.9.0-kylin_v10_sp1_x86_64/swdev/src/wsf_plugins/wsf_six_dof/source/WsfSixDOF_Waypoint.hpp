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

#ifndef WSFSIXDOFWAYPOINT_HPP
#define WSFSIXDOFWAYPOINT_HPP

#include "wsf_six_dof_export.h"

#include <string>

#include "UtLLAPos.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT Waypoint
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
      // Default speed of 400 ktas
      eSpeedType type = SPD_TYPE_TAS_KNOTS;
      double     val  = 400.0;
   };

   // Structure containing a turn G type and value
   struct sTurnG
   {
      // Default 2-g turn (60 deg bank)
      eTurnGType type = TURN_G_TYPE_PILOT;
      double     val  = 2.0;
   };

   // Default constructor
   Waypoint() = default;

   // Constructor given a lat/lon/alt
   Waypoint(double aLat, double aLon, double aAlt);

   // Remove the assignment operator
   Waypoint& operator=(const Waypoint& other) = delete;

   // Destructor
   ~Waypoint() = default;

   // Return a deep copy of this waypoint
   Waypoint* Clone();

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
   Waypoint(const Waypoint& aSrc) = default;

   UtLLAPos    mPosition_lla;                 // Position in lat, lon, alt_m
   sSpeed      mSpeed;                        // Speed type/value
   sTurnG      mMaxG_g;                       // Maximum radial G to use when turning
   std::string mLabel;                        // Waypoint label
   std::string mGoTo;                         // Label of waypoint to travel to
   bool        mFollowHorizontalTrack = true; // Follow the horizontal track (or just head straight to waypoint)
   bool mFollowVerticalTrack = false; // Follow the vertical track (or use max vertical speed to climb to altitude)
   bool mWaypointOnPassing   = false; // Waypoint is achieved on passing (or on approach)
   int  mId                  = -1;
};
} // namespace six_dof
} // namespace wsf

#endif
