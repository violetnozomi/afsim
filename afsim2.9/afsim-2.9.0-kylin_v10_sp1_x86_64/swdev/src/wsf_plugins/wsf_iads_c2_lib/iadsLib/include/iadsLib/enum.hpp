// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   enum.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 *
 */

#ifndef IADSLIB_ENUM_HPP
#define IADSLIB_ENUM_HPP

#include <string>
#include "iadslib_export.h"

namespace il
{

#define INVALID_UNIT_ID   -1

#define INVALID_UNIT_TYPE -1
#define INVALID_UNIT_SUBTYPE INVALID_UNIT_TYPE

#define GTIMTRACK_DROP 4
#define GTIMTRACK_PLOT 5
#define GTIMTRACK_POSSIBLE 6
#define GTIMTRACK_TENTATIVE 7
#define GTIMTRACK_FIRM 8
#define GTIMTRACK_CUE 9


const std::string C_EW_SENSOR_TYPE_STR = "EW";
const std::string C_TAR_SENSOR_TYPE_STR = "TAR";
const std::string C_TTR_SENSOR_TYPE_STR = "TTR";
const std::string C_RWR_SENSOR_TYPE_STR = "RWR";
const std::string C_UNKNOWN_SENSOR_TYPE_STR = "Unknown";

enum IADSLIB_EXPORT TRACK_UPDATE_STATUS
{
   UPDATE_NORMAL    = 0,
   UPDATE_COASTING  = 1,
   UPDATE_TIMED_OUT = 2,
   UPDATE_DROPPING  = 3,
   UPDATE_DROPPED   = 4
};

enum IADSLIB_EXPORT eIFFType
{
   E_IFF_UNKNOWN  = 0,
   E_IFF_NEUTRAL  = 1,
   E_IFF_FRIENDLY = 2,
   E_IFF_HOSTILE  = 3
};

/* Values for SAM EngagementStatus */

#define GTIMMSG_ASSIGN 0
#define GTIMMSG_CANCEL 1
#define GTIMMSG_REASSIGN 5
//#define GTIMMSG_WILCO 10
//#define GTIMMSG_CANTCO 15
#define GTIMMSG_CUE_TAR 20
#define GTIMMSG_CUE_TTR 22
#define GTIMMSG_TAR_SEARCHING 24
#define GTIMMSG_TTR_SEARCHING 26
#define GTIMMSG_TAR_TRACKING 30
#define GTIMMSG_TTR_TRACKING 40
//#define GTIMMSG_FIRING 50
//#define GTIMMSG_KILL 60
#define GTIMMSG_DAMAGE 70
//#define GTIMMSG_MISS 80
#define GTIMMSG_HAVCO 90

/* SYSTEMIC CANTCO Reasons */

#define GTIMMSG_CANTCO_NO_SELF_STATUS 1
#define GTIMMSG_CANTCO_NO_PREPPED_MISSILES 2
#define GTIMMSG_CANTCO_NO_WEAPON_PREFERENCE 3
#define GTIMMSG_CANTCO_NO_MUNITIONS 4
#define GTIMMSG_CANTCO_LAUNCH_FAILED 5

/* SITUATIONAL CANTCO Reasons */

#define GTIMMSG_CANTCO_SITUATIONAL 100
#define GTIMMSG_CANTCO_NO_TTR_ACQUIRE 101
#define GTIMMSG_CANTCO_NO_TAR_ACQUIRE 102
#define GTIMMSG_CANTCO_NO_INTERCEPT 103
#define GTIMMSG_CANTCO_NO_MASTER_TRACK 104
#define GTIMMSG_CANTCO_NO_ASSIGNABLE_UNIT 105
#define GTIMMSG_CANTCO_INVALID_TRACK 106
#define GTIMMSG_CANTCO_NO_READY_ASSIGNMENTS 107

/* Shot Doctrine */

#define GTIMTRACK_SHOOT_1 1
#define GTIMTRACK_SHOOT_2 2
#define GTIMTRACK_SHOOT_LOOK_SHOOT 3

/* Additional Values for AI EngagementStatus */

#define GTIMMSG_SCRAMBLE 120


const double cDEFAULT_YELLOW_AGEOUT_TIME_S = 15; // yellow after seconds without status
const double cDEFAULT_RED_AGEOUT_TIME_S    = 30; // red after seconds without status
const double cDEFAULT_REPORT_POSITION_EVERY_METERS = 100;
const double cDEFAULT_REPORT_POSITION_EVERY_SECONDS = 300;
const double cDEFAULT_REPORT_STATUS_EVERY_SECONDS = 10;
const bool cDEFAULT_AGGREGATE_UNIT_STATUS = false;
const bool cDEFAULT_STATUS_STATIONARY_OPNS_ONLY = false;
const bool cDEFAULT_STATUS_WEAPON_REQUIRED = false;
const bool cDEFAULT_STATUS_EW_REQUIRED = false;
const bool cDEFAULT_STATUS_TAR_REQUIRED = false;
const bool cDEFAULT_STATUS_TTR_REQUIRED = false;
const double cDEFAULT_TRACK_PROJECTION_TIME = 60.0;
const double cDEFAULT_TRACK_PROJECTION_INCR = 10.0;

enum IADSLIB_EXPORT eZoneType
{
   E_ZONE_TYPE_OTHER = 0x01 << 0,
   E_ZONE_TYPE_AOR   = 0x01 << 1,
   E_ZONE_TYPE_SUR   = 0x01 << 2,
   E_ZONE_TYPE_MEZ   = 0x01 << 3,
   E_ZONE_TYPE_FEZ   = 0x01 << 4,
   E_ZONE_TYPE_JEZ   = 0x01 << 5,
   E_ZONE_TYPE_GEZ   = 0x01 << 6,
   E_ZONE_TYPE_DA    = 0x01 << 7,
   E_ZONE_TYPE_COR   = 0x01 << 8,
};

typedef unsigned int tZoneTypeMask; // a mask of the above eZoneType values
#define E_NO_ZONES_SET  0

typedef double tZonePriority;

enum IADSLIB_EXPORT eTakeAction
{
   STRAIGHT,
   BREAK_LEFT,
   BREAK_RIGHT
};

enum IADSLIB_EXPORT eAssessEngage
{
   COLLISION_INTERCEPT_PT_INSIDE_ZONE,
   MUNITION_INTERCEPT_PT_INSIDE_ZONE,
   MUNITION_INTERCEPT_PT_IGNORE_ZONE
};

enum IADSLIB_EXPORT eRWRResponsePriority
{
   DEFAULT,
   CHOOSE_CLOSEST_THREAT
};

#define GTI_SUCCESS  true
#define GTI_FAILURE  false

enum IADSLIB_EXPORT eShotDoctrine
{
   SHOOT_1,
   SHOOT_2,
   SHOOT_LOOK_SHOOT
};

enum NOID
{
   NO_ID = 0
};

enum IADSLIB_EXPORT SYSTEM_STATUS
{
   STATUS_GREEN  = 0,
   STATUS_YELLOW = 1,
   STATUS_WHITE  = 2,
   STATUS_RED    = 3
};

std::string systemStatusToString(SYSTEM_STATUS statusEnum);

enum IADSLIB_EXPORT AssetTypeEnum
{
   WEAPON_OTHER     = 0,
   WEAPON_AI        = 1,
   WEAPON_SAM       = 2,
   C2_OTHER         = 3
};


// ********** Type enumerations **********
enum UnitTypeEnum
{
   UNKNOWN_TYPE          = 0,
   FIGHTER               = 1,
   STRIKE                = 2,
   BOMBER                = 3,
   CARGO_TANKER          = 4,
   OBSERVATION           = 5,
   EW                    = 6,
   RECON                 = 7,
   AEW                   = 8,

   SEPARATING            = 9,
   RADAR                 = 10
};


// ********** Subtype enumerations **********
enum FighterEnum
{
   UNKNOWN_FIGHTER = 0,
   F14             = 2,
   F16             = 3,
   F4_FIGHTER      = 4,
   F15             = 5,
   F22             = 6,
   F18             = 9,
   JSF             = 12
};

enum StrikeEnum
{
   UNKNOWN_STRIKE = 0,
   A4             = 1,
   A6             = 2,
   AV8B           = 8,
   F4_STRIKE      = 10,
   F117A          = 11,
   F111           = 12
};

enum BomberEnum
{
   UNKNOWN_BOMBER = 0,
   B1             = 1,
   B2             = 2,
   B52            = 3
};

enum CargoTankerEnum
{
   UNKNOWN_CARGO = 0
};

enum ObservationEnum
{
   UNKNOWN_OBSERVATION = 0
};

enum EWEnum
{
   UNKNOWN_EW = 0
};

enum ReconEnum
{
   UNKNOWN_RECON = 0,
   SR71          = 1,
   U2            = 3,
   F4_RECON      = 4,
   RC135         = 7
};

enum AEWEnum
{
   UNKNOWN_AEW = 0,
   E2          = 1,
   EA3         = 2,
   E6A         = 3,
   E3          = 4,
   E4          = 5,
   E8          = 6
};

enum IADSLIB_EXPORT SensorEnum
{
   SENSOR_UNKNOWN	= 0,
   SENSOR_RWR     = 1,
   SENSOR_EW      = 2,
   SENSOR_TAR		= 3,
   SENSOR_TTR		= 4
};

enum IADSLIB_EXPORT AIBehaviorEnum
{
   EVADE          = 0,
   TAKE_ACTION    = 1,
   RETURN_TO_BASE = 2,
   EGRESS         = 3,
   INTERCEPT      = 4,
   PURSUE         = 5
};
}

#endif
