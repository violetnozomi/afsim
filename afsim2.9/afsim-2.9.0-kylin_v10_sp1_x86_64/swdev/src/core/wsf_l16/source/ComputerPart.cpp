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

#include "ComputerPart.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

#include "ComputerProcessor.hpp"
#include "Interface.hpp"
#include "JMessage.hpp"
#include "Messages.hpp"
#include "MessagesFactory.hpp"
#include "ScriptMessage.hpp"
#include "UtAtmosphere.hpp"
#include "UtCallback.hpp"
#include "UtScriptFunction.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommUtil.hpp"
#include "WsfCommandChain.hpp"
#include "WsfJTIDS_Terminal.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptContext.hpp"

namespace WsfL16
{

ComputerPart::ComputerPart(WsfScenario& aScenario, const std::string& aClassName /*="WsfProcessor"*/)
   : mSendInterval(1.0)
   , mDebugMore(0)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), aClassName, "PROCESSOR"))
   , mCommandChainId(nullptr)
   , mCallbackPtr(nullptr)
   , mDebug(false)
{
   mContextPtr->GetContext().RegisterVariable("L16PROCESSOR", "Link16Computer");
}

ComputerPart::ComputerPart(const ComputerPart& aSrc)
   : WsfObject(aSrc)
   , mSendInterval(aSrc.mSendInterval)
   , mDebugMore(aSrc.mDebugMore)
   , mSlotParameters(aSrc.mSlotParameters)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mCommandChainId(aSrc.mCommandChainId)
   , mCallbackPtr(nullptr)
   , mDebug(aSrc.mDebug)
{
}

ComputerPart::~ComputerPart()
{
   delete mContextPtr;
}

bool ComputerPart::Initialize(double aSimTime, ComputerProcessor& aComputer)
{
   mComputer    = &aComputer;
   mPlatformPtr = mComputer->GetPlatform();
   bool ok      = mContextPtr->Initialize(aSimTime, GetPlatform(), mComputer);

   if (ok)
   {
      mContextPtr->GetContext().Var("L16PROCESSOR").GetPointer()->SetAppObject(GetComputer());

      const UtScriptScope::ScriptMap& scripts = mContextPtr->GetContext().GetScope()->GetScripts();
      for (UtScriptScope::ScriptMap::const_iterator i = scripts.begin(); i != scripts.end(); ++i)
      {
         if (!i->second.mIsLocal)
         {
            continue;
         }
         UtScript*   script = i->second.mScriptPtr;
         std::string name   = script->GetName();
         if (name.substr(0, 8) == "on_send_")
         {
            int lbl, sublbl;
            if (2 == sscanf(name.substr(8).c_str(), "%i_%i", &lbl, &sublbl))
            {
               bool              goodParams = false;
               std::stringstream ss;
               ss << "WsfTadilJ" << lbl << '_' << sublbl << "I";
               std::string goodType = ss.str();

               if (script->mPrototype->mArgs.size() == 1)
               {
                  UtScriptClass* classPtr = mContextPtr->GetTypes()->GetClass(script->mPrototype->mArgs[0]);
                  if (classPtr && classPtr->GetClassName() == goodType)
                  {
                     goodParams = true;
                     ScriptData data;
                     data.mScript                     = script;
                     data.mMessageScriptClass         = Messages::Factory::GetMessageScriptClass(lbl, sublbl);
                     mSendScripts[lbl * 100 + sublbl] = data;
                  }
               }
               if (!goodParams)
               {
                  auto logInfo = ut::log::info()
                                 << "Bad parameters for Script. Was expecting one parameter of a certain Type.";
                  logInfo.AddNote() << "Script: " << script->GetName();
                  logInfo.AddNote() << "Type: " << goodType;
                  ok = false;
               }
            }
         }
      }
   }
   if (ok)
   {
      mComputer = &aComputer;
      if (mCommandChainId == nullptr)
      {
         mCommandChainId = WsfCommandChain::GetDefaultNameId();
      }
      mCommandChainPtr = mComputer->GetPlatform()->GetComponent<WsfCommandChain>(mCommandChainId);
      if (mCommandChainPtr == nullptr)
      {
         auto logInfo = ut::log::info() << "Command Chain is not valid command chain on this platform.";
         logInfo.AddNote() << "Command Chain: " << mCommandChainId;
         ok = false;
      }
      //}
   }

   if (ok)
   {
      mCommPtr = mComputer->GetComm();
      mDebug   = mComputer->DebugEnabled();
      mSendInterval.Initialize(aSimTime, *mPlatformPtr->GetSimulation());
      mCallbacks.Add(mSendInterval.OnUpdate.Connect(&ComputerPart::HandleSend, this));
   }

   return ok;
}

void ComputerPart::SendJMessage(double aSimTime, Messages::InitialBase* aMessage)
{
   int lbl, sublbl;
   aMessage->GetLabel(lbl, sublbl);
   ScriptMap::iterator i = mSendScripts.find(lbl * 100 + sublbl);
   if (i != mSendScripts.end())
   {
      ScriptData&      data = i->second;
      UtScriptData     retVal;
      UtScriptDataList args;
      args.push_back(UtScriptData(new UtScriptRef(aMessage, data.mMessageScriptClass)));
      mContextPtr->ExecuteScript(aSimTime, data.mScript, retVal, args);
   }
   if (mDebug)
   {
      auto logDebug = ut::log::debug() << "Sending JMessage.";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << GetPlatform()->GetName();
      logDebug.AddNote() << "Computer Part: " << GetName();
      aMessage->PrintMessage(logDebug);
   }
   //   //the aux data does not hold the tail number anymore
   //   if (!(mComputer->GetPlatform()->GetAuxDataConst().AttributeExists("weapon_tail_number")))
   //   {
   // Only set this if the weapon did not receive "umbilical data" on launch,
   // otherwise this will overwrite the desired TN with the weapon's DIS
   // entity ID
   aMessage->GetHeader().mSourceTrackNumber = mComputer->Config().mTrackNumber;
   //   }
   JMessage::MessagePtr msgPtr(aMessage);
   JMessage             msg(msgPtr, GetPlatform());
   bool                 sentOverCommDevice(false);
   if (mCommandChainPtr != nullptr && mComputer->WSF_OutputEnabled())
   {
      std::vector<WsfPlatform*> recipients;
      wsf::comm::util::SelectRecipients(mCommandChainPtr,
                                        wsf::comm::util::cCOMMANDER | wsf::comm::util::cPEERS |
                                           wsf::comm::util::cSUBORDINATES,
                                        recipients);
      for (auto platform : recipients)
      {
         msg.SetGroup(mCommandChainPtr->GetNameId());
         for (WsfComponentList::RoleIterator<wsf::comm::Comm> it(*platform); !it.AtEnd(); ++it)
         {
            auto commRcvrPtr = *it;
            if (commRcvrPtr->IsTurnedOn() && commRcvrPtr->CanReceive())
            {
               auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();
               if (networkManagerPtr->PathExists(mCommPtr->GetAddress(), commRcvrPtr->GetAddress()))
               {
                  sentOverCommDevice = mCommPtr->Send(aSimTime, ut::clone<WsfMessage>(msg), commRcvrPtr->GetAddress());
                  break;
               }
            }
         }
      }
   }

   // If sending over DIS is enabled (this is initialized to the computer processor value)
   // and the comm device is not a JTIDS then send over the DIS interface here
   // or if the device is a JTIDS but the above send failed then send over the DIS interface here
   // JTIDS will trigger the sending of DIS using the MessageSent callback in the ComputerProcessor
   // Ultimately the l16 interface SendJMessage will get called if all the conditions are met
   if (mComputer->DisOutputEnabled() && (!mComputer->GetJTIDS_Comm() || !sentOverCommDevice)) // NOT JTIDS or not sent above
   {
      mComputer->SentJMessage(aSimTime, this, msg, mCommandChainPtr);
   }
}

bool ComputerPart::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mContextPtr->ProcessInput(aInput))
   {
   }
   else if (aInput.GetCommand() == "command_chain")
   {
      std::string lCmdChainName;
      aInput.ReadValue(lCmdChainName);
      mCommandChainId = lCmdChainName;
   }
   else if (mSendInterval.GetUpdateInterval() >= 0 && aInput.GetCommand() == "send_interval")
   {
      mSendInterval.SetUpdateInterval(ProcessSendInterval(aInput));
   }
   else if (aInput.GetCommand() == "debug_more") // NO_DOC | deprecated as of v 2.9 | not being used in code
   {
      ut::log::warning() << "'debug_more' has been deprecated. The command was not doing anything.";
      std::string lReadMore = "";
      aInput.ReadValue(lReadMore);
      if (lReadMore == "print_tgt_lat_lon")
      {
         mDebugMore = 1;
      }
   }
   else if (mSlotParameters.ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

double ComputerPart::ProcessSendInterval(UtInput& aInput) const
{
   double      interval;
   std::string input1;
   aInput.ReadValue(input1);
   if (input1 == "rrn" || input1 == "RRN")
   {
      int rrn;
      aInput.ReadValue(rrn);
      aInput.ValueInClosedRange(rrn, 1, 15);
      // assume 512 slots per frame, 12 second frame
      interval = static_cast<double>((1 << (15 - rrn))) / 512.0 * 12.0;
   }
   else
   {
      aInput.PushBack(input1);
      aInput.ReadValueOfType(interval, UtInput::cTIME);
   }
   return interval;
}

WsfSimulation* ComputerPart::GetSimulation()
{
   WsfPlatform* platformPtr = GetPlatform();
   return platformPtr ? platformPtr->GetSimulation() : nullptr;
}

void ComputerPart::SetPlatform(WsfPlatform* aPlatformPtr)
{
   mContextPtr->SetParent(&aPlatformPtr->GetScriptContext());
}

} // namespace WsfL16
