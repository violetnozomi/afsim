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
*   File: zoneRecordInterface.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#include <algorithm>
#include <iterator>
#include <sstream>

#include <logger.hpp>
#include <iadsLib/zoneRecordInterface.hpp>

namespace il
{

IADSLIB_EXPORT std::string zoneTypeToName(const eZoneType type_enum, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   std::string zone_type_str;

   switch (type_enum)
   {
      case E_ZONE_TYPE_OTHER:
         zone_type_str = "Other";
         break;
      case E_ZONE_TYPE_AOR:
         zone_type_str = "AOR";
         break;
      case E_ZONE_TYPE_SUR:
         zone_type_str = "SUR";
         break;
      case E_ZONE_TYPE_MEZ:
         zone_type_str = "MEZ";
         break;
      case E_ZONE_TYPE_FEZ:
         zone_type_str = "FEZ";
         break;
      case E_ZONE_TYPE_JEZ:
         zone_type_str = "JEZ";
         break;
      case E_ZONE_TYPE_GEZ:
         zone_type_str = "GEZ";
         break;
      case E_ZONE_TYPE_DA:
         zone_type_str = "DA";
         break;
      case E_ZONE_TYPE_COR:
         zone_type_str = "COR";
         break;
      default:
         std::stringstream strm_error;
         strm_error << "il::zoneTypeToName() Called with unknown zone type: " << type_enum << std::endl;
         HCL_FATAL_LOGGER(aGlobalLogger) << strm_error.str();
         throw std::runtime_error(strm_error.str());
         break;
   }

   return zone_type_str;
}

zoneContainer::zoneContainer(bool overwrite_existing)
   : m_overwrite_existing(overwrite_existing)
{
}

bool zoneContainer::Add(const std::shared_ptr<zoneRecordInterface> &zone)
{
   bool added = true;

   if (m_overwrite_existing ||
       m_zones.find(zone->getName()) == m_zones.end())
   { m_zones[zone->getName()] = zone; }
   else
   { added = false; }

   return added;
}

std::vector<std::shared_ptr<zoneRecordInterface>> zoneContainer::GetZones() const
{
   std::vector<std::shared_ptr<zoneRecordInterface>> zones;
   zones.reserve(m_zones.size());
   for (auto it = m_zones.cbegin(); it != m_zones.cend(); ++it)
   {
      zones.push_back(it->second);
   }
   return zones;
}

std::vector<std::shared_ptr<zoneRecordInterface>> zoneContainer::GetZones(const eZoneType& type) const
{
   std::vector<std::shared_ptr<zoneRecordInterface>> zones;
   zones.reserve(m_zones.size());
   for (auto it = m_zones.cbegin(); it != m_zones.cend(); ++it)
   {
      const auto &zone = it->second;
      if (zone->getTypeEnum() == type)
      {
         zones.push_back(zone);
      }
   }
   return zones;
}

std::vector<std::shared_ptr<zoneRecordInterface>> zoneContainer::GetZones(const unsigned long& e_zone_type_mask) const
{
   std::vector<std::shared_ptr<zoneRecordInterface>> zones;
   zones.reserve(m_zones.size());
   for (auto it = m_zones.cbegin(); it != m_zones.cend(); ++it)
   {
      const auto &zone = it->second;
      if (zone->getTypeEnum() & e_zone_type_mask)
      {
         zones.push_back(zone);
      }
   }
   return zones;
}

std::size_t zoneContainer::GetZoneCount() const
{
   return m_zones.size();
}

} // namespace il
