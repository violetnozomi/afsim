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

#include "UtScriptLanguageCtx.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

#include "UtDictionary.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptLanguageParser.hpp"
#include "UtScriptScope.hpp"
#include "UtScriptStruct.hpp"
#include "UtStringIdLiteral.hpp"

namespace UtScriptLanguage
{
using Opcode = UtScript::Opcode;

constexpr script_code_t cMAX_STACK_INDEX  = 100000;
constexpr ptrdiff_t     cERASED_TEMP_SLOT = -1;

UtScriptClass* UsVal::sVarClassPtr = nullptr;

UtScript& operator<<(UtScript& aScript, ptrdiff_t aVal)
{
   return aScript << static_cast<script_code_t>(aVal);
}

UsConstCtx::UsConstCtx()
{
   mEnv       = nullptr;
   mScriptPtr = nullptr;
}

UsConstCtx::~UsConstCtx()
{
   delete mScriptPtr;
}

void UsConstCtx::Reset()
{
   // UsVal::sVarClassPtr = tyVar;

   mGlobalVarMode      = false;
   mTopCtx             = nullptr;
   mInstancePtr        = nullptr;
   mScopePtr           = nullptr;
   mNextNameId         = 0;
   mDocumentOffset     = 0;
   mDocumentLineOffset = 0;
   mNameListMap.clear();
   delete mScriptPtr;
   mScriptPtr = new UtScript;
}

//! A Semantic error has occurred.
void UsCtx::SemErr(const char* aMsg)
{
   mConst->mParser->SemErr(aMsg);
}

void UsCtx::ExpectedType(UsVal& v, UtScriptClass* t)
{
   if (v.Type(*mConst) != t)
   {
      std::stringstream ss;
      ss << "Expected type " << t->GetClassName() << ", not " << v.TypeName(*mConst);
      SemErr(ss.str().c_str());
   }
}

UsVal UsCtx::CastToBasicType(UsVal& v, UtScriptClass* ty)
{
   if (v.mValue != nullptr)
   {
      UsVal r(v);
      if (ty->IsBool())
         r.mValue->CastToBool();
      else if (ty->IsInt())
         r.mValue->CastToInt();
      else if (ty->IsDouble())
         r.mValue->CastToDouble();
      else if (ty->IsString())
         r.mValue->CastToString();
      else if (ty == mConst->tyObject)
      {
      }
      r.mType = ty;
      return r;
   }
   return UsVal();
}

UsVal UsCtx::ImplicitCast(UsVal v, UtScriptClass* ty)
{
   if (v.mInitList)
   {
      return RealizeContainer(*v.mInitList, ty);
   }

   // This forces variable a variable in a script_struct to be loaded on to the stack if not already there.
   if (v.mStructVarName >= 0)
   {
      v = TempStack(v);
   }

   if ((v.Type(*mConst) != nullptr) && (ty != nullptr))
   {
      if (v.Type(*mConst) == ty)
         return v;
      if (v.Type(*mConst)->IsOfType(ty->GetClassName()) || v.Type(*mConst) == mConst->tyVar || ty == mConst->tyVar)
      {
         UsVal r = v;
         r.mType = ty;
         return r;
      }
      if (v.Type(*mConst)->CanImplicitCast(ty->GetClassName()))
      {
         return ExplicitCast(v, ty);
      }
      else
      {
         std::stringstream ss;
         ss << "Cannot implicitly cast to " << ty->GetClassName() << " from " << v.Type(*mConst)->GetClassName();
         if (v.Type(*mConst)->CanExplicitCast(ty->GetClassName()))
         {
            ss << ".  Explicitly cast with (" << ty->GetClassName() << ").";
         }
         SemErr(ss.str().c_str());
      }
   }
   return UsVal();
}

UsVal UsCtx::ExplicitCast(UsVal& v, UtScriptClass* ty)
{
   if (ty == mConst->tyNull)
   {
      ty = mConst->tyObject;
   }
   if ((v.Type(*mConst) != nullptr) && (ty != nullptr))
   {
      if (v.Type(*mConst) == ty)
         return v;
      if (v.Type(*mConst)->IsOfType(ty->GetClassName()))
         return v;
      if (v.Type(*mConst)->CanImplicitCast(ty->GetClassName()) ||
          v.Type(*mConst)->CanExplicitCast(ty->GetClassName()) || ty->IsOfType(v.Type(*mConst)->GetClassName()))
      {
         if (v.IsVariable())
         {
            UsVal v0 = TempStack(v);
            UsVal r  = UsVal::Temporary(ty, StackAlloc());
            Instr() << UtScript::Opcode::CAST;
            TagStoreLoc() << r.mStackIndex << ty << v0.GetStackIndex();
            return r;
         }
         else if (v.mValue != nullptr)
         {
            // If v is a literal, we can cast now
            UsVal r = CastToBasicType(v, ty);
            if (r.mValue != nullptr)
               return r;
         }
         else if ((v.Type(*mConst) != nullptr) && v.Type(*mConst)->IsNull())
         {
            UsVal r = v;
            r.mType = ty;
            return r;
         }
         else
         {
            // not sure if it's possible to get here.
            assert(!"need case here");
         }
      }

      {
         std::stringstream ss;
         ss << "Cannot cast to " << ty->GetClassName() << " from " << v.Type(*mConst)->GetClassName();
         SemErr(ss.str().c_str());
      }
   }
   return UsVal();
}

UsVal UsCtx::GetAttribute(UtScriptClass* ty, UtScriptLanguage::Token* t)
{
   if (ty != nullptr)
   {
      UtStringId methodNameId(t->val);
      if (ty->HasStaticMethod(methodNameId))
      {
         UsVal m         = mConst->ValFromType(ty);
         m.mMethodNameId = methodNameId;
         return m;
      }
      else
      {
         std::stringstream ss;
         ss << "Static method '" << t->val << "' does not exist on type " << ty->GetClassName();
         SemErr(ss.str().c_str());
      }
   }
   return UsVal();
}

UsVal UsCtx::GetDynAttribute(const UsVal& v, UtScriptLanguage::Token* t)
{
   UsVal attrVal(TempStack(v));
   attrVal.mStructVarName = NameId(t->val);
   return attrVal;
}

// Ensure that the method (in 'ident') is a valid within the given class/script_struct (in 'v')
UsVal UsCtx::GetClassMethod(UsVal& v, const char* ident)
{
   UtScriptClass* ty = v.Type(*mConst);
   if (ty != nullptr)
   {
      UtStringId identId{ident};
      if (ty->IsPseudoClass())
      {
         auto structPtr = dynamic_cast<UtScriptStruct*>(ty);
         if (!structPtr)
         {
            throw UtException("Unexpected script class encountered: " + ty->GetClassName());
         }
         UtScriptContext* contextPtr = structPtr->GetStructContext();
         UtScript*        scriptPtr  = contextPtr->FindScript(identId);
         if (scriptPtr != nullptr)
         {
            // Save the name and the pointer to the script method.
            // mType should be pointing at the name of the script_struct type.
            UsVal retVal(TempStack(v, true));
            retVal.mValName      = ident;
            retVal.mValNameId    = identId;
            retVal.mPrototypePtr = scriptPtr->mPrototype;
            return retVal;
         }
         std::stringstream ss;
         ss << "Method '" << ident << "' does not exist on script_struct " << ty->GetClassName();
         SemErr(ss.str().c_str());
      }
      else
      {
         UtScriptClass::MethodList methods;
         ty->FindMethods(identId, methods);
         if (!methods.empty())
         {
            UsVal retVal(v);
            retVal.mMethodNameId = identId;
            return retVal;
         }
         std::stringstream ss;
         ss << "Method '" << ident << "' does not exist on class " << ty->GetClassName();
         SemErr(ss.str().c_str());
      }
   }
   else
   {
      SemErr("Cannot call a method on type-less object");
   }
   return UsVal();
}

// Ensure that the variable (in 'ident') is a valid within the given class/script_struct (in 'v')
UsVal UsCtx::GetClassVariable(UsVal& v, const char* ident)
{
   UtScriptClass* ty = v.Type(*mConst);
   if (ty != nullptr)
   {
      if (ty->IsPseudoClass())
      {
         auto structPtr = dynamic_cast<UtScriptStruct*>(ty);
         if (!structPtr)
         {
            throw UtException("Unexpected script class encountered: " + ty->GetClassName());
         }
         UtScriptContext* contextPtr = structPtr->GetStructContext();
         UtScriptData*    varPtr     = contextPtr->FindVar(ident);
         if (varPtr != nullptr)
         {
            Ty*   varTypePtr = GetDataType(*varPtr);
            UsVal retVal(TempStack(v, true));
            retVal.mStructVarName = NameId(ident);
            retVal.mType          = varTypePtr;
            retVal.mFlags |= UsVal::cSTRUCT_VARIABLE;
            return retVal;
         }
         std::stringstream ss;
         ss << "Variable '" << ident << "' does not exist on script_struct " << ty->GetClassName();
         SemErr(ss.str().c_str());
      }
      else
      {
         // Variable access not currently defined for script classes (i.e.: those defined in code).
         std::stringstream ss;
         ss << "Variable '" << ident << "' does not exist on class " << ty->GetClassName();
         SemErr(ss.str().c_str());
      }
   }
   else
   {
      SemErr("Cannot access a variable on type-less object");
   }
   return UsVal();
}

// Given a UtScriptData, return its underlying UtScriptClass.
Ty* UsCtx::GetDataType(const UtScriptData& aData)
{
   Ty* ty(nullptr);
   switch (aData.GetType())
   {
   case ut::script::Data::Type::cBOOL:
      ty = mConst->tyBool;
      break;
   case ut::script::Data::Type::cINT:
      ty = mConst->tyInt;
      break;
   case ut::script::Data::Type::cDOUBLE:
      ty = mConst->tyDouble;
      break;
   case ut::script::Data::Type::cSTRING:
      ty = mConst->tyString;
      break;
   case ut::script::Data::Type::cPOINTER:
   {
      UtScriptRef* refPtr = aData.GetPointer();
      if (refPtr != nullptr)
      {
         ty = refPtr->GetScriptClass();
      }
      else
      {
         ut::log::error() << "Internal error in UsCtx::GetDataType";
         assert(false);
      }
   }
   break;
   default:
      break;
   }
   return ty;
}

void UsCtx::FreeIfTemporary(const UsVal& v)
{
   if ((v.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
   {
      return;
   }

   ptrdiff_t slot = v.GetStackIndex();
   assert(slot >= 0 && static_cast<script_code_t>(slot) < cMAX_STACK_INDEX);
   UsCtx* sc = ScriptContext();

   // if the slot is in the current scope used temp slots, erase it and put it into the
   // free stack slots.
   auto& curTemps = sc->mCurTempSlots;
   for (size_t i = curTemps.size() - 1; curTemps[i] >= cERASED_TEMP_SLOT; --i)
   {
      assert(i != 0); // iteration should have stopped already because of the negative current statement id in the temp stack.
      if (curTemps[i] == slot)
      {
         curTemps[i] = cERASED_TEMP_SLOT;
         sc->mFreeStackSlots.push_back(slot);
         return;
      }
   }

   // if the slot is in the used temp slots of the parent scope, erase it and put it into the
   // free stack slots.
   auto& parentTemps = sc->mParentTempSlots;
   for (size_t i = parentTemps.size() - 1; parentTemps[i] >= cERASED_TEMP_SLOT; --i)
   {
      assert(i != 0); // iteration should have stopped already because of the negative current statement id in the temp stack.
      if (parentTemps[i] == slot)
      {
         parentTemps[i] = cERASED_TEMP_SLOT;
         sc->mFreeStackSlots.push_back(slot);
         return;
      }
   }
}

UsVal UsCtx::TempStack(const UsVal& v, bool aDirectLoadData)
{
   if (v.GetStackIndex() >= 0)
      return v;
   UsVal  r  = ToStack(v, aDirectLoadData);
   UsCtx* sc = ScriptContext();
   assert(!sc->mCurrentStatements.empty());
   ptrdiff_t idx = r.GetStackIndex();
   if (idx >= 0 && static_cast<script_code_t>(idx) < cMAX_STACK_INDEX)
   {
      // If the temp slot is currently scheduled to be freed when the parent scope
      // exits, then promote the freeing to be done when current scope exis.
      // Otherwise, the slot should already be scheduled to be freed when the current scope exits.
      auto& parentSlots = sc->mParentTempSlots;
      auto& curSlots    = sc->mCurTempSlots;
      for (size_t i = parentSlots.size() - 1; parentSlots[i] >= cERASED_TEMP_SLOT; --i)
      {
         assert(i != 0); // iteration should have stopped already because of the negative
                         // current statement id in the temp stack.
         if (parentSlots[i] == idx)
         {
            parentSlots[i] = cERASED_TEMP_SLOT;
            curSlots.push_back(idx);
            break;
         }
      }
      assert(std::find(curSlots.rbegin(), curSlots.rend(), idx) != curSlots.rend());
   }
   return r;
}

UsVal UsCtx::ToStack(const UsVal& v, bool aDirectLoadData)
{
   if (v.GetStackIndex() >= 0)
      return v;

   if (v.mStructVarName != -1)
   {
      // Allocate a temporary stack variable where the value from the script_struct will be stored.
      //
      // NOTE: Variables accessed using '.' (stuct-var.member) will be marked with the type of the actual
      // variable, and as such can be implicitly casted. Variables accessed using '->' (struct->member)
      // are marked as a generic type that prevents implicit casting. See the notes in AAA_README.txt
      // concerning the differences.

      UtScriptClass* typePtr = v.mType;
      if ((v.mFlags & UsVal::cSTRUCT_VARIABLE) == 0)
      {
         typePtr = mConst->tyVar;
      }
      UsVal store(UsVal::Temporary(typePtr, StackAlloc()));

      UsVal base(v);
      base.mStructVarName        = -1;
      base                       = TempStack(base, true);
      UtStringId structVarName   = v.GetStructVarNameId(Code().mNameList);
      auto       structVarNameId = UtStringId::UnsafeGetNumber(structVarName);
      Instr() << UtScript::Opcode::LOAD_STRUCT_VAR;
      TagStoreLoc() << store.GetStackIndex() << base.GetStackIndex() << static_cast<script_code_t>(structVarNameId);
      return store;
   }
   else if (v.IsVariable())
   {
      if ((v.mFlags & (UsVal::cTEMPORARY_VARIABLE | UsVal::cLOCAL_VARIABLE)) != 0)
      {
         assert(v.GetStackIndex() >= 0);
         return v;
      }
      // Processing of cREAD_ONLY_FUNCTION_VARIABLE *MUST* appear befor cGLOBAL_VARIABLE. The former also has
      // the bit for cGLOBAL_VARIABLE, but the opcode must be different. cGLOBAL_VARIABLE must be set because
      // things won't work if it isn't.
      //
      // I found this out the hard way because I tried reorganizing based on expected order of occurrence
      // and it didn't work (JAJ).
      else if ((v.mFlags & UsVal::cREAD_ONLY_FUNCTION_VARIABLE) != 0)
      {
         UsVal      r         = UsVal::Temporary(v.Type(*mConst), StackAlloc());
         UtStringId valName   = v.GetValNameId();
         auto       valNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(valName));
         Instr() << UtScript::Opcode::LOAD_READ_ONLY_FUNCTION_VAR;
         TagStoreLoc() << r.mStackIndex << valNameId;
         return r;
      }
      else if ((v.mFlags & UsVal::cGLOBAL_VARIABLE) != 0)
      {
         UsVal      r         = UsVal::Temporary(v.Type(*mConst), StackAlloc());
         UtStringId valName   = v.GetValNameId();
         auto       valNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(valName));
         Instr() << UtScript::Opcode::LOAD_GLOBAL_VAR;
         TagStoreLoc() << r.mStackIndex << valNameId;
         return r;
      }
      else if ((v.mFlags & UsVal::cSTATIC_VARIABLE) != 0)
      {
         UsVal r = UsVal::Temporary(v.Type(*mConst), StackAlloc());
         Instr() << UtScript::Opcode::LOAD_STATIC_VAR;
         TagStoreLoc() << r.mStackIndex << v.mStaticVarIndex;
         return r;
      }
      assert(!"Can't push this value");
      return UsVal();
   }
   else if (v.mValue != nullptr)
   {
      if (aDirectLoadData)
      {
         UsVal r       = v;
         r.mStackIndex = cMAX_STACK_INDEX + DataIndex(*v.mValue);
         return r;
      }
      else
      {
         UsVal r = UsVal::Temporary(v.Type(*mConst), StackAlloc());
         Instr() << UtScript::Opcode::LOAD_LOCAL_VAR;
         TagStoreLoc() << r.mStackIndex << DataIndex(*v.mValue);
         return r;
      }
   }
   else if (v.IsAppMethod())
   {
      // static methods still require a value to push to access the class

      UsVal typeVal  = v;
      typeVal.mValue = new UtScriptData(new UtScriptRef(nullptr, v.Type(*mConst)));
      return ToStack(typeVal, true);
   }
   else if (v.Type(*mConst) != nullptr /* == mConst->tyNull*/)
   {
      UsVal typeVal       = v;
      typeVal.mStackIndex = StackAlloc(false);
      Instr() << UtScript::Opcode::CREATE_NULL_VAR;
      TagStoreLoc() << typeVal.mStackIndex << v.Type(*mConst);
      return typeVal;
   }
   else
   {
      // We can get to this point one of two ways. The first is an internal logic error and the second is if some
      // sort of syntax error occurred. In the first case we want to issue a message. In the second case we don't
      // because we don't want to confuse the user with a meaningless message.
      if (mConst->mParser->mErrorsPtr->count == 0)
      {
         ut::log::error() << "ToStack cannot push this value.";
         assert(false);
      }
   }
   return UsVal();
}

// Lookup parameter type by index.  if the method is overloaded, return the type only if all overloads share the same type.
Ty* UsCtx::LookupMethodParameterType(const UsVal& fn, int aParameterIndex)
{
   if (fn.IsAppMethod() && fn.GetMethodIndex() != ut::script::npos)
   {
      UtScriptClass::InterfaceMethod* fnPtr = fn.Type(*mConst)->GetMethodEntry(fn.GetMethodIndex());
      if (static_cast<int>(fnPtr->GetArgTypes().size()) > aParameterIndex)
      {
         return fnPtr->GetArgClass(aParameterIndex);
      }
   }
   else
   {
      std::vector<UtScriptClass::InterfaceMethod*> methods;
      fn.Type(*mConst)->FindMethods(fn.mMethodNameId, methods);
      Ty* tPtr = nullptr;
      for (const auto& method : methods)
      {
         if (static_cast<int>(method->GetArgTypes().size()) > aParameterIndex)
         {
            Ty* otherType = method->GetArgClass(aParameterIndex);
            if (tPtr == nullptr)
            {
               tPtr = otherType;
            }
            else if (tPtr != otherType)
            {
               tPtr = nullptr;
               // type cannot be inferred here
               break;
            }
         }
      }
      return tPtr;
   }
   return nullptr;
}

UsVal UsCtx::Call(UsVal& fn, UsValList args, UsVal* aReturnVal)
{
   if (fn.IsAppMethod())
   {
      UtScriptClass::InterfaceMethod* fnPtr = nullptr;
      std::vector<UtScriptClass*>     argTypes(args.size());
      for (size_t i = 0; i < args.size(); ++i)
      {
         if (args[i].mInitList)
         {
            args[i] = RealizeContainer(*args[i].mInitList, LookupMethodParameterType(fn, static_cast<int>(i)));
         }
         if (args[i].Type(*mConst) != nullptr)
         {
            argTypes[i] = args[i].Type(*mConst);
         }
      }
      bool isStatic = false;

      if (fn.IsAppMethod() && fn.GetMethodIndex() != ut::script::npos)
      {
         fnPtr    = fn.Type(*mConst)->GetMethodEntry(fn.GetMethodIndex());
         isStatic = true;
      }
      else
      {
         UtStringId methodName(fn.mMethodNameId);
         fnPtr = fn.Type(*mConst)->FindMethod(methodName, argTypes);
         if (fnPtr == nullptr)
         {
            std::stringstream ss;
            ss << "Cannot find method named " << methodName << " matching (";
            for (size_t i = 0; i < argTypes.size(); ++i)
            {
               if (i != 0)
                  ss << ", ";
               ss << (argTypes[i] != nullptr ? argTypes[i]->GetClassName() : "?");
            }
            ss << ')';
            std::vector<UtScriptClass::InterfaceMethod*> existingMethods;
            fn.Type(*mConst)->FindMethods(methodName, existingMethods);
            if (!existingMethods.empty())
            {
               for (const auto& method : existingMethods)
               {
                  UtScriptFunction fn(method->GetScriptFunction());
                  ss << "\n   Expected: " << Env()->GetPrototypeDescription(&fn, methodName);
               }
            }
            SemErr(ss.str().c_str());
            return UsVal();
         }
         isStatic = fnPtr->GetIsStatic();
      }

      if (fnPtr != nullptr)
      {
         UsValList argsOnStack;
         for (size_t i = 0; i < args.size(); ++i)
         {
            UsVal arg      = ImplicitCast(args[i], fnPtr->GetArgClass(i));
            UsVal argStack = TempStack(arg, true);
            argsOnStack.push_back(argStack);
         }
         UsVal methodBaseObject;
         if (!isStatic)
         {
            methodBaseObject = TempStack(fn, true);
            Instr() << UtScript::Opcode::CALL_APP_FUNCTION << methodBaseObject.GetStackIndex();
            Instr() << static_cast<script_code_t>(fnPtr->GetIndex());
            Instr() << static_cast<script_code_t>(args.size());
         }
         else
         {
            // Before one of these method calls, set the global debug flag depending on the context
            if (fn.mMethodNameId == UtStringIdLiteral("write_d") || fn.mMethodNameId == UtStringIdLiteral("writeln_d") ||
                fn.mMethodNameId == UtStringIdLiteral("writeln_debug"))
            {
               Instr() << UtScript::Opcode::SET_DEBUG_FLAG;
            }
            Instr() << UtScript::Opcode::CALL_STATIC_APP_FUNCTION << fnPtr->GetParent();
            Instr() << static_cast<script_code_t>(fnPtr->GetIndex());
            Instr() << static_cast<script_code_t>(args.size());
         }
         for (auto& i : argsOnStack)
         {
            Code() << i.GetStackIndex();
         }

         UsVal rval;
         if (aReturnVal != nullptr)
         {
            rval = *aReturnVal;
         }
         else if (fnPtr->GetHasReturnVal())
         {
            rval = UsVal::Temporary(GetReturnClass(fnPtr), StackAlloc());
         }
         else
         {
            rval = UsVal::Temporary(GetReturnClass(fnPtr), ut::script::npos);
         }

         TagStoreLoc() << rval.GetStackIndex();

         // If the old UT_DEFINE_SCRIPT_METHOD logic was fully removed in favor of the new UT_SCRIPT_METHOD macros,
         // the freeing of the arg and methodBaseObject slots could be done before the StackAlloc of the rval.
         // However we cant do this now because the UT_SCRIPT_DEFINE logic allows setting the return value in the app
         // function without actually exiting the function. This is considered super bad practice, and every major
         // programming langauge since the late 1960s has enforced those two actions being done at the same time, but
         // alas its another edge case of the UT_SCRIPT_DEFINE macros that hurt performance. example edge case that
         // makes this impossible to do currently, unless all methods that do this are fixed:
         // UT_DEFINE_SCRIPT_METHOD(MyScriptClass, MyClass, Foo, 1, "string", "Bar")
         // {
         //    // pretend either aVarArgs[0] or aObjectPtr share the same stack slot as aReturnVal because their slots
         //    // were freed before the stack slot of aReturnVal was allocated.
         //    aReturnVal.SetString("meow"); // overwrites the stack slot, causing either aObjectPtr or aVarArgs[0] to
         //    be freed. aObjectPtr->DoSomething(); // bad dereference, aObjectPtr was already freed
         //    aVarArgs[0].GetPointer()->GetAppObject<Bar>()->DoStuff(); // bad dereference, already overwritten by
         //    setting aReturnVal.
         // }

         for (auto& arg : argsOnStack)
         {
            if (arg.GetStackIndex() != rval.GetStackIndex())
            {
               FreeIfTemporary(arg);
            }
         }

         if (!isStatic && rval.GetStackIndex() != methodBaseObject.GetStackIndex())
         {
            FreeIfTemporary(methodBaseObject);
         }

         return rval;
      }
      else
      {
         std::stringstream ss;
         ss << "Invalid method call.";
         SemErr(ss.str().c_str());
      }
   }
   else if (fn.IsScript())
   {
      const UtScriptFunction* protoPtr = fn.mPrototypePtr;
      if (protoPtr != nullptr)
      {
         if (protoPtr->mArgs.size() == args.size())
         {
            // The following is true if the script is a 'normal' script and 'false' if the script is one
            // that resides in a 'script_struct' and is being accessed using the '.' notation.
            bool                            isNormalScript = (fn.mType == nullptr);
            const UtScriptScope::ScriptRef* scriptPtr      = nullptr;
            if (isNormalScript)
            {
               // Normal script call.
               scriptPtr = Scope().FindGlobalScript(fn.GetValNameId(), true);
            }
            else if (fn.mType->IsPseudoClass())
            {
               // Calling a script inside of a 'script_struct'. mType points to the pseudo_class script context.
               auto structPtr = dynamic_cast<UtScriptStruct*>(fn.mType);
               if (!structPtr)
               {
                  throw UtException("Unexpected script class encountered: " + fn.mType->GetClassName());
               }
               UtScriptContext* contextPtr = structPtr->GetStructContext();
               assert(contextPtr != nullptr);
               scriptPtr = contextPtr->GetScope()->FindScript(fn.GetValNameId());
            }
            if (scriptPtr != nullptr)
            {
               UsValList castedArgs;
               for (size_t i = 0; i < args.size(); ++i)
               {
                  // Cast argument to the type of the formal parameter
                  UtStringId     fpTypeId   = protoPtr->mArgs[i]; // Class Name (ID) of the formal parameter
                  UtScriptClass* fpClassPtr = Env()->GetClass(fpTypeId);
                  UsVal          arg        = ImplicitCast(args[i], fpClassPtr);
                  UsVal          argStack   = TempStack(arg, true);
                  castedArgs.push_back(argStack);
               }
               UtStringId scriptName   = fn.GetValNameId();
               auto       scriptNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(scriptName));
               if (isNormalScript)
               {
                  Instr() << UtScript::Opcode::CALL_SCRIPT << scriptNameId << static_cast<script_code_t>(args.size());
               }
               else
               {
                  // struct script functions must be called from the stack because of the
                  // potential scenario where-in the struct's function replaces the current
                  // struct being called.
                  UsVal base = TempStack(fn);
                  Instr() << UtScript::Opcode::CALL_STRUCT_SCRIPT << base.GetStackIndex() << scriptNameId
                          << static_cast<script_code_t>(args.size());
                  FreeIfTemporary(base);
               }
               for (const auto& arg : castedArgs)
               {
                  Code() << arg.GetStackIndex();
               }

               const UsVal* earlyFreedArg = nullptr;
               UsVal        rval;
               if (aReturnVal != nullptr)
               {
                  rval = *aReturnVal;
               }
               else if (!protoPtr->mReturnClassPtr->IsVoid())
               {
                  // Can free a arguement early to have its stack slot recycled as the return slot
                  // only if having that argument cleaned up during the assignment to the return
                  // won't cause memory management issues.
                  // See also wsf/test_mission/auto_script/test_return.txt
                  for (const auto& arg : castedArgs)
                  {
                     if ((arg.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0 &&
                         (arg.mType->IsBasicType() || (!arg.mType->IsOfType(protoPtr->mReturnClassPtr->GetClassName()) &&
                                                       !protoPtr->mReturnClassPtr->IsOfType(arg.mType->GetClassName()))))
                     {
                        FreeIfTemporary(arg);
                        earlyFreedArg = &arg;
                        break;
                     }
                  }
                  rval = UsVal::Temporary(protoPtr->mReturnClassPtr, StackAlloc());
               }
               else
               {
                  rval = UsVal::Temporary(protoPtr->mReturnClassPtr, ut::script::npos);
               }
               TagStoreLoc() << rval.GetStackIndex();

               // free any temporary args that couldn't be freed earlier.
               for (const auto& arg : castedArgs)
               {
                  if (&arg != earlyFreedArg)
                  {
                     FreeIfTemporary(arg);
                  }
               }

               return rval;
            }
            SemErr("No script with this name");
            return UsVal();
         }
         else
         {
            SemErr("Invalid number of arguments while calling script");
         }
      }
   }
   else if (fn.mStructVarName != -1)
   {
      UsVal base          = fn;
      base.mStructVarName = -1;
      base                = TempStack(base);
      for (auto& arg : args)
      {
         arg = TempStack(arg, true);
      }
      UsVal rval = UsVal::Temporary(mConst->tyVar, StackAlloc());

      UtStringId structVarName   = fn.GetStructVarNameId(Code().mNameList);
      auto       structVarNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(structVarName));
      Instr() << UtScript::Opcode::CALL_DYN_STRUCT_SCRIPT << base.GetStackIndex() << structVarNameId
              << static_cast<script_code_t>(args.size());
      for (const auto& arg : args)
      {
         Code() << arg.GetStackIndex();
      }
      TagStoreLoc() << rval.GetStackIndex();

      for (auto& arg : args)
      {
         if (arg.GetStackIndex() != rval.GetStackIndex())
         {
            FreeIfTemporary(arg);
         }
      }

      if (base.GetStackIndex() != rval.GetStackIndex())
      {
         FreeIfTemporary(base);
      }
      return rval;
   }
   else
   {
      SemErr("Invalid method call");
   }
   return UsVal();
}

UtScriptClass* UsCtx::qType(UtScriptLanguage::Token* t)
{
   using namespace UtScriptLanguage;
   switch (t->kind)
   {
   case Parser::_string:
      return mConst->tyString;
   case Parser::_int:
      return mConst->tyInt;
   case Parser::_double:
      return mConst->tyDouble;
   case Parser::_bool:
      return mConst->tyBool;
   default:
      if (Text(t) == "null")
         return nullptr;
      return Env()->GetClass(t->val);
   }
}

UtScriptClass* UsCtx::qType(UtScriptClass* aTypePtr, std::pair<UtScriptClass*, UtScriptClass*> aArgs)
{
   if (aTypePtr != nullptr) // should be valid
   {
      UtScriptTypes::TemplateArgs args;
      args.first  = aArgs.first != nullptr ? aArgs.first->GetClassName() : UtStringId();
      args.second = aArgs.second != nullptr ? aArgs.second->GetClassName() : UtStringId();

      UtScriptTypes::TemplateType tt(aTypePtr->GetClassName(), args);
      UtScriptClass*              specPtr = Env()->GetClass(tt);
      if (specPtr == nullptr)
      {
         SemErr("Invalid template specification");
      }
      return specPtr;
   }
   return nullptr;
}

UsVal UsCtx::CreateVal(UtScriptClass* aClassPtr, bool aCallCreate)
{
   if (aClassPtr == nullptr)
      return UsVal();
   if (aClassPtr->IsBasicType() || aClassPtr->IsVoid() || aClassPtr == mConst->tyVar)
   {
      return UsVal::Temporary(aClassPtr, StackAlloc());
   }
   else
   {
      UsVal v = UsVal::Temporary(aClassPtr, StackAlloc());
      if (aCallCreate)
      {
         Instr() << UtScript::Opcode::CREATE_VAR;
         TagStoreLoc() << v.mStackIndex << aClassPtr;
      }
      return v;
   }
}

UsVal UsCtx::RealizeContainer(UsInitializerList& aInitializer, UtScriptClass* aLhsType)
{
   // Tries to create the most specific container from an initializer list.
   std::vector<UsInitializerListData::Entry>& entries = aInitializer.mDataPtr->mEntries;

   if ((aLhsType != nullptr) && aLhsType != mConst->tyObject)
   {
      UsVal          containerVal = CreateVal(aLhsType);
      bool           isSet        = nullptr != (dynamic_cast<UtScriptSet*>(aLhsType));
      bool           isMap        = nullptr != (dynamic_cast<UtScriptMap*>(aLhsType));
      bool           isArray      = nullptr != (dynamic_cast<UtScriptArray*>(aLhsType));
      UsVal          insertFn     = containerVal;
      UtScriptClass* keyTypePtr   = nullptr;
      UtScriptClass* valTypePtr   = nullptr;
      if (isSet)
      {
         insertFn.mMethodNameId = UtStringIdLiteral("Insert");
         valTypePtr             = GetContainerValue(aLhsType);
      }
      if (isMap)
      {
         insertFn.mMethodNameId = UtStringIdLiteral("Set");
         keyTypePtr             = GetContainerKey(aLhsType);
         valTypePtr             = GetContainerValue(aLhsType);
      }
      if (isArray)
      {
         insertFn.mMethodNameId = UtStringIdLiteral("PushBack");
         valTypePtr             = GetContainerValue(aLhsType);
      }
      if (valTypePtr != nullptr)
      {
         for (const auto& entry : entries)
         {
            int       stat = BeginStat();
            UsValList args;
            args.push_back(TempStack(ImplicitCast(entry.val, valTypePtr), true));
            if (keyTypePtr != nullptr)
            {
               args.push_back(TempStack(ImplicitCast(entry.key, keyTypePtr), true));
               std::swap(args[0], args[1]);
            }
            Call(insertFn, args);
            EndStat(stat);
         }
      }
      else
      {
         // The type of the LHS was not a recognized container type. In this case the initializer
         // list must be empty, otherwise the user thinks the list is used when in fact it isn't.
         // If the list is empty it is equivalent to calling the constructor.
         //
         // Vec3 vec = { 1, 2, 3 };         # Invalid: Vec3 is not a container.
         // WsfDraw d = { };                # OK: equivalent to 'WsfDraw d = WsfDraw();'
         if (!entries.empty())
         {
            SemErr("Non-empty initializer list is invalid for non-container objects");
         }
         else if (!aLhsType->IsConstructible())
         {
            SemErr("Type not constructible.");
         }
      }
      return containerVal;
   }
   else
   {
      if (entries.empty())
      {
         return CreateVal(mConst->mTypes->GetOrCreateContainer("Array<Object>"));
      }
      else
      {
         UtScriptClass* specificVal = ValType(entries.front().val);
         UtScriptClass* specificKey = ValType(entries.front().key);
         if (specificVal != nullptr)
         {
            for (size_t i = 1; i < entries.size(); ++i)
            {
               if (specificVal != entries[i].val.Type(*mConst))
               {
                  specificVal = mConst->tyObject;
                  break;
               }
            }
            if (specificKey != nullptr)
            {
               for (size_t i = 1; i < entries.size(); ++i)
               {
                  if (specificKey != entries[i].key.Type(*mConst))
                  {
                     specificKey = mConst->tyObject;
                     break;
                  }
               }
            }
         }
         if (specificVal != nullptr)
         {
            if (specificKey != nullptr)
            {
               UtScriptClass* basePtr = mConst->mTypes->GetClass(UtStringIdLiteral("Map"));
               if (basePtr != nullptr)
               {
                  UsVal containerVal     = CreateVal(mConst->mTypes->GetOrCreateContainer(
                     "Map<" + specificKey->GetClassName() + ',' + specificVal->GetClassName() + ">"));
                  UsVal insertFn         = containerVal;
                  insertFn.mMethodNameId = UtStringIdLiteral("Set");
                  for (const auto& entry : entries)
                  {
                     int       stat = BeginStat();
                     UsValList args;
                     args.push_back(ImplicitCast(entry.key, specificKey));
                     args.push_back(ImplicitCast(entry.val, specificVal));
                     Call(insertFn, args);
                     EndStat(stat);
                  }
                  return containerVal;
               }
            }
            else
            {
               UtScriptClass* basePtr = mConst->mTypes->GetClass(UtStringIdLiteral("Array"));
               if (basePtr != nullptr)
               {
                  UsVal containerVal =
                     CreateVal(mConst->mTypes->GetOrCreateContainer("Array<" + specificVal->GetClassName() + ">"));
                  UsVal insertFn         = containerVal;
                  insertFn.mMethodNameId = UtStringIdLiteral("PushBack");
                  for (const auto& entry : entries)
                  {
                     int       stat = BeginStat();
                     UsValList args;
                     args.push_back(ImplicitCast(entry.val, specificVal));
                     Call(insertFn, args);
                     EndStat(stat);
                  }
                  return containerVal;
               }
            }
         }
      }
   }
   return UsVal();
}

UsVal UsCtx::NewInitializerList()
{
   UsVal v;
   v.mInitList = new UsInitializerList;
   return v;
}

UsVal UsCtx::VarDecl(UtScriptClass* ty, UtScriptLanguage::Token* t, int flags /*= 0*/)
{
   if (ty != nullptr)
   {
      if (ty == mConst->tyVoid)
      {
         SemErr("cant't declare a variable of type 'void'");
      }
      if (ty == mConst->tyNull)
      {
         SemErr("can't declare a variable of type 'null'");
      }
      if (mConst->mGlobalVarMode)
         flags |= UsVal::cGLOBAL_VARIABLE;

      if ((flags & UsVal::cEXTERN_VARIABLE) != 0)
      {
         std::string name = t->val;
         UsVal       sym  = Search(name);
         if (sym.IsValid() && ValType(sym) != ty)
         {
            std::stringstream ss;
            ss << "extern variable type does not match; previously declared as " << ty->GetClassName();
            SemErr(ss.str().c_str());
         }
         else
         {
            Scope().RegisterExternVariable(name, ty);
            UsVal newVar = UsVal::GlobalVar(t->val, ty);
            newVar.mFlags |= UsVal::cEXTERN_VARIABLE;
            return newVar;
         }
      }
      else if ((flags & (UsVal::cGLOBAL_VARIABLE | UsVal::cEXTERN_VARIABLE)) != 0)
      {
         UtScriptScope::InstanceVariable* newvar = nullptr;
         UtScriptScope::InstanceVariable* oldVar = Scope().GetLocalInstanceVariableDef(t->val);
         if ((oldVar != nullptr) && oldVar->mIsLocal)
         {
            SemErr("Variable with this name already exists in this scope");
         }
         else
         {
            newvar = Scope().RegisterVariable(t->val, ty);
            if (newvar != nullptr)
            {
               Instance().Var(t->val) = UtScriptData(ty, nullptr);
               return UsVal::GlobalVar(t->val, ty);
            }
            else
            {
               SemErr("Failed to create variable");
            }
         }
      }
      else if ((flags & UsVal::cSTATIC_VARIABLE) != 0)
      {
         UsVal newVar                   = UsVal::StaticVariable(t->val, ty, mNextStaticVarIndex++);
         newVar.mValidAfterInstruction  = CodeOffset();
         mLocalSymbols[newVar.mValName] = newVar;
         return newVar;
      }
      else
      {
         UsVal newVar                   = UsVal::LocalVariable(t->val, ty, StackAlloc(false));
         newVar.mValidAfterInstruction  = CodeOffset();
         mLocalSymbols[newVar.mValName] = newVar;
         return newVar;
      }
   }
   return UsVal();
}

void UsCtx::VarDeclInit(UsVal& v)
{
   if ((ValType(v) != nullptr) && (0 != (v.mFlags & UsVal::cLOCAL_VARIABLE)))
   {
      Instr() << UtScript::Opcode::INIT_VAR;
      TagStoreLoc() << v.GetStackIndex() << ValType(v);
   }
}

void UsCtx::AddToInitializerList(UsVal& initList, UsVal& p1, UsVal* p2) /*UsVal& val, UsVal* key) */
{
   if (!initList.mInitList)
      return;
   UsInitializerList*           initListPtr = initList.mInitList.Pointer();
   UsInitializerListData::Entry e;
   if (p2 != nullptr)
   {
      e.key = p1;
      e.val = *p2;
   }
   else
   {
      e.val = p1;
   }
   if (!initListPtr->mDataPtr->mEntries.empty())
   {
      if (ValType(initListPtr->mDataPtr->mEntries[0].key) != nullptr)
      {
         if (ValType(e.key) == nullptr)
            SemErr("No key specified");
      }
      else
      {
         if (ValType(e.key) != nullptr)
            SemErr("Initializer entry has key, unlike ealier entries.");
      }
   }
   initListPtr->mDataPtr->mEntries.push_back(e);
}

UtScriptClass* UsCtx::GetContainerValue(UtScriptClass* aType)
{
   return aType->GetContainerDataType();
}

UtScriptClass* UsCtx::GetContainerKey(UtScriptClass* aType)
{
   return aType->GetContainerKeyType();
}

UsVal UsCtx::Op(UsVal& lhs, UsVal& rhs, int op)
{
   switch (op)
   {
   case cASSIGN:
   case cASSIGN_INITIAL:
   case cASSIGN_PLUS:
   case cASSIGN_MINUS:
   case cASSIGN_TIMES:
   case cASSIGN_DIV:
   {
      UsVal rhs2 = ImplicitCast(rhs, lhs.Type(*mConst));
      if (rhs2.Type(*mConst) == lhs.Type(*mConst))
      {
         switch (op)
         {
         case cASSIGN_PLUS:
            rhs2 = Op(lhs, rhs2, cADD);
            break;
         case cASSIGN_MINUS:
            rhs2 = Op(lhs, rhs2, cSUBTRACT);
            break;
         case cASSIGN_TIMES:
            rhs2 = Op(lhs, rhs2, cMULTIPLY);
            break;
         case cASSIGN_DIV:
            rhs2 = Op(lhs, rhs2, cDIVIDE);
            break;
         default:
            break;
         }
         UsVal rhsOnStack = TempStack(rhs2, true);
         if (lhs.mStructVarName >= 0)
         {
            UsVal base                 = lhs;
            base.mStructVarName        = -1;
            base                       = TempStack(base, true);
            UtStringId structVarName   = lhs.GetStructVarNameId(Code().mNameList);
            auto       structVarNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(structVarName));
            Instr() << UtScript::Opcode::STORE_STRUCT_VAR << base.GetStackIndex() << structVarNameId
                    << rhsOnStack.GetStackIndex();
         }
         else if ((lhs.mFlags & UsVal::cSTATIC_VARIABLE) != 0)
         {
            Instr() << UtScript::Opcode::STORE_STATIC_VAR << lhs.mStaticVarIndex << rhsOnStack.GetStackIndex();
            if (mStaticVariableJump != ut::script::npos)
            {
               Code().mOps[mStaticVariableJump] = CodeOffset();
               mStaticVariableJump              = ut::script::npos;
            }
         }
         else if ((lhs.mFlags & UsVal::cGLOBAL_VARIABLE) != 0)
         {
            UtStringId valName   = lhs.GetValNameId();
            auto       valNameId = static_cast<script_code_t>(UtStringId::UnsafeGetNumber(valName));
            if ((rhsOnStack.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0)
            {
               Instr() << UtScript::Opcode::STORE_RVALUE_GLOBAL_VAR << valNameId << rhsOnStack.GetStackIndex();
            }
            else
            {
               Instr() << UtScript::Opcode::STORE_GLOBAL_VAR << valNameId << rhsOnStack.GetStackIndex();
            }
         }
         else if ((lhs.mFlags & UsVal::cLOCAL_VARIABLE) != 0)
         {
            assert(lhs.GetStackIndex() >= 0);
            auto rhsIndex = static_cast<script_code_t>(rhsOnStack.GetStackIndex());
            if (rhsIndex < cMAX_STACK_INDEX)
            {
               if ((rhsOnStack.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0 && mLastStoreCodeLoc != ut::script::npos &&
                   (mNoRecycleLastIdx != lhs.GetStackIndex() || op != cASSIGN) // prevents temp elimination in a=b+a;
                                                                               // but allows it for a+=a;
                   && Code().mOps[mLastStoreCodeLoc] == rhsIndex)
               {
                  // have the last store write directly into lhs, instead of the temp variable.
                  // additional work could be done to ensure the stack space for the
                  // temp variable never gets allocated.
                  Code().mOps[mLastStoreCodeLoc] = static_cast<script_code_t>(lhs.GetStackIndex());
               }
               else
               {
                  Instr() << Opcode::STORE_LOCAL_VAR_SP;
                  TagStoreLoc() << lhs.GetStackIndex() << rhsIndex;
               }
            }
            else
            {
               Instr() << Opcode::LOAD_LOCAL_VAR;
               TagStoreLoc() << lhs.GetStackIndex() << (rhsIndex - cMAX_STACK_INDEX);
            }
         }
         else
         {
            SemErr("Cannot assign to LHS");
         }

         if (rhsOnStack.GetStackIndex() != lhs.GetStackIndex())
         {
            FreeIfTemporary(rhsOnStack);
         }
         return lhs;
      }
      else
      {
         SemErr("Cannot assign: RHS type incompatible");
      }
      return UsVal();
   }
   case cEQ:
   case cNE:
   case cLT:
   case cGT:
   case cLE:
   case cGE:
   {
      if (!lhs.IsValid())
         return UsVal();

      // The former code that was here always tried to cast the RHS to the type of the LHS, but that is
      // WRONG! "if (1 == 1.1)" was returning true because 1.1 was cast to 1! Numeric comparisons should
      // occur with values promoted as necessary to the widest type.

      // If the type of the RHS is numeric, the type of the LHS is checked to see if it is narrower
      // than the RHS. If so, the LHS is promoted to the type of the RHS.
      //
      // Note that at the end the RHS will be casted to the type of the LHS (which may be promoted).
      // This takes care of the situation where the RHS is narrower.

      UsVal lhsVal  = lhs;
      Ty*   lhsType = lhsVal.Type(*mConst);
      Ty*   rhsType = ValType(rhs);
      if (rhsType != lhsType) // Types don't match.
      {
         if (rhsType == mConst->tyDouble)
         {
            if ((lhsType == mConst->tyInt) || (lhsType == mConst->tyBool))
            {
               lhsVal = ImplicitCast(lhsVal, rhsType); // promote to int or bool to double
            }
         }
         else if (rhsType == mConst->tyInt)
         {
            if (lhsType == mConst->tyBool)
            {
               lhsVal = ImplicitCast(lhsVal, rhsType); // promote bool to int
            }
         }
      }

      // Potentially promotes RHS to a wider value.
      UsVal rhsCasted = ImplicitCast(rhs, ValType(lhsVal));

      // If both values are constants, we can compute the answer right now
      if ((lhsVal.mValue != nullptr) && (rhsCasted.mValue != nullptr))
      {
         switch (op)
         {
         case cEQ:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue == *rhsCasted.mValue), mConst->tyBool);
         case cNE:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue != *rhsCasted.mValue), mConst->tyBool);
         case cLT:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue < *rhsCasted.mValue), mConst->tyBool);
         case cGT:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue > *rhsCasted.mValue), mConst->tyBool);
         case cLE:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue <= *rhsCasted.mValue), mConst->tyBool);
         case cGE:
            return UsVal::Value(ut::script::Data(*lhsVal.mValue >= *rhsCasted.mValue), mConst->tyBool);
         }
      }

      auto opcode    = static_cast<UtScript::Opcode>(0);
      bool swapSides = false;
      switch (op)
      {
      case cEQ:
         opcode = UtScript::Opcode::EQUAL;
         break;
      case cNE:
         opcode = UtScript::Opcode::NOT_EQUAL;
         break;
      case cLT:
         opcode = UtScript::Opcode::LESS;
         break;
      case cGT:
         opcode    = UtScript::Opcode::LESS;
         swapSides = true;
         break;
      case cLE:
         opcode = UtScript::Opcode::LESS_EQUAL;
         break;
      case cGE:
         opcode    = UtScript::Opcode::LESS_EQUAL;
         swapSides = true;
         break;
      }

      UsVal rhsVal = TempStack(rhsCasted, true);
      lhsVal       = TempStack(lhsVal, true);
      UsVal r;
      if ((rhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0)
      {
         r       = rhsVal;
         r.mType = mConst->tyBool;
      }
      else if ((lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0)
      {
         r       = lhsVal;
         r.mType = mConst->tyBool;
      }
      else
      {
         r = CreateVal(mConst->tyBool);
      }

      Instr() << opcode;
      if (swapSides)
      {
         TagStoreLoc() << r.GetStackIndex() << rhsVal.GetStackIndex() << lhsVal.GetStackIndex();
      }
      else
      {
         TagStoreLoc() << r.GetStackIndex() << lhsVal.GetStackIndex() << rhsVal.GetStackIndex();
      }

      if (r.GetStackIndex() != lhsVal.GetStackIndex() &&
          (lhsVal.GetStackIndex() != rhsVal.GetStackIndex() || (rhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0))
      {
         FreeIfTemporary(lhsVal);
      }
      if (r.GetStackIndex() != rhsVal.GetStackIndex() &&
          (lhsVal.GetStackIndex() != rhsVal.GetStackIndex() || (lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) != 0))
      {
         FreeIfTemporary(rhsVal);
      }
      return r;
   }
   case cADD:
   case cSUBTRACT:
   case cMULTIPLY:
   case cDIVIDE:
   {
      if (!lhs.IsValid())
         return UsVal();

      // The former logic here was better than the former comparison operator logic in terms of promotion,
      // but it didn't work quite right for when it was just int's and bool's. The logic is now the same in
      // both places (as it should be...)

      UsVal lhsVal  = lhs;
      Ty*   lhsType = lhsVal.Type(*mConst);
      Ty*   rhsType = ValType(rhs);
      if (rhsType != lhsType) // Types don't match.
      {
         if (rhsType == mConst->tyDouble)
         {
            if ((lhsType == mConst->tyInt) || (lhsType == mConst->tyBool))
            {
               lhsVal = ImplicitCast(lhsVal, rhsType); // promote to int or bool to double
            }
         }
         else if (rhsType == mConst->tyInt)
         {
            if (lhsType == mConst->tyBool)
            {
               lhsVal = ImplicitCast(lhsVal, rhsType); // promote bool to int
            }
         }
      }

      // Potentially promotes RHS to a wider value.
      UsVal rhsCasted = ImplicitCast(rhs, ValType(lhsVal));

      // If both values are constants, we can compute the answer right now
      if ((lhsVal.mValue != nullptr) && (rhsCasted.mValue != nullptr))
      {
         switch (op)
         {
         case cADD:
            return UsVal::Value(*lhsVal.mValue + *rhsCasted.mValue, ValType(lhsVal));
         case cSUBTRACT:
            return UsVal::Value(*lhsVal.mValue - *rhsCasted.mValue, ValType(lhsVal));
         case cMULTIPLY:
            return UsVal::Value(*lhsVal.mValue * *rhsCasted.mValue, ValType(lhsVal));
         case cDIVIDE:
            return UsVal::Value(*lhsVal.mValue / *rhsCasted.mValue, ValType(lhsVal));
         }
      }

      auto opcode = static_cast<UtScript::Opcode>(0);
      switch (op)
      {
      case cADD:
         opcode = UtScript::Opcode::ADD;
         break;
      case cSUBTRACT:
         opcode = UtScript::Opcode::SUBTRACT;
         break;
      case cMULTIPLY:
         opcode = UtScript::Opcode::MULTIPLY;
         break;
      case cDIVIDE:
         opcode = UtScript::Opcode::DIVIDE;
         break;
      }

      UsVal rhsVal = TempStack(rhsCasted, true);
      lhsVal       = TempStack(lhsVal, true);
      UsVal r(lhsVal);
      if (!lhsVal.mType->IsBasicType() || lhsVal.mType == mConst->tyString ||
          (lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
      {
         r = CreateVal(lhsVal.Type(*mConst));
         // can't try to recycle the stack slot of rhsVal for r because r will be assigned the value of lhsVal
         // then have rhsVal manipulate r directly. If rhsVal was recycled that would mean its slot (same as r) would
         // get overwritten before rhsVal gets used.
      }
      Instr() << opcode;
      TagStoreLoc() << r.GetStackIndex() << lhsVal.GetStackIndex() << rhsVal.GetStackIndex();

      if (r.GetStackIndex() != rhsVal.GetStackIndex() && lhsVal.GetStackIndex() != rhsVal.GetStackIndex())
      {
         FreeIfTemporary(rhsVal);
      }

      if (r.GetStackIndex() != lhsVal.GetStackIndex() && lhsVal.GetStackIndex() != rhsVal.GetStackIndex())
      {
         FreeIfTemporary(lhsVal);
      }

      if (lhsVal.mType == mConst->tyString && (lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
      {
         // concatenation to a non-temporary string must have an intermediate temporary.
         // otherwise string a=MakeStr(); a=a+"b"; will fail, due to unintended cleanup occuring.
         mLastStoreCodeLoc = ut::script::npos;
      }
      else if ((rhsVal.mFlags & UsVal::cLOCAL_VARIABLE) != 0)
      {
         // The intermediate temporary variable can't be eliminated for expressions in the form of a=b+a
         // because internally the elimnation would cause it to be implemented as a=b;a+=a; which is wrong.
         mNoRecycleLastIdx = rhsVal.GetStackIndex();
      }
      return r;
   }

   default:
      assert(false); // bad operator
      return UsVal();
   }
}

UsVal UsCtx::Op(UsVal& lhs, UsVal& idx, UsVal& rhs, int op)
{
   // Change index operator to a call to Set() or Get()
   if (op == cINDEX && (lhs.Type(*mConst) != nullptr))
   {
      if (rhs.mInitList)
      {
         rhs = RealizeContainer(*rhs.mInitList, GetContainerValue(lhs.Type(*mConst)));
      }
      if (rhs.Type(*mConst) != nullptr)
      {
         UsVal getMethod         = lhs;
         getMethod.mMethodNameId = UtStringIdLiteral("Set");
         UsValList args;
         args.push_back(idx);
         args.push_back(rhs);
         return Call(getMethod, args);
      }
      else
      {
         UsVal getMethod         = lhs;
         getMethod.mMethodNameId = UtStringIdLiteral("Get");
         UsValList args;
         args.push_back(idx);
         return Call(getMethod, args);
      }
   }
   return UsVal();
}

UsVal UsCtx::Op(UsVal& v, int op)
{
   switch (op)
   {
   case cNOT:
   {
      // apply not operator to constants now insted of at runtime
      if (v.mValue != nullptr)
      {
         return UsVal::Value(UtScriptData(!(v.mValue->IsTrue())), mConst->tyBool);
      }

      UsVal src = TempStack(v, true); // implicit cast??? , mConst->tyBool);

      UsVal result;
      if (src.mType != mConst->tyBool || (src.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
      {
         result = CreateVal(mConst->tyBool);
      }
      else
      {
         result = src;
      }
      Instr() << UtScript::Opcode::NOT;
      TagStoreLoc() << result.GetStackIndex() << src.GetStackIndex();
      return result;
   }
   case cUNARY_MINUS:
      if ((v.Type(*mConst) == nullptr) || !(v.Type(*mConst)->IsInt() || v.Type(*mConst)->IsDouble()))
      {
         std::stringstream ss;
         ss << "Cannot perform unary '-' on type " << v.TypeName(*mConst);
         SemErr(ss.str().c_str());
         return v;
      }
      else
      {
         // (-1) is parsed as unary_minus(1)
         // An important optimization: apply unary operator to constants now instead of at runtime
         if (v.mValue != nullptr)
         {
            UsVal r = v;
            -(*r.mValue);
            return r;
         }
         else
         {
            UsVal vVal = TempStack(v);
            UsVal r(vVal);
            if ((vVal.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
            {
               r = CreateVal(v.Type(*mConst));
            }
            Instr() << UtScript::Opcode::NEGATE;
            TagStoreLoc() << r.GetStackIndex() << vVal.GetStackIndex();
            return r;
         }
      }
      break;
   case cUNARY_PLUS:
      if ((v.Type(*mConst) == nullptr) || !(v.Type(*mConst)->IsInt() || v.Type(*mConst)->IsDouble()))
      {
         std::stringstream ss;
         ss << "Cannot perform unary '+' on type " << v.TypeName(*mConst);
         SemErr(ss.str().c_str());
      }
      // NOP
      return v;
   default:
      return UsVal();
   }
}

UsVal UsCtx::Ctor(UtScriptClass* aType, const UsValList& aArgs)
{
   if (aType != nullptr)
   {
      if (aArgs.empty())
      {
         if (aType->IsConstructible())
         {
            return CreateVal(aType);
         }
         SemErr("Type not constructible.");
      }
      else if (aArgs.size() == 1)
      {
         if (aType->IsCloneable())
         {
            // The argument must be of the same type as the requested type (or at least in the same
            // class heirarchy). The call to ImplicitCast will give an error if the cast is not possible.
            //
            // NOTE: This wasn't here before and strange things resulted. For instance:
            //
            //    Vec3 wcsLoc = ... something to create/populate a Vec3
            //    WsfGeoPoint pt = WsfGeoPoint(wcsLoc);
            //
            // No compile-time error was detected and the run-time results were very strange (basically,
            // 'pt' became a reference to a Vec3). Users ran into some very strange problems and hard-to-
            // diagnose problems.
            UsVal valToClone(ImplicitCast(aArgs.front(), aType));
            valToClone      = TempStack(valToClone, true);
            UsVal clonedVal = UsVal::Temporary(aType, StackAlloc());
            Instr() << UtScript::Opcode::CLONE_VAR;
            TagStoreLoc() << clonedVal.GetStackIndex() << valToClone.GetStackIndex();
            FreeIfTemporary(valToClone);
            return clonedVal;
         }
         SemErr("Type is not cloneable");
      }
      else
      {
         SemErr("Looks like a copy constructor, but has more than one parameter.");
      }
   }
   return UsVal();
}

UsVal UsCtx::Cast(UsVal& v, UtScriptClass* ty)
{
   return ExplicitCast(v, ty);
}

UsVal UsCtx::CreateData(bool aData)
{
   UsVal v;
   v.mValue = new UtScriptData(aData);
   v.mType  = mConst->tyBool;
   return v;
}
UsVal UsCtx::CreateData(int aData)
{
   UsVal v;
   v.mValue = new UtScriptData(aData);
   v.mType  = mConst->tyInt;
   return v;
}
UsVal UsCtx::CreateData(double aData)
{
   UsVal v;
   v.mValue = new UtScriptData(aData);
   v.mType  = mConst->tyDouble;
   return v;
}

// using a special dedicated dictionary for script string literals as they will never need to be
// looked up again, meaning it would only pollute the global dictionary.
static UtDictionary sScriptLiteralsDict;

UsVal UsCtx::CreateData(const std::string& aData)
{
   UsVal v;

   // archive the string literal into the sScriptLiteralsDict dictionary, and get the string thats stored in the dictionary.
   auto               id  = sScriptLiteralsDict.GetNumber(aData);
   const std::string& str = sScriptLiteralsDict.GetString(id);
   v.mValue               = new UtScriptData();
   v.mValue->SetExternallyManagedString(str);
   v.mType = mConst->tyString;

   return v;
}

UsVal UsCtx::StringLiteral(UtScriptLanguage::Token* t)
{
   std::string str(t->val + 1, t->val + t->len - 1);
   std::string unescapedStr;
   bool        hasSlash = false;
   for (char c : str)
   {
      if (!hasSlash)
      {
         if (c == '\\')
         {
            hasSlash = true;
         }
         else
         {
            unescapedStr += c;
         }
      }
      else
      {
         switch (c)
         {
         case 'a':
            unescapedStr += '\a';
            break;
         case 'b':
            unescapedStr += '\b';
            break;
         case 'r':
            unescapedStr += '\r';
            break;
         case 'f':
            unescapedStr += '\f';
            break;
         case 'n':
            unescapedStr += '\n';
            break;
         case 't':
            unescapedStr += '\t';
            break;
         case 'v':
            unescapedStr += '\v';
            break;
         case '\'':
         case '"':
         case '\\':
            unescapedStr += c;
            break;
         }
         hasSlash = false;
      }
   }
   return CreateData(unescapedStr);
}

UsVal UsCtx::DoubleLiteral(UtScriptLanguage::Token* t)
{
   return CreateData(atof(t->val));
}

UsVal UsCtx::IntLiteral(UtScriptLanguage::Token* t)
{
   return CreateData(atoi(t->val));
}

UsVal UsCtx::BoolLiteral(UtScriptLanguage::Token* t)
{
   return CreateData(t->kind == UtScriptLanguage::Parser::_true);
}

UsVal UsCtx::NullLiteral(UtScriptLanguage::Token* t)
{
   UsVal v;
   v.mType = mConst->tyNull;
   return v;
}

std::string UsCtx::Text(UtScriptLanguage::Token* t)
{
   std::string s;
   s.assign(t->val, t->val + t->len);
   return s;
}

UtScriptClass* UsCtx::FuncRType()
{
   const UtScriptFunction* protoPtr = mConst->mScriptVal.mPrototypePtr;
   if (protoPtr != nullptr)
   {
      return Env()->GetClass(protoPtr->mReturnType);
   }
   return nullptr;
}

UsVal UsCtx::Search(UtScriptLanguage::Token* aIdent)
{
   std::string name = Text(aIdent);
   if (name == "{")
   {
      return UsVal();
   }
   // ERR_CHECK
   UsVal v = Search(name);
   if (v.IsValid())
      return v;

   UsCtx* scriptCtxPtr = ScriptContext();
   if (scriptCtxPtr != nullptr)
   {
      UtStringId methodNameId = aIdent->val;
      UsVal      thisVal      = scriptCtxPtr->Search("this");
      if (thisVal.IsValid())
      {
         std::vector<UtScriptClass::InterfaceMethod*> methods;

         thisVal.Type(*mConst)->FindMethods(methodNameId, methods);
         if (!methods.empty())
         {
            return UsVal::Method(thisVal, methodNameId);
         }
      }
      UtScriptClass* builtinClassPtr = scriptCtxPtr->Types()->GetClassFromCharArray("__BUILTIN__");
      if ((builtinClassPtr != nullptr) && builtinClassPtr->HasStaticMethod(methodNameId))
      {
         UsVal m         = mConst->ValFromType(builtinClassPtr);
         m.mMethodNameId = methodNameId;
         return m;
      }
   }

   return v;
}

UsVal UsCtx::Search(const std::string& aName)
{
   UsCtx* ctxPtr = this;
   while (ctxPtr != nullptr)
   {
      auto i = ctxPtr->mLocalSymbols.find(aName);
      if (i != ctxPtr->mLocalSymbols.end())
      {
         return i->second;
      }
      ctxPtr = ctxPtr->mParentCtx;
   }
   UtScriptScope::InstanceVariable* varDefPtr = Scope().GetInstanceVariableDef(aName);
   if (varDefPtr == nullptr)
   {
      UtScriptContext* gInstPtr = Env()->GetGlobalInstancePrototype();
      if (gInstPtr != nullptr)
      {
         varDefPtr = gInstPtr->GetScope()->GetInstanceVariableDef(aName);
      }
   }
   if (varDefPtr != nullptr)
   {
      UsVal varVal = UsVal::GlobalVar(aName, varDefPtr->mType);
      if (varDefPtr->mIsReadOnlyFunction)
      {
         varVal.mFlags |= UsVal::cREAD_ONLY_FUNCTION_VARIABLE;
      }
      return varVal;
   }
   UtStringId              nameId   = aName;
   const UtScriptFunction* protoPtr = Scope().GetScriptPrototype(nameId);
   if (protoPtr != nullptr)
   {
      return UsVal::Script(aName, nameId, protoPtr);
   }
   return UsVal();
}

UsVal UsCtx::AddEntry(const std::string& aIdent, int aIdentPos, UtScriptFunction& aFunc, bool aExtern, UsCtx* aFnCtx)
{
   std::string identStr;
   UtStringId  ident;
   if (!aExtern && !mConst->mNamespace.empty())
   {
      identStr = mConst->mNamespace + "::" + aIdent;
      ident    = identStr;
   }
   else
   {
      identStr = aIdent;
      ident    = aIdent;
   }

   int                       protoid     = Env()->AddPrototype(aFunc);
   UtScriptScope::ScriptRef* existingPtr = Scope().FindScript(ident, true);

   // Check that a script isn't being defined differently.
   if (((existingPtr != nullptr) && aExtern) || ((existingPtr != nullptr) && !existingPtr->mIsLocal))
   {
      if (!((*existingPtr->mPrototypePtr) == aFunc))
      {
         std::stringstream ss;
         ss << "Signature differs from previous definition: "
            << Env()->GetPrototypeDescription(existingPtr->mPrototypePtr, ident);
         SemErr(ss.str().c_str());
      }
      else
      {
         existingPtr = nullptr;
      }
   }

   if (existingPtr == nullptr)
   {
      Scope().AddScript(ident, Env()->GetPrototype(protoid));
      return UsVal::Script(identStr, ident, Env()->GetPrototype(protoid));
   }

   else
   {
      std::stringstream ss;
      ss << "Script already exists with the name " << ident;
      SemErr(ss.str().c_str());
      return UsVal();
   }
}
UsVal UsCtx::AddEntry(UtScriptLanguage::Token* aIdent, UtScriptFunction& aFunc, bool aExtern, UsCtx* aFnCtx /*=0*/)
{
   return AddEntry(Text(aIdent), aIdent->pos, aFunc, aExtern, aFnCtx);
}

void UsCtx::FuncDefStart(UsVal& aScriptVal)
{
   SetBlockType(cSCRIPT_BLOCK);
   mConst->mScriptVal         = aScriptVal;
   const UtScriptFunction* fn = aScriptVal.mPrototypePtr;
   // UtScript* scriptPtr = new UtScript(mFuncDefSym->mName, fn);
   if (!Scope().AddScript(aScriptVal.GetValNameId(), fn))
   {
      SemErr("Cannot redefine script");
   }
   // The caller will push args in order, mark symbols with stack indices now
   for (const auto& argName : mArgNames)
   {
      UsVal argVal = Search(argName);
      assert(argVal.IsValid());
      if (argVal.IsValid())
      {
         argVal.mStackIndex = StackAlloc();
      }
   }
}

void UsCtx::SetRType(UtScriptFunction& fn, UtScriptClass* type)
{
   if (type != nullptr)
   {
      fn.mReturnType = type->GetClassName();
   }
}

void UsCtx::AddImplicitParam(Ty* type, const std::string& aIdent)
{
   UsVal newVar                   = UsVal::LocalVariable(aIdent, type, StackAlloc(false));
   newVar.mValidAfterInstruction  = CodeOffset();
   mLocalSymbols[newVar.mValName] = newVar;
}

void UsCtx::AddParam(UtScriptFunction& fn, UtScriptClass* type, UtScriptLanguage::Token* aIdent, bool aIsFormal)
{
   if (type != nullptr)
   {
      fn.mArgs.emplace_back(type->GetClassName());
   }
   if (aIsFormal)
   {
      mArgNames.emplace_back(aIdent->val);
      VarDecl(type, aIdent, UsVal::cLOCAL_VARIABLE);
   }
}

UsCtx::~UsCtx()
{
   EndContext();
}

void UsCtx::EndContext()
{
   if (mBlockEnded)
      return;
   if (mBlockType > cNORMAL_BLOCK)
   {
      if (mBlockType == cWHILE_BLOCK || mBlockType == cFOR_BLOCK || mBlockType == cFOREACH_BLOCK)
      {
         Continue();
      }

      for (offset_t ji : mBreakJumps)
      {
         assert(Code().mOps[ji] == ut::script::npos);
         Code().mOps[ji] = CodeOffset();
      }
      mBreakJumps.clear();
      if (mContinueJumpOffset != ut::script::npos)
      {
         for (offset_t ji : mContinueJumps)
         {
            assert(Code().mOps[ji] == ut::script::npos);
            Code().mOps[ji] = mContinueJumpOffset;
         }
      }
      mContinueJumps.clear();
   }

   if (mConst->mScriptPtr != nullptr)
   {
      // Store local variable information in the script (for debugging)
      for (const auto& localSymbol : mLocalSymbols)
      {
         UtScript::LocalVariable v;
         v.mName = localSymbol.first;
         if (localSymbol.second.mStaticVarIndex != ut::script::npos)
         {
            v.mStackIndex = -static_cast<ptrdiff_t>(localSymbol.second.mStaticVarIndex) - 1;
         }
         else
         {
            v.mStackIndex = localSymbol.second.GetStackIndex();
         }
         v.mValidAfter  = localSymbol.second.mValidAfterInstruction;
         v.mValidBefore = CodeOffset();
         v.mTypePtr     = localSymbol.second.Type(*mConst);
         Code().mLocalVariables.push_back(v);
      }
   }

   if (mParentCtx != nullptr)
   {
      mParentCtx->mNextStaticVarIndex = mNextStaticVarIndex;
      // Child scope, for now we DO want to increment automatic var index
      // A simple optimization routine can reduce stack usage later
      if (mBlockType != cSCRIPT_BLOCK)
      {
         assert(mNextAutomaticVarIndex >= mParentCtx->mNextAutomaticVarIndex);
         mParentCtx->mNextAutomaticVarIndex = mNextAutomaticVarIndex;
      }
      mParentCtx->mEmptyBlock = mParentCtx->mEmptyBlock && mEmptyBlock;
      if (mBlockType == cNORMAL_BLOCK)
      {
         if (mParentCtx->mBlockType <= cNORMAL_BLOCK && !mParentCtx->mIfBlockParent && !mParentCtx->mIfBlock)
         {
            // This block was a normal scoping block, unrelated to any control flow.
            // e.g `int a; {int b; use(b); } use(a);`
            mParentCtx->mAllCodePathsReturn = mAllCodePathsReturn && !mEmptyBlock;
         }
         else
         {
            mParentCtx->mAllCodePathsReturn = mParentCtx->mAllCodePathsReturn && mAllCodePathsReturn && !mEmptyBlock;
         }
      }
      else
      {
         mParentCtx->mAllCodePathsReturn = false;
      }
      mParentCtx = nullptr;
   }
   mBlockEnded = true;
}

void UsCtx::InitP()
{
   mBlockEnded         = false;
   mEmptyBlock         = true;
   mBlockHasReturned   = false;
   mAllCodePathsReturn = false;
   mIfBlockParent      = false;
   mIfBlock            = false;

   mWhileStartInstruction = ut::script::npos;
   mBlockType             = cNORMAL_BLOCK;
   mContinueJumpOffset    = ut::script::npos;
   mRegisteredScriptPtr   = nullptr;
   mForCondOffset         = ut::script::npos;
   mForCondJump           = ut::script::npos;
   mStaticVariableJump    = ut::script::npos;
}

UsCtx::UsCtx(UsCtx& p)
   : mParentCtx(&p)
   , mNextAutomaticVarIndex(p.mNextAutomaticVarIndex)
   , mNextStaticVarIndex(p.mNextStaticVarIndex)
{
   InitP();
   mConst              = p.mConst;
   mAllCodePathsReturn = p.mAllCodePathsReturn;
   mIfBlock            = p.mIfBlockParent; // true if this block is the ... of `if(b) { ... }`
}

UsCtx::UsCtx(UsConstCtx* c)
   : mParentCtx(nullptr)
   , mNextAutomaticVarIndex(0)
   , mNextStaticVarIndex(0)
{
   InitP();
   mConst = c;
}

int UsCtx::StringId(const std::string& aStr)
{
   return mConst->mTypes->GetDictionary().GetNumber(aStr);
}

void UsCtx::VarDeclPreAssign(UsVal& v)
{
   if ((v.mFlags & UsVal::cSTATIC_VARIABLE) != 0)
   {
      // If a static variable is being assigned to, only execute the first time through
      Instr() << UtScript::Opcode::JUMP_IF_NOT_FIRST;
      mStaticVariableJump = CodeOffset();
      Code() << ut::script::npos << static_cast<script_code_t>(1U);
   }
}

std::string UsVal::TypeName(const UsConstCtx& aCtx)
{
   if (Type(aCtx) != nullptr)
   {
      return Type(aCtx)->GetClassName();
   }
   return "No Type";
}

Ty* UsVal::Type(const UsConstCtx& aCtx) const
{
   // The type of a struct variable depends on how it was accessed.  If it was accessed with '.' then mType is
   // the type of variable. If accessed with '->' then it is the 'variable type' (i.e.: unknown until run-time.
   Ty* typePtr = mType; // Assume the type in the class.
   if ((mStructVarName >= 0) && ((mFlags & cSTRUCT_VARIABLE) == 0))
   {
      typePtr = aCtx.tyVar; // Use the 'variable' type (determined at run-time).
   }
   return typePtr;
}

UsInitializerList::UsInitializerList()
   : mRefCount(new UtReferenceCount(0))
{
   mDataPtr = new UsInitializerListData;
}

UsInitializerList::~UsInitializerList()
{
   delete mDataPtr;
}

void UsCtx::FuncComplete(UsVal aFnVal, UsCtx& aFnCtx)
{
   const UtScriptFunction* fnDef = aFnVal.mPrototypePtr;
   if (fnDef && fnDef->mReturnClassPtr != mConst->tyVoid)
   {
      if (!aFnCtx.mAllCodePathsReturn)
      {
         // consider changing to a warning instead of an error.
         SemErr(("missing return statement. end of " + aFnVal.mValName +
                 " script reached with possibility of failing to return a " + FuncRType()->GetClassName())
                   .c_str());
      }
   }

   Instr() << UtScript::Opcode::END;

   aFnCtx.EndContext();
   Code().mName          = aFnVal.mValName;
   Code().mPrototype     = fnDef;
   Code().mArgumentNames = aFnCtx.mArgNames;
   Code().mStackSize     = aFnCtx.mNextAutomaticVarIndex;
   Code().mStaticVariables.resize(aFnCtx.mNextStaticVarIndex);
   Code().mIsVarScript    = mConst->mGlobalVarMode;
   Code().mSourceFilePath = mConst->mDocumentPath;
   // Give ownership of script to scope
   mRegisteredScriptPtr = mConst->mScriptPtr;
   mConst->mScriptPtr   = new UtScript;

   Scope().ImplementScript(mRegisteredScriptPtr);
}

script_code_t UsCtx::DataIndex(const UtScriptData& aData)
{
   return Code().GetDataIndex(aData);
}

void UsCtx::Return(UsVal& v)
{
   UtScriptClass* rtype = FuncRType();
   if ((rtype != nullptr) && !rtype->IsVoid())
   {
      if (!v.IsValid())
      {
         SemErr("Script with non-void return type must return a value");
      }

      UsVal rval      = TempStack(ImplicitCast(v, rtype), true);
      auto  rvalIndex = static_cast<script_code_t>(rval.GetStackIndex());
      if (rvalIndex < cMAX_STACK_INDEX)
      {
         Instr() << Opcode::RETURN_FUNCTION_SP << rvalIndex;
         FreeIfTemporary(rval);
      }
      else
      {
         Instr() << Opcode::RETURN_FUNCTION_DP << (rvalIndex - cMAX_STACK_INDEX);
      }
   }
   else
   {
      if (v.IsValid())
      {
         SemErr("'Void' script cannot return a value");
      }
      Instr() << UtScript::Opcode::END;
   }
   mBlockHasReturned   = true;
   mAllCodePathsReturn = true;
}

offset_t UsCtx::CodeOffset()
{
   return static_cast<offset_t>(Code().mOps.size());
}

int UsCtx::OrCheck(UsVal& aLhs, UsVal& result)
{
   UsVal lhsVal = TempStack(aLhs);

   if (lhsVal.mType != mConst->tyBool || (lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
   {
      result.mType       = mConst->tyBool;
      result.mStackIndex = StackAlloc();
      Instr() << UtScript::Opcode::IS_TRUE;
      TagStoreLoc() << result.mStackIndex << lhsVal.GetStackIndex();
      FreeIfTemporary(lhsVal);
   }
   else
   {
      result = lhsVal;
   }
   // For short-circuit evaluation, add a jump.
   Instr() << UtScript::Opcode::JUMP_IF_TRUE;
   int instrOffset = static_cast<int>(CodeOffset());
   Code() << ut::script::npos; // Fill in the jump position later
   Code() << result.GetStackIndex();
   return instrOffset;
}

UsVal UsCtx::OrComplete(UsVal result, UsVal rhs, int aJumpOffset)
{
   rhs = TempStack(rhs);
   Instr() << UtScript::Opcode::IS_TRUE << result.mStackIndex << rhs.GetStackIndex();
   // can't do implicit temp write directly to local, as the short-circuit logic means
   // multiple instruction locations could be the latest write to the temporary, and
   // current implementation does support rewriting the store location of multiple code points.
   mLastStoreCodeLoc = ut::script::npos;
   assert(Code().mOps[aJumpOffset] == ut::script::npos);
   Code().mOps[aJumpOffset] = CodeOffset();
   if (rhs.GetStackIndex() != result.GetStackIndex())
   {
      FreeIfTemporary(rhs);
   }
   return result;
}

int UsCtx::AndCheck(UsVal& aLhs, UsVal& result)
{
   UsVal lhsVal = TempStack(aLhs);
   if (lhsVal.mType != mConst->tyBool || (lhsVal.mFlags & UsVal::cTEMPORARY_VARIABLE) == 0)
   {
      result             = UsVal();
      result.mType       = mConst->tyBool;
      result.mStackIndex = StackAlloc();

      Instr() << UtScript::Opcode::IS_TRUE;
      TagStoreLoc() << result.mStackIndex << lhsVal.GetStackIndex();
      FreeIfTemporary(lhsVal);
   }
   else
   {
      result = lhsVal;
   }
   // For short-circuit evaluation, add a jump.
   Instr() << UtScript::Opcode::JUMP_IF_FALSE;
   int instrOffset = static_cast<int>(CodeOffset());
   Code() << ut::script::npos; // Fill in the jump position later
   Code() << result.GetStackIndex();
   return instrOffset;
}

UsVal UsCtx::AndComplete(UsVal result, UsVal rhs, int aJumpOffset)
{
   rhs = TempStack(rhs);
   Instr() << UtScript::Opcode::IS_TRUE << result.mStackIndex << rhs.GetStackIndex();
   // can't do implicit temp write directly to local, as the short-circuit logic means
   // multiple instruction locations could be the latest write to the temporary, and
   // current implementation does support rewriting the store location of multiple code points.
   mLastStoreCodeLoc = ut::script::npos;
   assert(Code().mOps[aJumpOffset] == ut::script::npos);
   Code().mOps[aJumpOffset] = CodeOffset();
   if (rhs.GetStackIndex() != result.GetStackIndex())
   {
      FreeIfTemporary(rhs);
   }
   return result;
}

UtScriptClass* UsCtx::GetReturnClass(UtScriptClass::InterfaceMethod* aMethodPtr)
{
   UtScriptClass* rClass = aMethodPtr->GetReturnClass();
   if (rClass == nullptr)
   {
      rClass = mConst->tyVoid;
   }
   return rClass;
}

int UsCtx::IfCond(UsVal& v)
{
   UsVal condVal = TempStack(v);
   Instr() << UtScript::Opcode::JUMP_IF_FALSE;
   int jumpOffset = static_cast<int>(CodeOffset());
   Code() << ut::script::npos << condVal.GetStackIndex();
   FreeIfTemporary(condVal);
   mAllCodePathsReturn = true;
   mIfBlockParent      = true;
   return jumpOffset;
}

int UsCtx::IfTrueEnd(int falseTarget, bool aHasElse)
{
   int elseEnd = -1;
   // Jump past 'else'
   if (aHasElse)
   {
      bool allReturnRestore = mAllCodePathsReturn;
      mAllCodePathsReturn   = false;
      Instr() << UtScript::Opcode::JUMP;
      elseEnd = static_cast<int>(CodeOffset());
      Code() << ut::script::npos;
      mAllCodePathsReturn = allReturnRestore;
   }
   else
   {
      mAllCodePathsReturn = false;
      mIfBlockParent      = false;
   }
   // fill in jump location for case of 'if (false)'
   assert(Code().mOps[falseTarget] == ut::script::npos);
   Code().mOps[falseTarget] = CodeOffset();

   return elseEnd;
}

void UsCtx::IfElseEnd(int aElseEnd)
{
   // fill in jump location for case of 'if (true) ... jump past else'
   assert(Code().mOps[aElseEnd] == ut::script::npos);
   Code().mOps[aElseEnd] = CodeOffset();
   mIfBlockParent        = false;
}

void UsCtx::WhileCond(UsVal& v)
{
   assert(mBlockType == cWHILE_BLOCK);

   UsVal cond = ToStack(v);
   Instr() << UtScript::Opcode::JUMP_IF_FALSE;
   mBreakJumps.push_back(CodeOffset());
   Code() << ut::script::npos << cond.GetStackIndex();
   FreeIfTemporary(cond);
}

void UsCtx::Break()
{
   UsCtx* breakCtx = GetBreakableBlock();
   if (breakCtx != nullptr)
   {
      Instr() << UtScript::Opcode::JUMP;
      breakCtx->mBreakJumps.push_back(CodeOffset());
      Code() << ut::script::npos;
   }
   else
   {
      SemErr("'break' statement not allowed here");
   }
}

void UsCtx::Continue()
{
   UsCtx* breakCtx = GetBreakableBlock();
   if ((breakCtx != nullptr) && breakCtx->mContinueJumpOffset != ut::script::npos)
   {
      Instr() << UtScript::Opcode::JUMP;
      breakCtx->mContinueJumps.push_back(CodeOffset());
      Code() << ut::script::npos;
   }
   else
   {
      SemErr("'continue' statement not allowed here");
   }
}

UsCtx* UsCtx::GetBreakableBlock()
{
   switch (mBlockType)
   {
   case cWHILE_BLOCK:
   case cDO_BLOCK:
   case cFOR_BLOCK:
   case cFOREACH_BLOCK:
      return this;
   default:
      break;
   }
   if (mParentCtx != nullptr)
      return mParentCtx->GetBreakableBlock();
   return nullptr;
}

void UsCtx::SetBlockType(BlockType aBlockType)
{
   mBlockType = aBlockType;
   if (mBlockType == cWHILE_BLOCK)
   {
      mContinueJumpOffset = CodeOffset();
   }
   else if (mBlockType == cDO_BLOCK)
   {
      mWhileStartInstruction = CodeOffset();
   }
   else if (mBlockType == cFOR_BLOCK)
   {
      mForCondOffset = CodeOffset();
      // mContinueJumpOffset = CodeOffset();
   }
}

void UsCtx::DoWhileStart()
{
   assert(mBlockType == cDO_BLOCK);
   mContinueJumpOffset = CodeOffset();
}

void UsCtx::DoCond(UsVal& v)
{
   assert(mWhileStartInstruction != ut::script::npos);
   UsVal cond = ToStack(v);
   Instr() << UtScript::Opcode::JUMP_IF_TRUE << mWhileStartInstruction << cond.GetStackIndex();
   FreeIfTemporary(cond);
}

void UsCtx::ForCond(UsVal& v)
{
   assert(mForCondOffset != ut::script::npos);
   UsVal cond = ToStack(v);
   Instr() << UtScript::Opcode::JUMP_IF_TRUE;
   mForCondJump = CodeOffset();
   Code() << ut::script::npos << cond.GetStackIndex();
   Break();
   mContinueJumpOffset = CodeOffset(); // Continue directly before the increment
   FreeIfTemporary(cond);
}

void UsCtx::ForInc()
{
   assert(mForCondOffset != ut::script::npos);
   Instr() << UtScript::Opcode::JUMP << mForCondOffset;
   assert(Code().mOps[mForCondJump] == ut::script::npos);
   Code().mOps[mForCondJump] = CodeOffset();
}

int UsCtx::NameId(const std::string& aName)
{
   int& id = mConst->mNameListMap[aName];
   if (id == 0)
   {
      id = mConst->mNextNameId++;
      Code().mNameList.push_back(aName);
   }

   return id;
}

UsVal UsCtx::Identifier(UtScriptLanguage::Token* t)
{
   UsVal v = Search(t);

   if (!v.IsValid())
   {
      std::stringstream ss;
      ss << "Unknown identifier: '" << t->val << "'";
      SemErr(ss.str().c_str());
   }
   return v;
}

UsCtx* UsCtx::ScriptContext()
{
   UsCtx* c = this;
   while (c != nullptr)
   {
      if (c->mBlockType == cSCRIPT_BLOCK)
         return c;
      c = c->mParentCtx;
   }
   return c;
}

UtScript& UsCtx::Instr()
{
   if (mConst->mParser->t != nullptr)
   {
      UtScript::SourceInstructionOffset sio;
      sio.mInstructionIndex = CodeOffset();
      sio.mSourceOffset     = mConst->mParser->t->pos + mConst->mDocumentOffset;
      sio.mSourceLine =
         mConst->mParser->t->line + mConst->mDocumentLineOffset - 1; // -1 because UtTextDocument starts at line 0
      Code().mSourceOffsetToInstruction.push_back(sio);
   }
   if (mBlockHasReturned || (!mEmptyBlock && mAllCodePathsReturn))
   {
      // may want to change this to a warning instead of an error.
      SemErr("unreachable code after return statement");
      // ensure the error only gets printed once for the code after the return.
      mBlockHasReturned = false;
   }
   // block is no longer empty since an instruction is being emitted.
   mEmptyBlock         = false;
   mAllCodePathsReturn = false;
   return Code();
}

void UsCtx::ForEachBegin(UsVal& key, UsVal& val, UsVal& aContainer)
{
   UsVal cont = aContainer;
   if (cont.mInitList)
   {
      cont = RealizeContainer(cont.mInitList.Get(), nullptr);
   }
   if ((ValType(cont) == nullptr) || !ValType(cont)->IsContainer())
   {
      SemErr("Cannot use foreach on a non-container type");
      return;
   }
   bool hasKey = true;
   if (val.Type(*mConst) == nullptr)
   {
      val    = key;
      hasKey = false;
   }
   if (hasKey)
   {
      UtStringId keyType = ValType(cont)->GetContainerKeyTypeId();
      if ((key.Type(*mConst) == nullptr) ||
          !(key.Type(*mConst)->IsOfType(keyType) || key.Type(*mConst)->CanImplicitCast(keyType)))
      {
         SemErr("foreach key does not match container");
         return;
      }
   }
   UtScriptClass* containerDataTypePtr = ValType(cont)->GetContainerDataType();
   if ((val.Type(*mConst) == nullptr) || !(containerDataTypePtr->IsOfType(val.Type(*mConst)->GetClassName()) ||
                                           containerDataTypePtr->CanImplicitCast(val.Type(*mConst)->GetClassName())))
   {
      SemErr("foreach value type does not match container");
      return;
   }
   SetBlockType(cFOREACH_BLOCK);

   cont = ToStack(cont);

   // Even if its a temporary, we can't early free this, and must keep it around
   // for the lifetime of the FOREACH_BLOCK, which can be accomplished by overwriting
   // the TEMPORARY_VARIABLE flag with LOCAL_VARIABLE.
   cont.mFlags = UsVal::cLOCAL_VARIABLE;

   std::vector<UtScriptClass::InterfaceMethod*> methods;
   ValType(cont)->FindMethods(UtStringIdLiteral("GetIterator"), methods);
   if (methods.empty())
      return;
   UsVal getIteratorFn         = cont;
   getIteratorFn.mMethodNameId = UtStringIdLiteral("GetIterator");
   UsVal iterVal               = Call(getIteratorFn, UsValList());

   // Even if its a temporary, we can't early free this, and must keep it around
   // for the lifetime of the FOREACH_BLOCK, which can be accomplished by overwriting
   // the TEMPORARY_VARIABLE flag with LOCAL_VARIABLE.
   iterVal.mFlags = UsVal::cLOCAL_VARIABLE;

   UsVal iterHasNext         = iterVal;
   iterHasNext.mMethodNameId = UtStringIdLiteral("HasNext");
   UsVal iterNext            = iterVal;
   iterNext.mMethodNameId    = UtStringIdLiteral("Next");
   UsVal iterKey             = iterVal;
   iterKey.mMethodNameId     = UtStringIdLiteral("Key");

   mContinueJumpOffset = CodeOffset();
   UsVal cond          = Call(iterHasNext, UsValList());

   Instr() << UtScript::Opcode::JUMP_IF_FALSE;
   mBreakJumps.push_back(CodeOffset());
   Code() << ut::script::npos << cond.GetStackIndex();

   Call(iterNext, UsValList(), &val);
   if (hasKey)
   {
      Call(iterKey, UsValList(), &key);
   }
}

// Called at the start of a statement
int UsCtx::BeginStat()
{
   static int sStatId = 1;
   int        id      = ++sStatId;
   if (sStatId < 0)
   {
      sStatId = 1;
   }
   UsCtx* sc = ScriptContext();
   sc->mCurrentStatements.push_back(id);
   sc->mCurTempSlots.push_back(-id);
   sc->mParentTempSlots.push_back(-id);
   return sc->mCurrentStatements.back();
}

// Called at the end of a statement to trigger the clean up of temporaries
void UsCtx::EndStat(int aStatNum)
{
   UsCtx* sc = ScriptContext();
   while (sc->mCurrentStatements.back() != aStatNum)
   {
      EndStat(sc->mCurrentStatements.back());
   }

   assert(!sc->mCurrentStatements.empty());

   auto& curTemps = sc->mCurTempSlots;

   while (curTemps.back() != -aStatNum)
   {
      if (curTemps.back() != cERASED_TEMP_SLOT)
      {
         sc->mFreeStackSlots.push_back(curTemps.back());
      }
      curTemps.pop_back();
   }
   curTemps.pop_back();

   auto& parentTemps = sc->mParentTempSlots;
   while (parentTemps.back() != -aStatNum)
   {
      if (parentTemps.back() != cERASED_TEMP_SLOT)
      {
         curTemps.push_back(parentTemps.back());
      }
      parentTemps.pop_back();
   }
   parentTemps.pop_back();

   sc->mCurrentStatements.pop_back();
   mLastStoreCodeLoc = ut::script::npos;
}

// Allocate a stack slot
ptrdiff_t UsCtx::StackAlloc(bool aTempSlot /* = true*/)
{
   UsCtx*    sc = ScriptContext();
   ptrdiff_t a  = mNextAutomaticVarIndex;
   if (sc->mFreeStackSlots.empty())
   {
      if (static_cast<script_code_t>(a) >= cMAX_STACK_INDEX)
      {
         SemErr("Script engine doesn't allocate enough stack space to handle all the variables in this function. "
                "Try breaking the function up into smaller pieces, or reduce the scope of variables.");
      }
      ++mNextAutomaticVarIndex;
   }
   else
   {
      a = sc->mFreeStackSlots.back();
      sc->mFreeStackSlots.pop_back();
   }
   assert(a >= 0 && static_cast<script_code_t>(a) < cMAX_STACK_INDEX);

   if (aTempSlot)
   {
      sc->mCurTempSlots.push_back(a);
   }
   else
   {
      sc->mParentTempSlots.push_back(a);
   }
   return a;
}

Ty* UsCtx::Type(Token* t)
{
   Ty* ty = qType(t);
   if (ty == nullptr)
   {
      std::stringstream ss;
      ss << "Invalid Type: '" << t->val << "'";
      SemErr(ss.str().c_str());
   }
   return ty;
}

UtScript& UsCtx::TagStoreLoc()
{
   mLastStoreCodeLoc = CodeOffset();
   mNoRecycleLastIdx = static_cast<ptrdiff_t>(ut::script::npos);
   return Code();
}

} // end namespace UtScriptLanguage
