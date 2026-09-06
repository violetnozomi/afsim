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
*   File: WsfScriptUnclassAssetManagerClass.hpp
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

#ifndef WSF_SCRIPT_UNCLASS_ASSET_MANAGER_CLASS_HPP
#define WSF_SCRIPT_UNCLASS_ASSET_MANAGER_CLASS_HPP

#include <script/WsfScriptProcessorClass.hpp>
#include "WsfScriptAssetManagerClass.hpp"

class WsfScriptUnclassAssetManagerClass : public WsfScriptAssetManagerClass
{
   public:

      WsfScriptUnclassAssetManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void  Destroy(void* aObjectPtr) override;

      // Plugin API.
      static WSF_IADS_C2_LIB_EXPORT const char* GetScriptClassName();
      static WSF_IADS_C2_LIB_EXPORT const char* GetBaseClassName();

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;
};

#endif
