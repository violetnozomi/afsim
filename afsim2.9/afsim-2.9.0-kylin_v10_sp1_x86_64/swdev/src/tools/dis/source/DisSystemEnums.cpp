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

#include "DisSystemEnums.hpp"

#include <map>
#include <sstream>

namespace
{
typedef std::map<DisEnum16, std::string> NameMap;

class ConverterMaps
{
public:
   ConverterMaps()
   {
#define X(Value, String, Enum) mNationalMap[Value] = String;
#include "NationalNomenclature.xenum"
#undef X
#define X(Value, String, Enum) mNatoMap[Value] = String;
#include "NATO_ReportingName.xenum"
#undef X
#define X(Value, String, Enum) mCommercialMap[Value] = String;
#include "CommercialDesignation.xenum"
#undef X
   }
   ~ConverterMaps() {}
   NameMap mNationalMap;
   NameMap mNatoMap;
   NameMap mCommercialMap;
};

ConverterMaps sNameMaps;
} // namespace

namespace DisEnum
{
namespace system
{
namespace name
{
bool IsValid(DisEnum16 aEmitter)
{
   if (aEmitter == 0)
      return true;
   NameMap::const_iterator i;
   i = sNameMaps.mNationalMap.find(aEmitter);
   if (i != sNameMaps.mNationalMap.end())
      return true;
   i = sNameMaps.mNatoMap.find(aEmitter);
   if (i != sNameMaps.mNatoMap.end())
      return true;
   i = sNameMaps.mCommercialMap.find(aEmitter);
   return i != sNameMaps.mCommercialMap.end();
}
std::string ToString(DisEnum16 aEmitter)
{
   std::stringstream ss;
   ss << "[";
   NameMap::const_iterator i;
   i = sNameMaps.mNationalMap.find(aEmitter);
   if (i != sNameMaps.mNationalMap.end())
   {
      ss << i->second;
   }
   else
   {
      ss << "-";
   }
   ss << "|";

   i = sNameMaps.mNatoMap.find(aEmitter);
   if (i != sNameMaps.mNatoMap.end())
   {
      ss << i->second;
   }
   else
   {
      ss << "-";
   }
   ss << "|";

   i = sNameMaps.mCommercialMap.find(aEmitter);
   if (i != sNameMaps.mCommercialMap.end())
   {
      ss << i->second;
   }
   else
   {
      ss << "-";
   }
   ss << "]";
   return ss.str();
}
} // namespace name
namespace function
{
const std::string& ToString(DisEnum8 aFunction)
{
   static const std::string STR[] = {"-",
                                     "Multi-function",
                                     "Early Warning/Surveillance",
                                     "Height Finder",
                                     "Fire Control",
                                     "Acquisition/Detection",
                                     "Tracker",
                                     "Guidance/Illumination",
                                     "Firing point/launch point location",
                                     "Range-Only",
                                     "Radar Altimeter",
                                     "Imaging",
                                     "Motion Detection",
                                     "Navigation",
                                     "Weather / Meteorological",
                                     "Instrumentation",
                                     "Identification/Classification (including IFF)",
                                     "AAA (Anti-Aircraft Artillery) Fire Control",
                                     "Air Search/Bomb",
                                     "Air Intercept",
                                     "Altimeter",
                                     "Air Mapping",
                                     "Air Traffic Control",
                                     "Beacon",
                                     "Battlefield Surveillance",
                                     "Ground Control Approach",
                                     "Ground Control Intercept",
                                     "Coastal Surveillance",
                                     "Decoy/Mimic",
                                     "Data Transmission",
                                     "Earth Surveillance",
                                     "Gun Lay Beacon",
                                     "Ground Mapping",
                                     "Harbor Surveillance",
                                     "IFF (Identify Friend or Foe) (deprecated)",
                                     "ILS (Instrument Landing System)",
                                     "Ionospheric Sound",
                                     "Interrogator",
                                     "Barrage Jamming (deprecated)",
                                     "Click Jamming (deprecated)",
                                     "Deceptive Jamming (deprecated)",
                                     "Frequency Swept Jamming (deprecated)",
                                     "Jammer",
                                     "Noise Jamming (deprecated)",
                                     "Pulsed Jamming (deprecated)",
                                     "Repeater Jamming (deprecated)",
                                     "Spot Noise Jamming (deprecated)",
                                     "Missile Acquisition",
                                     "Missile Downlink",
                                     "Meteorological (deprecated)",
                                     "Space",
                                     "Surface Search",
                                     "Shell Tracking",
                                     // GAP1
                                     "Television",
                                     "Unknown",
                                     "Video Remoting",
                                     "Experimental or Training",
                                     "Missile Guidance",
                                     "Missile Homing",
                                     "Missile Tracking",
                                     // GAP2
                                     "Jamming, noise (deprecated)",
                                     "Jamming, deception (deprecated)",
                                     "Decoy (deprecated)",
                                     // GAP3
                                     "Navigation/Distance Measuring Equipment",
                                     "Terrain Following",
                                     "Weather Avoidance",
                                     "Proximity Fuse",
                                     "Instrumentation (deprecated)",
                                     "Radiosonde",
                                     "Sonobuoy",
                                     "Bathythermal Sensor",
                                     "Towed Counter Measure",
                                     // GAP4
                                     "Weapon, non-lethal",
                                     "Weapon, lethal"};
   static const size_t      GAP1  = GAP1_END - GAP1_START;
   static const size_t      GAP2  = GAP2_END - GAP2_START;
   static const size_t      GAP3  = GAP3_END - GAP3_START;
   static const size_t      GAP4  = GAP4_END - GAP4_START;

   size_t index = IsValid(aFunction) ? static_cast<size_t>(aFunction) : 0;
   if (index > GAP4_START)
      index -= GAP4;
   if (index > GAP3_START)
      index -= GAP3;
   if (index > GAP2_START)
      index -= GAP2;
   if (index > GAP1_START)
      index -= GAP1;
   return STR[index];
}
} // namespace function
} // namespace system
} // namespace DisEnum
