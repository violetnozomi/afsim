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
*   File: WsfScriptSensorsManagerClass.cpp
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
*   Abstract: This is an abstract script base class for all sensors manager processor
*     script classes to inherit from. It provides necessary common functionality.
*     It's not pure abstract in the sense that we actually instantiate an object of this
*     type purely for type registration purposes with the sole intent of giving AFSIM
*     scripts the ability to up/down cast to and from the inherited types.
*
*******************************************************************************/

#include <WsfScriptSensorsManagerClass.hpp>

namespace il
{
   class AssetManagerInterface;
   class BattleManagerInterface;
}

#include <UtPluginManager.hpp>
#include <WsfBMSensorsManager.hpp>
#include <iadsLib/disseminateC2Interface.hpp>
#include <WsfBattleManager.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/sensorsManagerInterface.hpp>

// Messages
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMStatusMessage.hpp>
#include <WsfBMTrackMessage.hpp>

#include <script/WsfScriptDefs.hpp>

const char* WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME = "WsfSensorsManager";
const char* WsfScriptSensorsManagerClass::BASE_CLASS_NAME = "WSF_SENSORS_MANAGER";

WsfScriptSensorsManagerClass::WsfScriptSensorsManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME);
}

void* WsfScriptSensorsManagerClass::Create(const UtScriptContext& aContext)
{
   std::shared_ptr<il::SensorsManagerInterface> sm(new il::SensorsManagerInterface);

   return reinterpret_cast<void*>(new WsfBMSensorsManager(*WsfScriptContext::GetSCENARIO(aContext), WsfScriptSensorsManagerClass::SCRIPT_CLASS_NAME, "PROCESSOR", sm));
}

void* WsfScriptSensorsManagerClass::Clone(void* aObjectPtr)
{
   WsfBMSensorsManager* objPtr = reinterpret_cast<WsfBMSensorsManager*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptSensorsManagerClass::Destroy(void* aObjectPtr)
{
   WsfBMSensorsManager* objPtr = reinterpret_cast<WsfBMSensorsManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptSensorsManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptSensorsManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}
