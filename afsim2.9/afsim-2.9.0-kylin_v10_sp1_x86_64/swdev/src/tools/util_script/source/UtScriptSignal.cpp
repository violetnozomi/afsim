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

#include "UtScriptSignal.hpp"

#include <algorithm>

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptMethod.hpp"

UtScriptSignal::UtScriptSignal(const UtScriptSignal& aSrc)
   : mMethodList(aSrc.mMethodList)
{
   for (size_t i = 0; i < mMethodList.size(); ++i)
   {
      mMethodList[i] = new UtScriptMethod(*mMethodList[i]);
   }
}

UtScriptSignal::~UtScriptSignal()
{
   Clear();
}

//! Remove all methods from the signal.
void UtScriptSignal::Clear()
{
   for (size_t i = 0; i < mMethodList.size(); ++i)
   {
      delete mMethodList[i];
   }
   mMethodList.clear();
}

//! Add a method to the signal.  UtScriptSignal takes ownership of the method.
void UtScriptSignal::Connect(UtScriptMethod* aMethodPtr)
{
   mMethodList.push_back(aMethodPtr);
}

//! Find and remove a method from the signal.
void UtScriptSignal::Disconnect(const UtScriptMethod& aMethod)
{
   for (size_t i = 0; i < mMethodList.size();)
   {
      if ((*mMethodList[i]) == aMethod)
      {
         delete mMethodList[i];
         mMethodList.erase(mMethodList.begin() + i);
      }
      else
      {
         ++i;
      }
   }
}

//! Invoke all methods in the signal.  Returns a list of return values.
UtScriptDataList UtScriptSignal::Call(UtScriptExecutor* aExecutorPtr, const UtScriptDataList& aArgs, UtScriptContext* aContextPtr)
{
   std::vector<UtScriptMethod*> failedMethods;
   UtScriptDataList             returnValues;
   for (auto* method : mMethodList)
   {
      UtScriptData rval;
      if (method->Call(aExecutorPtr, rval, aArgs, aContextPtr))
      {
         returnValues.push_back(rval);
      }
      else
         failedMethods.push_back(method);
   }
   for (auto* failedMethod : failedMethods)
   {
      mMethodList.erase(std::find(mMethodList.begin(), mMethodList.end(), failedMethod));
   }
   return returnValues;
}

//! Script Class for UtScriptMethodClass
class UtScriptSignalClass : public UtScriptClass
{
public:
   UtScriptSignalClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr);

   void*       Create(const UtScriptContext& aInstance) override;
   void        Destroy(void* aObject) override;
   void*       Clone(void* aObject) override;
   std::string ToString(void* aObjectPtr) const override;

   // Static Functions
   UT_DECLARE_SCRIPT_METHOD(Global);
   UT_DECLARE_SCRIPT_METHOD(RemoveGlobal);

   // Methods
   UT_DECLARE_SCRIPT_METHOD(Connect_1);
   UT_DECLARE_SCRIPT_METHOD(Connect_2);
   UT_DECLARE_SCRIPT_METHOD(Connect_3);
   UT_DECLARE_SCRIPT_METHOD(DisconnectAll);
   UT_DECLARE_SCRIPT_METHOD(Disconnect_1);
   UT_DECLARE_SCRIPT_METHOD(Disconnect_2);
   UT_DECLARE_SCRIPT_METHOD(Disconnect_3);
   UT_DECLARE_SCRIPT_METHOD(Call_1);
   UT_DECLARE_SCRIPT_METHOD(Call_2);
   UT_DECLARE_SCRIPT_METHOD(Size);
   UT_DECLARE_SCRIPT_METHOD(Methods);

   static std::map<std::string, UtScriptRef*> mGlobalSignals;
   static UtScriptSignalClass*                sClassPtr;

private:
};

std::map<std::string, UtScriptRef*> UtScriptSignalClass::mGlobalSignals;
UtScriptSignalClass*                UtScriptSignalClass::sClassPtr = nullptr;

UtScriptSignalClass::UtScriptSignalClass(const std::string& aClassName, UtScriptTypes* aScriptTypePtr)
   : UtScriptClass(aClassName, aScriptTypePtr)
{
   sClassPtr      = this;
   mConstructible = true;
   mCloneable     = true;
   SetClassName("Signal");
   AddStaticMethod(ut::make_unique<Global>());
   AddStaticMethod(ut::make_unique<RemoveGlobal>());
   AddMethod(ut::make_unique<Connect_1>("Connect"));
   AddMethod(ut::make_unique<Connect_2>("Connect"));
   AddMethod(ut::make_unique<Connect_3>("Connect"));
   AddMethod(ut::make_unique<Disconnect_1>("Disconnect"));
   AddMethod(ut::make_unique<Disconnect_2>("Disconnect"));
   AddMethod(ut::make_unique<Disconnect_3>("Disconnect"));
   AddMethod(ut::make_unique<DisconnectAll>("DisconnectAll"));
   AddMethod(ut::make_unique<Call_1>("Call"));
   AddMethod(ut::make_unique<Call_2>("Call"));
   AddMethod(ut::make_unique<Size>());
   AddMethod(ut::make_unique<Methods>());
}

void* UtScriptSignalClass::Create(const UtScriptContext& aInstance)
{
   return new UtScriptSignal;
}

void UtScriptSignalClass::Destroy(void* aObject)
{
   delete static_cast<UtScriptSignal*>(aObject);
}

void* UtScriptSignalClass::Clone(void* aObject)
{
   return new UtScriptSignal(*static_cast<UtScriptSignal*>(aObject));
}

std::string UtScriptSignalClass::ToString(void* aObjectPtr) const
{
   return "Signal";
}

// Static Functions
UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Global, 1, "Signal", "string")
{
   auto         iter   = UtScriptSignalClass::mGlobalSignals.find(aVarArgs[0].GetString());
   UtScriptRef* refPtr = nullptr;
   if (iter == UtScriptSignalClass::mGlobalSignals.end())
   {
      refPtr = new UtScriptRef(new UtScriptSignal, UtScriptSignalClass::sClassPtr, UtScriptRef::cMANAGE);
      UtScriptSignalClass::mGlobalSignals[aVarArgs[0].GetString()] = refPtr;
   }
   else
   {
      refPtr = iter->second;
   }
   aReturnVal.SetPointer(new UtScriptRef(*refPtr));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, RemoveGlobal, 1, "void", "string")
{
   UtScriptSignalClass::mGlobalSignals.erase(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Connect_1, 2, "void", "Object,string")
{
   aObjectPtr->Connect(new UtScriptMethod(aVarArgs[0].GetPointer(), aVarArgs[1].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Connect_2, 1, "void", "string")
{
   aObjectPtr->Connect(new UtScriptMethod(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Connect_3, 1, "void", "Method")
{
   aObjectPtr->Connect(new UtScriptMethod(*aVarArgs[0].GetPointer()->GetAppObject<UtScriptMethod>()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, DisconnectAll, 0, "void", "")
{
   aObjectPtr->Clear();
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Disconnect_1, 2, "void", "Object,string")
{
   aObjectPtr->Disconnect(UtScriptMethod(aVarArgs[0].GetPointer(), aVarArgs[1].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Disconnect_2, 1, "void", "string")
{
   aObjectPtr->Disconnect(UtScriptMethod(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Disconnect_3, 1, "void", "Method")
{
   aObjectPtr->Disconnect(UtScriptMethod(*aVarArgs[1].GetPointer()->GetAppObject<UtScriptMethod>()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Call_1, 0, "Array<Object>", "")
{
   UtScriptDataList* rVal = new UtScriptDataList(aObjectPtr->Call(aExecutorPtr, UtScriptDataList(), &aContext));
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Call_2, 1, "Array<Object>", "Array<Object>")
{
   auto*             callArgs = aVarArgs[0].GetPointer()->GetAppObject<UtScriptDataList>();
   UtScriptDataList* rVal     = new UtScriptDataList(aObjectPtr->Call(aExecutorPtr, *callArgs, &aContext));
   aReturnVal.SetPointer(new UtScriptRef(rVal, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Size, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetMethods().size()));
}

UT_DEFINE_SCRIPT_METHOD(UtScriptSignalClass, UtScriptSignal, Methods, 0, "Array<Method>", "")
{
   auto* methodListPtr = new UtScriptDataList;
   for (auto i : aObjectPtr->GetMethods())
   {
      UtScriptData data;
      data.SetPointer(new UtScriptRef(new UtScriptMethod(*i),
                                      reinterpret_cast<UtScriptClass*>(UtScriptMethod::sClassPtr),
                                      UtScriptRef::cMANAGE));
      methodListPtr->push_back(data);
   }
   aReturnVal.SetPointer(new UtScriptRef(methodListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

std::unique_ptr<UtScriptClass> UtScriptSignal::CreateScriptClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<UtScriptSignalClass>(aClassName, aScriptTypesPtr);
}
