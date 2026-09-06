// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "ScriptedPartClass.hpp"

#include <memory>

#include "Messages.hpp"
#include "ScriptedPart.hpp"
#include "UtMemory.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptDefs.hpp"
#include "script/WsfScriptManager.hpp"

namespace WsfL16
{

ScriptedPartClass::ScriptedPartClass(UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass("WsfTadilJProcessor", aScriptTypesPtr)
{
   AddClassName("WsfTadilJProcessor");

   AddMethod(ut::make_unique<SendJMessage>());
}

void ScriptedPartClass::RegisterType(UtScriptTypes& aTypes)
{
   aTypes.Register(ut::make_unique<ScriptedPartClass>(&aTypes));
}


UT_DEFINE_SCRIPT_METHOD(ScriptedPartClass, ScriptedPart, SendJMessage, 1, "void", "WsfTadilJMessageI")
{
   if (aObjectPtr != nullptr)
   {
      UtScriptRef*           arg    = aVarArgs[0].GetPointer();
      Messages::InitialBase* msgPtr = (Messages::InitialBase*)arg->GetAppObject();
      aObjectPtr->SendJMessage(TIME_NOW, msgPtr->Clone());
   }
}

} // namespace WsfL16
