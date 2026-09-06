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

#ifndef DISDATUMENUMS_HPP
#define DISDATUMENUMS_HPP

namespace DisEnum
{
namespace DatumTag
{
enum
{ // Representation (if known) below:
   IDENTIFICATION        = 10000,
   ENTITY_TYPE           = 11000,
   WAYPOINT_TYPE         = 31010,
   LATITUDE              = 31300,
   LONGITUDE             = 31400,
   ALTITUDE              = 31600,
   VELOCITY_X            = 36100,
   VELOCITY_Y            = 36200,
   VELOCITY_Z            = 36300,
   SPARE                 = 41001,
   MISSION_ID            = 42100,
   MISSION_TYPE          = 42200,
   MISSION_REQUEST_TIME  = 42300,
   EXERCISE_NAME         = 43100,
   ENTITIES              = 43200,
   APPLICATION_RATE      = 45000, // 32 bit floating point
   APPLICATION_TIME      = 45005,
   APPLICATION_TIME_STEP = 45010, // 32 bit unsigned int (Absolute LSB=1, Relative LSB=0)
   FEEDBACK_TIME         = 45020, // 32 bit unsigned int (Absolute LSB=1, Relative LSB=0)
   SIMULATION_RATE       = 45030, // 32-bit floating point
   SIMULATION_TIME       = 45040, // Clock Record = 64 bits (32 int = hours past (+/-), 32 uint = time past hour)
   SIMULATION_TIME_STEP  = 45050, // 32 bit unsigned int (Absolute LSB=1, Relative LSB=0)
   TIME_INTERVAL         = 45060, // 32 bit unsigned int (Absolute LSB=1, Relative LSB=0)
   TIME_LATENCY          = 45070, // 32 bit unsigned int (Absolute LSB=1, Relative LSB=0)
   TIME_SCHEME           = 45080, // 32 bit unsigned int
   TIME                  = 52000,
   TIME_HHMMSS           = 52001,
   DATE_MMDDYYYY         = 52602,
   JAMMING_STATUS        = 60400,
   JAMMER_RANGE          = 60401
};
}
} // namespace DisEnum

#endif
