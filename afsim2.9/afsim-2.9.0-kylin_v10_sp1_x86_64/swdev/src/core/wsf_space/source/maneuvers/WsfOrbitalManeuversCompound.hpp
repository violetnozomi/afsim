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

#ifndef WSFORBITALMANEUVERSCOMPOUND_HPP
#define WSFORBITALMANEUVERSCOMPOUND_HPP

#include "wsf_space_export.h"

#include <memory>

class UtCalendar;
class WsfOrbitalEvent;
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfStringId.hpp"

//! A set of useful orbital maneuvers (see WsfOrbitalEvent).
namespace WsfOrbitalManeuvers
{

//! A Maneuver consisting of two separate maneuvers, executed closely together in time.
//! The maneuvers will be sorted in time such that constraint met earlier in time is executed first.
class WSF_SPACE_EXPORT CompoundManeuver : public WsfOrbitalMissionSequence
{
public:
   explicit CompoundManeuver(const WsfScenario& aScenario);
   CompoundManeuver(const CompoundManeuver& aSrc) = default;
   ;
   ~CompoundManeuver() override = default;

   static WsfStringId GetTypeId();
   CompoundManeuver*  Clone() const override { return new CompoundManeuver(*this); }
   bool               ValidateParameterRanges(const std::string& aExecutingPlatformName) const override;
   bool               Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;
   const char*        GetScriptClassName() const override { return "WsfCompoundManeuver"; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

   std::unique_ptr<WsfOrbitalEvent> GetFirstEvent() const;
   std::unique_ptr<WsfOrbitalEvent> GetSecondEvent() const;

   void SetFirstEvent(std::unique_ptr<WsfOrbitalEvent>&& aEventPtr);
   void SetSecondEvent(std::unique_ptr<WsfOrbitalEvent>&& aEventPtr);
};

} // namespace WsfOrbitalManeuvers

#endif
