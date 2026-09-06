// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_UTILS_HPP
#define WSF_BM_UTILS_HPP

#include <map>
#include <string>
#include <set>
#include <memory>

#include <WsfBMCommon.hpp>
#include <WsfObject.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfBMTrackMessage.hpp>
#include <WsfPlatform.hpp>
#include <WsfTrackId.hpp>
#include <iadsLib/idRecord.hpp>
#include <WsfQuantumTask.hpp>
#include <iadsLib/baseMessage.hpp>
#include <WsfProcessor.hpp>
#include <WsfBattleManager.hpp>
#include <WsfTrackManager.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfMessage.hpp>
#include <UtVec3.hpp>
#include <iadsLib/enum.hpp>

// forward declarations
class WsfBMAssignmentStatusMessage;
class WsfSimulation;
class WsfAssetManager;
class WsfBattleManager;
class WsfBMDisseminateC2;
class WsfBMWeaponsManager;
class WsfBMSensorsManager;
class WsfWeapon;

namespace il
{
   class BattleManagerInterface;
   class AssetManagerInterface;
   class DisseminateC2Interface;
   class SensorsManagerInterface;
   class WeaponsManagerInterface;
}

class WSF_IADS_C2_LIB_EXPORT WsfBMUtils : public WsfObject
{
   public:

      WsfBMUtils* Clone() const override;
      const char* GetScriptClassName() const override { return "WsfBMUtils"; }

      static int GetStringId(const std::string& str_Value);
      static std::string GetStringFromId(const int stringid);

      static bool IsTypeUnknown(const std::string& type_str);
      static bool IsSubtypeUnknown(const std::string& subtype_str);

      static bool IsTypeAny(const std::string& type_str);
      static bool IsSubtypeAny(const std::string& subtype_str);

      static int GetUnknownType();
      static int GetUnknownSubtype();

      static int GetType(const std::string& type_str);
      static int GetSubType(const std::string& subtype_str);

      static std::string GetSpatialDomain(const WsfPlatform* platform);

      static WsfTrackId ConvertTrackId(const il::idRecord& il_record);
      static il::idRecord ConvertTrackId(const WsfTrackId& track_id);
      static il::idRecord ConvertTrackId(const int owning_plat_string_id, const int tan);

      static std::size_t GetTracksUpdated(const WsfAssetManager* am_proc, double sim_time, std::vector<WsfLocalTrack*>& tracks);
      static bool IsTrackGeneratedByLocalSensor(WsfPlatform* relative_to_platform, const WsfTrackId& track_id, il::SensorEnum reporting_sensor_type);

      static WsfPlatform* GetMessageDestination(/* const */ WsfSimulation* sim_ptr, const std::shared_ptr<const il::baseMessage>& message);

      static int GetUnitIDFromPlatform(const WsfPlatform* platform);
      static il::idRecord GetUnitIDRecordFromPlatform(const WsfPlatform* platform);
      static il::idRecord GetUnitIDRecordFromPlatformIndex(/* const */ WsfSimulation* sim_ptr, const int plat_idx);
      static WsfPlatform* GetPlatformFromUnitID(/* const */ WsfSimulation* sim_ptr, const int id);
      static WsfPlatform* GetPlatformFromUnitIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& id);
      // returns true if platforms have a common commander on any command chain
      static bool ArePlatformsPeers(const WsfPlatform* plat1, const WsfPlatform* plat2);
      static bool IsDirectCommander(const WsfPlatform* test_cmdr, const WsfPlatform* against_plat, const std::string& opt_cmd_chain_name = "");
      static WsfPlatform* GetCommander(const WsfPlatform* from, const std::string& opt_cmd_chain_name = "");
      static WsfPlatform* GetTopMostCommander(const WsfPlatform* from, const std::string& opt_cmd_chain_name = "");
      static bool BelievedAlive(/*const*/ WsfSimulation* sim, const WsfLocalTrack* track);
      static bool ConfirmedDead(/*const*/ WsfSimulation* sim, const WsfLocalTrack* track);

      static  il::idRecord GetAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const int plat_idx, const int wpn_idx);
      static  il::idRecord GetAssignedIDRecord(const WsfPlatform* plat, const WsfWeapon* weapon);
      static WsfPlatform* GetAssignedPlatformFromAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& assigned_id);
      static WsfWeapon* GetAssignedWeaponFromAssignedIDRecord(/* const */ WsfSimulation* sim_ptr, const il::idRecord& assigned_id);

      static WsfWeapon* GetWeapon(const WsfPlatform* platform, const std::string& weapon_name);
      static il::idRecord GetWeaponID(const WsfPlatform* platform, const std::string& weapon_name);
      static il::idRecord GetWeaponID(const WsfPlatform* platform, const WsfWeapon* weapon);
      static il::idRecord GetWeaponID(/* const */ WsfSimulation* sim_ptr, const int plat_idx, const int wpn_idx);
      static bool IsWeaponIDValid(const int id);

      static WsfSensor* GetSensor(const WsfPlatform* platform, const std::string& sensor_name);
      static WsfSensor* GetSensor(const WsfPlatform* platform, const int sensor_id);
      static int GetSensorID(/*const*/ WsfPlatform* platform, const std::string& sensor_name);
      static il::idRecord GetSensorID(const WsfPlatform* platform, const WsfSensor* sensor);
      static il::idRecord GetSensorID(/* const */ WsfSimulation* sim_ptr, const int plat_idx, const int snsr_idx);
      static il::SensorEnum GetSensorType(const WsfSensor& sensor);
      static il::SensorEnum GetTrackSensorType(WsfSimulation* sim, const WsfLocalTrack* track);
      static il::SensorEnum GetTrackSensorType(WsfSimulation* sim, const WsfLocalTrack* track, std::set<WsfTrackId>& oldTracks);
      static std::vector<const WsfTrack*> GetOriginalTracks(WsfSimulation* sim, const WsfLocalTrack* track);
      static void GetOriginalTracks(WsfSimulation* sim, const WsfLocalTrack* track, std::set<WsfTrackId>& tested_tracks, std::vector<const WsfTrack*>& original_tracks);
      static il::SensorEnum GetRawTrackSensorType(WsfSimulation* sim, const WsfPlatform* platform, const WsfTrack* raw_track);

      static void SetAssignmentStatusFromAuxDataString(WsfBMAssignmentStatusMessage* assignment_status, const std::string& status_string);
      static std::string AssignmentStatusToAuxDataString(const WsfBMAssignmentStatusMessage* assignment_status);

      static bool IsAssignedBy(const WsfPlatform* plat, const WsfBMAssignmentMessage& message);
      static WsfLocalTrack* FindCorrelatedTrack(/* const */ WsfTrackManager* trk_mgr, const WsfTrackId* aRawTrackId);

      static std::shared_ptr<il::AssetManagerInterface> FindAttachedAssetManagerRef(const WsfPlatform* platform, const bool log_errors = true);
      static std::shared_ptr<il::BattleManagerInterface> FindAttachedBattleManagerRef(const WsfPlatform* platform, const bool log_errors = true);
      static std::shared_ptr<il::DisseminateC2Interface> FindAttachedDisseminationManagerRef(const WsfPlatform* platform, const bool log_errors = true);
      static std::shared_ptr<il::SensorsManagerInterface> FindAttachedSensorsManagerRef(const WsfPlatform* platform, const bool log_errors = true);
      static std::shared_ptr<il::WeaponsManagerInterface> FindAttachedWeaponsManagerRef(const WsfPlatform* platform, const bool log_errors = true);
      static WsfAssetManager* FindAttachedAssetManagerRef2(const WsfPlatform* platform, const bool log_errors = false);
      static WsfBattleManager* FindAttachedBattleManagerRef2(const WsfPlatform* platform, const bool log_errors = false);
      static WsfBMDisseminateC2* FindAttachedDisseminationManagerRef2(const WsfPlatform* platform, const bool log_errors = false);
      static WsfBMSensorsManager* FindAttachedSensorsManagerRef2(const WsfPlatform* platform, const bool log_errors = false);
      static WsfBMWeaponsManager* FindAttachedWeaponsManagerRef2(const WsfPlatform* platform, const bool log_errors = false);

      // Aux Data-related calls.
      static bool AuxDataExists(const WsfPlatform* platform, const std::string& attrib_name); // not needed in script because this call is supported in script but here it's a helper for C++
      static bool AuxDataExists(const WsfWeapon* weapon, const std::string& attrib_name);
      static bool AuxDataExists(const WsfTrack* track, const std::string& attrib_name);
      static bool AuxDataExists(const WsfMessage& message, const std::string& attrib_name);

      // Track-related calls
      static double GetHeading(const WsfTrack* track);
      static bool HeadingValid(const WsfTrack* track);
      static WsfPlatform* GetTarget(WsfSimulation* sim_ptr, const WsfTrack* track);
      static WsfBMTrackMessage ConvertTrack(WsfSimulation*       aSimPtr,
                                            const WsfLocalTrack* aTrackPtr,
                                            const bool           aLogWarnings = true);

      // Platform-related calls
      static UtVec3d GetOrientationWCS(const double sim_time_s, WsfPlatform* plat);
      static int GetPlatformMaxAssignments(const WsfPlatform* plat);
      static int GetPlatformNumAssignments(const WsfPlatform* plat);
      static double GetPlatformNumAssignmentDelay(const WsfPlatform* plat);

   private:

      /*static const int UNKNOWN_TYPE;
      static const int UNKNOWN_SUBTYPE;
      static const int INVALID_ID;*/
      //const int UNKNOWN_TYPE = 0;
      //const int UNKNOWN_SUBTYPE = 0;
      //const int INVALID_ID = INVALID_UNIT_ID;

      static unsigned int GetWeaponIndexOnPlatform(const WsfPlatform* platform, const WsfWeapon* weapon);
};

#endif
