// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_ASSET_HPP
#define WSF_BM_ASSET_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <UtInput.hpp>
#include <WsfObject.hpp>
#include <WsfPlatform.hpp>
#include <WsfZoneDefinition.hpp>

#include "WsfBMCommon.hpp"
#include "WsfBMWeaponInterface.hpp"

#include <iadsLib/assetRecord.hpp>

class WsfAssetMap;

class WSF_IADS_C2_LIB_EXPORT WsfBMAssetRecord : public WsfObject
{
   public:

      WsfBMAssetRecord(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      WsfBMAssetRecord(const std::shared_ptr<il::assetRecord>& raw_record);
      WsfBMAssetRecord(const WsfBMAssetRecord& copy);

      WsfBMAssetRecord* Clone() const override;

      inline const char* GetScriptClassName() const override { return "WsfBMAssetRecord"; }

      // Wrapper accessors
      inline const std::shared_ptr<il::assetRecord>& GetRawRecord() const { return m_record; }
      inline void SetRawRecord(const std::shared_ptr<il::assetRecord>& record) { m_record = record; }

   private:
      std::shared_ptr<il::assetRecord> m_record;
};

#endif
