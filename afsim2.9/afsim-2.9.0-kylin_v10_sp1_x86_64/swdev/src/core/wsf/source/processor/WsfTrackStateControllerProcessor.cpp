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

#include "WsfTrackStateControllerProcessor.hpp"

#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfTimeDelayQueue.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackStateController.hpp"
#include "script/WsfScriptContext.hpp"

//! Constructor.
//! @param aScenario The scenario
//! @param aClassName The name of the script class to be associated with the PROCESSOR script variable. The
//! default value is "WsfTrackStateController", but can be overridden by a derived class if it has provided
//! additional script class methods.
WsfTrackStateControllerProcessor::WsfTrackStateControllerProcessor(
   WsfScenario&       aScenario,
   const std::string& aClassName /* = "WsfTrackStateController" */)
   : WsfScriptProcessor(aScenario, aClassName, "PROCESSOR")
   , mThinkerPtr(new WsfTimeDelayQueue())
   , mControllerPtr(new WsfTrackStateController(*mContextPtr))
{
}

WsfTrackStateControllerProcessor::WsfTrackStateControllerProcessor(const WsfTrackStateControllerProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mThinkerPtr(new WsfTimeDelayQueue(*aSrc.mThinkerPtr))
   , mControllerPtr(new WsfTrackStateController(*aSrc.mControllerPtr, *mContextPtr))
{
}

// virtual
WsfTrackStateControllerProcessor::~WsfTrackStateControllerProcessor() {}

// virtual
WsfProcessor* WsfTrackStateControllerProcessor::Clone() const
{
   return new WsfTrackStateControllerProcessor(*this);
}

// virtual
bool WsfTrackStateControllerProcessor::Initialize(double aSimTime)
{
   // TODO - For now assume we're working with the master track list.
   auto trackManagerPtr = &(GetPlatform()->GetTrackManager());

   bool ok = WsfScriptProcessor::Initialize(aSimTime);
   ok &= mThinkerPtr->Initialize(aSimTime, GetPlatform(), this, mContextPtr);
   ok &= mControllerPtr->Initialize(aSimTime, this, mThinkerPtr.get(), trackManagerPtr);
   return ok;
}

// virtual
bool WsfTrackStateControllerProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mThinkerPtr->ProcessInput(aInput))
   {
   }
   else if (mControllerPtr->ProcessInput(GetScenario(), aInput))
   {
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfTrackStateControllerProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool messageProcessed = true;
   if (mControllerPtr->ProcessMessage(aSimTime, aMessage))
   {
   }
   else
   {
      messageProcessed = WsfScriptProcessor::ProcessMessage(aSimTime, aMessage);
   }
   return messageProcessed;
}

// virtual
void WsfTrackStateControllerProcessor::TurnOff(double aSimTime)
{
   WsfScriptProcessor::TurnOff(aSimTime);
   mControllerPtr->TurnOff(aSimTime);
}

// virtual
void WsfTrackStateControllerProcessor::TurnOn(double aSimTime)
{
   WsfScriptProcessor::TurnOn(aSimTime);
   mControllerPtr->TurnOn(aSimTime);
}
