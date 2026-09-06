// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_StrikeCapabilityMessage.hpp"

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

UCI_StrikeCapabilityMessageClass::UCI_StrikeCapabilityMessageClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<Capability>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityMessageClass, UCI_StrikeCapabilityMessage, Capability, 1, "UCI_StrikeCapability", "int")
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
         auto strikeCapabilityTypePtr = new UCI_StrikeCapabilityType(*interfacePtr,
                                                                     capability.at(index));
         aReturnVal.SetPointer(new UtScriptRef(strikeCapabilityTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }   
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityMessageClass, UCI_StrikeCapabilityMessage, Size, 0, "int", "")
{
   aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->Get().getMessageData().getCapability().size()));
}

//====================================================
UCI_StrikeCapabilityTypeClass::UCI_StrikeCapabilityTypeClass(const std::string& aClassName,
                                                             UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CapabilityId>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_StrikeCapabilityTypeClass, UCI_StrikeCapabilityType, CapabilityId, 0, "UCI_CapabilityId", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto capabilityIdPtr = new UCI_CapabilityID_Type(*interfacePtr, aObjectPtr->Get().getCapabilityID());
      aReturnVal.SetPointer(new UtScriptRef(capabilityIdPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
