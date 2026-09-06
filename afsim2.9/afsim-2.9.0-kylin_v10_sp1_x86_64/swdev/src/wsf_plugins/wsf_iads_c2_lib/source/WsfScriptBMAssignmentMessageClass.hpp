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
*   File: WsfScriptBMAssignmentMessageClass.hpp
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

#ifndef WSF_SCRIPT_BM_ASSIGNMENT_MESSAGE_CLASS_HPP
#define WSF_SCRIPT_BM_ASSIGNMENT_MESSAGE_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>
#include <UtMemory.hpp>

class WsfScriptBMAssignmentMessageClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMAssignmentMessageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAssignmentMessageClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetAssignmentTime);
      UT_DECLARE_SCRIPT_METHOD(GetAssignmentTime);

      UT_DECLARE_SCRIPT_METHOD(SetReferenceTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetReferenceTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetMasterTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetMasterTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetAssignedWeapon);
      UT_DECLARE_SCRIPT_METHOD(GetAssignedPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetAssignedWeapon);

      UT_DECLARE_SCRIPT_METHOD(SetInitiatingPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetInitiatingPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetAssigningPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetAssigningPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetSelfDefenseFlag);
      UT_DECLARE_SCRIPT_METHOD(GetSelfDefenseFlag);

      UT_DECLARE_SCRIPT_METHOD(SetShotDoctrineS1);
      UT_DECLARE_SCRIPT_METHOD(SetShotDoctrineS2);
      UT_DECLARE_SCRIPT_METHOD(SetShotDoctrineSLS);
      UT_DECLARE_SCRIPT_METHOD(IsShotDoctrineS1);
      UT_DECLARE_SCRIPT_METHOD(IsShotDoctrineS2);
      UT_DECLARE_SCRIPT_METHOD(IsShotDoctrineSLS);
      UT_DECLARE_SCRIPT_METHOD(GetShotDoctrineString);

      UT_DECLARE_SCRIPT_METHOD(SetMaxFiringTime);
      UT_DECLARE_SCRIPT_METHOD(GetMaxFiringTime);

      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReasonNewAssignment);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReasonRetransmit);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReasonUpdate);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReasonReassignment);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReasonCancel);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentReasonNewAssignment);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentReasonRetransmit);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentReasonUpdate);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentReasonReassignment);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentReasonCancel);
      UT_DECLARE_SCRIPT_METHOD(GetAssignmentReasonString);

      UT_DECLARE_SCRIPT_METHOD(Print);
};

#endif
