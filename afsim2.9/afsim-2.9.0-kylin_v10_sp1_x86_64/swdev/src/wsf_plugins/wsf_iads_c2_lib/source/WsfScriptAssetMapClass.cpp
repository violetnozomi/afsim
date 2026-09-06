// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "WsfScriptAssetMapClass.hpp"

#include <logger.hpp>

#include "script/WsfScriptContext.hpp"

#include "WsfAssetMap.hpp"
#include "WsfBMAssetRecord.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Script Class /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WsfScriptAssetMapClass::WsfScriptAssetMapClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBMAssetMap");

   mConstructible = true;
   mCloneable = true;

   AddMethod(ut::make_unique<Add>());
   AddMethod(ut::make_unique<Exists>());
   AddMethod(ut::make_unique<Get>());
}

WsfScriptAssetMapClass::~WsfScriptAssetMapClass()
{
}

void* WsfScriptAssetMapClass::Create(const UtScriptContext& aContext)
{
   WsfAssetMap* map_ptr = new WsfAssetMap(IADSC2ScenarioExtension::GetGlobalLogger(*WsfScriptContext::GetSCENARIO(aContext)));
   return map_ptr;
}

void* WsfScriptAssetMapClass::Clone(void* aObjectPtr)
{
   return reinterpret_cast<WsfAssetMap*>(aObjectPtr)->Clone();
}

void  WsfScriptAssetMapClass::Destroy(void* aObjectPtr)
{
   WsfAssetMap* objPtr = reinterpret_cast<WsfAssetMap*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetMapClass, WsfAssetMap, Add, 1, "void", "WsfBMAssetRecord")
{
   WsfBMAssetRecord* asset_record = reinterpret_cast<WsfBMAssetRecord*>(aVarArgs[0].GetPointer()->GetAppObject());

   HCL_TRACE_LOGGER(asset_record->GetRawRecord()->getGlobalLogger()) << "In Asset Map Add..." << std::endl;

   aObjectPtr->Add(asset_record);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetMapClass, WsfAssetMap, Exists, 1, "bool", "int")
{
   aReturnVal.SetBool(aObjectPtr->Exists(aVarArgs[0].GetInt()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAssetMapClass, WsfAssetMap, Get, 1, "WsfBMAssetRecord", "int")
{
   if (aObjectPtr->Exists(aVarArgs[0].GetInt()))
   {
      WsfBMAssetRecord* asset_record = new WsfBMAssetRecord(aObjectPtr->GetGlobalLogger());
      aObjectPtr->GetAsset(aVarArgs[0].GetInt(), asset_record);
      aReturnVal.SetPointer(new UtScriptRef(asset_record, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(0, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}
