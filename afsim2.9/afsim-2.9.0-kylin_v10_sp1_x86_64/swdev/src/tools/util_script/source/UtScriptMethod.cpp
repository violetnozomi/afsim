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

#include "UtScriptMethod.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScript.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptRef.hpp"

UtScriptMethod::UtScriptMethod(const std::string& aMethodName)
   : mBasePtr(nullptr)
   , mMethodName(aMethodName)
{
}

UtScriptMethod::UtScriptMethod(UtScriptRef* aBaseObjectPtr, const std::string& aMethodName)
   : mBasePtr(new UtScriptRef(*aBaseObjectPtr))
   , mMethodName(aMethodName)
{
}

UtScriptMethod::UtScriptMethod(const UtScriptMethod& aSrc)
   : mBasePtr(aSrc.mBasePtr)
   , mMethodName(aSrc.mMethodName)
   , mBoundParameters(aSrc.mBoundParameters)
{
   if (mBasePtr != nullptr)
   {
      mBasePtr = new UtScriptRef(*mBasePtr);
   }
}
UtScriptMethod::~UtScriptMethod()
{
   delete mBasePtr;
}

//! Returns true if both UtScriptMethods refer to the same script
bool UtScriptMethod::operator==(const UtScriptMethod& aRhs) const
{
   if (mBasePtr != nullptr)
   {
      return (mMethodName == aRhs.mMethodName) && (aRhs.mBasePtr != nullptr) &&
             (mBasePtr->GetAppObject() == aRhs.mBasePtr->GetAppObject());
   }
   else
   {
      return (aRhs.mBasePtr == nullptr) && (mMethodName == aRhs.mMethodName);
   }
}

bool UtScriptMethod::CallAppMethod(UtScriptExecutor*  aExecutorPtr,
                                   UtScriptData&      aReturnVal,
                                   UtScriptMethodArgs aArgs,
                                   UtScriptContext*   aGlobalContext,
                                   bool               aArgsAreOptional)
{
   int            methodIndex = -1;
   UtScriptClass* classPtr    = nullptr;
   UtScriptTypes* typesPtr    = aGlobalContext->GetTypes();
   if (mBasePtr == nullptr)
   {
      size_t dotPos = mMethodName.find('.');
      if (dotPos != std::string::npos)
      {
         classPtr = typesPtr->GetClass(mMethodName.substr(0, dotPos));
         if (classPtr != nullptr)
         {
            methodIndex = classPtr->GetMethodIndex(mMethodName.substr(dotPos + 1));
         }
      }
   }
   else
   {
      classPtr = mBasePtr->GetScriptClass();
      if (classPtr != nullptr)
      {
         methodIndex = classPtr->GetMethodIndex(mMethodName);
      }
   }
   bool called(false);
   if (methodIndex != -1 && classPtr)
   {
      UtScriptClass::InterfaceMethod* methodPtr    = classPtr->GetMethodEntry(methodIndex);
      UtScriptRef*                    referencePtr = mBasePtr;
      UtScriptRef                     nullReference(nullptr, classPtr);
      if (!referencePtr)
      {
         referencePtr = &nullReference;
      }
      size_t expectedArgCount = methodPtr->GetArgTypes().size();
      bool   hasReturn;
      if (aArgsAreOptional && (expectedArgCount < aArgs.size()))
      {
         // Copy aArgs and truncate to expected arg count
         called = true;
         UtScriptMethodArgs newArgs(aArgs.begin(), expectedArgCount);
         classPtr->Call(aExecutorPtr, *aGlobalContext, *referencePtr, methodIndex, newArgs, hasReturn, aReturnVal);
      }
      else if (expectedArgCount == aArgs.size())
      {
         called = true;
         classPtr->Call(aExecutorPtr, *aGlobalContext, *referencePtr, methodIndex, aArgs, hasReturn, aReturnVal);
      }
      else
      {
         auto out = ut::log::warning() << "Bad number of arguments.";
         out.AddNote() << "Method: " << mMethodName;
      }
   }
   return called;
}

//! Find and load the referenced script
bool UtScriptMethod::Load(UtScriptContext* aGlobalContext, UtScriptContext*& aMethodContext, UtScript*& aScriptPtr)
{
   bool loaded    = false;
   aScriptPtr     = nullptr;
   aMethodContext = nullptr;
   if (mBasePtr != nullptr)
   {
      if (mBasePtr->IsValid())
      {
         void* appObjectPtr = mBasePtr->GetAppObject();
         aMethodContext     = mBasePtr->GetScriptClass()->GetContext(appObjectPtr);
      }
   }
   else
   {
      aMethodContext = aGlobalContext;
   }
   if (aMethodContext != nullptr)
   {
      aScriptPtr = aMethodContext->GetScript(mMethodName, aMethodContext);
      // aScriptPtr = aMethodContext->GetScope()->GetLocalScript(UtScriptData::GetDictionary().GetString(mMethodNameId));
      loaded = aScriptPtr != nullptr;
   }
   return loaded;
}

//! Invoke the referenced script
//! If aArgsAreOptional is true, this will execute the script
//! event if it takes less arguments than those supplied.
bool UtScriptMethod::Call(UtScriptExecutor*       aExecutorPtr,
                          UtScriptData&           aReturnVal,
                          const UtScriptDataList& aArgs,
                          UtScriptContext*        aGlobalContext,
                          bool                    aArgsAreOptional)
{
   const UtScriptDataList* inputArgs = &aArgs;

   // If argument binding is used, we need to make a copy of the arg list:
   UtScriptDataList bindArgs;
   if (!mBoundParameters.empty())
   {
      BuildArgs(aArgs, bindArgs);
      inputArgs = &bindArgs;
   }

   UtScriptContext* contextPtr;
   UtScript*        scriptPtr;
   bool             loaded = Load(aGlobalContext, contextPtr, scriptPtr);
   if (loaded)
   {
      size_t expectedArgCount = scriptPtr->GetArgumentCount();
      if (aArgsAreOptional && (expectedArgCount < inputArgs->size()))
      {
         // Copy aArgs and truncate to expected arg count
         UtScriptDataList newArgs(inputArgs->begin(), inputArgs->begin() + expectedArgCount);
         contextPtr->Execute(aExecutorPtr, scriptPtr, aReturnVal, newArgs);
      }
      else if (expectedArgCount == inputArgs->size())
      {
         contextPtr->Execute(aExecutorPtr, scriptPtr, aReturnVal, *inputArgs);
      }
      else
      {
         auto out = ut::log::warning() << "Bad number of arguments.";
         out.AddNote() << "Method: " << mMethodName;
      }
   }
   else if (CallAppMethod(aExecutorPtr, aReturnVal, {inputArgs->data(), inputArgs->size()}, aGlobalContext, aArgsAreOptional))
   {
   }
   else
   {
      auto out = ut::log::warning() << "Could not find method.";
      out.AddNote() << "Method: " << mMethodName;
   }
   return loaded;
}

void UtScriptMethod::Bind(int aParameterIndex, const UtScriptData& aData)
{
   mBoundParameters.resize(std::max(mBoundParameters.size(), (size_t)(aParameterIndex + 1)));
   mBoundParameters[aParameterIndex] = aData;
}

void UtScriptMethod::BuildArgs(const UtScriptDataList& aInputArgs, UtScriptDataList& aFinalArgs)
{
   size_t j = 0;
   for (size_t i = 0; i < mBoundParameters.size(); ++i)
   {
      const UtScriptData& arg = mBoundParameters[i];
      if (arg.GetType() != ut::script::Data::Type::cUNDEFINED)
      {
         aFinalArgs.push_back(arg);
      }
      else
      {
         if (aInputArgs.size() > j)
         {
            aFinalArgs.push_back(aInputArgs[j++]);
         }
      }
   }
   for (; j < aInputArgs.size(); ++j)
   {
      aFinalArgs.push_back(aInputArgs[j]);
   }
}

//! Script Class for UtScriptMethodClass
class UtScriptMethodClass : public UtScriptClass
{
public:
   UtScriptMethodClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);
   ~UtScriptMethodClass() override;

   void*       Create(const UtScriptContext& aInstance) override;
   void        Destroy(void* aObject) override;
   void*       Clone(void* aObject) override;
   std::string ToString(void* aObjectPtr) const override;

   // Static Functions
   UT_DECLARE_SCRIPT_METHOD(Create_1);
   UT_DECLARE_SCRIPT_METHOD(Create_2);

   // Methods
   UT_DECLARE_SCRIPT_METHOD(Call_1);
   UT_DECLARE_SCRIPT_METHOD(Call_2);
   UT_DECLARE_SCRIPT_METHOD(Base);
   UT_DECLARE_SCRIPT_METHOD(Name);
   UT_DECLARE_SCRIPT_METHOD(PrintCallStack); // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(Assert);         // NO_DOC | PENDING
   UT_DECLARE_SCRIPT_METHOD(Bind);

private:
};

UtScriptMethodClass* UtScriptMethod::sClassPtr = nullptr;


UtScriptMethodClass::UtScriptMethodClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr)
   : UtScriptClass(aClassName, aScriptTypePtr)
{
   SetClassName("Method");
   UtScriptMethod::sClassPtr = this;
   mConstructible            = false;
   mCloneable                = true;
   AddStaticMethod(ut::make_unique<Create_1>("Create"));
   AddStaticMethod(ut::make_unique<Create_2>("Create"));
   AddMethod(ut::make_unique<Call_1>("Call"));
   AddMethod(ut::make_unique<Call_2>("Call"));
   AddMethod(ut::make_unique<Base>("Base"));
   AddMethod(ut::make_unique<Name>("Name"));
   AddMethod(ut::make_unique<Bind>());

   // These debugging methods are here as a placeholder until we do a script debugger.
   AddStaticMethod(ut::make_unique<PrintCallStack>()); // NO_DOC | PENDING
   AddStaticMethod(ut::make_unique<Assert>());         // NO_DOC | PENDING
}

UtScriptMethodClass::~UtScriptMethodClass() {}

void* UtScriptMethodClass::Create(const UtScriptContext& aInstance)
{
   return nullptr;
}
void UtScriptMethodClass::Destroy(void* aObjectPtr)
{
   UtScriptMethod* methodPtr = (UtScriptMethod*)aObjectPtr;
   delete methodPtr;
}
void* UtScriptMethodClass::Clone(void* aObjectPtr)
{
   return new UtScriptMethod(*(UtScriptMethod*)aObjectPtr);
}
std::string UtScriptMethodClass::ToString(void* aObjectPtr) const
{
   UtScriptMethod*   methodPtr = (UtScriptMethod*)aObjectPtr;
   std::stringstream ss;
   ss << "Method(";
   if (methodPtr->GetBase() != nullptr)
   {
      ss << methodPtr->GetBase()->GetScriptClass()->GetClassName() << ".";
   }
   ss << methodPtr->GetName() << ")";
   return ss.str();
}

// Static Functions

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Create_1, 2, "Method", "Object, string")
{
   UtScriptRef*    refPtr    = aVarArgs[0].GetPointer();
   UtScriptMethod* methodPtr = nullptr;
   if (refPtr->GetScriptClass()->GetContext(refPtr->GetAppObject()) == nullptr)
   {
      ut::log::error() << "Object does not provide a script context.";
   }
   else
   {
      methodPtr = new UtScriptMethod(refPtr, aVarArgs[1].GetString());
   }
   aReturnVal.SetPointer(new UtScriptRef(methodPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Create_2, 1, "Method", "string")
{
   UtScriptMethod* methodPtr = new UtScriptMethod(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(methodPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Call_1, 0, "Object", "")
{
   aObjectPtr->Call(aExecutorPtr, aReturnVal, UtScriptDataList(), &aContext);
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Call_2, 1, "Object", "Array<Object>")
{
   auto* callArgs = aVarArgs[0].GetPointer()->GetAppObject<UtScriptDataList>();
   aObjectPtr->Call(aExecutorPtr, aReturnVal, *callArgs, &aContext);
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Base, 0, "Object", "")
{
   if (aObjectPtr->GetBase() != nullptr)
   {
      aReturnVal.SetPointer(new UtScriptRef(*aObjectPtr->GetBase()));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Bind, 2, "Method", "int, Object")
{
   UtScriptMethod* boundMethod = new UtScriptMethod(*aObjectPtr);
   boundMethod->Bind(aVarArgs[0].GetInt(), aVarArgs[1]);
   UtScriptRef* refPtr = new UtScriptRef(boundMethod, aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(refPtr);
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, PrintCallStack, 0, "void", "") // NO_DOC | DEPRECATED
{
   auto out = ut::log::debug() << "Callstack:";
   UtScriptExecutor::PrintAllCallStacks(out, true);
}

UT_DEFINE_SCRIPT_METHOD(UtScriptMethodClass, UtScriptMethod, Assert, 2, "void", "bool,string") // NO_DOC | DEPRECATED
{
   if (!aVarArgs[0].GetBool())
   {
      {
         auto out = ut::log::fatal() << "Assert Failed.";
         out.AddNote() << "Argument: " << aVarArgs[1].GetString();
         auto note = out.AddNote() << "Callstack:";
         UtScriptExecutor::PrintAllCallStacks(note, false);
      }
      exit(1);
   }
}

std::unique_ptr<UtScriptClass> UtScriptMethod::CreateScriptClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<UtScriptMethodClass>(aClassName, aScriptTypesPtr);
}
