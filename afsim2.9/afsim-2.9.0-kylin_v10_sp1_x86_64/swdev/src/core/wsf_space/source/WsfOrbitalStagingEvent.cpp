// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalStagingEvent.hpp"

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"

WsfOrbitalStagingEvent::~WsfOrbitalStagingEvent() {}

WsfOrbitalEvent* WsfOrbitalStagingEvent::Clone() const
{
   return new WsfOrbitalStagingEvent(*this);
}

bool WsfOrbitalStagingEvent::ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   return aContext.PerformStagingOperation();
}

WsfStringId WsfOrbitalStagingEvent::GetTypeId()
{
   return "PERFORM_STAGING";
}

WsfOrbitalStagingEvent::WsfOrbitalStagingEvent(const WsfScenario& aScenario)
   : WsfOrbitalEvent(aScenario)
{
   SetType(GetTypeId());
}

WsfOrbitalStagingEvent::WsfOrbitalStagingEvent(const WsfOrbitalStagingEvent& aSrc)
   : WsfOrbitalEvent(aSrc)
{
}

void WsfOrbitalStagingEvent::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitOrbitalStagingEvent(this);
}
