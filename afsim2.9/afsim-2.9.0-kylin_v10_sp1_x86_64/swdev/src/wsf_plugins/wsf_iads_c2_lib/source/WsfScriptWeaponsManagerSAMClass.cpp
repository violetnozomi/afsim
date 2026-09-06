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
*   File: WsfScriptWeaponsManagerSAMClass.cpp
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

#include <WsfScriptWeaponsManagerSAMClass.hpp>
#include <iadsLib/weaponsManagerSAM.hpp>
#include <WsfWeaponsManagerSAM.hpp>
#include "UtMemory.hpp"

const char* WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME = "WsfWeaponsManagerSAM";
const char* WsfScriptWeaponsManagerSAMClass::BASE_CLASS_NAME = "WSF_WEAPONS_MANAGER_SAM";

WsfScriptWeaponsManagerSAMClass::WsfScriptWeaponsManagerSAMClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptWeaponsManagerClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptWeaponsManagerSAMClass::SCRIPT_CLASS_NAME);

   AddMethod(ut::make_unique<SystemicCANTCOAssignmentsForWeapon>());
}

void* WsfScriptWeaponsManagerSAMClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::weaponsManagerSAM> wm = std::shared_ptr<il::weaponsManagerSAM>(new il::weaponsManagerSAM);

   return reinterpret_cast<void*>(new WsfWeaponsManagerSAM(*WsfScriptContext::GetSCENARIO(aContext), wm));
}

void* WsfScriptWeaponsManagerSAMClass::Clone(void* aObjectPtr)
{
   WsfWeaponsManagerSAM* objPtr = reinterpret_cast<WsfWeaponsManagerSAM*>(aObjectPtr);

   return objPtr->Clone();
}

void  WsfScriptWeaponsManagerSAMClass::Destroy(void* aObjectPtr)
{
   WsfWeaponsManagerSAM* objPtr = reinterpret_cast<WsfWeaponsManagerSAM*>(aObjectPtr);

   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerSAMClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptWeaponsManagerSAMClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWeaponsManagerSAMClass, WsfWeaponsManagerSAM, SystemicCANTCOAssignmentsForWeapon, 1, "void", "string")
{
   WsfWeaponsManagerSAM* objPtr = reinterpret_cast<WsfWeaponsManagerSAM*>(aObjectPtr);
   objPtr->SystemicCANTCOAssignmentsForWeapon(aVarArgs[0].GetString());
}
