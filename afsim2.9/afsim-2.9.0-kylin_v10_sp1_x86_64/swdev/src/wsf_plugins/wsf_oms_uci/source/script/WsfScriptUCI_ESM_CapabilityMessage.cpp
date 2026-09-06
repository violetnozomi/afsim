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

#include "WsfScriptUCI_ESM_CapabilityMessage.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_ESM_CapabilityMessageClass::UCI_ESM_CapabilityMessageClass(const std::string& aClassName,
                                                               UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Capability>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CapabilityMessageClass, UCI_ESM_CapabilityMessage, Capability, 1, "UCI_ESM_Capability", "int")
{
   bool set{ false };
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      auto& capability = aObjectPtr->Get().getMessageData().getCapability();
      if (!capability.empty() &&
          index < capability.size())
      {
         set = true;
         auto esmCapabilityTypePtr = new UCI_ESM_CapabilityType(*interfacePtr,
                                                                capability.at(index));
         aReturnVal.SetPointer(new UtScriptRef(esmCapabilityTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_ESM_CapabilityMessageClass, UCI_ESM_CapabilityMessage, Size, 0, "int", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->Get().getMessageData().getCapability().size()));
   }
   else
   {
      aReturnVal.SetInt(0);
   }
}

} // script
} // wsf
