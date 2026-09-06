// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED
#include <memory>

#include "WsfAssetMap.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfBMZoneRecord.hpp"
#include <iadsLib/enum.hpp>
#include <UtException.hpp>
#include <iadsLib/zoneRecordInterface.hpp>

WsfBMAssetRecord::WsfBMAssetRecord(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
{
   m_record = std::shared_ptr<il::assetRecord>(new il::assetRecord(aGlobalLogger));
}

WsfBMAssetRecord::WsfBMAssetRecord(const std::shared_ptr<il::assetRecord>& raw_record)
{
   m_record = raw_record;
}

WsfBMAssetRecord::WsfBMAssetRecord(const WsfBMAssetRecord& copy)
   : WsfObject(copy)
{
   m_record.reset(new il::assetRecord(copy.m_record->getGlobalLogger()));
   *m_record = *copy.m_record;
}

WsfBMAssetRecord* WsfBMAssetRecord::Clone() const
{
   return new WsfBMAssetRecord(*this);
}
