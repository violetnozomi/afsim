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
*   File: WsfScriptUnclassAssetManagerClass.cpp
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
*   Abstract:.
*
*******************************************************************************/

// We must forward declare here as well because we have a circular inheritance hierarchy
namespace il
{
   class BattleManagerInterface;
   class unclassifiedDisseminateC2;
}

#include "WsfScriptUnclassAssetManagerClass.hpp"

#include <iadsLib/assetManagerInterface.hpp>

#include "WsfIADSC2ScenarioExtension.hpp"
#include "WsfUnclassAssetManager.hpp"

const char* WsfScriptUnclassAssetManagerClass::SCRIPT_CLASS_NAME = "WsfUnclassAssetManager";
const char* WsfScriptUnclassAssetManagerClass::BASE_CLASS_NAME = "WSF_UNCLASS_ASSET_MANAGER";

WsfScriptUnclassAssetManagerClass::WsfScriptUnclassAssetManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptAssetManagerClass(aClassName, aScriptTypesPtr)
{
   AddClassName(SCRIPT_CLASS_NAME);
}

void* WsfScriptUnclassAssetManagerClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::AssetManagerInterface> am(new il::AssetManagerInterface(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext))));

#ifdef IC2DEBUG
   HCL_INFO_LOGGER() << "WsfScriptUnclassAssetManagerClass::Create(): am = " << am;
#endif

   return reinterpret_cast<void*>(new WsfUnclassifiedAssetManager(*WsfScriptContext::GetSCENARIO(aContext), am));
}

void* WsfScriptUnclassAssetManagerClass::Clone(void* aObjectPtr)
{
   WsfUnclassifiedAssetManager* objPtr = reinterpret_cast<WsfUnclassifiedAssetManager*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptUnclassAssetManagerClass::Destroy(void* aObjectPtr)
{
   WsfUnclassifiedAssetManager* objPtr = reinterpret_cast<WsfUnclassifiedAssetManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassAssetManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptUnclassAssetManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}
