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

#ifndef P6DOFOBJECTDATA_HPP
#define P6DOFOBJECTDATA_HPP

#include <iostream>
#include <vector>

class UtLLAPos;

namespace P6DOF
{
namespace DataType
{
enum Simple
{
   Float,
   Boolean
};

enum Angular
{
   Normalized,
   AngleDeg,
   AngleRad
};
} // namespace DataType

namespace Appearance
{
enum
{
   IsDead            = 0x00000001,
   SmokePlume        = 0x00000002,
   FlamesPresent     = 0x00000004,
   TrailingEffect    = 0x00000008,
   PowerPlantOn      = 0x00000010,
   EngineSmoking     = 0x00000020,
   AfterburnerOn     = 0x00000040,
   LightsNavOn       = 0x00000100,
   LightStrobeOn     = 0x00000200,
   LightsLandingOn   = 0x00000400,
   LightsTaxiOn      = 0x00000800,
   LightsFormationOn = 0x00001000,
   LightsLiveryOn    = 0x00002000,
   CanopyOpen        = 0x00004000,
   ChuteDeployed     = 0x00008000,
   LauncherRaised    = 0x00010000,
   LaunchFlash       = 0x00020000
};
}

// -----------------------------------------------------------------------------------------

// This data structure is used to define a control surface position
struct ControlSurfaceValue
{
   float             value;
   std::string       controlSurfaceName;
   DataType::Angular typeData;
};

// This data structure is used to define a control input and its value
struct FlightControlInputValue
{
   FlightControlInputValue()
      : normalizedValue(1.0)
      , controlInputName("")
      , typeData()
      , boolData(false)
   {
   }

   explicit FlightControlInputValue(const std::string& aName)
      : normalizedValue(1.0)
      , controlInputName(aName)
      , typeData(DataType::Float)
      , boolData(false)
   {
   }

   FlightControlInputValue(const FlightControlInputValue& aSrc) = default;

   float            normalizedValue;
   std::string      controlInputName;
   DataType::Simple typeData;
   bool             boolData;
};

struct AutopilotWaypointData
{
   float lat;
   float lon;
   float alt_m;
   float speed_kias;
   float speed_ktas;
   float speed_mach;
};

// This data structure is used to define a single PID's 'gain' data for both a single
// data case and an entry in a tabular data case
struct PidGainData
{
   PidGainData()
   {
      KpGain           = 0.0f;
      KiGain           = 0.0f;
      KdGain           = 0.0f;
      LowpassAlpha     = 0.0f;
      MaxAccum         = 0.0f;
      MaxErrorZero     = 0.0f;
      MinErrorZero     = 0.0f;
      KtAntiWindup     = 0.0f;
      ControllingValue = 0.0f;
   }

   void clearData()
   {
      KpGain           = 0.0f;
      KiGain           = 0.0f;
      KdGain           = 0.0f;
      LowpassAlpha     = 0.0f;
      MaxAccum         = 0.0f;
      MaxErrorZero     = 0.0f;
      MinErrorZero     = 0.0f;
      KtAntiWindup     = 0.0f;
      ControllingValue = 0.0f;
   }

   float KpGain;
   float KiGain;
   float KdGain;
   float LowpassAlpha;
   float MaxAccum;
   float MaxErrorZero;
   float MinErrorZero;
   float KtAntiWindup;
   float ControllingValue;
};

// This data structure is used to define a single PID's 'value' data
struct SinglePidValueData
{
   SinglePidValueData()
   {
      SetPoint      = 0.0f;
      CurrentValue  = 0.0f;
      KpValue       = 0.0f;
      KiValue       = 0.0f;
      KdValue       = 0.0f;
      FFValue       = 0.0f;
      OutputBase    = 0.0f;
      OutputLimited = 0.0f;
      AccumError    = 0.0f;
      FFValueValid  = false;
   }

   float SetPoint;
   float CurrentValue;
   float KpValue;
   float KiValue;
   float KdValue;
   float FFValue;
   float OutputBase;
   float OutputLimited;
   float AccumError;
   bool  FFValueValid;
};

namespace Pid
{
enum Type
{
   Unknown,
   Alpha,
   VertSpeed,
   PitchAngle,
   PitchRate,
   FltPathAngle,
   DeltaPitch,
   Altitude,
   Beta,
   YawRate,
   YawHeading,
   TaxiHeading,
   RollRate,
   DeltaRoll,
   BankAngle,
   RollHeading,
   ForwardAccel,
   Speed,
   TaxiForwardAccel,
   TaxiSpeed,
   TaxiYawRate,
   LastPidType // This must be the last entry
};

enum Flags
{
   UseAlpha   = 0x01,
   LimitMax   = 0x02,
   ZeroGtMax  = 0x04,
   ZeroLtMin  = 0x08,
   UseKt      = 0x10,
   ClearAccum = 0x20
};
} // namespace Pid

// This data structure is used to define an autopilot's PID 'value' data for a given technique
struct AutopilotPidGroupValueData
{
   SinglePidValueData alphaPID;
   SinglePidValueData vertSpeedPID;
   SinglePidValueData pitchAnglePID;
   SinglePidValueData pitchRatePID;
   SinglePidValueData fltpathAnglePID;
   SinglePidValueData deltaPitchPID;
   SinglePidValueData altitudePID;
   SinglePidValueData betaPID;
   SinglePidValueData yawRatePID;
   SinglePidValueData yawheadingPID;
   SinglePidValueData taxiHeadingPID;
   SinglePidValueData rollRatePID;
   SinglePidValueData deltaRollPID;
   SinglePidValueData bankAnglePID;
   SinglePidValueData rollHeadingPID;
   SinglePidValueData forwardAccelPID;
   SinglePidValueData speedPID;
   SinglePidValueData taxiForwardAccelPID;
   SinglePidValueData taxiSpeedPID;
   SinglePidValueData taxiYawRatePID;
};

// This data structure is used to define various autopilot limits
struct AutopilotLimitsAndSettings
{
   AutopilotLimitsAndSettings()
      : enableAfterburnerAutoControl(false)
      , enableSpeedBrakeAutoControl(false)
      , afterburnerThreshold(1.0f)
      , speedBrakeThreshold(0.0f)
      , turnRollInMultiplier(1.0f)
      , routeAllowableAngleError_rad(1.0f)
      , pitchGLoad_Min(0.0f)
      , pitchGLoad_Max(1.5f)
      , alpha_Min(0.0f)
      , alpha_Max(10.0f)
      , pitchRate_Min(-5.0f)
      , pitchRate_Max(10.0f)
      , vertSpd_Min(-3000.0f)
      , vertSpd_Max(3000.0f)
      , yawGLoad_Max(0.4f)
      , beta_Max(10.0f)
      , yawRate_Max(5.0f)
      , rollRate_Max(50.0f)
      , bankAngle_Max(45.0f)
      , forwardAccel_Min(-1.0f)
      , forwardAccel_Max(2.0f)
      , taxiSpeed_Max(10.0f)
      , taxiYawRate_Max(5.0f)
   {
   }

   bool  enableAfterburnerAutoControl;
   bool  enableSpeedBrakeAutoControl;
   float afterburnerThreshold;
   float speedBrakeThreshold;
   float turnRollInMultiplier;
   float routeAllowableAngleError_rad;
   // Limits
   float pitchGLoad_Min;
   float pitchGLoad_Max;
   float alpha_Min;
   float alpha_Max;
   float pitchRate_Min;
   float pitchRate_Max;
   float vertSpd_Min;
   float vertSpd_Max;
   float yawGLoad_Max;
   float beta_Max;
   float yawRate_Max;
   float rollRate_Max;
   float bankAngle_Max;
   float forwardAccel_Min;
   float forwardAccel_Max;
   float taxiSpeed_Max;
   float taxiYawRate_Max;
};

namespace Lateral
{
enum Mode
{
   Undefined,
   Waypoint,  // Fly waypoints
   Point,     // Fly to point
   Heading,   // Fly heading
   YawRate,   // Yaw cmd, rate
   YawGLoad,  // Yaw cmd, g-load
   Bank,      // Roll cmd, bank
   DeltaRoll, // Roll cmd, delta-roll angle
   RollRate,  // Roll cmd, rate
   Beta,
};
}

namespace Vertical
{
enum Mode
{
   Undefined,
   Waypoint,   // Fly waypoints
   Point,      // Fly to point
   Altitude,   // Hold altitude
   VertSpeed,  // Hold vert speed
   PitchAng,   // Pitch cmd, angle
   PitchRate,  // Pitch cmd, rate
   FltPathAng, // Pitch cmd, flt-path
   DeltaPitch, // Pitch cmd, delta-angle
   PitchGLoad, // Pitch cmd, g-load
   Alpha,      // Pitch cmd, alpha

};
}

namespace Speed
{
enum Mode
{
   Undefined,
   Waypoint,     // Fly waypoints
   ForwardAccel, // Fly controlling accel
   FPS,          // Fly controlling ft/sec
   KIAS,         // Fly controlling KIAS
   KTAS,         // Fly controlling KTAS
   Mach          // Fly controlling mach
};
}

namespace Control
{
enum Method
{
   Undefined,
   BankToTurnNoYaw,   // Bank-to-Turn-NoYaw   BTTNY  uses StickBack, StickRgt, Throttle
   BankToTurnWithYaw, // Bank-to-Turn-WithYaw BTTWY  uses StickBack, StickRgt, Throttle, Rudder
   YawToTurnNoRoll,   // Yaw-to-Turn-NoRoll   YTTNR  uses StickBack, RudderRgt, Throttle
   YawToTurnRollRate, // Yaw-to-Turn-RollRate YTTZRR uses StickBack, RudderRgt, Throttle, StickRgt
   YawToTurnZeroBank  // Yaw-to-Turn-ZeroBank YTTZB  uses StickBack, RudderRgt, Throttle, StickRgt
};
}

namespace Nav
{
enum LegType
{
   LegUndefined,
   FollowTrack,
   DirectTo
};

enum SwitchingType
{
   SwitchUndefined,
   OnApproach,
   OnPassing
};
} // namespace Nav

// This data structure is used to waypoint parameters to help study waypoint control performance
struct NavWaypointParameters
{
   bool               navWaypointDataValid;
   Nav::LegType       navWaypointLegType;
   Nav::SwitchingType navWaypointSwitchingType;
   float              navWaypointPrevLat;
   float              navWaypointPrevLon;
   float              navWaypointPrevAlt;
   float              navWaypointCurrLat;
   float              navWaypointCurrLon;
   float              navWaypointCurrAlt;
   float              navWaypointNextLat;
   float              navWaypointNextLon;
   float              navWaypointNextAlt;
   float              navWaypointAimHeading_rad;
   float              navWaypointStartTurnHdg_rad;
   float              navWaypointTurnRadius_m;
   float              navWaypointTurnAngle_rad;
   float              navWaypointTurnRefPtLat;
   float              navWaypointTurnRefPtLon;
   float              navWaypointTurnCenterLat;
   float              navWaypointTurnCenterLon;
};

// This data structure is used to define the main autopilot data coming repeatedly from the sim
struct AutopilotData
{
   // Autopilot current mode values
   bool                  autopilotEnabled;
   NavWaypointParameters navWaypointParameters;
};

// This data structure is used to define the waypoint data from the sim
struct AutopilotWaypointDataList
{
   unsigned char         currentWaypointIndex;
   unsigned char         waypointListNumItems;
   AutopilotWaypointData waypointListData[50];
};

struct TrackData
{
   std::string sensorName;
   std::string targetName;
   double      targetLat;
   double      targetLon;
   double      targetAlt_m;
};

struct CurrentTargetData
{
   bool        isValid;
   std::string targetName;
   float       targetAziumth_deg;
   float       targetElevation_deg;
   float       targetSlantRange_nm;
   float       guidanceYawAccelCommand_g;
   float       guidancePitchAccelCommand_g;
   float       guidanceGBias_g;
   float       guidanceMaxG;
   float       guidanceFlightPathAngle_deg;
   float       autopilotYawGLoadCommand_g;
   float       autopilotPitchGLoadCommand_g;
   float       autopilotRollRateCommand_dps;
};
}; // namespace P6DOF

#endif
