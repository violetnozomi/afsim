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

#include "WsfScriptWeaponThreatProcessorClass.hpp"

#include "UtMemory.hpp"
#include "WsfScriptThreatReportClass.hpp"
#include "WsfWeaponThreatProcessor.hpp"
#include "script/WsfScriptContext.hpp"

using WsfThreatReport = WsfWeaponThreatProcessor::WsfThreatReport;

// WsfScriptWeaponThreatProcessorClass* WsfScriptWeaponThreatProcessorClass::sClassPtr = 0;
//
//// static
// UtScriptRef* WsfScriptWeaponThreatProcessorClass::Create(const WsfWeaponThreatProcessor& aObjectPtr)
//{
//    return new UtScriptRef(new WsfWeaponThreatProcessor(aObjectPtr), sClassPtr, UtScriptRef::cMANAGE);
// }

WsfScriptWeaponThreatProcessorClass::WsfScriptWeaponThreatProcessorClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aScriptTypesPtr)
   //: UtScriptClass(aClassName, aTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfWeaponThreatProcessor");

   AddMethod(ut::make_unique<QueryDurationSec>());
   AddMethod(ut::make_unique<RunQuerySortProbabilityKillAtT0>());
   AddMethod(ut::make_unique<RunQuerySortProbabilityThreatAtT0>());
   AddMethod(ut::make_unique<RunQuerySortProbabilityKillMax>());
   AddMethod(ut::make_unique<RunQuerySortProbabilityThreatMax>());
   // AddMethod(ut::make_unique<QueryTimeSec>());
   // AddMethod(ut::make_unique<QueryIgnoreArcDeg>());
   // AddMethod(ut::make_unique<RunQuerySort_Pk_t0>());
   AddMethod(ut::make_unique<ReportCount>());
   AddMethod(ut::make_unique<ThreatReport>());
}

void* WsfScriptWeaponThreatProcessorClass::Create(const UtScriptContext& aInstance)
{
   // WsfWeaponThreatProcessor* evaluatorectPtr = new WsfWeaponThreatProcessor();
   // return evaluatorectPtr;
   WsfScenario* sPtr = WsfScriptContext::GetSCENARIO(aInstance);
   return static_cast<void*>(new WsfWeaponThreatProcessor(*sPtr));
}
//--------------------
////virtual
// void* WsfScriptQuantumTaskerProcessor::Create(const UtScriptContext& /*aContext*/)
//{
//    return static_cast<void*>(new WsfQuantumTaskerProcessor());
// }

// virtual
void* WsfScriptWeaponThreatProcessorClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfWeaponThreatProcessor*>(aObjectPtr)->Clone();
}
////virtual
// void* WsfScriptQuantumTaskerProcessor::Clone(void* aObjectPtr)
//{
//    WsfQuantumTaskerProcessor* evaluatorPtr = static_cast<WsfQuantumTaskerProcessor*>(aObjectPtr);
//    assert(evaluatorPtr != 0);
//    return evaluatorPtr->Clone();
// }

// virtual
void WsfScriptWeaponThreatProcessorClass::Destroy(void* aObjectPtr)
{
   WsfWeaponThreatProcessor* aObjectPtrCast = static_cast<WsfWeaponThreatProcessor*>(aObjectPtr);
   delete aObjectPtrCast;
}

UtScriptContext* WsfScriptWeaponThreatProcessorClass::GetContext(void* aObjectPtr)
{
   UtScriptContext*          contextPtr   = nullptr;
   WsfWeaponThreatProcessor* processorPtr = (WsfWeaponThreatProcessor*)aObjectPtr;
   // dynamic_cast<WsfWeaponThreatProcessor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

//------------------------ Script Functions ------------------------

/** Sets the duration window the search should through.  A longer window, can mean
 * longer execution times, and means the extrapolation is more uncertain.
 */
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass, WsfWeaponThreatProcessor, QueryDurationSec, 1, "void", "double")
{
   // WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   // double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double                    args0     = aVarArgs[0].GetDouble();
   WsfWeaponThreatProcessor* evaluator = aObjectPtr;
   evaluator->SetQueryTime(args0);
}

/** Sets the fan behind the shooter that can/will be ignored. If not set, none will be
 * used and it may assume an instantaneous heading change.
 */
/*
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass, WsfWeaponThreatProcessor, QueryIgnoreArcDeg, 1, "void", "double")
{
   WsfPlatform* platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   double simTime = WsfScriptContext::GetTIME_NOW(aContext);
   double args0 = aVarArgs[0].GetDouble();
   cout << "ScriptWeaponThreatProcessorClass: Got query arc = " << args0 << endl;
   WsfWeaponThreatProcessor* evaluator = aObjectPtr;
   evaluator->SetQueryArcDeg(args0);
}
*/

/** Runs the query.  The reports will be ordered by: highest pk (not pt) at t0. It
returns the number of threat reports: = \#query_tracks*\#entity_types*\#weapons_types - filtered_out
*/
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass,
                        WsfWeaponThreatProcessor,
                        RunQuerySortProbabilityKillAtT0,
                        0,
                        "int",
                        "")
{
   WsfPlatform*              platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfWeaponThreatProcessor* evaluator   = aObjectPtr;
   evaluator->SetQuerySort(evaluator->QUERY_SORT_PK_T0);
   evaluator->CreateThreatList(*platformPtr);
   aReturnVal.SetInt((int)evaluator->GetThreatList().size());
}

/** Runs the query.  The reports will be ordered by: highest pt (not pk) at t0. It
returns the number of threat reports: = \#query_tracks*\#entity_types*\#weapons_types - filtered_out
*/
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass,
                        WsfWeaponThreatProcessor,
                        RunQuerySortProbabilityThreatAtT0,
                        0,
                        "int",
                        "")
{
   WsfPlatform*              platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfWeaponThreatProcessor* evaluator   = aObjectPtr;
   evaluator->SetQuerySort(evaluator->QUERY_SORT_PT_T0);
   evaluator->CreateThreatList(*platformPtr);
   aReturnVal.SetInt((int)evaluator->GetThreatList().size());
}

/** Runs the query.  The reports will be ordered by: highest pk (not pt). It
returns the number of threat reports: = \#query_tracks*\#entity_types*\#weapons_types - filtered_out
*/
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass, WsfWeaponThreatProcessor, RunQuerySortProbabilityKillMax, 0, "int", "")
{
   WsfPlatform*              platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfWeaponThreatProcessor* evaluator   = aObjectPtr;
   evaluator->SetQuerySort(evaluator->QUERY_SORT_PK_MAX);
   evaluator->CreateThreatList(*platformPtr);
   aReturnVal.SetInt((int)evaluator->GetThreatList().size());
}

/** Runs the query.  The reports will be ordered by: highest pt (not pk). It
returns the number of threat reports: = \#query_tracks*\#entity_types*\#weapons_types - filtered_out
*/
UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass,
                        WsfWeaponThreatProcessor,
                        RunQuerySortProbabilityThreatMax,
                        0,
                        "int",
                        "")
{
   WsfPlatform*              platformPtr = WsfScriptContext::GetPLATFORM(aContext);
   WsfWeaponThreatProcessor* evaluator   = aObjectPtr;
   evaluator->SetQuerySort(evaluator->QUERY_SORT_PT_MAX);
   evaluator->CreateThreatList(*platformPtr);
   aReturnVal.SetInt((int)evaluator->GetThreatList().size());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass, WsfWeaponThreatProcessor, ReportCount, 0, "int", "")
{
   WsfWeaponThreatProcessor* evaluator = aObjectPtr;
   aReturnVal.SetInt((int)evaluator->GetThreatList().size());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponThreatProcessorClass,
                        WsfWeaponThreatProcessor,
                        ThreatReport,
                        1,
                        WsfScriptThreatReportClass::GetScriptClassname(),
                        "int")
{
   int                       i         = aVarArgs[0].GetInt();
   WsfWeaponThreatProcessor* evaluator = aObjectPtr;
   WsfThreatReport*          value     = new WsfThreatReport(evaluator->GetThreatList()[i]);
   aReturnVal.SetPointer(new UtScriptRef(value, aReturnClassPtr, UtScriptRef::cMANAGE));
}
