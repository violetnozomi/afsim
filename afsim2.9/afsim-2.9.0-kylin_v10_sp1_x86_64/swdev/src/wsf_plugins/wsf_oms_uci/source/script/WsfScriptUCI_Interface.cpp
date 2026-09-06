// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_Interface.hpp"

#include "UtMemory.hpp"
#include "script/WsfScriptDefs.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_SensorComponent.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_InterfaceClass::UCI_InterfaceClass(const std::string& aClassName,
                                       UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<GetSensorFromCapabilityID>());
   AddStaticMethod(ut::make_unique<GenerateRandomUUID>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_InterfaceClass, UCI_InterfaceClass, GetSensorFromCapabilityID, 1, "WsfSensor", "string")
{
   bool set{ false };
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityUUID = aVarArgs[0].GetString();
      auto componentPtr = interfacePtr->GetUCI_Component(capabilityUUID);
      auto sensorComponentPtr = dynamic_cast<UCI_SensorComponent*>(componentPtr);
      if (sensorComponentPtr != nullptr)
      {
         set = true;
         aReturnVal.SetPointer(new UtScriptRef(sensorComponentPtr->GetSensor(), aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_InterfaceClass, UCI_InterfaceClass, GenerateRandomUUID, 0, "string", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      aReturnVal.SetString(interfacePtr->GetConnection()->generateUUID().getValueAsString());
   }
   else
   {
      aReturnVal.SetString(std::string());
   }
}

} // script
} // wsf
