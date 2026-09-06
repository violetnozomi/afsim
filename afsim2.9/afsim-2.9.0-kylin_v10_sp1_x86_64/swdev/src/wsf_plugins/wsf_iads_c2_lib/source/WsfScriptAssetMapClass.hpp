// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_ASSET_MAP_CLASS_HPP
#define WSF_SCRIPT_ASSET_MAP_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <UtScriptBasicTypes.hpp>
#include <UtMemory.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptAssetMapClass : public UtScriptClass
{
   public:
      WsfScriptAssetMapClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      ~WsfScriptAssetMapClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(Add);
      UT_DECLARE_SCRIPT_METHOD(Exists);
      UT_DECLARE_SCRIPT_METHOD(Get);
};

#endif
