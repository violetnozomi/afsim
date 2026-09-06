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
*   File: trackROE.cpp
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
*   Abstract: Class for abstracting track rules of engagement.
*
*******************************************************************************/



#include <iadsLib/trackROE.hpp>
#include <iadsLib/trackRecord.hpp>

namespace il
{

void trackROE::SetEngageIffRule (const eIFFType iff, bool value)
{
   m_rules[iff] = value;
}

bool trackROE::IsEngageable (const std::shared_ptr<trackRecord> &track) const
{
   bool is_engageable = false;

   auto rule = m_rules.find (track->getIFF());

   if (rule != m_rules.end())
      is_engageable = rule->second;

   return is_engageable;
}

} // namespace il
