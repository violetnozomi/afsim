// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfExtScriptExtensions.hpp"

#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "WsfPlatform.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{
namespace ext
{
// =================================================================================================
//! EntityType(); return the entity type
UT_DEFINE_SCRIPT_METHOD_EXT(WsfPlatform, EntityType, 0, "EntityType", "")
{
   auto entityType      = ut::make_unique<WsfExtEntityType>(0, 0, 0, 0, 0, 0, 0);
   auto extInterfacePtr = WsfExtInterface::Find(*SIMULATION);
   if (extInterfacePtr != nullptr)
   {
      extInterfacePtr->GetEntityType(aObjectPtr, *entityType);
   }
   aReturnVal.SetPointer(new UtScriptRef(entityType.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

void ScriptExtensions::ExtendScriptClasses(UtScriptTypes& aTypes)
{
   aTypes.AddClassMethod("WsfPlatform", ut::make_unique<EntityType>());
   aTypes.RegisterExtension(ut::make_unique<ScriptExtensions>());
}

bool ScriptExtensions::AddExtClassMethods(const std::string& aClassName,
                                          const std::string& aBaseName,
                                          UtScriptTypes*     aScriptTypesPtr)
{
   if (aBaseName == aClassName)
   {
      //! Avoid registering twice. Accomplished via ExtendScriptClasses()
   }
   else if (aBaseName == "WsfPlatform")
   {
      aScriptTypesPtr->AddClassMethod(aClassName, ut::make_unique<EntityType>());
   }
   else
   {
      return false;
   }

   return true;
}

} // namespace ext
} // namespace wsf
