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

#ifndef WSFPATH_HPP
#define WSFPATH_HPP

#include "wsf_export.h"

#include "UtMath.hpp"

//! A base namespace used by almost all WsfMover classes.
//! This namespace contains a collection of enumerations used to set the path of a
//! WsfMover guided by WsfWayPoints.  The enumerations were placed in a namespace,
//! as a base for other classes which will use them.
namespace WsfPath
{
//! Unique value, tested internally to see if a floating point value is set or defaulted.
constexpr double cDOUBLE_NOT_SET = -1.0E-12;

//! A waypoint parameter with this value is a request to use the previously used value. (i.e.: the current speed)
constexpr double cUSE_PREVIOUS = WsfPath::cDOUBLE_NOT_SET;

//! A waypoint parameter with this value is a request to use the mover's default value. (i.e.: maximum_radial_acceleration)
constexpr double cUSE_DEFAULT = -1.0E-13;

//! A waypoint parameter with this value is a request to use a computed value to reach the target with a constant rate
//! (climb_rate, acceleration ...)
constexpr double cUSE_COMPUTED = -1.0E-14;

//! Speed below which an object is not considered to be moving:
constexpr double cNEGLIGIBLE_SPEED = 5280.0 / 3600.0 * UtMath::cM_PER_FT; // 1 mph to m/s

//! Heading error, below which we are considered to be on course (not turning).
constexpr double cHEADING_FUZZ = 5.0 * UtMath::cRAD_PER_DEG;

//! Height delta, above which we are considered airborne.
constexpr double cGROUND_PROXIMITY_FUZZ = 4.0; // meters above ground

//! Altitude reference option
enum AltRef
{
   cALT_REF_MOVER_DEFAULT, //!< Use the WsfMover altitude reference
   cALT_REF_MSL,           //!< Altitude is referenced to MSL
   cALT_REF_AGL            //!< Altitude is referenced to the terrain height
};

//! Heading reference option
enum HdgRef
{
   cHDG_REF_NORTH,   //!< North-relative
   cHDG_REF_RELATIVE //!< Implementation-defined (usually body-relative)
};

//! End of-path option
enum EndPath
{
   cEP_MOVER_DEFAULT, //!< Use the WsfMover end-of-path option
   cEP_EXTRAPOLATE,   //!< Extrapolate at end-of-path
   cEP_STOP,          //!< Stop at end-of-path
   cEP_REMOVE         //!< Remove the platform at end-of-path
};

//! Option setting when to react as a WsfWaypoint is approached.
enum Switch
{
   cSWITCH_MOVER_DEFAULT, //!< Use the WsfMover switch setting.
   cSWITCH_ON_TIME,       //!< Switch when a specified time since the last WsfWaypoint has elapsed.
   cSWITCH_ON_DISTANCE,   //!< Switch when a specified distance since last WsfWaypoint has passed.
   cSWITCH_ON_APPROACH,   //!< Switch when approaching within one turn radius of the WsfWaypoint.
   cSWITCH_ON_PASSING,    //!< Switch only when abreast of the WsfWaypoint (DEFAULT).
   cSWITCH_EXTERNAL       //!< Ignore switching internally, as it is done externally
   //!< via software commands, or in script, or the path is being extrapolated.
};

//! Turn direction for TurnToHeading and TurnToRelativeHeading.
enum TurnDirection
{
   cTURN_DIR_LEFT     = -1, //!< Turn to the left, even if right is closer.
   cTURN_DIR_SHORTEST = 0,  //!< Turn in the direction that results in the shortest turn (DEFAULT).
   cTURN_DIR_RIGHT    = 1   //!< Turn to the right, even if left is closer.
};

WSF_EXPORT AltRef GetAltRef(AltRef aMoverDefault, AltRef aWaypointValue);

}; // namespace WsfPath

#endif
