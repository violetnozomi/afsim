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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtScriptStruct.hpp"

#include <memory>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtLogPublisher.hpp"
#include "UtMemory.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptParser.hpp"

//! Constructor used for creating an object of type 'struct'.
//! This form is used for a 'struct' variable that can point to any type of struct.
//! The user invokes the static method New("<struct-type>").
//! @note With the form the user must use the '->' syntax to access variables and methods.
//! It is much slower and more error-prone because everything is dynamic at script run-time.
UtScriptStruct::UtScriptStruct(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("struct", aScriptTypesPtr)
{
   SetClassName("struct");

   mConstructible      = true;
   mCloneable          = true;
   mEqualityComparable = true;
   AddStaticMethod(ut::make_unique<New>());
}

//! Constructor used for creating a user-defined pseudo-class with the 'script_struct' block.
//! This contains the prototype script context that contains the script variables and methods
//! defined by the user within the 'script_struct' block.
//!
//! @param aName             The user-defined name of the 'script_struct'
//! @param aScriptTypesPtr   Pointer to the global script types.
//! @param aScriptContextPtr Pointer to the script context that contains the script variables and methods.
UtScriptStruct::UtScriptStruct(const std::string&               aName,
                               UtScriptTypes*                   aScriptTypesPtr,
                               std::unique_ptr<UtScriptContext> aScriptContextPtr)
   : UtScriptClass(aName, aScriptTypesPtr)
   , mScriptContextPtr(std::move(aScriptContextPtr))
{
   SetClassName(aName);
   AddImplicitCastType("struct");
   mConstructible      = true;
   mCloneable          = true;
   mEqualityComparable = true;
   mIsPseudoClass      = (mScriptContextPtr != nullptr);
}

void* UtScriptStruct::Create(const UtScriptContext& aContext)
{
   UtScriptContext* prototypePtr = mScriptContextPtr.get();
   if (prototypePtr != nullptr)
   {
      // User is creating a variable of a specific type of 'script_struct'. If a struct was defined:
      //    script_struct Dog
      //       ...
      //    end_script_struct
      // Then an instance of Dog is created as:
      //    Dog rover = Dog();
   }
   else
   {
      // User is creating a variable of type 'struct'. To actually create a usable instance the user does:
      //   struct rover = struct.New("Dog");
      prototypePtr = aContext.GetEnvironment()->GetEmptyStruct();
   }
   UtScriptContext* contextPtr = new UtScriptContext(*prototypePtr);
   return contextPtr;
}

void* UtScriptStruct::Clone(void* aObjectPtr)
{
   return new UtScriptContext(*static_cast<UtScriptContext*>(aObjectPtr));
}

void UtScriptStruct::Destroy(void* aObjectPtr)
{
   delete static_cast<UtScriptContext*>(aObjectPtr);
}

UtScriptContext* UtScriptStruct::GetContext(void* aObjectPtr)
{
   return static_cast<UtScriptContext*>(aObjectPtr);
}

bool UtScriptStruct::ProcessInput(UtInput& aInput, UtScriptEnvironment* aEnvironmentPtr)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "script_struct")
   {
      std::string structName;
      aInput.ReadValue(structName);
      auto contextPtr = ut::make_unique<UtScriptContext>(*aEnvironmentPtr->GetEmptyStruct());
      auto classPtr   = ut::make_unique<UtScriptStruct>(structName, nullptr, std::move(contextPtr));
      classPtr->ProcessInput(aInput);
      if (!aEnvironmentPtr->AddStruct(structName, std::move(classPtr)))
      {
         throw UtInput::BadValue(aInput, "Duplicate struct type: " + structName);
      }
      myCommand = true;
   }
   return myCommand;
}

std::string UtScriptStruct::ToString(void* aObjectPtr) const
{
   UtScriptContext* instPtr = (UtScriptContext*)aObjectPtr;
   if (instPtr != nullptr)
   {
      // This section is written this way so that a single definition of "Print" can be used.

      std::stringstream ss;
      ss << "struct { ";
      for (const auto& var : instPtr->GetVariables())
      {
         ss << var.first << ": " << *var.second << "; ";
      }
      ss << "}";

      return ss.str();
   }
   else
   {
      return "struct (null)";
   }
}

bool UtScriptStruct::EqualTo(void* aLHS, void* aRHS)
{
   if ((aLHS != nullptr) && (aLHS != nullptr))
   {
      const auto& lhsVars = static_cast<UtScriptContext*>(aLHS)->GetVariables();
      const auto& rhsVars = static_cast<UtScriptContext*>(aRHS)->GetVariables();

      // A script struct can be identified as equal if it has the same number of variables and the same names and are equal.
      if (lhsVars.size() == rhsVars.size())
      {
         // For every variable in the left, make sure the right has a variable named the same and those variables are equal.
         for (auto& lhsVar : lhsVars)
         {
            auto rhsVar = rhsVars.find(lhsVar.first);
            if (rhsVar == rhsVars.end() || *lhsVar.second != *rhsVar->second)
            {
               return false;
            }
         }

         return true;
      }
   }

   return false;
}

void UtScriptStruct::ProcessInput(UtInput& aInput)
{
   UtInputBlock block(aInput);
   std::string  command;
   while (block.ReadCommand(command))
   {
      if (command == "script_variables")
      {
         static int        varDefCount = 0;
         std::stringstream ss;
         ss << "__var_defs_" << ++varDefCount;
         UtScriptStream scriptStream(aInput, "end_script_variables");
         if (!mScriptContextPtr->Parse(ss.str(), "void", &scriptStream, "", true))
         {
            throw UtInput::BadValue(aInput, "Script compilation failed");
         }
      }
      else if (command == "script")
      {
         UtScriptStream scriptStream(aInput, "end_script");
         if (!mScriptContextPtr->Parse(&scriptStream))
         {
            throw UtInput::BadValue(aInput, "Script compilation failed");
         }
      }
      else if (command == "uncloneable")
      {
         mCloneable = false;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(UtScriptStruct, UtScriptContext, New, 1, "struct", "string")
{
   auto classPtr = aContext.GetEnvironment()->FindStruct(aVarArgs[0].GetString());
   if (classPtr != nullptr)
   {
      auto newStructPtr = ut::make_unique<UtScriptContext>(*classPtr->GetStructContext());
      newStructPtr->SetParent(aContext.GetRoot());
      newStructPtr->ExecuteInitScripts(aExecutorPtr);
      aReturnVal.SetPointer(new UtScriptRef(newStructPtr.release(), aObjectClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      UT_SCRIPT_ABORT("No struct exists with this name");
   }
}
