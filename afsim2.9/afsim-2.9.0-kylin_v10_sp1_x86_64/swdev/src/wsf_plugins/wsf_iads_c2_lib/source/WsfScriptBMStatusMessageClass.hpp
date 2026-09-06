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
*   File: WsfScriptBMStatusMessageClass.hpp
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
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_SCRIPT_BM_STATUS_MESSAGE_CLASS_HPP
#define WSF_SCRIPT_BM_STATUS_MESSAGE_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>
#include <UtMemory.hpp>

class WsfScriptBMStatusMessageClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMStatusMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMStatusMessageClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetStatusTime);
      UT_DECLARE_SCRIPT_METHOD(GetStatusTime);

      UT_DECLARE_SCRIPT_METHOD(SetPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetMaxAssignments);
      UT_DECLARE_SCRIPT_METHOD(GetMaxAssignments);
      UT_DECLARE_SCRIPT_METHOD(SetNumAssignments);
      UT_DECLARE_SCRIPT_METHOD(GetNumAssignments);

      UT_DECLARE_SCRIPT_METHOD(SetWeaponStatus);

      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusWhite);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusGreen);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusYellow);
      UT_DECLARE_SCRIPT_METHOD(SetSystemStatusRed);

      UT_DECLARE_SCRIPT_METHOD(IsSystemStatusWhite);
      UT_DECLARE_SCRIPT_METHOD(IsSystemStatusGreen);
      UT_DECLARE_SCRIPT_METHOD(IsSystemStatusYellow);
      UT_DECLARE_SCRIPT_METHOD(IsSystemStatusRed);

      UT_DECLARE_SCRIPT_METHOD(SetPosition);
      UT_DECLARE_SCRIPT_METHOD(HasPosition);
      UT_DECLARE_SCRIPT_METHOD(GetPosition);
      UT_DECLARE_SCRIPT_METHOD(GetPositionTime);

      UT_DECLARE_SCRIPT_METHOD(SetVelocityECEF);
      UT_DECLARE_SCRIPT_METHOD(HasVelocity);
      UT_DECLARE_SCRIPT_METHOD(GetVelocityECEF);
};

#endif
