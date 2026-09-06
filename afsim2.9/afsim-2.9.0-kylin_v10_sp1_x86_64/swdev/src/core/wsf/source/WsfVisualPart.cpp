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

#include "WsfVisualPart.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

#include "UtInput.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPartObserver.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfVisualPart::WsfVisualPart(const WsfScenario& aScenario)
   : WsfArticulatedPart(aScenario, cCOMPONENT_ROLE<WsfVisualPart>())
   , mUpdateInterval(0.0)
   , mRequestId(0)
   , mActiveRequestList()
   , mIcon("")
{
   WsfPlatformPart::SetInitiallyTurnedOn(true);
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfVisualPart::WsfVisualPart(const WsfVisualPart& aSrc)
   : WsfArticulatedPart(aSrc)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mRequestId(0)
   , mActiveRequestList()
   , mIcon(aSrc.mIcon)
{
}

// =================================================================================================
// virtual
WsfVisualPart::~WsfVisualPart() {}

// =================================================================================================
// virtual
bool WsfVisualPart::Initialize(double aSimTime)
{
   return WsfArticulatedPart::Initialize(aSimTime);
}

// =================================================================================================
// virtual
bool WsfVisualPart::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "icon")
   {
      std::string icon;
      aInput.ReadValue(icon);
      mIcon = icon;
   }
   else
   {
      myCommand = WsfArticulatedPart::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Update the state of the visual part to the supplied time value.
// virtual
void WsfVisualPart::Update(double aSimTime) {}

// =================================================================================================
const char* WsfVisualPart::GetScriptClassName() const
{
   return "WsfVisualPart";
}

// =================================================================================================
const int* WsfVisualPart::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_VISUAL,
                               cWSF_COMPONENT_ARTICULATED_PART,
                               cWSF_COMPONENT_PLATFORM_PART,
                               cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* WsfVisualPart::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_VISUAL)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_ARTICULATED_PART)
   {
      return dynamic_cast<WsfArticulatedPart*>(this);
   }
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return dynamic_cast<WsfPlatformPart*>(this);
   }
   return nullptr;
}


void WsfVisualPart::TurnOff(double aSimTime)
{
   if (IsTurnedOn())
   {
      WsfArticulatedPart::TurnOff(aSimTime);
      WsfObserver::VisualPartTurnedOff(GetSimulation())(aSimTime, this);
   }
}

void WsfVisualPart::TurnOn(double aSimTime)
{
   if (!IsTurnedOn())
   {
      WsfArticulatedPart::TurnOn(aSimTime);
      WsfObserver::VisualPartTurnedOn(GetSimulation())(aSimTime, this);
   }
}

void WsfVisualPart::SetNonOperational(double aSimTime)
{
   WsfArticulatedPart::SetNonOperational(aSimTime);
   WsfObserver::VisualPartNonOperational(GetSimulation())(aSimTime, this);
}

void WsfVisualPart::SetOperational(double aSimTime)
{
   WsfArticulatedPart::SetOperational(aSimTime);
   WsfObserver::VisualPartOperational(GetSimulation())(aSimTime, this);
}

void WsfVisualPart::PartBroken(double aSimTime)
{
   WsfArticulatedPart::PartBroken(aSimTime);
   WsfObserver::VisualPartBroken(GetSimulation())(aSimTime, this);
}
