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

#ifndef WSFWAYPOINT_HPP
#define WSFWAYPOINT_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <memory>
#include <string>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtLog.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfAuxDataEnabled.hpp"
class WsfGeoPoint;
#include "WsfObject.hpp"
#include "WsfPath.hpp"
#include "WsfStringId.hpp"

//! A point on a path within a WsfRoute.
//! The use of waypoints (WsfWaypoint) within the context of routes (WsfRoute) is powerful and varied.  A minimalist
//! waypoint consists of a latitude, longitude, altitude, and possibly a speed that the associated platform
//! should have when intersecting it.  In addition to the basic capability, waypoints may be of other types
//! corresponding to specific needs.  The type of waypoint is stored in the attribute WsfWaypoint::mPointType.
//! In addition to having a (lat, lon, alt) designation, it can also be of 'offset' type, in which case the
//! coordinates represent a linear offset from a given reference, and at run-time the corresponding route is
//! given context in the world coordinate system by applying the WsfRoute::Transform method to it.  A waypoint
//! heading can be designated as either north- or body-relative; heading types absolute ('turn_to_heading') or
//! relative ('turn_right' or 'turn_left') must be combined with either a specified 'time' or 'distance' to the
//! next waypoint.  Waypoint altitudes can be specified as above mean sea level ('msl') or above ground level
//! ('agl').  If desired, a climb rate ('climb_rate') may be specified to select the rate of altitude change
//! when traveling between waypoints.  Target speeds and altitudes are maintained when crossing waypoints,
//! unless overridden with new values.  Linear and radial acceleration values may be specified to constrain
//! speed changes, and impose g-limits on turns.  Each waypoint may specify what behavior to use if that waypoint
//! is at the end of a path ('extrapolate', 'stop', or 'remove').  Waypoint transitions can be triggered either
//! 'switch_on_passing' (the default), or 'switch_on_approach', which uses an internally computed turn-radius to
//! begin the turn early.  If a waypoint is the first in a route, a 'pause_time' may specify a duration that must
//! elapse before movement begins.  A named scripted 'action' or 'execute' may be taken upon crossing a waypoint,
//! or an implementation-defined WsfMover 'mode' may change when a waypoint is crossed.  Consult the WSF User
//! Documentation for more details.
class WSF_EXPORT WsfWaypoint : public WsfAuxDataEnabled
{
public:
   //! Enumeration describing the type of WsfWaypoint indicated.
   enum
   {
      cLATITUDE_AND_LONGITUDE = 0x0001, //!< A (latitude, longitude) waypoint (altitude is optional).
      cRELATIVE_OFFSET        = 0x0002, //!< A WsfWaypoint specified as an (X,Y,Z) relative offset.
      cLOCATION_MASK          = 0x000F, //!< Utility value to mask off location commands.

      cABSOLUTE_HEADING = 0x0010, //!< A specified heading is absolute (north-relative).
      cRELATIVE_HEADING = 0x0020, //!< A specified heading is relative (usually body-relative).
      cHEADING_MASK     = 0x0030, //!< Utility value to mask off heading commands.

      cDISTANCE      = 0x0100, //!< Specifies a target elapsed distance value.
      cTIME          = 0x0200, //!< Specifies a target elapsed time value.
      cTIME_TO_POINT = 0x0400, //!< Specifies time duration to reach point.
      cDURATION_MASK = 0x0F00  //!< Utility value to mask off time and distance commands.
   };

   WsfWaypoint() = default;
   WsfWaypoint(double aLat, double aLon, double aAlt, double aSpeed = WsfPath::cUSE_PREVIOUS);
   WsfWaypoint(const WsfGeoPoint& aPoint, double aSpeed = WsfPath::cUSE_PREVIOUS);
   WsfWaypoint(const WsfWaypoint&) = default;
   WsfWaypoint(WsfWaypoint&&)      = default;
   WsfWaypoint& operator=(const WsfWaypoint&) = default;
   WsfWaypoint& operator=(WsfWaypoint&&) = default;
   ~WsfWaypoint() override               = default;

   WsfWaypoint* Clone() const;
   bool         ProcessInput(UtInput& aInput);
   bool         ProcessAllInput(UtInput& aInput);

   //! Determine whether two waypoints have exactly the same position (lat, lon, alt).
   //! @return 'true' if the two waypoints have exactly the same position; 'false' otherwise.
   bool operator==(const WsfWaypoint& aRhs) const
   {
      return ((mLat == aRhs.mLat) && (mLon == aRhs.mLon) && (mAlt == aRhs.mAlt));
   }

   //! Determine whether two waypoints do not have the same position (lat, lon, alt).
   //! @return 'false' if the two waypoints have exactly the same position; 'true' otherwise.
   bool operator!=(const WsfWaypoint& aRhs) const { return !(operator==(aRhs)); }

   void ExtrapolateAlongHeading(double aHeading);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void GreatCircleHeadingAndDistance(const WsfWaypoint& aWaypoint, double& aHeading, double& aDistance) const;

   void Print(ut::log::MessageStream& aStream) const;

   void       SetCallback(WsfObject* aCallbackPtr);
   WsfObject* GetCallback() const;

   double GetPauseTime() const { return mPauseTime; }
   void   SetPauseTime(double aPauseTime) { mPauseTime = aPauseTime; }

   WsfStringId GetLabelId() const { return mLabelId; }
   void        SetLabelId(WsfStringId aLabelId) { mLabelId = aLabelId; }

   WsfStringId GetScriptId() const { return mScriptId; }
   void        SetScriptId(WsfStringId aScriptId) { mScriptId = aScriptId; }

   WsfStringId GetGotoId() const { return mGotoId; }
   void        SetGotoId(WsfStringId aGotoId) { mGotoId = aGotoId; }

   WsfStringId GetNewModeId() const { return mNewModeId; }
   void        SetNewModeId(WsfStringId aNewModeId) { mNewModeId = aNewModeId; }

   int  GetPointType() const { return mPointType; }
   void SetPointType(int aPointType) { mPointType = aPointType; }

   double GetLat() const { return mLat; }
   void   SetLat(double aLat) { mLat = aLat; }

   double GetLon() const { return mLon; }
   void   SetLon(double aLon) { mLon = aLon; }

   double GetAlt() const { return mAlt; }
   void   SetAlt(double aAlt) { mAlt = aAlt; }

   void GetLocationLLA(double& aLat, double& aLon, double& aAlt) const;
   void SetLocationLLA(double aLat, double aLon, double aAlt);

   double GetHeading() const { return mHeading; }
   void   SetHeading(double aHeading) { mHeading = aHeading; }

   double GetLinearAccel() const { return mLinearAccel; }
   void   SetLinearAccel(double aLinearAccel) { mLinearAccel = aLinearAccel; }

   double GetRadialAccel() const { return mRadialAccel; }
   void   SetRadialAccel(double aRadialAccel) { mRadialAccel = aRadialAccel; }

   double GetClimbRate() const { return mClimbRate; }
   void   SetClimbRate(double aClimbRate) { mClimbRate = aClimbRate; }

   double GetMaximumFlightPathAngle() const { return mMaximumFlightPathAngle; }
   void SetMaximumFlightPathAngle(double aMaximumFlightPathAngle) { mMaximumFlightPathAngle = aMaximumFlightPathAngle; }

   void   SetSpeed(double aSpeed) { mSpeed = aSpeed; }
   double GetSpeed() const { return mSpeed; }

   double GetDistanceAlongRoute() const { return mDistanceAlongRoute; }
   void   SetDistanceAlongRoute(double aDistanceAlongRoute) { mDistanceAlongRoute = aDistanceAlongRoute; }

   double GetDistanceOrTime() const { return mDistanceOrTime; }
   void   SetDistanceOrTime(double aDistanceOrTime) { mDistanceOrTime = aDistanceOrTime; }

   WsfPath::AltRef GetAltRef() const { return mAltRef; }
   void            SetAltRef(WsfPath::AltRef aAltRef) { mAltRef = aAltRef; }

   WsfPath::EndPath GetEndOfPathOption() const { return mEndOfPathOption; }
   void             SetEndOfPathOption(WsfPath::EndPath aEndOfPathOption) { mEndOfPathOption = aEndOfPathOption; }

   WsfPath::TurnDirection GetTurnDirection() const { return mTurnDirection; }
   void                   SetTurnDirection(WsfPath::TurnDirection aTurnDirection) { mTurnDirection = aTurnDirection; }

   WsfPath::Switch GetSwitch() const { return mSwitch; }
   void            SetSwitch(WsfPath::Switch aSwitch) { mSwitch = aSwitch; }

   unsigned int GetRouteId() const { return mRouteId; }
   void         SetRouteId(unsigned int aRouteId) { mRouteId = aRouteId; }

   WsfStringId GetNodeId() const { return mNodeId; }
   void        SetNodeId(WsfStringId aNodeId) { mNodeId = aNodeId; }

   unsigned int GetNodeIdNumber() const { return mNodeIdNumber; }
   void         SetNodeIdNumber(unsigned int aNodeIdNumber) { mNodeIdNumber = aNodeIdNumber; }

   unsigned int GetPositionInRoute() const { return mPositionInRoute; }
   void         SetPositionInRoute(unsigned int aPositionInRoute) { mPositionInRoute = aPositionInRoute; }

   // ================================= WARNING: New member variables ===========================
   //
   //   This class is transmitted over the network.  If you add a new member variable, it is
   //   YOUR RESPONSIBILITY to verify that the member is serialized correctly.
   //   Please see the implementation of WsfWaypoint::SerializeP in xio/WsfXIO_SerializeTypes.cpp
   //
   // ===========================================================================================

   //! For XIO (de)serialization.
   template<typename T>
   void SerializeP(T& aBuff);

private:
   //! A pointer to a callback object associated with this waypoint.
   ut::CloneablePtr<WsfObject> mCallbackPtr;

   //! The point type, which is one of the enumerated values above, or some combination thereof (see notes):
   //! @note cABSOLUTE_HEADING, cRELATIVE_HEADING, cALTITUDE, and cSPEED must be combined with either
   //! cDISTANCE or cTIME.  For example, combining absolute heading with time for a waypoint called myWaypoint'
   //! is accomplished as follows:
   //! myWaypoint.mPointType = WsfWaypoint::cABSOLUTE_HEADING  |  WsfWaypoint::cTIME;
   //! @note See documentation of mLat, mLon, mAlt, mHeading, and mDistanceOrTime for an explanation of the type-
   //! dependent meanings of the waypoint data.
   int mPointType = cLATITUDE_AND_LONGITUDE;

   //! The latitude of the waypoint (degrees, range [-90, 90]), for points of type cLATITUDE_AND_LONGITUDE
   //! The relative X position (in the direction of movement), for points of type cRELATIVE_OFFSET.
   double mLat = 0.0;

   //! The longitude of the waypoint (degrees, range [-180, 180], for points of type cLATITUDE_AND_LONGITUDE
   //! The relative Y position (to the right of the direction of movement), for points of type cRELATIVE_OFFSET.
   double mLon = 0.0;

   //! The altitude of the waypoint above the ellipsoid.
   //! @note For the ground mover (WsfGroundMover) this is the above ground
   //! level.  For an air mover (WsfAirMover) it is above mean sea level.
   //! For either of these movers, the altitude will be interpreted as indicated,
   //! regardless of the value of WsfWaypoint::mAltRef.
   double mAlt = WsfPath::cUSE_PREVIOUS;

   //! The time (seconds) or distance (meters) to travel for a waypoint that is of type
   //! cDISTANCE or cTIME.
   double mDistanceOrTime = 0.0;

   //! The waypoint's distance in meters from the start of the route
   double mDistanceAlongRoute = 0.0;

   //! The heading in radians clockwise from north.
   //! This attribute can be used to assign a heading where there one and only one waypoint
   //! (such as defining the pointing angle for a ground site).
   //!
   //! @note If the point is a cRELATIVE_HEADING then this is the amount
   //! of the heading change (+ for right, - for left).
   double mHeading = WsfPath::cUSE_PREVIOUS;

   //! The speed (in meters/seconds) at arrival to this waypoint.
   double mSpeed = WsfPath::cUSE_PREVIOUS;

   //! The linear acceleration (in meters/second^2) to be used to accelerate
   //! the speed to the next waypoint.
   double mLinearAccel = WsfPath::cUSE_PREVIOUS;

   //! The radial acceleration (in meters/second^2) to be used when performing
   //! a turn towards the next waypoint.
   double mRadialAccel = WsfPath::cUSE_PREVIOUS;

   //! The climb rate (in meters/second) to be used to rise to the next waypoint altitude.
   //! (Absolute value, will be negated to descend in altitude.)
   double mClimbRate = WsfPath::cUSE_PREVIOUS;

   //! The maximum angle at which a climb or dive will be performed.
   double mMaximumFlightPathAngle = WsfPath::cUSE_PREVIOUS;

   //! The time to pause (in seconds) upon reaching this waypoint before
   //! starting on the path to the next waypoint.
   double mPauseTime = 0.0;

   //! The string ID of the label associated with this waypoint.
   //! If non-zero then a 'goto' to this point can occur.
   WsfStringId mLabelId = nullptr;

   //! The string ID of the script action associated with this waypoint.
   //! @note Currently there is only one script associated with a given waypoint
   //! and it is up to the code processing this waypoint to determine when and
   //! how to initiate a script based on this identifier.
   WsfStringId mScriptId = nullptr;

   //! The string ID of the label to goto when this point is reached.
   //! If the value is zero, then there is no goto.
   WsfStringId mGotoId = nullptr;

   //! The dictionary ID of the Mover Mode to switch to when this point is reached.
   //! If the value is zero, then the mode remains unchanged.
   WsfStringId mNewModeId = nullptr;

   //! The altitude reference identifier, which can be one of the following:
   //! cALT_REF_MOVER_DEFAULT, (Use the WsfMover altitude reference)
   //! cALT_REF_MSL, (above Mean Sea Level)
   //! cALT_REF_AGL  (Above Ground Level)
   WsfPath::AltRef mAltRef = WsfPath::cALT_REF_MOVER_DEFAULT;

   //! what the mover will do when this waypoint is reached and there are no more waypoints following.
   //! The end of path options, which can be one of the following:
   //! If 'cEP_MOVER_DEFAULT' then the mover decides the option.
   //! If 'cEP_EXTRAPOLATE' then the mover will continue to extrapolate the position
   //! along the current course.
   //! If 'cEP_STOP' then the mover will stop updating the entity position.
   //! If 'cEP_REMOVE' then the platform will be removed from the simulation.
   //! The default value is 'cEP_MOVER_DEFAULT'.
   WsfPath::EndPath mEndOfPathOption = WsfPath::cEP_MOVER_DEFAULT;

   //! The turn direction to be used when turning to the target point or heading.
   WsfPath::TurnDirection mTurnDirection = WsfPath::cTURN_DIR_SHORTEST;

   //! The criteria to be used when switching TO this waypoint as a destination.
   WsfPath::Switch mSwitch = WsfPath::cSWITCH_MOVER_DEFAULT;

   //! @name WsfRouteNetwork associated values
   //! The following three values are applicable only for WsfRouteNetwork.
   //@{

   //! The route ID to which this waypoint belongs (if any).
   unsigned int mRouteId = 0;

   //! The string ID of the waypoint's node within a route network.
   //! If the value is > 0 then it is the string ID of the node name.
   //! If the value is < 0, then it is an end-node (the first or last
   //! point in a route and is not marked as an intersection).
   //! @note A node is either the intersection of two routes or either of the endpoints.
   WsfStringId mNodeId = nullptr;

   unsigned int mNodeIdNumber = 0;

   //! The offset(index) of the waypoint from the start of the route (zero being the first).
   unsigned int mPositionInRoute = 0;
   //@}
};

#endif
