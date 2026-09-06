// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   assetRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *  @brief Class to represent players.
 *
 *  Classification: UNCLASSIFIED
 */

#ifndef IADSLIB_ASSETRECORD_HPP
#define IADSLIB_ASSETRECORD_HPP

#include <map>
#include <vector>
#include <tuple>
#include <memory>
#include <functional>

#include <iadsLib/enum.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/positionRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>
#include <iadsLib/zoneRecord.hpp>

#include <iadsLib/combatStatusMessage.hpp>
#include <vclMath/Vector3.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/sensorRecordInterface.hpp>

/**
 *  @brief Encapsulate IADS functions.
 */
namespace il
{
   class assetRecord;

   typedef std::vector<std::shared_ptr<weaponRecord>> tReadyWeapons;

   class IADSLIB_EXPORT assetMap
   {
      public:
         void UpdateAge(const double sim_time_s);
         ~assetMap() = default;
         std::shared_ptr<assetRecord> GetAsset(const idRecord& id) const;
         void AddAsset(const idRecord& id, const std::shared_ptr<assetRecord>& asset);
         std::vector<std::shared_ptr<const assetRecord>> GetAssets() const;
         std::vector<std::shared_ptr<const assetRecord>> GetAssets(std::function<bool(const std::shared_ptr<const assetRecord> &)> pred) const;

      protected:
         std::map<idRecord, std::shared_ptr<assetRecord>> m_assets;
   };


   /**
    *  @brief Class to represent assets.
    */
   class IADSLIB_EXPORT assetRecord
   {
      public:
         enum GTIPLRUnitType
         {
            GTI_COMMAND_POST = 0,
            GTI_AIR_COMMAND_POST = 1,
            GTI_SAM_COMMAND_POST = 2,
            GTI_AIR_BASE = 3,
            GTI_FIGHTER_CONTROLLER = 4,
            GTI_FIGHTER = 5,
            GTI_SFU = 6
         };

         enum GTIPLRWeaponModeType
         {
            GTIPLR_WEAPONS_UNDEFINED = 0,
            GTIPLR_WEAPONS_FREE = 1,
            GTIPLR_WEAPONS_HOLD = 2,
            GTIPLR_WEAPONS_TIGHT = 3
         };

         enum GTIPLRControlModeType
         {
            GTIPLR_CONTROL_UNDEFINED = 0,
            GTIPLR_CONTROL_CENTRALIZED = 1,
            GTIPLR_CONTROL_DECENTRALIZED = 2,
            GTIPLR_CONTROL_AUTONOMOUS = 3
         };

         /**
            @brief Default constructor.
         */
         assetRecord(std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                     const std::string& Name = "Unknown",
                     const std::string& Side = "Unknown",
                     const float& DefensePriority = 0.0f,
                     const double& AgeYellowTime = 15.0,
                     const double& AgeRedTime = 30.0,
                     const double& AssignmentDelay = 10.0,
                     const unsigned int& MaxAssignments = 0,
                     const bool& SaturationMode = false);

         void addDirectSubordinate(const idRecord& SubordinateAssetID, assetMap& Assets);
         void removeDirectSubordinate(const idRecord& SubordinateAssetID, assetMap& Assets);
         inline std::vector< idRecord > getDirectSubordinates() const { return m_Subordinates; }
         std::vector< idRecord > getSubordinates(assetMap& Assets) const;

         void addDirectPeer(const idRecord& AssetID, assetMap& Assets);
         void removeDirectPeer(const idRecord& AssetID, assetMap& Assets);
         std::vector< idRecord > getDirectPeers() const { return m_Peers; }

         void addDirectCommander(const idRecord& AssetID, assetMap& Assets);
         void removeDirectCommander(assetMap& Assets);

         bool isDirectSubordinate(const idRecord& AssetID) const;
         bool isDirectPeer(const idRecord& AssetID) const;
         bool isDirectCommander(const idRecord& AssetID) const;
         bool isInCommandChain(const idRecord& AssetID, const assetMap& Assets) const;
         bool isSubordinate(const idRecord& AssetID, const assetMap& Assets) const;

         unsigned int getNumDirectSubordinates() const;
         unsigned int getNumDirectPeers() const;
         unsigned int getNumSubordinates(const assetMap& Assets) const;

         // find the next subordinate on the path between from_asset to to_asset - returns invalid idRecord if from_asset == to_asset
         idRecord findNextSubordinateInChain(const idRecord& AssetID, const assetMap& Assets) const;
         // find the next commander on the path between from_asset to to_asset - returns invalid idRecord if from_asset == to_asset
         idRecord findNextCommanderInChain(const idRecord& AssetID, const assetMap& Assets) const;


         void updateAge(const double& simTime);
         // should only really be called when updating own platform since we don't self-send status messages. All other
         // times should come naturally through combatStatus message updates.
         void setStatusTime(const double& time);
         double getStatusTime() const;

         void setPositionTime(const double& time);
         double getPositionTime() const;

         void processMessage(const il::combatStatusMessage& csm, const bool unit_has_assignments);
         std::shared_ptr<il::combatStatusMessage> generateCombatStatusMessage() const;

         void logMyStatus() const;
         void logSubordinateStatus(const assetMap& Assets, bool direct_only = true) const;

         void setID(const idRecord& ID);
         const idRecord& getID() const;

         bool hasCommander() const;
         void setCommanderID(const idRecord& ID);
         const idRecord& getCommanderID() const;

         bool doesCommanderUseZone(const assetMap& Assets) const;
         static bool doesAnyChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end);
         static bool doesAllChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end);
         static bool doesAllChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offenders);
         static bool doesAllChainOfCommandHaveReadyAssignments(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end);
         static bool doesAllChainOfCommandHaveReadyAssignments(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offenders);
         static bool isAnyChainOfCommandRed(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end);
         static bool isAnyChainOfCommandRed(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offender);
         static double calculateAssignmentDelays(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end);

         /**
            @brief Set the name of the asset.
            @param name Name to assign to asset.
         */
         void setName(const std::string& name);

         /**
            @brief Get the name of the asset.
            @returns Name of the asset.
         */
         const std::string& getName() const;

         /**
            @brief Set the team name of the asset.
            @param team Team name to assign to asset.
         */
         void setSide(const std::string& team);

         /**
            @brief Get the team name of the asset.
            @returns Tean name of the asset.
         */
         const std::string& getSide() const;

         /**
            @brief Set the type of the asset.
            @param Type Type/SubType to assign to asset.
         */
         void setType(const unitTypeRecord& Type);

         /**
            @brief Get the type of the asset.
            @returns Type/SubType of the asset.
         */
         const unitTypeRecord& getType() const;


         /**
            @brief Set asset's position in ECEF(m).
            @param x ECEF X position(m).
            @param y ECEF Y position(m).
            @param z ECEF Z position(m).
         */
         void setPositionXYZ(const double& x, const double& y, const double& z);

         /**
            @brief Set asset's position in ECEF(m).
            @param xyz Position in ECEF(m).
            @returns
         */
         void setPositionXYZ(const vm::Vector3<double>& xyz);

         /**
            @brief Get asset's position in ECEF(m).
            @returns Assets' position in ECEF(m).
         */
         const vm::Vector3<double>& getPositionXYZ() const;

         void getPositionXYZ(double& X, double& Y, double& Z) const;

         /**
            @brief Get asset's Lat/Lon/Alt position.
            @returns Asset's Lat/Lon/Alt position.
         */
         const vm::Vector3<double>& getPositionLLA() const;

         /**
            @brief Set asset's Lat/Lon/Alt position.
            @param lat Asset's latitude in radians.
            @param lon Asset's longitude in radians.
            @param alt Asset's altitude in meters.
         */
         void setPositionLLA(const double& lat, const double& lon, const double& alt);

         /**
            @brief Set asset's Lat/Lon/Alt position.
            @param lat Asset's latitude in degrees.
            @param lon Asset's longitude in degrees.
            @param alt Asset's altitude in meters.
         */
         void setPositionLLADegrees(const double& lat, const double& lon, const double& alt);
         const positionRecord& getPosition() const;


         /**
            @brief Set asset's velocity in ECEF(m/s).
            @param x ECEF X velocity(m/s).
            @param y ECEF Y velocity(m/s).
            @param z ECEF Z velocity(m/s).
         */
         void setVelocityXYZ(const double& x, const double& y, const double& z);

         /**
            @brief Set asset's velocity in ECEF(m/s).
            @param xyz velocity in ECEF(m/s).
            @returns
         */
         void setVelocityXYZ(const vm::Vector3<double>& xyz);

         /**
            @brief Get asset's velocity in ECEF(m/s).
            @returns Assets' velocity in ECEF(m/s).
         */
         vm::Vector3<double> getVelocityXYZ() const;
         void getVelocityXYZ(double& x, double& y, double& z) const;
         double getSpeed() const;

         /**
            @brief Get references to all zones attached on the asset-level.
            @returns Const references to zones.
         */
         const std::vector<std::shared_ptr<zoneRecordInterface>> GetZones() const;

         /**
            @brief Get references to all zones of the specified type attached on the asset-level.
            @returns Const references to zones.
         */
         const std::vector<std::shared_ptr<zoneRecordInterface>> GetZones(const eZoneType& type) const;

         /**
            @brief Get references to defended zones attached on the asset-level.
            @returns Const references to zones.
         */
         const std::vector<std::shared_ptr<zoneRecordInterface>> GetDefendedZones() const;

         /**
            @brief Add a defended zone attached to this asset.
            @param zone Zone to assign.
         */
         // Note: there's two kinds of attachments that can be done - a unit-level zone and a weapon-level zone.
         // How these are handled is internal to the battle managers
         void AddZone(const std::shared_ptr<zoneRecordInterface>& zone);

         void AddWeapon(const std::shared_ptr<weaponRecord>& weapon);

         void AddSensor(const std::shared_ptr<sensorRecord>& sensor);

         /**
            @brief Query if a zone has been assigned to this asset.
            @returns true if zone has been assigned, false otherwise.
         */
         bool hasDefendedZone() const;

         bool hasZones() const;

         /**
            @brief Setters/getters for managers.
         */
         void setHasAssetManager(const bool has_manager);
         bool hasAssetManager() const;

         void setHasBattleManager(const bool has_manager);
         bool hasBattleManager() const;

         void setHasSensorManager(const bool has_manager);
         bool hasSensorManager() const;

         void setHasWeaponManager(const bool has_manager);
         bool hasWeaponManager() const;

         void setHasC2DisseminationManager(const bool has_manager);
         bool hasC2DisseminationManager() const;


         /**
            @brief Set if an asset is defended. This is independent from having defended zones since an asset could be zoneless but defended.
            @brief This is primarily for backwards compatibility with the VCL original battle manager.
         */
         inline void setDefended(const il::tZonePriority priority) { m_isDefended = true, m_default_defended_priority = priority; }
         inline void setDefaultDefendedPriority(const il::tZonePriority priority) { m_default_defended_priority = priority; }
         inline bool isDefended() const { return m_isDefended || hasDefendedZone(); }
         inline il::tZonePriority getDefaultDefendedPriority() const { return m_default_defended_priority; }


         /**
           @brief Set if an asset is C2 capable. If true then this unit can be considered
           @brief to be able to handle all C2 messages: Tracks, Assignments, Asset Status, and Assignment Status
           @brief and fufill the expected interface as expected by the battle managers
         */
         inline void setC2Capable(const bool is_capable = true) { m_IsC2Capable = is_capable; }
         inline bool getC2Capable() const { return m_IsC2Capable; }

         /**
            @brief Query whether asset has a weapon or not.
            @return True if asset has a weapon, false otherwise.
         */
         bool hasWeapon() const;
         bool hasMunitions(const bool require_all_weapons = false) const;
         std::size_t getNumWeapons() const;
         // 1-based
         std::shared_ptr<weaponRecord> getNthWeapon(const size_t nth_weapon) const;
         std::vector<std::shared_ptr<weaponRecord>> getWeapons() const;

         std::shared_ptr<weaponRecord> getWeapon(const idRecord& weapon_id) const;

         void updatePerceptions(const tWeaponID& weaponID);

         /**
            @brief Query whether asset has a sensor or not.
            @return True if asset has a sensor, false otherwise.
         */
         bool hasSensor() const;
         std::size_t getNumSensors() const;
         // 1-based
         std::shared_ptr<sensorRecord> getNthSensor(const size_t nth_sensor) const;
         std::shared_ptr<sensorRecord> getNthSensorOfType(const size_t nth_sensor, const il::SensorEnum sensor_type) const;
         std::shared_ptr<sensorRecord> getSensor(const tSensorID& sensor_id) const;
         std::vector<std::shared_ptr<sensorRecord>> getSensors() const;

         // max assignments in this case refers to the unit-level, the weapon has a separate fire channel concept
         void setMaxAssignments(const unsigned int& num);
         const int getMaxAssignments() const;
         void setNumAssignments(const int NumAssignments);
         int getNumAssignments() const;
         int getNumOpenAssignments() const;
         bool hasOpenAssignments() const;

         enum eStatusIncrFlag { DECREMENT = 0, INCREMENT = 1 };
         void updateC2ChainAssignedUnitStatus(const idRecord& EndAssetID, assetMap* Assets, const eStatusIncrFlag incr_flag, const int assignments_incr, const int munitions_incr);

         void setAssignmentDelay(const double& delay);
         const double& getAssignmentDelay() const;

         void setAggregateUnit(const bool aggregate_unit);
         bool getAggregateUnit() const;

         void setSystemStatus(const SYSTEM_STATUS Status, const bool External = false);
         SYSTEM_STATUS getSystemStatus() const;
         void setExternallyDefinedSystemStatus(const bool External);

         SYSTEM_STATUS getAssetStatus(const idRecord& AssetID, const assetMap& Assets);

         void setSaturationMode(const bool& SaturationMode = true);
         const bool& inSaturationMode() const;

         bool isCombatReady() const;

         std::tuple<tReadyWeapons, std::vector<double>>  getReadyWeaponList(const assetMap& Assets) const;

         std::tuple<tReadyWeapons, std::vector<double>> buildReadyWeaponList(const assetRecord* Asset, const assetMap& Assets) const;

         /// \brief Gets the relative age when the status changes to yellow
         /// \return The relative age when the status changes to yellow (sec)
         double getAgeYellow(void) const;

         /// \brief Sets the relative age when the status changes to yellow
         /// \param aTime The relative age when the status changes to yellow (sec)
         void setAgeYellow(const double& aTime);

         /// \brief Gets the relative age when the status changes to red
         /// \return The relative age when the status changes to red (sec)
         double getAgeRed(void) const;

         /// \brief Sets the relative age when the status changes to red
         /// \param aTime The relative age when the status changes to red (sec)
         void setAgeRed(const double& aTime);


         // These are proxy interfaces for all of the HELIOS status flags, our approach is a little more dynamic in that we return the statuses on demand
         // versus just storing the state
         SYSTEM_STATUS getC2Status() const;
         // note: commo status appears to not be used in HELIOS anymore
         SYSTEM_STATUS getWeaponStatus(const bool require_all_weapons = false) const;
         SYSTEM_STATUS getWeaponStatus(const assetMap& assets, const bool require_all_weapons = false) const;
         SYSTEM_STATUS getEWStatus() const;
         SYSTEM_STATUS getEWStatus(const assetMap& assets) const;
         SYSTEM_STATUS getTARStatus() const;
         SYSTEM_STATUS getTARStatus(const assetMap& assets) const;
         SYSTEM_STATUS getTTRStatus() const;
         SYSTEM_STATUS getTTRStatus(const assetMap& assets) const;
         SYSTEM_STATUS getStationaryStatus() const;


         bool isGreen() const;
         bool isYellow() const;
         bool isRed() const;


         // updates status of all subordinates of this asset based on last status time
         void checkSubordinatesTimeout(assetMap& assets, const double current_time);

         bool checkSubordinateStatus(const assetMap& assets, std::string& not_green_list);

         // generic BFS subordinate invoker
         void foreachSubordinateBFS(const assetMap& assets, std::function<void(std::shared_ptr<assetRecord>)> invokefun);

         void setLogMyStatus(const bool log_status) { m_LogMyStatus = log_status; }
         bool getLogMyStatus() const { return m_LogMyStatus; }

         // Helpers
         virtual void IncrementAssetAssignments(const int amount);
         virtual void DecrementAssetAssignments(const int amount);

         virtual bool   HasMinRangeAttribute() const;
         virtual double GetMinRange() const;
         virtual void   SetMinRange(const double min_range);
         virtual bool   HasMaxRangeAttribute() const;
         virtual double GetMaxRange() const;
         virtual void   SetMaxRange(const double max_range);

         std::weak_ptr<logger::GlobalLogger> getGlobalLogger() const { return mGlobalLogger; }

      protected:
         /// Unique ID for this asset.
         idRecord m_ID;

         /// Commander of this asset
         idRecord m_CommanderID;

         /// Direct subordinates of this asset.
         std::vector< idRecord > m_Subordinates;

         /// Direct peers of this asset.
         std::vector< idRecord > m_Peers;

         /// Name of asset.
         std::string m_Name;

         /// side of the asset
         std::string m_Side;

         /// Is the asset defended or not.
         bool m_isDefended;
         tZonePriority m_default_defended_priority;

         /// Zone data, if assigned.
         zoneContainer m_attached_zones;

         /// Type/SubType of asset.
         unitTypeRecord m_Type;


         /// Relative time to age out
         double m_AgeYellowTime;
         double m_AgeRedTime;

         /// Time for asset to accept a fire command.
         double m_AssignmentDelay;

         /// If set, indicates this unit is to be considered an aggregate unit in which any subordinates
         /// that report a failed status cause this unit to report a failed status
         bool m_aggregate_unit;

         /// Number of maximum assignments asset can handle.
         int m_MaxAssignments;

         /// Number of total assigments the asset currently has.
         int m_NumAssignments;

         /// System status
         SYSTEM_STATUS m_SystemStatus;

         // Flag indicating if this unit's status has been set externally (true), in which case the status
         // will not be updated internally again until this flag is reset to false.
         bool m_ExternallyDefinedSystemStatus;

         bool m_SaturationMode;

         /// Time of last status report.
         double m_StatusTime;

         /// Time of last position report.
         double m_PositionTime;

         positionRecord m_Position;
         vm::Vector3<double> m_Velocity; // ECEF m/s

         /// Flag indicating if this unit is C2-capable. If true then this unit can be considered
         /// to be able to handle all C2 messages: Tracks, Assignments, Asset Status, and Assignment Status
         /// and fufill the expected interface as expected by the battle managers
         bool m_IsC2Capable;

         std::map<tWeaponID, std::shared_ptr<weaponRecord>> m_weapons;
         std::map<tSensorID, std::shared_ptr<sensorRecord>> m_sensors;

         // manager classes
         bool m_has_asset_manager;
         bool m_has_battle_manager;
         bool m_has_sensor_manager;
         bool m_has_weapon_manager;
         bool m_has_disseminate_c2_manager;

         /// Flag indicating if this unit should output its status
         bool m_LogMyStatus;

         double m_MinRange;
         double m_MaxRange;

         std::weak_ptr<logger::GlobalLogger> mGlobalLogger;

      private:

   };

}

#endif

