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

#include "WsfScriptUCI_AMTI_CapabilityTypes.hpp"

#include "script/WsfScriptDefs.hpp"
#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptTypes.hpp"
#include "WsfUCI_Interface.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_AMTI_CapabilityTypeClass::UCI_AMTI_CapabilityTypeClass(const std::string& aClassName,
                                                           UtScriptTypes*     aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   AddMethod(ut::make_unique<CapabilityId>());
}

UT_DEFINE_SCRIPT_METHOD(UCI_AMTI_CapabilityTypeClass, UCI_AMTI_CapabilityType, CapabilityId, 0, "UCI_CapabilityId", "")
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
