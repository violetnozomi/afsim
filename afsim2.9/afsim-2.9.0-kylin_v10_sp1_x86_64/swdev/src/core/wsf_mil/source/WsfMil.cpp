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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfMil.hpp"

#include "wsf_mil_export.h"

#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAtmosphericTurbulence.hpp"
#include "WsfCSV_EventOutput.hpp"
#include "WsfEventOutput.hpp"
#include "WsfFrameStepSimulation.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfMilEventPipe.hpp"
#include "WsfMilEventResults.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfMilScriptExtensions.hpp"
#include "WsfMilScriptObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfQuantumTaskerObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfTaskObserver.hpp"
#include "WsfWeaponObserver.hpp"
#include "WsfWeaponTaskObserver.hpp"
#include "WsfWeaponTypes.hpp"
#include "dis/WsfDisExtension.hpp"
#include "ext/WsfExtInterface.hpp"
#include "script/WsfScriptObserver.hpp"

// Antenna Patterns
#include "WsfAntennaPatternTypes.hpp"
#include "WsfESA_AntennaPattern.hpp"

// Comm
#include "WsfMilCommTypes.hpp"

// Correlation
#include "WsfClusterCorrelation.hpp"
#include "WsfCorrelationStrategyTypes.hpp"

// EW
#include "WsfEW_EA_EP.hpp"
#include "WsfEW_EA_EP_Types.hpp"
#include "WsfEW_Effect.hpp"
#include "WsfEW_EffectManager.hpp"
#include "WsfEW_EffectTypes.hpp"
#include "WsfEW_Effects.hpp"
#include "WsfEW_Technique.hpp"
#include "WsfEW_TechniqueTypes.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfFalseTargetScreenerTypes.hpp"
#include "WsfFalseTargetTypes.hpp"
#include "WsfFusionCenter.hpp"

// Sensors
#include "WsfBistaticErrorModel.hpp"
#include "WsfCTD_SensorComponent.hpp"
#include "WsfESM_Sensor.hpp"
#include "WsfMilSensorTypes.hpp"
#include "WsfOTH_RadarSensorErrorModel.hpp"
#include "WsfSAR_Sensor.hpp"
#include "WsfSensorErrorModelTypes.hpp"
#include "WsfSurfaceWaveRadarSensorErrorModel.hpp"

// Optical
#include "WsfEM_AttenuationTypes.hpp"
#include "WsfOpticalAttenuation.hpp"
#include "WsfOpticalPath.hpp"

// Clutter
#include "WsfEM_ClutterTypes.hpp"
#include "WsfEM_SurfaceClutter.hpp"

// Movers
#include "WsfFormationFlyer.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfParabolicMover.hpp"
#include "WsfStraightLineMover.hpp"
#include "WsfSubsurfaceMover.hpp"
#include "WsfTBM_Mover.hpp"
#include "WsfTowedMover.hpp"

// Launch Computers
#include "WsfLaunchComputer.hpp"
#include "WsfLaunchComputerTypes.hpp"
#include "WsfOrbitalLaunchComputer.hpp"
#include "WsfScriptLaunchComputer.hpp"
#include "WsfScriptLaunchComputerClass.hpp"
#include "WsfScriptLaunchHandoffDataClass.hpp"
#include "WsfScriptOrbitalLaunchComputerClass.hpp"

// Processors
#include "WsfAirTargetFuse.hpp"
#include "WsfAssetMessage.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfClassificationProcessor.hpp"
#include "WsfGroundTargetFuse.hpp"
#include "WsfGuidanceComputer.hpp"
#include "WsfImageProcessor.hpp"
#include "WsfIntersectProcessor.hpp"
#include "WsfPerceptionProcessor.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfQuantumTask.hpp"
#include "WsfQuantumTaskerProcessor.hpp"
#include "WsfScriptClusterManagerClass.hpp"
#include "WsfScriptQuantumTaskerProcessor.hpp"
#include "WsfScriptThreatProcessorClass.hpp"
#include "WsfScriptThreatReportClass.hpp"
#include "WsfScriptWeaponThreatProcessorClass.hpp"
#include "WsfThreatProcessor.hpp"
#include "WsfUplinkProcessor.hpp"
#include "WsfWeaponFuse.hpp"
#include "WsfWeaponTask.hpp"
#include "WsfWeaponTaskManager.hpp"
#include "WsfWeaponThreatProcessor.hpp"
#include "WsfWeaponTrackProcessor.hpp"

// Weapons
#include "WsfHighEnergyLaser.hpp"
#include "WsfPkTable.hpp"
#include "WsfWeaponTypes.hpp"

// Weapon Effects
#include "WsfExplicitWeapon.hpp"
#include "WsfGuidanceProgram.hpp"
#include "WsfScriptDirectedEnergyWeaponClass.hpp"
#include "WsfScriptFormationFlyerClass.hpp"
#include "WsfScriptGuidanceComputerClass.hpp"
#include "WsfScriptGuidedMoverClass.hpp"
#include "WsfScriptGuidedMoverTestObject.hpp"
#include "WsfScriptImplicitWeaponClass.hpp"
#include "WsfScriptLaserWeaponClass.hpp"
#include "WsfScriptTowedMoverClass.hpp"
#include "WsfScriptWeaponClass.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponFuse.hpp"

// Signatures
#include "WsfAcousticSignatureTypes.hpp"
#include "WsfCompositeOpticalSignature.hpp"
#include "WsfInfraredSignatureTypes.hpp"
#include "WsfInherentContrastTypes.hpp"
#include "WsfMilRadarSignature.hpp"
#include "WsfOpticalReflectivityTypes.hpp"
#include "WsfOpticalSignatureTypes.hpp"
#include "WsfRadarSignatureTypes.hpp"

namespace
{
// The actual application extension
class MilApplicationExtension : public WsfApplicationExtension
{
public:
   void AddedToApplication(WsfApplication& aApplication) override;
   void ScenarioCreated(WsfScenario& aScenario) override;
};

void MilApplicationExtension::AddedToApplication(WsfApplication& aApplication)
{
   UtScriptTypes* scriptTypesPtr = aApplication.GetScriptTypes();

   // Register new script classes provided by this extension.
   scriptTypesPtr->Register(WsfESM_Sensor::CreateScriptClass("WsfESM_Sensor", scriptTypesPtr));
   scriptTypesPtr->Register(WsfSAR_Sensor::CreateScriptClass("WsfSAR_Sensor", scriptTypesPtr));
   scriptTypesPtr->Register(WsfEW_Effects::CreateScriptClass("WsfEW_Effects", scriptTypesPtr));
   scriptTypesPtr->Register(WsfQuantumTask::CreateScriptClass("WsfQuantumTask", scriptTypesPtr));
   scriptTypesPtr->Register(WsfWeaponEngagement::CreateScriptClass("WsfWeaponEngagement", scriptTypesPtr));
   scriptTypesPtr->Register(WsfWeaponFuse::CreateScriptClass("WsfWeaponFuse", scriptTypesPtr));

   scriptTypesPtr->Register(ut::make_unique<WsfScriptAssetMessageClass>("WsfAssetMessage", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptAssetPerceptionClass>("WsfAssetPerception", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptClusterManagerClass>("WsfClusterManager", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptGuidanceComputerClass>("WsfGuidanceComputer", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptLaunchHandoffDataClass>("WsfLaunchHandoffData", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptLaunchComputerClass>("WsfLaunchComputer", scriptTypesPtr));
   scriptTypesPtr->Register(
      ut::make_unique<WsfScriptOrbitalLaunchComputerClass>("WsfOrbitalLaunchComputer", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptPerceptionProcessor>("WsfPerceptionProcessor", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptQuantumTaskerProcessor>("WsfQuantumTaskerProcessor", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptThreatProcessorClass>("WsfThreatProcessor", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptThreatReportClass>("WsfThreatReport", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptUplinkProcessor>("WsfUplinkProcessor", scriptTypesPtr));
   scriptTypesPtr->Register(
      ut::make_unique<WsfScriptWeaponThreatProcessorClass>("WsfWeaponThreatProcessor", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptFormationFlyerClass>("WsfFormationFlyer", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptGuidedMoverClass>("WsfGuidedMover", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptGuidedMoverTestObject>("WsfGuidedMoverTestObject", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptTowedMoverClass>("WsfTowedMover", scriptTypesPtr));

   // Register weapon script classes in order of inheritance hierarchy, so that script methods are indexed correctly
   scriptTypesPtr->Register(ut::make_unique<WsfScriptWeaponClass>("WsfWeapon", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptImplicitWeaponClass>("WsfImplicitWeapon", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptDirectedEnergyWeaponClass>("WsfDirectedEnergyWeapon", scriptTypesPtr));
   scriptTypesPtr->Register(ut::make_unique<WsfScriptLaserWeaponClass>("WsfLaserWeapon", scriptTypesPtr));

   WsfWeaponTask::RegisterScriptTypes(scriptTypesPtr);

   // Add methods to existing script classes.
   WsfMilScriptExtensions::ExtendScriptClasses(*scriptTypesPtr);
}

void MilApplicationExtension::ScenarioCreated(WsfScenario& aScenario)
{
   aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfMilExtension>());
}
} // namespace

namespace
{
// Register wsf_mil events with event output extensions
using EventOutput = wsf::event::output::SimulationExtension;
void Register_event_output(EventOutput& aEventOutput)
{
   WsfSimulation* simPtr = &aEventOutput.GetSimulation();

   using namespace wsf::event;
   aEventOutput.AddEvent<DirectedEnergyWeaponAbortShot>("DIRECTED_ENERGY_WEAPON_ABORT_SHOT",
                                                        WsfObserver::DirectedEnergyWeaponAbortShot(simPtr));
   aEventOutput.AddEvent<DirectedEnergyWeaponBeginShot>("DIRECTED_ENERGY_WEAPON_BEGIN_SHOT",
                                                        WsfObserver::DirectedEnergyWeaponBeginShot(simPtr));
   aEventOutput.AddEvent<DirectedEnergyWeaponCooldownComplete>("DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE",
                                                               WsfObserver::DirectedEnergyWeaponCooldownComplete(simPtr));
   aEventOutput.AddEvent<DirectedEnergyWeaponEndShot>("DIRECTED_ENERGY_WEAPON_END_SHOT",
                                                      WsfObserver::DirectedEnergyWeaponEndShot(simPtr));
   aEventOutput.AddEvent<DirectedEnergyWeaponUpdateShot>("DIRECTED_ENERGY_WEAPON_UPDATE_SHOT",
                                                         WsfObserver::DirectedEnergyWeaponUpdateShot(simPtr));
   aEventOutput.AddEvent<ImplicitWeaponBeginEngagement>("IMPLICIT_WEAPON_BEGIN_ENGAGEMENT",
                                                        WsfObserver::ImplicitWeaponBeginEngagement(simPtr));
   aEventOutput.AddEvent<ImplicitWeaponEndEngagement>("IMPLICIT_WEAPON_END_ENGAGEMENT",
                                                      WsfObserver::ImplicitWeaponEndEngagement(simPtr));
   aEventOutput.AddEvent<JammingAttempt>("JAMMING_ATTEMPT", WsfObserver::JammingAttempt(simPtr));
   aEventOutput.AddEvent<JammingRequestCanceled>("JAMMING_REQUEST_CANCELED", WsfObserver::JammingRequestCanceled(simPtr));
   aEventOutput.AddEvent<JammingRequestInitiated>("JAMMING_REQUEST_INITIATED",
                                                  WsfObserver::JammingRequestInitiated(simPtr));
   aEventOutput.AddEvent<JammingRequestUpdated>("JAMMING_REQUEST_UPDATED", WsfObserver::JammingRequestUpdated(simPtr));
   aEventOutput.AddEvent<GuidanceComputerPhaseChanged>("MOVER_GUIDANCE_PHASE_CHANGED",
                                                       WsfObserver::GuidanceComputerPhaseChanged(simPtr));
   aEventOutput.AddEvent<WeaponFireAborted>("WEAPON_FIRE_ABORTED", WsfObserver::WeaponFireAborted(simPtr));
   aEventOutput.AddEvent<WeaponFireRequested>("WEAPON_FIRE_REQUESTED", WsfObserver::WeaponFireRequested(simPtr));
   aEventOutput.AddEvent<WeaponFired>("WEAPON_FIRED", WsfObserver::WeaponFired(simPtr));
   aEventOutput.AddEvent<WeaponHit>("WEAPON_HIT", WsfObserver::WeaponHit(simPtr));
   aEventOutput.AddEvent<WeaponKilled>("WEAPON_KILLED", WsfObserver::WeaponKilled(simPtr));
   aEventOutput.AddEvent<WeaponMissed>("WEAPON_MISSED", WsfObserver::WeaponMissed(simPtr));
   aEventOutput.AddEvent<WeaponModeActivated>("WEAPON_MODE_ACTIVATED", WsfObserver::WeaponModeActivated(simPtr));
   aEventOutput.AddEvent<WeaponModeDeactivated>("WEAPON_MODE_DEACTIVATED", WsfObserver::WeaponModeDeactivated(simPtr));
   aEventOutput.AddEvent<WeaponNonOperational>("WEAPON_NON_OPERATIONAL", WsfObserver::WeaponNonOperational(simPtr));
   aEventOutput.AddEvent<WeaponOperational>("WEAPON_OPERATIONAL", WsfObserver::WeaponOperational(simPtr));
   aEventOutput.AddEvent<WeaponReloadEnded>("WEAPON_RELOAD_ENDED", WsfObserver::WeaponReloadEnded(simPtr));
   aEventOutput.AddEvent<WeaponReloadStarted>("WEAPON_RELOAD_STARTED", WsfObserver::WeaponReloadStarted(simPtr));
   aEventOutput.AddEvent<WeaponSelected>("WEAPON_SELECTED", WsfObserver::WeaponSelected(simPtr));
   aEventOutput.AddEvent<WeaponTerminated>("WEAPON_TERMINATED", WsfObserver::WeaponTerminated(simPtr));
   aEventOutput.AddEvent<WeaponTurnedOff>("WEAPON_TURNED_OFF", WsfObserver::WeaponTurnedOff(simPtr));
   aEventOutput.AddEvent<WeaponTurnedOn>("WEAPON_TURNED_ON", WsfObserver::WeaponTurnedOn(simPtr));

   aEventOutput.AddEventAlias("WEAPON_LAUNCHED", "WEAPON_FIRED");
   aEventOutput.AddEventAlias("PLATFORM_KILLED", "PLATFORM_BROKEN"); // DEPRECATED as of 2.9
}

void PrintAssetMessage(const EventOutput& aEventOutput, double aSimTime, const WsfMessage& aMessage)
{
   const auto&         message  = static_cast<const WsfAssetMessage&>(aMessage);
   WsfAssetPerception* assetPtr = message.GetAsset();
   if (assetPtr != nullptr)
   {
      WsfPlatform* platformPtr = aEventOutput.GetSimulation().GetPlatformByIndex(assetPtr->mIndex);
      if (platformPtr != nullptr)
      {
         aEventOutput.StreamRef() << " Asset: " << platformPtr->GetName();
      }
      else
      {
         aEventOutput.StreamRef() << " Asset: not_found";
      }
      aEventOutput.StreamRef() << " TimeStamp: " << assetPtr->mTime;
   }
}

void PrintAssetMessageCSV(const EventOutput& aEventOutput, double aSimTime, const WsfMessage& aMessage)
{
   const auto&         message  = static_cast<const WsfAssetMessage&>(aMessage);
   WsfAssetPerception* assetPtr = message.GetAsset();
   if (assetPtr != nullptr)
   {
      WsfPlatform* platformPtr = aEventOutput.GetSimulation().GetPlatformByIndex(assetPtr->mIndex);
      if (platformPtr != nullptr)
      {
         aEventOutput.StreamRef() << platformPtr->GetName();
      }
      else
      {
         aEventOutput.StreamRef() << "UNKNOWN";
      }
      aEventOutput.StreamRef() << ',' << assetPtr->mTime;
   }
}

// CSV event output headers
const std::vector<std::string> mEW_InteractionTags = {
   // PrintEM_AbsXmtrRcvrData (Xmtr) (Rcvr)
   "xmtr_type<string>",
   "xmtr_lat<lat>",
   "xmtr_lon<lon>",
   "xmtr_alt<double>",
   "xmtr_x<double>",
   "xmtr_y<double>",
   "xmtr_z<double>",
   "xmtr_heading<angle>",
   "xmtr_pitch<angle>",
   "xmtr_roll<angle>",
   "xmtr_speed<double>",
   "rcvr_type<string>",
   "rcvr_lat<lat>",
   "rcvr_lon<lon>",
   "rcvr_alt<double>",
   "rcvr_x<double>",
   "rcvr_y<double>",
   "rcvr_z<double>",
   "rcvr_heading<angle>",
   "rcvr_pitch<angle>",
   "rcvr_roll<angle>",
   "rcvr_speed<double>",
   // PrintEM_AbsTargetData (tgt)
   "target_type<string>",
   "target_lat<lat>",
   "target_lon<lon>",
   "target_alt<double>",
   "target_x<double>",
   "target_y<double>",
   "target_z<double>",
   "target_heading<angle>",
   "target_pitch<angle>",
   "target_roll<angle>",
   "target_speed<double>",
   // PrintEM_RelXmtrRcvrData(Xmtr->Rcvr) (Rcvr->Xmtr)
   "xmtr->rcvr_range<double>",
   "xmtr->rcvr_bearing<angle>",
   "xmtr->rcvr_elev<angle>",
   "xmtr->rcvr_apparent_bearing<angle>",
   "xmtr->rcvr_apparent_elev<angle>",
   "rcvr->xmtr_range<double>",
   "rcvr->xmtr_bearing<angle>",
   "rcvr->xmtr_elev<angle>",
   "rcvr->xmtr_apparent_bearing<angle>",
   "rcvr->Xmtr_apparent_elev<angle>",
   // PrintEM_RelXmtrRcvrData (Xmtr->Tgt)
   "xmtr->tgt_range<double>",
   "xmtr->tgt_bearing<angle>",
   "xmtr->tgt_elev<angle>",
   "xmtr->tgt_apparent_bearing<angle>",
   "xmtr->tgt_apparent_elev<angle>",
   // PrintEM_RelTargetData (Tgt->Xmtr)
   "tgt->xmtr_range<double>",
   "tgt->xmtr_bearing<angle>",
   "tgt->xmtr_elev<angle>",
   "tgt->xmtr_apparent_bearing<angle>",
   "tgt->xmtr_apparent_elev<angle>",
   // PrintEM_RelXmtrRcvrData (Rcvr->Tgt)
   "rcvr->tgt_range<double>",
   "rcvr->tgt_bearing<angle>",
   "rcvr->tgt_elev<angle>",
   "rcvr->tgt_apparent_bearing<angle>",
   "rcvr->tgt_apparent_elev<angle>",
   // PrintEM_RelTargetData (Tgt->Rcvr)
   "tgt->rcvr_range<double>",
   "tgt->rcvr_bearing<angle>",
   "tgt->rcvr_elev<angle>",
   "tgt->rcvr_apparent_bearing<angle>",
   "tgt->rcvr_apparent_elev<angle>",
   // PrintEM_BeamData (Xmtr) (Rcvr)
   "xmtr_beam_bearing<angle>",
   "xmtr_beam_elevation<angle>",
   "xmtr_rel_az<angle>",
   "xmtr_rel_el<angle>",
   "xmtr_EBS_Az<angle>",
   "xmtr_EBS_El<angle>",
   "xmtr_Omega<angle>",
   "xmtr_gain<double>",
   "rcvr_beam_bearing<angle>",
   "rcvr_beam_elevation<angle>",
   "rcvr_rel_az<angle>",
   "rcvr_rel_el<angle>",
   "rcvr_EBS_Az<angle>",
   "rcvr_EBS_El<angle>",
   "rcvr_Omega<angle>",
   "rcvr_gain<double>",
   "RCS<double>",
   "RCS_(linear)<double>",
   "RCS_azimuth<angle>",
   "RCS_elev<angle>",
   "Optical_Sig<double>",
   "Optical_Sig_(linear)<double>",
   "Optical_Sig_azimuth<angle>",
   "Optical_Sig_elev<angle>",
   "Optical_Sig_Reflectivity<double>",
   "IR_Sig<double>",
   "IR_Sig_azimuth<angle>",
   "IR_Sig_elev<angle>",
   "BackgroundRadiantIntensity<double>",
   "ContrastRadiantIntensity<double>",
   "Transmittance<double>",
   "AbsorptionFactor<double>",
   "AbsorptionFactor_(linear)<double>",
   "PropagationFactor_F^4<double>",
   "PropagationFactor_(linear)<double>",
   "PropagationFactor_^0.25<double>",
   "masking_factor<double>",
   "TransmittedPower<double>",
   "ReceivedPower<double>",
   "ReceiverNoisePower<double>",
   "ClutterPower<double>",
   "InterferencePower<double>",
   "NoiseJammerPower<double>",
   "PulseJammerPower<double>",
   "CoherentJammerPower<double>",
   "PixelCount<double>",
   "SignalToNoise<double>",
   "DetectionThreshold<double>",
   "SignalPower/noisePower<double>",
   "SignalPower/(noisePower+clutterPower)<double>",
   "SignalPower/(noisePower+clutterPower+jammerPower)<double>",
   "EW_Effects<string>",
   "failure_reason<string>"};

const std::vector<std::string> mWE_PlayerDataTags = {"platform<string>",
                                                     "target_platform<string>",
                                                     "weapon_platform<string>",
                                                     "engagement_serial_number<int>"};

const std::vector<std::string> mWE_TerminationTags = {"launch_Pk<bool>",
                                                      "intercept_Pk<bool>",
                                                      "PK<double>",
                                                      "Pk_degrade<double>",
                                                      "Pk_constraint<double>",
                                                      "Pk_drawn<double>",
                                                      "CEP<double>",
                                                      "damage_factor_type<string>",
                                                      "damage_factor<double>",
                                                      "result<string>",
                                                      "extended_result<string>",
                                                      "weapon_lat<lat>",
                                                      "weapon_lon<lon>",
                                                      "weapon_alt<double>",
                                                      "weapon_eci_x<double>",
                                                      "weapon_eci_y<double>",
                                                      "weapon_eci_z<double>",
                                                      "weapon_heading<angle>",
                                                      "weapon_pitch<angle>",
                                                      "weapon_roll<angle>",
                                                      "weapon_speed_ned<double>",
                                                      "weapon_velocity_n<double>",
                                                      "weapon_velocity_e<double>",
                                                      "weapon_velocity_d<double>",
                                                      "weapon_speed_eci<double>",
                                                      "weapon_velocity_x<double>",
                                                      "weapon_velocity_y<double>",
                                                      "weapon_velocity_z<double>",
                                                      "weapon_acceleration_ned<double>",
                                                      "weapon_acceleration_n<double>",
                                                      "weapon_acceleration_e<double>",
                                                      "weapon_acceleration_d<double>",
                                                      "weapon_acceleration_eci<double>",
                                                      "weapon_acceleration_x<double>",
                                                      "weapon_acceleration_y<double>",
                                                      "weapon_acceleration_z<double>",
                                                      "target_lat<lat>",
                                                      "target_lon<lon>",
                                                      "target_alt<double>",
                                                      "target_x<lat>",
                                                      "target_y<lon>",
                                                      "target_z<double>",
                                                      "target_heading<angle>",
                                                      "target_pitch<angle>",
                                                      "target_roll<angle>",
                                                      "target_speed_ned<double>",
                                                      "target_velocity_n<double>",
                                                      "target_velocity_e<double>",
                                                      "target_velocity_d<double>",
                                                      "target_speed_eci<double>",
                                                      "target_velocity_x<double>",
                                                      "target_velocity_y<double>",
                                                      "target_velocity_z<double>",
                                                      "target_acceleration_ned<double>",
                                                      "target_acceleration_n<double>",
                                                      "target_acceleration_e<double>",
                                                      "target_acceleration_d<double>",
                                                      "target_acceleration_eci<double>",
                                                      "target_acceleration_x<double>",
                                                      "target_acceleration_y<double>",
                                                      "target_acceleration_z<double>",
                                                      "slant_range<double>",
                                                      "ground_range<double>",
                                                      "down_range<double>",
                                                      "cross_range<double>",
                                                      "relative_alt_offset<double>", // implicit
                                                      "miss_distance<double>",
                                                      "weapon_ecs_x<double>",
                                                      "weapon_ecs_y<double>",
                                                      "weapon_ecs_z<double>",
                                                      "impact_speed<double>",
                                                      "ecs_weapon_velocity_x<double>",
                                                      "ecs_weapon_velocity_y<double>",
                                                      "ecs_weapon_velocity_z<double>"}; // explicit

void AddCSV_DataTags(const WsfCSV_EventOutput& aCSV_EventOutput)
{
   std::vector<std::string>       temp;
   const std::vector<std::string> printSystemHeader = {"time<time>",
                                                       "event<string>",
                                                       "platform<string>",
                                                       "side<string>",
                                                       "type<string>",
                                                       "system_platform<string>",
                                                       "system_type<string>",
                                                       "lat<lat>",
                                                       "lon<lon>",
                                                       "alt<double>",
                                                       "heading<angle>",
                                                       "pitch<angle>",
                                                       "roll<angle>",
                                                       "speed<double>",
                                                       "velocity_x<double>",
                                                       "velocity_y<double>",
                                                       "velocity_z<double>",
                                                       "acceleration<double>",
                                                       "acceleration_x<double>",
                                                       "acceleration_y<double>",
                                                       "acceleration_z<double>"};
   // OVERRIDES
   {
      // MESSAGE_DELIVERY_ATTEMPT
      temp = {"time<time>",
              "event<string>",
              "transmitter<string>",
              "transmitter_side<string>",
              "receiver<string>",
              "receiver_system<string>",
              "message_serial_number<int>",
              "data_tag<double>",
              "message_type<string>",
              "message_size<int>",
              "data_rate<double>",
              "noise_density<double>",
              "bit_error_rate<double>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, mEW_InteractionTags);
      WsfCSV_EventOutputData::AppendDataTags(temp, {"success_failure<bool>"});
      WsfCSV_EventOutputData::AddDataTags("MESSAGE_DELIVERY_ATTEMPT", temp);
   }
   {
      // SENSOR_DETECTION
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "side<string>",
              "target_platform<string>",
              "sensor<string>",
              "mode<string>",
              "beam_index<int>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, mEW_InteractionTags);
      WsfCSV_EventOutputData::AppendDataTags(temp,
                                             {"sensor_warning<string>", "Pd<double>", "required_Pd<double>", "detected<bool>"});
      WsfCSV_EventOutputData::AddDataTags("SENSOR_DETECTION_ATTEMPT", temp);
   }

   // ADDITIONAL
   {
      // DIRECTED_ENERGY_WEAPON_SHOT
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "target_platform<string>",
              "weapon<string>",
              "weapon_type<string>",
              "target_type<string>",
              "target_relative_N<double>",
              "target_relative_E<double>",
              "target_relative_D<double>",
              "target_range<double>",
              "target_bearing<angle>",
              "target_elevation<angle>",
              "power<double>",
              "peak_irradiance<double>",
              "edge_irradiance<double>",
              "damage_radius<double>",
              "energy_density<double>",
              "energy<double>",
              "beam_semi_major_axis<double>",
              "beam_semi_minor_axis<double>",
              "incidence_angle<double>"};
      WsfCSV_EventOutputData::AddDataTags("DIRECTED_ENERGY_WEAPON_ABORT_SHOT", temp);
      WsfCSV_EventOutputData::AddDataTags("DIRECTED_ENERGY_WEAPON_BEGIN_SHOT", temp);
      WsfCSV_EventOutputData::AddDataTags("DIRECTED_ENERGY_WEAPON_END_SHOT", temp);
      WsfCSV_EventOutputData::AddDataTags("DIRECTED_ENERGY_WEAPON_UPDATE_SHOT", temp);
   }
   WsfCSV_EventOutputData::AddDataTags("DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE",
                                       {"time<time>", "event<string>", "platform<string>", "weapon<string>"});
   WsfCSV_EventOutputData::AddDataTags("IMAGE_CREATED",
                                       {"time<time>",
                                        "event<string>",
                                        "platform<string>",
                                        "sensor<double>",
                                        "sensor_mode<double>",
                                        "creation_time<time>",
                                        "image_number<double>",
                                        "size_(bits)<double>",
                                        "originator_lat<lat>",
                                        "originator_lon<lon>",
                                        "originator_alt<double>",
                                        "center_lat<lat>",
                                        "center_lon<lon>",
                                        "center_alt<double>",
                                        "image_width<double>",
                                        "image_height<double>",
                                        "width_resolution<double>",
                                        "height_resolution<double>",
                                        "collection_factor<double>",
                                        "noise_level<double>",
                                        "background_level<double>",
                                        "min_level<double>",
                                        "max_level<double>",
                                        "object_count<double>",
                                        "truth_name<string>",
                                        "truth_type<string>",
                                        "truth_side<string>",
                                        "pixel_count<double>",
                                        "signal_level<double>",
                                        "pixel_intensity<double>",
                                        "rel_lat<lat>",
                                        "rel_lon<lon>",
                                        "rel_alt<double>",
                                        "em_range<double>",
                                        "azimuth<angle>",
                                        "elevation<angle>"});
   {
      // IMPLICIT_WEAPON_ENGAGEMENT
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "target_platform<string>",
              "weapon_name<string>",
              "weapon_type<string>",
              "target_type<string>",
              "target_relative_N<double>",
              "target_relative_E<double>",
              "target_relative_D<double>",
              "target_range<double>",
              "target_bearing<angle>",
              "target_elevation<angle>"};
      WsfCSV_EventOutputData::AddDataTags("IMPLICIT_WEAPON_BEGIN_ENGAGEMENT", temp);
      WsfCSV_EventOutputData::AddDataTags("IMPLICIT_WEAPON_END_ENGAGEMENT", temp);
   }
   {
      // JAMMING_ATTEMPT
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "receiver_platform<string>",
              "transmitter<string>",
              "beam_number<int>",
              "receiver<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, mEW_InteractionTags);
      WsfCSV_EventOutputData::AppendDataTags(temp,
                                             {"(receiver_power/receiver_noise_power)<double>",
                                              "(coherent_jammer_power/receiver_noise_power)<double>"});
      WsfCSV_EventOutputData::AddDataTags("JAMMING_ATTEMPT", temp);
   }
   {
      // JAMMING_REQUEST
      std::vector<std::string> jamRequestTags = {"time<time>",
                                                 "event<string>",
                                                 "platform<string>",
                                                 "weapon<string>",
                                                 "current_mode<string>",
                                                 "active_requests_(eM_Xmtrs)<int>",
                                                 "frequency<double>",
                                                 "bandwidth<double>",
                                                 "target_platform<string>"};
      WsfCSV_EventOutputData::AddDataTags("JAMMING_REQUEST_CANCELED", jamRequestTags);
      WsfCSV_EventOutputData::AddDataTags("JAMMING_REQUEST_INITIATED", jamRequestTags);
      WsfCSV_EventOutputData::AddDataTags("JAMMING_REQUEST_UPDATED", jamRequestTags);
   }
   WsfCSV_EventOutputData::AddDataTags("MOVER_GUIDANCE_PHASE_CHANGED",
                                       {"time<time>", "event<string>", "platform<string>", "phase<string>"});
   {
      // WEAPON_FIRE_STATUS
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "target_platform<string>",
              "weapon<string>",
              "weapon_track_id<string>",
              "weapon_qty<int>",
              "weapon_lat<lat>",
              "weapon_lon<lon>",
              "weapon_alt<double>",
              "weapon_x<double>",
              "weapon_y<double>",
              "weapon_z<double>"};
      WsfCSV_EventOutputData::AddDataTags("WEAPON_FIRE_REQUESTED", temp);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_FIRE_ABORTED", temp);
   }
   {
      // WEAPON_FIRED
      temp = {"time<time>",
              "event<string>",
              "platform<string>",
              "target<string>",
              "weapon_platform<string>",
              "engagement_serial_number<int>",
              "firing_lat<lat>",
              "firing_lon<lon>",
              "firing_alt<double>",
              "firing_x<double>",
              "firing_y<double>",
              "firing_z<double>",
              "firing_heading<angle>",
              "firing_pitch<angle>",
              "firing_roll<angle>",
              "ned_firing_speed<double>",
              "firing_velocity_n<double>",
              "firing_velocity_e<double>",
              "firing_velocity_d<double>",
              "eci_firing_speed<double>",
              "firing_velocity_x<double>",
              "firing_velocity_y<double>",
              "firing_velocity_z<double>",
              "ned_firing_acceleration<double>",
              "firing_acceleration_n<double>",
              "firing_acceleration_e<double>",
              "firing_acceleration_d<double>",
              "eci_firing_acceleration<double>",
              "firing_acceleration_x<double>",
              "firing_acceleration_y<double>",
              "firing_acceleration_z<double>",
              "target_lat<lat>",
              "target_lon<lon>",
              "target_alt<double>",
              "target_x<double>",
              "target_y<double>",
              "target_z<double>",
              "target_heading<angle>",
              "target_pitch<angle>",
              "target_roll<angle>",
              "ned_target_speed<double>",
              "target_velocity_n<double>",
              "target_velocity_e<double>",
              "target_velocity_d<double>",
              "eci_target_speed<double>",
              "target_velocity_x<double>",
              "target_velocity_y<double>",
              "target_velocity_z<double>",
              "ned_target_acceleration<double>",
              "target_acceleration_n<double>",
              "target_acceleration_e<double>",
              "target_acceleration_d<double>",
              "eci_target_acceleration<double>",
              "target_acceleration_x<double>",
              "target_acceleration_y<double>",
              "target_acceleration_z<double>",
              "slant_range<double>",
              "ground_range<double>",
              "down_range<double>",
              "cross_range<double>",
              "relative_altitude<double>",
              "track_id<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, WsfCSV_EventOutputData::mTrackDataTags);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_FIRED", temp);
   }
   {
      // WEAPON_HITMISSKILL
      temp = {"time<time>", "event<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, mWE_PlayerDataTags);
      WsfCSV_EventOutputData::AppendDataTags(temp, {"start_time<time>"});
      WsfCSV_EventOutputData::AppendDataTags(temp, mWE_TerminationTags);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_HIT", temp);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_MISSED", temp);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_KILLED", temp);
   }
   {
      // WEAPON_MODE_STATUS
      temp = {"time<time>", "event<string>", "platform<string>", "weapon<string>", "mode<string>"};
      WsfCSV_EventOutputData::AddDataTags("WEAPON_MODE_ACTIVATED", temp);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_MODE_DEACTIVATED", temp);
   }
   {
      // WEAPON_RELOAD_STATUS
      temp = {"time<time>", "event<string>", "platform<string>", "weapon<string>", "weapon_qty<int>"};
      WsfCSV_EventOutputData::AddDataTags("WEAPON_RELOAD_STARTED", temp);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_RELOAD_ENDED", temp);
   }
   {
      // WEAPON_TERMINATED
      temp = {"time<time>", "event<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, mWE_PlayerDataTags);
      WsfCSV_EventOutputData::AppendDataTags(temp, {"start_time<time>"});
      WsfCSV_EventOutputData::AppendDataTags(temp, mWE_TerminationTags);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_TERMINATED", temp);
   }
   {
      // WEAPON_STATUS
      WsfCSV_EventOutputData::AddDataTags("WEAPON_OPERATIONAL", printSystemHeader);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_TURNED_ON", printSystemHeader);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_TURNED_OFF", printSystemHeader);
      WsfCSV_EventOutputData::AddDataTags("WEAPON_NON_OPERATIONAL", printSystemHeader);
   }
   {
      // WEAPON_SELECTED
      temp = {"time<time>", "event<string>", "platform<string>", "weapon<string>"};
      WsfCSV_EventOutputData::AppendDataTags(temp, {aCSV_EventOutput.GetLocationDataStr()});
      WsfCSV_EventOutputData::AddDataTags("WEAPON_SELECTED", temp);
   }
}
} // namespace

void WSF_MIL_EXPORT Register_wsf_mil(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_mil"))
   {
      aApplication.RegisterFeature("wsf_mil");
      aApplication.RegisterFeature("mil", "wsf_mil");
      aApplication.RegisterExtension("wsf_mil", ut::make_unique<MilApplicationExtension>());
      aApplication.ExtensionDepends("wsf_mil", "ext_interface", true);
      aApplication.ExtensionDepends("wsf_mil", "dis_interface", false);
      aApplication.ExtensionDepends("wsf_mil", "event_output", false);
      aApplication.ExtensionDepends("wsf_mil", "event_pipe", false);
      aApplication.ExtensionDepends("wsf_mil", "csv_event_output", false);

      WSF_REGISTER_EXTENSION(aApplication, xio_weapon_interface);
      WSF_REGISTER_EXTENSION(aApplication, laser_designations);
   }
}

//! Returns a pointer to the mil extension associated with a given scenario.
//! @note This can return a null pointer if the extension does not exist.
WsfMilExtension* WsfMilExtension::Find(const WsfScenario& aScenario)
{
   return static_cast<WsfMilExtension*>(aScenario.FindExtension("wsf_mil"));
}

//! Returns a reference to the mil extension associated with a given scenario.
//! @note This will throw an exception if the extension does not exist.
WsfMilExtension& WsfMilExtension::Get(const WsfScenario& aScenario)
{
   WsfMilExtension* milExtPtr(static_cast<WsfMilExtension*>(aScenario.FindExtension("wsf_mil")));
   if (milExtPtr != nullptr)
   {
      return *milExtPtr;
   }
   throw UtException("wsf_mil extension does not exist");
}

WsfMilExtension::WsfMilExtension()
   : mPkTableManagerPtr(new WsfPk::TableManager)
   , mAcousticSignatureTypes(nullptr)
   , mCN2_TypesPtr(nullptr)
   , mEA_EP_Types(nullptr)
   , mEW_EffectTypes(nullptr)
   , mEW_TechniqueTypes(nullptr)
   , mFalseTargetScreenerTypes(nullptr)
   , mFalseTargetTypes(nullptr)
   , mGuidanceProgramTypes(nullptr)
   , mHighEnergyLaserTypes(nullptr)
   , mInfraredSignatureTypes(nullptr)
   , mInherentContrastTypes(nullptr)
   , mLaunchComputerTypes(nullptr)
   , mOpticalPathCoefficientTypes(nullptr)
   , mOpticalReflectivityTypes(nullptr)
   , mOpticalSignatureTypes(nullptr)
   , mWeaponEffectsTypes(nullptr)
   , mWeaponTypes(nullptr)
{
}

WsfMilExtension::~WsfMilExtension()
{
   delete mGuidanceProgramTypes;        // not registered with scenario types
   delete mOpticalPathCoefficientTypes; // not registered with scenario types
   delete mPkTableManagerPtr;           // not registered with scenario types
}

void WsfMilExtension::AddedToScenario()
{
   WsfScenario& scenario = GetScenario();

   mGuidanceProgramTypes = new WsfGuidanceProgramTypes;

   // Comm
   WsfMilCommTypes::Register(scenario);

   // Movers
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_FORMATION_FLYER", ut::make_unique<WsfFormationFlyer>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_GUIDED_MOVER", ut::make_unique<WsfGuidedMover>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_PARABOLIC_MOVER", ut::make_unique<WsfParabolicMover>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_STRAIGHT_LINE_MOVER", ut::make_unique<WsfStraightLineMover>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_SUBSURFACE_MOVER", ut::make_unique<WsfSubsurfaceMover>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_TBM_MOVER", ut::make_unique<WsfTBM_Mover>(scenario));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_UNGUIDED_MOVER", ut::make_unique<WsfGuidedMover>(scenario, false));
   WsfMoverTypes::Get(scenario).AddCoreType("WSF_TOWED_MOVER", ut::make_unique<WsfTowedMover>(scenario));

   // Processors
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_AIR_TARGET_FUSE", ut::make_unique<WsfAirTargetFuse>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_FUSION_CENTER", ut::make_unique<WsfFusionCenter>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_GROUND_TARGET_FUSE", ut::make_unique<WsfGroundTargetFuse>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_GUIDANCE_COMPUTER", ut::make_unique<WsfGuidanceComputer>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_IMAGE_PROCESSOR", ut::make_unique<WsfImageProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_INTERSECT_PROCESSOR",
                                                ut::make_unique<WsfIntersectProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_VIDEO_PROCESSOR",
                                                ut::make_unique<WsfImageProcessor>(scenario)); // For compatibility
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_PERCEPTION_PROCESSOR",
                                                ut::make_unique<WsfPerceptionProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_QUANTUM_TASKER_PROCESSOR",
                                                ut::make_unique<WsfQuantumTaskerProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_THREAT_PROCESSOR", ut::make_unique<WsfThreatProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_TRACK_CLASSIFIER",
                                                ut::make_unique<WsfClassificationProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_UPLINK_PROCESSOR", ut::make_unique<WsfUplinkProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_WEAPON_FUSE", ut::make_unique<WsfWeaponFuse>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_WEAPON_THREAT_PROCESSOR",
                                                ut::make_unique<WsfWeaponThreatProcessor>(scenario));
   WsfProcessorTypes::Get(scenario).AddCoreType("WSF_WEAPON_TRACK_PROCESSOR",
                                                ut::make_unique<WsfWeaponTrackProcessor>(scenario));

   // Register the component factory for the WsfTaskManager component that handles weapon stuff.
   WsfWeaponTaskManager::RegisterComponentFactory(scenario);

   // Correlation Strategies
   WsfCorrelationStrategyTypes::Get(scenario).AddCoreType("cluster", ut::make_unique<WsfClusterCorrelation>(scenario));

   // Antenna Patterns
   WsfAntennaPatternTypes::Get(scenario).AddObjectFactory(WsfESA_AntennaPattern::ObjectFactory);

   // Sensors
   WsfMilSensorTypes::Register(scenario);
   WsfSensorErrorModelTypes::Get(scenario).AddObjectFactory(wsf::BistaticErrorModel::ObjectFactory);
   WsfSensorErrorModelTypes::Get(scenario).AddObjectFactory(wsf::OTH_RadarSensorErrorModel::ObjectFactory);
   WsfSensorErrorModelTypes::Get(scenario).AddObjectFactory(wsf::SurfaceWaveRadarSensorErrorModel::ObjectFactory);
   WsfCTD_SensorComponent::RegisterComponentFactory(scenario);

   // Optical
   mOpticalPathCoefficientTypes = new WsfOpticalPathCoefficientTypes;
   WsfEM_AttenuationTypes::Get(scenario).AddObjectFactory(WsfOpticalAttenuation::GetObjectFactory(scenario));

   // Clutter
   WsfEM_ClutterTypes::Get(scenario).AddObjectFactory(WsfEM_SurfaceClutter::ObjectFactory);

   // Weapon Effects
   auto weaponEffectsTypes = ut::make_unique<WsfWeaponEffectsTypes>(scenario);
   mWeaponEffectsTypes     = weaponEffectsTypes.get();
   scenario.AddTypeList(std::move(weaponEffectsTypes));

   // Launch Computers
   auto launchComputerTypes = ut::make_unique<WsfLaunchComputerTypes>(scenario);
   mLaunchComputerTypes     = launchComputerTypes.get();
   scenario.AddTypeList(std::move(launchComputerTypes));
   void Register_BallisticMissileLaunchComputer(WsfScenario&);
   Register_BallisticMissileLaunchComputer(scenario);

   // Weapons
   auto highEnergyLaserTypes = ut::make_unique<WsfHighEnergyLaserTypes>(scenario);
   mHighEnergyLaserTypes     = highEnergyLaserTypes.get();
   scenario.AddTypeList(std::move(highEnergyLaserTypes));
   auto weaponTypes = ut::make_unique<WsfWeaponTypes>(scenario);
   mWeaponTypes     = weaponTypes.get();
   scenario.AddTypeList(std::move(weaponTypes));

   // Electronic Warfare
   auto EA_EP_Types = ut::make_unique<WsfEW_EA_EP_Types>(scenario);
   mEA_EP_Types     = EA_EP_Types.get();
   scenario.AddTypeList(std::move(EA_EP_Types));
   auto EW_EffectTypes = ut::make_unique<WsfEW_EffectTypes>(scenario);
   mEW_EffectTypes     = EW_EffectTypes.get();
   scenario.AddTypeList(std::move(EW_EffectTypes));
   auto EW_TechniqueTypes = ut::make_unique<WsfEW_TechniqueTypes>(scenario);
   mEW_TechniqueTypes     = EW_TechniqueTypes.get();
   scenario.AddTypeList(std::move(EW_TechniqueTypes));
   auto falseTargetScreenerTypes = ut::make_unique<WsfFalseTargetScreenerTypes>(scenario);
   mFalseTargetScreenerTypes     = falseTargetScreenerTypes.get();
   scenario.AddTypeList(std::move(falseTargetScreenerTypes));
   auto falseTargetTypes = ut::make_unique<WsfFalseTargetTypes>(scenario);
   mFalseTargetTypes     = falseTargetTypes.get();
   scenario.AddTypeList(std::move(falseTargetTypes));

   // Signatures
   auto acousticSignatureTypes = ut::make_unique<WsfAcousticSignatureTypes>(scenario);
   mAcousticSignatureTypes     = acousticSignatureTypes.get();
   scenario.AddTypeList(std::move(acousticSignatureTypes));
   auto infraredSignatureTypes = ut::make_unique<WsfInfraredSignatureTypes>(scenario);
   mInfraredSignatureTypes     = infraredSignatureTypes.get();
   scenario.AddTypeList(std::move(infraredSignatureTypes));
   auto inherentContrastTypes = ut::make_unique<WsfInherentContrastTypes>(scenario);
   mInherentContrastTypes     = inherentContrastTypes.get();
   scenario.AddTypeList(std::move(inherentContrastTypes));
   auto opticalReflectivityTypes = ut::make_unique<WsfOpticalReflectivityTypes>(scenario);
   mOpticalReflectivityTypes     = opticalReflectivityTypes.get();
   scenario.AddTypeList(std::move(opticalReflectivityTypes));
   auto opticalSignatureTypes = ut::make_unique<WsfOpticalSignatureTypes>(scenario);
   mOpticalSignatureTypes     = opticalSignatureTypes.get();
   scenario.AddTypeList(std::move(opticalSignatureTypes));

   WsfOpticalSignatureTypes::Get(scenario).AddObjectFactory(WsfCompositeOpticalSignature::ObjectFactory);
   WsfRadarSignatureTypes::Get(scenario).SetObjectFactoryDefault<WsfMilRadarSignature>();
   WsfRadarSignatureTypes::Get(scenario).AddObjectFactory(WsfMilRadarSignature::ObjectFactory);

   // CN2
   auto CN2_Types = ut::make_unique<wsf::CN2_Types>(scenario);
   mCN2_TypesPtr  = CN2_Types.get();
   scenario.AddTypeList(std::move(CN2_Types));

   WsfExtInput::Find(GetScenario())->mData.AddComponent(new WsfMilExtInterface(this));

   wsf::dis::ScenarioExtension* disExtensionPtr = wsf::dis::ScenarioExtension::Find(GetScenario());
   if (disExtensionPtr != nullptr)
   {
      disExtensionPtr->AddComponent(new WsfMilDisInterface(this));
   }
   // If the "event_pipe" extension has been defined then hook things up so our events will also be logged.
   WsfEventPipeExtension* eventPipePtr = WsfEventPipeExtension::Find(GetScenario());
   if (eventPipePtr != nullptr)
   {
      WsfMilEventPipe::RegisterEvents(*eventPipePtr);
   }
}

bool WsfMilExtension::ProcessInput(UtInput& aInput)
{
   bool myCommand(true);
   if (mPkTableManagerPtr->ProcessInput(aInput))
   {
   }
   else if (mOpticalPathCoefficientTypes->LoadType(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfMilExtension::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfMilInterface>());
}

WsfMilInterface::WsfMilInterface()
   : mEW_EffectManagerPtr(ut::make_unique<WsfEW_EffectManager>())
   , mQuantumTaskerObserverPtr(ut::make_unique<WsfQuantumTaskerObserver>())
   , mWeaponObserverPtr(ut::make_unique<WsfWeaponObserver>())
   , mWeaponTaskObserverPtr(ut::make_unique<WsfWeaponTaskObserver>())
   , mMilEventPipePtr()
{
}

WsfMilInterface::~WsfMilInterface() {}

WsfMilInterface* WsfMilInterface::Find(const WsfSimulation& aSimulation)
{
   return static_cast<WsfMilInterface*>(aSimulation.FindExtension("wsf_mil"));
}

WsfMilInterface* WsfMilInterface::Find(const WsfSimulation* aSimulationPtr)
{
   return static_cast<WsfMilInterface*>(aSimulationPtr->FindExtension("wsf_mil"));
}

void WsfMilInterface::AddedToSimulation()
{
   // Hook in the handers for our events...
   WsfMilScriptObserver::Register_script_observer(*WsfScriptObserver::Find(GetSimulation()));

   // If the event_output extension is available, hook in the handlers for our events.
   WsfEventOutput* eventOutputPtr = WsfEventOutput::Find(GetSimulation());
   if (eventOutputPtr != nullptr)
   {
      Register_event_output(*eventOutputPtr);
      eventOutputPtr->GetSettings().SetSNR_InterferenceLabel("S/(N+C+J)");
      eventOutputPtr->GetSettings().SetInterferencePowerLabel("Noise_Jammer_Power");
      auto msgPrinter = [eventOutputPtr](double aSimTime, const WsfMessage& aMessage)
      { PrintAssetMessage(*eventOutputPtr, aSimTime, aMessage); };
      eventOutputPtr->RegisterMessagePrinter(WsfAssetMessage::GetTypeId(), msgPrinter);
   }

   // If the csv_event_output extension is available, hook in the handlers for our events.
   WsfCSV_EventOutput* csvEventOutputPtr = WsfCSV_EventOutput::Find(GetSimulation());
   if (csvEventOutputPtr != nullptr)
   {
      Register_event_output(*csvEventOutputPtr);
      auto msgPrinter = [csvEventOutputPtr](double aSimTime, const WsfMessage& aMessage)
      { PrintAssetMessageCSV(*csvEventOutputPtr, aSimTime, aMessage); };
      csvEventOutputPtr->RegisterMessagePrinter(WsfAssetMessage::GetTypeId(), msgPrinter);
      AddCSV_DataTags(*csvEventOutputPtr);
   }

   // If the event_pipe extension is available, hook in the handers for our events.
   WsfEventPipeInterface* eventPipePtr = WsfEventPipeInterface::Find(GetSimulation());
   if (eventPipePtr != nullptr)
   {
      mMilEventPipePtr = ut::make_unique<WsfMilEventPipe>(eventPipePtr);
      eventPipePtr->AddLogger(mMilEventPipePtr.get());
   }

   WsfFrameStepSimulation* simPtr = dynamic_cast<WsfFrameStepSimulation*>(&GetSimulation());
   if (simPtr != nullptr)
   {
      mCallbacks += simPtr->AdvanceFrameObjects.Connect(&WsfMilInterface::FrameObjectUpdate, this);
      mCallbacks += mWeaponObserverPtr->WeaponTurnedOn.Connect(&WsfMilInterface::WeaponTurnedOn, this);
      mCallbacks += mWeaponObserverPtr->WeaponTurnedOff.Connect(&WsfMilInterface::WeaponTurnedOff, this);
      mCallbacks += WsfObserver::PlatformDeleted(&GetSimulation()).Connect(&WsfMilInterface::PlatformDeleted, this);
   }
}

////////////////////////////////////////////////////////////////////////////
//  Frame-step simulation support
////////////////////////////////////////////////////////////////////////////
void WsfMilInterface::FrameObjectUpdate(double aCurrentFrameTime)
{
   // Update list of weapons given state changes
   for (auto& i : mWeaponsChangingState)
   {
      auto wpnListIter = std::find(mWeapons.begin(), mWeapons.end(), i.first);
      if (i.second && (wpnListIter == mWeapons.end()))
      {
         mWeapons.push_back(i.first);
      }
      else if ((!i.second) && (wpnListIter != mWeapons.end()))
      {
         mWeapons.erase(wpnListIter);
      }
   }
   mWeaponsChangingState.clear();

   // now update weapons
   for (auto& weapon : mWeapons)
   {
      weapon->Update(aCurrentFrameTime);
   }
}

void WsfMilInterface::WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mWeaponsChangingState[aWeaponPtr] = false;
}

void WsfMilInterface::WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr)
{
   mWeaponsChangingState[aWeaponPtr] = true;
}

void WsfMilInterface::PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr)
{
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*aPlatformPtr); !iter.AtEnd(); ++iter)
   {
      mWeaponsChangingState[*iter] = false;
   }
}

// =================================================================================================
//! Kill a platform.
//!
//! This is very similar to DeletePlatform except that is uses the platform 'on_death_action' to
//! determine if the platform should be deleted (removed from the simulation), or retained and disabled.
//!
//! @param aSimTime     Current simulation time.
//! @param aPlatformPtr Pointer to platform to be killed.
//!
//! @note This is only intended to be called from the weapon engagement routines.
// virtual
void WsfMilInterface::KillPlatform(double aSimTime, WsfPlatform* aPlatformPtr)
{
   WsfSimulation& sim = GetSimulation();
   if ((aPlatformPtr == nullptr) || (sim.GetPlatformByIndex(aPlatformPtr->GetIndex()) == nullptr))
   {
      // No platform specified, or the platform is not active. Let DeletePlatform handle these actions.
      sim.DeletePlatform(aSimTime, aPlatformPtr);
   }
   else if (aPlatformPtr->GetOnBrokenAction() == WsfPlatform::cON_BROKEN_REMOVE)
   {
      // If the platform is to simply be removed just treat it as a normal platform deletion.
      sim.DeletePlatform(aSimTime, aPlatformPtr);
   }
   else if (!aPlatformPtr->IsBroken())
   {
      aPlatformPtr->SetBroken();

      // This shouldn't happen, but in case the platform could be deleted before the broken event is dispatched,
      // capture the platform index rather than the pointer to the platform itself.
      size_t index = aPlatformPtr->GetIndex();
      sim.AddEvent(ut::make_unique<WsfOneShotEvent>(aSimTime,
                                                    [=, &sim]()
                                                    {
                                                       auto* platformPtr = sim.GetPlatformByIndex(index);
                                                       if (platformPtr != nullptr)
                                                       {
                                                          sim.ProcessPlatformBrokenEvent(aSimTime, platformPtr);
                                                       }
                                                    }));
   }
}
