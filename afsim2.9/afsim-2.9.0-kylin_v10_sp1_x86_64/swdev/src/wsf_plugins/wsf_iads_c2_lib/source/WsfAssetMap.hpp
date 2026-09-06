// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_ASSET_MAP
#define WSF_ASSET_MAP

#include <map>
#include <memory>
#include <iadsLib/assetRecord.hpp>
#include <WsfObject.hpp>
#include <logger.hpp>

class WsfBMAssetRecord;

class WsfAssetMap : public WsfObject
{
   public:
      WsfAssetMap(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
         : m_assets(nullptr)
      {
         HCL_TRACE_LOGGER(aGlobalLogger) << "WsfAssetMap(): m_assets = nullptr\n";
      }

      WsfAssetMap(il::assetMap* assets, std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
         : m_assets(assets)
      {
#ifdef IADSLIB_POINTER_TRACE
         HCL_TRACE_LOGGER(aGlobalLogger) << "WsfAssetMap(il::assetMap *assets): m_assets = " << m_assets << std::endl;
#endif
      }

      virtual void SetAssetMap(il::assetMap* assets)
      {
         m_assets = assets;
         HCL_TRACE_LOGGER(mGlobalLogger) << "SetAssetMap(): m_assets = " << m_assets << std::endl;
      }

      WsfAssetMap* Clone() const override;
      const char* GetScriptClassName() const override { return "WsfBMAssetMap"; }

      inline il::assetMap* GetRawAssetsPtr()
      {
         // HCL_TRACE_LOGGER() << "GetRawAssetsPtr(): m_assets = " << m_assets << std::endl;
         return m_assets;
      }

      void Add(const WsfBMAssetRecord* record);
      bool Exists(const il::idRecord& asset_id) const;
      bool GetAsset(const il::idRecord& asset_id, WsfBMAssetRecord* record) const;
      std::weak_ptr<il::logger::GlobalLogger> GetGlobalLogger() const { return mGlobalLogger; }

   protected:
      std::weak_ptr<il::logger::GlobalLogger> mGlobalLogger;

   private:

      il::assetMap*  m_assets; // TTD: JLK need to better define the shared vs nonshared semantics
};

#endif
