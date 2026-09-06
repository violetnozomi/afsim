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

#include "WsfScriptUCI_AMTI_CapabilityStatusMessage.hpp"

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfScriptUCI_Message.hpp"
#include "WsfScriptUCI_CommonTypes.hpp"
#include "script/WsfScriptDefs.hpp"
#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_AMTI_CapabilityStatusMessageClass::UCI_AMTI_CapabilityStatusMessageClass(const std::string& aClassName,
                                                                             UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CapabilityStatus>());
   AddMethod(ut::make_unique<Size>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityStatusMessageClass, UCI_AMTI_CapabilityStatusMessage, CapabilityStatus, 1, "UCI_CapabilityStatus", "int")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   bool set{ false };
   if (interfacePtr != nullptr)
   {
      auto index = static_cast<size_t>(aVarArgs[0].GetInt());
      if (!aObjectPtr->Get().getMessageData().getCapabilityStatus().empty() &&
          index < aObjectPtr->Get().getMessageData().getCapabilityStatus().size())
      {
         set = true;
         auto capabilityStatusTypePtr = new UCI_CapabilityStatusType(*interfacePtr,
                                                                     aObjectPtr->Get().getMessageData().getCapabilityStatus().at(index));
         aReturnVal.SetPointer(new UtScriptRef(capabilityStatusTypePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
      }
   }
   if (!set)
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityStatusMessageClass, UCI_AMTI_CapabilityStatusMessage, Size, 0, "int", "")
{
   if (UCI_Util::GetInterface(SIMULATION) != nullptr)
   {
      aReturnVal.SetInt(ut::cast_to_int(aObjectPtr->Get().getMessageData().getCapabilityStatus().size()));
   }
   else
   {
      aReturnVal.SetInt(0);
   }
}

} // script
} // wsf
