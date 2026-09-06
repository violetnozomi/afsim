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
*   File: WsfScriptBMAuxDataZoneAttachmentClass.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: Script exports for WsfBMAuxDataZoneAttachment.
*
*******************************************************************************/

#ifndef WSF_SCRIPT_BM_AUX_DATA_ZONE_ATTACHMENT_CLASS_HPP
#define WSF_SCRIPT_BM_AUX_DATA_ZONE_ATTACHMENT_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <UtScriptBasicTypes.hpp>
#include <script/WsfScriptObjectClass.hpp>
#include <UtMemory.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMAuxDataZoneAttachmentClass : public WsfScriptObjectClass
{
   public:
      WsfScriptBMAuxDataZoneAttachmentClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAuxDataZoneAttachmentClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(Add);
      UT_DECLARE_SCRIPT_METHOD(GetCount);
      UT_DECLARE_SCRIPT_METHOD(IsNthScopeGlobal);
      UT_DECLARE_SCRIPT_METHOD(IsNthScopePlatform);
      UT_DECLARE_SCRIPT_METHOD(GetNthPriority);
      UT_DECLARE_SCRIPT_METHOD(GetNthZoneName);
      UT_DECLARE_SCRIPT_METHOD(IsNthTypeMEZ);
      UT_DECLARE_SCRIPT_METHOD(IsNthTypeFEZ);
      UT_DECLARE_SCRIPT_METHOD(IsNthTypeJEZ);
      UT_DECLARE_SCRIPT_METHOD(GetNthZoneTypeString);
};

#endif
