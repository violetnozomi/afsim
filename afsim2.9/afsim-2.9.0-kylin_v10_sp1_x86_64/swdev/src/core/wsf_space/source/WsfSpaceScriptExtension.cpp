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

#include "WsfSpaceScriptExtension.hpp"

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfScriptAtmosphericDragTerm.hpp"
#include "WsfScriptConjunctionReport.hpp"
#include "WsfScriptConstellation.hpp"
#include "WsfScriptConstellationMaker.hpp"
#include "WsfScriptDE_File.hpp"
#include "WsfScriptEarthJ2Term.hpp"
#include "WsfScriptEarthMonopoleTerm.hpp"
#include "WsfScriptIntegratingSpaceMover.hpp"
#include "WsfScriptJupiterMonopoleTerm.hpp"
#include "WsfScriptLibrationPoint.hpp"
#include "WsfScriptMoonMonopoleTerm.hpp"
#include "WsfScriptNASA_BreakupModel.hpp"
#include "WsfScriptOrbitDesigner.hpp"
#include "WsfScriptOrbitalConjunctionProcessor.hpp"
#include "WsfScriptOrbitalDynamics.hpp"
#include "WsfScriptOrbitalDynamicsTerm.hpp"
#include "WsfScriptOrbitalIntegrator.hpp"
#include "WsfScriptOrbitalReferenceFrame.hpp"
#include "WsfScriptPrinceDormand45.hpp"
#include "WsfScriptPrinceDormand78.hpp"
#include "WsfScriptSatelliteBreakupFragment.hpp"
#include "WsfScriptScriptedDynamicsTerm.hpp"
#include "WsfScriptSolarTerminator.hpp"
#include "WsfScriptSpaceMoverClass.hpp"
#include "WsfScriptSunMonopoleTerm.hpp"
#include "WsfScriptTargetPoint.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceScriptOrbitalBlendedCost.hpp"
#include "WsfSpaceScriptOrbitalManeuverSequence.hpp"
#include "WsfSpaceScriptOrbitalManeuvers.hpp"
#include "WsfSpaceScriptOrbitalOptimizationCost.hpp"
#include "WsfSpaceScriptOrientation.hpp"
#include "script/WsfScriptDefs.hpp"

namespace wsf
{

namespace space
{

namespace ScriptExtensions
{
void Register(UtScriptTypes& aScriptTypes)
{
   // "New" Types are WsfOrbitalEventCondition and WsfOrbitalMissionSequence.
   aScriptTypes.Register(ut::make_unique<script::OrbitalEventCondition>("WsfOrbitalEventCondition", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::OrbitalMissionSequence>("WsfOrbitalMissionSequence", &aScriptTypes));

   // Deprecated types are WsfOrbitalManeuverCondition and WsfOrbitalManeuverSequence.
   aScriptTypes.Register(ut::make_unique<script::OrbitalManeuverCondition>("WsfOrbitalManeuverCondition", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::OrbitalManeuverSequence>("WsfOrbitalManeuverSequence", &aScriptTypes));

   aScriptTypes.Register(ut::make_unique<script::OrbitalEvent>("WsfOrbitalEvent", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::OrbitalManeuver>("WsfOrbitalManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptSpaceMoverClass>("WsfSpaceMover", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeEccentricity>("WsfChangeEccentricity", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeInclination>("WsfChangeInclination", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeRAAN>("WsfChangeRAAN", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeRAAN_Inclination>("WsfChangeRAAN_Inclination", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeSemiMajorAxis>("WsfChangeSemiMajorAxis", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Circularize>("WsfCircularize", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::CompoundManeuver>("WsfCompoundManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::DeltaV>("WsfDeltaV_Maneuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Drift>("WsfDriftManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::HohmannTransfer>("WsfHohmannTransfer", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Intercept>("WsfInterceptManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::MatchVelocity>("WsfMatchVelocity", &aScriptTypes));
   aScriptTypes.Register(
      ut::make_unique<script::NaturalMotionCircumnavigation>("WsfNaturalMotionCircumnavigation", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Normal>("WsfNormalManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::OrbitalEventCondition>("WsfOrbitalEventCondition", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::PerformStaging>("WsfPerformStaging", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Rendezvous>("WsfRendezvousManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Scripted>("WsfOrbitalScriptEvent", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Tangent>("WsfTangentManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Target>("WsfTargetManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::Teardrop>("WsfTeardropManeuver", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::ChangeAttitude>("WsfChangeAttitude", &aScriptTypes));

   aScriptTypes.Register(ut::make_unique<WsfScriptTargetPoint>("WsfTargetPoint", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptTargetOffsetFrame>("WsfTargetOffsetFrame", &aScriptTypes));

   aScriptTypes.Register(
      ut::make_unique<wsf::space::ScriptOrbitalReferenceFrame>("WsfOrbitalReferenceFrame", &aScriptTypes));

   aScriptTypes.Register(ut::make_unique<script::OrbitalOptimizationCost>("WsfOrbitalOptimizationCost", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<script::OrbitalBlendedCost>("WsfOrbitalBlendedCost", &aScriptTypes));

   // Report type for Conjunction Assessment
   aScriptTypes.Register(ut::make_unique<script::Conjunction>("WsfOrbitalConjunctionReport", &aScriptTypes));

   // Conjunction Processor
   aScriptTypes.Register(
      ut::make_unique<WsfScriptOrbitalConjunctionProcessor>("WsfOrbitalConjunctionProcessor", &aScriptTypes));

   // Solar Terminator Modeling
   aScriptTypes.Register(ut::make_unique<WsfScriptSolarTerminator>("WsfSolarTerminator", &aScriptTypes));

   // Orientation
   aScriptTypes.Register(ut::make_unique<script::Orientation>("WsfSpaceOrientation", &aScriptTypes));
   // Satellite Breakup Modeling
   aScriptTypes.Register(ut::make_unique<WsfScriptNASA_BreakupModel>(&aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptSatelliteBreakupFragment>(&aScriptTypes));

   // Constellation Maker-related
   aScriptTypes.Register(ut::make_unique<WsfScriptConstellation>(&aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptConstellationMaker>(&aScriptTypes));

   // Integrating Propagator
   aScriptTypes.Register(ut::make_unique<WsfScriptIntegratingSpaceMover>("WsfIntegratingSpaceMover", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptOrbitalDynamics>(&aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptOrbitalDynamicsTerm>("WsfOrbitalDynamicsTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptEarthMonopoleTerm>("WsfEarthMonopoleTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptMoonMonopoleTerm>("WsfMoonMonopoleTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptSunMonopoleTerm>("WsfSunMonopoleTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptJupiterMonopoleTerm>("WsfJupiterMonopoleTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptEarthJ2Term>("WsfEarthJ2Term", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptAtmosphericDragTerm>("WsfAtmosphericDragTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptScriptedDynamicsTerm>("WsfScriptedDynamicsTerm", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptOrbitalIntegrator>("WsfOrbitalIntegrator", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptPrinceDormand45Integrator>("WsfPrinceDormand45Integrator", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptPrinceDormand78Integrator>("WsfPrinceDormand78Integrator", &aScriptTypes));

   aScriptTypes.Register(ut::make_unique<WsfScriptLibrationPoint>("WsfLibrationPoint", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptLibrationPointSystem>("WsfLibrationPointSystem", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptLibrationPointSelection>("WsfLibrationPointSelection", &aScriptTypes));

   // Orbit Designer
   aScriptTypes.Register(ut::make_unique<script::OrbitDesigner>("WsfOrbitDesigner", &aScriptTypes));

   // DE File -- NOTE: This is not documented
   aScriptTypes.Register(ut::make_unique<WsfScriptDE_File>("WsfDE_File", &aScriptTypes));
}
} // namespace ScriptExtensions

} // namespace space

} // namespace wsf
