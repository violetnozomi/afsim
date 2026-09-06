// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_WEAPON_INTERFACE_HPP
#define WSF_BM_WEAPON_INTERFACE_HPP

#include <memory>

#include <WsfObject.hpp>
#include <WsfPlatform.hpp>

#include "WsfBMCommon.hpp"
#include "WsfBMWeaponInfoRecord.hpp"

class WsfBMAssetRecord;

class WSF_IADS_C2_LIB_EXPORT WsfBMWeaponInterface : public WsfObject
{
   public:

      WsfBMWeaponInterface();
      WsfBMWeaponInterface(const WsfBMWeaponInterface& aSrc);
      virtual ~WsfBMWeaponInterface();

      //! The standard Clone() method (virtual copy constructor).
      WsfBMWeaponInterface* Clone() const override;
      const char* GetScriptClassName() const override { return "WsfBMWeaponInterface"; }

      virtual std::shared_ptr<WsfBMWeaponInfoRecord>& GetRawRecord();

   private:
      std::shared_ptr<WsfBMWeaponInfoRecord> m_record;
};

#endif
