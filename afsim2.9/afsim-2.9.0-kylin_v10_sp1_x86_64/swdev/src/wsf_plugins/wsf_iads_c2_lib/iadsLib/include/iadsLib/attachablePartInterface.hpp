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
*   File: attachablePartInterface.hpp
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

#ifndef IADSLIB_ATTACHABLE_PART_INTERFACE
#define IADSLIB_ATTACHABLE_PART_INTERFACE

#include <memory>

#include <iadsLib/idRecord.hpp>
#include <iadsLib/unitTypeRecord.hpp>

namespace il
{
   class assetRecord;
   class positionRecord;

   typedef idRecord tPartID;
   typedef unitTypeRecord tPartType;

   class attachablePartInterface
   {
   public:
      attachablePartInterface ();
      ~attachablePartInterface() = default;
      attachablePartInterface (const std::shared_ptr<assetRecord> &parent_asset, const tPartID &part_id, const unitTypeRecord &part_type);

      virtual unitTypeRecord GetPartType () const;
      virtual tPartID GetPartID () const;
      virtual std::shared_ptr<assetRecord> GetParentAsset () const;

      // parent-convenience routines
      virtual idRecord GetParentAssetID () const;
      virtual positionRecord GetPosition () const;

   protected:
      std::shared_ptr<assetRecord> m_parent_asset;
      tPartID m_part_id;
      tPartType m_part_type;
   };

}


#endif

