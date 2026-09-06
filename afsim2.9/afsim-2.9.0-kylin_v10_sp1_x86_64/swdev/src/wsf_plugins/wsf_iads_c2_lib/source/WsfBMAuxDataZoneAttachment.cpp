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
*   File: WsfBMAuxDataZoneAttachment.cpp
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
*   Abstract: A container class for providing zone attachments to weapons or
*    platforms within aux data. This is to get around the fact that container
*    classes aren't supported within AUX data for the AFSIM core currently.
*
*******************************************************************************/

#include "WsfBMAuxDataZoneAttachment.hpp"

WsfBMAuxDataZoneAttachment::WsfBMAuxDataZoneAttachment()
{
}

WsfBMAuxDataZoneAttachment* WsfBMAuxDataZoneAttachment::Clone() const
{
   return new WsfBMAuxDataZoneAttachment(*this);
}

void WsfBMAuxDataZoneAttachment::AddZoneAttachment(const eAttachmentType attach_type, const double priority, const std::string& zone_name, const il::eZoneType zone_type)
{
   m_zones.push_back(tAttachment(priority, attach_type, zone_name, zone_type));
}

std::size_t WsfBMAuxDataZoneAttachment::GetZoneAttachmentCount() const
{
   return m_zones.size();
}

void WsfBMAuxDataZoneAttachment::GetNthZoneAttachment(const std::size_t nth_zone, eAttachmentType& attach_type, double& priority, std::string& zone_name, il::eZoneType& zone_type) const
{
   auto& entry = m_zones.at(nth_zone);
   attach_type = entry.m_attachment;
   priority = entry.m_priority;
   zone_name = entry.m_zone_name;
   zone_type = entry.m_zone_type;
}
