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

#include "WsfLinkedProcessor.hpp"

#include "UtInput.hpp"
#include "WsfExternalLinks.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"

WsfLinkedProcessor::WsfLinkedProcessor(WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mExternalLinks()
{
}

// protected
WsfLinkedProcessor::WsfLinkedProcessor(const WsfLinkedProcessor& aSrc)
   : WsfProcessor(aSrc)
   , mExternalLinks(aSrc.mExternalLinks)

{
}

// virtual
WsfLinkedProcessor::~WsfLinkedProcessor() {}

// virtual
WsfProcessor* WsfLinkedProcessor::Clone() const
{
   return new WsfLinkedProcessor(*this);
}

// virtual
bool WsfLinkedProcessor::Initialize(double aSimTime)
{
   bool ok = WsfProcessor::Initialize(aSimTime);

   // Establish links to external (offboard) recipients
   ok &= mExternalLinks.Initialize(aSimTime, GetPlatform());

   return ok;
}

// virtual
bool WsfLinkedProcessor::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;
   // string command(aInput.GetCommand());
   if (mExternalLinks.ProcessInput(aInput))
   {
      myCommand = true;
   }
   else
   {
      myCommand = WsfProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfLinkedProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   SendMessage(aSimTime, aMessage);
   return true;
}

//! A convenience for derived classes to send a message to linked objects.
//!
//! @param aSimTime The current simulation time.
//! @param aMessage The message to be sent.
// virtual
void WsfLinkedProcessor::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   WsfPlatformPart::SendMessage(aSimTime, aMessage); // Send message to onboard recipients
   if (mExternalLinks.HasLinks())
   {
      mExternalLinks.SendMessage(aSimTime, aMessage); // Send message to offboard recipients
   }
}
