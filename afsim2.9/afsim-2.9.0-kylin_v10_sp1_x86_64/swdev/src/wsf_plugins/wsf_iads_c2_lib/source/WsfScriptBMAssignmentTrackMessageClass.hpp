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
*   File: WsfScriptBMAssignmentTrackMessageClass.hpp
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

#ifndef WSF_SCRIPT_BM_ASSIGNMENT_TRACK_MESSAGE_CLASS_HPP
#define WSF_SCRIPT_BM_ASSIGNMENT_TRACK_MESSAGE_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>

class WsfScriptBMAssignmentTrackMessageClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMAssignmentTrackMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAssignmentTrackMessageClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetTrack);
      UT_DECLARE_SCRIPT_METHOD(GetTrack);

      UT_DECLARE_SCRIPT_METHOD(SetAssignedPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetAssignedPlatform);
};

#endif
