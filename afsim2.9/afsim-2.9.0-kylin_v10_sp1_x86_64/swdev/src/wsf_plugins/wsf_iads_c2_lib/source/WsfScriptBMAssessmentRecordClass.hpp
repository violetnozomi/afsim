// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_SCRIPT_BM_ASSESSMENT_RECORD_CLASS_HPP
#define WSF_SCRIPT_BM_ASSESSMENT_RECORD_CLASS_HPP

#include <WsfBMCommon.hpp>
#include <UtScriptBasicTypes.hpp>
#include <iadsLib/enum.hpp>

class WsfSimulation;

class WSF_IADS_C2_LIB_EXPORT WsfScriptBMAssessmentRecordClass : public UtScriptClass
{
   public:
      WsfScriptBMAssessmentRecordClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
      virtual ~WsfScriptBMAssessmentRecordClass();

	  UT_DECLARE_SCRIPT_METHOD(SetWeaponTypePriority);
	  UT_DECLARE_SCRIPT_METHOD(GetWeaponTypePriority);
	  UT_DECLARE_SCRIPT_METHOD(SetWeaponSubtypePriority);
	  UT_DECLARE_SCRIPT_METHOD(GetWeaponSubtypePriority);
	  UT_DECLARE_SCRIPT_METHOD(SetWeaponExcluded);
	  UT_DECLARE_SCRIPT_METHOD(IsWeaponExcluded);
	  UT_DECLARE_SCRIPT_METHOD(SetPriority);
	  UT_DECLARE_SCRIPT_METHOD(GetPriority);
	  UT_DECLARE_SCRIPT_METHOD(GetDirectC2ID);
	  UT_DECLARE_SCRIPT_METHOD(GetDirectC2SubID);
	  UT_DECLARE_SCRIPT_METHOD(SetDirectC2ID);
	  UT_DECLARE_SCRIPT_METHOD(GetTrackID);
	  UT_DECLARE_SCRIPT_METHOD(GetTrackSubID);
	  UT_DECLARE_SCRIPT_METHOD(GetExistingAssignment);
	  UT_DECLARE_SCRIPT_METHOD(GetAssetID);
	  UT_DECLARE_SCRIPT_METHOD(GetAssetSubID);
	  UT_DECLARE_SCRIPT_METHOD(GetWeaponID);
	  UT_DECLARE_SCRIPT_METHOD(GetWeaponSubID);
	  UT_DECLARE_SCRIPT_METHOD(GetAssessedZoneTypes);
	  UT_DECLARE_SCRIPT_METHOD(GetPredictedInterceptPt);
	  UT_DECLARE_SCRIPT_METHOD(CanInterceptTrack);
	  UT_DECLARE_SCRIPT_METHOD(IsInterceptInsideC2Zone);
	  UT_DECLARE_SCRIPT_METHOD(IsInterceptInsideMaxRange);
	  UT_DECLARE_SCRIPT_METHOD(IsInterceptInsideZone);
	  UT_DECLARE_SCRIPT_METHOD(IsInterceptOutsideMinRange);
	  UT_DECLARE_SCRIPT_METHOD(IsLOSMasked);
	  UT_DECLARE_SCRIPT_METHOD(GetPotentialShots);
	  UT_DECLARE_SCRIPT_METHOD(GetAssignmentTime);
	  UT_DECLARE_SCRIPT_METHOD(GetDeflectionAngleRads);
	  UT_DECLARE_SCRIPT_METHOD(GetDistanceToPCA);
	  UT_DECLARE_SCRIPT_METHOD(GetInterceptDistance);
	  UT_DECLARE_SCRIPT_METHOD(GetInterceptPK);
	  UT_DECLARE_SCRIPT_METHOD(GetInterceptTime);
	  UT_DECLARE_SCRIPT_METHOD(GetMaxRange);
	  UT_DECLARE_SCRIPT_METHOD(GetProjectedTime);
};

#endif
