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
*   File: WsfScriptBattleManagerClass.cpp
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
*   Abstract: This is an abstract script base class for all battle manager
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#include <vector>

#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/assetManagerInterface.hpp>

#include "UtMemory.hpp"
#include <UtPluginManager.hpp>

#include "WsfScriptBattleManagerClass.hpp"
#include "WsfBattleManager.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include "WsfBMTrackMessage.hpp"
#include "WsfBMStatusMessage.hpp"
#include "WsfBMAssignmentStatusMessage.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include "WsfBMEvalAssignment.hpp"
#include "WsfBMUtils.hpp"
#include <script/WsfScriptManager.hpp>
#include "WsfBMMOELogger.hpp"

#include <WsfTrack.hpp>

const char* WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME = "WsfBattleManager";
const char* WsfScriptBattleManagerClass::BASE_CLASS_NAME = "WSF_BATTLE_MANAGER";

WsfScriptBattleManagerClass::WsfScriptBattleManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptBattleManagerClass::SCRIPT_CLASS_NAME);
   //AddClassName (WsfScriptBattleManagerClass::BASE_CLASS_NAME); // TODO: ?

   AddMethod(ut::make_unique<RunModel>());
   AddMethod(ut::make_unique<HasCommitAuthority>());
}

void* WsfScriptBattleManagerClass::Create(const UtScriptContext& aContext)
{
   return reinterpret_cast<void*>(UninitializedWsfBattleManager(*WsfScriptContext::GetSCENARIO(aContext), "WsfScriptBattleManagerClass::Create"));
}

void* WsfScriptBattleManagerClass::Clone(void* aObjectPtr)
{
   WsfBattleManager* objPtr = reinterpret_cast<WsfBattleManager*>(aObjectPtr);

   return objPtr->Clone();
}

void  WsfScriptBattleManagerClass::Destroy(void* aObjectPtr)
{
   WsfBattleManager* objPtr = reinterpret_cast<WsfBattleManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptBattleManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptBattleManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBattleManagerClass, WsfBattleManager, RunModel, 0, "void", "")
{
   aObjectPtr->GetCoreBMRef()->run(*IADSC2SimulationExtension::Find(*aObjectPtr->GetSimulation())->GetLogger());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBattleManagerClass, WsfBattleManager, HasCommitAuthority, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreBMRef()->getCommitAuthority());
}
