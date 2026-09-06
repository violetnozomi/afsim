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
*   File: WsfScriptSimpleSensorsManagerClass.cpp
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

#include "WsfScriptSimpleSensorsManagerClass.hpp"
#include <WsfSimpleSensorsManager.hpp>
#include <WsfSimpleSensorsManagerImpl.hpp>

const char* WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME = "WsfSimpleSensorsManager";
const char* WsfScriptSimpleSensorsManagerClass::BASE_CLASS_NAME = "WSF_SIMPLE_SENSORS_MANAGER";

WsfScriptSimpleSensorsManagerClass::WsfScriptSimpleSensorsManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptSensorsManagerClass(aClassName, aScriptTypesPtr)
{
   AddClassName(WsfScriptSimpleSensorsManagerClass::SCRIPT_CLASS_NAME);
}

void* WsfScriptSimpleSensorsManagerClass::Create(const UtScriptContext& aContext)
{
   auto ssm = std::shared_ptr<WsfSimpleSensorsManagerImpl>(new WsfSimpleSensorsManagerImpl);

   return reinterpret_cast<void*>(new WsfSimpleSensorsManager(*WsfScriptContext::GetSCENARIO(aContext), ssm));
}

void* WsfScriptSimpleSensorsManagerClass::Clone(void* aObjectPtr)
{
   WsfSimpleSensorsManager* objPtr = reinterpret_cast<WsfSimpleSensorsManager*>(aObjectPtr);
   return objPtr->Clone();
}

void  WsfScriptSimpleSensorsManagerClass::Destroy(void* aObjectPtr)
{
   WsfSimpleSensorsManager* objPtr = reinterpret_cast<WsfSimpleSensorsManager*>(aObjectPtr);
   delete objPtr;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptSimpleSensorsManagerClass::GetScriptClassName()
{
   return SCRIPT_CLASS_NAME;
}

extern "C" WSF_IADS_C2_LIB_EXPORT const char* WsfScriptSimpleSensorsManagerClass::GetBaseClassName()
{
   return BASE_CLASS_NAME;
}
