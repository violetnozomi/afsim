// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <WsfBMAssetRecord.hpp>
#include <WsfAssetMap.hpp>

WsfAssetMap* WsfAssetMap::Clone() const
{
   return new WsfAssetMap(*this);
}

void WsfAssetMap::Add(const WsfBMAssetRecord* record)
{
   m_assets->AddAsset(record->GetRawRecord()->getID(), record->GetRawRecord());
}

bool WsfAssetMap::Exists(const il::idRecord& asset_id) const
{
   return m_assets->GetAsset(asset_id) != nullptr;
}

bool WsfAssetMap::GetAsset(const il::idRecord& asset_id, WsfBMAssetRecord* record) const
{
   bool got_record = false;

   auto asset = m_assets->GetAsset(asset_id);

   if (asset)
   {
      record->SetRawRecord(asset);
      got_record = true;
   }

   return got_record;
}
