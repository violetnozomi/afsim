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

#ifndef WSFORBITALEVENTVISITOR_HPP
#define WSFORBITALEVENTVISITOR_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalAttitudeChangeEvent.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfOrbitalScriptEvent.hpp"
#include "WsfOrbitalStagingEvent.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeEccentricity.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeInclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeRAAN_Inclination.hpp"
#include "maneuvers/WsfOrbitalManeuversChangeSemiMajorAxis.hpp"
#include "maneuvers/WsfOrbitalManeuversCircularize.hpp"
#include "maneuvers/WsfOrbitalManeuversCompound.hpp"
#include "maneuvers/WsfOrbitalManeuversDeltaV.hpp"
#include "maneuvers/WsfOrbitalManeuversDrift.hpp"
#include "maneuvers/WsfOrbitalManeuversHohmannTransfer.hpp"
#include "maneuvers/WsfOrbitalManeuversIntercept.hpp"
#include "maneuvers/WsfOrbitalManeuversMatchVelocity.hpp"
#include "maneuvers/WsfOrbitalManeuversNaturalMotionCircumnavigation.hpp"
#include "maneuvers/WsfOrbitalManeuversNormal.hpp"
#include "maneuvers/WsfOrbitalManeuversRendezvous.hpp"
#include "maneuvers/WsfOrbitalManeuversTangent.hpp"
#include "maneuvers/WsfOrbitalManeuversTarget.hpp"
#include "maneuvers/WsfOrbitalManeuversTeardrop.hpp"

class WSF_SPACE_EXPORT WsfOrbitalEventVisitor
{
public:
   WsfOrbitalEventVisitor()          = default;
   virtual ~WsfOrbitalEventVisitor() = default;

   virtual void VisitMissionSequence(WsfOrbitalMissionSequence* aSequencePtr);

   virtual void VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr);
   virtual void VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr);
   virtual void VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr);
   virtual void VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr);
   virtual void VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr);
   virtual void VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr);
   virtual void VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr);
   virtual void VisitDrift(WsfOrbitalManeuvers::Drift* aEventPtr);
   virtual void VisitDeltaV(WsfOrbitalManeuvers::DeltaV* aEventPtr);
   virtual void VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr);
   virtual void VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr);
   virtual void VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr);
   virtual void VisitNaturalMotionCircumnavigation(WsfOrbitalManeuvers::NaturalMotionCircumnavigation* aEventPtr);
   virtual void VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr);
   virtual void VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr);
   virtual void VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr);
   virtual void VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr);
   virtual void VisitTeardrop(WsfOrbitalManeuvers::Teardrop* aEventPtr);
   virtual void VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr);
   virtual void VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr);
   virtual void VisitOrbitalAttitudeChangeEvent(WsfOrbitalAttitudeChangeEvent* aEventPtr);
};

#endif // WSFORBITALEVENTVISITOR_HPP
