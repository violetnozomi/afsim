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
*   File: WsfScriptWeaponsManagerAIClass.cpp
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

#include <WsfScriptWeaponsManagerAIClass.hpp>
#include <iadsLib/weaponsManagerAI.hpp>
#include <WsfWeaponsManagerAI.hpp>

#include "UtMemory.hpp"

const char* WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME = "WsfWeaponsManagerAI";
const char* WsfScriptWeaponsManagerAIClass::BASE_CLASS_NAME = "WSF_WEAPONS_MANAGER_AI";

WsfScriptWeaponsManagerAIClass::WsfScriptWeaponsManagerAIClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptWeaponsManagerClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptWeaponsManagerAIClass::SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<SetIsLowOnFuel>());
}

void* WsfScriptWeaponsManagerAIClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::weaponsManagerAI> wm = std::shared_ptr<il::weaponsManagerAI>(new il::weaponsManagerAI);
   return reinterpret_cast<void*>(new WsfWeaponsManagerAI(*WsfScriptContext::GetSCENARIO(aContext), wm));
}

void* WsfScriptWeaponsManagerAIClass::Clone(void* aObjectPtr)
{
   WsfWeaponsManagerAI* objPtr = reinterpret_cast<WsfWeaponsManagerAI*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptWeaponsManagerAIClass::Destroy(void* aObjectPtr)
{
   WsfWeaponsManagerAI* objPtr = reinterpret_cast<WsfWeaponsManagerAI*>(aObjectPtr);

   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerAIClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerAIClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerAIClass, WsfWeaponsManagerAI, SetIsLowOnFuel, 1, "void", "bool")
{
   auto value = aVarArgs[0].GetBool();
   static_cast<il::weaponsManagerAI*>(aObjectPtr->GetCoreWMRef().get())->setIsLowOnFuel(value);
}
