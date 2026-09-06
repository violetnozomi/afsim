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

#ifndef DisSystemEnums_hpp
#define DisSystemEnums_hpp

#include <string>

#include "DisTypes.hpp"

// The following Enumerations used within the DIS System Record are obtained
// from the SISO-REF-010-00v20-0 (published 19 November 2013).

namespace DisEnum
{
namespace system
{
namespace name
{
#define X(Value, String, Enum) Enum = Value,
enum NationalNomenclature
{
#include "NationalNomenclature.xenum"
};
enum NATO_RepotingName
{
#include "NATO_ReportingName.xenum"
};
enum CommercialDesignation
{
#include "CommercialDesignation.xenum"
};
#undef X
//! Returns True if the Emitter Enumeration is Valid
bool IsValid(DisEnum16 aEmitter);
//! Returns a string of the Emitter Name(s)
/*!
 *  The format of the returned string is:
 *  "[ National Nomenclature | NATO Reporting Name | Commercial Designation ]"
 */
std::string ToString(DisEnum16 aEmitter);
} // namespace name
namespace function
{
enum FunctionType
{
   MultiFunction                        = 1,
   EarlyWarningSurveillance             = 2,
   HeightFinder                         = 3,
   FireControl                          = 4,
   AcquisitionDetection                 = 5,
   Tracker                              = 6,
   GuidanceIllumination                 = 7,
   FiringPointLaunchPointLocation       = 8,
   RangeOnly                            = 9,
   RadarAltimeter                       = 10,
   Imaging                              = 11,
   MotionDetection                      = 12,
   Navigation                           = 13,
   WeatherMeteorological                = 14,
   Instrumentation                      = 15,
   IdentificationClassification         = 16,
   AntiAircraftArtilleryFireControl     = 17,
   AirSearchBomb                        = 18,
   AirIntercept                         = 19,
   Altimeter                            = 20,
   AirMapping                           = 21,
   AirTrafficControl                    = 22,
   Beacon                               = 23,
   BattlefieldSurveillance              = 24,
   GroundControlApproach                = 25,
   GroundControlIntercept               = 26,
   CoastalSurveillance                  = 27,
   DecoyMimic                           = 28,
   DataTransmission                     = 29,
   EarthSurveillance                    = 30,
   GunLayBeacon                         = 31,
   GroundMapping                        = 32,
   HarborSurveillance                   = 33,
   IdentifyFriendOrFoe_deprecated       = 34,
   InstrumentLandingSystem              = 35,
   IonosphericSound                     = 36,
   Interrogator                         = 37,
   BarrageJamming_deprecated            = 38,
   ClickJamming_deprecated              = 39,
   DeceptiveJamming_deprecated          = 40,
   FrequencySweptJamming_deprecated     = 41,
   Jammer                               = 42,
   NoiseJamming_deprecated              = 43,
   PulsedJamming_deprecated             = 44,
   RepeaterJamming_deprecated           = 45,
   SpotNoiseJamming_deprecated          = 46,
   MissileAcquisition                   = 47,
   MissileDownlink                      = 48,
   Meteorological_deprecated            = 49,
   Space                                = 50,
   SurfaceSearch                        = 51,
   ShellTracking                        = 52,
   GAP1_START                           = 53,
   GAP1_END                             = 56,
   Television                           = 56,
   Unknown                              = 57,
   VideoRemoting                        = 58,
   ExperimentalOrTraining               = 59,
   MissileGuidance                      = 60,
   MissileHoming                        = 61,
   MissileTracking                      = 62,
   GAP2_START                           = 63,
   GAP2_END                             = 64,
   JammingNoise_deprecated              = 64,
   JammingDeception_deprecated          = 65,
   Decoy_deprecated                     = 66,
   GAP3_START                           = 67,
   GAP3_END                             = 71,
   NavigationDistanceMeasuringEquipment = 71,
   TerrainFollowing                     = 72,
   WeatherAvoidance                     = 73,
   ProximityFuse                        = 74,
   Instrumentation_deprecated           = 75,
   Radiosonde                           = 76,
   Sonobuoy                             = 77,
   BathythermalSensor                   = 78,
   TowedCounterMeasure                  = 79,
   GAP4_START                           = 80,
   GAP4_END                             = 96,
   WeaponNonLethal                      = 96,
   WeaponLethal                         = 97,
};
inline bool IsValid(DisEnum8 aFunction)
{
   return (aFunction < GAP1_START || (aFunction >= GAP1_END && aFunction < GAP2_START) ||
           (aFunction >= GAP2_END && aFunction < GAP3_START) || (aFunction >= GAP3_END && aFunction < GAP4_START) ||
           (aFunction >= GAP4_END && aFunction < WeaponLethal));
}
const std::string& ToString(DisEnum8 aFunction);
} // namespace function
} // namespace system
} // namespace DisEnum

#endif // DisSystemEnums_hpp
