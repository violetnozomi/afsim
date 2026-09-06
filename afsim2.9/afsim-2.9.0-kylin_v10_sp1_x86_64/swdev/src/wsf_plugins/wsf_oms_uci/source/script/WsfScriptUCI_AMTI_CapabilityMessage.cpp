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

#include "WsfScriptUCI_AMTI_CapabilityMessage.hpp"

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "script/WsfScriptDefs.hpp"
#include "WsfScriptUCI_Message.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_AMTI_CapabilityMessageClass::UCI_AMTI_CapabilityMessageClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Capability>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityMessageClass, UCI_AMTI_CapabilityMessage, Capability, 1, "UCI_AMTI_Capability", "int")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   bool set{ false };
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      auto& capability = aObjectPtr->Get().getMessageData().getCapability();
      if (!capability.empty() &&
          index < capability.size())
      {
         set = true;
         auto amtiCapabilityTypePtr = new UCI_AMTI_CapabilityType(*interfacePtr,
                                                                  capability.at(index));
         aReturnVal.SetPointer(new UtScriptRef(amtiCapabilityTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityMessageClass, UCI_AMTI_CapabilityMessage, Size, 0, "int", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetInt(static_cast<int>(aObjectPtr->Get().getMessageData().getCapability().size()));
   }
   else
   {
      aReturnVal.SetInt(0);
   }
}

} // script
} // wsf
