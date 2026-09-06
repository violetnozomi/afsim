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

#ifndef DISDATUMSPEC_HPP
#define DISDATUMSPEC_HPP

#include "dis_export.h"

#include <ostream>
#include <vector>

#include "DisFixedDatum.hpp"
#include "DisTypes.hpp"
#include "DisVariableDatum.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDatumSpec
{
public:
   DisDatumSpec();
   ~DisDatumSpec();

   // Allow compiler to build copy constructor and operator=
   // DisDatumSpec(const DisDatumSpec& aSrc);
   // DisDatumSpec& operator=(const DisDatumSpec& aRhs);
   // Enumerations for Datum ID in the Datum Specification Record
   // (EBV-DOC 7.1)
   // Called Variable Record Types [UID 66] in the SISO Standard
   enum
   {
      UpiD_IDENTIFICATION                  = 10000,
      UpiD_ENTITY_TYPE                     = 11000,
      UpId_OWNERSHIP_STATUS                = 15800,
      UpID_FUEL                            = 23000,
      UpiD_GIMBAL_YAW                      = 26000,
      UpiD_GIMBAL_PITCH                    = 26001,
      UpiD_FREE_TRACK                      = 26002,
      UpiD_CAGE                            = 26003,
      UpiD_SLAVE_TRACK                     = 26004,
      UpiD_SCAN_RAIL                       = 26005,
      UpiD_TRACKING                        = 26010,
      UpiD_LOST_TRACK_ENUM                 = 26015,
      UpiD_PK_ADJUSTMENT                   = 26020,
      UpiD_WEAPON_TRACK_NUMBER             = 28000,
      UpiD_INITIAL_TARGET_TRACK_NUMBER     = 28001,
      UpiD_CONTROLLER_TRACK_NUMBER         = 28002,
      UpiD_ATTACK_MODE                     = 28003,
      UpiD_INITIAL_TARGET_INDEX_ORIGINATOR = 28005,
      UpiD_INITIAL_TARGET_INDEX            = 28006,
      UpiD_TARGET_LATITUDE                 = 28100,
      UpiD_TARGET_LONGITUDE                = 28101,
      UpiD_TARGET_ALTITUDE                 = 28102,
      UpiD_TARGET_VELOCITY_NORTH           = 28103,
      UpiD_TARGET_VELOCITY_EAST            = 28104,
      UpiD_TARGET_VELOCITY_DOWN            = 28105,
      UpiD_WAYPOINT_LATITUDE               = 28106,
      UpiD_WAYPOINT_LONGITUDE              = 28107,
      UpiD_WAYPOINT_ALTITUDE               = 28108,
      UpiD_TARGET_SITE                     = 28109,
      UpiD_TARGET_APP                      = 28110,
      UpiD_TARGET_ENTITY                   = 28111,
      UpiD_WEAPON_ALLOC_FUNCTION           = 28200,
      UpiD_LAUNCH_PLATFORM_SITE            = 28201,
      UpiD_LAUNCH_PLATFORM_APP             = 28202,
      UpiD_LAUNCH_PLATFORM_ENTITY          = 28203,
      UpiD_WEAPON_SITE                     = 28204,
      UpiD_WEAPON_APP                      = 28205,
      UpiD_WEAPON_ENTITY                   = 28206,
      UpiD_WEAPON_KIND                     = 28207,
      UpiD_WEAPON_DOMAIN                   = 28208,
      UpiD_WEAPON_COUNTRY                  = 28209,
      UpiD_WEAPON_CATEGORY                 = 28210,
      UpiD_WEAPON_SUBCATEGORY              = 28211,
      UpiD_WEAPON_SPECIFIC                 = 28212,
      UpiD_WEAPON_EXTRA                    = 28213,
      UpiD_EXTERNAL_SEEKER                 = 28220,
      UpiD_WAYPOINT_TYPE                   = 31010,
      UpiD_LATITUDE                        = 31300,
      UpiD_LONGITUDE                       = 31400,
      UpiD_ALTITUDE                        = 31600,
      UpiD_VELOCITY_X                      = 36100,
      UpiD_VELOCITY_Y                      = 36200,
      UpiD_VELOCITY_Z                      = 36300,
      UpiD_SPARE                           = 41001,
      UpiD_MISSION_ID                      = 42100,
      UpiD_MISSION_TYPE                    = 42200,
      UpiD_ENTITIES                        = 43200,
      UpiD_APPLICATION_RATE                = 45000,
      UpiD_APPLICATION_TIME                = 45005,
      UpiD_APPLICATION_TIMESTEP            = 45010,
      UpiD_FEEDBACK_TIME                   = 45020,
      UpiD_SIMULATION_RATE                 = 45030,
      UpiD_SIMULATION_TIME                 = 45040,
      UpiD_SIMULATION_TIMESTEP             = 45050,
      UpiD_TIME_LATENCY                    = 45070,
      UpiD_TIME_SCHEME                     = 45080,
      UpiD_TIME                            = 52000,
      UpiD_TIME_HHMMSS                     = 52001,
      UpiD_DATE_MMDDYYYY                   = 52602,
      UpiD_CYBER_DEVICE                    = 60201,
      UpiD_COLLECTION                      = 60202,
      UpiD_ROLE                            = 60203,
      UpiD_PERMISSION                      = 60204,
      UpiD_STATION                         = 60205,
      UpiD_TEAM                            = 60206,
      UpiD_ATTACK_ACTION_REPORT            = 60301,
      UpiD_DETECTION_REPORT                = 60302,
      UpiD_REGISTER_ROLE                   = 60303,
      UpiD_DEREGISTER_ROLE                 = 60304,
      UpiD_REPORT_ROLE_REGISTRATION        = 60305,
      UpiD_REPORT_ROLE_DEREGISTRATION      = 60306,
      UpiD_CREATE_ROLE                     = 60307,
      UpiD_DELETE_ROLE                     = 60308,
      UpiD_CREATE_TEAM                     = 60309,
      UpiD_DELETE_TEAM                     = 60310,
      UpiD_ATTACK_ACTION                   = 60311,
      UpiD_PERMISSION_CHANGE_REQUEST       = 60312,
      UpiD_RECOVERY_ACTION                 = 60313,
      UpiD_REFRESH_REQUEST                 = 60314,
      UpiD_RECOVERY_ACTION_REPORT          = 60315,
      UpiD_FUEL_QUANTITY                   = 240043,
      UpiD_AIR_SPEED                       = 240054,
      UpiD_TRUE_ALTITUDE                   = 240055,
      UpiD_TURN_TYPE                       = 240056,
      UpiD_CLIMB_DESCENT_TYPE              = 240057,
      UpiD_STALL_STATUS                    = 240058,
      UpiD_PERCENT_STALL                   = 240059,
      UpiD_INTERNAL_STALL_RECOVERY         = 240060,
      UpiD_TRUE_HEADING                    = 240066,
      UpiD_BANK_ANGLE                      = 240070,
      UpiD_HEADING_COMMAND_MODE            = 240075,
      UpiD_PITCH_ANGLE                     = 240080,
      UpiD_ALTITUDE_COMMAND_MODE           = 240085,
      UpiD_STICK_AND_THROTTLE_COMMAND      = 240090,
      UpiD_STICK_X_POSITION                = 240091,
      UpiD_STICK_Y_POSITION                = 240092,
      UpiD_LEFT_THROTTLE_RANGE             = 240093,
      UpiD_LEFT_THROTTLE_POSITION          = 240094,
      UpiD_RIGHT_THROTTLE_RANGE            = 240095,
      UpiD_RIGHT_THROTTLE_POSITION         = 240096,
      UpiD_MAX_FUEL                        = 240147,
      UpiD_FUEL_FREEZE                     = 260000,
      UpiD_AERO_MODE                       = 260100,
      UpiD_PROFILE                         = 260150,
      UpiD_DIS_MODE                        = 260200,
      UpiD_MSG_TYPE                        = 260250,
      UpiD_LAUNCHER_LATITUDE               = 260300,
      UpiD_LAUNCHER_LONGITUDE              = 260350,
      UpiD_LAUNCHER_ALTITUDE               = 260400,
      UpiD_LAUNCHER_X_POSITION             = 260410,
      UpiD_LAUNCHER_Y_POSITION             = 260420,
      UpiD_LAUNCHER_Z_POSITION             = 260430,
      UpiD_LAUNCHER_X_VELOCITY             = 260440,
      UpiD_LAUNCHER_Y_VELOCITY             = 260450,
      UpiD_LAUNCHER_Z_VELOCITY             = 260460,
      UpiD_TARGET_X_POSITION               = 260470,
      UpiD_TARGET_Y_POSITION               = 260480,
      UpiD_TARGET_Z_POSITION               = 260490,
      UpiD_TARGET_X_VELOCITY               = 260500,
      UpiD_TARGET_Y_VELOCITY               = 260510,
      UpiD_TARGET_Z_VELOCITY               = 260520,
      UpiD_LAUNCH_PLAN_TIME                = 260530,
      UpiD_LAUNCH_WINDOW_START_TIME        = 260540,
      UpiD_LAUNCH_WINDOW_STOP_TIME         = 260550,
      UpiD_MUNITION                        = 500001,
      UpiD_ENGINE_FUEL                     = 500002,
      UpiD_STORAGE_FUEL                    = 500003,
      UpiD_EXPENDABLE                      = 500005,
      UpiD_TOTAL_RECORD_SETS               = 500006,
      UpiD_LAUNCHED_MUNITION               = 500007,
      UpiD_SENSOR                          = 500009,
      UpiD_MUNITION_RELOAD                 = 500010,
      UpiD_ENGINE_FUEL_RELOAD              = 500011,
      UpiD_STORAGE_FUEL_RELOAD             = 500012,
      UpiD_EXPENDABLE_RELOAD               = 500013,
   };
   // Accessors

   DisUint32 GetNumFixedDatums() const;
   DisUint32 GetNumVariableDatums() const;

   const std::vector<DisFixedDatum>&    GetFixedDatums() const;
   const std::vector<DisVariableDatum>& GetVariableDatums() const;

   // Mutators
   // Datums can be added in two ways.  One way is by passing a vector of datums.
   // The second way is by passing one datum at a time.

   void SetFixedDatums(const std::vector<DisFixedDatum>& aFixedDatumVec);
   void SetVariableDatums(const std::vector<DisVariableDatum>& aVariableDatumVec);

   void AddFixedDatum(const DisFixedDatum& aFixedDatum);
   void AddVariableDatum(const DisVariableDatum& aVariableDatum);

   void ClearAll();
   void ClearFixedDatums();
   void ClearVariableDatums();

   // Returns length of class in bytes
   DisUint16 GetLength() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   void Stream(std::ostream& aStream) const;

   // Data Validation
   bool IsValid() const;

private:
   std::vector<DisFixedDatum>    mFixedDatumVec;
   std::vector<DisVariableDatum> mVariableDatumVec;
};

// Accessors

inline DisUint32 DisDatumSpec::GetNumFixedDatums() const
{
   return static_cast<unsigned int>(mFixedDatumVec.size());
}

inline DisUint32 DisDatumSpec::GetNumVariableDatums() const
{
   return static_cast<unsigned int>(mVariableDatumVec.size());
}

inline const std::vector<DisFixedDatum>& DisDatumSpec::GetFixedDatums() const
{
   return mFixedDatumVec;
}

inline const std::vector<DisVariableDatum>& DisDatumSpec::GetVariableDatums() const
{
   return mVariableDatumVec;
}

// Mutators

inline void DisDatumSpec::SetFixedDatums(const std::vector<DisFixedDatum>& aFixedDatumVec)
{
   mFixedDatumVec = aFixedDatumVec;
}

inline void DisDatumSpec::SetVariableDatums(const std::vector<DisVariableDatum>& aVariableDatumVec)
{
   mVariableDatumVec = aVariableDatumVec;
}

inline void DisDatumSpec::AddFixedDatum(const DisFixedDatum& aFixedDatum)
{
   mFixedDatumVec.push_back(aFixedDatum);
}

inline void DisDatumSpec::AddVariableDatum(const DisVariableDatum& aVariableDatum)
{
   mVariableDatumVec.push_back(aVariableDatum);
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisDatumSpec& aDisDatumSpec)
{
   aDisDatumSpec.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisDatumSpec& aDisDatumSpec)
{
   aDisDatumSpec.Get(aGenI);
   return aGenI;
}

#endif
