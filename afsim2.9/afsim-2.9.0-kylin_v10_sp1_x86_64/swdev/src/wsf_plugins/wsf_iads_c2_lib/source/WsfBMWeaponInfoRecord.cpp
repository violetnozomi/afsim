// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "WsfBMWeaponInfoRecord.hpp"
#include <WsfWeapon.hpp>
#include <WsfSimulation.hpp>
#include <WsfAttributeContainer.hpp>
#include "WsfBMAssetRecord.hpp"
#include "WsfBMUtils.hpp"
#include <logger.hpp>
#include <iadsLib/util.hpp>
#include "UtCast.hpp"

WsfBMWeaponInfoRecord::WsfBMWeaponInfoRecord()
   : m_attached_plat(WSF_INVALID_PLAT_ID)
   , m_sim_ptr(nullptr)
{
}

WsfBMWeaponInfoRecord::~WsfBMWeaponInfoRecord()
{
}

WsfBMWeaponInfoRecord* WsfBMWeaponInfoRecord::Clone() const
{
   return new WsfBMWeaponInfoRecord(*this);
}

void WsfBMWeaponInfoRecord::Set(WsfSimulation* sim_ptr, WsfBMAssetRecord* asset, tWeaponSysID weapon_sys_id, il::AssetTypeEnum weapon_type)
{
   Set(sim_ptr, asset->GetRawRecord(), weapon_sys_id, weapon_type);
}

void WsfBMWeaponInfoRecord::Set(WsfSimulation* sim_ptr, const std::shared_ptr<il::assetRecord>& asset, tWeaponSysID weapon_sys_id, il::AssetTypeEnum weapon_type)
{
   m_sim_ptr = sim_ptr;

   m_attached_plat = ut::safe_cast<tPlatformRef>(asset->getID().getID());
   m_weapon_id = WsfBMUtils::GetWeaponID(m_sim_ptr, m_attached_plat, weapon_sys_id);

   m_parent_asset = asset;

   auto type_id = weapon_type;
   auto type_str = il::AssetTypeEnumToString(weapon_type);
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      auto subtype_id = WsfStringInt(weapon->GetNameId());
      auto subtype_str = weapon->GetName();

      m_weapon_type.set(type_id, type_str, subtype_id, subtype_str);
   }
}

int WsfBMWeaponInfoRecord::GetNumMunitions() const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = static_cast<int>(weapon->GetQuantityRemaining());
   }
   return value;
}

int WsfBMWeaponInfoRecord::GetMaxEngagements()
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->GetMaximumRequestCount();
   }
   return value;
}

double WsfBMWeaponInfoRecord::GetInterceptSpeed()
{
   double xcept_speed = 0;
   // if there is aux data matching the intercept speed, get it
   auto weapon = GetWeaponRef();
   if (weapon && weapon->GetAuxData().AttributeExists(WSF_IADS_C2_XCEPT_SPEED_AUX_DATA_NAME))
   {
      xcept_speed = weapon->GetAuxData().GetDouble(WSF_IADS_C2_XCEPT_SPEED_AUX_DATA_NAME);
   }

   return xcept_speed;
}

tWeaponSysID WsfBMWeaponInfoRecord::GetWsysID() const
{
   return m_weapon_id.getID();
}

il::unitTypeRecord WsfBMWeaponInfoRecord::GetTypeRecord() const
{
   return m_weapon_type;
}

WsfBMWeaponInfoRecord::WsfBMWeaponInfoRecord(const WsfBMWeaponInfoRecord& aSrc)
   : weaponRecord(aSrc)
   , m_attached_plat(aSrc.m_attached_plat)
{
}

const WsfWeapon* WsfBMWeaponInfoRecord::GetWeaponRef() const
{
   WsfWeapon* value = nullptr;
   auto platform = m_sim_ptr->GetPlatformByIndex(m_attached_plat);
   if (platform)
   {
      value = platform->GetComponentEntry<WsfWeapon>(m_weapon_id.getID());
   }
   return value;
}

// Optionally Abstract interface
int WsfBMWeaponInfoRecord::GetTruthMunitionsPrepped() const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = static_cast<int>(weapon->GetQuantityRemaining());
   }
   return value;
}

int WsfBMWeaponInfoRecord::GetTruthTotalMunitions() const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = static_cast<int>(weapon->GetQuantityRemaining());
   }
   return value;
}

int WsfBMWeaponInfoRecord::GetTruthAllocatedFireChannels() const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = static_cast<int>(weapon->GetActiveRequestCount());
   }
   return value;
}

// Pure Abstract interface

double WsfBMWeaponInfoRecord::GetTimeBetweenShots() const
{
   double value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->GetFiringInterval();
   }
   return value;
}

double WsfBMWeaponInfoRecord::GetTimeToFire() const
{
   double value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = const_cast<WsfWeapon*>(weapon)->GetFiringDelay();  // const correctness issue because it does a random draw each time - this seems weird
   }
   return value;
}

bool WsfBMWeaponInfoRecord::HasMinRangeAttribute() const
{
   bool value = false;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->GetAuxData().AttributeExists(WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME);
   }
   return value;
}

double WsfBMWeaponInfoRecord::GetMinRange() const
{
   double value = 0;
   // if there is aux data matching the data, get it
   auto weapon = GetWeaponRef();
   if (weapon && weapon->GetAuxData().AttributeExists(WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME))
   {
      value = weapon->GetAuxData().GetDouble(WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME);
   }

   return value;
}

bool WsfBMWeaponInfoRecord::HasMaxRangeAttribute() const
{
   bool value = false;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->GetAuxData().AttributeExists(WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME);
   }
   return value;
}

double WsfBMWeaponInfoRecord::GetMaxRange() const
{
   double value = DBL_MAX;
   // if there is aux data matching the intercept data, get it
   auto weapon = GetWeaponRef();
   if (weapon && weapon->GetAuxData().AttributeExists(WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME))
   {
      value = weapon->GetAuxData().GetDouble(WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME);
   }

   return value;
}

double WsfBMWeaponInfoRecord::GetAvgSpeed() const
{
   double value = 0.0;
   // if there is aux data matching the data, get it
   auto weapon = GetWeaponRef();
   if (weapon && weapon->GetAuxData().AttributeExists(WSF_IADS_C2_AVG_SPEED_AUX_DATA_NAME))
   {
      value = weapon->GetAuxData().GetDouble(WSF_IADS_C2_AVG_SPEED_AUX_DATA_NAME);
   }

   return value;
}

double WsfBMWeaponInfoRecord::GetEstPK() const
{
   double value = 0.0;
   // if there is aux data matching the data, get it
   auto weapon = GetWeaponRef();
   if (weapon && weapon->GetAuxData().AttributeExists(WSF_IADS_C2_EST_PK_AUX_DATA_NAME))
   {
      value = weapon->GetAuxData().GetDouble(WSF_IADS_C2_EST_PK_AUX_DATA_NAME);
   }

   return value;
}

std::string WsfBMWeaponInfoRecord::GetName() const
{
   std::string value = "";
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->GetName();
   }
   return value;
}

int WsfBMWeaponInfoRecord::GetShotsFiredAtTrack(const il::idRecord& track_id) const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->RoundsFiredAt(WsfBMUtils::ConvertTrackId(track_id));
   }
   return value;
}

void WsfBMWeaponInfoRecord::SetShotsFiredAtTrack(const il::idRecord& track_id, const int fired)
{
}

int WsfBMWeaponInfoRecord::GetShotsPendingForTrack(const il::idRecord& track_id) const
{
   int value = 0;
   auto weapon = GetWeaponRef();
   if (weapon)
   {
      value = weapon->WeaponsPendingFor(WsfBMUtils::ConvertTrackId(track_id));
   }
   return value;
}

void WsfBMWeaponInfoRecord::SetShotsPendingForTrack(const il::idRecord& track_id, const int fired)
{
}
