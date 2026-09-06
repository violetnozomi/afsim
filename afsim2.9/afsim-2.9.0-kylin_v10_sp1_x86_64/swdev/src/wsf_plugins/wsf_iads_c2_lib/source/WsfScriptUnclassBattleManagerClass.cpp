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
*   File: WsfScriptUnclassBattleManagerClass.cpp
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

// We must forward declare here as well because we have a circular inheritance hierarchy
namespace il
{
   class unclassifiedAssetManager;
   class unclassifiedBattleManager;
}

#include <vector>
#include "WsfScriptUnclassBattleManagerClass.hpp"
#include "WsfUnclassBattleManager.hpp"
#include "WsfBMAssignmentMessage.hpp"
#include "WsfBMTrackMessage.hpp"
#include "WsfBMStatusMessage.hpp"

#include <WsfTrack.hpp>

#include <iadsLib/unclassifiedBattleManager.hpp>
#include <iadsLib/unclassifiedAssetManager.hpp>

WsfScriptUnclassBattleManagerClass::WsfScriptUnclassBattleManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptBattleManagerClass(aClassName, aScriptTypesPtr)
{
   AddClassName(UNCLASS_BM_SCRIPT_CLASS_NAME);
}

void* WsfScriptUnclassBattleManagerClass::Create(const UtScriptContext& aContext)
{
   return reinterpret_cast<void*>(UninitializedWsfUnclassifiedBattleManager(*WsfScriptContext::GetSCENARIO(aContext), "WsfScriptUnclassBattleManagerClass::Create():"));
}

void* WsfScriptUnclassBattleManagerClass::Clone(void* aObjectPtr)
{
   WsfUnclassifiedBattleManager* objPtr = reinterpret_cast<WsfUnclassifiedBattleManager*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptUnclassBattleManagerClass::Destroy(void* aObjectPtr)
{
   WsfUnclassifiedBattleManager* objPtr = reinterpret_cast<WsfUnclassifiedBattleManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassBattleManagerClass::GetScriptClassName()
{
   return UNCLASS_BM_SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassBattleManagerClass::GetBaseClassName()
{
   return UNCLASS_BM_BASE_CLASS_NAME;
}
