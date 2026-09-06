// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <sstream>
#include "WsfBMUtils.hpp"
#include <UtDictionary.hpp>
#include <logger.hpp>
#include <stringUtils.hpp>
#include <iadsLib/util.hpp>
#include <WsfSimulation.hpp>
#include <WsfTrackManager.hpp>
#include <iterator>
#include <iadsLib/NumericUtils.hpp>
#include <WsfSimulation.hpp>
#include <WsfCommandChain.hpp>
#include <iadsLib/enum.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfAssetManager.hpp>
#include <WsfBMDisseminateC2.hpp>
#include <WsfBMSensorsManager.hpp>
#include <WsfBMWeaponsManager.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include <WsfScriptAssetManagerClass.hpp>
#include <WsfScriptBattleManagerClass.hpp>
#include <WsfScriptDisseminateC2Class.hpp>
#include <WsfScriptWeaponsManagerClass.hpp>
#include <WsfScriptSensorsManagerClass.hpp>
#include <WsfWeapon.hpp>
#include <WsfSensor.hpp>

//const int WsfBMUtils::UNKNOWN_TYPE = 0;
//const int WsfBMUtils::UNKNOWN_SUBTYPE = 0;
//const int WsfBMUtils::INVALID_ID = INVALID_UNIT_ID;

WsfBMUtils* WsfBMUtils::Clone() const
{
   return new WsfBMUtils(*this);
}

int WsfBMUtils::GetStringId(const std::string& str_Value)
{
   auto& string_dict = UtDictionary::GetDictionary();
   return string_dict.GetNumber(str_Value);
}

std::string WsfBMUtils::GetStringFromId(const int stringid)
{
   auto& string_dict = UtDictionary::GetDictionary();
   return string_dict.GetString(stringid);
}

bool WsfBMUtils::IsTypeUnknown(const std::string& type_str)
{
   return type_str.empty()
          || il::equalsIgnoreCase(type_str, "n/a")
          || il::equalsIgnoreCase(type_str, "unknown");
}

bool WsfBMUtils::IsSubtypeUnknown(const std::string& subtype_str)
{
   // use the same criteria as the unknown type
   return WsfBMUtils::IsTypeUnknown(subtype_str);
}

bool WsfBMUtils::IsTypeAny(const std::string& type_str)
{
   return type_str == "*" || il::equalsIgnoreCase(type_str, "any");
}

bool WsfBMUtils::IsSubtypeAny(const std::string& subtype_str)
{
   // use the same criteria as the any type
   return WsfBMUtils::IsTypeAny(subtype_str);
}

int WsfBMUtils::GetUnknownType()
{
   //return UNKNOWN_TYPE;
   return 0;
}

int WsfBMUtils::GetUnknownSubtype()
{
   //return UNKNOWN_SUBTYPE;
   return 0;
}

int WsfBMUtils::GetType(const std::string& type_str)
{
   return WsfBMUtils::GetStringId(type_str);
}

int WsfBMUtils::GetSubType(const std::string& subtype_str)
{
   return WsfBMUtils::GetStringId(subtype_str);
}

// Does not check for nullptr on platform.
std::string WsfBMUtils::GetSpatialDomain(const WsfPlatform* platform)
{
   std::string domain_str;

   switch (platform->GetSpatialDomain())
   {
      case WSF_SPATIAL_DOMAIN_UNKNOWN:
         domain_str = "unknown";
         break;
      case WSF_SPATIAL_DOMAIN_LAND:
         domain_str = "land";
         break;
      case WSF_SPATIAL_DOMAIN_AIR:
         domain_str = "air";
         break;
      case WSF_SPATIAL_DOMAIN_SURFACE:
         domain_str = "surface";
         break;
      case WSF_SPATIAL_DOMAIN_SUBSURFACE:
         domain_str = "subsurface";
         break;
      case WSF_SPATIAL_DOMAIN_SPACE:
         domain_str = "space";
         break;
      default:
         std::stringstream strm_error;
         strm_error << "Invalid platform domain type: " << domain_str;
         HCL_FATAL_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << strm_error.str();
         throw UtException(strm_error.str());
         break;
   }

   return domain_str;
}

WsfTrackId WsfBMUtils::ConvertTrackId(const il::idRecord& il_record)
{
   return WsfTrackId(WsfStringId(il_record.getSystemUnitIDString()), il_record.getSystemTrackID());
}

il::idRecord WsfBMUtils::ConvertTrackId(const WsfTrackId& track_id)
{
   return WsfBMUtils::ConvertTrackId(WsfStringInt(track_id.GetOwningPlatformId()), track_id.GetLocalTrackNumber());
}

il::idRecord WsfBMUtils::ConvertTrackId(const int owning_plat_string_id, const int tan)
{
   // note: the id provided from WsfTrackId is a string id, not a platform id.
   return il::idRecord(owning_plat_string_id, tan, WsfBMUtils::GetStringFromId(owning_plat_string_id));
}

std::size_t WsfBMUtils::GetTracksUpdated(const WsfAssetManager* am_proc, double sim_time, std::vector<WsfLocalTrack*>& tracks)
{
   const auto& all_tracks = am_proc->GetPlatform()->GetMasterTrackList();

   for (unsigned int idx = 0; idx < all_tracks.GetTrackCount(); ++idx)
   {
      auto track = const_cast<WsfLocalTrack*>(all_tracks.GetTrackEntry(idx));
      if (NumericUtils::GreaterThan(track->GetRawTrack()->GetUpdateTime(), am_proc->TimeLastReceivedTrackUpdate(track)))
      {
         tracks.push_back(track);
      }
   }

   return tracks.size();
}

// Returns true if a raw track that is contributing to the local track represented by track_id is:
// (1) "local" to the platform (came from a sensor on the platform, one of its peer platforms, or its direct commander)
// and (2) came from a sensor of a type that matches sensor_type.
// Does not check relative_to_platform for nullptr.
bool WsfBMUtils::IsTrackGeneratedByLocalSensor(WsfPlatform* relative_to_platform, const WsfTrackId& track_id, const il::SensorEnum sensor_type)
{
   auto sim_ptr = relative_to_platform->GetSimulation();
   auto& track_mgr = relative_to_platform->GetTrackManager();

   const WsfLocalTrack* master_track = track_mgr.FindTrack(track_id);
   if (master_track)
   {
      // Examine each contributing lowest-level track until we find one that originated from the chosen sensor type and is "local" to our platform.
      std::vector<const WsfTrack*> lowest_level_tracks = WsfBMUtils::GetOriginalTracks(sim_ptr, master_track);
      for (auto iter = lowest_level_tracks.begin(); iter != lowest_level_tracks.end(); ++iter)
      {
         const WsfTrack* contributor_track = *iter;

         if (contributor_track)
         {
            WsfPlatform* contributor_platform = sim_ptr->GetPlatformByIndex(contributor_track->GetOriginatorIndex());
            if (contributor_platform)
            {
               // The lowest-level track originated from the chosen sensor type.
               if (GetRawTrackSensorType(sim_ptr, contributor_platform, contributor_track) == sensor_type)
               {
                  bool peer_platforms = ArePlatformsPeers(relative_to_platform, contributor_platform);
                  bool direct_commander = IsDirectCommander(contributor_platform, relative_to_platform);

                  // If the track originated from this platform, a peer platform, or a direct commander, return true.
                  if (peer_platforms || direct_commander)
                  {
                     return true;
                  }
               }
            }
         }
      }
   }
   return false;
}

WsfPlatform* WsfBMUtils::GetMessageDestination(/* const */ WsfSimulation* sim_ptr, const std::shared_ptr<const il::baseMessage>& message)
{
   return GetPlatformFromUnitIDRecord(sim_ptr, message->getDestinationID());
}

// Does not check platform for nullptr.
int WsfBMUtils::GetUnitIDFromPlatform(const WsfPlatform* platform)
{
   return static_cast<int>(platform->GetIndex());
}

// Does not check platform for nullptr.
il::idRecord WsfBMUtils::GetUnitIDRecordFromPlatform(const WsfPlatform* platform)
{
   return GetUnitIDRecordFromPlatformIndex(platform->GetSimulation(), static_cast<int>(platform->GetIndex()));
}

il::idRecord WsfBMUtils::GetUnitIDRecordFromPlatformIndex(/* const */ WsfSimulation* sim_ptr, const int plat_idx)
{
   auto plat = sim_ptr->GetPlatformByIndex(plat_idx);
   if (plat)
   {
      return il::idRecord(plat_idx, 0, plat->GetName());
   }

   return plat_idx;
}

WsfPlatform* WsfBMUtils::GetPlatformFromUnitID(/* const */ WsfSimulation* sim_ptr, const int id)
{
   return sim_ptr->GetPlatformByIndex(id);
}

WsfPlatform* WsfBMUtils::GetPlatformFromUnitIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& id)
{
   return sim_ptr->GetPlatformByIndex(id.getID());
}

// returns true if platforms have a common commander on any command chain. Does not check for nullptr.
bool WsfBMUtils::ArePlatformsPeers(const WsfPlatform* plat1, const WsfPlatform* plat2)
{
   std::set <std::string> plat1_cmdrs, plat2_cmdrs;

   auto get_commandernames = [](const WsfPlatform * plat) -> std::set <std::string>
   {
      std::set <std::string> commandernames;
      //for (unsigned int chain_idx = 0; chain_idx < plat->GetComponentEntry<WsfCommandChain>Count(); ++chain_idx)
      for (unsigned int chain_idx = 0; chain_idx < plat->GetComponentCount<WsfCommandChain>(); ++chain_idx)
      {
         //auto command_chain = plat->GetComponentEntry<WsfCommandChain>Entry (chain_idx);
         auto command_chain = plat->GetComponentEntry<WsfCommandChain>(chain_idx);
         commandernames.insert(command_chain->GetCommanderName());
      }

      return commandernames;
   };

   plat1_cmdrs = get_commandernames(plat1);
   plat2_cmdrs = get_commandernames(plat2);

   std::vector<std::string> intersection;
   std::set_intersection(plat1_cmdrs.begin(), plat1_cmdrs.end(), plat2_cmdrs.begin(), plat2_cmdrs.end(), std::back_inserter(intersection));

   return intersection.size() > 0;
}

// Does not check test_cmdr or against_plat for nullptr.
bool WsfBMUtils::IsDirectCommander(const WsfPlatform* test_cmdr, const WsfPlatform* against_plat, const std::string& opt_cmd_chain_name)
{
   auto suspected_sub_cmdr = GetCommander(against_plat, opt_cmd_chain_name);

   return suspected_sub_cmdr ? test_cmdr->GetIndex() == suspected_sub_cmdr->GetIndex() : false;
}

// Does not check argument "from" for nullptr.
WsfPlatform* WsfBMUtils::GetCommander(const WsfPlatform* from, const std::string& opt_cmd_chain_name)
{
   auto command_chain = opt_cmd_chain_name.empty() ? from->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId()) : from->GetComponent<WsfCommandChain>(opt_cmd_chain_name);
   if (command_chain)
   {
      return command_chain->GetCommander();
   }

   return nullptr;
}

// Does not check argument "from" for nullptr.
WsfPlatform* WsfBMUtils::GetTopMostCommander(const WsfPlatform* from, const std::string& opt_cmd_chain_name)
{
   auto next_cmdr = GetCommander(from, opt_cmd_chain_name);
   while (next_cmdr)
   {
      auto cmdr = GetCommander(next_cmdr);
      if (cmdr && cmdr != next_cmdr) // since the command chain is external to platform, we have to check for self-commandment
      {
         next_cmdr = cmdr;
      }
      else
      {
         break;
      }
   }

   return next_cmdr;
}

il::idRecord WsfBMUtils::GetAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const int plat_idx, const int wpn_idx)
{
   il::idRecord assigned_id;

   auto plat = WsfBMUtils::GetPlatformFromUnitID(sim_ptr, plat_idx);
   if (plat)
   {
      //auto wpn = plat->GetComponentEntry<WsfWeapon> (wpn_idx);
      auto wpn = plat->GetComponentEntry<WsfWeapon>(wpn_idx);
      if (wpn)
      {
         assigned_id = WsfBMUtils::GetAssignedIDRecord(plat, wpn);
      }
      else
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "Invalid weapon entry " << wpn_idx << " on platform: " << plat->GetName();
      }
   }

   return assigned_id;
}

// Does check plat and weapon for nullptr.
il::idRecord WsfBMUtils::GetAssignedIDRecord(const WsfPlatform* plat, const WsfWeapon* weapon)
{
   il::idRecord value = { 0, 0, "", "" };
   if (plat)
   {
      value = il::idRecord(static_cast<int32_t>(plat->GetIndex()), WsfBMUtils::GetWeaponIndexOnPlatform(plat, weapon), plat->GetName(), weapon->GetName());
   }
   return value;
}

WsfPlatform* WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& assigned_id)
{
   return sim_ptr->GetPlatformByIndex(assigned_id.getID());
}

WsfWeapon* WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& assigned_id)
{
   WsfWeapon* wpn = nullptr;

   auto plat = WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(sim_ptr, assigned_id);
   if (plat)
   {
      wpn = plat->GetComponentEntry<WsfWeapon>(assigned_id.getSubID());
   }

   return wpn;
}

// Does check platform for nullptr.
WsfWeapon* WsfBMUtils::GetWeapon(const WsfPlatform* platform, const std::string& weapon_name)
{
   WsfWeapon* value = nullptr;
   if (platform)
   {
      value = platform->GetComponent<WsfWeapon>(weapon_name);
   }
   return value;
}

// Does check platform for nullptr.
il::idRecord WsfBMUtils::GetWeaponID(const WsfPlatform* platform, const std::string& weapon_name)
{
   il::idRecord weapon_id(INVALID_UNIT_ID, INVALID_UNIT_ID, "*error*");

   if (platform)
   {
      auto weapon = platform->GetComponent<WsfWeapon>(weapon_name);
      if (weapon)
      {
         unsigned int weapon_idx = WsfBMUtils::GetWeaponIndexOnPlatform(platform, weapon);
         weapon_id.setID(weapon_idx, weapon->GetName());
      }
   }

   return weapon_id;
}

// Does check weapon and platform for nullptr.
il::idRecord WsfBMUtils::GetWeaponID(const WsfPlatform* platform, const WsfWeapon* weapon)
{
   if (weapon)
   {
      return WsfBMUtils::GetWeaponID(platform, weapon->GetName());
   }

   return il::idRecord(INVALID_UNIT_ID, INVALID_UNIT_ID, "*error*");
}

il::idRecord WsfBMUtils::GetWeaponID(WsfSimulation* sim_ptr, const int plat_idx, const int wpn_idx)
{
   auto plat = GetPlatformFromUnitID(sim_ptr, plat_idx);
   if (plat)
   {
      auto wpn = plat->GetComponentEntry<WsfWeapon>(wpn_idx);

      return WsfBMUtils::GetWeaponID(plat, wpn);
   }

   HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario())) << "WsfBMUtils::GetWeaponID(): invalid weapon entry: platidx = " << plat_idx << " wpn_idx = " << wpn_idx;

   return il::idRecord();
}

bool WsfBMUtils::IsWeaponIDValid(const int id)
{
   return id != INVALID_UNIT_ID;
}

// Does check platform for nullptr.
WsfSensor* WsfBMUtils::GetSensor(const WsfPlatform* platform, const std::string& sensor_name)
{
   if (platform)
   {
      for (unsigned int sidx = 0; sidx < platform->GetComponentCount<WsfSensor>(); ++sidx)
      {
         if (platform->GetComponentEntry<WsfSensor>(sidx)->GetName() == sensor_name)
         {
            return platform->GetComponentEntry<WsfSensor>(sidx);
         }
      }
   }

   return nullptr;
}

// Does check platform for nullptr.
WsfSensor* WsfBMUtils::GetSensor(const WsfPlatform* platform, const int sensor_id)
{
   if (platform)
   {
      return platform->GetComponentEntry<WsfSensor>(sensor_id);
   }

   return nullptr;
}

// Does check platform for nullptr.
int WsfBMUtils::GetSensorID(WsfPlatform* platform, const std::string& sensor_name)
{
   int sensor_id = INVALID_UNIT_ID;
   if (platform)
   {
      for (unsigned int sidx = 0; sidx < platform->GetComponentCount<WsfSensor>(); ++sidx)
      {
         if (platform->GetComponentEntry<WsfSensor>(sidx)->GetName() == sensor_name)
         {
            sensor_id = sidx;
            break;
         }
      }
   }

   return sensor_id;
}

// Does check platform and sensor for nullptr.
il::idRecord WsfBMUtils::GetSensorID(const WsfPlatform* platform, const WsfSensor* sensor)
{
   il::idRecord sensor_id(INVALID_UNIT_ID, INVALID_UNIT_ID);
   if (platform && sensor)
   {
      for (unsigned int sidx = 0; sidx < platform->GetComponentCount<WsfSensor>(); ++sidx)
      {
         if (platform->GetComponentEntry<WsfSensor>(sidx)->GetName() == sensor->GetName())
         {
            sensor_id = il::idRecord(sidx, 0, sensor->GetName());
            break;
         }
      }
   }

   return sensor_id;
}

il::idRecord WsfBMUtils::GetSensorID(WsfSimulation* sim_ptr, const int plat_idx, const int snsr_idx)
{
   auto plat = GetPlatformFromUnitID(sim_ptr, plat_idx);
   if (plat)
   {
      auto snsr = GetSensor(plat, snsr_idx);
      return il::idRecord(snsr_idx, 0, snsr->GetName());
   }

   return il::idRecord(snsr_idx);
}

il::SensorEnum WsfBMUtils::GetSensorType(const WsfSensor& sensor)
{
   il::SensorEnum type = il::SENSOR_UNKNOWN;

   auto categories = sensor.GetCategories().GetCategoryList();
   bool multiple_categories_set = false;
   for (auto it = categories.cbegin(); it != categories.cend(); ++it)
   {
      const auto& category = *it;
      if (il::equalsIgnoreCase(category.GetString(), il::C_TTR_SENSOR_TYPE_STR))
      {
         if (type != il::SENSOR_UNKNOWN)
         {
            multiple_categories_set = true;
         }
         else
         {
            type = il::SENSOR_TTR;
         }
      }
      else if (il::equalsIgnoreCase(category.GetString(), il::C_TAR_SENSOR_TYPE_STR))
      {
         if (type != il::SENSOR_UNKNOWN)
         {
            multiple_categories_set = true;
         }
         else
         {
            type = il::SENSOR_TAR;
         }
      }
      else if (il::equalsIgnoreCase(category.GetString(), il::C_EW_SENSOR_TYPE_STR))
      {
         if (type != il::SENSOR_UNKNOWN)
         {
            multiple_categories_set = true;
         }
         else
         {
            type = il::SENSOR_EW;
         }
      }
      else if (il::equalsIgnoreCase(category.GetString(), il::C_RWR_SENSOR_TYPE_STR))
      {
         if (type != il::SENSOR_UNKNOWN)
         {
            multiple_categories_set = true;
         }
         else
         {
            type = il::SENSOR_RWR;
         }
      }

      if (multiple_categories_set)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sensor.GetScenario())) << "Warning: Multiple valid sensor categories exist on platform/sensor: "
                           << sensor.GetPlatform()->GetName() << "/" << sensor.GetName()
                           << ": Setting to unknown. Sensors within the C2 system can only solely operate as EW, TAR or TTR.";
         type = il::SENSOR_UNKNOWN;
         break;
      }
   }

   return type;
}

// Does not check track for nullptr.
inline il::SensorEnum WsfBMUtils::GetTrackSensorType(WsfSimulation* sim, const WsfLocalTrack* track)
{
   std::set<WsfTrackId> testedTracks;
   return WsfBMUtils::GetTrackSensorType(sim, track, testedTracks);
}

// Determine and return the highest quality sensor type with an active contribution to the local track.
// This method aims to iterate through all the raw tracks that make up
// Does not check track for nullptr.
il::SensorEnum WsfBMUtils::GetTrackSensorType(WsfSimulation* sim, const WsfLocalTrack* track, std::set<WsfTrackId>& testedTracks)
{
   il::SensorEnum trackSensorType = il::SENSOR_UNKNOWN;
   testedTracks.insert(track->GetTrackId());

   for (unsigned int iter = 0; iter < track->GetRawTrackIds().GetCount(); ++iter)
   {
      const auto trackId = *(track->GetRawTrackIds().GetEntry(iter));
      const auto rawTrack = track->GetTrackManager()->FindRawTrack(trackId);

      auto originatorIndex = rawTrack->GetOriginatorIndex();
      auto sensorName = rawTrack->GetSensorNameId();
      auto platform = GetPlatformFromUnitID(sim, static_cast<int>(originatorIndex));

      if (platform)
      {
         if (sensorName != 0)
         {
            for (unsigned sidx = 0; sidx < platform->GetComponentCount<WsfSensor>(); ++sidx)
            {
               WsfSensor* sensor = platform->GetComponentEntry<WsfSensor>(sidx);

               if (sensor && rawTrack->GetSensorNameId() == sensor->GetNameId())
               {
                  auto type = GetSensorType(*sensor);
                  if (trackSensorType < type)
                  {
                     trackSensorType = type;
                  }
                  sidx = platform->GetComponentCount<WsfSensor>();
               }
            }
         }
         else if (testedTracks.find(trackId) == testedTracks.end())
         {
            auto innerTrack = platform->GetTrackManager().FindTrack(trackId);
            if (innerTrack)
            {
               auto innerType = GetTrackSensorType(sim, innerTrack, testedTracks);
               if (trackSensorType < innerType)
               {
                  trackSensorType = innerType;
               }
            }
         }
      }
   }

   testedTracks.erase(track->GetTrackId());
   return trackSensorType;
}
//TODO: change params to &
// Returns a vector containing all "original" tracks contributing to the local track. An "original" track is defined as a WsfTrack that originated from
// a sensor, so for each WsfTrack* in the vector the sensor name id != 0 and the originating platform is a sensor.
// Does not check track for nullptr.
std::vector<const WsfTrack*> WsfBMUtils::GetOriginalTracks(WsfSimulation* sim, const WsfLocalTrack* track)
{
   std::set<WsfTrackId> tested_tracks;
   std::vector<const WsfTrack*> original_tracks;
   GetOriginalTracks(sim, track, tested_tracks, original_tracks);
   return original_tracks;
}

// Does not check track for nullptr.
void WsfBMUtils::GetOriginalTracks(WsfSimulation* sim, const WsfLocalTrack* track, std::set<WsfTrackId>& tested_tracks, std::vector<const WsfTrack*>& original_tracks)
{
   tested_tracks.insert(track->GetTrackId());

   // Evaluate every raw track that contributes to this local track.
   for (unsigned int i = 0, count = track->GetRawTrackIds().GetCount(); i != count; ++i)
   {
      const WsfTrackId track_id = *(track->GetRawTrackIds().GetEntry(i));
      const WsfTrack* raw_track = track->GetTrackManager()->FindRawTrack(track_id);

      // Find the raw track's originating platform.
      size_t originator_index = raw_track->GetOriginatorIndex();
      WsfStringId sensor_name = raw_track->GetSensorNameId();
      WsfPlatform* originating_platform = GetPlatformFromUnitID(sim, static_cast<int>(originator_index));

      if (originating_platform)
      {
         // We've found a bottom-level track: the sensor that generated the track is located on this track's originating platform.
         if (sensor_name != 0)
         {
            original_tracks.push_back(raw_track);
         }

         // This isn't a bottom-level track, so we need to recursively evaluate its contributing tracks (if we haven't tested it yet).
         else if (tested_tracks.find(track_id) == tested_tracks.end())
         {
            WsfLocalTrack* inner_track = originating_platform->GetTrackManager().FindTrack(track_id);
            if (inner_track)
            {
               GetOriginalTracks(sim, inner_track, tested_tracks, original_tracks);
            }
         }
      }
   }

   tested_tracks.erase(track->GetTrackId());
   return;
}

// If the raw track is an "original" track (actually originated from a sensor), returns the type of the sensor. Else, returns SENSOR_UNKNOWN.
// Does not check originating_platform or raw_track for nullptr.
il::SensorEnum WsfBMUtils::GetRawTrackSensorType(WsfSimulation* sim, const WsfPlatform* originating_platform, const WsfTrack* raw_track)
{
   auto sensor_name = raw_track->GetSensorNameId();
   if (originating_platform)
   {
      if (sensor_name != 0)
      {
         // Attempt to find the sensor that contributed the raw track on the platform.
         for (int i = 0, count = originating_platform->GetComponentCount<WsfSensor>(); i != count; ++i)
         {
            WsfSensor* sensor = originating_platform->GetComponentEntry<WsfSensor>(i);
            // We've found the sensor: return its type.
            if (raw_track->GetSensorNameId() == sensor->GetNameId())
            {
               return GetSensorType(*sensor);
            }
         }
      }
   }
   return il::SENSOR_UNKNOWN;
}

//TODO: change params to &
// Does not check trk_mgr or aRawTrackId for nullptr.
WsfLocalTrack* WsfBMUtils::FindCorrelatedTrack(WsfTrackManager* trk_mgr, const WsfTrackId* aRawTrackId)
{
   return trk_mgr->FindCorrelatedTrack(*aRawTrackId);
}

// Does check plat and message assigning platform for nullptr.
bool WsfBMUtils::IsAssignedBy(const WsfPlatform* plat, const WsfBMAssignmentMessage& message)
{
   // compare platform ids for assigner versus passed in platform
   if (plat)
   {
      auto assigningPlat = message.GetAssigningPlatform(plat->GetSimulation());

      if (assigningPlat)
      {
         return assigningPlat->GetIndex() == plat->GetIndex();
      }
   }

   return false;
}

namespace
{
   // Does check platform for nullptr.
   template <class tReturn>
   tReturn* LocateProcessorOfType(const WsfPlatform* platform, const std::string& base_class_name, const bool log_errors = true)
   {
      tReturn* raw_proc_ptr = nullptr;

      if (platform)
      {
         for (unsigned procidx = 0; procidx < platform->GetComponentCount<WsfProcessor>(); ++procidx)
         {
            auto processor = platform->GetComponentEntry<WsfProcessor>(procidx);
            if (processor->IsA_TypeOf(base_class_name))
            {
               raw_proc_ptr = dynamic_cast<tReturn*>(processor);
               if (raw_proc_ptr == nullptr && log_errors)
               {
                  HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << "Found local processor of type " << base_class_name << " but could not cast to base class type " << typeid(tReturn).name();
               }
            }
         }
      }

      if (!raw_proc_ptr && log_errors)
      {
         HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(platform->GetScenario())) << "Could not locate processor of type " << base_class_name << " on platform " << platform->GetName();
      }

      return raw_proc_ptr;
   }
}

// Does not check platform for nullptr.
std::shared_ptr<il::AssetManagerInterface> WsfBMUtils::FindAttachedAssetManagerRef(const WsfPlatform* platform, const bool log_errors)
{
   auto raw_ptr = LocateProcessorOfType<WsfAssetManager>(platform, WsfScriptAssetManagerClass::BASE_CLASS_NAME, log_errors);
   return raw_ptr ? raw_ptr->GetCoreAMRef() : nullptr;
}

// Does not check platform for nullptr.
std::shared_ptr<il::BattleManagerInterface> WsfBMUtils::FindAttachedBattleManagerRef(const WsfPlatform* platform, const bool log_errors)
{
   auto raw_ptr = LocateProcessorOfType<WsfBattleManager>(platform, WsfScriptBattleManagerClass::BASE_CLASS_NAME, log_errors);
   return raw_ptr ? raw_ptr->GetCoreBMRef() : nullptr;
}

// Does not check platform for nullptr.
std::shared_ptr<il::DisseminateC2Interface> WsfBMUtils::FindAttachedDisseminationManagerRef(const WsfPlatform* platform, const bool log_errors)
{
   auto raw_ptr = LocateProcessorOfType<WsfBMDisseminateC2>(platform, WsfScriptDisseminateC2Class::BASE_CLASS_NAME, log_errors);
   return raw_ptr ? raw_ptr->GetCoreDC2Ref() : nullptr;
}

// Does not check platform for nullptr.
std::shared_ptr<il::SensorsManagerInterface> WsfBMUtils::FindAttachedSensorsManagerRef(const WsfPlatform* platform, const bool log_errors)
{
   auto raw_ptr = LocateProcessorOfType<WsfBMSensorsManager>(platform, WsfScriptSensorsManagerClass::BASE_CLASS_NAME, log_errors);
   return raw_ptr ? raw_ptr->GetCoreSMRef() : nullptr;
}

// Does not check platform for nullptr.
std::shared_ptr<il::WeaponsManagerInterface> WsfBMUtils::FindAttachedWeaponsManagerRef(const WsfPlatform* platform, const bool log_errors)
{
   auto raw_ptr = LocateProcessorOfType<WsfBMWeaponsManager>(platform, WsfScriptWeaponsManagerClass::BASE_CLASS_NAME, log_errors);
   return raw_ptr ? raw_ptr->GetCoreWMRef() : nullptr;
}

// Does not check platform for nullptr.
WsfAssetManager* WsfBMUtils::FindAttachedAssetManagerRef2(const WsfPlatform* platform, const bool log_errors)
{
   return LocateProcessorOfType<WsfAssetManager>(platform, WsfScriptAssetManagerClass::BASE_CLASS_NAME, log_errors);
}

// Does not check platform for nullptr.
WsfBattleManager* WsfBMUtils::FindAttachedBattleManagerRef2(const WsfPlatform* platform, const bool log_errors)
{
   return LocateProcessorOfType<WsfBattleManager>(platform, WsfScriptBattleManagerClass::BASE_CLASS_NAME, log_errors);
}

// Does not check platform for nullptr.
WsfBMDisseminateC2* WsfBMUtils::FindAttachedDisseminationManagerRef2(const WsfPlatform* platform, const bool log_errors)
{
   return LocateProcessorOfType<WsfBMDisseminateC2>(platform, WsfScriptDisseminateC2Class::BASE_CLASS_NAME, log_errors);
}

// Does not check platform for nullptr.
WsfBMSensorsManager* WsfBMUtils::FindAttachedSensorsManagerRef2(const WsfPlatform* platform, const bool log_errors)
{
   return LocateProcessorOfType<WsfBMSensorsManager>(platform, WsfScriptSensorsManagerClass::BASE_CLASS_NAME, log_errors);
}

// Does not check platform for nullptr.
WsfBMWeaponsManager* WsfBMUtils::FindAttachedWeaponsManagerRef2(const WsfPlatform* platform, const bool log_errors)
{
   return LocateProcessorOfType<WsfBMWeaponsManager>(platform, WsfScriptWeaponsManagerClass::BASE_CLASS_NAME, log_errors);
}

// Does check platform for nullptr.
bool WsfBMUtils::AuxDataExists(const WsfPlatform* platform, const std::string& attrib_name)
{
   bool value = false;
   if (platform)
   {
      value = platform->HasAuxData() && platform->GetAuxData().AttributeExists(attrib_name);
   }
   return value;
}

// Does check weapon for nullptr.
bool WsfBMUtils::AuxDataExists(const WsfWeapon* weapon, const std::string& attrib_name)
{
   bool value = false;
   if (weapon)
   {
      value = weapon->HasAuxData() && weapon->GetAuxData().AttributeExists(attrib_name);
   }
   return value;
}

// Does check track for nullptr.
bool WsfBMUtils::AuxDataExists(const WsfTrack* track, const std::string& attrib_name)
{
   bool value = false;
   if (track)
   {
      value = track->HasAuxData() && track->GetAuxData().AttributeExists(attrib_name);
   }
   return value;
}

bool WsfBMUtils::AuxDataExists(const WsfMessage& message, const std::string& attrib_name)
{
   return message.HasAuxData() && message.GetAuxData().AttributeExists(attrib_name);
}

// Equivalent to UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, HeadingValid, 0, "bool", "") since WsfTrack doesn't have this method natively
// Does check track for nullptr.
bool WsfBMUtils::HeadingValid(const WsfTrack* track)
{
   bool headingValid = false;
   if (track &&
       track->LocationValid() &&
       track->VelocityValid())
   {
      headingValid = true;
   }

   return headingValid;
}

// Equivalent to UT_DEFINE_SCRIPT_METHOD(WsfScriptTrackClass, WsfTrack, Heading, 0, "double", "") since WsfTrack doesn't have this method natively
// Does check track for nullptr.
double WsfBMUtils::GetHeading(const WsfTrack* track)
{
   double heading = 0.0;
   if (track &&
       track->LocationValid() &&
       track->VelocityValid())
   {
      double trackLocWCS[3];
      track->GetLocationWCS(trackLocWCS);
      double trackVelWCS[3];
      track->GetVelocityWCS(trackVelWCS);
      UtEntity entity;
      entity.SetLocationWCS(trackLocWCS);
      entity.SetVelocityWCS(trackVelWCS);
      double trackVelNED[3];
      entity.GetVelocityNED(trackVelNED);
      heading = atan2(trackVelNED[1], trackVelNED[0]);
      heading = UtMath::NormalizeAngle0_360(heading * UtMath::cDEG_PER_RAD);
   }

   return heading;
}

// Does check track for nullptr.
WsfPlatform* WsfBMUtils::GetTarget(WsfSimulation* sim_ptr, const WsfTrack* track)
{
   if (track)
   {
      return sim_ptr->GetPlatformByIndex(track->GetTargetIndex());
   }

   return nullptr;
}

// Does not check aTrackPtr for nullptr.
WsfBMTrackMessage WsfBMUtils::ConvertTrack(WsfSimulation*       aSimPtr,
                                           const WsfLocalTrack* aTrackPtr,
                                           const bool           aLogWarnings)
{
   // convert to a BM track
   WsfBMTrackMessage bm_track_message(aSimPtr);
   double lat, lon, alt;
   aTrackPtr->GetLocationLLA(lat, lon, alt);
   bm_track_message.SetPosition(aTrackPtr->GetUpdateTime(), WsfGeoPoint(lat, lon, alt));

   // Make the update interval large since we don't want internal age-outs. All maintenance will come externally
   bm_track_message.SetUpdateInterval(1000);

   bm_track_message.SetTrackID(aTrackPtr->GetTrackId());

   if (!aTrackPtr->Is3D()) // same as LocationValid() from script
   {
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario()))
               << "Assessing track with invalid location: bearing track? Location estimation will be estimate for trackid "
               << aTrackPtr->GetTrackId() << std::endl;
      }
   }
   else
   {
      bm_track_message.SetAltitudeReliable(true);
   }

   if (!aTrackPtr->VelocityValid())
   {
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Track: " << aTrackPtr->GetTrackId() << " doesn't have velocity set, BM will not be able to estimate projections correctly for track" << std::endl;
      }
   }

   bm_track_message.SetVelocityECEF(aTrackPtr->GetVelocityWCS());

   if (!aTrackPtr->mStateCovariancePtr.IsValid())
   {
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Track: " << aTrackPtr->GetTrackId() << " doesn't have state covariance set, BM will not be able to estimate projections correctly for track" << std::endl;
      }
   }
   else
   {
      bm_track_message.SetCovarianceMatrix(*aTrackPtr->GetStateCovariance());
   }

   if (!WsfBMUtils::HeadingValid(aTrackPtr))
   {
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Track: " << aTrackPtr->GetTrackId() << " doesn't have heading set, BM will not be able to estimate projections correctly for track" << std::endl;
      }
   }
   else
   {
      bm_track_message.SetHeadingENUDegs(WsfBMUtils::GetHeading(aTrackPtr));
   }

   auto target = WsfBMUtils::GetTarget(aSimPtr, aTrackPtr);
   bm_track_message.SetTargetTruthInfo(target);

   // TTD: add type mapping option
   std::string type_string = "**error**";
   if (aTrackPtr->TypeIdValid())
   {
      type_string = aTrackPtr->GetTypeId();
   }
   else if (target)
   {
      type_string = target->GetTypeId();
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Warning track input but no type is associated, using platform truth - " << type_string << std::endl;
      }
   }
   else
   {
      type_string = "unknown";
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Warning track input but no type is associated and not target truth, forcing to " << type_string << std::endl;
      }
   }

   // if we have aux data on the actual track or platform for subtype, use it. Track takes precedent over platform
   std::string subtype_string = "**error**";
   if (WsfBMUtils::AuxDataExists(aTrackPtr, "TARGET_SUBTYPE"))
   {
      subtype_string = aTrackPtr->GetAuxData().GetString("TARGET_SUBTYPE");
   }
   else if (target && WsfBMUtils::AuxDataExists(target, "TARGET_SUBTYPE"))
   {
      subtype_string = target->GetAuxData().GetString("TARGET_SUBTYPE");
   }
   else
   {
      subtype_string = "unknown";
      if (aLogWarnings)
      {
         HCL_WARN_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(aSimPtr->GetScenario())) << "Unable to determine track subtype, forcing to " << subtype_string << std::endl;
      }
   }

   bm_track_message.SetTargetType(type_string, subtype_string);

   // The tracker only reports updates to us so assume normal state
   bm_track_message.SetTrackingStatus(WsfBMTrackMessage::STATUS_NORMAL);

   bm_track_message.SetIFF(aTrackPtr->GetIFF_Status());

   if (WsfBMUtils::AuxDataExists(aTrackPtr, "MANEUVERING"))
   {
      bm_track_message.SetManeuveringFlag(aTrackPtr->GetAuxData().GetBool("MANEUVERING"));
   }

   if (WsfBMUtils::AuxDataExists(aTrackPtr, "QUANTITY")) // JLK - delete this? should group track be handled differently?
   {
      bm_track_message.SetQuantity(aTrackPtr->GetAuxData().GetInt("QUANTITY"));
   }

   // TTD: do we need to add jamming to the track script? right now, only the jamming bool is added but no power.
   bm_track_message.SetJamming(false);
   bm_track_message.SetJammingPower(0);

   // Set the reporting sensor type of the track to the highest quality sensor type that has actively contributed to the track
   //  If the old sensor type was a higher quality, then print a warning
   il::SensorEnum trackSensorType = WsfBMUtils::GetTrackSensorType(aSimPtr, aTrackPtr);
   bm_track_message.SetReportingSensorType(trackSensorType);

   return bm_track_message;
}

// Equivalent to UT_DEFINE_SCRIPT_METHOD(WsfScriptPlatformClass, WsfPlatform, OrientationWCS, 0, "Vec3", "") since WsfPlatform doesn't have this method natively
// Does check plat for nullptr.
UtVec3d WsfBMUtils::GetOrientationWCS(const double sim_time_s, WsfPlatform* plat)
{
   double simTime = sim_time_s;
   UtVec3d wcs;
   if (plat)
   {
      plat->Update(simTime);        // Ensure source platform position is current
      plat->GetOrientationWCS(wcs[0], wcs[1], wcs[2]);
      wcs[0] *= UtMath::cDEG_PER_RAD;
      wcs[1] *= UtMath::cDEG_PER_RAD;
      wcs[2] *= UtMath::cDEG_PER_RAD;
   }
   return wcs;
}

// Does check for nullptr.
int WsfBMUtils::GetPlatformMaxAssignments(const WsfPlatform* plat)
{
   int max_assignments = 0;

   if (auto asset_manager = LocateProcessorOfType<WsfAssetManager>(plat, WsfScriptAssetManagerClass::BASE_CLASS_NAME, false))
   {
      max_assignments = asset_manager->GetMaxAssignments();
   }

   return max_assignments;
}

// Does check for nullptr.
int WsfBMUtils::GetPlatformNumAssignments(const WsfPlatform* plat)
{
   int num_assignments = 0;

   if (auto asset_manager = LocateProcessorOfType<WsfAssetManager>(plat, WsfScriptAssetManagerClass::BASE_CLASS_NAME, false))
   {
      num_assignments = asset_manager->GetCurrentNumAssignments();
   }

   return num_assignments;
}

// Does check for nullptr.
double WsfBMUtils::GetPlatformNumAssignmentDelay(const WsfPlatform* plat)
{
   double delay = 0;

   if (auto asset_manager = LocateProcessorOfType<WsfAssetManager>(plat, WsfScriptAssetManagerClass::BASE_CLASS_NAME, false))
   {
      delay = asset_manager->GetAssignmentDelaySecs();
   }

   return delay;
}

// Does check platform and weapon for nullptr.
unsigned int WsfBMUtils::GetWeaponIndexOnPlatform(const WsfPlatform* platform, const WsfWeapon* weapon)
{
   if (platform && weapon)
   {
      auto weapon_name = weapon->GetName();
      for (unsigned int widx = 0; widx < platform->GetComponentCount<WsfWeapon>(); ++widx)
      {
         if (platform->GetComponentEntry<WsfWeapon>(widx)->GetName() == weapon_name)
         {
            return widx;
         }
      }

      throw UtException("GetWeaponIndexOnPlatform invalid weapon: " + weapon->GetName() + " on platform: " + platform->GetName());
   }

   return 0;
}

// Does check track for nullptr.
bool WsfBMUtils::BelievedAlive(WsfSimulation* sim, const WsfLocalTrack* track)
{
   if (!track)
   {
      return false;
   }

   auto target = sim->GetPlatformByIndex(track->GetTargetIndex());
   return target && !(target->IsBroken() || target->IsDeleted());
}

// track must be valid and !BelievedAlive
// Does check track for nullptr.
bool WsfBMUtils::ConfirmedDead(WsfSimulation* sim, const WsfLocalTrack* track)
{
   return track && !BelievedAlive(sim, track);
}
