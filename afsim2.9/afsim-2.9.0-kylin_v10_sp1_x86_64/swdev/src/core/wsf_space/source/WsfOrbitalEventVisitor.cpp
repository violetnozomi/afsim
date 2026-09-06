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

#include "WsfOrbitalEventVisitor.hpp"

void WsfOrbitalEventVisitor::VisitMissionSequence(WsfOrbitalMissionSequence* aSequencePtr) {}

void WsfOrbitalEventVisitor::VisitChangeEccentricity(WsfOrbitalManeuvers::ChangeEccentricity* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitChangeInclination(WsfOrbitalManeuvers::ChangeInclination* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitChangeRAAN(WsfOrbitalManeuvers::ChangeRAAN* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitChangeRAAN_Inclination(WsfOrbitalManeuvers::ChangeRAAN_Inclination* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitChangeSemiMajorAxis(WsfOrbitalManeuvers::ChangeSemiMajorAxis* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitCircularize(WsfOrbitalManeuvers::Circularize* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitCompoundManeuver(WsfOrbitalManeuvers::CompoundManeuver* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitDrift(WsfOrbitalManeuvers::Drift* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitDeltaV(WsfOrbitalManeuvers::DeltaV* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitHohmannTransfer(WsfOrbitalManeuvers::HohmannTransfer* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitIntercept(WsfOrbitalManeuvers::Intercept* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitMatchVelocity(WsfOrbitalManeuvers::MatchVelocity* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitNaturalMotionCircumnavigation(WsfOrbitalManeuvers::NaturalMotionCircumnavigation* aEventPtr)
{
}

void WsfOrbitalEventVisitor::VisitNormal(WsfOrbitalManeuvers::Normal* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitRendezvous(WsfOrbitalManeuvers::Rendezvous* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitTangent(WsfOrbitalManeuvers::Tangent* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitTarget(WsfOrbitalManeuvers::Target* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitTeardrop(WsfOrbitalManeuvers::Teardrop* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitOrbitalScriptEvent(WsfOrbitalScriptEvent* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitOrbitalStagingEvent(WsfOrbitalStagingEvent* aEventPtr) {}

void WsfOrbitalEventVisitor::VisitOrbitalAttitudeChangeEvent(WsfOrbitalAttitudeChangeEvent* aEventPtr) {}
