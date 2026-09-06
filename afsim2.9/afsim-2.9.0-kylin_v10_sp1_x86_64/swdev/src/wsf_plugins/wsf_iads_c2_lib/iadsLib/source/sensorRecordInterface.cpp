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
*   File: sensorRecordInterface.cpp
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

#include <iadsLib/sensorRecordInterface.hpp>
#include <iadsLib/assetRecord.hpp>
#include <logger.hpp>

namespace il
{

sensorRecord::sensorRecord ()
{

}

sensorRecord::sensorRecord (const std::shared_ptr<assetRecord> &parent_asset, const tSensorID &sensor_id, const unitTypeRecord &sensor_type)
	: m_parent_asset (parent_asset)
	, m_sensor_id (sensor_id)
	, m_sensor_type (sensor_type)
   , m_estimated_active_cues (0)
   , m_max_cues (0)
{

}

idRecord sensorRecord::GetParentAssetID () const
{
	return m_parent_asset->getID();
}

unitTypeRecord sensorRecord::GetType () const
{
	return m_sensor_type;
}

tSensorID sensorRecord::GetSensorID () const
{
	return m_sensor_id;
}

double sensorRecord::GetAssignmentDelay () const
{
	return m_parent_asset->getAssignmentDelay();
}

positionRecord sensorRecord::GetPosition () const
{
	return m_parent_asset->getPosition();
}

std::shared_ptr<assetRecord> sensorRecord::GetParentAsset () const
{
	return m_parent_asset;
}

// based off status perceptions
void sensorRecord::SetMaxActiveCues (const int max_cues)
{
   m_max_cues = max_cues;
}

int sensorRecord::GetMaxActiveCues () const
{
   return m_max_cues;
}

void sensorRecord::SetEstimatedActiveCues (const int num_cues)
{
   m_estimated_active_cues = num_cues;
}

int sensorRecord::GetEstimatedActiveCues() const
{
   return m_estimated_active_cues;
}

bool sensorRecord::HasEstimatedAvailableCues() const
{
   return m_max_cues - m_estimated_active_cues > 0;
}

void sensorRecord::IncrementAvailableCues(const int by_num)
{
   m_estimated_active_cues += by_num;
}

void sensorRecord::DecrementAvailableCues(const int by_num)
{
   m_estimated_active_cues -= by_num;
}


// All four Is__() functions return false. Subclass functions will override the function to return true as appropriate.
bool sensorRecord::IsEW () const
{
	return false;
}

bool sensorRecord::IsTAR () const
{
	return false;
}

bool sensorRecord::IsTTR () const
{
	return false;
}

bool sensorRecord::IsRWR() const
{
   return false;
}

bool sensorRecord::IsCueable () const
{
   return IsTTR();
}


int sensorRecord::GetTruthActiveCues(std::weak_ptr<logger::GlobalLogger> aGlobalLogger) const
{
   HCL_WARN_LOGGER(aGlobalLogger) << "Warning: Call to interface intended to be abstract: sensorRecord::GetTruthActiveCues()";
   return GetMaxActiveCues();
}

} // namespace il
