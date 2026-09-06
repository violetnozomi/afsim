// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"

#include "iadsLib/assessmentRecord.hpp"

#include "WsfScriptBMAssessmentRecordClass.hpp"
#include "WsfBMWeaponInfoRecord.hpp"
#include "WsfBMUtils.hpp"

WsfScriptBMAssessmentRecordClass::WsfScriptBMAssessmentRecordClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMAssessmentRecord");
   mConstructible = false;
   mCloneable = false;

   AddMethod(ut::make_unique<SetWeaponTypePriority>());
   AddMethod(ut::make_unique<GetWeaponTypePriority>());
   AddMethod(ut::make_unique<SetWeaponSubtypePriority>());
   AddMethod(ut::make_unique<GetWeaponSubtypePriority>());
   AddMethod(ut::make_unique<SetWeaponExcluded>());
   AddMethod(ut::make_unique<IsWeaponExcluded>());
   AddMethod(ut::make_unique<SetPriority>());
   AddMethod(ut::make_unique<GetPriority>());
   AddMethod(ut::make_unique<GetDirectC2ID>());
   AddMethod(ut::make_unique<GetDirectC2SubID>());
   AddMethod(ut::make_unique<SetDirectC2ID>());
   //AddMethod(ut::make_unique<GetWeapon>());
   //AddMethod(ut::make_unique<GetTrack>());
   //AddMethod(ut::make_unique<GetExistingAssignment>());
   //AddMethod(ut::make_unique<GetAssessedThreatRecord>());
   //AddMethod(ut::make_unique<GetAssetID>());
   //AddMethod(ut::make_unique<GetWeaponID>());
   AddMethod(ut::make_unique<GetAssessedZoneTypes>());
   AddMethod(ut::make_unique<GetPredictedInterceptPt>());
   AddMethod(ut::make_unique<CanInterceptTrack>());
   AddMethod(ut::make_unique<IsInterceptInsideC2Zone>());
   AddMethod(ut::make_unique<IsInterceptInsideMaxRange>());
   AddMethod(ut::make_unique<IsInterceptInsideZone>());
   AddMethod(ut::make_unique<IsInterceptOutsideMinRange>());
   AddMethod(ut::make_unique<IsLOSMasked>());
   AddMethod(ut::make_unique<GetPotentialShots>());
   AddMethod(ut::make_unique<GetAssignmentTime>());
   AddMethod(ut::make_unique<GetDeflectionAngleRads>());
   AddMethod(ut::make_unique<GetDistanceToPCA>());
   AddMethod(ut::make_unique<GetInterceptDistance>());
   AddMethod(ut::make_unique<GetInterceptPK>());
   AddMethod(ut::make_unique<GetInterceptTime>());
   AddMethod(ut::make_unique<GetMaxRange>());
   AddMethod(ut::make_unique<GetProjectedTime>());
}

WsfScriptBMAssessmentRecordClass::~WsfScriptBMAssessmentRecordClass()
{
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, SetWeaponTypePriority, 1, "void", "int")
{
	aObjectPtr->setWeaponTypePriority(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetWeaponTypePriority, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getWeaponTypePriority());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, SetWeaponSubtypePriority, 1, "void", "int")
{
	aObjectPtr->setWeaponSubtypePriority(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetWeaponSubtypePriority, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getWeaponSubtypePriority());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, SetWeaponExcluded, 1, "void", "bool")
{
	aObjectPtr->setWeaponExcluded(aVarArgs[0].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsWeaponExcluded, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isWeaponExcluded());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, SetPriority, 1, "void", "double")
{
	aObjectPtr->setPriority(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetPriority, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getPriority());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetAssessedZoneTypes, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->GetAssessedZoneTypes());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetTrackID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getTrack()->getID().getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetTrackSubID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getTrack()->getID().getSubID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetExistingAssignment, 0, "", "")
{
	auto& internal_ptr = aObjectPtr->getExistingAssignment();

	if (internal_ptr)
	{
		WsfBMAssignmentMessage* ext_ptr = new WsfBMAssignmentMessage(WsfScriptContext::GetPLATFORM(aContext), internal_ptr);
		aReturnVal.SetPointer(new UtScriptRef(ext_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
	}
	else
	{
		aReturnVal.SetPointer(new UtScriptRef(0, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
	}
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetAssetID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->GetAssetID().getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetAssetSubID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->GetAssetID().getSubID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetWeaponID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->GetWeaponID().getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetWeaponSubID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->GetWeaponID().getSubID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetDirectC2ID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getDirectC2ID().getID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetDirectC2SubID, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getDirectC2ID().getSubID());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, SetDirectC2ID, 2, "void", "int, int")
{
	aObjectPtr->setDirectC2ID(il::idRecord(aVarArgs[0].GetInt(), aVarArgs[1].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetPredictedInterceptPt, 0, "WsfGeoPoint", "")
{
	il::positionRecord position = aObjectPtr->getPredictedInterceptPt();
	double locWCS[] = { position.getX(), position.getY(), position.getZ() };
	WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
	aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, CanInterceptTrack, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->canInterceptTrack());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsInterceptInsideC2Zone, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isInterceptInsideC2Zone());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsInterceptInsideMaxRange, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isInterceptInsideMaxRange());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsInterceptInsideZone, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isInterceptInsideZone());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsInterceptOutsideMinRange, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isInterceptOutsideMinRange());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, IsLOSMasked, 0, "bool", "")
{
	aReturnVal.SetBool(aObjectPtr->isLOSMasked());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetPotentialShots, 0, "int", "")
{
	aReturnVal.SetInt(aObjectPtr->getPotentialShots());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetAssignmentTime, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getAssignmentTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetDeflectionAngleRads, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getDeflectionAngleRads());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetDistanceToPCA, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getDistanceToPCA());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetInterceptDistance, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getInterceptDistance());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetInterceptPK, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getInterceptPK());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetInterceptTime, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getInterceptTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetMaxRange, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getMaxRange());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBMAssessmentRecordClass, il::assessmentRecord, GetProjectedTime, 0, "double", "")
{
	aReturnVal.SetDouble(aObjectPtr->getProjectedTime());
}
