// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfDisScriptClass.hpp"

#include "UtMemory.hpp"
#include "WsfStringId.hpp"
#include "dis/WsfDisInterface.hpp"
#include "ext/WsfExtInterface.hpp"

WsfDisScriptClass* WsfDisScriptClass::mInstancePtr(nullptr);

WsfDisScriptClass::WsfDisScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mInstancePtr = this;
   SetClassName("WsfDIS");

   AddStaticMethod(ut::make_unique<EntityType_1>("EntityType"));
   AddStaticMethod(ut::make_unique<EntityType_2>("EntityType"));
}

#include "script/WsfScriptDefs.hpp"

#define DIS_INTERFACE WsfDisInterface::Find(*SIMULATION)

UT_DEFINE_SCRIPT_METHOD(WsfDisScriptClass, WsfDisInterface, EntityType_1, 1, "EntityType", "string")
{
   WsfExtEntityType* entityType     = new WsfExtEntityType(0, 0, 0, 0, 0, 0, 0);
   WsfStringId       objectTypeName = aVarArgs[0].GetString();
   WsfDisInterface*  interfacePtr   = DIS_INTERFACE;
   if (interfacePtr != nullptr)
   {
      interfacePtr->GetExtInterface()->GetEntityType(objectTypeName, *entityType);
   }
   aReturnVal.SetPointer(new UtScriptRef(entityType, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfDisScriptClass, WsfDisInterface, EntityType_2, 1, "EntityType", "WsfObject")
{
   WsfExtEntityType* entityType   = new WsfExtEntityType(0, 0, 0, 0, 0, 0, 0);
   WsfDisInterface*  interfacePtr = DIS_INTERFACE;
   if (interfacePtr != nullptr)
   {
      interfacePtr->GetExtInterface()->GetEntityType(static_cast<WsfObject*>(aVarArgs[0].GetPointer()->GetAppObject()),
                                                     *entityType);
   }
   aReturnVal.SetPointer(new UtScriptRef(entityType, aReturnClassPtr, UtScriptRef::cMANAGE));
}
