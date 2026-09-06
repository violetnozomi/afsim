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
*   File: exclusionArray.cpp
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
*   Abstract: Class for abstracting assignment exclusion arrays.
*
*******************************************************************************/

#include <utility>
#include <iadsLib/exclusionArray.hpp>
#include <iadsLib/assignmentACKMessage.hpp>

namespace il
{

void exclusionArray::addExclusion (const std::shared_ptr<assignmentACKMessage> &ack)
{
   assignmentACKMessage::eSystemicCantcoScope scope;
   if (ack->getSystemicCantcoScope(scope))
   {
      if (scope == assignmentACKMessage::LOCAL)
      {
         m_local_exclusion_array.insert (std::make_pair (ack->getAssignmentReferenceTrackID(), ack->getAssignedID()));
      }
      else if (scope == assignmentACKMessage::GLOBAL)
      {
         m_global_exclusion_array[ack->getAssignedID()] = true;
      }
   }
}

bool exclusionArray::isAssetExcluded (const idRecord &track_id, const idRecord &asset_id) const
{
   return isAssetGloballyExcluded (asset_id) || isAssetLocallyExcluded (track_id, asset_id);
}


bool exclusionArray::isAssetGloballyExcluded (const idRecord &asset_id) const
{
   bool excluded = false;

   auto entry = m_global_exclusion_array.find (asset_id);
   if (entry != m_global_exclusion_array.end())
      excluded = entry->second;

   return excluded;
}

bool exclusionArray::isAssetLocallyExcluded (const idRecord &track_id, const idRecord &asset_id) const
{
   bool excluded = false;

   auto excluded_assets = m_local_exclusion_array.equal_range (track_id);

   // Loop through all assignments regarding this track ID.
   for (auto &excluded_asset = excluded_assets.first; excluded_asset != excluded_assets.second && !excluded; ++excluded_asset)
   {
      if (excluded_asset->second == asset_id)
         excluded = true;
   }

   return excluded;
}

} // namespace il
