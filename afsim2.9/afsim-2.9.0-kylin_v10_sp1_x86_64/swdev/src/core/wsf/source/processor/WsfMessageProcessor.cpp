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

#include "WsfMessageProcessor.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommandChain.hpp"
#include "WsfControlMessage.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfStatusMessage.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"

// =================================================================================================
WsfMessageProcessor::WsfMessageProcessor(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario)
   , mProcessList()
   , mDefaultProcess(true)
   , mDefaultInternalLinks()
   , mDefaultExternalLinks()
   , mDelayQueue()
   , mMessageVarPtr(nullptr)
   , mCurrentActionPtr(nullptr)
{
   // Indicate that this processor does not need periodic updates
   WsfProcessor::SetUpdateInterval(-1.0);

   // Default to an infinite number of servers like the old WSF_DELAY_PROCESSOR.
   // This makes any delay a simple delay.
   mDelayQueue.SetNumberOfServers(0);
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfMessageProcessor::WsfMessageProcessor(const WsfMessageProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mProcessList(aSrc.mProcessList)
   , mDefaultProcess(aSrc.mDefaultProcess)
   , mDefaultInternalLinks(aSrc.mDefaultInternalLinks)
   , mDefaultExternalLinks(aSrc.mDefaultExternalLinks)
   , mDelayQueue(aSrc.mDelayQueue)
   , mMessageVarPtr(nullptr)
   , mCurrentActionPtr(nullptr)
{
}

// =================================================================================================
// virtual
WsfMessageProcessor::~WsfMessageProcessor() {}

// =================================================================================================
// virtual
WsfProcessor* WsfMessageProcessor::Clone() const
{
   return new WsfMessageProcessor(*this);
}

// =================================================================================================
// virtual
bool WsfMessageProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   mMessageVarPtr = mContextPtr->GetContext().FindVar(UtStringIdLiteral("MESSAGE"));
   if (mMessageVarPtr == nullptr)
   {
      ut::log::error() << "Unable to locate MESSAGE variable.";
   }

   // Initialize the various components
   ok &= mDefaultProcess.Initialize(aSimTime, this);
   ok &= mDefaultInternalLinks.Initialize(aSimTime, this);
   ok &= mDefaultExternalLinks.Initialize(aSimTime, GetPlatform());
   ok &= mDelayQueue.Initialize(aSimTime, GetPlatform(), this, mContextPtr);

   for (std::list<Process>::iterator pli = mProcessList.begin(); pli != mProcessList.end(); ++pli)
   {
      ok &= (*pli).Initialize(aSimTime, this);
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfMessageProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (mDelayQueue.ProcessInput(aInput))
   {
   }
   else if (command == "process")
   {
      std::ostringstream oss;
      oss << "_process_" << mProcessList.size() + 1;

      mProcessList.push_back(Process());
      Process&     process = mProcessList.back();
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!process.ProcessInput(aInput, this, oss.str()))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      if (process.mSelectorList.empty())
      {
         throw UtInput::BadValue(aInput, "No message selectors provided in the 'process' block.");
      }
   }
   else if (command == "default_process")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!mDefaultProcess.ProcessInput(aInput, this, "default_process", true))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "default_routing")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (mDefaultInternalLinks.ProcessInput(aInput))
         {
         }
         else if (mDefaultExternalLinks.ProcessInput(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   // This is primarily for compatibility with WSF_DELAY_PROCESSOR or any other derived class
   // that just wants to the same processing for all received messages.
   else if (mDefaultProcess.ProcessInput(aInput, this, "default_process", false))
   {
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

//! Execute custom actions (scripts) based on the message type.
bool WsfMessageProcessor::ProcessAction(double aSimTime, const WsfMessage& aMessage)
{
   // Use the action as determined by 'ReceiveMessage' (which should always be the case).
   // But if that hasn't been determined then make the determination and use it.
   Action* actionPtr = mCurrentActionPtr;
   if (actionPtr == nullptr)
   {
      actionPtr = FindAction(aSimTime, aMessage);
   }

   bool processed = false;

   if (actionPtr->mScriptPtr != nullptr)
   {
      processed = true;

      // Set the MESSAGE variable to point the message.
      mMessageVarPtr->GetPointer()->SetAppObject(static_cast<void*>(const_cast<WsfMessage*>(&aMessage)),
                                                 aMessage.GetReferenceCount());
      mContextPtr->ExecuteScript(aSimTime, actionPtr->mScriptPtr);
   }

   return processed;
}

// =================================================================================================
// virtual
bool WsfMessageProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Processing message.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Message Processor: " << GetName();
      out.AddNote() << "Message Type: " << aMessage.GetType();
      out.AddNote() << "Message Sub-Type: " << aMessage.GetSubType();
      out.AddNote() << "Serial Number: " << aMessage.GetSerialNumber();
   }

   mSuppressMessage = false;

   // Perform any local processing; this may change the value of mSuppressMessge.
   mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);

   // Message Processor-Specific Processing
   ProcessAction(aSimTime, aMessage);

   bool performRouting = true; // Seemingly a stub variable for later functionality?
   if (performRouting && (!mSuppressMessage))
   {
      SendMessage(aSimTime, aMessage);
   }

   return true;
}

// =================================================================================================
// Derived class version of 'ReceiveMessage'.
// virtual
bool WsfMessageProcessor::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   // Find the actions that apply to this message. This will be from either a 'process' block or the
   // 'default_process' block.

   Action* actionPtr = FindAction(aSimTime, aMessage);

   // If the action indicates the message is to be ignored, skip the rest of the processing.

   bool received = false;
   if (!actionPtr->mIgnoreMessage)
   {
      received = true;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Received message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Message Processor: " << GetName();
         out.AddNote() << "Message Type: " << aMessage.GetType();
         out.AddNote() << "Message Sub-Type: " << aMessage.GetSubType();
         out.AddNote() << "Serial Number: " << aMessage.GetSerialNumber();
      }
      double delayTime = actionPtr->mDelayTime.Draw();
      if (delayTime <= 0.0)
      {
         mCurrentActionPtr = actionPtr;
         ProcessMessage(aSimTime, aMessage);
         mCurrentActionPtr = nullptr;
      }
      else
      {
         ScheduleCompletionEvent(aSimTime, aMessage, actionPtr, delayTime);
      }
   }
   else
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Ignoring message.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Message Processor: " << GetName();
         out.AddNote() << "Message Type: " << aMessage.GetType();
         out.AddNote() << "Message Sub-Type: " << aMessage.GetSubType();
         out.AddNote() << "Serial Number: " << aMessage.GetSerialNumber();
      }
   }
   return received;
}

// =================================================================================================
// Overriden to send the message to all links defined by the current action
// virtual
void WsfMessageProcessor::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   // NOTE: If the current action is the default action, the external and internal links from the
   //       base class are used (see the Action class in the header file for more information).

   if ((mCurrentActionPtr == nullptr) || (mCurrentActionPtr->mIsDefaultAction))
   {
      // No current action (called by a derived class without a current received message),
      // or the action is the default action.
      //
      // Use the routing from the default process.
      if (mExternalLinks.HasLinks() || GetInternalLinks().HasLinks())
      {
         WsfScriptProcessor::SendMessage(aSimTime, aMessage);
      }
      else
      {
         // No routing provided at the base class level. Use the default routing.
         mDefaultInternalLinks.SendMessage(aSimTime, aMessage);
         mDefaultExternalLinks.SendMessage(aSimTime, aMessage);
      }
   }
   else if (mCurrentActionPtr->mExternalLinks.HasLinks() || mCurrentActionPtr->mInternalLinks.HasLinks())
   {
      // Use routing provided in the current action.
      mCurrentActionPtr->mInternalLinks.SendMessage(aSimTime, aMessage);
      mCurrentActionPtr->mExternalLinks.SendMessage(aSimTime, aMessage);
   }
   else
   {
      // No routing in the current action. Use the default routing.
      mDefaultInternalLinks.SendMessage(aSimTime, aMessage);
      mDefaultExternalLinks.SendMessage(aSimTime, aMessage);
   }
}

// =================================================================================================
// virtual
void WsfMessageProcessor::TurnOff(double aSimTime)
{
   mDelayQueue.TurnOff(aSimTime);
   WsfScriptProcessor::TurnOff(aSimTime);
}

// =================================================================================================
// virtual
void WsfMessageProcessor::TurnOn(double aSimTime)
{
   mDelayQueue.TurnOn(aSimTime);
   WsfScriptProcessor::TurnOn(aSimTime);
}

// =================================================================================================
// virtual
void WsfMessageProcessor::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime);
}

// =================================================================================================
// private methods
// =================================================================================================

// =================================================================================================
// private
WsfMessageProcessor::Action* WsfMessageProcessor::FindAction(double aSimTime, const WsfMessage& aMessage)
{
   // Set the MESSAGE variable to point the message.
   mMessageVarPtr->GetPointer()->SetAppObject(static_cast<void*>(const_cast<WsfMessage*>(&aMessage)),
                                              aMessage.GetReferenceCount());

   for (std::list<Process>::iterator pli = mProcessList.begin(); pli != mProcessList.end(); ++pli)
   {
      Process& process = *pli;
      for (std::list<Selector>::const_iterator sli = process.mSelectorList.begin(); sli != process.mSelectorList.end();
           ++sli)
      {
         if ((*sli).Matches(aSimTime, aMessage, this))
         {
            return &process.mAction;
         }
      }
   }
   return &mDefaultProcess;
}

// =================================================================================================
// private
void WsfMessageProcessor::ProcessDelayedMessage(double aSimTime, const WsfMessage& aMessage, Action* aActionPtr)
{
   mCurrentActionPtr = aActionPtr;
   ProcessMessage(aSimTime, aMessage);
   mCurrentActionPtr = nullptr;
}

// =================================================================================================
// private
void WsfMessageProcessor::ScheduleCompletionEvent(double aSimTime, const WsfMessage& aMessage, Action* aActionPtr, double aDelayTime)
{
   DelayRequest* requestPtr = new DelayRequest(this, aMessage, aActionPtr, aDelayTime);
   mDelayQueue.SubmitRequest(aSimTime, requestPtr);
}

// =================================================================================================
// Nested class Action
// =================================================================================================

// =================================================================================================
WsfMessageProcessor::Action::Action(bool aIsDefaultAction)
   : mIsDefaultAction(aIsDefaultAction)
   , mIgnoreMessage(false)
   , mDelayTime(WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE)
   , mScriptPtr(nullptr)
   , mScriptNameId(nullptr)
   , mExternalLinks()
   , mInternalLinks()
{
}

// =================================================================================================
bool WsfMessageProcessor::Action::Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr)
{
   bool ok = true;
   if (mScriptNameId != 0)
   {
      mScriptPtr = aProcessorPtr->mContextPtr->FindScript(mScriptNameId);
      if (mScriptPtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to locate script.";
         out.AddNote() << "Script: " << mScriptNameId;
         ok = false;
      }
   }

   ok &= mDelayTime.Initialize("delay_time", aProcessorPtr, aProcessorPtr->mContextPtr, *aProcessorPtr->GetSimulation());
   if (!mIsDefaultAction)
   {
      ok &= mExternalLinks.Initialize(aSimTime, aProcessorPtr->GetPlatform());
      ok &= mInternalLinks.Initialize(aSimTime, aProcessorPtr);
   }
   return ok;
}

// =================================================================================================
bool WsfMessageProcessor::Action::ProcessInput(UtInput&             aInput,
                                               WsfMessageProcessor* aProcessorPtr,
                                               const std::string&   aNamespace,
                                               bool                 aAllowScript)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "delay_time") || (command == "time_distribution")) // For backward compatibility with WSF_DELAY_PROCESSOR
   {
      mDelayTime.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "ignore_message")
   {
      mIgnoreMessage = true;
   }
   else if (aAllowScript && (command == "script"))
   {
      UtScript* scriptPtr =
         aProcessorPtr->mContextPtr->Compile("process", UtStringIdLiteral("void"), aInput, "end_script", aNamespace);
      mScriptNameId = scriptPtr->GetName();
   }
   else if (mIsDefaultAction)
   {
      // NOTE: If the current action is the default action, the external and internal links from the
      //       base class are used (see the Action class in the header file for more information).
      if (aProcessorPtr->mExternalLinks.ProcessInput(aInput))
      {
      }
      else if (aProcessorPtr->GetInternalLinks().ProcessInput(aInput))
      {
      }
      else
      {
         myCommand = false;
      }
   }
   else
   {
      if (mExternalLinks.ProcessInput(aInput))
      {
      }
      else if (mInternalLinks.ProcessInput(aInput))
      {
      }
      else
      {
         myCommand = false;
      }
   }
   return myCommand;
}

// =================================================================================================
// Nested class Process
// =================================================================================================

// =================================================================================================
WsfMessageProcessor::Process::Process()
   : mSelectorList()
   , mAction(false)
{
}

// =================================================================================================
bool WsfMessageProcessor::Process::Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr)
{
   bool ok = true;
   for (std::list<Selector>::iterator sli = mSelectorList.begin(); sli != mSelectorList.end(); ++sli)
   {
      ok &= (*sli).Initialize(aSimTime, aProcessorPtr);
   }
   ok &= mAction.Initialize(aSimTime, aProcessorPtr);
   return ok;
}

// =================================================================================================
bool WsfMessageProcessor::Process::ProcessInput(UtInput&             aInput,
                                                WsfMessageProcessor* aProcessorPtr,
                                                const std::string&   aNamespace)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "select")
   {
      Selector     selector;
      int          selectorIndex = static_cast<int>(mSelectorList.size());
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (!selector.ProcessInput(inputBlock.GetInput(), aProcessorPtr, aNamespace, selectorIndex))
         {
            throw UtInput::UnknownCommand(inputBlock.GetInput());
         }
      }
      mSelectorList.push_back(selector);
   }
   else if (mAction.ProcessInput(aInput, aProcessorPtr, aNamespace, true))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// Nested class Selector
// =================================================================================================

// =================================================================================================
WsfMessageProcessor::Selector::Selector()
   : mScriptPtr(nullptr)
   , mScriptNameId(nullptr)
   , mMessageTypeId(nullptr)
   , mMessageSubTypeId(nullptr)
   , mSensorNameId(nullptr)
   , mSensorTypeId(nullptr)
   , mSensorModeId(nullptr)
   , mSystemNameId(nullptr)
   , mSenderMask(0)
{
}

// =================================================================================================
bool WsfMessageProcessor::Selector::Initialize(double aSimTime, WsfMessageProcessor* aProcessorPtr)
{
   bool ok = true;
   if (mScriptNameId != 0)
   {
      mScriptPtr = aProcessorPtr->mContextPtr->FindScript(mScriptNameId);
      if (mScriptPtr == nullptr)
      {
         auto out = ut::log::error() << "Unable to resolve reference to script.";
         out.AddNote() << "Script: " << mScriptNameId;
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
bool WsfMessageProcessor::Selector::ProcessInput(UtInput&             aInput,
                                                 WsfMessageProcessor* aProcessorPtr,
                                                 const std::string&   aNamespace,
                                                 int                  aSelectorIndex)
{
   std::string strValue;
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "type")
   {
      aInput.ReadValue(strValue);
      mMessageTypeId = strValue;
   }
   else if (command == "subtype")
   {
      aInput.ReadValue(strValue);
      mMessageSubTypeId = strValue;
   }
   else if (command == "sensor_name")
   {
      aInput.ReadValue(strValue);
      mSensorNameId = strValue;
   }
   else if (command == "sensor_type")
   {
      aInput.ReadValue(strValue);
      mSensorTypeId = strValue;
   }
   else if (command == "sensor_mode")
   {
      aInput.ReadValue(strValue);
      mSensorModeId = strValue;
   }
   else if (command == "system_name")
   {
      aInput.ReadValue(strValue);
      mSystemNameId = strValue;
   }
   else if (command == "sender")
   {
      aInput.ReadValue(strValue);
      if (strValue == "commander")
      {
         mSenderMask |= cCOMMANDER;
      }
      else if (strValue == "peer")
      {
         mSenderMask |= cPEER;
      }
      else if (strValue == "subordinate")
      {
         mSenderMask |= cSUBORDINATE;
      }
      else if (strValue == "self")
      {
         mSenderMask |= cSELF;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "script")
   {
      std::ostringstream oss;
      oss << "selector_" << aSelectorIndex + 1;
      UtScript* scriptPtr = aProcessorPtr->mContextPtr->Compile(oss.str(), "bool", aInput, "end_script", aNamespace);
      mScriptNameId       = scriptPtr->GetName();
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Determines if the message meets the current selectors criteria.
bool WsfMessageProcessor::Selector::Matches(double aSimTime, const WsfMessage& aMessage, WsfMessageProcessor* aProcessorPtr) const
{
   WsfStringId messageTypeId = aMessage.GetType();
   if (mMessageTypeId != 0)
   {
      // When WSF was first developed, the message type for WsfTrackDropMessage was confusingly
      // called WSF_DROP_TRACK_MESSAGE instead of WSF_TRACK_DROP_MESSAGE. If the user defines
      // processing for one of these types then it will also be added for the other type. This will
      // make any transition transparent.

      if ((mMessageTypeId == UtStringIdLiteral("WSF_TRACK_DROP_MESSAGE")) ||
          (mMessageTypeId == UtStringIdLiteral("WSF_DROP_TRACK_MESSAGE")))
      {
         if ((messageTypeId != UtStringIdLiteral("WSF_TRACK_DROP_MESSAGE")) &&
             (messageTypeId != UtStringIdLiteral("WSF_DROP_TRACK_MESSAGE")))
         {
            return false;
         }
      }
      else if (mMessageTypeId != messageTypeId)
      {
         return false;
      }
   }

   if ((mMessageSubTypeId != 0) && (mMessageSubTypeId != aMessage.GetSubType()))
   {
      return false;
   }

   // sensor_name, sensor_type and sensor_mode criteria apply to messages that support
   // them. Typically these are messages generated by a sensor.
   // If one of these criteria is used and the message does not contain that field,
   // the return value will be a null string ID (0) and the comparison will be false.

   if ((mSensorNameId != 0) && (mSensorNameId != aMessage.GetComponentNameId()))
   {
      return false;
   }
   if ((mSensorTypeId != 0) && (mSensorTypeId != aMessage.GetComponentTypeId()))
   {
      return false;
   }
   if ((mSensorModeId != 0) && (mSensorModeId != aMessage.GetComponentModeId()))
   {
      return false;
   }

   if (mSystemNameId != 0)
   {
      // system_name only applies to WSF_STATUS_MESSAGE.
      if (aMessage.GetType() != WsfStatusMessage::GetTypeId())
      {
         return false;
      }
      const WsfStatusMessage& m = static_cast<const WsfStatusMessage&>(aMessage);
      if ((mSystemNameId != 0) && (mSystemNameId != m.GetSystemNameId()))
      {
         return false;
      }
   }

   if (mSenderMask != 0)
   {
      // NOTE: The default command chain should ALWAYS exist on the platform (WsfPlatform ensures it).
      WsfCommandChain* ccPtr =
         aProcessorPtr->GetPlatform()->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
      assert(ccPtr != nullptr);

      auto        origComm = aProcessorPtr->GetSimulation()->GetCommNetworkManager()->GetComm(aMessage.GetSrcAddr());
      WsfStringId senderNameId = origComm != nullptr ? origComm->GetPlatform()->GetNameId() : "";

      if ((mSenderMask & cCOMMANDER) != 0)
      {
         if (senderNameId != ccPtr->GetCommanderNameId())
         {
            return false;
         }
      }

      if ((mSenderMask & cPEER) != 0)
      {
         bool                                 senderMatches = false;
         const WsfCommandChain::PlatformList& platformList  = ccPtr->GetPeers();
         for (WsfCommandChain::PlatformList::const_iterator pli = platformList.begin(); pli != platformList.end(); ++pli)
         {
            WsfPlatform* platformPtr = *pli;
            if (platformPtr->GetNameId() == senderNameId)
            {
               senderMatches = true;
               break;
            }
         }
         if (!senderMatches)
         {
            return false;
         }
      }

      if ((mSenderMask & cSUBORDINATE) != 0)
      {
         bool                                 senderMatches = false;
         const WsfCommandChain::PlatformList& platformList  = ccPtr->GetSubordinates();
         for (WsfCommandChain::PlatformList::const_iterator pli = platformList.begin(); pli != platformList.end(); ++pli)
         {
            WsfPlatform* platformPtr = *pli;
            if (platformPtr->GetNameId() == senderNameId)
            {
               senderMatches = true;
               break;
            }
         }
         if (!senderMatches)
            if (senderNameId.Empty() || (senderNameId == aProcessorPtr->GetPlatform()->GetNameId()))
            {
               return false;
            }
      }

      if ((mSenderMask & cSELF) != 0)
      {
         if (!senderNameId.Empty() && (senderNameId != aProcessorPtr->GetPlatform()->GetNameId()))
         {
            return false;
         }
      }
   }

   if (mScriptPtr != nullptr)
   {
      UtScriptData     retVal;
      UtScriptDataList argList;
      aProcessorPtr->mContextPtr->ExecuteScript(aSimTime, mScriptPtr, retVal, argList);
      if (!retVal.GetBool())
      {
         return false;
      }
   }

   return true;
}

// =================================================================================================
// Nested class DelayRequest
// =================================================================================================

// =================================================================================================
WsfMessageProcessor::DelayRequest::DelayRequest(WsfMessageProcessor* aProcessorPtr,
                                                const WsfMessage&    aMessage,
                                                Action*              aActionPtr,
                                                double               aDelayTime)
   : WsfTimeDelayQueue::Request(aDelayTime)
   , mProcessorPtr(aProcessorPtr)
   , mMessagePtr(aMessage.Clone())
   , mActionPtr(aActionPtr)
   , mPriority(aMessage.GetPriority())
{
}

// =================================================================================================
// virtual
WsfMessageProcessor::DelayRequest::~DelayRequest()
{
   delete mMessagePtr;
}

// =================================================================================================
// virtual
bool WsfMessageProcessor::DelayRequest::CompleteRequest(double aSimTime)
{
   mProcessorPtr->ProcessDelayedMessage(aSimTime, *mMessagePtr, mActionPtr);
   return true;
}
