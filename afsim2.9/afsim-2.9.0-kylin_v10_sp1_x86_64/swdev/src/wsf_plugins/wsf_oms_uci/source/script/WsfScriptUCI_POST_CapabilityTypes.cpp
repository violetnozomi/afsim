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

#include "WsfScriptUCI_POST_CapabilityTypes.hpp"

#include "UtMemory.hpp"
#include "script/WsfScriptDefs.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Types.hpp"
#include "WsfUCI_Utils.hpp"

namespace wsf
{
namespace script
{

UCI_POST_CapabilityTypeClass::UCI_POST_CapabilityTypeClass(const std::string& aClassName,
                                                           UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CapabilityId>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_POST_CapabilityTypeClass, UCI_POST_CapabilityType, CapabilityId, 0, "UCI_CapabilityId", "")
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
