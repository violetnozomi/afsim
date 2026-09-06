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
*   File: WsfScriptWeaponsManagerClass.cpp
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
*   Abstract: This is an abstract script base class for all weapons manager processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#include <WsfScriptWeaponsManagerClass.hpp>

namespace il
{
   class AssetManagerInterface;
   class BattleManagerInterface;
}

#include "UtMemory.hpp"
#include <UtPluginManager.hpp>

#include <WsfBMWeaponsManager.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include <WsfBattleManager.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/weaponsManagerInterface.hpp>

// Messages
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>

#include <script/WsfScriptDefs.hpp>

const char* WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME = "WsfWeaponsManager";
const char* WsfScriptWeaponsManagerClass::BASE_CLASS_NAME = "WSF_WEAPONS_MANAGER";

WsfScriptWeaponsManagerClass::WsfScriptWeaponsManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<IsWEZLibrarySpecified>());
   AddMethod(ut::make_unique<GetWEZLibraryName>());
   AddMethod(ut::make_unique<IsWEZTacticalRangeAero>());
   AddMethod(ut::make_unique<IsWEZTacticalRangeMax1>());
   AddMethod(ut::make_unique<IsWEZTacticalRangeMax2>());
   AddMethod(ut::make_unique<GetEngageEWTargets>());
   AddMethod(ut::make_unique<GetEngageTARTargets>());
   AddMethod(ut::make_unique<GetEngageTTRTargets>());
}

void* WsfScriptWeaponsManagerClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::WeaponsManagerInterface> wm = std::shared_ptr<il::WeaponsManagerInterface>(new il::WeaponsManagerInterface);
   return reinterpret_cast<void*>(new WsfBMWeaponsManager(*WsfScriptContext::GetSCENARIO(aContext),
                                                          WsfScriptWeaponsManagerClass::SCRIPT_CLASS_NAME,
                                                          "PROCESSOR",
                                                          wm));
}

void* WsfScriptWeaponsManagerClass::Clone(void* aObjectPtr)
{
   WsfBMWeaponsManager* objPtr = reinterpret_cast<WsfBMWeaponsManager*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptWeaponsManagerClass::Destroy(void* aObjectPtr)
{
   WsfBMWeaponsManager* objPtr = reinterpret_cast<WsfBMWeaponsManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, IsWEZLibrarySpecified, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getUseWEZCalculator());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, GetWEZLibraryName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCoreWMRef()->getWEZLibraryName());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, IsWEZTacticalRangeAero, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getWEZTacticalRange() == il::WeaponsManagerInterface::E_AERO);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, IsWEZTacticalRangeMax1, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getWEZTacticalRange() == il::WeaponsManagerInterface::E_MAX1);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, IsWEZTacticalRangeMax2, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getWEZTacticalRange() == il::WeaponsManagerInterface::E_MAX2);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, GetEngageEWTargets, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getEngageEWTargets());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, GetEngageTARTargets, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getEngageTARTargets());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerClass, WsfBMWeaponsManager, GetEngageTTRTargets, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetCoreWMRef()->getEngageTTRTargets());
}
