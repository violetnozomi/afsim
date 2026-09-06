// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptThreatReportClass.hpp"

#include "UtMemory.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"
#include "WsfWeaponThreatProcessor.hpp"

// typedef WsfWeaponThreatProcessor::PkSearchParameters     PkSearchParameters;
using WsfThreatReport = WsfWeaponThreatProcessor::WsfThreatReport;
// typedef WsfWeaponThreatProcessor::WsfThreatReportList    WsfThreatReportList;
// typedef WsfWeaponThreatProcessor::ThreatEvaluator        ThreatEvaluator;


WsfScriptThreatReportClass::WsfScriptThreatReportClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfThreatReport");
   ////AddClassName(GetScriptClassname());

   AddMethod(ut::make_unique<TrackId>());
   // AddMethod(ut::make_unique<EntityTypeId>());
   //   AddMethod(ut::make_unique<WeaponTypeId>());
   AddMethod(ut::make_unique<EntityType>());
   AddMethod(ut::make_unique<WeaponType>());
   AddMethod(ut::make_unique<ProbabilityEnityType>());
   AddMethod(ut::make_unique<ProbabilityKillAtT0>());
   AddMethod(ut::make_unique<ProbabilityThreatAtT0>());
   AddMethod(ut::make_unique<TimeOfProbabilityThreatMax>());
   AddMethod(ut::make_unique<ProbabilityKillMax>());
   AddMethod(ut::make_unique<ProbabilityThreatMax>());
   AddMethod(ut::make_unique<ProbabilityOfDetection>());
   AddMethod(ut::make_unique<ProbabilityWeaponType>());
}

// virtual
void* WsfScriptThreatReportClass::Clone(void* aObjectPtr)
{
   // return static_cast<WsfThreatReport*>(aObjectPtr)->Clone();
   return new WsfThreatReport(*static_cast<WsfThreatReport*>(aObjectPtr));
}

void* WsfScriptThreatReportClass::Create(const UtScriptContext& /*aInstance*/)
{
   WsfThreatReport* reportPtr = new WsfThreatReport();
   // Perform any special initialization here if needed, like WsfTrack does.
   return reportPtr;
}

// virtual
void WsfScriptThreatReportClass::Destroy(void* aObjectPtr)
{
   WsfThreatReport* aObjectPtrCast = static_cast<WsfThreatReport*>(aObjectPtr);
   delete aObjectPtrCast;
}

//------------- Script Functions --------------------------

/** The ID of the track that was used for the report->
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, TrackId, 0, "WsfTrackId", "")
{
   WsfThreatReport* report = aObjectPtr;
   // These "script" objects are sent to Destroy after they
   // go out of scope.
   WsfTrackId* trackIdPtr = new WsfTrackId(report->mTrackId);
   aReturnVal.SetPointer(new UtScriptRef(trackIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

/** The candidate entity type.
 */
// UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, EntityTypeId, 0, "int", "")
//{
//    WsfThreatReport* report = aObjectPtr;
//    aReturnVal.SetInt(report->mEntityTypeId);
// }

/** The weapon type used for this reports pk.
 */
// UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, WeaponTypeId, 0, "int", "")
//{
//    WsfThreatReport* report = aObjectPtr;
//    aReturnVal.SetInt(report->mWeaponTypeId);
// }

/** The candidate entity type.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, EntityType, 0, "string", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetString(report->mEntityTypeId);
}

/** The weapon type used for this reports pk.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, WeaponType, 0, "string", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetString(report->mWeaponTypeId);
}

/** The probability of the given track being this type of entity
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityEnityType, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPe);
}

/** The probability of kill at the time of track, although threat can increase if approaching.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityKillAtT0, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPk_t0);
}

/** The probability of threat (score) at the time of track, although threat can increase if approaching.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityThreatAtT0, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPt_t0);
}

/** The relative time (sec) from the time of track when the maximum probability of kill occurs.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, TimeOfProbabilityThreatMax, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mTimePtMax);
}

/** The maximum probability of kill for this weapon based on current geometry.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityKillMax, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPk_max);
}

/** The maximum probability of threat (score) for the weapon against ownship.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityThreatMax, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPt_max);
}

/** The probability of ownship detection.  This value should be the same for all reports for this ownship.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityOfDetection, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPoD);
}

/** The probability this weapon type is on board.  Just echos the data inputted.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptThreatReportClass, WsfThreatReport, ProbabilityWeaponType, 0, "double", "")
{
   WsfThreatReport* report = aObjectPtr;
   aReturnVal.SetDouble(report->mPw);
}
