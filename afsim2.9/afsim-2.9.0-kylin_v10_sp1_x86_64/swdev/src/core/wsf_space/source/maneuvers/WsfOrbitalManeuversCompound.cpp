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

#include "WsfOrbitalManeuversCompound.hpp"

#include <iostream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfOrbitalEventVisitor.hpp"

namespace WsfOrbitalManeuvers
{

CompoundManeuver::CompoundManeuver(const WsfScenario& aScenario)
   : WsfOrbitalMissionSequence(aScenario)
{
   SetType(GetTypeId());
}

WsfStringId CompoundManeuver::GetTypeId()
{
   return "COMPOUND";
}

bool WsfOrbitalManeuvers::CompoundManeuver::ValidateParameterRanges(const std::string& aExecutingPlatformName) const
{
   bool retval{true};
   auto iter = GetSequence().begin();
   retval    = (*iter)->ValidateParameterRanges(aExecutingPlatformName) && retval;
   ++iter;
   retval = (*iter)->ValidateParameterRanges(aExecutingPlatformName) && retval;
   retval = WsfOrbitalEvent::ValidateParameterRanges(aExecutingPlatformName) && retval;
   return retval;
}

bool CompoundManeuver::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   bool ok = (GetSequence().size() == 2);
   if (ok)
   {
      MissionEvents::const_iterator iter             = GetSequence().begin();
      auto                          maneuver1CopyPtr = *iter;
      maneuver1CopyPtr->Initialize(aEpoch, aContext);
      ++iter;
      auto maneuver2CopyPtr = *iter;
      maneuver2CopyPtr->Initialize(aEpoch, aContext);
      if (maneuver2CopyPtr->GetStartTime() < maneuver1CopyPtr->GetStartTime())
      {
         // swap the two.
         std::unique_ptr<WsfOrbitalEvent> maneuver2Ptr(iter->get());
         GetSequence().pop_back();
         GetSequence().emplace_front(std::move(maneuver2Ptr));
      }
      ok = WsfOrbitalMissionSequence::Initialize(aEpoch, aContext);
   }
   else
   {
      auto logger = ut::log::error() << "Compound maneuvers must consist of two maneuvers.";
      logger.AddNote() << "Type: " << GetType();
   }
   return ok;
}

void CompoundManeuver::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitCompoundManeuver(this);
}

//! Return a copy of the first event in this compound maneuver.
//! Note that if this is called before Initialize(), the order of the
//! two events may be swapped for execution.
//! \return - The first event if it exists; nullptr otherwise.
std::unique_ptr<WsfOrbitalEvent> CompoundManeuver::GetFirstEvent() const
{
   std::unique_ptr<WsfOrbitalEvent> retvalPtr{nullptr};
   if (!GetSequence().empty())
   {
      retvalPtr.reset(GetSequence().front()->Clone());
   }
   return retvalPtr;
}

//! Return a copy of the second event in this compound maneuver.
//! Note that if this is called before Initialize(), the order of the
//! two events may be swapped for execution.
//! \return - The second event if it exists; nullptr otherwise.
std::unique_ptr<WsfOrbitalEvent> CompoundManeuver::GetSecondEvent() const
{
   std::unique_ptr<WsfOrbitalEvent> retvalPtr{nullptr};
   if (GetSequence().size() > 1)
   {
      auto iter = GetSequence().begin();
      ++iter;
      retvalPtr.reset((*iter)->Clone());
   }
   return retvalPtr;
}

//! Set the first event in the compound maneuver.
//! This should not be called after Initialize() has been invoked.
void CompoundManeuver::SetFirstEvent(std::unique_ptr<WsfOrbitalEvent>&& aEventPtr)
{
   if (aEventPtr)
   {
      MissionEvents& evts = GetSequence();
      if (!evts.empty())
      {
         evts.front() = std::move(aEventPtr);
      }
      else
      {
         evts.emplace_back(std::move(aEventPtr));
      }
   }
}

//! Set the second event in the compound maneuver.
//! This should not be called after Initialize() has been invoked.
//! If this is called before a first event is set, this will add a null event
//! as the first event. It is the caller's responsibility to assure that a
//! first event is assigned to this maneuver.
void CompoundManeuver::SetSecondEvent(std::unique_ptr<WsfOrbitalEvent>&& aEventPtr)
{
   if (aEventPtr)
   {
      MissionEvents& evts = GetSequence();
      if (evts.empty())
      {
         evts.emplace_back(nullptr);
         evts.emplace_back(std::move(aEventPtr));
      }
      else if (evts.size() == 1)
      {
         evts.emplace_back(std::move(aEventPtr));
      }
      else
      {
         auto iter = evts.begin();
         ++iter;
         *iter = std::move(aEventPtr);
      }
   }
}

} // namespace WsfOrbitalManeuvers
