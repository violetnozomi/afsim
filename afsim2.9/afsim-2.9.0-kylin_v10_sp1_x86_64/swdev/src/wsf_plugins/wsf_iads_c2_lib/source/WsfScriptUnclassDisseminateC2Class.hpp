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
*   File: WsfScriptUnclassDisseminateC2Class.hpp
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

#ifndef WSF_SCRIPT_UNCLASS_DISSEMINATE_C2_CLASS_HPP
#define WSF_SCRIPT_UNCLASS_DISSEMINATE_C2_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <script/WsfScriptProcessorClass.hpp>
#include "WsfScriptDisseminateC2Class.hpp"

#define UNCLASS_DC2_SCRIPT_CLASS_NAME "WsfUnclassDisseminateC2"
#define UNCLASS_DC2_BASE_CLASS_NAME "WSF_UNCLASS_DISSEMINATE_C2"

class WsfScriptUnclassDisseminateC2Class : public WsfScriptDisseminateC2Class
{
   public:

      WsfScriptUnclassDisseminateC2Class(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptUnclassDisseminateC2Class() = default;

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void  Destroy(void* aObjectPtr) override;

      // Plugin API.
      static WSF_IADS_C2_LIB_EXPORT const char* GetScriptClassName();
      static WSF_IADS_C2_LIB_EXPORT const char* GetBaseClassName();
};

#endif
