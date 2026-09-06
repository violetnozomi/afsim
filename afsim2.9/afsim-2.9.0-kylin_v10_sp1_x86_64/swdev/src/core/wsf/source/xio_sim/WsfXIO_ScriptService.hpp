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

#ifndef WSFXIO_SCRIPTSERVICE_HPP
#define WSFXIO_SCRIPTSERVICE_HPP

#include "wsf_export.h"

#include <vector>

#include "UtCallbackHolder.hpp"
class UtScript;
#include "UtScriptData.hpp"
#include "UtScriptDebugger.hpp"
class UtScriptExecutor;
struct UtScriptExecutorProgramCounter;
class WsfPlatform;
class WsfPlatformPart;
class WsfSimulation;
#include "xio/WsfXIO_PacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimPacketRegistry.hpp"
#include "xio_sim/WsfXIO_SimService.hpp"
class WsfXIO_Simulation;

//! Handles requests for script listings, and remote script execution
class WSF_EXPORT WsfXIO_ScriptService : public WsfXIO_ServiceT<WsfXIO_RequestScriptSessionPkt, WsfXIO_SimService>
{
public:
   using BaseClassType = WsfXIO_ServiceT<WsfXIO_RequestScriptSessionPkt, WsfXIO_SimService>;

   WsfXIO_ScriptService(WsfXIO_Simulation& aXIO_Simulation);

   static bool CheckParams(UtScript*                  aScriptPtr,
                           std::vector<UtScriptData>& aParams,
                           WsfPlatform*               aContextPlatformPtr,
                           UtScriptTypes*             aScriptTypes,
                           WsfSimulation*             aSimulationPtr);

   static bool CheckParams(const std::vector<std::string>& aArgTypes,
                           std::vector<UtScriptData>&      aParams,
                           WsfPlatform*                    aContextPlatformPtr,
                           UtScriptTypes*                  aScriptTypes,
                           WsfSimulation*                  aSimulationPtr);

   void SubscriptionCancelledT(WsfXIO_Subscription* aSubscriptionPtr, WsfXIO_RequestScriptSessionPkt& aRequest) override;

protected:
   using ScriptMap = std::map<UtScriptData, UtScriptData>;
   WsfXIO_RequestScriptDataResponsePkt::VariableValue PackValue(const UtScriptData& aEntry, std::vector<int> aIndex);

   // Note UtScriptData are passed by value to avoid potential problems with references getting deleted while processing
   WsfXIO_RequestScriptDataResponsePkt::VariableValue PackValue(UtScriptData aEntry);
   WsfXIO_RequestScriptDataResponsePkt::VariableValue GetEntry(UtScriptData aBaseEntry, std::vector<int>& aEntryIndex);

   WsfXIO_RequestScriptDataResponsePkt::VariableValue GetEntryFromEnumerationScript(UtScript*         aEnumScriptPtr,
                                                                                    UtScriptData*     aBaseEntry,
                                                                                    std::vector<int>& aAddress);

   struct ExpressionResult;

   WsfXIO_Subscription* GetSubscription()
   {
      if (!GetSubscriptions().empty())
      {
         return GetSubscriptions().begin()->second;
      }
      return nullptr;
   }
   void OnBreak(UtScriptDebugger::BreakType aBreakType);
   void OnContinue();
   void OnUpdate();

   void SendState(UtScriptDebugger::BreakType aBreakType, bool aIsInitialSend = false);
   void HandleRequest(WsfXIO_RequestScriptSessionPkt& aPkt);

   void HandleInformationRequest(WsfXIO_InformationRequestPkt& aPkt);
   void HandleScriptDataRequest(WsfXIO_RequestScriptDataPkt& aPkt);
   void HandleExecute(WsfXIO_ScriptExecutePkt& aPkt);
   void HandleScriptCommand(WsfXIO_ScriptCommandPkt& aPkt);

   void UpdateWhileAtBreak();
   void StoreScript(WsfPlatformPart* aPartPtr, UtScript* aScript, WsfXIO_ScriptListPkt& aPkt);

   void StoreAllScripts(WsfPlatformPart* aPartPtr, UtScriptContext* aInstancePtr, WsfXIO_ScriptListPkt& aPkt);
   void InitializeEnumerateScripts();
   using ExpressionId = std::pair<int, int>;
   ExpressionResult* ExpressionEval(ExpressionId                    aExpressionId,
                                    UtScriptExecutor&               aContextExecutor,
                                    UtScriptExecutorProgramCounter* pc,
                                    std::string                     aExpression);

   struct ExpressionResult
   {
      ExpressionId     mExpressionId;
      bool             mCompiled;
      UtScriptData     mReturnValue;
      UtScriptContext* mContextPtr;
   };

   int               mNextExpressionId;
   UtScriptDebugger* GetDebugger();

   WsfSimulation&       mSimulation;
   UtCallbackHolder     mCallbacks;
   UtScriptEnvironment* mScriptEnvironmentPtr;

   std::map<ExpressionId, ExpressionResult> mEvaluatedExpressions;
   bool                                     mInitializedEnumerateScripts;
   std::map<UtScriptClass*, UtScript*>      mDebugEnumerateScripts;
   std::map<std::string, UtScript*>         mDebugEnumerateGlobalScripts;
   UtScript*                                mDebugEnumerateGobalsScript;
};
#endif
