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

#ifndef WSFSIXDOFVEHICLEDATA_HPP
#define WSFSIXDOFVEHICLEDATA_HPP

#include <limits>
#include <string>
#include <vector>

class UtLLAPos;

namespace wsf
{
namespace six_dof
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

struct AutopilotWaypointData
{
   float lat        = 0.0f;
   float lon        = 0.0f;
   float alt_m      = 0.0f;
   float speed_kias = 0.0f;
   float speed_ktas = 0.0f;
   float speed_mach = 0.0f;
};

// This data structure is used to define a single PID's 'gain' data for both a single
// data case and an entry in a tabular data case
struct PidGainData
{
   void clearData() { *this = PidGainData(); }

   float KpGain           = 0.0f;
   float KiGain           = 0.0f;
   float KdGain           = 0.0f;
   float LowpassAlpha     = 1.0f;
   float MaxAccum         = std::numeric_limits<float>::max();
   float MaxErrorZero     = std::numeric_limits<float>::max();
   float MinErrorZero     = std::numeric_limits<float>::min();
   float KtAntiWindup     = 0.0f;
   float ControllingValue = 0.0f;
};

// This data structure is used to define a single PID's 'value' data
struct SinglePidValueData
{
   float SetPoint      = 0.0f;
   float CurrentValue  = 0.0f;
   float KpValue       = 0.0f;
   float KiValue       = 0.0f;
   float KdValue       = 0.0f;
   float FFValue       = 0.0f;
   float OutputBase    = 0.0f;
   float OutputLimited = 0.0f;
   float AccumError    = 0.0f;
   bool  FFValueValid  = false;
};

namespace Pid
{
enum Type
{
   Unknown,
   Alpha,
   VerticalSpeed,
   PitchAngle,
   PitchRate,
   FlightPathAngle,
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
}

// This data structure is used to define an autopilot's PID 'value' data for a given technique
struct AutopilotPidGroupValueData
{
   SinglePidValueData alphaPID;
   SinglePidValueData verticalSpeedPID;
   SinglePidValueData pitchAnglePID;
   SinglePidValueData pitchRatePID;
   SinglePidValueData flightPathAnglePID;
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
   bool  enableAfterburnerAutoControl = false;
   bool  enableSpeedBrakeAutoControl  = false;
   float afterburnerThreshold         = 1.0f;
   float speedBrakeThreshold          = 0.0f;
   float turnRollInMultiplier         = 1.0f;
   float routeAllowableAngleError_rad = 1.0f;

   // Limits
   float pitchGLoad_Min   = 0.0f;
   float pitchGLoad_Max   = 1.5f;
   float alpha_Min        = 0.0f;
   float alpha_Max        = 10.0f;
   float pitchRate_Min    = -5.0f;
   float pitchRate_Max    = 10.0f;
   float vertSpd_Min      = -3000.0f;
   float vertSpd_Max      = 3000.0f;
   float yawGLoad_Max     = 0.4f;
   float beta_Max         = 10.0f;
   float yawRate_Max      = 5.0f;
   float rollRate_Max     = 50.0f;
   float bankAngle_Max    = 45.0f;
   float forwardAccel_Min = -1.0f;
   float forwardAccel_Max = 2.0f;
   float taxiSpeed_Max    = 10.0f;
   float taxiYawRate_Max  = 5.0f;
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
   Throttle,     // Hold a particular throttle setting
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

// Convenience functions for identifying whether BTT or YTT assumptions are appropriate
bool IsBankToTurn(Method aMethod);
bool IsYawToTurn(Method aMethod);
} // namespace Control

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

struct FreezeFlags
{
   bool location       = false;
   bool altitude       = false;
   bool verticalSpeed  = false;
   bool fuelBurn       = false;
   bool speed          = false;
   bool yaw            = false;
   bool pitch          = false;
   bool roll           = false;
   bool testingNoLag   = false;
   bool testingNoAlpha = false;
};
} // namespace six_dof
} // namespace wsf

#endif
