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
*   File: WsfScriptBMEvalAssignmentClass.hpp
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

#ifndef WSF_SCRIPT_BM_EVAL_ASSIGNMENT_CLASS_HPP
#define WSF_SCRIPT_BM_EVAL_ASSIGNMENT_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <UtScriptBasicTypes.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMEvalAssignmentClass : public UtScriptClass
{
   public:
      WsfScriptBMEvalAssignmentClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMEvalAssignmentClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      UT_DECLARE_SCRIPT_METHOD(GetTrack);
      UT_DECLARE_SCRIPT_METHOD(GetDelegation);
      UT_DECLARE_SCRIPT_METHOD(GetAssignment);
};

#endif
