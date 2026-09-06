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
*   File: weaponRecordInterface.cpp
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

#include <logger.hpp>
#include <iadsLib/weaponRecordInterface.hpp>
#include <iadsLib/assessmentRecord.hpp>


namespace il
{

weaponRecord::weaponRecord ()
   : m_EstimatedMunitionsPrepped (0)
   , m_EstimatedTotalMunitions (0)
   , m_EstimatedAllocatedFireChannels (0)
   , m_TotalFireChannels (0)
{

}

weaponRecord::weaponRecord (const std::shared_ptr<assetRecord> &parent_asset, const tWeaponID &weapon_id, const unitTypeRecord &weapon_type)
   : m_parent_asset (parent_asset)
   , m_weapon_id (weapon_id)
   , m_weapon_type (weapon_type)
   , m_EstimatedMunitionsPrepped (0)
   , m_EstimatedTotalMunitions (0)
   , m_EstimatedAllocatedFireChannels (0)
   , m_TotalFireChannels (0)
{

}

idRecord weaponRecord::GetParentAssetID () const
{
   return m_parent_asset->getID();
}

unitTypeRecord weaponRecord::GetType () const
{
   return m_weapon_type;
}

tWeaponID weaponRecord::GetWeaponID () const
{
   return m_weapon_id;
}

double weaponRecord::GetAssignmentDelay () const
{
   return m_parent_asset->getAssignmentDelay();
}

positionRecord weaponRecord::GetPosition () const
{
   return m_parent_asset->getPosition();
}

std::shared_ptr<assetRecord> weaponRecord::GetParentAsset () const
{
   return m_parent_asset;
}


void weaponRecord::SetEstimatedMunitionsPrepped (const int value)
{
	if (value > m_EstimatedTotalMunitions)
		m_EstimatedMunitionsPrepped = m_EstimatedTotalMunitions;
	else
		m_EstimatedMunitionsPrepped = value;
}

void weaponRecord::SetEstimatedTotalMunitions (const int value)
{
   m_EstimatedTotalMunitions = value;
}


int weaponRecord::GetEstimatedMunitionsPrepped () const
{
	int est_munitions = 0;

	if (m_EstimatedMunitionsPrepped >= 0)
		est_munitions = m_EstimatedMunitionsPrepped;

	return est_munitions;
}

int weaponRecord::GetEstimatedTotalMunitions () const
{
	int est_munitions = 0;

	if (m_EstimatedTotalMunitions >= 0)
		est_munitions = m_EstimatedTotalMunitions;

	return est_munitions;
}

void weaponRecord::SetTotalFireChannels (const int value)
{
   m_TotalFireChannels = value;
}

int weaponRecord::GetTotalFireChannels () const
{
   return m_TotalFireChannels;
}

void weaponRecord::SetEstimatedAllocatedFireChannels (const int value)
{
   if (value < 0)
      m_EstimatedAllocatedFireChannels = 0;
   else if (value > m_TotalFireChannels)
      m_EstimatedAllocatedFireChannels = m_TotalFireChannels;
   else
      m_EstimatedAllocatedFireChannels = value;
}

int weaponRecord::GetEstimatedAllocatedFireChannels () const
{
   return m_EstimatedAllocatedFireChannels;
}

int weaponRecord::GetEstimatedAvailableFireChannels () const
{
   return m_TotalFireChannels - m_EstimatedAllocatedFireChannels;
}

int weaponRecord::GetTruthAllocatedFireChannels () const
{
   HCL_WARN_LOGGER(m_parent_asset->getGlobalLogger()) << "Warning: Call to interface intended to be abstract: weaponRecord::GetTruthAllocatedFireChannels()";
   return GetEstimatedAllocatedFireChannels();
}

void weaponRecord::IncrementEstimations (const int assign_incr, const int munition_incr)
{
   // adjust assignments - to increment allocations, we are adding back fire channels so it is reversed
   SetEstimatedAllocatedFireChannels (m_EstimatedAllocatedFireChannels - assign_incr);

   // adjust munitions
   SetEstimatedTotalMunitions (m_EstimatedTotalMunitions + munition_incr);
   SetEstimatedMunitionsPrepped (m_EstimatedMunitionsPrepped + munition_incr);
}

void weaponRecord::DecrementEstimations (const int assign_incr, const int munition_incr)
{
   // adjust assignments - to decrement allocations, we are subtracting, so it is reversed
   SetEstimatedAllocatedFireChannels (m_EstimatedAllocatedFireChannels + assign_incr);

   // adjust munitions
   SetEstimatedTotalMunitions (m_EstimatedTotalMunitions - munition_incr);
   SetEstimatedMunitionsPrepped (m_EstimatedMunitionsPrepped - munition_incr);
}

int weaponRecord::GetTruthMunitionsPrepped () const
{
   HCL_WARN_LOGGER(m_parent_asset->getGlobalLogger()) << "Warning: Call to interface intended to be abstract: weaponRecord::GetTruthMunitionsPrepped()";
   return GetEstimatedMunitionsPrepped();
}

int weaponRecord::GetTruthTotalMunitions () const
{
   HCL_WARN_LOGGER(m_parent_asset->getGlobalLogger()) << "Warning: Call to interface intended to be abstract: weaponRecord::GetTruthTotalMunitions()";
   return GetEstimatedTotalMunitions();
}

void weaponRecord::AddZone(const std::shared_ptr<zoneRecordInterface> &zone)
{
   m_attached_zones.Add (zone);
}

const std::vector<std::shared_ptr<zoneRecordInterface>> weaponRecord::GetAttachedZones() const
{
   return m_attached_zones.GetZones();
}

bool weaponRecord::HasZones () const
{
   return m_attached_zones.GetZoneCount() != 0;
}

} // namespace il
