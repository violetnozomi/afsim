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

#ifndef DISEMISSIONENUMS_HPP
#define DISEMISSIONENUMS_HPP

#include <string>

#include "DisTypes.hpp"

namespace DisEnum
{
namespace emission
{
namespace state_update
{
enum StateUpdateType
{
   HeartbeatUpdate   = 0,
   ChangedDataUpdate = 1
};
inline bool IsValid(DisEnum8 aSUI)
{
   return aSUI <= ChangedDataUpdate;
}
const std::string& ToString(DisEnum8 aSUI);
} // namespace state_update
} // namespace emission

// Deprecated, use DisSystemEnums.hpp
namespace EmitterFunction
{
enum
{
   Other                                 = 0,
   _40_deleted_                          = 40,
   MultiFunction                         = 1,
   FrequencySweptJamming                 = 41,
   EarlyWarning_Surveillance             = 2,
   Jamming                               = 42,
   HeightFinding                         = 3,
   _43_deleted_                          = 43,
   FireControl                           = 4,
   PulsedJamming                         = 44,
   Acquisition_Detection                 = 5,
   RepeaterJamming                       = 45,
   Tracking                              = 6,
   SpotNoiseJamming                      = 46,
   Guideance_Illumination                = 7,
   MissileAcquisition                    = 47,
   FiringPoint_LaunchPointLocation       = 8,
   MissileDownlink                       = 48,
   Ranging                               = 9,
   _49_deleted_                          = 49,
   RadarAltimeter                        = 10,
   Space                                 = 50,
   Imaging                               = 11,
   SurfaceSearch                         = 51,
   MotionDetection                       = 12,
   ShellTracking                         = 52,
   Navigation                            = 13,
   Television                            = 56,
   Weather_Meterological                 = 14,
   Unknown                               = 57,
   Instrumentation                       = 15,
   VideoRemoting                         = 58,
   Identification_Classification         = 16,
   ExperimentalOrTraining                = 59,
   AAA_FireControl                       = 17,
   MissileGuidance                       = 60,
   AirSearch_Bomb                        = 18,
   MissileHoming                         = 61,
   AirIntercept                          = 19,
   MissileTracking                       = 62,
   Altimeter                             = 20,
   Jamming_Noise                         = 64,
   AirMapping                            = 21,
   Jamming_Deception                     = 65,
   AirTrafficControl                     = 22,
   _66_deleted_                          = 66,
   Beacon                                = 23,
   Navigation_DistanceMeasuringEquipment = 71,
   BattlefieldSurveillance               = 24,
   TerrainFollowing                      = 72,
   GroundControlApproach                 = 25,
   WeatherAvoidance                      = 73,
   GroundControlIntercept                = 26,
   ProximityFuse                         = 74,
   CostalSurveillance                    = 27,
   _75_deleted_                          = 75,
   Decoy_Mimic                           = 28,
   Radiosonde                            = 76,
   DataTransmission                      = 29,
   Sonobuoy                              = 77,
   EarthSurveillance                     = 30,
   Weapon_Non_Lethal                     = 96,
   GunLayBeacon                          = 31,
   Weapon_Lethal                         = 97,
   GroundMapping                         = 32,
   HarborSurveillance                    = 33,
   _34_deleted                           = 34,
   ILS_                                  = 35,
   IonosphericSound                      = 36,
   Interrogator                          = 37,
   BarrageJamming                        = 38,
   ClickJamming                          = 39

};
}

//! Deprecated, use enums in DisBeamEnums.hpp
namespace BeamFunction
{
enum
{
   Other                         = 0,
   Search                        = 1,
   HeightFinder                  = 2,
   Acquisition                   = 3,
   Tracking                      = 4,
   AcquisitionAndTracking        = 5,
   CommandGuidance               = 6,
   Illumination                  = 7,
   RangeOnlyRadar                = 8,
   MissileBeacon                 = 9,
   MissileFuze                   = 10,
   ActiveRadarMissileSeeker      = 11,
   Jammer                        = 12,
   IFF                           = 13,
   Navigational_Weather          = 14,
   Meteorlogical                 = 15,
   DataTransmission              = 16,
   NavigationalDirectionalBeacon = 17
};
}

//! \warn RadarState is not part of the DIS Spec!
namespace RadarState
{
enum
{
   LongRangeSearch   = 1,
   VelocitySearch    = 2,
   ShortRangeSearch  = 3,
   SuperSearch       = 4,
   Boresight         = 5,
   ManualSearch      = 6,
   AutomaticFlood    = 7,
   ManualAcquisition = 8,
   SingleTargetTrack = 9,
   TrackMemory       = 10,
   VerticalScan      = 11,
   TrackWhileScan    = 14,
   AutoGun           = 15,
   Map               = 16,
   Range             = 17,
   Doppler           = 18,
   AirToGroundBeacon = 19,
   AirToAirBeacon    = 20,
   Off               = 21,
   StandBy           = 22,
   ManualFlood       = 25,
   Pulse             = 26
};
}
//! \warn RadarPRF is not part of the DIS Spec!
namespace RadarPRF
{
enum
{
   MED        = 1,
   HIGH       = 5,
   ILLUMINATE = 10
};
}
} // namespace DisEnum

#endif
