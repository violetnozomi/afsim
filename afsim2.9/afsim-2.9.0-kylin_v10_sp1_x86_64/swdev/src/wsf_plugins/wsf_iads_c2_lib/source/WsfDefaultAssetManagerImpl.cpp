// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <memory>

/*******************************************************************************
*
*   File: WsfDefaultAssetManagerImpl.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract: This class provides default implementations in C++ for
*    what could be overridden in script for the Asset Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#include "WsfDefaultAssetManagerImpl.hpp"

#include <memory>

#include "UtMath.hpp"
#include "UtMemory.hpp"

#include "WsfAssetMessage.hpp"
#include "WsfCommandChain.hpp"
#include "WsfCovariance.hpp"
#include "WsfDefaultSensorScheduler.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfWeapon.hpp"

#include "logger.hpp"
#include "iadsLib/BattleManagerIface.hpp"
#include "iadsLib/NumericUtils.hpp"
#include "iadsLib/util.hpp"

#include "WsfAssetManager.hpp"
#include "WsfBMAssignmentStatusMessage.hpp"
#include "WsfBMAssignmentTrackMessage.hpp"
#include "WsfBMAuxDataZoneAttachment.hpp"
#include "WsfBMCueMessage.hpp"
#include "WsfBMDisseminateC2.hpp"
#include "WsfBMUtils.hpp"
#include "WsfBMSensorInfoRecord.hpp"
#include "WsfBMSensorsManager.hpp"
#include "WsfBMWeaponInfoRecord.hpp"
#include "WsfBMWeaponsManager.hpp"
#include "WsfBMZoneRecord.hpp"
#include "WsfSensorsManagerFovSensorScheduler.hpp"
#include "WsfSensorsManagerFovImpl.hpp"
#include "WsfScriptAssetManagerClass.hpp"
#include "WsfScriptBattleManagerClass.hpp"
#include "WsfScriptDisseminateC2Class.hpp"
#include "WsfScriptSensorsManagerClass.hpp"
#include "WsfScriptWeaponsManagerClass.hpp"

WsfDefaultAssetManagerImpl::WsfDefaultAssetManagerImpl(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger)
   : m_iads_initialized(false)
   , m_bm_ref(nullptr)
   , m_dc2_ref(nullptr)
   , m_wm_ref(nullptr)
   , m_sm_ref(nullptr)
   , m_threat_check_believed_alive(false)
   , m_max_track_grouping_distance_m(50)
   , m_uncorrelated_assignments()
   , m_uncorrelated_cues()
   , mGlobalLogger(aGlobalLogger)
{
}

WsfDefaultAssetManagerImpl::~WsfDefaultAssetManagerImpl()
{
   if (!m_uncorrelated_assignments.empty())
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Destroying WsfDefaultAssetManagerImpl but pending uncorrelated assignments exist. Need culling timer.\n";
   }
   if (!m_uncorrelated_cues.empty())
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Destroying WsfDefaultAssetManagerImpl but pending uncorrelated cues exist. Need culling timer.\n";
   }
}

bool WsfDefaultAssetManagerImpl::ProcessInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "filter_dead_tracks")
   {
      m_threat_check_believed_alive = aInput.ReadBool();
   }
   else if (command == "max_track_grouping_distance")
   {
      aInput.ReadValueOfType(m_max_track_grouping_distance_m, UtInput::cLENGTH);
   }
   else
      // unknown block
   {
      processed = false;
   }

   return processed;
}

void WsfDefaultAssetManagerImpl::Initialize(WsfAssetManager* am)
{
   HCL_DEBUG_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << "/" << am->GetName() << ": " << am->GetScriptClassName() << ": in on_initialize" << std::endl;
}

void WsfDefaultAssetManagerImpl::Initialize2(WsfAssetManager* am)
{
   HCL_DEBUG_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << "/" << am->GetName() << ": " << am->GetScriptClassName() << ": in on_initialize2" << std::endl;

   m_bm_ref = WsfBMUtils::FindAttachedBattleManagerRef2(am->GetPlatform(), false);
   m_dc2_ref = WsfBMUtils::FindAttachedDisseminationManagerRef2(am->GetPlatform(), false);
   m_wm_ref = WsfBMUtils::FindAttachedWeaponsManagerRef2(am->GetPlatform(), false);
   m_sm_ref = WsfBMUtils::FindAttachedSensorsManagerRef2(am->GetPlatform(), false);

   // just some debugging carryover from script - may eventually delete
   for (unsigned proc_idx = 0; proc_idx < am->GetPlatform()->GetComponentCount<WsfProcessor>(); ++proc_idx)
   {
      auto proc = am->GetPlatform()->GetComponentEntry<WsfProcessor>(proc_idx);
      HCL_DEBUG_LOGGER(mGlobalLogger) << "proc name = " << proc->GetName() << " type = " << proc->GetType() << std::endl;

      if (proc->IsA_TypeOf(WsfScriptBattleManagerClass::BASE_CLASS_NAME))
      {
         HCL_INFO_LOGGER(mGlobalLogger) << "found battle manager = " << proc->GetName() << std::endl;
      }
   }
}

void WsfDefaultAssetManagerImpl::PrintStatus(WsfAssetManager* am, const bool direct_only) const
{
   if (!am->GetCoreAMRef()->getLogMyStatus())
   {
      return;
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "------------------------------------------------------------------------\n"
                     << "------------------------------------- Logging AM ("
                     << am->GetCoreAMRef()->getThisAsset()->getName()
                     << ") Asset status -------------------------------------";
   am->GetCoreAMRef()->logMyStatus();
   am->GetCoreAMRef()->logSubordinateStatus(direct_only);
   HCL_INFO_LOGGER(mGlobalLogger) << "------------------------------------------------------------------------";
}

void WsfDefaultAssetManagerImpl::on_update(const double sim_time_s, WsfPlatform* this_plat, WsfAssetManager* am)
{
   // note: since the chain of command objects are not initialized until after initialze2, we have no choice but to put it here
   if (!m_iads_initialized)
   {
      InitializeIADSChainOfCommand(sim_time_s, this_plat, am);
      m_iads_initialized = true;
   }

   // Progress time
   am->GetCoreAMRef()->updateTime(sim_time_s);

   // check that we have offset by decision update delay before doing anything else. HELIOS would
   // invoke status checking and whatnot but this over-complicates things. It worked in HELIOS because
   // they didn't have a formal outbound queue, just transmit flags. This meant that nothing would be transmitter
   // anyways and since every other processor is driven off this processor, nothing local uses it anyway
   // FJulianType DeltaProcessTime = Time - this->LastProcessTime
   auto delta_process_time = sim_time_s - am->GetCoreAMRef()->getLastProcessTime();
   if (!NumericUtils::GreaterThanEqual(delta_process_time, am->GetCoreAMRef()->getDecisionUpdateDelay()))
   {
      return;
   }

   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger();

   // The next call is basically GTIQDAssetManagerProcess::GTIQDAssetManagerSetPosition() and then
   // GTIQDAssetManagerProcess::GTIQDAssetManagerSetStatus() rolled into one.
   // Update our own location and status.
   double loc_xyz[3] = { 0 }, vel_xyz[3] = { 0 };
   this_plat->GetLocationWCS(loc_xyz);
   this_plat->GetVelocityWCS(vel_xyz);
   am->GetCoreAMRef()->updateOwnStatus(*logger, sim_time_s, loc_xyz, vel_xyz);

   // GTIQDAssetManagerProcess()::...UpdateAssignmentTrackParameters()

   // for regressions, print statuses each time we go through the loop
   PrintStatus(am);

   bool updated_threats = UpdateThreatPerceptions(sim_time_s, this_plat, am) > 0;
   // If we've updated the threat perceptions, re-run strength calculations
   if (updated_threats)
   {
      am->GetCoreAMRef()->CalculateTrackStrengths(sim_time_s, m_max_track_grouping_distance_m);
   }

   // GTIQDAssetManagerProcess()::GTIQDAssetManagerAssessOnGoingAssignments()
   am->GetCoreAMRef()->AssessOngoingAssignments(*logger);

   // Update our assignable track array
   am->GetCoreAMRef()->PrepareTracksForEvaluation();

   // Ensure our DA array is properly updated
   am->GetCoreAMRef()->UpdateDefendedAssetsArray();

   // Run the model
   if (m_bm_ref && !m_bm_ref->AllowAutonomousUpdates())
   {
      m_bm_ref->Run(sim_time_s);
   }

   // Promote evaluated assignments to active
   am->GetCoreAMRef()->CreateAssignments(*logger);

   // TTD: GTIQDAssetManagerProcess()::GTIQDAssetManagerProcessInitiatedAssignments()
   // This unit is really concerned with assignments made on self. I guess we need to ensure we don't
   // get into an infinite loop of assignment processing but this unit is more along the lines of an implementation
   // detail of the HELIOS asset manager moreso than operational processing.

   // GTIQDAssetManagerProcess()::GTIQDAssetManagerReadyCompleteAssignments()
   am->GetCoreAMRef()->CompleteAssignments(*logger);

   // Invoke monitor assignments
   if (am->GetCoreBMRef() && am->GetCoreBMRef()->getCommitAuthority())
   {
      am->GetCoreAMRef()->MonitorAssignments(*logger);
   }

   // Sensor Manager
   if (m_sm_ref && !m_sm_ref->AllowAutonomousUpdates())
   {
      m_sm_ref->Run(sim_time_s);
   }

   // Weapons Manager
   if (m_wm_ref && !m_wm_ref->AllowAutonomousUpdates())
   {
      m_wm_ref->Run(sim_time_s);
   }

   // Ensure the Sensor Manager doesn't miss completed assignments in the single pass
   if (m_sm_ref && !m_sm_ref->AllowAutonomousUpdates())
   {
      m_sm_ref->GetCoreSMRef()->DropCompletedAssignments(*logger, sim_time_s);
   }

   // C2 Dissemination
   if (m_dc2_ref && !m_dc2_ref->AllowAutonomousUpdates())
   {
      m_dc2_ref->Run(sim_time_s);
   }

   // Update last process time
   am->GetCoreAMRef()->setLastProcessTime(sim_time_s);
}

bool WsfDefaultAssetManagerImpl::on_message(const double sim_time_s, const WsfMessage& message, WsfAssetManager* am)
{
   // if we don't want to block other processors from recieving the message, we must keep this flag false,
   // otherwise nobody else will get a chance to process it. We do this so that other processors have a shot regardless if
   // we process it or not.
   bool block_processing_message = false;

   if (message.GetType() == WsfAssetMessage::GetTypeId())
   {
      // nothing to do - assets are handled periodically from the perception processor
   }
   else if (message.GetType() == WsfTrackMessage::GetTypeId())
   {
      // nothing to do - tracks are handled periodically from the perception processor
   }
   else if (message.GetType() == WsfTrackDropMessage::GetTypeId())
   {
      // nothing to do - drop tracks are handled asynchronously from the track processor->WsfBattleManager drop track callback connection
   }
   else if (message.GetType() == WsfBMAssignmentMessage::GetTypeId())
   {
      auto assignment = static_cast<const WsfBMAssignmentMessage&>(message);
      auto assigned_plat = assignment.GetAssignedPlatform(am->GetSimulation());
      auto assigned_wpn = assignment.GetAssignedWeapon(am->GetSimulation());
      HCL_INFO_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << ":" << am->GetName() << " Received WsfBMAssignmentMessage for "
                        << (assigned_plat ? assigned_plat->GetName() : "*ERROR * Deleted platform? ") << ":"
                        << (assigned_wpn ? assigned_wpn->GetName() : "*ERROR * Deleted Weapon? ")
                        << ": " << assignment.GetAssignmentReasonString() << std::endl;

      HandleAssignmentMessage(sim_time_s, assignment, am);
   }
   else if (message.GetType() == WsfBMAssignmentStatusMessage::GetTypeId())
   {
      auto astatus = static_cast<const WsfBMAssignmentStatusMessage&>(message);
      auto assigned_plat = astatus.GetAssignedPlatform(am->GetSimulation());
      auto assigned_wpn = astatus.GetAssignedWeapon(am->GetSimulation());

      HCL_DEBUG_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << ":" << am->GetName() << " Received WsfBMAssignmentStatusMessage for "
                         << (assigned_plat ? assigned_plat->GetName() : "*ERROR * Deleted platform? ") << ":"
                         << (assigned_wpn ? assigned_wpn->GetName() : "*ERROR * Deleted Weapon? ")
                         << ": " << il::AssignmentStatusToString(astatus.GetAssignmentStatus()) << std::endl;
      HandleAssignmentStatusUpdate(sim_time_s, astatus, am);
   }
   else if (message.GetType() == WsfBMStatusMessage::GetTypeId())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << ":" << am->GetName() << " Received WsfBMStatusMessage for " << static_cast<const WsfBMStatusMessage&>(message).GetPlatformName() << " at time " << sim_time_s << std::endl;
      if (IsAssetPerceptionValid(am->GetSimulation(), static_cast<const WsfBMStatusMessage&>(message)))
      {
         HandleAssetStatusUpdate(am, static_cast<const WsfBMStatusMessage&>(message));
      }
      else
      {
         HCL_ERROR_LOGGER(mGlobalLogger) << "Received WsfBMStatusMessage but IsAssetPerceptionValid() failed..." << std::endl;
      }
   }
   else if (message.GetType() == WsfBMCueMessage::GetTypeId())
   {
      auto cued_plat = static_cast<const WsfBMCueMessage&>(message).GetCuedPlatform(am->GetSimulation());
      auto cued_sensor = static_cast<const WsfBMCueMessage&>(message).GetCuedSensor(am->GetSimulation());

      HCL_DEBUG_LOGGER(mGlobalLogger) << am->GetPlatform()->GetName() << ":" << am->GetName() << " Received WsfBMCueMessage for "
                         << (cued_plat ? cued_plat->GetName() : "*ERROR * Deleted platform? ") << ":"
                         << (cued_sensor ? cued_sensor->GetName() : "*ERROR * Deleted sensor? ") << std::endl;

      HandleSensorCueMessage(sim_time_s, am, static_cast<const WsfBMCueMessage&>(message));
   }
   else if (message.GetType() == WsfBMAssignmentTrackMessage::GetTypeId())
   {
      auto assignment_track = static_cast<const WsfBMAssignmentTrackMessage&>(message);

      HCL_DEBUG_LOGGER(mGlobalLogger) << sim_time_s << ": " << am->GetPlatform()->GetName() << ":" << am->GetName()
                         << " Received WsfBMAssignmentTrackMessage: " << assignment_track.GetTrack().GetTrackId()
                         << std::endl;

      HandleAssignmentTrackMessage(sim_time_s, am, assignment_track);
   }
   else if (message.GetType() == WsfStatusMessage::GetTypeId())
   {
      auto afsim_status = static_cast<const WsfStatusMessage&>(message);
      HCL_DEBUG_LOGGER(mGlobalLogger) << sim_time_s << ": "
                         << am->GetPlatform()->GetName() << ":" << am->GetName() << " Received " << message.GetType() << "/" << message.GetSubType()
                         << " for " << afsim_status.GetOriginator() << ":" << afsim_status.GetSystemName();
   }
   else
   {
      HCL_WARN_LOGGER(mGlobalLogger) << sim_time_s << ": Got unhandled message!!!! Type/SubType = " << message.GetType() << "/" << message.GetSubType() << std::endl;
   }

   return block_processing_message;
}

void WsfDefaultAssetManagerImpl::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, WsfAssetManager* am)
{
   HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": HandleDropTrackCallback: Got called" << std::endl;

   am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::airTargetMessage>(ConvertTrackDrop(am->GetSimulation(), aLocalTrackPtr).GetRawMessagePtr()));
}

void WsfDefaultAssetManagerImpl::LocalTrackInitiated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr, WsfAssetManager* am)
{
   if (!m_uncorrelated_assignments.empty())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::LocalTrackInitiated() and pending uncorrelated assignments...\n";
   }
   if (!m_uncorrelated_cues.empty())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::LocalTrackInitiated() and pending uncorrelated cues...\n";
   }

   CheckPendingAssignmentCorrelations(aSimTime, am->GetSimulation(), am);
   CheckPendingCueCorrelations(aSimTime, am->GetSimulation(), am);

   UpdateTrack(aSimTime, aLocalTrackPtr, am);
}

void WsfDefaultAssetManagerImpl::LocalTrackUpdated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr, WsfAssetManager* am)
{
   if (!m_uncorrelated_assignments.empty())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::LocalTrackUpdated() and pending uncorrelated assignments...\n";
   }
   if (!m_uncorrelated_cues.empty())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::LocalTrackInitiated() and pending uncorrelated cues...\n";
   }

   CheckPendingAssignmentCorrelations(aSimTime, am->GetSimulation(), am);
   CheckPendingCueCorrelations(aSimTime, am->GetSimulation(), am);

   UpdateTrack(aSimTime, aLocalTrackPtr, am);
}

void WsfDefaultAssetManagerImpl::InitializeIADSChainOfCommand(const double sim_time_s, WsfPlatform* this_plat, WsfAssetManager* am)
{
   HCL_INFO_LOGGER(mGlobalLogger) << "IADS chain of command not initialized, initializing asset map for platform " << this_plat->GetName() << std::endl;
   SetupIADSLaydown(this_plat, am);
   am->GetCoreAMRef()->setOwnID(WsfBMUtils::GetUnitIDRecordFromPlatform(this_plat)); // InitOwnNode()

   am->GetCoreAMRef()->setStartTime(sim_time_s);

   double loc_xyz[3] = { 0 }, vel_xyz[3] = { 0 };
   this_plat->GetLocationWCS(loc_xyz);
   this_plat->GetVelocityWCS(vel_xyz);
   am->GetCoreAMRef()->updateOwnStatus(*IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger(), sim_time_s, loc_xyz, vel_xyz);
}

void WsfDefaultAssetManagerImpl::SetupIADSLaydown(WsfPlatform* initing_plat, WsfAssetManager* am)
{
   // In effort to minimize overall memory usage of the asset map, asset records are created for all subordinates, direct peers and all commanders (and commanders of commanders)
   //  of only the initial platform for which the asset map is created. Each platform with an asset manager has its own asset map, representing its perception of the relevant
   //  command structure.

   auto& asset_map = am->GetCoreAMRef()->getAssets();

   // Initialize this asset record and recursively initialize subordinates
   std::shared_ptr<il::assetRecord> initing_asset = InitializeAssetAndSubordinates(initing_plat, asset_map, am);

   // Initialize commander and peers
   WsfPlatform* commander = WsfBMUtils::GetCommander(initing_plat);
   // NOTE: Adding the nullptr check for the result of WsfBMUtils::GetCommander means that this code won't attempt to call InitAssetRecord
   // and otherwise handle the peers of initing_plat IF WsfCommandChain::GetCommander() returns a nullptr.
   if (commander && commander != initing_plat)
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "Found commander, platform name: " << commander->GetName() << std::endl;
      auto cmdr_record = InitAssetRecord(commander, asset_map, am);

      auto command_chain = commander->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
      // Add subordinates of commander
      if (command_chain)
      {
         auto peers = command_chain->GetSubordinates();
         for (auto it = peers.cbegin(); it != peers.cend(); ++it)
         {
            const auto& peer = *it;
            if (peer != initing_plat)
            {
               HCL_DEBUG_LOGGER(mGlobalLogger) << "Found peer, platform name: " << peer->GetName() << std::endl;
               auto peer_record = InitAssetRecord(peer, asset_map, am);
               cmdr_record->addDirectSubordinate(peer_record->getID(), asset_map);
               initing_asset->addDirectPeer(peer_record->getID(), asset_map);
            }
            else
            {
               cmdr_record->addDirectSubordinate(initing_asset->getID(), asset_map);
            }
         }
      }
      // Initialize commander chain recursively
      InitializeAssetCommanders(commander, asset_map, am);
   }
}

std::shared_ptr<il::assetRecord> WsfDefaultAssetManagerImpl::InitializeAssetAndSubordinates(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am)
{
   auto asset_record = InitAssetRecord(plat, asset_map, am);

   auto command_chain = plat->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   // Add subordinates
   if (command_chain)
   {
      auto subordinates = command_chain->GetSubordinates();
      for (auto it = subordinates.cbegin(); it != subordinates.cend(); ++it)
      {
         const auto& sub = *it;
         HCL_DEBUG_LOGGER(mGlobalLogger) << "Found subordinate, platform name: " << sub->GetName() << std::endl;
         auto sub_record = InitializeAssetAndSubordinates(sub, asset_map, am);
         asset_record->addDirectSubordinate(sub_record->getID(), asset_map);
      }
   }

   return asset_record;
}

void WsfDefaultAssetManagerImpl::InitializeAssetCommanders(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am)
{
   auto commander = WsfBMUtils::GetCommander(plat);
   if (commander && commander != plat)
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "Found commander, platform name: " << commander->GetName() << std::endl;
      auto cmdr_record = InitAssetRecord(commander, asset_map, am);
      cmdr_record->addDirectSubordinate(WsfBMUtils::GetUnitIDRecordFromPlatform(plat), asset_map);

      InitializeAssetCommanders(commander, asset_map, am);
   }
}

void WsfDefaultAssetManagerImpl::EstablishNewCmdrSubRelationship(const WsfPlatform* new_plat, std::shared_ptr<il::assetRecord>& new_asset, il::assetMap& asset_map)
{
   auto commander_plat = WsfBMUtils::GetCommander(new_plat); // TTD: add support for command chain initializations that are not default
   if (commander_plat)
   {
      new_asset->addDirectCommander(WsfBMUtils::GetUnitIDRecordFromPlatform(commander_plat), asset_map);
   }
}

std::shared_ptr<il::assetRecord> WsfDefaultAssetManagerImpl::InitAssetRecord(const WsfPlatform* plat, il::assetMap& asset_map, WsfAssetManager* am)
{
   // Check if the asset already exists, and if so, only refresh the data
   // This can happen if a substantial portion of a friendly tree comes online
   // mid-run.
   bool new_record = false;

   auto asset_id = WsfBMUtils::GetUnitIDRecordFromPlatform(plat);
   auto asset_record = asset_map.GetAsset(asset_id);
   if (!asset_record)
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "InitAssetRecord: creating asset record for platform: " << plat->GetName() << '\n';
      asset_record = std::make_shared<il::assetRecord>(mGlobalLogger);
      new_record = true;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "InitAssetRecord: asset is already known for platform: " << plat->GetName() << '\n';
   }

   // Add all of the basic information first, then add it to the map before continuing,
   // This ensures that the map is primed with data initially
   if (new_record)
   {
      // InitNode
      asset_record->setID(asset_id);
      asset_record->setName(plat->GetName());
      asset_record->setSide(plat->GetSide());
      asset_record->setType(il::unitTypeRecord(il::C2_OTHER, "Other", INVALID_UNIT_TYPE, "<unset>"));
      if (WsfBMUtils::FindAttachedAssetManagerRef(plat))
      {
         asset_record->setLogMyStatus(WsfBMUtils::FindAttachedAssetManagerRef(plat)->getLogMyStatus());
      }

      // TTD: put in support for subtype for the other battle manager
      HCL_TRACE_LOGGER(mGlobalLogger) << "In Asset Map Add..." << '\n';
      asset_map.AddAsset(asset_record->getID(), asset_record);
   }

   // New record initialization - don't overwrite perception for existing units
   if (new_record)
   {
      // Basic info
      double loc_xyz[3] = { 0 }, vel_xyz[3] = { 0 };
      const_cast<WsfPlatform*>(plat)->GetLocationWCS(loc_xyz);
      const_cast<WsfPlatform*>(plat)->GetVelocityWCS(vel_xyz);
      asset_record->setPositionXYZ(loc_xyz[0], loc_xyz[1], loc_xyz[2]);
      asset_record->setVelocityXYZ(vel_xyz[0], vel_xyz[1], vel_xyz[2]);

      // Find zones specified for this asset
      InitializeAssetZones(plat, asset_record);

      // Default to Red until we hear from the asset
      asset_record->setSystemStatus(il::STATUS_RED);

      // C2 max assignments
      asset_record->setMaxAssignments(WsfBMUtils::GetPlatformMaxAssignments(plat));

      // Current assignments - for ITASE, this concept isn't stored within the platforms so be wary of
      // accounting of assignments at the intermediate C2 nodes - it currently only works for weapons

      // current number of assignments -- sum across all weapons on platform - this only works for weapons units
      asset_record->setNumAssignments(WsfBMUtils::GetPlatformNumAssignments(plat));

      // Assignment delay
      asset_record->setAssignmentDelay(WsfBMUtils::GetPlatformNumAssignmentDelay(plat));

      // Weapon Info
      for (unsigned widx = 0; widx < plat->GetComponentCount<WsfWeapon>(); ++widx)
      {
         WsfWeapon* weapon = plat->GetComponentEntry<WsfWeapon>(widx);

         int num_munitions = static_cast<int>(weapon->GetQuantityRemaining());
         int prepped_munitions = num_munitions; // no concept of prepped vs. unprepped

         std::shared_ptr<WsfBMWeaponInfoRecord> weapon_interface(new WsfBMWeaponInfoRecord);

         weapon_interface->SetEstimatedTotalMunitions(num_munitions);
         weapon_interface->SetEstimatedMunitionsPrepped(prepped_munitions);

         std::string domain = WsfBMUtils::GetSpatialDomain(plat);
         if (domain == "air")
         {
            weapon_interface->Set(plat->GetSimulation(), asset_record, widx, il::WEAPON_AI);
         }
         else
         {
            weapon_interface->Set(plat->GetSimulation(), asset_record, widx, il::WEAPON_SAM);
         }
         // Add a reference to the attached weapons manager to the weapon record
         weapon_interface->SetWeaponsManager(WsfBMUtils::FindAttachedWeaponsManagerRef(plat, false));

         // Add a reference to the attached weapons manager to the weapon record
         weapon_interface->SetWeaponsManager(WsfBMUtils::FindAttachedWeaponsManagerRef(plat, false));

         // link weapon zones if any aux data is scripted for that
         InitializeWeaponZones(plat, weapon, weapon_interface.get());

         asset_record->AddWeapon(weapon_interface);

         switch (weapon_interface->GetTypeRecord().getType())
         {
            case il::WEAPON_SAM:
               asset_record->setType(il::unitTypeRecord(il::WEAPON_SAM, "SAM", INVALID_UNIT_TYPE, "<unset>"));
               break;
            case il::WEAPON_AI:
               asset_record->setType(il::unitTypeRecord(il::WEAPON_AI, "AI", INVALID_UNIT_TYPE, "<unset>"));
               break;
            case il::WEAPON_OTHER:
            default:
               asset_record->setType(il::unitTypeRecord(il::WEAPON_OTHER, "Other", INVALID_UNIT_TYPE, "<unset>"));
         }

         // TTD: add per-weapon fire channel logic here
         weapon_interface->SetTotalFireChannels(weapon->GetMaximumRequestCount());
         weapon_interface->SetEstimatedAllocatedFireChannels(weapon->GetActiveRequestCount());
         // need to rectify this with concept of max assignments at the platform level
      }

      il::SensorsManagerInterface* smInterface = am->GetCoreSMRef().get();
      bool isSensorManagerFov = dynamic_cast<WsfSensorsManagerFovImpl*>(smInterface) != nullptr;
      for (unsigned sidx = 0; sidx < plat->GetComponentCount<WsfSensor>(); ++sidx)
      {
         WsfSensor& sensor = *(plat->GetComponentEntry<WsfSensor>(sidx));
         il::SensorEnum sensorType = WsfBMUtils::GetSensorType(sensor);

         std::shared_ptr<WsfBMSensorInfoRecord> sensor_interface;
         switch (sensorType)
         {
            case il::SENSOR_EW:
               sensor_interface = std::make_shared<WsfBMEWSensorInfoRecord>();
               break;
            case il::SENSOR_TAR:
               sensor_interface = std::make_shared<WsfBMTARSensorInfoRecord>();
               break;
            case il::SENSOR_TTR:
               sensor_interface = std::make_shared<WsfBMTTRSensorInfoRecord>();
               break;
            case il::SENSOR_RWR:
               sensor_interface = std::make_shared<WsfBMRWRSensorInfoRecord>();
               break;
            default:
               HCL_ERROR_LOGGER(mGlobalLogger) <<
                                  "Initializing sensor " << sensor.GetName() << " on platform " << sensor.GetPlatform()->GetName() <<
                                  ", but sensor is of unknown type. Did you forget to assign category to EW, TAR, or TTR?\n";
         }

         if (sensor_interface)
         {
            // WsfDefaultSensorScheduler has some behaviors that interfere with the
            // FOV sensor manager's operation. The only way to get around this problem
            // is to replace the scheduler at runtime.
            if (isSensorManagerFov && sensorType == il::SENSOR_TTR)
            {
               auto* defaultScheduler = dynamic_cast<WsfDefaultSensorScheduler*>(sensor.GetScheduler());
               if (defaultScheduler == nullptr)
               {
                  HCL_ERROR_LOGGER(mGlobalLogger) <<
                                     sensor_interface->GetSsysID() <<
                                     ": TTRs controlled by an FOV sensor manager must use the default sensor scheduler";
               }
               else
               {
                  auto schedulerPtr = ut::make_unique<WsfSensorsManagerFovSensorScheduler>(*defaultScheduler);
                  // since WsfSensorsManagerFovSensorScheduler::Initialize may throw an exception,
                  // it is important to give the sensor ownership of the scheduler pointer first,
                  // so that something is able to delete it
                  sensor.SetScheduler(std::move(schedulerPtr));
                  // The copy constructor for WsfDefaultSensorScheduler (from which the FOV scheduler inherits)
                  // does not copy all fields. Initialize must be called to set up the rest of the fields. Note
                  // that the first parameter to Initialize (the simulation time) isn't used in
                  // WsfDefaultSensorScheduler, so it can be ignored.
                  if (!sensor.GetScheduler()->Initialize(0.0, &sensor, sensor.GetTracker()))
                  {
                     HCL_ERROR_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::InitAssetRecord: "
                                        << sensor_interface->GetSsysID() << ": Failed to initialize sensor scheduler";
                  }

                  // TurnOn was already called on defaultScheduler, which we're replacing. TurnOn must be called on the
                  // WsfSensorsManagerFovSensorScheduler in order to set up fields not handled by either the copy constructor
                  // or Initialize, such as mNextTrackVisitTime, which must be set to a large number in order to avoid a crash
                  // in the case that a TTR managed by an FOV sensors manager has been (incorrectly) manually turned on.
                  sensor.GetScheduler()->TurnOn(0.0);
                  // Trackers store a pointer to the scheduler, so they also have to be re-initialized
                  if (!sensor.GetTracker()->Initialize(0.0, &sensor, sensor.GetScheduler()))
                  {
                     HCL_ERROR_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::InitAssetRecord: "
                                        << sensor_interface->GetSsysID() << ": Failed to initialize sensor tracker";
                  }
               }

               if (sensor.GetAzSlewRate() >= WsfArticulatedPart::cINFINITE_SLEW_RATE)
               {
                  HCL_ERROR_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::InitAssetRecord: "
                                     << sensor_interface->GetSsysID() << ": sensor azimuth slew rate should be set to infinity";
               }

               if (sensor.GetAzCueRate() >= WsfArticulatedPart::cINFINITE_SLEW_RATE)
               {
                  HCL_ERROR_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::InitAssetRecord: "
                                     << sensor_interface->GetSsysID() << ": sensor azimuth cue rate should be set to infinity";
               }
            }

            // grab values defined in the sensor's aux data
            WsfAttributeContainer& sensorAuxData = sensor.GetAuxData();
            {
               const char* auxKey = "RESTING_AZIMUTH";
               const ut::Variant& var = sensorAuxData.GetVar(auxKey);
               switch (var.GetType())
               {
                  case ut::Variant::Type::cUNSET:
                     break;
                  case ut::Variant::Type::cDOUBLE:
                  {
                     double value = var.GetDouble();
                     if (value >= -UtMath::cPI && value <= UtMath::cPI)
                     {
                        sensor_interface->SetRestingAzimuth(value);
                     }
                     else
                     {
                        HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be in the range [-180, 180] degrees";
                     }
                     break;
                  }
                  default:
                     HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be a 'double' type";
                     break;
               }
            }
            {
               const char* auxKey = "COARSE_SLEW_RATE_AZIMUTH";
               const ut::Variant& var = sensorAuxData.GetVar(auxKey);
               switch (var.GetType())
               {
                  case ut::Variant::Type::cUNSET:
                     break;
                  case ut::Variant::Type::cDOUBLE:
                  {
                     double value = var.GetDouble();
                     if (value > 0.0)
                     {
                        sensor_interface->SetCoarseSlewRate(value);
                     }
                     else
                     {
                        HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be positive";
                     }
                     break;
                  }
                  default:
                     HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be a 'double' type";
                     break;
               }
            }
            {
               const char* auxKey = "FINE_SLEW_RATE_AZIMUTH";
               const ut::Variant& var = sensorAuxData.GetVar(auxKey);
               switch (var.GetType())
               {
                  case ut::Variant::Type::cUNSET:
                     break;
                  case ut::Variant::Type::cDOUBLE:
                  {
                     double value = var.GetDouble();
                     if (value > 0.0)
                     {
                        sensor_interface->SetFineSlewRate(value);
                     }
                     else
                     {
                        HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be positive";
                     }
                     break;
                  }
                  default:
                     HCL_ERROR_LOGGER(mGlobalLogger) << sensor_interface->GetSsysID() << ": " << auxKey << " aux value must be a 'double' type";
                     break;
               }
            }

            sensor_interface->Set(plat->GetSimulation(), asset_record, sidx, il::C2_OTHER);
            auto max_request_count = sensor.GetMaximumRequestCount();
            if (!max_request_count && sensor_interface->IsTTR())
            {
               HCL_ERROR_LOGGER(mGlobalLogger) <<
                                  "Error initializing TTR: " << sensor_interface->GetName() <<
                                  ". Maximum request count == 0. will not be able to cue radar.";
            }
            else if (max_request_count > 0 && (sensor_interface->IsTAR() || sensor_interface->IsEW()))
            {
               HCL_WARN_LOGGER(mGlobalLogger) << "Error initializing " << (sensor_interface->IsTAR() ? "TAR" : "EW") << " "
                                 << sensor_interface->GetName() << ". Sensor has maximum request counts of " << max_request_count
                                 << ". The Sensor manager expects sensors of these categories to not have tracking modes.";
            }

            // we can't know what specific modes might be invoked at this point, maybe it
            // needs to be implemented in terms of truth interface on-demand?
            sensor_interface->SetMaxActiveCues(max_request_count);
            asset_record->AddSensor(sensor_interface);
         }
      }

      // Detect other C2 processors
      for (unsigned procidx = 0; procidx < plat->GetComponentCount<WsfProcessor>(); ++procidx)
      {
         const auto proc_entry = plat->GetComponentEntry<WsfProcessor>(procidx);
         if (proc_entry->IsA_TypeOf(WsfScriptBattleManagerClass::BASE_CLASS_NAME))
         {
            asset_record->setDefaultDefendedPriority(1.0);
            asset_record->setHasBattleManager(true);
            HCL_DEBUG_LOGGER(mGlobalLogger) << "Found a battle manager on platform: " << plat->GetName() << ".";
         }
         else if (proc_entry->IsA_TypeOf(WsfScriptAssetManagerClass::BASE_CLASS_NAME))
         {
            // Handle the case where there is a task processor employed on an asset that we may need to talk to
            // We have to do some behind-the scenes rejiggering to make sure this works so mark it independently with
            // some aux data as well

            asset_record->setC2Capable(true);
            asset_record->setHasAssetManager(true);
            // This is not one of our battle managers so don't treat it as a defended asset since technically it
            // isn't although you could mark it as one if you desired.
            HCL_DEBUG_LOGGER(mGlobalLogger) << "Found a asset manager on platform: " << plat->GetName() << ". Setting C2 capable flag" << '\n';
         }
         else if (proc_entry->IsA_TypeOf(WsfScriptSensorsManagerClass::BASE_CLASS_NAME))
         {
            asset_record->setHasSensorManager(true);
         }
         else if (proc_entry->IsA_TypeOf(WsfScriptWeaponsManagerClass::BASE_CLASS_NAME))
         {
            asset_record->setHasWeaponManager(true);
         }
         else if (proc_entry->IsA_TypeOf(WsfScriptDisseminateC2Class::BASE_CLASS_NAME))
         {
            asset_record->setHasC2DisseminationManager(true);
         }
      }

      // Age out times - this is really getting passed on by the battle managers asset management scripting
      // but may allow for per-asset-type scripting in the future
      asset_record->setAgeYellow(am->GetCoreAMRef()->getAssetYellowAgeout());
      asset_record->setAgeRed(am->GetCoreAMRef()->getAssetRedAgeout());

      // Set min and max range for asset. These are used for the Collision_Intercept_In_Zone Engagement Assessment of the WMAI.
      if (plat->GetAuxData().AttributeExists(WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME))
      {
         asset_record->SetMinRange(plat->GetAuxData().GetDouble(WSF_IADS_C2_MIN_RANGE_AUX_DATA_NAME));
      }
      if (plat->GetAuxData().AttributeExists(WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME))
      {
         asset_record->SetMaxRange(plat->GetAuxData().GetDouble(WSF_IADS_C2_MAX_RANGE_AUX_DATA_NAME));
      }
   } // new record

   HCL_TRACE_LOGGER(mGlobalLogger) << "In Asset Map Add...\n";

   // do some quick sanity checks of the asset
   if (!asset_record->getC2Capable())
   {
      HCL_WARN_LOGGER(mGlobalLogger) <<
                        "Initializing platform " << asset_record->getName() <<
                        " within IADS chain of command but platform is not C2-Capable. Did you forget to employ an asset manager?\n";
   }

   asset_map.AddAsset(asset_record->getID(), asset_record);

   return asset_record;
}

void WsfDefaultAssetManagerImpl::InitializeAssetDAZones(const WsfPlatform* plat, std::shared_ptr<il::assetRecord>& asset_record) const
{
   const std::string da_zones_aux_name = "da_zones";

   if (WsfBMUtils::AuxDataExists(plat, da_zones_aux_name))
   {
      WsfBMAuxDataZoneAttachment da_zones;
      plat->GetAuxData().GetAttribute(da_zones_aux_name).Get(da_zones);
      for (std::size_t zone_idx = 0; zone_idx < da_zones.GetZoneAttachmentCount(); ++zone_idx)
      {
         // get each zone and pass it on to the asset initialization
         WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
         double zone_priority;
         std::string zone_name;
         il::eZoneType zone_type;
         da_zones.GetNthZoneAttachment(zone_idx, attach_type, zone_priority, zone_name, zone_type);
         bool is_scope_global = attach_type == WsfBMAuxDataZoneAttachment::E_GLOBAL;

         WsfZone* zone = nullptr;
         if (is_scope_global)
         {
            zone = (WsfZone*)plat->GetScenario().FindType("zone", zone_name);
         }
         else
         {
            zone = plat->GetComponent<WsfZone>(zone_name);
         }

         if (!zone)
         {
            HCL_ERROR_LOGGER(mGlobalLogger) << "InitializeAssetDAZones(): Attempt to locate zone: " << zone_name << ". global? " << std::boolalpha << is_scope_global << std::endl;
         }
         else
         {
            std::shared_ptr<WsfBMZoneRecord> zone_ref(new WsfBMZoneRecord(il::E_ZONE_TYPE_DA, il::zoneTypeToName(il::E_ZONE_TYPE_DA, mGlobalLogger), zone_priority, reinterpret_cast<WsfZoneDefinition*>(zone), plat->GetSimulation()));
            asset_record->AddZone(zone_ref);
         }
      }

      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform: " << plat->GetName() << " has " << da_zones.GetZoneAttachmentCount() << " DA zones" << std::endl;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform: " << plat->GetName() << " has no DA zones" << std::endl;
   }
}

void WsfDefaultAssetManagerImpl::InitializeAssetZones(const WsfPlatform* plat, std::shared_ptr<il::assetRecord>& asset_record) const
{
   const std::string zones_aux_name = "iads_c2_zones";

   if (WsfBMUtils::AuxDataExists(plat, zones_aux_name))
   {
      WsfBMAuxDataZoneAttachment iads_c2_zones;
      plat->GetAuxData().GetAttribute(zones_aux_name).Get(iads_c2_zones);
      for (std::size_t zone_idx = 0; zone_idx < iads_c2_zones.GetZoneAttachmentCount(); ++zone_idx)
      {
         // get each zone and pass it on to the asset initialization
         WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
         double zone_priority;
         std::string zone_name;
         il::eZoneType zone_type;
         iads_c2_zones.GetNthZoneAttachment(zone_idx, attach_type, zone_priority, zone_name, zone_type);
         bool is_scope_global = attach_type == WsfBMAuxDataZoneAttachment::E_GLOBAL;

         WsfZone* zone = nullptr;
         if (is_scope_global)
         {
            zone = (WsfZone*)plat->GetScenario().FindType("zone", zone_name);
         }
         else
         {
            zone = plat->GetComponent<WsfZone>(zone_name);
         }

         if (!zone)
         {
            HCL_ERROR_LOGGER(mGlobalLogger) << "InitializeAssetZones(): Attempt to locate zone: " << zone_name << ". global? " << std::boolalpha << is_scope_global << std::endl;
         }
         else
         {
            std::shared_ptr<WsfBMZoneRecord> zone_ref(new WsfBMZoneRecord(zone_type, il::zoneTypeToName(zone_type, mGlobalLogger), zone_priority, reinterpret_cast<WsfZoneDefinition*>(zone), plat->GetSimulation()));
            asset_record->AddZone(zone_ref);
         }
      }

      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform: " << plat->GetName() << " has " << iads_c2_zones.GetZoneAttachmentCount() << " zones." << std::endl;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform: " << plat->GetName() << " has no attached zones." << std::endl;
   }
}

void WsfDefaultAssetManagerImpl::InitializeWeaponZones(const WsfPlatform* owning_plat, WsfWeapon* weapon, WsfBMWeaponInfoRecord* weapon_interface)
{
   if (WsfBMUtils::AuxDataExists(weapon, "weapon_zones"))
   {
      std::unique_ptr<WsfBMAuxDataZoneAttachment> weapon_zones(new WsfBMAuxDataZoneAttachment());
      try
      {
         weapon->GetAuxData().Get("weapon_zones", *weapon_zones);
      }
      catch (UtException& except)
      {
         HCL_FATAL_LOGGER(mGlobalLogger) << "InitializeWeaponZones() could not retrieve weapon zones: " << except.what() << std::endl;
      }

      // has zones attached to unit -- in this model, DA zones
      for (unsigned zone_idx = 0; weapon_zones && zone_idx < weapon_zones->GetZoneAttachmentCount(); ++zone_idx)
      {
         // get each zone and pass it on to the asset initialization
         WsfBMAuxDataZoneAttachment::eAttachmentType attach_type;
         double zone_priority;
         std::string zone_name;
         il::eZoneType zone_type;
         weapon_zones->GetNthZoneAttachment(zone_idx, attach_type, zone_priority, zone_name, zone_type);
         bool is_scope_global = attach_type == WsfBMAuxDataZoneAttachment::E_GLOBAL;

         WsfZone* zone;
         if (is_scope_global)
         {
            zone = (WsfZone*)weapon->GetScenario().FindType("zone", zone_name);
         }
         else
         {
            zone = owning_plat->GetComponent<WsfZone>(zone_name);
         }

         if (!zone)
         {
            HCL_ERROR_LOGGER(mGlobalLogger) << "InitializeWeaponZones(): Attempt to locate zone: " << zone_name << ". global? " << is_scope_global << " failed..." << std::endl;
         }
         else
         {
            std::shared_ptr<WsfBMZoneRecord> zone_ref(new WsfBMZoneRecord(zone_type, il::zoneTypeToName(zone_type, mGlobalLogger), zone_priority, reinterpret_cast<WsfZoneDefinition*>(zone), owning_plat->GetSimulation()));
            weapon_interface->AddZone(zone_ref);
         }
      }

      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform/weapon: " << owning_plat->GetName() << "/" << weapon->GetName() << " has " << weapon_zones->GetZoneAttachmentCount() << " weapon zones" << std::endl;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "platform/weapon: " << owning_plat->GetName() << "/" << weapon->GetName() << " has no weapon zones" << std::endl;
   }
}

bool WsfDefaultAssetManagerImpl::IsAssetPerceptionValid(WsfSimulation* sim, const WsfBMStatusMessage& status) const
{
   return status.GetPlatform(sim) != nullptr;
}

void WsfDefaultAssetManagerImpl::HandleAssetStatusUpdate(WsfAssetManager* am, const WsfBMStatusMessage& status)
{
   auto reference_plat = status.GetPlatform(am->GetSimulation());

   if (!reference_plat)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Got asset status from " << status.GetOriginator() << ", but unable to resolve referenced platform.\n";
      return;
   }

   HCL_INFO_LOGGER(mGlobalLogger) << "Got asset status for asset " << reference_plat->GetName() << std::endl;

   auto asset_map = am->GetAssetMapPtr();
   bool print_new_statuses = false;
   if (!asset_map->Exists(WsfBMUtils::GetUnitIDRecordFromPlatform(reference_plat)))
   {
      HCL_INFO_LOGGER(mGlobalLogger) << "Received asset is not initialized within original IADS percepts -- adding\n";
      // Get the associated platform
      auto new_plat = reference_plat;
      auto new_asset = InitAssetRecord(new_plat, *asset_map->GetRawAssetsPtr(), am); // TODO Ensure late additions to asset map are correctly handled in single asset and multiple asset cases
      EstablishNewCmdrSubRelationship(new_plat, new_asset, *asset_map->GetRawAssetsPtr());
      print_new_statuses = true;
   }
   // We need to determine if this platform has already been initialized or not, if not, add the asset.
   // this allows us to handle late-added friendlies to the simulation.
   am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::combatStatusMessage>(status.GetRawMessagePtr()));

   if (print_new_statuses)
   {
      PrintStatus(am);
   }
}

std::size_t WsfDefaultAssetManagerImpl::UpdateThreatPerceptions(const double sim_time_s, const WsfPlatform* this_plat, WsfAssetManager* am)
{
   std::size_t num_updates = 0;

   std::vector<WsfLocalTrack*> threats;
   WsfBMUtils::GetTracksUpdated(am, sim_time_s, threats);

   for (const auto& track : threats)
   {
      if (UpdateTrack(sim_time_s, track, am))
      {
         num_updates++;
      }
      else if (this->m_threat_check_believed_alive && !WsfBMUtils::BelievedAlive(am->GetSimulation(), track))
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::UpdateThreatPerceptions(). Detected track that we don't believe is alive. Dropping internally.";
         // drop the track internally since we don't trust it's state
         am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::airTargetMessage>(ConvertTrackDrop(am->GetSimulation(), track).GetRawMessagePtr()));
      }
   }

   return num_updates;
}

bool WsfDefaultAssetManagerImpl::UpdateTrack(const double sim_time_s, const WsfLocalTrack* track, WsfAssetManager* am)
{
   bool updated = false;

   bool believedAlive = WsfBMUtils::BelievedAlive(am->GetSimulation(), track);
   if (!m_threat_check_believed_alive || believedAlive)
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "UpdateTrack(): updated track " << track->GetTrackId() << ": believed alive = " << believedAlive << std::endl;
      am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::airTargetMessage>(ConvertTrackUpdate(sim_time_s, am, track).GetRawMessagePtr()));
      updated = true;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "UpdateTrack(): not updating track because it is believed to be dead.";
   }

   return updated;
}

WsfBMTrackMessage WsfDefaultAssetManagerImpl::ConvertTrackUpdate(const double sim_time_s, WsfAssetManager* am, const WsfLocalTrack* track) const
{
   WsfSimulation* sim = am->GetSimulation();
   HCL_INFO_LOGGER(mGlobalLogger) << "Got track for target " << track->GetTargetName() << std::endl;

   // convert to a BM track
   WsfBMTrackMessage bm_track_message = WsfBMUtils::ConvertTrack(sim, track, true);

   auto trackSensorType = bm_track_message.GetReportingSensorType();

   auto oldTrack = am->GetCoreAMRef()->getMasterTrack(WsfBMUtils::ConvertTrackId(track->GetTrackId()));

   if (oldTrack && trackSensorType < il::getSensorTypeEnumFromString(oldTrack->getReportingSensorType()))
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "Track " << track->GetTrackId() << " reporting sensor type reduced to " << il::getSensorTypeStringFromEnum(trackSensorType, mGlobalLogger);
   }

   return bm_track_message;
}

WsfBMTrackMessage WsfDefaultAssetManagerImpl::ConvertTrackDrop(const WsfSimulation* sim, const WsfLocalTrack* drop_track) const
{
   // convert to a BM track
   WsfBMTrackMessage bm_track_message(const_cast<WsfSimulation*>(sim));
   auto bm_track = bm_track_message.GetRawMessagePtr();
   bm_track_message.SetTrackID(drop_track->GetTrackId());
   bm_track_message.SetTrackingStatus(WsfBMTrackMessage::STATUS_DROPPED);

   return bm_track_message;
}

void WsfDefaultAssetManagerImpl::HandleAssignmentStatusUpdate(const double sim_time_s, const WsfBMAssignmentStatusMessage& message, WsfAssetManager* am)
{
   std::unique_ptr<WsfBMAssignmentStatusMessage> msg_copy(message.Clone());

   // submit the assignment status to the battle manager
   am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::assignmentACKMessage>(msg_copy->GetRawMessagePtr()));
}

void WsfDefaultAssetManagerImpl::HandleAssignmentMessage(const double aSimTime, const WsfBMAssignmentMessage& message, WsfAssetManager* am)
{
   HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::HandleAssignmentMessage(): Handling incoming assignment...\n";

   if (message.GetRawMessagePtr()->getAssignmentReason() != il::assignmentMessage::CANCEL &&
       am->GetCoreAMRef()->getNumAssignments() >= am->GetCoreAMRef()->getMaxAssignments())
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\t received assignment on " << am->GetName() << " but has NO Ready Assignments\n";
      am->GetCoreAMRef()->cantcoAssignment(*IADSC2SimulationExtension::Find(*am->GetSimulation())->GetLogger(), std::const_pointer_cast<il::assignmentMessage>(message.GetRawMessagePtr()), "NO Ready Assignments");
   }

   // first, we need to get a master track associated with this assignment, if no track is associated, we need to submit it to the track processor and then potentially wait it out
   std::unique_ptr<WsfBMAssignmentMessage> msg_copy(static_cast<WsfBMAssignmentMessage*>(message.Clone()));

   // TTD: have potential to override track manager by specifying a name to the asset manager

   if (CorrelateMasterTrackWithAssignment(*msg_copy, am))
   {
      // this is basically GTIQDAssetManagerProcessTrack with a twist...
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tAssignment is ready, proceeding with delegation processing.\n";
      AddDelegation(aSimTime, *msg_copy, am);
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tDid not get an immediate correlation of assignment track: " << msg_copy->GetAssignmentLocalTrackID() << " to a master track. Adding to uncorrelated assignment queue.\n";
      m_uncorrelated_assignments.emplace_back(aSimTime, message);
   }
}

void WsfDefaultAssetManagerImpl::HandleSensorCueMessage(const double aSimTime, WsfAssetManager* am, const WsfBMCueMessage& message)
{
   // note: cues don't count against the active assignment count since they are not assignments by or in itself.
   // HELIOS handles cues kind of weirdly, they go into the assignment array as weird non-assignment assignments (cue assignments)
   // which explodes the complexity of assignment handling.
   // we will treat it differently according to the following:
   //
   // sensor manager units will create cues and maintain them within the responsible assignment array.
   // cue messages go out to the cued units
   // upon receipt of a cue message:
   //    a) the referenced master track will be resolved
   //       no processing occurs until the master track can be resolved referencing the cue (same for assignments)
   //    b) if the cue is not destined for this platform, it is simply added to the queued messages for the DC2 processor
   //    c) if the cue is destined to this platform, it goes into the active cue array for further processing by the asset manager
   //

   HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::HandleSensorCueMessage(): Handling incoming sensor cue...\n";

   // first, we need to get a master track associated with this assignment, if no track is associated, we need to submit it to the track processor and then potentially wait it out
   std::unique_ptr<WsfBMCueMessage> msg_copy(static_cast<WsfBMCueMessage*>(message.Clone()));

   // TTD: have potential to override track manager by specifying a name to the asset manager
   if (CorrelateMasterTrackWithCue(*msg_copy, am))
   {
      am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::cueMessage>(message.GetRawMessagePtr()));;
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tDid not get an immediate correlation of cue track: " << msg_copy->GetLocalTrackID() << " to a master track. Adding to uncorrelated cues queue.\n";
      m_uncorrelated_cues.emplace_back(aSimTime, message);
   }
}

void WsfDefaultAssetManagerImpl::HandleAssignmentTrackMessage(const double aSimTime, WsfAssetManager* am, const WsfBMAssignmentTrackMessage& message)
{
   HCL_DEBUG_LOGGER(mGlobalLogger) << aSimTime << ": WsfDefaultAssetManagerImpl::HandleAssignmentTrackMessage(): Handling incoming assignment track...\n";

   std::unique_ptr<WsfBMAssignmentTrackMessage> msg_copy(static_cast<WsfBMAssignmentTrackMessage*>(message.Clone()));

   // note: we have experimented with adding additional logic here as to whether to accept the assignment track blindly or not.
   // Most efforts to see if we have a master track yet or not have failed due to the way we're recommending to set up the ASV
   // network to only flow up. This means that when track quality shifts occur, it is very hard to check for without spiraling into
   // a massive chunk of checks. We have found that a little bit of feedback loop is ok and that the most bang for the buck is
   // if the user has set to not engage believed dead tracks, we internally age these out of the iadsLib models until updated
   // by the glue layer.

   // add the report
   am->GetPlatform()->GetTrackManager().AddTrackReport(aSimTime, msg_copy->GetTrack());
}

bool WsfDefaultAssetManagerImpl::IsSelfAssignment(const WsfBMAssignmentMessage& message, WsfAssetManager* am) const
{
   return WsfBMUtils::IsAssignedBy(am->GetPlatform(), message);
}

bool WsfDefaultAssetManagerImpl::IsSelfCue(const WsfBMCueMessage& message, WsfAssetManager* am) const
{
   auto plat = message.GetInitiatingPlatform(am->GetSimulation());
   if (plat)
   {
      return plat->GetIndex() == am->GetPlatform()->GetIndex();
   }
   return false;
}

bool WsfDefaultAssetManagerImpl::CorrelateMasterTrackWithAssignment(WsfBMAssignmentMessage& message, WsfAssetManager* am)
{
   bool assignment_ready = false;

   if (!IsSelfAssignment(message, am))
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tAssignment was NOT self assignment. Running assignment track correlation logic.\n";

      WsfTrackManager track_manager = am->GetPlatform()->GetTrackManager();
      WsfTrackId track_id = message.GetAssignmentLocalTrackID();
      WsfLocalTrack* master_track = WsfBMUtils::FindCorrelatedTrack(&track_manager, &track_id); // at this point local track ID is the local track of the sender (a remote track for us)
      if (!master_track) // perhaps it is a remote track?
      {
         master_track = am->GetPlatform()->GetTrackManager().FindTrack(message.GetAssignmentLocalTrackID());
         if (master_track)
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << "\tDidn't find correlated track but found track via remote association.\n";
         }
      }

      if (master_track)
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::CorrelateMasterTrackWithAssignment(): correlated assignment track "
                            << message.GetAssignmentLocalTrackID() << " with master track " << master_track->GetTrackId() << std::endl;
         message.SetAssignmentLocalTrackID(master_track->GetTrackId());

         assignment_ready = true;
      }
      else
      {
         // we must search through the assignments and locate it based on the reference track id because this could be a situation where we are getting an assignment cancel due to
         // the loss of a track (that we've already lost as well)
         auto assignments = am->GetCoreAMRef()->getAssignmentsOnReferenceTrack(*IADSC2SimulationExtension::Find(*am->GetSimulation())->GetLogger(),
            WsfBMUtils::ConvertTrackId(message.GetAssignmentReferenceTrackID()),
            WsfBMUtils::GetAssignedIDRecord(message.GetAssignedPlatform(am->GetSimulation()),
               message.GetAssignedWeapon(am->GetSimulation())));
         if (assignments.size() > 1)
         {
            HCL_WARN_LOGGER(mGlobalLogger) << "\tWsfDefaultAssetManagerImpl::CorrelateMasterTrackWithAssignment(): Retreived " << assignments.size() << " assignment possibilities, picking the first.";
         }
         if (!assignments.empty())
         {
            auto local_track_id = assignments[0].Assignment()->getLocalTrackID();
            message.SetAssignmentLocalTrackID(WsfBMUtils::ConvertTrackId(local_track_id));
            HCL_DEBUG_LOGGER(mGlobalLogger) << "\tWsfDefaultAssetManagerImpl::CorrelateMasterTrackWithAssignment(): found correlated assignment track by searching through reference assignment tracks "
                               << message.GetAssignmentLocalTrackID() << " with master track " << local_track_id << std::endl;
         }

       // regardless of correlation state, don't mark it for queuing since if something goes wrong, we want to make a best-effort to get the cancellation where it needs to go
       if (message.GetAssignmentReason() == il::assignmentMessage::CANCEL)
         {
            assignment_ready = true;
         }
      }
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tAssignment was self assignment, no need for assignment track correlation\n";
      assignment_ready = true; // track reference is already master track reference, nothing to correlate
   }

   return assignment_ready;
}

// Note: this may appear confusing because we are taking in an assignment and need to re-orient the local track associated with it.
// This means that initially the LocalTrack is actually a remote track of the sender until we get a correlation
bool WsfDefaultAssetManagerImpl::CorrelateMasterTrackWithCue(WsfBMCueMessage& message, WsfAssetManager* am)
{
   bool cue_ready = false;

   if (!IsSelfCue(message, am))
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tCUE was NOT self CUE. Running assignment track correlation logic.\n";

      WsfTrackManager track_manager = am->GetPlatform()->GetTrackManager();
      WsfTrackId track_id = message.GetLocalTrackID();
      WsfLocalTrack* master_track = WsfBMUtils::FindCorrelatedTrack(&track_manager, &track_id); // at this pointlocal track ID is the local track of the sender (a remote track for us)
      if (!master_track) // perhaps it is a remote track?
      {
         master_track = am->GetPlatform()->GetTrackManager().FindTrack(message.GetLocalTrackID());
         if (master_track)
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << "\tDidn't find correlated track but found track via remote association.\n";
         }
      }

      if (master_track)
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << "WsfDefaultAssetManagerImpl::CorrelateMasterTrackWithCue(): correlated assignment track "
                            << message.GetLocalTrackID() << " with master track " << master_track->GetTrackId() << std::endl;
         message.SetLocalTrackID(master_track->GetTrackId());

         cue_ready = true;
      }
      else
      {
         if (message.GetCueReason() == il::cueMessage::CANCEL)
         {
            cue_ready = true;
         } // regardless of correlation state, don't mark it for queueing since if something goes wrong, we want to make a best-effort to get the cancellation where it needs to go
      }
   }
   else
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "\tCUE was self CUE, no need for assignment track correlation\n";
      cue_ready = true; // track reference is already master track reference, nothing to correlate
   }

   return cue_ready;
}

void WsfDefaultAssetManagerImpl::AddDelegation(const double sim_time_s, const WsfBMAssignmentMessage& message, WsfAssetManager* am)
{
   // this is an incoming new assignment, we must make sure our track perceptions are up to date first
   UpdateThreatPerceptions(sim_time_s, am->GetPlatform(), am);

   am->GetCoreAMRef()->processMessage(*IADSC2SimulationExtension::Find(*am->GetSimulation())->GetLogger(), std::const_pointer_cast<il::assignmentMessage>(message.GetRawMessagePtr()));
}

void WsfDefaultAssetManagerImpl::CheckPendingAssignmentCorrelations(const double sim_time_s, WsfSimulation* sim, WsfAssetManager* am)
{
   for (auto pending_iter = m_uncorrelated_assignments.begin(); pending_iter != m_uncorrelated_assignments.end();)
   {
      if (CorrelateMasterTrackWithAssignment(pending_iter->second, am))
      {
         AddDelegation(sim_time_s, pending_iter->second, am);
         pending_iter = m_uncorrelated_assignments.erase(pending_iter);
      }
      else
      {
         ++pending_iter;
      }
   }
}

void WsfDefaultAssetManagerImpl::CheckPendingCueCorrelations(const double sim_time_s, WsfSimulation* sim, WsfAssetManager* am)
{
   for (auto pending_iter = m_uncorrelated_cues.begin(); pending_iter != m_uncorrelated_cues.end();)
   {
      if (CorrelateMasterTrackWithCue(pending_iter->second, am))
      {
         am->GetCoreAMRef()->processMessage(std::const_pointer_cast<il::cueMessage>(pending_iter->second.GetRawMessagePtr()));
         pending_iter = m_uncorrelated_cues.erase(pending_iter);
      }
      else
      {
         ++pending_iter;
      }
   }
}

bool WsfDefaultAssetManagerImpl::ShouldDelayIncomingMessage(double aSimTime, const WsfMessage& aMessage, const WsfAssetManager* am, double& amount_s)
{
   bool should_delay = false;
   amount_s = -1;

   const bool is_assignment = aMessage.GetType() == WsfBMAssignmentMessage::GetTypeId();
   const auto bm_ref = am->GetCoreAMRef()->GetBattleManager();
   const auto has_commit_authority = bm_ref && bm_ref->getCommitAuthority();
   const double assignment_delay = am->GetCoreAMRef()->getAssignmentDelay();

   if (is_assignment && !has_commit_authority && NumericUtils::GreaterThan(assignment_delay, 0))
   {
      should_delay = true;
      amount_s = assignment_delay;
   }

   return should_delay;
}
