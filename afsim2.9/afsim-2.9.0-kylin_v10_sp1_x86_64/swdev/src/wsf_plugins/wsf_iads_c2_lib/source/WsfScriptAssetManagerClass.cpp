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
*   File: WsfScriptAssetManagerClass.cpp
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
*   Abstract: This is an abstract script base class for all asset manager processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

namespace il
{
   class AssetManagerInterface;
   class BattleManagerInterface;
   class DisseminateC2Interface;
}

#include <UtPluginManager.hpp>
#include <script/WsfScriptDefs.hpp>

#include "WsfScriptAssetManagerClass.hpp"
#include <WsfAssetManager.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include <WsfBattleManager.hpp>
#include <WsfBMDisseminateC2.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <WsfBMUtils.hpp>

// Messages
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>

#include <UtMemory.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"

const char* WsfScriptAssetManagerClass::SCRIPT_CLASS_NAME = "WsfAssetManager";
const char* WsfScriptAssetManagerClass::BASE_CLASS_NAME = "WSF_ASSET_MANAGER";

WsfScriptAssetManagerClass::WsfScriptAssetManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName(SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<InitOwnNode>());
   AddMethod(ut::make_unique<GetAssetYellowTime>());
   AddMethod(ut::make_unique<GetAssetRedTime>());
   AddMethod(ut::make_unique<GetReportPositionEveryMeters>());
   AddMethod(ut::make_unique<GetReportPositionEverySeconds>());
   AddMethod(ut::make_unique<GetReportStatusEverySeconds>());
   AddMethod(ut::make_unique<GetAggregateUnitStatus>());
   AddMethod(ut::make_unique<GetStationaryOpnsOnly>());
   AddMethod(ut::make_unique<GetWeaponRequired>());
   AddMethod(ut::make_unique<GetEWRequired>());
   AddMethod(ut::make_unique<GetTARRequired>());
   AddMethod(ut::make_unique<GetTTRRequired>());

   AddMethod(ut::make_unique<GetAssetMap>());
   AddMethod(ut::make_unique<GetAsset>());
   AddMethod(ut::make_unique<PrintStatus>());
   AddMethod(ut::make_unique<SetStartTime>());
   AddMethod(ut::make_unique<ProgressTime>());
   AddMethod(ut::make_unique<CheckSubordinateTimeout>());
   AddMethod(ut::make_unique<ProcessTrackMessage>());
   AddMethod(ut::make_unique<ProcessStatusMessage>());
   AddMethod(ut::make_unique<ProcessAssignmentStatusMessage>());
   AddMethod(ut::make_unique<ProcessAssignmentMessage>());
   AddMethod(ut::make_unique<ClearProcessedMessages>());
   AddMethod(ut::make_unique<RunModel>());
   AddMethod(ut::make_unique<CreateAssignments>());
   AddMethod(ut::make_unique<CANTCOAssignment>());
   AddMethod(ut::make_unique<MonitorAssignments>());
   AddMethod(ut::make_unique<ConnectLocalTrackDropCallback>());
   AddMethod(ut::make_unique<ConnectLocalTrackInitiatedCallback>());
   AddMethod(ut::make_unique<ConnectLocalTrackUpdatedCallback>());
   AddMethod(ut::make_unique<UpdateOwnStatus>());
   AddMethod(ut::make_unique<AssessOngoingAssignments>());
   AddMethod(ut::make_unique<ReadyCompleteAssignments>());
   AddMethod(ut::make_unique<CalculateTrackStrengths>());
   AddMethod(ut::make_unique<PrepareTracksForEvaluation>());
   AddMethod(ut::make_unique<UpdateDefendedAssetsArray>());
   AddMethod(ut::make_unique<GetAssignment>());
}

void* WsfScriptAssetManagerClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::AssetManagerInterface> proc = std::shared_ptr<il::AssetManagerInterface>(new il::AssetManagerInterface(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))));
   return reinterpret_cast<void*>(new WsfAssetManager(*WsfScriptContext::GetSCENARIO(aContext), SCRIPT_CLASS_NAME, "PROCESSOR", proc));
}

void* WsfScriptAssetManagerClass::Clone(void* aObjectPtr)
{
   WsfAssetManager* objPtr = reinterpret_cast<WsfAssetManager*>(aObjectPtr);

   return objPtr->Clone();
}

void  WsfScriptAssetManagerClass::Destroy(void* aObjectPtr)
{
   WsfAssetManager* objPtr = reinterpret_cast<WsfAssetManager*>(aObjectPtr);

   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptAssetManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptAssetManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, InitOwnNode, 1, "void", "int")
{
   aObjectPtr->GetCoreAMRef()->setOwnID(aVarArgs[0].GetInt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAssetYellowTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCoreAMRef()->getAssetYellowAgeout());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAssetRedTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCoreAMRef()->getAssetRedAgeout());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetReportPositionEveryMeters, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCoreAMRef()->getReportPositionEveryMeters());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetReportPositionEverySeconds, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCoreAMRef()->getReportPositionEverySeconds());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetReportStatusEverySeconds, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCoreAMRef()->getReportStatusEverySeconds());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAggregateUnitStatus, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getAggregateUnitStatus());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetStationaryOpnsOnly, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getStationaryOpnsOnly());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetWeaponRequired, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getWeaponRequired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetRequireAllWeapons, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getRequireAllWeapons());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetEWRequired, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getEWRequired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetTARRequired, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getTARRequired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetTTRRequired, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->getTTRRequired());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAssetMap, 0, "WsfBMAssetMap", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssetMapPtr(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAsset, 0, "WsfBMAssetRecord", "")
{
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetAssetPtr(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, PrintStatus, 1, "void", "bool")
{
   HCL_INFO_LOGGER(aObjectPtr->GetCoreAMRef()->getGlobalLogger()) << "------------------------------------------------------------------------\n"
                     << "------------------------------------- Logging BM ("
                     << aObjectPtr->GetCoreAMRef()->getThisAsset()->getName()
                     << ") Asset status -------------------------------------";
   aObjectPtr->GetCoreAMRef()->logMyStatus();
   aObjectPtr->GetCoreAMRef()->logSubordinateStatus(aVarArgs[0].GetBool());
   HCL_INFO_LOGGER(aObjectPtr->GetCoreAMRef()->getGlobalLogger()) << "------------------------------------------------------------------------";
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, SetStartTime, 1, "void", "double")
{
   aObjectPtr->GetCoreAMRef()->setStartTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ProgressTime, 1, "void", "double")
{
   aObjectPtr->GetCoreAMRef()->updateTime(aVarArgs[0].GetDouble());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, CheckSubordinateTimeout, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->CheckSubordinateTimeout(aObjectPtr->GetCoreAMRef()->getCurrentTime());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ProcessTrackMessage, 1, "void", "WsfBMTrackMessage")
{
   WsfBMTrackMessage* message = (WsfBMTrackMessage*)aVarArgs[0].GetPointer()->GetAppObject();

   aObjectPtr->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::airTargetMessage>(message->GetRawMessagePtr()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ProcessStatusMessage, 1, "void", "WsfBMStatusMessage")
{
   WsfBMStatusMessage* message = (WsfBMStatusMessage*)aVarArgs[0].GetPointer()->GetAppObject();

   aObjectPtr->ProcessMessage(SIMULATION->GetSimTime(), *message);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ProcessAssignmentStatusMessage, 1, "void", "WsfBMAssignmentStatusMessage")
{
   WsfBMAssignmentStatusMessage* message = (WsfBMAssignmentStatusMessage*)aVarArgs[0].GetPointer()->GetAppObject();

   auto internal_ptr = std::const_pointer_cast<il::assignmentACKMessage>(message->GetRawMessagePtr());

   aObjectPtr->GetCoreAMRef()->processMessage(internal_ptr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ProcessAssignmentMessage, 1, "void", "WsfBMAssignmentMessage")
{
   WsfBMAssignmentMessage* message = (WsfBMAssignmentMessage*)aVarArgs[0].GetPointer()->GetAppObject();
   auto internal_ptr = std::const_pointer_cast<il::assignmentMessage>(message->GetRawMessagePtr());
   aObjectPtr->GetCoreAMRef()->processMessage(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger(), internal_ptr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ClearProcessedMessages, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->clearOutgoingMessages();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, RunModel, 0, "void", "")
{
   // nothing to do yet
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, CreateAssignments, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->CreateAssignments(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, CANTCOAssignment, 5, "void", "WsfBMAssignmentMessage, string, bool, bool, bool")
{
   // get paremeters
   auto assignment = reinterpret_cast<WsfBMAssignmentMessage*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto reason = aVarArgs[1].GetString();
   auto is_systemic = aVarArgs[3].GetBool();
   auto is_global = aVarArgs[4].GetBool();
   aObjectPtr->GetCoreAMRef()->cantcoAssignment(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger(), std::const_pointer_cast<il::assignmentMessage>(assignment->GetRawMessagePtr()), reason, is_systemic, is_global);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, MonitorAssignments, 0, "void", "")
{
   // invoke the asset manager's monitor assignments
   aObjectPtr->GetCoreAMRef()->MonitorAssignments(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger());
}

// You pass the name of a script function with the following signature: void (double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
// so you can get notified of track drops correctly. Note, the context of the script must be attached to the battle manager processor
UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ConnectLocalTrackDropCallback, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->ConnectTrackDropCallback(aVarArgs[0].GetString()));
}

// You pass the name of a script function with the following signature: void (double aSimTime, const WsfLocalTrack* aLocalTrackPtr, WsfTrack *aRawTrackPtr)
// so you can get notified of track updates/initiates correctly. Note, the context of the script must be attached to the asset manager processor
UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ConnectLocalTrackInitiatedCallback, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->ConnectTrackInitiatedCallback(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ConnectLocalTrackUpdatedCallback, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->ConnectTrackUpdatedCallback(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, UpdateOwnStatus, 4, "bool", "double, WsfGeoPoint, Vec3, bool")
{
   double sim_time = aVarArgs[0].GetDouble();
   WsfGeoPoint* pos = reinterpret_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   UtVec3d* vel = reinterpret_cast<UtVec3d*>(aVarArgs[2].GetPointer()->GetAppObject());
   bool is_green = aVarArgs[3].GetBool();

   double loc_xyz[3];
   pos->GetLocationWCS(loc_xyz);
   aReturnVal.SetBool(aObjectPtr->GetCoreAMRef()->updateOwnStatus(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger(), sim_time, loc_xyz, vel->GetData(), is_green ? il::STATUS_GREEN : il::STATUS_RED));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, AssessOngoingAssignments, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->AssessOngoingAssignments(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, ReadyCompleteAssignments, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->CompleteAssignments(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, CalculateTrackStrengths, 2, "void", "double, double")
{
   // get max grouping distance parameter
   double sim_time = aVarArgs[0].GetDouble();
   double max_grouping_distance_m = aVarArgs[1].GetDouble();
   aObjectPtr->GetCoreAMRef()->CalculateTrackStrengths(sim_time, max_grouping_distance_m);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, PrepareTracksForEvaluation, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->PrepareTracksForEvaluation();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, UpdateDefendedAssetsArray, 0, "void", "")
{
   aObjectPtr->GetCoreAMRef()->UpdateDefendedAssetsArray();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetManagerClass, WsfAssetManager, GetAssignment, 3, "WsfBMAssignmentMessage", "WsfTrackId, int, int")
{
   auto* track_id = reinterpret_cast<WsfTrackId*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto assigned_unit_id = aVarArgs[1].GetInt();
   auto assigned_weapon_id = aVarArgs[2].GetInt();

   // auto
   auto assignment = aObjectPtr->GetCoreAMRef()->getAssignment(WsfBMUtils::ConvertTrackId(*track_id), il::idRecord(assigned_unit_id, assigned_weapon_id));

   WsfBMAssignmentMessage* assign_ptr = nullptr;
   if (assignment)
   {
      assign_ptr = new WsfBMAssignmentMessage(WsfScriptContext::GetPLATFORM(aContext), assignment);
   }

   // caller must check for IsInvalid()
   aReturnVal.SetPointer(new UtScriptRef(assign_ptr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
