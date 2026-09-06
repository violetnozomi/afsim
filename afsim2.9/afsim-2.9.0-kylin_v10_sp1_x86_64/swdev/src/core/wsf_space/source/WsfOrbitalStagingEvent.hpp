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

#ifndef WSFORBITALSTAGINGEVENT_HPP
#define WSFORBITALSTAGINGEVENT_HPP

#include "wsf_space_export.h"

class UtCalendar;
class UtInput;
class UtOrbitalPropagatorBase;
#include "WsfOrbitalEvent.hpp"
#include "WsfStringId.hpp"

class WSF_SPACE_EXPORT WsfOrbitalStagingEvent : public WsfOrbitalEvent
{
public:
   explicit WsfOrbitalStagingEvent(const WsfScenario& aScenario);
   WsfOrbitalStagingEvent(const WsfOrbitalStagingEvent& aSrc);
   ~WsfOrbitalStagingEvent() override;

   //! Make a copy of this object.
   WsfOrbitalEvent* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfPerformStaging"; }

   static WsfStringId GetTypeId();

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

protected:
   bool ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext) override;
};

#endif
