// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: vclWeaponRecord.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <iadsLib/vclWeaponRecord.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>

namespace il
{

vclWeaponRecord::vclWeaponRecord (const std::shared_ptr<assetRecord> &parent_asset, const tWeaponID &weapon_id, const unitTypeRecord &weapon_type)
   : weaponRecord (parent_asset, weapon_id, weapon_type)
   , m_TimeBetweenShots (0)
   , m_TimeToFire (0)
   , m_MinRange (0)
   , m_MaxRange (0)
   , m_InterceptSpeed (0)
{

}

// Abstract interface
double vclWeaponRecord::GetTimeBetweenShots () const
{
   return m_TimeBetweenShots;
}

double vclWeaponRecord::GetTimeToFire () const
{
   return m_TimeToFire;
}

bool vclWeaponRecord::HasMinRangeAttribute() const
{
   return true;
}

double vclWeaponRecord::GetMinRange () const
{
   return m_MinRange;
}

bool vclWeaponRecord::HasMaxRangeAttribute() const
{
   return true;
}


double vclWeaponRecord::GetMaxRange () const
{
   return m_MaxRange;
}

double vclWeaponRecord::GetInterceptSpeed () const
{
   return m_InterceptSpeed;
}


int vclWeaponRecord::GetShotsFiredAtTrack (const idRecord &track_id) const
{
   int shots_fired = 0;

   auto entry = m_shots_fired.find (track_id);
   if (entry != m_shots_fired.end())
      shots_fired = entry->second;

   return shots_fired;
}

void vclWeaponRecord::SetShotsFiredAtTrack (const idRecord &track_id, const int fired)
{
   // doesn't increment/decrement, overwrites
   m_shots_fired[track_id] = fired;
}

} // namespace il
