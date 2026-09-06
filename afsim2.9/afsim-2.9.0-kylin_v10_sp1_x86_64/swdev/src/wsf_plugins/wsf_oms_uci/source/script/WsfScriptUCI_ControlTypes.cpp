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

#include "WsfScriptUCI_ControlTypes.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_ControlTypeClass::UCI_ControlTypeClass(const std::string& aClassName,
                                           UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<MISSION>());
   AddStaticMethod(ut::make_unique<CAPABILITY_PRIMARY>());
   AddStaticMethod(ut::make_unique<CAPABILITY_SECONDARY>());
   AddStaticMethod(ut::make_unique<CAPABILITY_MANAGER>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlTypeClass, UCI_ControlType, MISSION, 0, "UCI_Control", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto controlTypePtr = new UCI_ControlType(SIMULATION);
      controlTypePtr->Get().setValue(uci::type::ControlTypeEnum::UCI_MISSION);
      aReturnVal.SetPointer(new UtScriptRef(controlTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlTypeClass, UCI_ControlType, CAPABILITY_PRIMARY, 0, "UCI_Control", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto controlTypePtr = new UCI_ControlType(SIMULATION);
      controlTypePtr->Get().setValue(uci::type::ControlTypeEnum::UCI_CAPABILITY_PRIMARY);
      aReturnVal.SetPointer(new UtScriptRef(controlTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlTypeClass, UCI_ControlType, CAPABILITY_SECONDARY, 0, "UCI_Control", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto controlTypePtr = new UCI_ControlType(SIMULATION);
      controlTypePtr->Get().setValue(uci::type::ControlTypeEnum::UCI_CAPABILITY_SECONDARY);
      aReturnVal.SetPointer(new UtScriptRef(controlTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlTypeClass, UCI_ControlType, CAPABILITY_MANAGER, 0, "UCI_Control", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto controlTypePtr = new UCI_ControlType(SIMULATION);
      controlTypePtr->Get().setValue(uci::type::ControlTypeEnum::UCI_CAPABILITY_MANAGER);
      aReturnVal.SetPointer(new UtScriptRef(controlTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

wsf::script::UCI_ControlRequestTypeClass::UCI_ControlRequestTypeClass(const std::string& aClassName,
                                                                      UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName(aClassName);

   AddStaticMethod(ut::make_unique<ACQUIRE>());
   AddStaticMethod(ut::make_unique<RELEASE>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestTypeClass, UCI_ControlRequestTypeClass, ACQUIRE, 0, "UCI_ControlRequest", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto requestTypePtr = new UCI_ControlRequestType(SIMULATION);
      requestTypePtr->Get().setValue(uci::type::ControlRequestEnum::UCI_ACQUIRE);
      aReturnVal.SetPointer(new UtScriptRef(requestTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ControlRequestTypeClass, UCI_ControlRequestTypeClass, RELEASE, 0, "UCI_ControlRequest", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      auto requestTypePtr = new UCI_ControlRequestType(SIMULATION);
      requestTypePtr->Get().setValue(uci::type::ControlRequestEnum::UCI_RELEASE);
      aReturnVal.SetPointer(new UtScriptRef(requestTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
