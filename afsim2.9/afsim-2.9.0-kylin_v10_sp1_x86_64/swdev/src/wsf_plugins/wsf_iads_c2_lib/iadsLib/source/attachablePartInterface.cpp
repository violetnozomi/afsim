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
*   File: attachablePartInterface.cpp
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

#include <exception>

#include <iadsLib/attachablePartInterface.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/positionRecord.hpp>


namespace il
{

attachablePartInterface::attachablePartInterface ()
{ }

attachablePartInterface::attachablePartInterface (const std::shared_ptr<assetRecord> &parent_asset, const tPartID &part_id, const unitTypeRecord &part_type)
   : m_parent_asset (parent_asset)
   , m_part_id (part_id)
   , m_part_type (part_type)
{ }

unitTypeRecord attachablePartInterface::GetPartType () const
{
   return m_part_type;
}

tPartID attachablePartInterface::GetPartID () const
{
   return m_part_id;
}

std::shared_ptr<assetRecord> attachablePartInterface::GetParentAsset () const
{
   return m_parent_asset;
}

idRecord attachablePartInterface::GetParentAssetID () const
{
   if (!m_parent_asset)
      throw std::runtime_error ("attachablePartInterface::GetParentAssetID() but parent asset not set");

   return m_parent_asset->getID();
}

positionRecord attachablePartInterface::GetPosition () const
{
   if (!m_parent_asset)
      throw std::runtime_error ("attachablePartInterface::GetPosition() but parent asset not set");

   return m_parent_asset->getPosition();
}

}



