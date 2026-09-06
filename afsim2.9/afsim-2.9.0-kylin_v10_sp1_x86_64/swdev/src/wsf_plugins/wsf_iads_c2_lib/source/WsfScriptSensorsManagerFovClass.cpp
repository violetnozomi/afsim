// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <memory>

#include "UtScriptClass.hpp"
#include "script/WsfScriptContext.hpp"
#include "WsfScriptSensorsManagerFovClass.hpp"
#include "WsfSensorsManagerFov.hpp"
#include "WsfSensorsManagerFovImpl.hpp"

const char* WsfScriptSensorsManagerFovClass::GetScriptClassName()
{
   return "WsfSensorsManagerFov";
}

const char* WsfScriptSensorsManagerFovClass::GetBaseClassName()
{
   return "WSF_SENSORS_MANAGER_FOV";
}

WsfScriptSensorsManagerFovClass::WsfScriptSensorsManagerFovClass(UtScriptTypes* scriptTypes)
   : WsfScriptProcessorClass(WsfScriptSensorsManagerFovClass::GetScriptClassName(), scriptTypes)
{
   mLessThanComparable = false;
   mEqualityComparable = false;
   AddClassName(WsfScriptSensorsManagerFovClass::GetScriptClassName());
}

void* WsfScriptSensorsManagerFovClass::Create(const UtScriptContext& context)
{
   return new WsfSensorsManagerFov(*WsfScriptContext::GetSCENARIO(context),
                                   std::make_shared<WsfSensorsManagerFovImpl>());
}

void* WsfScriptSensorsManagerFovClass::Clone(void* obj)
{
   return ((WsfSensorsManagerFov*)obj)->Clone();
}

void WsfScriptSensorsManagerFovClass::Destroy(void* obj)
{
   delete ((WsfSensorsManagerFov*)obj);
}
