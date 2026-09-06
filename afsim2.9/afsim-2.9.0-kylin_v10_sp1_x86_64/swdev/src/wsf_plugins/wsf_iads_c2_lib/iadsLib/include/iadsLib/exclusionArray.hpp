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
*   File: exclusionArray.hpp
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

#ifndef IADSLIB_EXCLUSION_ARRAY_HPP
#define IADSLIB_EXCLUSION_ARRAY_HPP

#include <map>
#include <memory>
#include <iadsLib/idRecord.hpp>


namespace il
{
   class assignmentACKMessage;

   class exclusionArray
   {
   public:
      ~exclusionArray() = default;
      virtual void addExclusion (const std::shared_ptr<assignmentACKMessage> &ack);
      virtual bool isAssetExcluded (const idRecord &track_id, const idRecord &asset_id) const;
   protected:
      // Exclusion arrays
      std::multimap <idRecord, idRecord> m_local_exclusion_array; // track, asset - remove this asset from further consideration for this track
      std::map <idRecord, bool> m_global_exclusion_array;          // asset - remove this asset from consideration of all tracks in the future

      bool isAssetGloballyExcluded (const idRecord &asset_id) const;
      bool isAssetLocallyExcluded (const idRecord &track_id, const idRecord &asset_id) const;
   };


}

#endif