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
*   File: WsfScriptBMAssignmentStatusClass.hpp
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

#ifndef WSF_SCRIPT_BM_ASSIGNMENT_STATUS_CLASS_HPP
#define WSF_SCRIPT_BM_ASSIGNMENT_STATUS_CLASS_HPP

#include <script/WsfScriptMessageClass.hpp>
#include <UtMemory.hpp>

class WsfScriptBMAssignmentStatusClass : public WsfScriptMessageClass
{
   public:
      WsfScriptBMAssignmentStatusClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAssignmentStatusClass();

      void* Create(const UtScriptContext& aContext) override;
      void* Clone(void* aObjectPtr) override;
      void Destroy(void* aObjectPtr) override;

      static const char* SCRIPT_CLASS_NAME;
      static const char* BASE_CLASS_NAME;

      UT_DECLARE_SCRIPT_METHOD(SetAckTime);
      UT_DECLARE_SCRIPT_METHOD(GetAckTime);

      UT_DECLARE_SCRIPT_METHOD(SetAssignmentReferenceTrackID);
      UT_DECLARE_SCRIPT_METHOD(GetAssignmentReferenceTrackID);

      UT_DECLARE_SCRIPT_METHOD(SetAssignedWeapon);
      UT_DECLARE_SCRIPT_METHOD(GetAssignedPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetAssignedWeapon);

      UT_DECLARE_SCRIPT_METHOD(SetInitiatingPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetInitiatingPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetAssigningPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetAssigningPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusUnacknowledged);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusWilco);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusCommitted);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusCueTAR);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusCueTTR);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusTARSearching);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusTTRSearching);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusTARTracking);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusTTRTracking);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusFiring);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusMiss);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusChangedAssignedUnit);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusDoctrineUpdated);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusKill);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusCancelled);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusHavecoSuccess);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusHavecoFailure);
      UT_DECLARE_SCRIPT_METHOD(SetAssignmentStatusCantco);

      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusUnacknowledged);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusWilco);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusCommitted);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusCueTAR);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusCueTTR);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusTARSearching);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusTTRSearching);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusTARTracking);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusTTRTracking);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusFiring);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusMiss);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusChangedAssignedUnit);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusDoctrineUpdated);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusKill);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusCancelled);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusHavecoSuccess);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusHavecoFailure);
      UT_DECLARE_SCRIPT_METHOD(IsAssignmentStatusCantco);

      UT_DECLARE_SCRIPT_METHOD(SetSalvosFired);
      UT_DECLARE_SCRIPT_METHOD(GetSalvosFired);

      UT_DECLARE_SCRIPT_METHOD(SetCantcoReason);
      UT_DECLARE_SCRIPT_METHOD(GetCantcoReason);

      UT_DECLARE_SCRIPT_METHOD(SetSystemicCantcoGlobal);
      UT_DECLARE_SCRIPT_METHOD(SetSystemicCantcoLocal);
      UT_DECLARE_SCRIPT_METHOD(IsSystemicCantcoGlobal);
      UT_DECLARE_SCRIPT_METHOD(IsSystemicCantcoLocal);

      UT_DECLARE_SCRIPT_METHOD(SetOverridingPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetOverridingPlatform);

      UT_DECLARE_SCRIPT_METHOD(SetNewlyAssignedWeapon);
      UT_DECLARE_SCRIPT_METHOD(GetNewlyAssignedPlatform);
      UT_DECLARE_SCRIPT_METHOD(GetNewlyAssignedWeapon);

      UT_DECLARE_SCRIPT_METHOD(Print);
};

#endif
