// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "script/WsfScriptObjectClass.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfObject.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptObjectClass::WsfScriptObjectClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   mIsScriptAccessible = true;
   SetClassName("WsfObject");

   // Add each of the method objects to the class.

   AddMethod(ut::make_unique<Name>());
   AddMethod(ut::make_unique<Type>());
   // Do not be tempted to at SetType as it has a remote possibility of thread-safety issues.
   // See WsfObject.cpp
   AddMethod(ut::make_unique<BaseType>());
   AddMethod(ut::make_unique<IsA_TypeOf>());
   AddMethod(ut::make_unique<ProcessInput>());
}

WsfScriptObjectClass::~WsfScriptObjectClass() {}

// virtual
void* WsfScriptObjectClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfUncloneableObject();
}

// virtual
void* WsfScriptObjectClass::Clone(void* aObjectPtr)
{
   return nullptr;
}

// virtual
void WsfScriptObjectClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfObject*>(aObjectPtr);
}
// virtual
std::string WsfScriptObjectClass::ToString(void* aObjectPtr) const
{
   WsfObject*        objectPtr = (WsfObject*)aObjectPtr;
   std::stringstream ss;
#undef GetClassName
   ss << GetClassName() << "(";
   if (objectPtr != nullptr)
   {
      ss << "Name: " << objectPtr->GetName() << "; Type: " << objectPtr->GetType();
   }
   else
   {
      ss << "Null";
   }
   ss << ")";
   return ss.str();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptObjectClass, WsfObject, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetNameId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptObjectClass, WsfObject, Type, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptObjectClass, WsfObject, BaseType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetBaseTypeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptObjectClass, WsfObject, IsA_TypeOf, 1, "bool", "string")
{
   aReturnVal.SetBool(aObjectPtr->IsA_TypeOf(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptObjectClass, WsfObject, ProcessInput, 1, "bool", "string")
{
   bool inputOk = true;

   std::string command;
   std::string inputString(aVarArgs[0].GetString());
   UtInput     input;
   SCENARIO->ConfigureInput(input);
   input.PushInput(ut::make_unique<UtInputString>(inputString));

   // This is a little ugly because we allow multiple commands in the string.
   // It is a lot simpler if one only allows a single command.
   while (inputOk && input.TryReadCommand(command))
   {
      try
      {
         if (!aObjectPtr->ProcessInput(input))
         {
            throw UtInput::UnknownCommand(input);
         }
      }
      catch (std::exception& e)
      {
         auto out = ut::log::error() << "Exception thrown while processing script.";
         out.AddNote() << "Script: " << inputString;
         out.AddNote() << "Type: " << ut::TypeNameOf(e);
         out.AddNote() << "What: " << e.what();
         inputOk = false;
      }
      catch (ut::FatalException& e)
      {
         std::string msg = "Fatal exception thrown while processing script " + inputString + ": " + e.what();
         throw ut::FatalException(std::move(msg), e.GetStacktrace());
      }
      catch (...)
      {
         std::string msg = "Unknown exception thrown while processing script " + inputString + ".";
         throw ut::FatalException(std::move(msg), ut::stacktrace::current());
      }
   }

   aReturnVal.SetBool(inputOk);
}
