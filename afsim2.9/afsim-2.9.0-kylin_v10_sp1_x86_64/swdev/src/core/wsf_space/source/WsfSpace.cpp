// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpace.hpp"

#include "UtAttribute.hpp"
#include "UtInput.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfAccessReport.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAtmosphereScenarioExtension.hpp"
#include "WsfAtmosphereScriptRegistration.hpp"
#include "WsfAttitudeControllerTypes.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfConstellationManager.hpp"
#include "WsfDE_FileManager.hpp"
#include "WsfDefaultTrackExtrapolationStrategy.hpp"
#include "WsfEclipseEventManager.hpp"
#include "WsfEclipseReport.hpp"
#include "WsfEventOutput.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfInstantAttitudeController.hpp"
#include "WsfIntegratingPropagator.hpp"
#include "WsfIntegratingSpaceMover.hpp"
#include "WsfJ2_PerturbationOrbitalPropagator.hpp"
#include "WsfKeplerianOrbitalPropagator.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfNORAD_SpaceMover.hpp"
#include "WsfOpticalSignatureTypes.hpp"
#include "WsfOrbitDeterminationFusion.hpp"
#include "WsfOrbitDeterminationKalmanFilter.hpp"
#include "WsfOrbitalAttitudeChangeEvent.hpp"
#include "WsfOrbitalConjunctionProcessor.hpp"
#include "WsfOrbitalDynamicsTermTypes.hpp"
#include "WsfOrbitalIntegratorTypes.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfOrbitalManeuveringTypes.hpp"
#include "WsfOrbitalMissionEventTypes.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
#include "WsfOrbitalScriptEvent.hpp"
#include "WsfOrbitalStagingEvent.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRateLimitedAttitudeController.hpp"
#include "WsfRocketOrbitalManeuvering.hpp"
#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfSimpleOrbitalManeuvering.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceEventPipe.hpp"
#include "WsfSpaceEventResults.hpp"
#include "WsfSpaceKinematicStateExtrapolation.hpp"
#include "WsfSpaceMover.hpp"
#include "WsfSpaceObserver.hpp"
#include "WsfSpaceOpticalSignature.hpp"
#include "WsfSpaceOrbitalPropagatorConditionTypes.hpp"
#include "WsfSpaceScriptExtension.hpp"
#include "WsfTrackExtrapolationStrategyTypes.hpp"
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
#include "maneuvers/WsfOrbitalManeuversTeardrop.hpp"
#include "script/WsfScriptObserver.hpp"

namespace
{
// This is the name under which the application, scenario and simulations extensions are known.
const std::string sExtensionName("wsf_space");

// The actual application extension
class ApplicationExtension : public WsfApplicationExtension
{
public:
   void AddedToApplication(WsfApplication& aApplication) override;
   void ScenarioCreated(WsfScenario& aScenario) override;
};

void ApplicationExtension::AddedToApplication(WsfApplication& aApplication)
{
   wsf::space::ScriptExtensions::Register(*(aApplication.GetScriptTypes()));
   wsf::space::AtmosphereScriptRegistration(*(aApplication.GetScriptTypes()));
}

void ApplicationExtension::ScenarioCreated(WsfScenario& aScenario)
{
   aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfSpaceExtension>());
   aScenario.RegisterExtension("wsf_configure_access_report", ut::make_unique<WsfConfigureAccessReport>());
   aScenario.RegisterExtension("wsf_configure_eclipse_report", ut::make_unique<WsfConfigureEclipseReport>());
   aScenario.RegisterExtension("wsf_configure_eclipse_event_manager", ut::make_unique<WsfConfigureEclipseEventManger>());
   aScenario.RegisterExtension(wsf::space::AtmosphereScenarioExtension::cEXTENSION_NAME,
                               ut::make_unique<wsf::space::AtmosphereScenarioExtension>());
}

void Register_event_output(wsf::event::output::SimulationExtension& aEventOutput)
{
   auto simPtr = &aEventOutput.GetSimulation();

   using namespace wsf::event;
   aEventOutput.AddEvent<EclipseEntry>("ECLIPSE_ENTRY", WsfObserver::EclipseEntry(simPtr));
   aEventOutput.AddEvent<EclipseExit>("ECLIPSE_EXIT", WsfObserver::EclipseExit(simPtr));
   aEventOutput.AddEvent<OrbitDeterminationInitiated>("ORBIT_DETERMINATION_INITIATED",
                                                      WsfObserver::OrbitDeterminationInitiated(simPtr));
   aEventOutput.AddEvent<OrbitDeterminationUpdated>("ORBIT_DETERMINATION_UPDATED",
                                                    WsfObserver::OrbitDeterminationUpdated(simPtr));
   aEventOutput.AddEvent<OrbitalManeuverInitiated>("ORBITAL_MANEUVER_INITIATED",
                                                   WsfObserver::OrbitalManeuverInitiated(simPtr));
   aEventOutput.AddEvent<OrbitalManeuverUpdated>("ORBITAL_MANEUVER_UPDATED", WsfObserver::OrbitalManeuverUpdated(simPtr));
   aEventOutput.AddEvent<OrbitalManeuverCanceled>("ORBITAL_MANEUVER_CANCELED",
                                                  WsfObserver::OrbitalManeuverCanceled(simPtr));
   aEventOutput.AddEvent<OrbitalManeuverCompleted>("ORBITAL_MANEUVER_COMPLETED",
                                                   WsfObserver::OrbitalManeuverCompleted(simPtr));
}

void EclipseScriptPacker(UtScriptDataPacker& aScriptArgs, WsfSpaceMoverBase* aSpaceMoverPtr)
{
   aScriptArgs << aSpaceMoverPtr->GetPlatform() << aSpaceMoverPtr;
}

void ManeuverScriptPacker(UtScriptDataPacker& aScriptArgs, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver)
{
   aScriptArgs << aSpaceMoverPtr->GetPlatform() << aSpaceMoverPtr << aManeuver;
}

void OrbitDeterminationScriptPacker(UtScriptDataPacker&            aScriptArgs,
                                    WsfPlatform*                   aPlatformPtr,
                                    const WsfLocalTrack&           aLocalTrack,
                                    const UtOrbitalPropagatorBase& aPropagator)
{
   aScriptArgs << aPlatformPtr << aLocalTrack;
}

void Register_script_observer(WsfScriptObserver& aScriptObserver)
{
   auto& sim = aScriptObserver.GetSimulation();
   aScriptObserver.AddEvent("ECLIPSE_ENTRY",
                            WsfObserver::EclipseEntry(&sim),
                            "EclipseEntry",
                            "WsfPlatform, WsfSpaceMover",
                            UtStd::Bind(&EclipseScriptPacker));
   aScriptObserver.AddEvent("ECLIPSE_EXIT",
                            WsfObserver::EclipseExit(&sim),
                            "EclipseExit",
                            "WsfPlatform, WsfSpaceMover",
                            UtStd::Bind(&EclipseScriptPacker));
   aScriptObserver.AddEvent("ORBITAL_MANEUVER_INITIATED",
                            WsfObserver::OrbitalManeuverInitiated(&sim),
                            "OrbitalManeuverInitiated",
                            "WsfPlatform, WsfSpaceMover, WsfOrbitalManeuver",
                            UtStd::Bind(&ManeuverScriptPacker));
   aScriptObserver.AddEvent("ORBITAL_MANEUVER_UPDATED",
                            WsfObserver::OrbitalManeuverUpdated(&sim),
                            "OrbitalManeuverUpdated",
                            "WsfPlatform, WsfSpaceMover, WsfOrbitalManeuver",
                            UtStd::Bind(&ManeuverScriptPacker));
   aScriptObserver.AddEvent("ORBITAL_MANEUVER_CANCELED",
                            WsfObserver::OrbitalManeuverCanceled(&sim),
                            "OrbitalManeuverUpdated",
                            "WsfPlatform, WsfSpaceMover, WsfOrbitalManeuver",
                            UtStd::Bind(&ManeuverScriptPacker));
   aScriptObserver.AddEvent("ORBITAL_MANEUVER_COMPLETED",
                            WsfObserver::OrbitalManeuverCompleted(&sim),
                            "OrbitalManeuverCompleted",
                            "WsfPlatform, WsfSpaceMover, WsfOrbitalManeuver",
                            UtStd::Bind(&ManeuverScriptPacker));
   aScriptObserver.AddEvent("ORBIT_DETERMINATION_INITIATED",
                            WsfObserver::OrbitDeterminationInitiated(&sim),
                            "OrbitDeterminationInitiated",
                            "WsfPlatform, WsfLocalTrack",
                            UtStd::Bind(&OrbitDeterminationScriptPacker));
   aScriptObserver.AddEvent("ORBIT_DETERMINATION_UPDATED",
                            WsfObserver::OrbitDeterminationUpdated(&sim),
                            "OrbitDeterminationUpdated",
                            "WsfPlatform, WsfLocalTrack",
                            UtStd::Bind(&OrbitDeterminationScriptPacker));
}
} // namespace

void WSF_SPACE_EXPORT Register_wsf_space(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_space"))
   {
      aApplication.RegisterFeature("wsf_space");
      aApplication.RegisterFeature("space", "wsf_space");
      aApplication.RegisterExtension("wsf_space", ut::make_unique<ApplicationExtension>());
      aApplication.ExtensionDepends("wsf_space", "wsf_mil", true);
   }
}

//! Returns a reference to the space extension associated with a given scenario.
//! @note This will throw an exception if the extension does not exist.
WsfSpaceExtension& WsfSpaceExtension::Get(const WsfScenario& aScenario)
{
   WsfSpaceExtension* spaceExtPtr(static_cast<WsfSpaceExtension*>(aScenario.FindExtension("wsf_space")));
   if (spaceExtPtr != nullptr)
   {
      return *spaceExtPtr;
   }
   throw UtException("wsf_space extension does not exist");
}

void WsfSpaceExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   scenario.AddTypeList(ut::make_unique<wsf::space::OrbitalPropagatorConditionTypes>(scenario));

   WsfFusionStrategyTypes::Get(scenario).Add("orbit_determination", ut::make_unique<WsfOrbitDeterminationFusion>(scenario));
   auto orbitalMissionEventTypes = ut::make_unique<WsfOrbitalMissionEventTypes>(GetScenario());
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Tangent::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Tangent>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::CompoundManeuver::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::CompoundManeuver>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::ChangeEccentricity::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::ChangeEccentricity>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Circularize::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Circularize>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::ChangeInclination::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::ChangeInclination>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::ChangeRAAN::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::ChangeRAAN_Inclination::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::ChangeRAAN_Inclination>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::ChangeSemiMajorAxis::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::ChangeSemiMajorAxis>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::HohmannTransfer::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::HohmannTransfer>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Normal::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Normal>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Intercept::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Intercept>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Rendezvous::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Rendezvous>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::MatchVelocity::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::MatchVelocity>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Target::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Target>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::DeltaV::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::DeltaV>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::NaturalMotionCircumnavigation::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::NaturalMotionCircumnavigation>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Teardrop::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Teardrop>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalManeuvers::Drift::GetTypeId(),
                                 ut::make_unique<WsfOrbitalManeuvers::Drift>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalScriptEvent::GetTypeId(), ut::make_unique<WsfOrbitalScriptEvent>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalStagingEvent::GetTypeId(),
                                 ut::make_unique<WsfOrbitalStagingEvent>(GetScenario()));
   orbitalMissionEventTypes->Add(WsfOrbitalAttitudeChangeEvent::GetTypeId(),
                                 ut::make_unique<WsfOrbitalAttitudeChangeEvent>(GetScenario()));

   scenario.AddTypeList(std::move(orbitalMissionEventTypes));

   auto orbitalManeuveringTypes = ut::make_unique<WsfOrbitalManeuveringTypes>(scenario);
   orbitalManeuveringTypes->Add("simple", ut::make_unique<WsfSimpleOrbitalManeuvering>());
   orbitalManeuveringTypes->Add("rocket", ut::make_unique<WsfRocketOrbitalManeuvering>());

   scenario.AddTypeList(std::move(orbitalManeuveringTypes));

   auto attitudeControllerTypes = ut::make_unique<WsfAttitudeControllerTypes>(scenario);
   attitudeControllerTypes->Add("instant", ut::make_unique<WsfInstantAttitudeController>());
   attitudeControllerTypes->Add("rate_limited", ut::make_unique<WsfRateLimitedAttitudeController>());
   scenario.AddTypeList(std::move(attitudeControllerTypes));

   WsfMoverTypes::Get(scenario).Add("WSF_NORAD_SPACE_MOVER", ut::make_unique<WsfNORAD_SpaceMover>(scenario));
   WsfMoverTypes::Get(scenario).Add("WSF_SPACE_MOVER", ut::make_unique<WsfSpaceMover>(scenario));
   WsfMoverTypes::Get(scenario).Add("WSF_INTEGRATING_SPACE_MOVER", ut::make_unique<WsfIntegratingSpaceMover>(scenario));

   WsfProcessorTypes::Get(scenario).Add("WSF_ORBITAL_CONJUNCTION_PROCESSOR",
                                        ut::make_unique<WsfOrbitalConjunctionProcessor>(scenario));

   // Note: not sure why filter types overrides Add; using AddCoreType instead.
   GetScenario().GetFilterTypes().AddCoreType("WSF_ORBIT_DETERMINATION_FILTER",
                                              ut::make_unique<WsfOrbitDeterminationKalmanFilter>(scenario));

   WsfOpticalSignatureTypes::Get(scenario).AddObjectFactory(WsfSpaceOpticalSignature::ObjectFactory);

   scenario.GetApplication().ExtensionDepends("space_event_output", "space_observer", true);
   scenario.GetApplication().ExtensionDepends("space_event_output", "event_output", true);

   // If the "event_pipe" extension has been defined then hook things up so our events will also be logged.
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr)
   {
      wsf::space::EventPipe::RegisterEvents(*eventPipePtr);
   }

   // Integrating space mover related types
   scenario.AddTypeList(ut::make_unique<WsfOrbitalDynamicsTermTypes>(scenario));
   scenario.AddTypeList(ut::make_unique<WsfOrbitalIntegratorTypes>(scenario));

   mOrbitalPropagatorTypes = ut::make_unique<WsfOrbitalPropagatorTypes>();
   mOrbitalPropagatorTypes->AddObjectFactory(WsfIntegratingPropagator::ObjectFactory(scenario));
   mOrbitalPropagatorTypes->AddObjectFactory(WsfNORAD_OrbitalPropagator::ObjectFactory(scenario));
   mOrbitalPropagatorTypes->AddObjectFactory(wsf::space::KeplerianOrbitalPropagator::ObjectFactory(scenario));
   mOrbitalPropagatorTypes->AddObjectFactory(wsf::space::J2_PerturbationOrbitalPropagator::ObjectFactory(scenario));

   mOrbitalTargetingCostTypesPtr = ut::make_unique<wsf::space::OrbitalTargetingCostTypes>();

   wsf::TrackExtrapolationStrategyTypes::Get(scenario).GetDefaultStrategy().RegisterExtrapolation(
      wsf::TrackExtrapolationIds::cSPACE_KINEMATIC_STATE_EXTRAPOLATION,
      ut::make_unique<wsf::space::KinematicStateExtrapolation>());
}

void WsfSpaceExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension("space_observer", ut::make_unique<WsfSpaceObserver>());
   aSimulation.RegisterExtension("wsf_space_eventpipe", ut::make_unique<wsf::space::EventPipeInterface>());

   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(aSimulation);
   if (eventOutputPtr != nullptr)
   {
      Register_event_output(*eventOutputPtr);
   }

   // If the csv_event_output extension is available, hook in the handlers for our csv events.
   WsfCSV_EventOutput* csv_EventOutputPtr = WsfCSV_EventOutput::Find(aSimulation);
   if (csv_EventOutputPtr != nullptr)
   {
      static std::vector<std::string> orbitDeterminationHeader = {"time<time>",
                                                                  "event<string>",
                                                                  "platform<string>",
                                                                  "target_platform<string>"};
      static std::vector<std::string> orbitalManeuverHeader    = {"time<time>",
                                                               "event<string>",
                                                               "platform<string>",
                                                               "id<int>",
                                                               "maneuver_type<string>",
                                                               "maneuver_constraint<string>",
                                                               "maneuver_constratint_value<double>",
                                                               "duration<double>",
                                                               "intercept_time<double>"};
      static std::vector<std::string> orbitalElementsHeader    = {"semi_major_axis<double>",
                                                               "eccentricity<double>",
                                                               "inclination<double>",
                                                               "raan<double>",
                                                               "argument_of_periapsis<double>",
                                                               "mean_anomaly<double>",
                                                               "true_anomaly<double>"};
      static std::vector<std::string> locationHeader           = {"truth_lat<lat>",
                                                        "truth_lon<lon>",
                                                        "truth_alt<double>",
                                                        "truth_x<double>",
                                                        "truth_y<double>",
                                                        "truth_z<double>"};

      std::vector<std::string> temp = {"time<time>", "event<string>", "platform<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, orbitalElementsHeader);
      WsfCSV_EventOutputData::AppendDataTags(temp, locationHeader);
      WsfCSV_EventOutputData::AddDataTags("ECLIPSE_ENTRY", temp);
      WsfCSV_EventOutputData::AddDataTags("ECLIPSE_EXIT", temp);

      temp = orbitalManeuverHeader;
      WsfCSV_EventOutputData::AppendDataTags(temp, orbitalElementsHeader);
      temp.emplace_back("special_orbit_type<string>");
      WsfCSV_EventOutputData::AddDataTags("ORBITAL_MANEUVER_INITIATED", temp);
      temp.emplace_back("delta_v<double>");
      WsfCSV_EventOutputData::AddDataTags("ORBITAL_MANEUVER_UPDATED", temp);
      WsfCSV_EventOutputData::AddDataTags("ORBITAL_MANEUVER_COMPLETED", temp);
      WsfCSV_EventOutputData::AddDataTags("ORBITAL_MANEUVER_CANCELED", temp);

      temp = orbitDeterminationHeader;
      WsfCSV_EventOutputData::AppendDataTags(temp, orbitalElementsHeader);
      WsfCSV_EventOutputData::AddDataTags("ORBIT_DETERMINATION_INITIATED", temp);
      WsfCSV_EventOutputData::AddDataTags("ORBIT_DETERMINATION_UPDATED", temp);
      Register_event_output(*csv_EventOutputPtr);
   }

   auto scriptObserverPtr = WsfScriptObserver::Find(aSimulation);
   if (scriptObserverPtr)
   {
      Register_script_observer(*scriptObserverPtr);
   }

   aSimulation.RegisterExtension(WsfConstellationManager::cEXTENSION_NAME, ut::make_unique<WsfConstellationManager>());
   aSimulation.RegisterExtension(WsfDE_FileManager::cEXTENSION_NAME, ut::make_unique<WsfDE_FileManager>());
}
