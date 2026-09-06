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

#include "WsfScriptUCI_POST_CapabilityMessage.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_POST_CapabilityMessageClass::UCI_POST_CapabilityMessageClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Capability>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CapabilityMessageClass, UCI_POST_CapabilityMessage, Capability, 1, "UCI_POST_Capability", "int")
{
   bool set{ false };
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      if (!aObjectPtr->Get().getMessageData().getCapability().empty() &&
          index < aObjectPtr->Get().getMessageData().getCapability().size())
      {
         set = true;
         auto postCapabilityTypePtr = new UCI_POST_CapabilityType(*interfacePtr,
                                                                  aObjectPtr->Get().getMessageData().getCapability().at(index));
         aReturnVal.SetPointer(new UtScriptRef(postCapabilityTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CapabilityMessageClass, UCI_POST_CapabilityMessage, Size, 0, "int", "")
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
