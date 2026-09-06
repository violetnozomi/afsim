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

#include "WsfTaskProcessor.hpp"

#include <iostream>

#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfTimeDelayQueue.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackStateController.hpp"
#include "script/WsfScriptMessageHandler.hpp"

// =================================================================================================
//! Constructor.
//! @param aScenario The scenario
//! @param aClassName The name of the script class to be associated with the PROCESSOR script variable. The
//! default value is "WsfTaskProcessor", but can be overridden by a derived class if it has provided additional script
//! class methods (note that this class does not provide any additional methods, so it uses WsfTaskProcessor).
WsfTaskProcessor::WsfTaskProcessor(WsfScenario& aScenario, const std::string& aClassName)
   : WsfTaskManager(aScenario, aClassName)
   , mControllerPtr(new WsfTrackStateController(*mContextPtr))
{
}

// =================================================================================================
// protected
WsfTaskProcessor::WsfTaskProcessor(const WsfTaskProcessor& aSrc)
   : WsfTaskManager(aSrc)
   , mControllerPtr(new WsfTrackStateController(*aSrc.mControllerPtr, *mContextPtr))
{
}

// =================================================================================================
// virtual
WsfTaskProcessor::~WsfTaskProcessor() {}

// =================================================================================================
// virtual
const char* WsfTaskProcessor::GetScriptClassName() const
{
   return "WsfTaskProcessor";
}

// =================================================================================================
// virtual
WsfProcessor* WsfTaskProcessor::Clone() const
{
   return new WsfTaskProcessor(*this);
}

// =================================================================================================
// virtual
bool WsfTaskProcessor::Initialize(double aSimTime)
{
   bool ok = WsfTaskManager::Initialize(aSimTime);
   return ok;
}

// =================================================================================================
// virtual
bool WsfTaskProcessor::Initialize2(double aSimTime)
{
   bool ok = WsfTaskManager::Initialize2(aSimTime);
   // The track state controller must be initialized after phase 2 of the task manager phase 2 initialization
   // because it requires access to the track manager.
   ok &= mControllerPtr->Initialize(aSimTime, this, mThinkerPtr.get(), mTrackManagerPtr);
   return ok;
}

// =================================================================================================
// virtual
void WsfTaskProcessor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   WsfTaskManager::PlatformDeleted(aPlatformPtr);
}

// =================================================================================================
// virtual
bool WsfTaskProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   // std::string command(aInput.GetCommand());
   if (mControllerPtr->ProcessInput(GetScenario(), aInput))
   {
   }
   else
   {
      myCommand = WsfTaskManager::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfTaskProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   // NOTE: The task manager MUST be first in this chain.
   //       In general, the task manager will always return 'false' even though it does process the message.
   //       Sometimes a user script will also want to at least know the message has been received and then
   //       perform some additional processing. See note in WsfTaskManager::ProcessMessage.
   bool messageProcessed = true;
   if (WsfTaskManager::ProcessMessage(aSimTime, aMessage))
   {
   }
   else if (!IsExternallyControlled())
   {
      if (mControllerPtr->ProcessMessage(aSimTime, aMessage))
      {
      }
      else if (mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage))
      {
      }
      else
      {
         messageProcessed = false;
      }
   }
   else
   {
      messageProcessed = false;
   }

   return messageProcessed;
}

// =================================================================================================
// virtual
void WsfTaskProcessor::TurnOff(double aSimTime)
{
   WsfTaskManager::TurnOff(aSimTime);
   if (mControllerPtr->IsTurnedOn())
   {
      mControllerPtr->TurnOff(aSimTime);
   }
}

// =================================================================================================
// virtual
void WsfTaskProcessor::TurnOn(double aSimTime)
{
   WsfTaskManager::TurnOn(aSimTime);
   if (!IsExternallyControlled())
   {
      mControllerPtr->TurnOn(aSimTime);
   }
}

// =================================================================================================
// This method is called whenever a task is completed. It is used here simply to
// schedule an immediate evaluation of the track as the task completion
// may result in a state change.
// protected virtual
void WsfTaskProcessor::TaskCompleted(double aSimTime, WsfTask& aTask)
{
   const WsfTrackId& trackId = aTask.GetLocalTrackId();
   if (!IsExternallyControlled())
   {
      WsfStringId currentState = mControllerPtr->StateOfTrack(trackId);
      if (currentState != 0)
      {
         mControllerPtr->EnterState(aSimTime, trackId, currentState);
      }
   }

   WsfTaskManager::TaskCompleted(aSimTime, aTask);
}

// =================================================================================================
// virtual
void WsfTaskProcessor::SetExternallyControlled(double aSimTime, bool aIsExternallyControlled)
{
   WsfTaskManager::SetExternallyControlled(aSimTime, aIsExternallyControlled);
   if (aIsExternallyControlled)
   {
      if (mControllerPtr->IsTurnedOn())
      {
         mControllerPtr->TurnOff(aSimTime);
      }
   }
   else
   {
      if (IsTurnedOn())
      {
         mControllerPtr->TurnOn(aSimTime);
      }
   }
}
