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

#include "WsfScriptUCI_Message.hpp"

#include "UtMemory.hpp"
#include "script/WsfScriptDefs.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_MessageBaseType.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_MessageClass::UCI_MessageClass(const std::string& aClassName,
                                   UtScriptTypes*     aTypesPtr)
   : WsfScriptMessageClass(aClassName, aTypesPtr)
{
   SetClassName("UCI_Message");

   AddMethod(ut::make_unique<Header>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_MessageClass, UCI_MessageBaseType, Header, 0, "UCI_MessageHeader", "")
{
   auto interfacePtr = UCI_Util::GetInterface(SIMULATION);
   if (interfacePtr != nullptr)
   {
      auto rawHeaderPtr = aObjectPtr->GetHeader();
      auto headerPtr = new UCI_HeaderType(*interfacePtr, *rawHeaderPtr);
      aReturnVal.SetPointer(new UtScriptRef(headerPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

} // script
} // wsf
