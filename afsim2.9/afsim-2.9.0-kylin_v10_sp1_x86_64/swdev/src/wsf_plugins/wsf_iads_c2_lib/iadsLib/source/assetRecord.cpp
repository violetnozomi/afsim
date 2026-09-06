// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   assetRecord.cpp
 *  @author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <algorithm>
#include <sstream>
#include <queue>
#include <type_traits>

#include <logger.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/colorText.hpp>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/util.hpp>

namespace il
{

namespace
{
   void appendAssetNameToList(std::string& name_list, const std::shared_ptr<assetRecord>& asset_ptr)
   {
      name_list.length() ? name_list.append(", " + asset_ptr->getName()) : name_list.append(asset_ptr->getName());
   }

   template <typename tPredicateFunctor>
   bool isAnyChainOfCommand(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, tPredicateFunctor any_pred, std::string& match)
   {
      bool any_meet_pred = false;

      auto curr_asset_ptr = assets.GetAsset(cmdr_begin);
      if (!curr_asset_ptr)
      {
         return false;
      }

      while (curr_asset_ptr && !any_meet_pred)
      {
         if (any_pred(curr_asset_ptr))
         {
            any_meet_pred = true;
            appendAssetNameToList(match, curr_asset_ptr);
         }

         auto next_id = curr_asset_ptr->findNextSubordinateInChain(sub_end, assets);
         curr_asset_ptr = assets.GetAsset(next_id);
      }

      return any_meet_pred;
   }

   template <typename tPredicateFunctor>
   bool isAnyNotChainOfCommand(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, tPredicateFunctor any_not_pred, std::string& match)
   {
      bool any_not = false;

      auto curr_asset_ptr = assets.GetAsset(cmdr_begin);
      if (!curr_asset_ptr)
      {
         return false;
      }

      while (curr_asset_ptr && !any_not)
      {
         if (any_not_pred(curr_asset_ptr))
         {
            any_not = true;
            appendAssetNameToList(match, curr_asset_ptr);
         }

         auto next_id = curr_asset_ptr->findNextSubordinateInChain(sub_end, assets);
         curr_asset_ptr = assets.GetAsset(next_id);
      }

      return any_not;
   }

   template <typename tMapFoldFunctor>
   void subordinateMapFold(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, tMapFoldFunctor& mapfold_func)
   {
      auto curr_asset_ptr = assets.GetAsset(cmdr_begin);
      if (!curr_asset_ptr)
      {
         return;
      }

      while (curr_asset_ptr)
      {
         mapfold_func(curr_asset_ptr);
         auto next_id = curr_asset_ptr->findNextSubordinateInChain(sub_end, assets);
         curr_asset_ptr = assets.GetAsset(next_id);
      }

      return;
   }

   void foreachSubordinate(const assetMap& assets, const idRecord& cmdr_begin, std::function<void(std::shared_ptr<assetRecord>)>& invokefun)
   {
      // this will be a bare-bones traditional BFS implementation. Note that we're not decorating the
      // graph to keep track of distance, color, or parent for things like shortest path and the like, just run the graph
      // and invoke the desired function
      std::queue<std::shared_ptr<assetRecord>> Q;

      auto pop_front = [](decltype(Q) &queue) -> std::remove_reference<decltype(Q.front())>::type
      {
         auto value = queue.front();
         queue.pop();

         return value;
      };


      auto v = assets.GetAsset(cmdr_begin);
      if (v)
      {
         Q.push(v);
      }

      while (!Q.empty())
      {
         auto u = pop_front(Q);
         auto subordinates = u->getDirectSubordinates();
         for (auto it = subordinates.begin(); it != subordinates.end(); ++it)
         {
            auto& sub_id = *it;
            auto subordinate = assets.GetAsset(sub_id);
            if (subordinate)
            {
               invokefun(subordinate);
               Q.push(subordinate);
            }
         }
      }
   }
}

assetRecord::assetRecord(std::weak_ptr<logger::GlobalLogger> aGlobalLogger,
                         const std::string& Name,
                         const std::string& Side,
                         const float& DefensePriority,
                         const double& AgeYellowTime,
                         const double& AgeRedTime,
                         const double& AssignmentDelay,
                         const unsigned int& MaxAssignments,
                         const bool& SaturationMode
                        ) :
   m_Name(Name),
   m_Side(Side),
   m_isDefended(true),
   m_default_defended_priority(0),
   m_AgeYellowTime(AgeYellowTime),
   m_AgeRedTime(AgeRedTime),
   m_AssignmentDelay(AssignmentDelay),
   m_MaxAssignments(MaxAssignments),
   m_NumAssignments(0),
   m_SystemStatus(il::STATUS_RED),
   m_ExternallyDefinedSystemStatus(false),
   m_SaturationMode(SaturationMode),
   m_StatusTime(0.0),
   m_PositionTime(0.0),
   m_IsC2Capable(false),
   m_has_asset_manager(false),
   m_has_battle_manager(false),
   m_has_sensor_manager(false),
   m_has_weapon_manager(false),
   m_has_disseminate_c2_manager(false),
   m_LogMyStatus(true),
   m_MinRange(0),
   m_MaxRange(0),
   mGlobalLogger(aGlobalLogger)
{
}

void assetRecord::setID(const idRecord& ID)
{
   m_ID = ID;
}

const idRecord& assetRecord::getID() const
{
   return m_ID;
}

bool assetRecord::hasCommander() const
{
   return (m_CommanderID.isValid());
}

void assetRecord::setCommanderID(const idRecord& ID)
{
   m_CommanderID = ID;
}

const idRecord& assetRecord::getCommanderID() const
{
   return m_CommanderID;
}

bool assetRecord::doesCommanderUseZone(const assetMap& Assets) const
{
   if (m_CommanderID.isValid())
   {
      auto commander = Assets.GetAsset(m_CommanderID);
      if (commander)
      {
         return commander->hasDefendedZone();
      }
      else
      {
         return false;
      }
   }
   else
   {
      return false;
   }
}

bool assetRecord::doesAnyChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end)
{
   std::string na_offenders;
   return isAnyChainOfCommand(assets, cmdr_begin, sub_end, [](const std::shared_ptr<assetRecord>& asset) -> bool { return asset->hasZones(); }, na_offenders);
}

bool assetRecord::doesAllChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end)
{
   std::string na_offenders;
   return doesAllChainOfCommandUseZone(assets, cmdr_begin, sub_end, na_offenders);
}

bool assetRecord::doesAllChainOfCommandUseZone(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offenders)
{
   // and All predicate check is the opposite of !any
   return !isAnyNotChainOfCommand(assets, cmdr_begin, sub_end, [](std::shared_ptr<assetRecord> asset) -> bool { return !asset->hasZones(); }, offenders);
}

bool assetRecord::doesAllChainOfCommandHaveReadyAssignments(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end)
{
   std::string na_offenders;
   return doesAllChainOfCommandHaveReadyAssignments(assets, cmdr_begin, sub_end, na_offenders);
}

bool assetRecord::doesAllChainOfCommandHaveReadyAssignments(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offenders)
{
   return !isAnyNotChainOfCommand(assets, cmdr_begin, sub_end, [](std::shared_ptr<assetRecord> asset) -> bool { return asset->getC2Capable() && asset->getNumOpenAssignments() <= 0; }, offenders);
}

bool assetRecord::isAnyChainOfCommandRed(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end)
{
   std::string na_offenders;
   return isAnyChainOfCommandRed(assets, cmdr_begin, sub_end, na_offenders);
}

bool assetRecord::isAnyChainOfCommandRed(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end, std::string& offender)
{
   return isAnyChainOfCommand(assets, cmdr_begin, sub_end, [](const std::shared_ptr<assetRecord>& asset) -> bool { return asset->isRed(); }, offender);
}

double assetRecord::calculateAssignmentDelays(const assetMap& assets, const idRecord& cmdr_begin, const idRecord& sub_end)
{
   double total_delay_s = 0;
   auto delay_calc = [&total_delay_s](const std::shared_ptr<assetRecord>& asset) -> void
   {
      total_delay_s += asset->getAssignmentDelay();
   };

   subordinateMapFold(assets, cmdr_begin, sub_end, delay_calc);

   return total_delay_s;
}


void assetRecord::setName(const std::string& name)
{
   m_Name = name;
}

const std::string& assetRecord::getName() const
{
   return m_Name;
}

void assetRecord::setSide(const std::string& team)
{
   m_Side = team;
}

const std::string& assetRecord::getSide() const
{
   return m_Side;
}

void assetRecord::setType(const unitTypeRecord& Type)
{
   m_Type = Type;
}

const unitTypeRecord& assetRecord::getType() const
{
   return m_Type;
}

void assetRecord::setPositionXYZ(const double& x,
                                 const double& y,
                                 const double& z)
{
   m_Position.setXYZ(x, y, z);
}

void assetRecord::setPositionXYZ(const vm::Vector3<double>& xyz)
{
   m_Position.setXYZ(xyz);
}

const vm::Vector3<double>& assetRecord::getPositionXYZ() const
{
   return m_Position.getXYZ();
}

void assetRecord::getPositionXYZ(double& X, double& Y, double& Z) const
{
   m_Position.getXYZ(X, Y, Z);
}

const vm::Vector3<double>& assetRecord::getPositionLLA() const
{
   return m_Position.getLLA();
}

void assetRecord::setPositionLLA(const double& lat,
                                 const double& lon,
                                 const double& alt
                                )
{
   m_Position.setLLA(lat, lon, alt);
}

void assetRecord::setPositionLLADegrees(const double& lat,
                                        const double& lon,
                                        const double& alt
                                       )
{
   m_Position.setLLADegrees(lat, lon, alt);
}

const positionRecord& assetRecord::getPosition() const
{
   return m_Position;
}


void assetRecord::setVelocityXYZ(const double& x, const double& y, const double& z)
{
   m_Velocity[0] = x;
   m_Velocity[1] = y;
   m_Velocity[2] = z;
}

void assetRecord::setVelocityXYZ(const vm::Vector3<double>& xyz)
{
   m_Velocity = xyz;
}

vm::Vector3<double> assetRecord::getVelocityXYZ() const
{
   return m_Velocity;
}

void assetRecord::getVelocityXYZ(double& x, double& y, double& z) const
{
   x = m_Velocity[0];
   y = m_Velocity[1];
   z = m_Velocity[2];
}

double assetRecord::getSpeed() const
{
   return m_Velocity.norm();
}

const std::vector<std::shared_ptr<zoneRecordInterface>> assetRecord::GetZones() const
{
   return m_attached_zones.GetZones();
}

const std::vector<std::shared_ptr<zoneRecordInterface>> assetRecord::GetZones(const eZoneType& type) const
{
   return m_attached_zones.GetZones(type);
}


const std::vector<std::shared_ptr<zoneRecordInterface>> assetRecord::GetDefendedZones() const
{
   return m_attached_zones.GetZones(E_ZONE_TYPE_DA);
}

void assetRecord::AddZone(const std::shared_ptr<zoneRecordInterface>& zone)
{
   m_attached_zones.Add(zone);
}

bool assetRecord::hasDefendedZone() const
{
   auto da_zones = GetDefendedZones();
   return da_zones.size() > 0;
}

bool assetRecord::hasZones() const
{
   return m_attached_zones.GetZones().size() > 0;
}

void assetRecord::AddWeapon(const std::shared_ptr<weaponRecord>& weapon)
{
   m_weapons[weapon->GetWeaponID()] = weapon;
}

void assetRecord::AddSensor(const std::shared_ptr<sensorRecord>& sensor)
{
   m_sensors[sensor->GetSensorID()] = sensor;
}

void assetRecord::setHasAssetManager(const bool has_manager)
{
   m_has_asset_manager = has_manager;
}

bool assetRecord::hasAssetManager() const
{
   return m_has_asset_manager;
}

void assetRecord::setHasBattleManager(const bool has_manager)
{
   m_has_battle_manager = has_manager;
}

bool assetRecord::hasBattleManager() const
{
   return m_has_battle_manager;
}

void assetRecord::setHasSensorManager(const bool has_manager)
{
   m_has_sensor_manager = has_manager;
}

bool assetRecord::hasSensorManager() const
{
   return m_has_sensor_manager;
}

void assetRecord::setHasWeaponManager(const bool has_manager)
{
   m_has_weapon_manager = has_manager;
}

bool assetRecord::hasWeaponManager() const
{
   return m_has_weapon_manager;
}

void assetRecord::setHasC2DisseminationManager(const bool has_manager)
{
   m_has_disseminate_c2_manager = has_manager;
}

bool assetRecord::hasC2DisseminationManager() const
{
   return m_has_disseminate_c2_manager;
}

void assetRecord::setMaxAssignments(const unsigned int& num)
{
   m_MaxAssignments = num;
}

const int assetRecord::getMaxAssignments() const
{
   return m_MaxAssignments;
}

void assetRecord::setNumAssignments(const int NumAssignments)
{
   if (NumAssignments > m_MaxAssignments)
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "assetRecord::setNumAssignments(): attempt to set # assignments " << NumAssignments
                        << " but max assignments = " << m_MaxAssignments << " idrecord = " << m_ID;
      m_NumAssignments = m_MaxAssignments;
   }
   else if (NumAssignments < 0)
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "assetRecord::setNumAssignments(): attempt to set # assignments to " << NumAssignments
                        << " idrecord = " << m_ID;
      m_NumAssignments = 0;
   }
   else
   {
      m_NumAssignments = NumAssignments;
   }
}

int assetRecord::getNumAssignments() const
{
   return m_NumAssignments;
}

int assetRecord::getNumOpenAssignments() const
{
   return m_MaxAssignments - m_NumAssignments;
}

bool assetRecord::hasOpenAssignments() const
{
   return getNumOpenAssignments() > 0;
}


void assetRecord::setAssignmentDelay(const double& delay)
{
   m_AssignmentDelay = delay;
}

const double& assetRecord::getAssignmentDelay() const
{
   return m_AssignmentDelay;
}

void assetRecord::setAggregateUnit(const bool aggregate_unit)
{
   m_aggregate_unit = aggregate_unit;
}

bool assetRecord::getAggregateUnit() const
{
   return m_aggregate_unit;
}

bool assetRecord::hasWeapon() const
{
   return m_weapons.size() > 0;
}

// If the asset has weapons, this method returns:
//      true, if all weapons have munitions;
//      false, if no weapons have munitions;
//      true, if require_all_weapons is false, and at least one weapon has munitions;
//      false, if require_all_weapons is true, and at least one weapon does not have munitions.
// If the asset has no weapons, this method returns:
//      false, if require_all_weapons is false;
//      true, if require_all_weapons is true (as technically, all weapons do have munitions).
//         [For this reason, a call to this function should in some cases be combined with "&& assetRecord::hasWeapon()".]
bool assetRecord::hasMunitions(const bool require_all_weapons) const
{
   for (std::size_t wpn_idx = 1; wpn_idx <= getNumWeapons(); ++wpn_idx)
   {
      if (getNthWeapon(wpn_idx)->GetTruthTotalMunitions() != 0)
      {
         // Return true if not all weapons are required to have munitions and munitions are found on a single weapon
         if (!require_all_weapons)
         {
            return true;
         }
      }
      // Return false if all weapons are required to have munitions and munitions are not found on a single weapon
      else if (require_all_weapons)
      {
         return false;
      }
   }

   // If all weapons have been checked and this code is reached, either all weapons have munitions and require_all_weapons is true,
   // or no weapons have munitions and require_all_weapons is false.
   return require_all_weapons;
}

std::size_t assetRecord::getNumWeapons() const
{
   return m_weapons.size();
}

// 1-based
std::shared_ptr<weaponRecord> assetRecord::getNthWeapon(const size_t nth_weapon) const
{
   size_t n = 1;
   auto it = m_weapons.cbegin();
   for (;;)
   {
      if (it == m_weapons.cend())
      {
         return std::shared_ptr<weaponRecord>();
      }
      if (n == nth_weapon)
      {
         return it->second;
      }
      ++it;
      ++n;
   }
}

std::vector<std::shared_ptr<weaponRecord>> assetRecord::getWeapons() const
{
   std::vector<std::shared_ptr<weaponRecord>> weapons;
   weapons.reserve(m_weapons.size());
   for (auto it = m_weapons.cbegin(); it != m_weapons.cend(); ++it)
   {
      weapons.push_back(it->second);
   }
   return weapons;
}

std::shared_ptr<weaponRecord> assetRecord::getWeapon(const idRecord& weapon_id) const
{
   auto it = m_weapons.cbegin();
   for (;;)
   {
      if (it == m_weapons.cend())
      {
         return std::shared_ptr<weaponRecord>();
      }

      const auto& weapon = it->second;
      auto curr_weapon_id = GetAssignedIDFromWeaponIDs(weapon->GetParentAssetID(), weapon->GetWeaponID());
      if (curr_weapon_id == weapon_id)
      {
         return weapon;
      }
      ++it;
   }
}

// If a weaponRecord which matches weaponID can be found on this asset, update the estimated munitions prepped, total munitions and allocated fire channels
// of that weaponRecord.
void assetRecord::updatePerceptions(const tWeaponID& weaponID)
{
   auto weapon_iter = m_weapons.find(weaponID);
   if (weapon_iter != m_weapons.end())
   {
      auto weapon = weapon_iter->second;
      weapon->SetEstimatedTotalMunitions(weapon->GetTruthTotalMunitions());
      weapon->SetEstimatedMunitionsPrepped(weapon->GetTruthMunitionsPrepped());
      weapon->SetEstimatedAllocatedFireChannels(weapon->GetTruthAllocatedFireChannels());
   }
}

bool assetRecord::hasSensor() const
{
   return getNumSensors() > 0;
}

std::size_t assetRecord::getNumSensors() const
{
   return m_sensors.size();
}

// 1-based
std::shared_ptr<sensorRecord> assetRecord::getNthSensor(const size_t nth_sensor) const
{
   size_t n = 1;
   auto it = m_sensors.cbegin();
   for (;;)
   {
      if (it == m_sensors.cend())
      {
         return std::shared_ptr<sensorRecord>();
      }
      if (n == nth_sensor)
      {
         return it->second;
      }
      ++it;
      ++n;
   }
}

std::shared_ptr<sensorRecord> assetRecord::getNthSensorOfType(const size_t nth_sensor, const il::SensorEnum sensor_type) const
{
   size_t n = 1;
   auto it = m_sensors.cbegin();
   for (;;)
   {
      if (it == m_sensors.cend())
      {
         return std::shared_ptr<sensorRecord>();
      }
      const auto& sensor = it->second;
      // check type first since count is only sensors of 'this' type
      if (sensor->GetSensorType() == sensor_type)
      {
         if (n == nth_sensor)
         {
            return sensor;
         }
         ++n;
      }
      ++it;
   }
}

std::shared_ptr<sensorRecord> assetRecord::getSensor(const tSensorID& sensor_id) const
{
   auto sensor_iter = m_sensors.find(sensor_id);
   if (sensor_iter != m_sensors.end())
   {
      return sensor_iter->second;
   }

   return nullptr;
}

std::vector<std::shared_ptr<sensorRecord>> assetRecord::getSensors() const
{
   std::vector<std::shared_ptr<sensorRecord>> sensors;
   sensors.reserve(m_sensors.size());
   for (auto it = m_sensors.cbegin(); it != m_sensors.cend(); ++it)
   {
      sensors.push_back(it->second);
   }
   return sensors;
}

void assetRecord::setSystemStatus(const SYSTEM_STATUS Status, const bool External)
{
   if (External)
   {
      m_SystemStatus = Status;
      m_ExternallyDefinedSystemStatus = true;
   }
   else if (!m_ExternallyDefinedSystemStatus)
   {
      m_SystemStatus = Status;
   }
}

SYSTEM_STATUS assetRecord::getSystemStatus() const
{
   return m_SystemStatus;
}

void assetRecord::setExternallyDefinedSystemStatus(const bool External)
{
   m_ExternallyDefinedSystemStatus = External;
}

SYSTEM_STATUS assetRecord::getAssetStatus(const idRecord& AssetID, const assetMap& Assets)
{
   auto asset = Assets.GetAsset(AssetID);
   if (asset)
   {
      return asset->getSystemStatus();
   }
   else
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Asset " << AssetID << " not found int asset map." << std::endl;
      return STATUS_RED;
   }
}

void assetRecord::setSaturationMode(const bool& SaturationMode)
{
   m_SaturationMode = SaturationMode;
}

const bool& assetRecord::inSaturationMode() const
{
   return m_SaturationMode;
}

bool assetRecord::isCombatReady() const
{
   return isGreen();
}


double assetRecord::getAgeYellow(void) const
{
   return m_AgeYellowTime;
}

void assetRecord::setAgeYellow(const double& aTime)
{
   m_AgeYellowTime = aTime;
}

double assetRecord::getAgeRed(void) const
{
   return m_AgeRedTime;
}

void assetRecord::setAgeRed(const double& aTime)
{
   m_AgeRedTime = aTime;
}

bool assetRecord::isGreen() const
{
   return (STATUS_GREEN == m_SystemStatus);
}

bool assetRecord::isYellow() const
{
   return (STATUS_YELLOW == m_SystemStatus);
}

bool assetRecord::isRed() const
{
   return (STATUS_RED == m_SystemStatus);
}

std::tuple<tReadyWeapons, std::vector<double>> assetRecord::getReadyWeaponList(const assetMap& Assets) const
{
   return assetRecord::buildReadyWeaponList(this, Assets);
}

std::tuple<tReadyWeapons, std::vector<double>> assetRecord::buildReadyWeaponList(const assetRecord* Asset, const assetMap& Assets) const
{
   std::tuple<tReadyWeapons, std::vector<double>> retvals;
   auto& m_ReadyWeaponList = std::get<0>(retvals);
   auto& m_ReadyWeaponDelayList = std::get<1>(retvals);
   //step 1: check self and enumerate weapons
   // Skip if its not ready or cannot accept assignments...
   if (!Asset->isCombatReady())
   {
      if (Asset->hasWeapon())
         HCL_TRACE_LOGGER(mGlobalLogger) << "\tExcluding Asset " << Asset->getName() << "\tOpen assignments: " << Asset->getNumOpenAssignments()
                            << "\t (not GREEN)";
      else
         HCL_TRACE_LOGGER(mGlobalLogger) << "\tExcluding Asset " << Asset->getName() << "\tOpen assignments: " << Asset->getNumOpenAssignments()
                            << "\t (not a weapon)";
   }

   // if I'm a weapon, iterate my weapons
   if (Asset->hasWeapon())
   {
      for (std::size_t idx = 0; idx < Asset->getNumWeapons(); ++idx)
      {
         auto curr_weapon = Asset->getNthWeapon(idx + 1);
         if (curr_weapon)
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << "\t Asset: " << Asset->getName() << "\tOpen assignments: " << Asset->getNumOpenAssignments()
                               << "\t Weapon ID: " << curr_weapon->GetWeaponID() << ", Estimated prepped weapons: " << curr_weapon->GetEstimatedMunitionsPrepped()
                               << ", Open fire Channels: " << curr_weapon->GetEstimatedAvailableFireChannels();

            if (curr_weapon->GetEstimatedMunitionsPrepped() > 0)
            {
               m_ReadyWeaponList.push_back(curr_weapon);
               m_ReadyWeaponDelayList.push_back(Asset->getAssignmentDelay());
            }
         }
      }
   }
   else
   {
      HCL_TRACE_LOGGER(mGlobalLogger) << "\tExcluding Asset " << Asset->getName() << "\tOpen assignments: " << Asset->getNumOpenAssignments()
                         << "\t (not a weapon)";
   }

   //step 2: check subordinates and merge lists, delays
   if (Asset->m_Subordinates.empty())
   {
#if 0
      HCL_TRACE_LOGGER(mGlobalLogger) << "Asset has no subordinates." << std::endl;
#endif
      return retvals;
   }

#if 0
   HCL_TRACE_LOGGER(mGlobalLogger) << "Num direct subordinates: " << Asset->m_Subordinates.size() << std::endl;
#endif

   // recurse all my direct subordinates.
   for (unsigned int i = 0; i < Asset->m_Subordinates.size(); ++i)
   {
      auto asset = Assets.GetAsset(Asset->m_Subordinates[i]);
      if (!asset)
      {
         continue;
      }
      auto temp = assetRecord::buildReadyWeaponList(asset.get(), Assets);
      //add weapons from children to return list
      m_ReadyWeaponList.insert(std::end(m_ReadyWeaponList), std::begin(std::get<0>(temp)), std::end(std::get<0>(temp)));
      m_ReadyWeaponDelayList.insert(std::end(m_ReadyWeaponDelayList), std::begin(std::get<1>(temp)), std::end(std::get<1>(temp)));
   }
   return retvals;
}

void assetRecord::addDirectSubordinate(const idRecord& SubordinateAssetID, assetMap& Assets)
{
   // Do nothing if SubordinateAssetID is invalid.
   if (!SubordinateAssetID.isValid())
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Asset ID " << SubordinateAssetID << " is invalid."
                         << std::endl;
      return;
   }

   if (SubordinateAssetID == m_ID)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Asset cannot have itself as a subordinate." << std::endl;
      return;
   }


   auto subAsset = Assets.GetAsset(SubordinateAssetID);
   if (!subAsset)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Asset ID " << SubordinateAssetID
                         << " not found in assetMap" << std::endl;
      return;
   }

   // Set my ID as sub's commander ID.
   subAsset->setCommanderID(m_ID);

   // If not already in subordinate list, then add.
   if (std::find(m_Subordinates.begin(), m_Subordinates.end(),
                 SubordinateAssetID) == m_Subordinates.end())
   {
      m_Subordinates.push_back(SubordinateAssetID);
   }
   HCL_TRACE_LOGGER(mGlobalLogger) << "Setting asset " << m_ID
                      << " as commander of asset " << SubordinateAssetID
                      << std::endl;
}

void assetRecord::removeDirectSubordinate(const idRecord& SubordinateAssetID, assetMap& Assets)
{
   // Do nothing if SubordinateAssetID is invalid.
   if (!SubordinateAssetID.isValid()
       || SubordinateAssetID == m_ID)
   {
      return;
   }

   auto subAsset = Assets.GetAsset(SubordinateAssetID);
   if (subAsset)
   {
      // Set 0 as sub's commander ID.
      subAsset->setCommanderID(idRecord(0, 0));
   }

   // If in subordinate list, then remove.
   std::vector<idRecord>::iterator
   subAssetIter = std::find(m_Subordinates.begin(), m_Subordinates.end(),
                            SubordinateAssetID);
   if (m_Subordinates.end() != subAssetIter)
   {
      m_Subordinates.erase(subAssetIter);
   }
}

std::vector<idRecord> assetRecord::getSubordinates(assetMap& Assets) const
{
   std::vector<idRecord> subordinates = m_Subordinates;
   for (size_t i = 0; i < subordinates.size(); ++i)
   {
      idRecord subordinate = subordinates[i];
      std::vector<idRecord> subordinateSubordinates = Assets.GetAsset(subordinate)->getDirectSubordinates();
      subordinates.insert(
         subordinates.end(),
         subordinateSubordinates.begin(),
         subordinateSubordinates.end());
   }
   return subordinates;
}

bool assetRecord::isDirectSubordinate(const idRecord& AssetID) const
{
   // Do nothing if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return false;
   }

   return (std::find(m_Subordinates.begin(), m_Subordinates.end(),
                     AssetID) != m_Subordinates.end());
}

void assetRecord::addDirectPeer(const idRecord& AssetID, assetMap& Assets)
{
   // Do nothing if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return;
   }

   // If not already in peer list, then add.
   if (std::find(m_Peers.begin(), m_Peers.end(),
                 AssetID) == m_Peers.end())
   {
      m_Peers.push_back(AssetID);
   }
}

void assetRecord::removeDirectPeer(const idRecord& AssetID, assetMap& Assets)
{
   // Do nothing if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return;
   }

   // If in peer list, then remove.
   std::vector<idRecord>::iterator
   peerAssetIter = std::find(m_Peers.begin(), m_Peers.end(), AssetID);
   if (m_Peers.end() != peerAssetIter)
   {
      m_Peers.erase(peerAssetIter);
   }
}

void assetRecord::addDirectCommander(const idRecord& AssetID, assetMap& Assets)
{
   // Do nothing if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return;
   }

   // don't force the commander to exist this is so we can add a reference to a unit that will be the
   // target of messages but won't neccessarily send us statuses
   m_CommanderID = AssetID;

   // if we have the asset, go ahead and add us to him
   auto cmdr_asset = Assets.GetAsset(m_CommanderID);
   if (cmdr_asset)
   {
      cmdr_asset->addDirectSubordinate(m_ID, Assets);
   }
}

void assetRecord::removeDirectCommander(assetMap& Assets)
{
   // Do nothing if AssetID is invalid.
   if (!m_CommanderID.isValid())
   {
      return;
   }


   // if we have the asset, go ahead and remove us from him
   auto cmdr_asset = Assets.GetAsset(m_CommanderID);
   if (cmdr_asset)
   {
      cmdr_asset->removeDirectSubordinate(m_ID, Assets);
   }

   // reset the id record
   m_CommanderID = idRecord();

}

bool assetRecord::isDirectPeer(const idRecord& AssetID) const
{
   // Do nothing if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return false;
   }

   return (std::find(m_Peers.begin(), m_Peers.end(), AssetID)
           != m_Peers.end());
}

bool assetRecord::isDirectCommander(const idRecord& AssetID) const
{
   // False if AssetID is invalid.
   if (!AssetID.isValid() || !m_CommanderID.isValid() || AssetID == m_ID)
   {
      return false;
   }

   return (AssetID == m_CommanderID);
}

bool assetRecord::
isInCommandChain(const idRecord& AssetID, const assetMap& Assets) const
{
   // False if AssetID is invalid.
   if (!AssetID.isValid() || !m_CommanderID.isValid() || AssetID == m_ID)
   {
      return false;
   }

   if (isDirectCommander(AssetID))
   {
      return true;
   }
   else
   {
      auto commander = Assets.GetAsset(m_CommanderID);
      if (!commander)
      {
         return false;
      }

      return commander->isInCommandChain(AssetID, Assets);
   }
}

bool assetRecord::isSubordinate(const idRecord& AssetID,
                                const assetMap& Assets
                               ) const
{
   // False if AssetID is invalid.
   if (!AssetID.isValid() || AssetID == m_ID)
   {
      return false;
   }

   if (isDirectSubordinate(AssetID))
   {
      return true;
   }
   else
   {
      // Loop through list of subordinates.
      for (unsigned int i = 0; i < m_Subordinates.size(); ++i)
      {
         auto sub = Assets.GetAsset(m_Subordinates[i]);
         if (sub)
         {
            if (sub->isSubordinate(AssetID, Assets))
            {
               return true;
            }
         }
      }

      return false;
   }
}

unsigned int assetRecord::getNumDirectSubordinates() const
{
   return static_cast<unsigned int>(m_Subordinates.size());
}

unsigned int assetRecord::getNumDirectPeers() const
{
   return static_cast<unsigned int>(m_Peers.size());
}

unsigned int assetRecord::
getNumSubordinates(const assetMap& Assets) const
{
   unsigned int total = getNumDirectSubordinates();

   // Loop through list of subordinates.
   for (unsigned int i = 0; i < getNumDirectSubordinates(); ++i)
   {
      auto sub = Assets.GetAsset(m_Subordinates[i]);
      if (sub)
      {
         total += sub->getNumSubordinates(Assets);
      }
   }

   return total;
}

void assetRecord::updateAge(const double& simTime)
{
   double assetAge = simTime - m_StatusTime;

#if 0
   HCL_TRACE_LOGGER(mGlobalLogger) << "    Asset ID: " << m_ID << std::endl
                      << "     simTime: " << simTime << std::endl
                      << "m_StatusTime: " << m_StatusTime << std::endl
                      << "    assetAge: " << assetAge << std::endl;
#endif

   if (assetAge > m_AgeRedTime)
   {
      setSystemStatus(STATUS_RED);
   }
   else if ((assetAge > m_AgeYellowTime) && (m_SystemStatus < STATUS_YELLOW))
   {
      setSystemStatus(STATUS_YELLOW);
   }
}

void assetRecord::setStatusTime(const double& time)
{
   m_StatusTime = time;
}

double assetRecord::getStatusTime() const
{
   return m_StatusTime;
}

void assetRecord::setPositionTime(const double& time)
{
   m_PositionTime = time;
}

double assetRecord::getPositionTime() const
{
   return m_PositionTime;
}

void assetRecord::processMessage(const il::combatStatusMessage& csm, const bool unit_has_assignments)
{
   if (csm.getAssetID() == m_ID.getSystemUnitID())
   {
#if 0
      HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << csm.getDataTime() << std::endl;
            << "Status of "
            << static_cast<int>(csm.getSystemStatus())
            << " from " << m_ID << std::endl
            << "Max assignments: " << csm.getNumOperationalChannels()
            << std::endl;
#endif
      m_StatusTime = csm.getStatusTime();
      m_SystemStatus = csm.getSystemStatus();
      m_MaxAssignments = csm.getMaxAssignments();

      m_PositionTime = csm.getPositionTime();
      double lla_degsm[3] = { 0 };
      if (csm.getPosition(lla_degsm[0], lla_degsm[1], lla_degsm[2]))
      {
         HCL_TRACE_LOGGER(mGlobalLogger) << std::setprecision(2) << std::fixed
                            << "asset lat/lon/alt = " << lla_degsm[0] << "/" << lla_degsm[1] << "/" << lla_degsm[2] << std::endl;

         m_Position.setLLADegrees(lla_degsm[0], lla_degsm[1], lla_degsm[2]);
      }

      if (csm.getVelocity(m_Velocity[0], m_Velocity[1], m_Velocity[2]))
      {
         HCL_TRACE_LOGGER(mGlobalLogger) << "asset ECEF vel = " << m_Velocity[0] << "/" << m_Velocity[1] << "/" << m_Velocity[2] << std::endl;
      }

      // weapon system updates
      if (!unit_has_assignments)
      {
         // update platform-level assignment count
         m_NumAssignments = csm.getCurrentAssignments();

         // only update weapon perceptions if the unit is not involved in any assignments. The reason being is that
         // once the unit is involved in assignments, the local battle manager will handle local estimates of munitions
         // based on assignment allocations before the weapon actually shoots (commited vs uncommitted munitions).
         // Once the weapon is free from assignments, the munition counts will be updated from status messages once again.
         for (size_t widx = 0; widx < csm.getNumWeaponSystems(); ++widx)
         {
            // foreach weapon
            combatStatusMessage::WeaponSystem wsys;
            if (csm.getNthWeapon(widx + 1, wsys))
            {
               auto weapon_iter = m_weapons.find(wsys._WSysID);
               if (weapon_iter != m_weapons.end())
               {
                  auto weapon_rec = weapon_iter->second;
                  weapon_rec->SetEstimatedTotalMunitions(wsys._TotalMunitions);
                  weapon_rec->SetEstimatedMunitionsPrepped(wsys._MunitionsReady);
                  weapon_rec->SetEstimatedAllocatedFireChannels(wsys._NumAllocatedFireChannels);
                  weapon_rec->SetTotalFireChannels(wsys._TotalFireChannels);
               }
            }
         } // foreach weapon
      }

   }
}

std::shared_ptr<il::combatStatusMessage> assetRecord::generateCombatStatusMessage() const
{
   std::shared_ptr<combatStatusMessage> csm(new combatStatusMessage(mGlobalLogger));
   csm->setStatusTime(m_StatusTime);
   csm->setSenderID(m_ID);
   csm->setAssetID(m_ID);
   csm->setSystemStatus(m_SystemStatus);
   csm->setMaxAssignments(m_MaxAssignments);
   csm->setCurrentAssignments(m_NumAssignments);

   // Add the weapons to the message
   for (auto it = m_weapons.cbegin(); it != m_weapons.cend(); ++it)
   {
      auto& weapon = it->second;
      combatStatusMessage::WeaponSystem wsys;
      wsys._WSysID = weapon->GetWeaponID();
      wsys._MunitionsReady = weapon->GetEstimatedMunitionsPrepped();
      wsys._TotalMunitions = weapon->GetEstimatedTotalMunitions();
      wsys._TotalFireChannels = weapon->GetTotalFireChannels();
      wsys._NumAllocatedFireChannels = weapon->GetEstimatedAllocatedFireChannels();

      csm->addWeapon(wsys);
   }

   csm->setPositionTime(m_PositionTime);
   csm->setPosition(m_Position.getLatDegs(), m_Position.getLonDegs(), m_Position.getAltM());
   csm->setVelocity(m_Velocity.getX(), m_Velocity.getY(), m_Velocity.getZ());

   return csm;
}

// a recursive version of the above
void assetRecord::logSubordinateStatus(const assetMap& Assets, bool direct_only) const
{
   if (!m_LogMyStatus)
   {
      return;
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "Subordinate status" << std::endl
                     << "------------------" << std::endl;
   if (m_Subordinates.empty())
   {
      HCL_INFO_LOGGER(mGlobalLogger) << "No subordinates." << std::endl;
      return;
   }
   else
   {
      // Loop through list of subordinates.
      for (unsigned int i = 0; i < getNumDirectSubordinates(); ++i)
      {
         auto sub = Assets.GetAsset(m_Subordinates[i]);
         if (sub)
         {
            sub->logMyStatus();
            if (!direct_only)
            {
               sub->logSubordinateStatus(Assets, direct_only);
            }
         }
      }
   }
}


void assetRecord::logMyStatus() const
{
   if (!m_LogMyStatus)
   {
      return;
   }

   std::stringstream strm_out;

   strm_out << "System: " << m_Name << ", ID: " << m_ID.getSystemUnitID() << ", Side: " << m_Side << ", Status: ";
   switch (m_SystemStatus)
   {
      case STATUS_GREEN:
         strm_out << GREEN_TEXT_BLACK_BG << "GREEN";
         break;
      case STATUS_YELLOW:
         strm_out << YELLOW_TEXT_BLACK_BG << "YELLOW";
         break;
      case STATUS_RED:
         strm_out << RED_TEXT_BLACK_BG << "RED";
         break;
      case STATUS_WHITE:
         strm_out << WHITE_TEXT_BLACK_BG << "WHITE";
         break;
      default:
         strm_out << "Unknown";
         break;
   }

   if (hasWeapon())
   {
      for (auto it = m_weapons.cbegin(); it != m_weapons.cend(); ++it)
      {
         auto& weapon = it->second;
         strm_out << ", Weapon: " << weapon->GetWeaponID().getIDString()
                  << ", WeaponID: " << weapon->GetWeaponID().getSystemUnitID()
                  << ":" << weapon->GetWeaponID().getSubID()
                  << ", Estimated Munitions: " << weapon->GetEstimatedMunitionsPrepped()
                  << ", Estimated Fire Channels: " << weapon->GetEstimatedAvailableFireChannels();
      }
   }
   else
   {
      strm_out << ", is weapon: No";
   }

   if (hasSensor())
   {
      for (auto it = m_sensors.cbegin(); it != m_sensors.cend(); ++it)
      {
         auto& sensor = it->second;
         strm_out << ", Sensor: " << sensor->GetSensorID().getIDString()
                  << ", SensorID: " << sensor->GetSensorID().getSystemUnitID()
                  << ":" << sensor->GetSensorID().getSubID()
                  << ", Type: " << getSensorTypeStringFromEnum(sensor->GetSensorType(), mGlobalLogger);
      }
   }
   else
   {
      strm_out << ", is sensor: No";
   }

   strm_out << ", Assignments: " << getNumAssignments() << "/" << getMaxAssignments() << DEFAULT_TEXT;

   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}


/// Find next subordinate who is in the chain of command of the desired asset.
idRecord assetRecord::findNextSubordinateInChain(const idRecord& AssetID, const assetMap& Assets) const
{
   if (isDirectSubordinate(AssetID))
   {
      return AssetID;
   }

   for (std::size_t i = 0; i < m_Subordinates.size(); ++i)
   {
      auto sub = Assets.GetAsset(m_Subordinates[i]);
      if (sub && (sub->isSubordinate(AssetID, Assets)))
      {
         return m_Subordinates[i];
      }
   }

   return idRecord(0, 0);
}

// find the next commander on the path between from_asset to to_asset - returns invalid idRecord if from_asset == to_asset
idRecord assetRecord::findNextCommanderInChain(const idRecord& AssetID, const assetMap& Assets) const
{
   if (isDirectCommander(AssetID))
   {
      return AssetID;
   }

   if (hasCommander())
   {
      return findNextCommanderInChain(getCommanderID(), Assets);
   }

   return idRecord(0, 0);
}



void assetRecord::updateC2ChainAssignedUnitStatus(const idRecord& EndAssetID, assetMap* Assets, const eStatusIncrFlag incr_flag, const int assignments_incr, const int munitions_incr)
{

   bool increment = incr_flag == INCREMENT ? true : false;
   // all units including intermediate units get ready assignments updated
   increment ? IncrementAssetAssignments(assignments_incr) : DecrementAssetAssignments(assignments_incr);

   if (EndAssetID.getID() != m_ID.getID())
   {
      idRecord nextSubID = findNextSubordinateInChain(EndAssetID.getID(), *Assets);
      auto nextAsset = Assets->GetAsset(nextSubID);
      if (nextAsset)
      {
         return nextAsset->updateC2ChainAssignedUnitStatus(EndAssetID, Assets, incr_flag, assignments_incr, munitions_incr);
      }
   }
   else
   {
      // weapon-only
      auto weapon_entry = m_weapons.find(idRecord(EndAssetID.getSubID(), -1));  // make sure we look up based on the weapon ID which is the second of the tuple
      if (weapon_entry != m_weapons.end())
      {
         auto& weapon_ref = weapon_entry->second;
         increment ? weapon_ref->IncrementEstimations(assignments_incr, munitions_incr) : weapon_ref->DecrementEstimations(assignments_incr, munitions_incr);
      }
   }
}

void assetRecord::IncrementAssetAssignments(const int amount)
{
   // increment allocates an assignment so subtract
   setNumAssignments(getNumAssignments() - amount);
}

void assetRecord::DecrementAssetAssignments(const int amount)
{
   // decrement opens up an assignment so add
   setNumAssignments(getNumAssignments() + amount);
}

il::SYSTEM_STATUS assetRecord::getC2Status() const
{
   return getC2Capable() ? il::STATUS_GREEN : il::STATUS_RED;
}

// This function returns STATUS_GREEN if a weapon with munitions could be found on this asset; STATUS_RED, otherwise.
// If require_all_weapons is true, all weapons on the asset must have munitions for STATUS_GREEN.
il::SYSTEM_STATUS assetRecord::getWeaponStatus(const bool require_all_weapons) const
{
   if (hasWeapon())
      if (hasMunitions(require_all_weapons))
         // Has weapon, has munitions
      {
         return STATUS_GREEN;
      }
      else
         // Has weapon, does not have munitions
      {
         return STATUS_WHITE;
      }
   else
      // Does not have weapon
   {
      return STATUS_RED;
   }
}

// This function returns STATUS_GREEN  if a weapon could be found on this asset or any asset with STATUS_GREEN in assetMap assets; STATUS_RED, otherwise.
// If require_all_weapon is true, all weapons on the asst must have munitions for STATUS_GREEN.
SYSTEM_STATUS assetRecord::getWeaponStatus(const assetMap& assets, const bool require_all_weapons) const
{
   SYSTEM_STATUS weaponStatus = getWeaponStatus(require_all_weapons);

   if (weaponStatus == STATUS_GREEN)
   {
      return STATUS_GREEN;
   }

   std::function<void(std::shared_ptr<assetRecord>)> hasGreenWeaponStatus = [&weaponStatus, require_all_weapons](std::shared_ptr<assetRecord> subAsset)
   {
      if (weaponStatus == STATUS_GREEN)
      {
         return;
      }

      auto subWeaponStatus = subAsset->getWeaponStatus(require_all_weapons);

      if (subWeaponStatus == STATUS_GREEN && subAsset->getSystemStatus() == STATUS_GREEN)
      {
         weaponStatus = STATUS_GREEN;
      }
      else if (subWeaponStatus < weaponStatus && subAsset->getSystemStatus() < weaponStatus)
      {
         weaponStatus = subWeaponStatus;
      }
   };

   foreachSubordinate(assets, m_ID, hasGreenWeaponStatus);

   return weaponStatus;
}

// This function returns STATUS_GREEN if an EW could be found on this asset; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getEWStatus() const
{
   for (std::size_t sens_idx = 1; sens_idx <= getNumSensors(); ++sens_idx)
   {
      if (getNthSensor(sens_idx)->IsEW())
      {
         return STATUS_GREEN;
      }
   }

   return STATUS_RED;
}

// This function returns STATUS_GREEN  if an EW could be found on this asset or any asset with STATUS_GREEN in assetMap assets; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getEWStatus(const assetMap& assets) const
{
   if (getEWStatus() == STATUS_GREEN)
   {
      return STATUS_GREEN;
   }

   SYSTEM_STATUS ewStatus = STATUS_RED;

   std::function<void(std::shared_ptr<assetRecord>)> hasGreenEW = [&ewStatus](std::shared_ptr<assetRecord> subAsset)
   {
      if (subAsset->getEWStatus() == STATUS_GREEN && subAsset->getSystemStatus() == STATUS_GREEN)
      {
         ewStatus = STATUS_GREEN;
      }
   };

   foreachSubordinate(assets, m_ID, hasGreenEW);

   return ewStatus;
}

// This function returns STATUS_GREEN if a tar could be found on this asset; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getTARStatus() const
{
   for (std::size_t sens_idx = 1; sens_idx <= getNumSensors(); ++sens_idx)
   {
      if (getNthSensor(sens_idx)->IsTAR())
      {
         return STATUS_GREEN;
      }
   }

   return STATUS_RED;
}

// This function returns STATUS_GREEN  if a tar could be found on this asset or any asset with STATUS_GREEN in assetMap assets; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getTARStatus(const assetMap& assets) const
{
   if (getTARStatus() == STATUS_GREEN)
   {
      return STATUS_GREEN;
   }

   SYSTEM_STATUS tarStatus = STATUS_RED;

   std::function<void(std::shared_ptr<assetRecord>)> hasGreenTAR = [&tarStatus](std::shared_ptr<assetRecord> subAsset)
   {
      if (subAsset->getTARStatus() == STATUS_GREEN && subAsset->getSystemStatus() == STATUS_GREEN)
      {
         tarStatus = STATUS_GREEN;
      }
   };

   foreachSubordinate(assets, m_ID, hasGreenTAR);

   return tarStatus;
}

// This function returns STATUS_GREEN if a ttr could be found on this asset; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getTTRStatus() const
{
   for (std::size_t sens_idx = 1; sens_idx <= getNumSensors(); ++sens_idx)
   {
      if (getNthSensor(sens_idx)->IsTTR())
      {
         return STATUS_GREEN;
      }
   }

   return STATUS_RED;
}

// This function returns STATUS_GREEN  if a ttr could be found on this asset or any asset with STATUS_GREEN in assetMap assets; STATUS_RED, otherwise.
SYSTEM_STATUS assetRecord::getTTRStatus(const assetMap& assets) const
{
   if (getTTRStatus() == STATUS_GREEN)
   {
      return STATUS_GREEN;
   }

   SYSTEM_STATUS ttrStatus = STATUS_RED;

   std::function<void(std::shared_ptr<assetRecord>)> hasGreenTTR = [&ttrStatus](std::shared_ptr<assetRecord> subAsset)
   {
      if (subAsset->getTTRStatus() == STATUS_GREEN && subAsset->getSystemStatus() == STATUS_GREEN)
      {
         ttrStatus = STATUS_GREEN;
      }
   };

   foreachSubordinate(assets, m_ID, hasGreenTTR);

   return ttrStatus;
}

SYSTEM_STATUS assetRecord::getStationaryStatus() const
{
   return getSpeed() > 0 ? STATUS_RED : STATUS_GREEN;
}


void assetRecord::checkSubordinatesTimeout(assetMap& assets, const double current_time)
{
   std::function<void(std::shared_ptr<assetRecord>)> update_age_fn = [&current_time](std::shared_ptr<assetRecord> asset)
   {
      asset->updateAge(current_time);
   };

   foreachSubordinate(assets, m_ID, update_age_fn);
}

bool assetRecord::checkSubordinateStatus(const assetMap& assets, std::string& not_green_list)
{
   bool all_green = true;
   std::function<void(std::shared_ptr<assetRecord>)> get_status = [&all_green, &not_green_list](std::shared_ptr<assetRecord> asset)
   {
      if (asset->getSystemStatus() != il::STATUS_GREEN)
      {
         all_green = false;
         appendAssetNameToList(not_green_list, asset);
      }
   };

   foreachSubordinate(assets, m_ID, get_status);

   return all_green;
}

void assetRecord::foreachSubordinateBFS(const assetMap& assets, std::function<void(std::shared_ptr<assetRecord>)> invokefun)
{
   foreachSubordinate(assets, m_ID, invokefun);
}


//////////////
// assetMap //
//////////////
void assetMap::UpdateAge(const double sim_time_s)
{
   for (auto asset = m_assets.begin(); asset != m_assets.end(); ++asset)
   {
      asset->second->updateAge(sim_time_s);
   }
}

std::shared_ptr<assetRecord> assetMap::GetAsset(const idRecord& id) const
{
   auto asset_iter = m_assets.find(id);
   return asset_iter != m_assets.end() ? asset_iter->second : nullptr;
}

void assetMap::AddAsset(const idRecord& id, const std::shared_ptr<assetRecord>& asset)
{
   m_assets[id] = asset;
}

std::vector<std::shared_ptr<const assetRecord>> assetMap::GetAssets() const
{
   std::vector<std::shared_ptr<const assetRecord>> records;
   records.reserve(m_assets.size());
   for (auto it = m_assets.cbegin(); it != m_assets.cend(); ++it)
   {
      records.push_back(it->second);
   }
   return records;
}

std::vector<std::shared_ptr<const assetRecord>> assetMap::GetAssets(std::function<bool(const std::shared_ptr<const assetRecord> &)> pred) const
{
   std::vector<std::shared_ptr<const assetRecord>> records;
   records.reserve(m_assets.size());
   for (auto it = m_assets.cbegin(); it != m_assets.cend(); ++it)
   {
      const auto& record = it->second;
      if (pred(record))
      {
         records.push_back(record);
      }
   }

   return records;
}

bool assetRecord::HasMinRangeAttribute() const
{
   return m_MinRange != 0;
}

double assetRecord::GetMinRange() const
{
   return m_MinRange;
}

void assetRecord::SetMinRange(const double min_range)
{
   m_MinRange = min_range;
}

bool assetRecord::HasMaxRangeAttribute() const
{
   return m_MaxRange != 0;
}

double assetRecord::GetMaxRange() const
{
   return m_MaxRange;
}

void assetRecord::SetMaxRange(const double max_range)
{
   m_MaxRange = max_range;
}

} // namespace il
