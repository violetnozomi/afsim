// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_ASSET_CLASS_HPP
#define WSF_SCRIPT_BM_ASSET_CLASS_HPP

#include <UtInput.hpp>
#include <script/WsfScriptObjectClass.hpp>
#include "WsfBMCommon.hpp"

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMAssetClass : public WsfScriptObjectClass
{
   public:
      WsfScriptBMAssetClass(const std::string& aClassName, UtScriptTypes*     aScriptTypesPtr);

      void* Create(const UtScriptContext& aInstance) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(SetPlatform); // Set the underlying Platform this asset references
      UT_DECLARE_SCRIPT_METHOD(SetCommanderPlatform);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentDelay);
      UT_DECLARE_SCRIPT_METHOD(SetWeaponInfo);
      UT_DECLARE_SCRIPT_METHOD(IsWeapon);

      UT_DECLARE_SCRIPT_METHOD(AddDirectSubordinate);
      UT_DECLARE_SCRIPT_METHOD(AddPeer);
};

#endif
