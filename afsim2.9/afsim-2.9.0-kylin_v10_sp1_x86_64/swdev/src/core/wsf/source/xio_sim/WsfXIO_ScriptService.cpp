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

#include "xio_sim/WsfXIO_ScriptService.hpp"

#include <sstream>

#include "UtScriptBasicTypes.hpp"
#include "UtScriptDebugger.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptMethod.hpp"
#include "UtScriptScope.hpp"
#include "WsfApplication.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

WsfXIO_ScriptService::WsfXIO_ScriptService(WsfXIO_Simulation& aXIO_Simulation)
   : BaseClassType(aXIO_Simulation.GetInterface())
   , mSimulation(aXIO_Simulation.GetSimulation())
   , mScriptEnvironmentPtr(mSimulation.GetScriptExecutor().GetScriptEnvironment())
   , mDebugEnumerateGobalsScript(nullptr)
{
   mInitializedEnumerateScripts = false;
   mNextExpressionId            = 1;
   mCallbacks += GetInterface()->Connect(&WsfXIO_ScriptService::HandleInformationRequest, this);
   mCallbacks += GetInterface()->Connect(&WsfXIO_ScriptService::HandleScriptDataRequest, this);
   mCallbacks += GetInterface()->Connect(&WsfXIO_ScriptService::HandleExecute, this);
   mCallbacks += GetInterface()->Connect(&WsfXIO_ScriptService::HandleScriptCommand, this);
   mCallbacks += GetInterface()->Connect(&WsfXIO_ScriptService::HandleRequest, this);
   WsfXIO_SimService::Initialize(aXIO_Simulation.GetSimulation());
}

void WsfXIO_ScriptService::HandleInformationRequest(WsfXIO_InformationRequestPkt& aPkt)
{
   WsfXIO_Connection* senderPtr = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
   if (senderPtr == nullptr)
   {
      return;
   }
   WsfXIO_ScriptListPkt pkt;
   if (aPkt.mType == WsfXIO_InformationRequestPkt::cSCRIPT_LIST)
   {
      // Send Globals if mPlatformIndex == 0
      if (aPkt.mPlatformIndex == 0)
      {
         WsfScriptContext& context = mSimulation.GetScriptContext();
         StoreAllScripts(nullptr, &context.GetContext(), pkt);
         senderPtr->Send(pkt);
      }
      else
      {
         WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
         if (platformPtr != nullptr)
         {
            pkt.mPlatformIndex = static_cast<int32_t>(platformPtr->GetIndex());
            StoreAllScripts(nullptr, &platformPtr->GetScriptContext().GetContext(), pkt);
            for (WsfComponentList::RoleIterator<WsfProcessor> iter(*platformPtr); !iter.AtEnd(); ++iter)
            {
               WsfProcessor*       procPtr       = *iter;
               WsfScriptProcessor* scriptProcPtr = dynamic_cast<WsfScriptProcessor*>(procPtr);
               if (scriptProcPtr != nullptr)
               {
                  StoreAllScripts(procPtr, &scriptProcPtr->GetScriptContext().GetContext(), pkt);
               }
            }
            senderPtr->Send(pkt);
         }
      }
   }
}

//! Stores a script to a packet.
void WsfXIO_ScriptService::StoreScript(WsfPlatformPart* aPartPtr, UtScript* aScript, WsfXIO_ScriptListPkt& aPkt)
{
   WsfXIO_ScriptListPkt::Script script;

   UtScriptTypes* typesPtr = &WsfScriptManager::GetTypes();
   for (unsigned i = 0; i < aScript->mPrototype->mArgs.size(); ++i)
   {
      WsfXIO_ScriptListPkt::Argument arg;
      UtScriptClass*                 argTypePtr = typesPtr->GetClass(aScript->mPrototype->mArgs[i]);
      if (argTypePtr != nullptr)
      {
#undef GetClassName
         arg.mArgumentType = argTypePtr->GetClassName();
         arg.mArgumentName = aScript->GetArgumentName(i);
      }
      script.mArguments.push_back(arg);
   }
   script.mName = aScript->GetName();
   if (aPartPtr != nullptr)
   {
      script.mPartName = aPartPtr->GetName();
      script.mPartType = aPartPtr->GetPartType();
   }
   aPkt.mScripts.push_back(script);
}

void WsfXIO_ScriptService::StoreAllScripts(WsfPlatformPart* aPartPtr, UtScriptContext* aInstancePtr, WsfXIO_ScriptListPkt& aPkt)
{
   UtScriptScope*                  scopePtr = aInstancePtr->GetScope();
   const UtScriptScope::ScriptMap& scripts  = scopePtr->GetScripts();
   for (const auto& script : scripts)
   {
      if (script.second.mIsLocal)
      {
         StoreScript(nullptr, script.second.mScriptPtr, aPkt);
      }
   }
}

namespace
{
//! Searches a vector of scripts for a script with a specified ID
// UtScript*  FindScript(int                            aScriptId,
//                       const std::vector<UtScript* >& aScripts)
// {
//    UtScript*  lReturn = 0;
//    for (unsigned i = 0; i < aScripts.size(); ++i)
//    {
//       if (aScripts[i]->GetNameId() == aScriptId)
//       {
//          lReturn = aScripts[i];
//          break;
//       }
//    }
//    return lReturn;
// }
}

void WsfXIO_ScriptService::HandleExecute(WsfXIO_ScriptExecutePkt& aPkt)
{
   if (!aPkt.mIsApplicationMethod)
   {
      if (aPkt.mPlatformIndex == 0)
      {
         if (aPkt.mScriptName != 0)
         {
            UtScript* scriptPtr = mSimulation.GetScriptContext().FindScript(aPkt.mScriptName);
            if (scriptPtr != nullptr)
            {
               if (CheckParams(scriptPtr, aPkt.mArgumentList, nullptr, mSimulation.GetApplication().GetScriptTypes(), &mSimulation))
               {
                  UtScriptData scriptReturn;
                  mSimulation.GetScriptContext().ExecuteScript(scriptPtr, scriptReturn, aPkt.mArgumentList);
               }
            }
         }
         else if (!aPkt.mScriptText.empty())
         {
            UtScriptData rval;
            mSimulation.GetScriptContext().ExecuteScript(mSimulation.GetSimTime(), rval, aPkt.mArgumentList, aPkt.mScriptText);
         }
      }
      else
      {
         WsfPlatform* platformPtr = mSimulation.GetPlatformByIndex(aPkt.mPlatformIndex);
         if (platformPtr != nullptr)
         {
            if (aPkt.mPartName == nullptr)
            {
               if (aPkt.mScriptName != 0)
               {
                  UtScript* scriptPtr = platformPtr->GetScriptContext().FindScript(aPkt.mScriptName);
                  if (scriptPtr != nullptr)
                  {
                     if (CheckParams(scriptPtr,
                                     aPkt.mArgumentList,
                                     platformPtr,
                                     mSimulation.GetApplication().GetScriptTypes(),
                                     &mSimulation))
                     {
                        UtScriptData scriptReturn;
                        platformPtr->ExecuteScript(mSimulation.GetSimTime(),
                                                   scriptPtr->GetName(),
                                                   scriptReturn,
                                                   aPkt.mArgumentList);
                     }
                  }
               }
               else if (!aPkt.mScriptText.empty())
               {
                  UtScriptData rval;
                  platformPtr->GetScriptContext().ExecuteScript(mSimulation.GetSimTime(),
                                                                rval,
                                                                aPkt.mArgumentList,
                                                                aPkt.mScriptText);
               }
            }
            else
            {
               WsfProcessor*       procPtr       = platformPtr->GetComponent<WsfProcessor>(aPkt.mPartName);
               WsfScriptProcessor* scriptProcPtr = dynamic_cast<WsfScriptProcessor*>(procPtr);
               if (scriptProcPtr != nullptr)
               {
                  if (aPkt.mScriptName != 0)
                  {
                     UtScript* scriptPtr = scriptProcPtr->GetScriptContext().FindScript(aPkt.mScriptName);
                     if (scriptPtr != nullptr)
                     {
                        if (CheckParams(scriptPtr,
                                        aPkt.mArgumentList,
                                        platformPtr,
                                        mSimulation.GetApplication().GetScriptTypes(),
                                        &mSimulation))
                        {
                           UtScriptData scriptReturn;
                           scriptProcPtr->GetScriptContext().ExecuteScript(mSimulation.GetSimTime(),
                                                                           scriptPtr->GetName(),
                                                                           scriptReturn,
                                                                           aPkt.mArgumentList);
                        }
                     }
                  }
                  else if (!aPkt.mScriptText.empty())
                  {
                     UtScriptData rval;
                     scriptProcPtr->GetScriptContext().ExecuteScript(mSimulation.GetSimTime(),
                                                                     rval,
                                                                     aPkt.mArgumentList,
                                                                     aPkt.mScriptText);
                  }
               }
            }
         }
      }
   }
   else // Application method
   {
      UtScriptClass*                  classPtr    = nullptr;
      int                             methodIndex = -1;
      std::string                     name        = aPkt.mScriptName.GetString();
      UtScriptClass::InterfaceMethod* methodPtr   = nullptr;
      if (aPkt.mBaseObject == nullptr)
      {
         size_t dotPos = name.find('.');
         if (dotPos != std::string::npos)
         {
            classPtr = mSimulation.GetApplication().GetScriptTypes()->GetClass(name.substr(0, dotPos));
            if (classPtr != nullptr)
            {
               methodIndex = classPtr->GetMethodIndex(name.substr(dotPos + 1));
               if (methodIndex >= 0)
               {
                  methodPtr = classPtr->GetMethodEntry(methodIndex);
               }
            }
         }
      }
      else if (aPkt.mBaseObject->GetPointer())
      {
         classPtr = aPkt.mBaseObject->GetPointer()->GetScriptClass();
         if (classPtr != nullptr)
         {
            methodIndex = classPtr->GetMethodIndex(name);
         }
      }
      if (methodIndex >= 0 && classPtr)
      {
         methodPtr = classPtr->GetMethodEntry(methodIndex);
         if (methodPtr != nullptr)
         {
            std::vector<std::string> argTypeNames;
            for (auto argType : methodPtr->GetArgTypes())
            {
               argTypeNames.push_back(argType);
            }
            if (CheckParams(argTypeNames, aPkt.mArgumentList, nullptr, mSimulation.GetApplication().GetScriptTypes(), &mSimulation))
            {
               UtScriptMethod* scriptPtr = nullptr;
               if (aPkt.mBaseObject)
               {
                  scriptPtr = new UtScriptMethod(aPkt.mBaseObject->GetPointer(), aPkt.mScriptName.GetString());
               }
               else
               {
                  scriptPtr = new UtScriptMethod(aPkt.mScriptName.GetString());
               }
               UtScriptData returnValue;
               scriptPtr->Call(&(mSimulation.GetScriptExecutor()),
                               returnValue,
                               aPkt.mArgumentList,
                               &(mSimulation.GetScriptContext().GetContext()),
                               true);
            }
         }
      }
   }
}

bool WsfXIO_ScriptService::CheckParams(UtScript*                  aScriptPtr,
                                       std::vector<UtScriptData>& aParams,
                                       WsfPlatform*               aContextPlatformPtr,
                                       UtScriptTypes*             aScriptTypes,
                                       WsfSimulation*             aSimulationPtr)
{
   std::vector<UtStringId> args = aScriptPtr->mPrototype->mArgs;
   if (args.size() != aParams.size())
   {
      return false;
   }

   std::vector<std::string> argTypes(args.size());
   for (size_t i = 0; i < args.size(); ++i)
   {
      UtScriptClass* classPtr = WsfScriptManager::GetTypes().GetClass(args[i]);
      if (classPtr != nullptr)
      {
#undef GetClassName
         argTypes[i] = classPtr->GetClassName();
      }
   }
   return CheckParams(argTypes, aParams, aContextPlatformPtr, aScriptTypes, aSimulationPtr);
}

bool WsfXIO_ScriptService::CheckParams(const std::vector<std::string>& aArgTypes,
                                       std::vector<UtScriptData>&      aParams,
                                       WsfPlatform*                    aContextPlatformPtr,
                                       UtScriptTypes*                  aScriptTypes,
                                       WsfSimulation*                  aSimulationPtr)
{
   if (aArgTypes.size() != aParams.size())
   {
      return false;
   }
   // Some script types are not readily transmittable over the network.
   // Let integers be placeholders for platforms (by index)
   for (size_t i = 0; i < aArgTypes.size(); ++i)
   {
      std::string   type = aArgTypes[i];
      UtScriptData& arg  = aParams[i];
      if (type == "WsfPlatform" && aSimulationPtr)
      {
         WsfPlatform* platformPtr = nullptr;
         if (arg.GetType() == ut::script::Data::Type::cINT)
         {
            platformPtr = aSimulationPtr->GetPlatformByIndex(arg.GetInt());
         }
         else if (arg.GetType() == ut::script::Data::Type::cSTRING)
         {
            platformPtr = aSimulationPtr->GetPlatformByName(arg.GetString());
         }

         if (platformPtr)
         {
            UtScriptClass* platformClassPtr = aScriptTypes->GetClass("WsfPlatform");
            arg.SetPointer(new UtScriptRef(platformPtr, platformClassPtr));
         }
      }
      else if (type == "Array<WsfPlatform>" && aSimulationPtr)
      {
         UtScriptDataList* arrayPtr   = static_cast<UtScriptDataList*>(arg.GetPointer()->GetAppObject());
         UtScriptDataList* argPointer = new UtScriptDataList;
         for (const auto& scriptData : *arrayPtr)
         {
            WsfPlatform* platformPtr = nullptr;
            if (scriptData.GetType() == ut::script::Data::Type::cINT)
            {
               platformPtr = aSimulationPtr->GetPlatformByIndex(scriptData.GetInt());
            }
            else if (scriptData.GetType() == ut::script::Data::Type::cSTRING)
            {
               platformPtr = aSimulationPtr->GetPlatformByName(scriptData.GetString());
            }

            if (platformPtr)
            {
               UtScriptClass* platformClassPtr = aScriptTypes->GetClass("WsfPlatform");
               argPointer->emplace_back(new UtScriptRef(platformPtr, platformClassPtr));
            }
         }
         UtScriptClass* arrayClassPtr = aScriptTypes->GetClass("Array<WsfPlatform>");
         arg.SetPointer(new UtScriptRef(argPointer, arrayClassPtr, UtScriptRef::cMANAGE));
      }
      else if ((type == "WsfTrack" || type == "WsfLocalTrack") && aContextPlatformPtr != nullptr)
      {
         if (arg.GetType() == ut::script::Data::Type::cPOINTER)
         {
            UtScriptClass* trackIdClassPtr = aScriptTypes->GetClass("WsfTrackId");
            if (trackIdClassPtr == arg.GetPointer()->GetScriptClass())
            {
               WsfTrackId* trackIdPtr = static_cast<WsfTrackId*>(arg.GetPointer()->GetAppObject());
               if (trackIdPtr != nullptr)
               {
                  WsfLocalTrackList& localTrackList = aContextPlatformPtr->GetMasterTrackList();
                  WsfLocalTrack*     trackPtr       = localTrackList.FindTrack(*trackIdPtr);
                  if (trackPtr == nullptr && type == "WsfTrack")
                  {
                     WsfTrack*      rawTrackPtr   = aContextPlatformPtr->GetMasterRawTrackList().FindTrack(*trackIdPtr);
                     UtScriptClass* trackClassPtr = aScriptTypes->GetClass("WsfTrack");
                     arg.SetPointer(new UtScriptRef(rawTrackPtr, trackClassPtr));
                  }
                  else
                  {
                     UtScriptClass* localTrackClassPtr = aScriptTypes->GetClass("WsfLocalTrack");
                     arg.SetPointer(new UtScriptRef(trackPtr, localTrackClassPtr));
                  }
               }
            }
         }
      }
   }
   return true;
}
namespace
{


void FillData(UtScriptContext* aContext, WsfXIO_RequestScriptDataResponsePkt::ScriptContextData& aContextData)
{
   aContextData.mContextName = aContext->GetContextName();
   aContextData.mScriptContext.SetPointer(aContext);
   aContextData.mParentContext.SetPointer(aContext->GetParent());
   aContextData.mScope.SetPointer(aContext->GetScope());
}


} // namespace

WsfXIO_ScriptService::ExpressionResult* WsfXIO_ScriptService::ExpressionEval(ExpressionId      aExpressionId,
                                                                             UtScriptExecutor& aContextExecutor,
                                                                             UtScriptExecutorProgramCounter* pc,
                                                                             std::string aExpression)
{
   ExpressionResult                      exprResult;
   UtScriptDebugger*                     debuggerPtr = GetDebugger();
   UtScriptDebugger::EvaluationFunction* evalFn      = debuggerPtr->CompileExpression(pc, aExpression);
   if (evalFn)
   {
      exprResult.mCompiled = true;
      UtScriptDataList argData;
      for (UtScriptDebugger::EvaluationFunction::Arg& arg : evalFn->mLocalVariableArguments)
      {
         if (UtScript::IsStaticVariableIndex(arg.mVariableIndex))
         {
            argData.push_back(pc->mScriptPtr->mStaticVariables[UtScript::GetStaticVariableIndex(arg.mVariableIndex)]);
         }
         else
         {
            ptrdiff_t varIndex = arg.mVariableIndex + pc->mFrameIndex;
            if (varIndex >= static_cast<ptrdiff_t>(aContextExecutor.GetStack().size()))
            {
               continue;
            }
            argData.push_back(aContextExecutor.GetStack()[varIndex]);
         }
      }
      UtScriptContext* tmpContext = new UtScriptContext(pc->mInstancePtr->GetScope());
      tmpContext->SetParent(pc->mInstancePtr);
      debuggerPtr->GetDebugExecutor()->Execute(evalFn->mExpressionScriptPtr, exprResult.mReturnValue, argData, *tmpContext);
      delete tmpContext;
      delete evalFn;
   }
   else
   {
      exprResult.mCompiled = false;
   }
   mEvaluatedExpressions[aExpressionId] = exprResult;
   return &mEvaluatedExpressions[aExpressionId];

   //    // Create a new script and pass local variables as arguments to it
   //    UtScriptDataList argData;
   //    UtScriptParser::ImplicitArgumentList argList;
   //    std::set<std::string> usedNames;
   //    for (size_t i = 0; i < pc->mScriptPtr->mLocalVariables.size(); ++i)
   //    {
   //       // only add the variable if it's in scope
   //       UtScript::LocalVariable& var = pc->mScriptPtr->mLocalVariables[i];
   //       if (var.mValidAfter <= pc->mCodeIndex && var.mValidBefore >= pc->mCodeIndex)
   //       {
   //          if (usedNames.find(var.mName) != usedNames.end()) continue;
   //
   //          // Quick and dirty check that the expression uses the variable
   //          if (aExpression.find(var.mName) != std::string::npos)
   //          {
   //             if (var.IsStaticVariable())
   //             {
   //                argData.push_back(pc->mScriptPtr->mStaticVariables[var.GetStaticVariableIndex()]);
   //             }
   //             else
   //             {
   //                int varIndex = var.mStackIndex + pc->mFrameIndex;
   //                if (varIndex >= (int)aContextExecutor.GetStack().size()) continue;
   //                argData.push_back(aContextExecutor.GetStack()[varIndex]);
   //             }
   //
   //             usedNames.insert(var.mName);
   //
   //             UtScriptParser::ImplicitArgument arg;
   //             arg.mName = var.mName;
   //             arg.mTypePtr = var.mTypePtr;
   //             argList.push_back(arg);
   //          }
   //       }
   //    }
   //    for (size_t i = 0; i < pc->mScriptPtr->mPrototype->mArgs.size() && i < pc->mScriptPtr->mArgumentNames.size() &&
   //    i < pc->mArgsPtr->size(); ++i)
   //    {
   //       std::string argName = pc->mScriptPtr->mArgumentNames[i];
   //       if (usedNames.find(argName) != usedNames.end()) continue;
   //       UtScriptClass* typePtr = aContextExecutor.GetTypes()->GetClass(pc->mScriptPtr->mPrototype->mArgs[i]);
   //       if (typePtr != 0)
   //       {
   //          argData.push_back(pc->mArgsPtr->at(i));
   //          UtScriptParser::ImplicitArgument arg;
   //          arg.mName = pc->mScriptPtr->mArgumentNames[i];
   //          arg.mTypePtr = typePtr;
   //          argList.push_back(arg);
   //       }
   //    }
   //
   //    UtScriptContext* tmpContext = new UtScriptContext(pc->mInstancePtr->GetScope());
   //    tmpContext->SetParent(pc->mInstancePtr);
   //    UtScriptParser& parser = aContextExecutor.GetTypes()->GetParser();
   //    static int evalCount = 0;
   //    std::stringstream nameSS;
   //    nameSS << "__debug_eval_" << evalCount;
   //    UtInput scriptInput;
   //
   //    ExpressionResult exprResult;
   //    exprResult.mExpressionId = aExpressionId;
   //    scriptInput.PushInputString("return " + aExpression + "; <END>");
   //    UtScriptStream stream(scriptInput, "<END>");
   //    UtScript* scriptPtr = parser.Parse(nameSS.str(), "Object", &stream, tmpContext, "debug", false, argList);
   //    if (scriptPtr != 0)
   //    {
   //       exprResult.mCompiled = true;
   //       UtScriptExecutor executor(aContextExecutor.GetTypes());
   //       executor.SetIgnoreExceptions();
   //       executor.Execute(scriptPtr, exprResult.mReturnValue, argData, *tmpContext);
   //    }
   //    else
   //    {
   //       exprResult.mCompiled = false;
   //    }
   //    delete tmpContext;
   //    mEvaluatedExpressions[aExpressionId] = exprResult;
   //    return &mEvaluatedExpressions[aExpressionId];
}

// Determine if a string needs to be evaluated as an expression
bool IsExpressionQuery(const std::string& aStr)
{
   if (aStr.empty())
   {
      return false;
   }
   if (isalpha(aStr[0]) || aStr[0] == '_')
   {
      for (size_t i = 1; i < aStr.size(); ++i)
      {
         if (!(isalpha(aStr[i]) || aStr[i] == '_' || isdigit(aStr[i])))
         {
            return true;
         }
      }
      return false;
   }
   return true;
}

void WsfXIO_ScriptService::HandleScriptDataRequest(WsfXIO_RequestScriptDataPkt& aPkt)
{
   WsfXIO_Connection* senderPtr = dynamic_cast<WsfXIO_Connection*>(aPkt.GetSender());
   if (senderPtr == nullptr)
   {
      return;
   }
   switch (aPkt.mInformationType)
   {
   case WsfXIO_RequestScriptDataPkt::cCONTEXT_INFO:
   {
      UtScriptContext* ctxPtr = (UtScriptContext*)aPkt.mScriptContext.GetPointer();
      if (mScriptEnvironmentPtr->ScriptContextIsValid(ctxPtr))
      {
         WsfXIO_RequestScriptDataResponsePkt response;
         response.mFromProvider = true;
         response.mRequestId    = aPkt.mSubscriptionRequestId;
         WsfXIO_RequestScriptDataResponsePkt::ScriptContextData ctxData;
         FillData(ctxPtr, ctxData);
         response.mContexts.push_back(ctxData);
         senderPtr->Send(response);
      }
   }
   break;
   case WsfXIO_RequestScriptDataPkt::cCONTEXT_VARIABLES:
   {
      UtScriptContext* ctxPtr = (UtScriptContext*)aPkt.mScriptContext.GetPointer();
      if (mScriptEnvironmentPtr->ScriptContextIsValid(ctxPtr))
      {
         WsfXIO_RequestScriptDataResponsePkt response;
         response.mFromProvider              = true;
         response.mRequestId                 = aPkt.mSubscriptionRequestId;
         const UtScriptContext::VarMap& vars = ctxPtr->GetVariables();
         for (const auto& var : vars)
         {
            response.mContextVariables.mNames.push_back(var.first);
         }
         senderPtr->Send(response);
      }
   }
   break;
   case WsfXIO_RequestScriptDataPkt::cSCOPE_DETAILS:
   {
      UtScriptScope* scopePtr = (UtScriptScope*)aPkt.mScope.GetPointer();
      if (mScriptEnvironmentPtr->ScopeIsValid(scopePtr))
      {
         WsfXIO_RequestScriptDataResponsePkt response;
         response.mFromProvider                                          = true;
         response.mRequestId                                             = aPkt.mSubscriptionRequestId;
         WsfXIO_RequestScriptDataResponsePkt::ScriptScopeData& scopeData = response.mScopeData;
         scopeData.mParentScope.SetPointer(scopePtr->GetParent());
         for (const auto& script : scopePtr->GetScripts())
         {
            if (script.second.mIsLocal && script.second.mScriptPtr)
            {
               scopeData.mScripts.emplace_back(script.second.mScriptPtr);
            }
         }
         senderPtr->Send(response);
      }
   }
   break;
   case WsfXIO_RequestScriptDataPkt::cVARIABLE_QUERY:
   {
      if (!mInitializedEnumerateScripts)
      {
         InitializeEnumerateScripts();
      }
      WsfXIO_RequestScriptDataResponsePkt response;
      response.mFromProvider = true;
      response.mRequestId    = aPkt.mSubscriptionRequestId;
      for (WsfXIO_RequestScriptDataPkt::VariableQuery& query : aPkt.mVariableQueries)
      {
         UtScriptExecutorProgramCounter* pcPtr        = nullptr;
         bool                            isParseError = false;
         UtScriptData                    baseResultData;

         if (query.mExpressionReferenceNumber > 0)
         {
            ExpressionId exprId(senderPtr->GetConnectionId(), query.mExpressionReferenceNumber);
            auto         i = mEvaluatedExpressions.find(exprId);
            if (i != mEvaluatedExpressions.end())
            {
               ExpressionResult& result = i->second;
               if (!result.mCompiled)
               {
                  isParseError = true;
               }
               else
               {
                  baseResultData = result.mReturnValue;
               }
            }
         }
         UtScriptContext* ctxPtr = (UtScriptContext*)query.mContext.GetPointer();
         if (baseResultData.GetType() == ut::script::Data::Type::cUNDEFINED)
         {
            if (ctxPtr == nullptr)
            {
               UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
               if (query.mStackFrame == -1)
               {
                  pcPtr = executor.GetCallstackTop();
               }
               else if (query.mStackFrame >= 0 && static_cast<size_t>(query.mStackFrame) < executor.GetCallstackSize())
               {
                  pcPtr = executor.GetCallstack()[query.mStackFrame];
               }
               if (pcPtr != nullptr)
               {
                  ctxPtr = pcPtr->mInstancePtr;
               }
            }
         }
         if (baseResultData.GetType() == ut::script::Data::Type::cUNDEFINED &&
             mScriptEnvironmentPtr->ScriptContextIsValid(ctxPtr))
         {
            if (pcPtr != nullptr)
            {
               if (IsExpressionQuery(query.mVariableName))
               {
                  ExpressionId      exprId(senderPtr->GetConnectionId(), query.mExpressionReferenceNumber);
                  ExpressionResult* resultPtr =
                     ExpressionEval(exprId, mSimulation.GetScriptExecutor(), pcPtr, query.mVariableName);
                  if (!resultPtr->mCompiled)
                  {
                     isParseError = true;
                  }
                  else
                  {
                     baseResultData = resultPtr->mReturnValue;
                  }
               }
               else
               {
                  UtScript::LocalVariable* localPtr =
                     pcPtr->mScriptPtr->FindLocalVariable(query.mVariableName, pcPtr->mCodeIndex);
                  if (localPtr != nullptr)
                  {
                     if (!localPtr->IsStaticVariable())
                     {
                        UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
                        ptrdiff_t         varIndex = pcPtr->mFrameIndex + localPtr->mStackIndex;
                        if (static_cast<ptrdiff_t>(executor.GetStack().size()) > varIndex)
                        {
                           baseResultData = executor.GetStack()[varIndex];
                        }
                     }
                     else
                     {
                        UtScriptData* varData = pcPtr->mScriptPtr->GetStaticVariable(localPtr->GetStaticVariableIndex());
                        if (varData != nullptr)
                        {
                           baseResultData = *varData;
                        }
                     }
                  }
               }
            }
            if (baseResultData.GetType() == ut::script::Data::Type::cUNDEFINED)
            {
               UtScriptData* varPtr = ctxPtr->FindGlobalVar(query.mVariableName);
               if (varPtr != nullptr)
               {
                  baseResultData = *varPtr;
               }
            }
         }
         WsfXIO_RequestScriptDataResponsePkt::VariableQueryResult* queryResultPtr =
            new WsfXIO_RequestScriptDataResponsePkt::VariableQueryResult;
         if (isParseError)
         {
            for (size_t ea = 0; ea < query.mEntryAddresses.size(); ++ea)
            {
               WsfXIO_RequestScriptDataResponsePkt::VariableValue val;
               val.mTypeName = "<ParseError>";
               queryResultPtr->mValues.push_back(val);
            }
         }
         else if (baseResultData.GetType() != ut::script::Data::Type::cUNDEFINED)
         {
            for (const auto& addresses : query.mEntryAddresses)
            {
               WsfXIO_RequestScriptDataResponsePkt::VariableValue resultVal = PackValue(baseResultData, addresses);
               queryResultPtr->mValues.push_back(resultVal);
            }
         }
         else if (query.mStackFrame == -3 && baseResultData.GetType() == ut::script::Data::Type::cUNDEFINED)
         {
            for (size_t ea = 0; ea < query.mEntryAddresses.size(); ++ea)
            {
               auto iter = mDebugEnumerateGlobalScripts.find(query.mVariableName);
               if (iter != mDebugEnumerateGlobalScripts.end())
               {
                  std::vector<int> index = query.mEntryAddresses[ea];
                  // index values are popped off the end, so reverse
                  std::reverse(index.begin(), index.end());

                  WsfXIO_RequestScriptDataResponsePkt::VariableValue resultVal =
                     GetEntryFromEnumerationScript(iter->second, nullptr, index);
                  queryResultPtr->mValues.push_back(resultVal);
               }
               else
               {
                  queryResultPtr->mValues.emplace_back();
               }
            }
         }
         else
         {
            for (size_t ea = 0; ea < query.mEntryAddresses.size(); ++ea)
            {
               queryResultPtr->mValues.emplace_back();
            }
         }
         response.mVariableValues.push_back(queryResultPtr);
      }
      senderPtr->Send(response);
   }
   break;
   case WsfXIO_RequestScriptDataPkt::cSCRIPT_DETAILS:
   {
      UtScript* scriptPtr = (UtScript*)aPkt.mScript.GetPointer();
      if (mScriptEnvironmentPtr->ScriptIsValid(scriptPtr))
      {
         WsfXIO_RequestScriptDataResponsePkt response;
         response.mFromProvider                                         = true;
         response.mRequestId                                            = aPkt.mSubscriptionRequestId;
         WsfXIO_RequestScriptDataResponsePkt::ScriptDetails* detailsPtr = response.mScriptDetailsPtr =
            new WsfXIO_RequestScriptDataResponsePkt::ScriptDetails;
         detailsPtr->mArgNames   = scriptPtr->mArgumentNames;
         detailsPtr->mScriptName = scriptPtr->mName;
         detailsPtr->mPrototype = mScriptEnvironmentPtr->GetPrototypeDescription(scriptPtr->mPrototype, scriptPtr->mName);
         detailsPtr->mSourceFilePath = scriptPtr->mSourceFilePath;
         std::string name;
         for (auto& localVariable : scriptPtr->mLocalVariables)
         {
            if (localVariable.mName != name)
            {
               name = localVariable.mName;
               detailsPtr->mLocalVariableNames.push_back(name);
            }
         }
         scriptPtr->GetSourceLineRange(detailsPtr->mFirstLine, detailsPtr->mLastLine);
         senderPtr->Send(response);
      }
   }
   break;
   case WsfXIO_RequestScriptDataPkt::cCALLSTACK_LISTING:
   {
      WsfXIO_RequestScriptDataResponsePkt::CallstackData* callstackDataPtr =
         new WsfXIO_RequestScriptDataResponsePkt::CallstackData;
      UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
      for (auto* pcPtr : executor.GetCallstack())
      {
         WsfXIO_RequestScriptDataResponsePkt::BreakpointData entry;
         entry.mScriptId   = pcPtr->mScriptPtr;
         entry.mFilePath   = pcPtr->mScriptPtr->mSourceFilePath;
         entry.mLineNumber = pcPtr->mScriptPtr->GetLineFromInstructionIndex(std::max<size_t>(0U, pcPtr->mCodeIndex));
         callstackDataPtr->mCallstackEntries.push_back(entry);
      }
      WsfXIO_RequestScriptDataResponsePkt response;
      response.mFromProvider  = true;
      response.mRequestId     = aPkt.mSubscriptionRequestId;
      response.mCallstackData = callstackDataPtr;
      senderPtr->Send(response);
   }
   break;
   }
}

void WsfXIO_ScriptService::HandleScriptCommand(WsfXIO_ScriptCommandPkt& aPkt)
{
   switch (aPkt.mCommandType)
   {
   case WsfXIO_ScriptCommandPkt::cSET_BREAKPOINT:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         WsfXIO_Subscription*       subPtr = GetSubscription();
         UtScriptDebuggerBreakpoint brk    = debuggerPtr->MakeBreakpointFromLine(aPkt.mFilePath, aPkt.mLineNumber);
         if (brk.IsValid())
         {
            brk.mId        = aPkt.mBreakpointId;
            brk.mCondition = aPkt.mCondition;
            debuggerPtr->AddBreakpoint(brk);
            if (subPtr != nullptr)
            {
               WsfXIO_RequestScriptSessionResponsePkt pkt;
               pkt.mState        = WsfXIO_RequestScriptSessionResponsePkt::cBREAKPOINT_ADDED;
               pkt.mBreakpointId = brk.mId;
               pkt.mFilePath     = aPkt.mFilePath;
               pkt.mLineNumber   = brk.mLineNumber;
               pkt.mSimTime      = mSimulation.GetSimTime();
               subPtr->SendResponse(pkt);
            }
         }
         else if (subPtr != nullptr)
         {
            WsfXIO_RequestScriptSessionResponsePkt pkt;
            pkt.mState        = WsfXIO_RequestScriptSessionResponsePkt::cBREAKPOINT_ADDED;
            pkt.mBreakpointId = aPkt.mBreakpointId;
            pkt.mLineNumber   = ut::npos;
            subPtr->SendResponse(pkt);
         }
      }
   }
   break;
   case WsfXIO_ScriptCommandPkt::cCLEAR_BREAKPOINT:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         debuggerPtr->RemoveBreakpoint(aPkt.mBreakpointId);
      }
   }
   break;
   case WsfXIO_ScriptCommandPkt::cRESUME:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         debuggerPtr->Resume();
      }
   }
   break;
   case WsfXIO_ScriptCommandPkt::cSTEP_INTO:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         debuggerPtr->StepInto();
      }
   }
   break;
   case WsfXIO_ScriptCommandPkt::cSTEP_OVER:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         debuggerPtr->StepOver();
      }
   }
   break;
   case WsfXIO_ScriptCommandPkt::cSTEP_OUT:
   {
      UtScriptDebugger* debuggerPtr = GetDebugger();
      if (debuggerPtr != nullptr)
      {
         debuggerPtr->StepOut();
      }
   }
   break;
   }
}

// void WsfXIO_ScriptService::SetDebuggerEnabled(bool aEnabled)
// {
//    UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
//
//    if (aEnabled != (0 != executor.GetAttachedDebugger()))
//    {
//       if (aEnabled)
//       {
//          UtScriptDebugger* dbg = new UtScriptDebugger(&WsfScriptManager::GetTypes());
//          mCallbacks += dbg->UpdateWhileAtBreak.Connect(&WsfXIO_ScriptService::UpdateWhileAtBreak, this);
//          executor.Attach(dbg);
//       }
//       else
//       {
//          executor.DetatchDebugger();
//       }
//    }
// }

void WsfXIO_ScriptService::UpdateWhileAtBreak()
{
   // This method is called while the script system is at a breakpoint
   // This forces the XIO interface to update
   GetInterface()->AdvanceTime(mSimulation.GetSimTime());
}

void WsfXIO_ScriptService::OnUpdate()
{
   // This method is called if a script has been running for a while
   // This forces the XIO interface to update
   GetInterface()->AdvanceTime(mSimulation.GetSimTime());
}

void WsfXIO_ScriptService::HandleRequest(WsfXIO_RequestScriptSessionPkt& aPkt)
{
   if (GetDebugger())
   {
      DenyRequest(aPkt); // Can't have more than one debugger
   }
   UtScriptExecutor& executor = mSimulation.GetScriptExecutor();

   // This lets WSF do sloppy-realtime
   mSimulation.SetFlexibleRealtime();

   UtScriptDebugger* debuggerPtr = new UtScriptDebugger(mScriptEnvironmentPtr);
   mCallbacks += debuggerPtr->OnBreak.Connect(&WsfXIO_ScriptService::OnBreak, this);
   mCallbacks += debuggerPtr->OnContinue.Connect(&WsfXIO_ScriptService::OnContinue, this);
   mCallbacks += debuggerPtr->UpdateWhileAtBreak.Connect(&WsfXIO_ScriptService::UpdateWhileAtBreak, this);
   mCallbacks += debuggerPtr->OnUpdate.Connect(&WsfXIO_ScriptService::OnUpdate, this);
   executor.Attach(debuggerPtr);

   // Send a response so that receiver knows the request is accepted.  No actual data flows without further requests.
   WsfXIO_ResponsePkt response;
   CreateSubscription(aPkt);
   // cBREAKPOINT_BREAK not really used here...
   SendState(UtScriptDebugger::cBREAKPOINT_BREAK, true);
}

UtScriptDebugger* WsfXIO_ScriptService::GetDebugger()
{
   UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
   return executor.GetAttachedDebugger();
}

void WsfXIO_ScriptService::SubscriptionCancelledT(WsfXIO_Subscription*            aSubscriptionPtr,
                                                  WsfXIO_RequestScriptSessionPkt& aRequest)
{
   UtScriptExecutor& executor = mSimulation.GetScriptExecutor();
   executor.DetatchDebugger();
   WsfXIO_ServiceT<WsfXIO_RequestScriptSessionPkt, WsfXIO_SimService>::SubscriptionCancelledT(aSubscriptionPtr, aRequest);
}

void WsfXIO_ScriptService::OnBreak(UtScriptDebugger::BreakType aBreakType)
{
   // Remove old expression data
   mEvaluatedExpressions.clear();
   SendState(aBreakType);
}

void WsfXIO_ScriptService::OnContinue()
{
   SendState(UtScriptDebugger::cSTEP_BREAK); // break type not used
}

void WsfXIO_ScriptService::SendState(UtScriptDebugger::BreakType aBreakType, bool aIsInitialSend)
{
   WsfXIO_Subscription* subPtr = GetSubscription();
   if (subPtr != nullptr)
   {
      UtScriptDebugger*                      debuggerPtr = GetDebugger();
      WsfXIO_RequestScriptSessionResponsePkt pkt;
      pkt.mSimTime     = mSimulation.GetSimTime();
      pkt.mInitialData = nullptr;
      switch (aBreakType)
      {
      case UtScriptDebugger::cBREAKPOINT_BREAK:
         pkt.mState = WsfXIO_RequestScriptSessionResponsePkt::cAT_BREAKPOINT;
         break;
      case UtScriptDebugger::cSTEP_BREAK:
         pkt.mState = WsfXIO_RequestScriptSessionResponsePkt::cPAUSED;
         break;
      case UtScriptDebugger::cEXCEPTION_BREAK:
         pkt.mState                = WsfXIO_RequestScriptSessionResponsePkt::cAT_EXCEPTION;
         pkt.mExceptionDescription = debuggerPtr->GetExceptionDescription();
         break;
      }
      if (!debuggerPtr->IsBreaking())
      {
         pkt.mState = WsfXIO_RequestScriptSessionResponsePkt::cRUNNING;
      }
      if (debuggerPtr->IsBreaking())
      {
         UtScriptExecutor&               executor = mSimulation.GetScriptExecutor();
         UtScriptExecutorProgramCounter* pcPtr    = executor.GetCallstackTop();
         if (pcPtr != nullptr)
         {
            // code index of -1 is possible when breaking before start of function
            size_t line     = pcPtr->mScriptPtr->GetLineFromInstructionIndex(std::max<size_t>(0U, pcPtr->mCodeIndex));
            pkt.mLineNumber = line;
            pkt.mFilePath   = pcPtr->mScriptPtr->mSourceFilePath;
         }
      }
      if (aIsInitialSend)
      {
         if (!mInitializedEnumerateScripts)
         {
            InitializeEnumerateScripts();
         }
         pkt.mInitialData = new WsfXIO_RequestScriptSessionResponsePkt::InitialData;
         for (auto& debugEnumerateGlobalScript : mDebugEnumerateGlobalScripts)
         {
            pkt.mInitialData->mGlobalEnumerateScripts.push_back(debugEnumerateGlobalScript.first);
         }
      }
      subPtr->SendResponse(pkt);
   }
}

void WsfXIO_ScriptService::InitializeEnumerateScripts()
{
   UtScriptContext& ctx = mSimulation.GetScriptContext().GetContext();

   UtScriptClass* returnTypePtr = ctx.GetTypes()->GetClass("Array<Object>");
   UtScriptClass* indexTypePtr  = ctx.GetTypes()->GetClass("Array<int>");
   if (returnTypePtr == nullptr || indexTypePtr == nullptr)
   {
      return;
   }

   // Find any scripts with the prefix "_debug_enumerate_";
   // The full name is ignored, but the parameter type is used to determine what types the script should be showing
   const UtScriptScope::ScriptMap& scripts = ctx.GetScope()->GetScripts();
   for (const auto& script : scripts)
   {
      const std::string& name = script.first;
      if (name[0] == '_' && name.size() > 17 && name.substr(0, 17) == "_debug_enumerate_")
      {
         UtScript* scriptPtr = script.second.mScriptPtr;
#undef GetClassName
         if (scriptPtr->mPrototype->mReturnClassPtr == returnTypePtr && scriptPtr->mPrototype->mArgs.size() == 2 &&
             scriptPtr->mPrototype->mArgs[1] == indexTypePtr->GetClassName())
         {
            UtScriptClass* valueClassPtr = ctx.GetTypes()->GetClass(scriptPtr->mPrototype->mArgs[0]);
            if (valueClassPtr != nullptr)
            {
               mDebugEnumerateScripts[valueClassPtr] = scriptPtr;
            }
         }
      }
      if (name[0] == '_' && name.size() > 24 && name.substr(0, 24) == "_debug_global_enumerate_")
      {
         UtScript* scriptPtr = script.second.mScriptPtr;
#undef GetClassName
         if (scriptPtr->mPrototype->mReturnClassPtr == returnTypePtr && scriptPtr->mPrototype->mArgs.size() == 1 &&
             scriptPtr->mPrototype->mArgs[0] == indexTypePtr->GetClassName())
         {
            mDebugEnumerateGlobalScripts[name.substr(24)] = scriptPtr;
         }
      }
   }

   // Loop through all script classes and try to find an enumeration script that will work.
   // For instance:  WsfTaskProcessor will use the _debug_enumerate_WsfProcessor() script
   for (UtScriptTypes::ConstClassMapItr i = ctx.GetTypes()->Begin(); i != ctx.GetTypes()->End(); ++i)
   {
      UtScriptClass*          classPtr  = i->second.get();
      std::vector<UtStringId> baseNames = classPtr->GetClassNameHierachy();
      while (!baseNames.empty())
      {
         UtScriptClass* ancestorPtr = ctx.GetTypes()->GetClass(baseNames.back());
         baseNames.pop_back();
         if (mDebugEnumerateScripts.find(ancestorPtr) != mDebugEnumerateScripts.end())
         {
            mDebugEnumerateScripts[classPtr] = mDebugEnumerateScripts[ancestorPtr];
            break;
         }
      }
   }

   UtScriptScope::ScriptRef* scriptPtr = ctx.GetScope()->FindScript("_debug_global_enumerate_Simulation", true);
   if (scriptPtr && scriptPtr->mScriptPtr)
   {
      mDebugEnumerateGobalsScript = scriptPtr->mScriptPtr;
   }

   mInitializedEnumerateScripts = true;
}

WsfXIO_RequestScriptDataResponsePkt::VariableValue
WsfXIO_ScriptService::GetEntryFromEnumerationScript(UtScript*         aEnumScriptPtr,
                                                    UtScriptData*     aBaseEntryPtr,
                                                    std::vector<int>& aAddress)
{
   size_t           originalAddressSize = aAddress.size();
   UtScriptTypes*   typesPtr            = &WsfScriptManager::GetTypes();
   UtScriptExecutor executor(mScriptEnvironmentPtr);
   executor.SetIgnoreExceptions();
   UtScriptClass*   classPtr = nullptr;
   UtScriptDataList args;
   if (aBaseEntryPtr != nullptr)
   {
      args.push_back(*aBaseEntryPtr);
      if (aBaseEntryPtr->GetPointer())
      {
         classPtr = aBaseEntryPtr->GetPointer()->GetScriptClass();
      }
   }
   UtScriptDataList* entryArrayPtr = new UtScriptDataList;
   UtScriptClass*    intArrayType  = typesPtr->GetClassFromCharArray("Array<int>");
   for (int entry : aAddress)
   {
      entryArrayPtr->push_back(UtScriptData((int)entry));
   }
   args.push_back(UtScriptData(new UtScriptRef(entryArrayPtr, intArrayType, UtScriptRef::cMANAGE)));
   UtScriptData returnValue;
   mSimulation.GetScriptContext().GetContext().Execute(&executor, aEnumScriptPtr, returnValue, args);

   // Expecting a pointer to an Array<Object>
   if (returnValue.GetType() == ut::script::Data::Type::cPOINTER)
   {
      UtScriptClass* returnClassPtr = returnValue.GetPointer()->GetScriptClass();
      UtScriptArray* arrayClassPtr  = dynamic_cast<UtScriptArray*>(returnClassPtr);
      if (arrayClassPtr && returnValue.GetPointer()->IsValid())
      {
         UtScriptDataList* returnArrayPtr = static_cast<UtScriptDataList*>(returnValue.GetPointer()->GetAppObject());
         if (returnArrayPtr->size() >= 2)
         {
            if (entryArrayPtr->size() < aAddress.size())
            {
               aAddress.resize(entryArrayPtr->size());
            }
            bool                                               isFinalEntry = aAddress.empty();
            WsfXIO_RequestScriptDataResponsePkt::VariableValue value;
            std::string                                        entryName = returnArrayPtr->at(0).GetString();
            if (returnArrayPtr->size() == 3)
            {
               if (isFinalEntry)
               {
                  value             = PackValue(returnArrayPtr->at(1));
                  value.mEntryCount = returnArrayPtr->at(2).GetInt();
                  if (value.mValueText == "<list>" || value.mValueText == "<map>")
                  {
                     value.mTypeName  = value.mValueText;
                     value.mValueText = '[';
                     value.mValueText += UtStringUtil::ToString(value.mEntryCount);
                     value.mValueText += ']';
                  }
               }
            }
            else
            {
               if (isFinalEntry)
               {
                  // UtScriptData data = returnArrayPtr->at(1);
                  // value = PackValue();
                  UtScriptData resultData = returnArrayPtr->at(1);
                  // Check for an immediate cycle so that we don't loop infinitely
                  if (originalAddressSize != aAddress.size() || !resultData.GetPointer() ||
                      resultData.GetPointer()->GetScriptClass() != classPtr)
                  {
                     value = GetEntry(returnArrayPtr->at(1), aAddress);
                  }
                  else
                  {
                     value = PackValue(resultData);
                  }
               }
               else
               {
                  value = GetEntry(returnArrayPtr->at(1), aAddress);
               }
            }
            if (isFinalEntry)
            {
               value.mEntryName = entryName;
            }
            return value;
         }
      }
      else
      {
         aAddress.clear();
      }
   }
   return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
}

WsfXIO_RequestScriptDataResponsePkt::VariableValue WsfXIO_ScriptService::GetEntry(UtScriptData      aBaseEntry,
                                                                                  std::vector<int>& aAddress)
{
   if (aBaseEntry.GetType() != ut::script::Data::Type::cPOINTER)
   {
      if (!aAddress.empty())
      {
         return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
      }
      return PackValue(aBaseEntry);
   }
   UtScriptRef* refPtr = aBaseEntry.GetPointer();
   if (refPtr == nullptr)
   {
      return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
   }
   UtScriptClass* classPtr = refPtr->GetScriptClass();
   if (mDebugEnumerateScripts.find(classPtr) != mDebugEnumerateScripts.end())
   {
      UtScript* enumScriptPtr = mDebugEnumerateScripts[classPtr];
      return GetEntryFromEnumerationScript(enumScriptPtr, &aBaseEntry, aAddress);
   }
   if (aAddress.empty())
   {
      return PackValue(aBaseEntry);
   }

   // Process a single address entry
   int aEntryIndex = aAddress.back();
   aAddress.pop_back();

   if (aEntryIndex < 0)
   {
      return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
   }

   // if (! classPtr->IsContainer()) return 0;
   UtScriptArray* arrayTypePtr  = dynamic_cast<UtScriptArray*>(classPtr);
   UtScriptSet*   setTypePtr    = dynamic_cast<UtScriptSet*>(classPtr);
   UtScriptMap*   mapTypePtr    = dynamic_cast<UtScriptMap*>(classPtr);
   int            enumerateSize = classPtr->EnumerateSize(refPtr->GetAppObject());
   if (enumerateSize > 0)
   {
      if (enumerateSize > aEntryIndex)
      {
         return GetEntry(classPtr->Enumerate(refPtr->GetAppObject(), aEntryIndex), aAddress);
      }
      return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
   }
   else if (arrayTypePtr != nullptr)
   {
      UtScriptDataList* arrayData = static_cast<UtScriptDataList*>(refPtr->GetAppObject());
      if (aEntryIndex >= (int)arrayData->size())
      {
         return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
      }
      return GetEntry(arrayData->at(aEntryIndex), aAddress);
   }
   if (setTypePtr != nullptr)
   {
      std::set<UtScriptData>* setData = static_cast<std::set<UtScriptData>*>(refPtr->GetAppObject());
      auto                    iter    = setData->begin();
      for (int i = 0; i < aEntryIndex && iter != setData->end(); ++iter, ++i)
      {
      }
      return GetEntry(*iter, aAddress);
   }
   else if (mapTypePtr != nullptr)
   {
      ScriptMap* mapData = static_cast<ScriptMap*>(refPtr->GetAppObject());
      auto       iter    = mapData->begin();
      for (int i = 0; i < aEntryIndex && iter != mapData->end(); ++iter, ++i)
      {
      }
      if (iter != mapData->end())
      {
         if (aAddress.empty())
         {
            WsfXIO_RequestScriptDataResponsePkt::VariableValue val;
            val.mTypeName   = "MAP_ITERATOR";
            val.mEntryCount = 2;
            return val;
         }
         else
         {
            if (aAddress.back() == 0)
            {
               aAddress.pop_back();
               return GetEntry(iter->first, aAddress);
            }
            else if (aAddress.back() == 1)
            {
               aAddress.pop_back();
               return GetEntry(iter->second, aAddress);
            }
         }
      }
      return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
   }
   return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
}

WsfXIO_RequestScriptDataResponsePkt::VariableValue WsfXIO_ScriptService::PackValue(const UtScriptData& aEntry,
                                                                                   std::vector<int>    aIndex)
{
   // index values are popped off the end, so reverse
   std::reverse(aIndex.begin(), aIndex.end());
   return GetEntry(aEntry, aIndex);
}

WsfXIO_RequestScriptDataResponsePkt::VariableValue WsfXIO_ScriptService::PackValue(UtScriptData aEntry)
{
   if (aEntry.GetType() == ut::script::Data::Type::cUNDEFINED)
   {
      return WsfXIO_RequestScriptDataResponsePkt::VariableValue();
   }

   WsfXIO_RequestScriptDataResponsePkt::VariableValue value;

   if (aEntry.GetType() == ut::script::Data::Type::cUNDEFINED)
   {
      return value;
   }

   std::stringstream valueStream;
   aEntry.ToStringN(valueStream, 100);
   value.mValueText = valueStream.str();

   if (aEntry.GetType() == ut::script::Data::Type::cPOINTER)
   {
      UtScriptRef* refPtr = aEntry.GetPointer();
      if (!(refPtr && refPtr->IsValid()))
      {
         return value;
      }
      UtScriptClass* classPtr = refPtr->GetScriptClass();
#undef GetClassName
      value.mTypeName   = classPtr->GetClassName();
      int enumerateSize = classPtr->EnumerateSize(refPtr->GetAppObject());
      if (enumerateSize > 0)
      {
         value.mEntryCount = enumerateSize;
      }
      else if (classPtr->IsContainer())
      {
         UtScriptArray* arrayTypePtr = dynamic_cast<UtScriptArray*>(classPtr);
         UtScriptSet*   setTypePtr   = dynamic_cast<UtScriptSet*>(classPtr);
         UtScriptMap*   mapTypePtr   = dynamic_cast<UtScriptMap*>(classPtr);
         if (arrayTypePtr != nullptr)
         {
            UtScriptDataList* arrayData = static_cast<UtScriptDataList*>(refPtr->GetAppObject());
            value.mEntryCount           = (int)arrayData->size();
         }
         else if (setTypePtr != nullptr)
         {
            std::set<UtScriptData>* setData = static_cast<std::set<UtScriptData>*>(refPtr->GetAppObject());
            value.mEntryCount               = (int)setData->size();
         }
         else if (mapTypePtr != nullptr)
         {
            ScriptMap* mapData = static_cast<ScriptMap*>(refPtr->GetAppObject());
            value.mEntryCount  = (int)mapData->size(); // (int)value->mSubEntries.size();
         }
      }
   }
   else
   {
      switch (aEntry.GetType())
      {
      case ut::script::Data::Type::cBOOL:
         value.mTypeName = "bool";
         break;
      case ut::script::Data::Type::cINT:
         value.mTypeName = "int";
         break;
      case ut::script::Data::Type::cDOUBLE:
         value.mTypeName = "double";
         break;
      case ut::script::Data::Type::cSTRING:
         value.mTypeName = "string";
         break;
      default: /* can't get here */
         break;
      }
   }
   return value;
}
