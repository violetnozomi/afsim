// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: assetManagerInterface.cpp
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
*   Abstract: Asset Management for interacting with the battle manager
*
*******************************************************************************/

#include <utility>

#include <logger.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/sensorsManagerInterface.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/NumericUtils.hpp>
#include <iadsLib/cueMessage.hpp>
#include <iadsLib/assignmentTrackMessage.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

namespace il
{

AssetManagerInterface::AssetManagerInterface(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : ReferencesBattleManager()
   , ReferencesSensorsManager()
   , ReferencesWeaponsManager()
   , mGlobalLogger(aGlobalLogger)
   , m_StartTime(0)
   , m_CurrentTime(0)
   , m_LastProcessTime(-std::numeric_limits<double>::max())
   , m_status_initialized(false)
   , m_asset_yellow_ageout_time(cDEFAULT_YELLOW_AGEOUT_TIME_S)
   , m_asset_red_ageout_time(cDEFAULT_RED_AGEOUT_TIME_S)
   , m_report_position_every_meters(cDEFAULT_REPORT_POSITION_EVERY_METERS)
   , m_report_position_every_seconds(cDEFAULT_REPORT_POSITION_EVERY_SECONDS)
   , m_report_status_every_seconds(cDEFAULT_REPORT_STATUS_EVERY_SECONDS)
   , m_aggregate_unit_status(cDEFAULT_AGGREGATE_UNIT_STATUS)
   , m_stationary_opns_only(cDEFAULT_STATUS_STATIONARY_OPNS_ONLY)
   , m_weapon_required(cDEFAULT_STATUS_WEAPON_REQUIRED)
   , m_ew_required(cDEFAULT_STATUS_EW_REQUIRED)
   , m_tar_required(cDEFAULT_STATUS_TAR_REQUIRED)
   , m_ttr_required(cDEFAULT_STATUS_TTR_REQUIRED)
   , m_require_all_weapons(false)
   , m_max_assignments(0)
   , m_assignment_delay_s(0)
   , m_decision_update_delay_s(0)
   , m_log_my_status(true)
{

}


// all shared pointers will be sliced here because they are associations
// versus aggregations.
AssetManagerInterface::AssetManagerInterface(const AssetManagerInterface& from)
   : ReferencesBattleManager(from)
   , ReferencesSensorsManager(from)
   , ReferencesWeaponsManager(from)
   , mGlobalLogger(from.mGlobalLogger)
   , m_Assets(from.m_Assets)
   , m_MasterTracks(from.m_MasterTracks)
   , m_DefendedAssets(from.m_DefendedAssets)
   , m_OutgoingMessages(from.m_OutgoingMessages)
   , m_AssignmentArray(from.m_AssignmentArray)
   , m_ExclusionArray(from.m_ExclusionArray)
   , m_ThisAssetID(from.m_ThisAssetID)
   , m_ThisAsset(from.m_ThisAsset)
   , m_StartTime(from.m_StartTime)
   , m_CurrentTime(from.m_CurrentTime)
   , m_LastProcessTime(from.m_LastProcessTime)
   , m_status_initialized(from.m_status_initialized)
   , m_asset_yellow_ageout_time(from.m_asset_yellow_ageout_time)
   , m_asset_red_ageout_time(from.m_asset_red_ageout_time)
   , m_report_position_every_meters(from.m_report_position_every_meters)
   , m_report_position_every_seconds(from.m_report_position_every_seconds)
   , m_report_status_every_seconds(from.m_report_status_every_seconds)
   , m_aggregate_unit_status(from.m_aggregate_unit_status)
   , m_stationary_opns_only(from.m_stationary_opns_only)
   , m_weapon_required(from.m_weapon_required)
   , m_ew_required(from.m_ew_required)
   , m_tar_required(from.m_tar_required)
   , m_ttr_required(from.m_ttr_required)
   , m_require_all_weapons(from.m_require_all_weapons)
   , m_max_assignments(from.m_max_assignments)
   , m_assignment_delay_s(from.m_assignment_delay_s)
   , m_decision_update_delay_s(from.m_decision_update_delay_s)
   , m_PendingDelegateAssignments(from.m_PendingDelegateAssignments)
   , m_log_my_status(from.m_log_my_status)
{
}

AssetManagerInterface* AssetManagerInterface::Clone()
{
   return new AssetManagerInterface(*this);
}


void AssetManagerInterface::processMessage(std::shared_ptr<airTargetMessage> atm)
{
   idRecord TrackID = atm->getTrackID();

   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Air target message updated: "
                      << " track " << TrackID;


   // Verify track id is valid, ignore otherwise.
   if (!TrackID.isValid())
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Invalid track number: " << TrackID << " not allowed!" << "\n";
      return;
   }

   switch (atm->getTrackingStatus())
   {
      case il::UPDATE_NORMAL:
      case il::UPDATE_COASTING:
      case il::UPDATE_DROPPING:
         updateMasterTrack(TrackID, atm);
         queueOutgoingMessage(E_UPDATED_TRACK, atm);
         break;

      case il::UPDATE_TIMED_OUT:
      case il::UPDATE_DROPPED:
         if (deleteMasterTrack(TrackID, atm))
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << "Dropped track: " << atm->getTargetTruthName();
            queueOutgoingMessage(E_DROPPED_TRACK, atm);
         }
         break;

      default:
         HCL_ERROR_LOGGER(mGlobalLogger) << "Received unknown status for track message: " << atm->getTrackingStatus();
         atm->logSTD();
   }
}

void AssetManagerInterface::processMessage(std::shared_ptr<plotMessage> pm)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Plot Message from: " << pm->getSenderID()
                      << " ignoring...";
}

void AssetManagerInterface::processMessage(std::shared_ptr<bearingReportMessage> brm)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Bearing Report Message from: " << brm->getSenderID()
                      << " ignoring...";
}

void AssetManagerInterface::processMessage(std::shared_ptr<combatStatusMessage> csm)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << std::setprecision(2) << (getCurrentTime())
                      << ", -----> Combat Status message for: " << csm->getAssetID();

   auto asset_record = m_Assets.GetAsset(csm->getAssetID());

   if (asset_record)
   {
      asset_record->processMessage(*csm, unitHasAssignments(csm->getAssetID(), false));

      queueOutgoingMessage(E_STATUS_RCVD, csm);
   }
   else
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "Received combat status message for unknown asset "
                        << csm->getAssetID() << " sent from asset " << csm->getSenderID() << "\n";
   }
}

void AssetManagerInterface::processMessage(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> am)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Assignment message from: " << am->getSenderID()
                      << " reference track " << am->getReferenceTrackID()
                      << ", local track " << am->getLocalTrackID();

   switch (am->getAssignmentReason())
   {
      case assignmentMessage::NEW_ASSIGNMENT:
      case assignmentMessage::RETRANSMIT:
      case assignmentMessage::UPDATE:
         handleCreateAssignmentMsgIn(aMOELogger, am);
         break;

      case assignmentMessage::REASSIGNMENT:
         HCL_ERROR_LOGGER(mGlobalLogger) << "assigment reassign directives are not supported!";
         break;

      case assignmentMessage::CANCEL:
         handleCancelAssignmentMsgIn(am);
         break;
   }
}

void AssetManagerInterface::processMessage(std::shared_ptr<assignmentACKMessage> ack)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Assignment Status message from: " << ack->getSenderID()
                      << " reference track " << ack->getAssignmentReferenceTrackID();

   // get related assignment
   evaluationTrack null_track(mGlobalLogger);
   evaluationAssignment eval_assignment(null_track, nullptr);
   if (!getEvaluationAssignment(*ack, eval_assignment))
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "processMessage(assignment status): received assignment status but could not resolve assignment";
      return;
   }

   auto assignment = eval_assignment.Assignment();

   switch (ack->getStatus())
   {
      case assignmentACKMessage::WILCO:
      case assignmentACKMessage::COMITTED:
      case assignmentACKMessage::CUE_TAR:
      case assignmentACKMessage::CUE_TTR:
      case assignmentACKMessage::TAR_SEARCHING:
      case assignmentACKMessage::TTR_SEARCHING:
      case assignmentACKMessage::TAR_TRACKING:
      case assignmentACKMessage::TTR_TRACKING:
      case assignmentACKMessage::FIRING:
      case assignmentACKMessage::MISS:
         // required: assignment status update
         assignment->setStatus(ack);
         break;
      case assignmentACKMessage::CHANGED_ASSIGNED_UNIT:
         // required: asset perception update
         //           assignment update
         //           assignment status update

         // add assignments back to the originally assigned unit
         m_ThisAsset->updateC2ChainAssignedUnitStatus(assignment->getAssignedID(), &m_Assets, assetRecord::INCREMENT, 1, GetIncrementMissileCount(assignment));
         // decrement assignments to the newly assigned unit -- based on the shot doctrine since this engagement presumably hasn't occurred yet
         m_ThisAsset->updateC2ChainAssignedUnitStatus(ack->getNewlyAssignedID(), &m_Assets, assetRecord::DECREMENT, 1, assignment->getRequestedMissilesToCommit());

         // update assignment
         assignment->setAssignedID(ack->getNewlyAssignedID());
         assignment->setAssigningID(ack->getOverridingID());
         // initiating unit id should never change
         // delete direct sub and assigned cmdr since this can be inferred from the IADS.
         // update assignment status
         assignment->setStatus(ack);
         break;

      case assignmentACKMessage::UPDATED_SHOT_DOCTRINE:
      {
         // determine the difference between the new and old doctrine
         ack->getUpdatedShotDoctrine();
         assignment->getShotDoctrine();
         auto missile_difference = il::DetermineShotDoctrineDifference(ack->getUpdatedShotDoctrine(), assignment->getShotDoctrine(), mGlobalLogger);  // new - old
         if (missile_difference != 0)
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << "Updated perceived munitions to match updated shot doctrine difference of " << missile_difference;
            m_ThisAsset->updateC2ChainAssignedUnitStatus(assignment->getAssignedID(), &m_Assets, assetRecord::DECREMENT, 0, missile_difference);
         }

         // update assignment status
         assignment->setStatus(ack);
      }
      break;

      case assignmentACKMessage::KILL:
      case assignmentACKMessage::CANCELLED:
      case assignmentACKMessage::HAVCO_SUCCESS:
      case assignmentACKMessage::HAVCO_FAILURE:
      case assignmentACKMessage::CANTCO:
         // required: asset perception update
         //           assignment status update
         if (!assignment->isAssignmentComplete())
         {
            m_ThisAsset->updateC2ChainAssignedUnitStatus(assignment->getAssignedID(), &m_Assets, assetRecord::INCREMENT, 1, GetIncrementMissileCount(assignment));
            assignment->setStatus(ack);
         }
         break;

      default:
         HCL_ERROR_LOGGER(mGlobalLogger) << "Received unknown assignment status: " << ack->getStatus();
         break;
   }


   // systemic cantco? update the exclusion arrays
   if (ack->isSystemicCantco())
   {
      m_ExclusionArray.addExclusion(ack);
   }

   // queue assignment status message
   if (ack->getInitiatingID() != getOwnID())
   {
      queueOutgoingMessage(E_ASSIG_ACK_RCVD, ack);
   }
}

void AssetManagerInterface::processMessage(std::shared_ptr<weaponsControlMessage> wcm)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Weapons Control Message from: " << wcm->getSenderID()
                      << " ignoring..." << "\n";
}

void AssetManagerInterface::processMessage(std::shared_ptr<cueMessage> cue)
{
   HCL_TRACE_LOGGER(mGlobalLogger) << "Time: " << std::fixed << (getCurrentTime())
                      << ", " << this->getThisAsset()->getName()
                      << " -----> Cue message from: " << cue->getSenderID()
                      << " reference track " << cue->getReferenceTrackID();

   // determine if this cue is to us, if so put it in the active cues, otherwise, forward it
   if (cue->getCuedUnitID() == getOwnID())
   {
      handleIncomingCue(cue);
   }
   else
   {
      queueOutgoingMessage(E_SENSOR_CUE, cue);
   }
}

void AssetManagerInterface::UpdateDefendedAssetsArray()
{
   m_DefendedAssets.clear();

   // collect all defended assets in our chain of command
   auto assets = m_Assets.GetAssets();
   for (auto it = assets.begin(); it != assets.end(); ++it)
   {
      const auto& asset = *it;
      auto defended_zones = asset->GetDefendedZones();
      m_DefendedAssets.insert(m_DefendedAssets.end(), defended_zones.begin(), defended_zones.end());
   }

   if (!m_DefendedAssets.size())
   {
      // create a point-defense zone
      std::shared_ptr<zoneRecordInterface> pt_defense(CreatePointDefenseZone(m_ThisAsset->getName(), m_ThisAsset->getPosition(), m_ThisAsset->getDefaultDefendedPriority(), mGlobalLogger));
      m_DefendedAssets.push_back(pt_defense);
   }
}

void AssetManagerInterface::PrepareTracksForEvaluation()
{
   if (!m_battle_manager)
   {
      return;
   }

   m_battle_manager->clearEvaluationTracks();

   if (m_battle_manager->getCommitAuthority())
   {
      // note: we check if the track is already assigned or not, this does not mean that a track can't get multiply assigned,
      // simply that a track will not be considered for evaluation twice.
      for (auto it = m_MasterTracks.begin(); it != m_MasterTracks.end(); ++it)
      {
         const auto& track = it->second;
         if (m_battle_manager->isTrackEngageable(track) && !isTrackAssigned(track->getID()))
         {
            m_battle_manager->addTrackForEvaluation(evaluationTrack(track, mGlobalLogger));
         }
      }
   }
   else
   {
      // push all firm, engagable, tracks that have delegated assignments to us not yet assigned
      for (auto it = m_PendingDelegateAssignments.begin(); it != m_PendingDelegateAssignments.end(); ++it)
      {
         const auto& delegate_assignment = *it;

         // get associated track
         auto track = FindTrack(delegate_assignment.first, m_MasterTracks);
         if (track && m_battle_manager->isTrackEngageable(track) && !isExistingAssignment(delegate_assignment.second))
         {
            m_battle_manager->addTrackForEvaluation(evaluationTrack(track, delegate_assignment.second, mGlobalLogger));
         }
      }
   }
}

void AssetManagerInterface::CreateAssignments(il::moeLoggingInterface& aMOELogger)
{
   il::evalAssignmentArray unevaled_delegate_assignments;
   if (!m_battle_manager)
   {
      // if we don't have a battle manager, we need to pass the assignments from the pending delegates
      // over to the real assignment array below.
      for (auto it = m_PendingDelegateAssignments.begin(); it != m_PendingDelegateAssignments.end(); ++it)
      {
         const auto& delegate_assignment = *it;
         auto assignment_track = getMasterTrack(delegate_assignment.second->getLocalTrackID());
         if (assignment_track)
         {
            unevaled_delegate_assignments.push_back(il::evaluationAssignment(il::evaluationTrack(assignment_track, std::make_shared<assignmentMessage>(*delegate_assignment.second), mGlobalLogger), delegate_assignment.second));
            m_ThisAsset->updateC2ChainAssignedUnitStatus(delegate_assignment.second->getAssignedID(), &m_Assets, assetRecord::DECREMENT, 1, delegate_assignment.second->getRequestedMissilesToCommit());
         }
         else
         {
            HCL_ERROR_LOGGER(mGlobalLogger) << "AssetManagerInterface::CreateAssignments() attempt to propagate a pending assignment for local track id: " << delegate_assignment.second->getLocalTrackID() << " but no master track\n";
         }
      }
   }


   auto const& assignment_eval_array = m_battle_manager ? m_battle_manager->getEvaluatedAssignmentsArray() : unevaled_delegate_assignments;

   for (auto it = assignment_eval_array.begin(); it != assignment_eval_array.end(); ++it)
   {
      const auto& evaled_assignment = *it;
      LogAssignmentCreation(aMOELogger, evaled_assignment);
      // promote assignment
      m_AssignmentArray.insert(std::make_pair(evaled_assignment.Track()->getID(), evaled_assignment));
      // note: the battle managers assignment algorithms are responsible for maintaining
      // asset assignment perception wrt newly created assignments -- don't adjust asset assignment or munition
      // perceptions
      if (evaled_assignment.IsOverridden())
      {
         queueOutgoingMessage(E_ASSIGN_OVERRIDE_NOTIF, generateAssignmentOverrideNotification(evaled_assignment));
      }

      if (evaled_assignment.IsShotDoctrineUpdated())
      {
         queueOutgoingMessage(E_ASSIGN_OVERRIDE_NOTIF, generateDoctrineOverrideNotification(evaled_assignment));
      }

      // queue new assignment
      if (evaled_assignment.Assignment()->getAssignedID().getID() != this->m_ThisAssetID.getID())
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << "\tAssignment destination: " << evaled_assignment.Assignment()->getAssignedID().getID() << " is not us: " << this->m_ThisAssetID.getID() << ". Queueing for dissemination.\n";
         queueOutgoingMessage(E_ASSIGN_MSG_OUT, evaled_assignment.Assignment());
      }
      else
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << "\tAssignment destination: " << evaled_assignment.Assignment()->getAssignedID().getID() << " is us.";
      }
   }

   // clear the temp evaluated assignments array since the assignments have been promoted
   m_battle_manager ? m_battle_manager->clearEvaluatedAssignmentsArray() : unevaled_delegate_assignments.clear(); // not necessary on unevaled_delegate_assignments but more for symmetry
}

void AssetManagerInterface::LogAssignmentCreation(il::moeLoggingInterface& aMOELogger, const evaluationAssignment& evaled_assignment)
{
   bool isDelegation = !(this->getThisAsset()->isDirectSubordinate(evaled_assignment.Assignment()->getAssignedID().getID()) || evaled_assignment.Assignment()->getAssignedID().getID() == this->m_ThisAssetID.getID());

   const std::string assignment_flag = isDelegation ? "DELEGATE" : "COMMANDED";

   aMOELogger.Assignment(m_CurrentTime, &evaled_assignment, assignment_flag);
}


void AssetManagerInterface::CreateSelfDefenseAssignment(const evaluationAssignment& assignment)
{
   m_AssignmentArray.insert(std::make_pair(assignment.Track()->getID(), assignment));   // E_SELF_DEFENSE_ASSIGNMENT_CREATED

   queueOutgoingMessage(E_SELF_DEFENSE_ASSIGNMENT_CREATED, assignment.Assignment());
}


void AssetManagerInterface::MonitorAssignments(il::moeLoggingInterface& aMOELogger)
{
   m_battle_manager->runMonitorAssignments(aMOELogger, getCurrentTime());

   // run internal checks as well

   // for all assignment tracks
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      // am I the assigning id? and is the assignment not complete
      if (assignment_itor->second->getAssigningID() == am_ptr->getMyAsset().getID() &&
          !assignment_itor->second->isAssignmentComplete())
      {
         // master track still exists?
         auto master_track = FindTrack(assigned_track_itor->first, am_ptr->GetMasterTracksArray());
         if (!master_track)
         {
            // mark assignment as complete - cancelled
            am_ptr->cancelAssignment(aMOELogger, assignment_itor->second, "No Master Track");
            HCL_DEBUG_LOGGER(mGlobalLogger) << "AssetManagerInterface::MonitorAssignments(): cancelling assignment because lost Master track: " << assigned_track_itor->first;
         }
         // assignment expired?
         else if (am_ptr->getCurrentTime() - assignment_itor->second->getAssignTime() > assignment_itor->second->getMaxFiringTime())
         {
            // mark assignment as complete - cancelled
            am_ptr->cancelAssignment(aMOELogger, assignment_itor->second, "Shot Expired");
            HCL_DEBUG_LOGGER(mGlobalLogger) << "AssetManagerInterface::MonitorAssignments(): cancelling assignment because shot expired. Current time: "
                               << am_ptr->getCurrentTime()
                               << ", assignment time = "
                               << assignment_itor->second->getAssignTime()
                               << ", max firing time = "
                               << assignment_itor->second->getMaxFiringTime();
         }
         // any chain of command not green?
         else if (assetRecord::isAnyChainOfCommandRed(am_ptr->getAssets(), assignment_itor->second->getAssigningID(), assignment_itor->second->getAssignedID().getID()))
         {
            // mark assignment as complete - cancelled
            am_ptr->cancelAssignment(aMOELogger, assignment_itor->second, "Command chain went red");
            HCL_DEBUG_LOGGER(mGlobalLogger) << "AssetManagerInterface::MonitorAssignments(): cancelling assignment because command chain red. Current time: "
                               << am_ptr->getCurrentTime()
                               << ", assigning id = "
                               << assignment_itor->second->getAssigningID()
                               << ", assigned id = "
                               << assignment_itor->second->getAssignedID().getID();
         }
      }

      assignment_itor++;
   },
   this);
}

// any assignment marked as complete - erase
// any messages that needed to be sent have already - the responsiblity of this
// unit is to simply clear them out since the context has since been lost
void AssetManagerInterface::CompleteAssignments(il::moeLoggingInterface& aMOELogger)
{
   // loop through each active assignment and ensure the assigned asset is still green, if it has turned red, cancel the assignment
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (assignment_itor->second->isAssignmentComplete())
      {
         if (assignment_itor->second->wasSuccess())
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << am_ptr->getThisAsset()->getName() << " - CompleteAssignments(): Assignment was complete and successful, erasing master track: " << assignment_itor->second->getLocalTrackID() << ", quality = " << assignment_itor->second.Track()->getReportingSensorType();
            m_MasterTracks.erase(assignment_itor->second->getLocalTrackID());
         }
         else
         {
            HCL_DEBUG_LOGGER(mGlobalLogger) << am_ptr->getThisAsset()->getName() << " - CompleteAssignments(): Assignment is complete but not successful, erasing assignment but keeping track. Track: " << assignment_itor->second->getLocalTrackID() << ", quality = " << assignment_itor->second.Track()->getReportingSensorType() << ", assigned id = " << assignment_itor->second->getAssignedID();
         }
         assignment_itor = am_ptr->getAssignmentArray().erase(assignment_itor); // set assignment_itor to element after the erased element
      }
      else
      {
         ++assignment_itor;
      }
   },
   this);


   // at this point, clear out any pending delegate assignments, they have already been handled
   m_PendingDelegateAssignments.clear();

}


void AssetManagerInterface::AssessOngoingAssignments(il::moeLoggingInterface& aMOELogger)
{
   // loop through each active assignment and ensure the assigned asset is still green, if it has turned red, cancel the assignment
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (!assignment_itor->second->isAssignmentComplete() &&
          am_ptr->getThisAsset()->isAnyChainOfCommandRed(am_ptr->getAssets(), am_ptr->getOwnID(), assignment_itor->second->getAssignedID().getID()))
      {
         // assignment ongoing and we've had a change in the status of an intermediate subordinate, cancel it
         am_ptr->cancelAssignment(aMOELogger, assignment_itor->second, "Subordinate status no longer green");
      }
      ++assignment_itor;
   },
   this);

}

bool AssetManagerInterface::SendSensorCueMessage(const double sim_time, const tTrackID& master_track_id, std::shared_ptr<sensorRecord> sensor, const cueMessage::CueReason& reason)
{
   std::shared_ptr<il::cueMessage> cue(new il::cueMessage(mGlobalLogger));
   cue->create(sim_time, reason, master_track_id, getThisAsset()->getID(), sensor->GetParentAssetID(), sensor->GetSensorID());
   queueOutgoingMessage(AssetManagerInterface::E_SENSOR_CUE, cue);

   return true;
}


void AssetManagerInterface::queueOutgoingMessage(const eMessageAction action, std::shared_ptr<baseMessage> message)
{
   // HCL_TRACE_LOGGER(mGlobalLogger) << this->getCurrentTime() << ": " << this->getThisAsset()->getName() << ": Queueing message for dispatch: " << MsgActionToStr (action) << std::endl;

   m_OutgoingMessages.push_back(std::make_pair(action, std::shared_ptr<il::baseMessage>(message->clone())));
}

void AssetManagerInterface::queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::combatStatusMessage> message)
{
   // some caveats: we only want the latest statuses, and don't want to bother with stale status messages if our processor update rate is
   // way slower than the sender's status send rate
   auto unique_status = [&action, &message](std::pair<eMessageAction, std::shared_ptr<baseMessage>> item) -> bool
   {
      auto status_msg = dynamic_cast<il::combatStatusMessage*>(item.second.get());
      return item.first == action &&
      status_msg->getAssetID() == message->getAssetID();

   };


   auto iter = std::find_if(std::begin(m_OutgoingMessages), std::end(m_OutgoingMessages), unique_status);
   if (iter != std::end(m_OutgoingMessages))
   {
      // update
      auto to_update = dynamic_cast<il::combatStatusMessage*>(iter->second.get());
      if (NumericUtils::LessThan(to_update->getPositionTime(), message->getPositionTime()) ||
          NumericUtils::LessThan(to_update->getStatusTime(), message->getStatusTime()))
      {
         iter->second = message;
      }
      else
         HCL_INFO_LOGGER(mGlobalLogger) << "Not updating status for: " << to_update->getAssetID() << ". Stale update time: "
                           << to_update->getPositionTime() << "/" << to_update->getStatusTime() << " vs. "
                           << message->getPositionTime() << "/" << message->getStatusTime() << std::endl;
   }
   else
   {
      m_OutgoingMessages.push_back(std::make_pair(action, std::shared_ptr<il::baseMessage>(message->clone())));
   }
}

void AssetManagerInterface::queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::assignmentMessage> message)
{
   // if we have commit authority and an assignment delay, we delay on the send side of the house to simulate the thought side of the house
   // in generating the assignments. If we don't have commit authority, we delay on the input side of the house. This replicates HELIOS
   // functionality in that both sides attempt to model a delay when an assignment gets updated (commit authority when created, delegate when processed).
   auto bm_ref = this->GetBattleManager();
   bool has_assignment_delay = bm_ref && bm_ref->getCommitAuthority() && NumericUtils::GreaterThan(getAssignmentDelay(), 0.0);
   double latent_assignment_time = getCurrentTime() + getAssignmentDelay();

   // for every assignment we send out, we need to ensure we send out an assignment track as well
   if (message->getAssignmentReason() != il::assignmentMessage::CANCEL &&
       message->getAssignedID().getID() != getOwnID().getID())
   {
      const auto master_track_record = getMasterTrack(message->getLocalTrackID());
      if (master_track_record)
      {
         std::shared_ptr<il::assignmentTrackMessage> assignment_track(new il::assignmentTrackMessage(mGlobalLogger));
         assignment_track->setAssignedUnitID(message->getAssignedID());
         assignment_track->setTrack(*master_track_record);
         // if we have an assignment update delay time of nonzero and we have commit authority, add a latent process time
         if (has_assignment_delay)
         {
            assignment_track->setLatentProcessTime(latent_assignment_time);
         }

         queueOutgoingMessage(E_ASSIGNMENT_TRACK_UPDATE, assignment_track);
      }
      else
      {
         HCL_WARN_LOGGER(mGlobalLogger) << "Could not locate master track record: " << message->getLocalTrackID() << " for assignment. Cannot send Assignment Track Update";
      }
   }

   // queue the assignment
   auto assignment_copy = message->clone();
   if (has_assignment_delay)
   {
      assignment_copy->setLatentProcessTime(latent_assignment_time);
   }
   m_OutgoingMessages.push_back(std::make_pair(action, std::shared_ptr<il::baseMessage>(assignment_copy)));

}


void AssetManagerInterface::queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::assignmentACKMessage> message)
{
   evaluationTrack null_track(mGlobalLogger);
   evaluationAssignment eval_assignment(null_track, nullptr);
   if (!getEvaluationAssignment(*message, eval_assignment))
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "queueOutgoingMessage(assignment status): could not resolve assignment";
      return;
   }

   // create a clone for sending but also in case we need to update the ACK due to this being a delegation
   auto message_clone = message->clone();

   // if this was a delegation, update the status we send out to reflect the next level assigners
   // reference track
   auto delegate_assignment = eval_assignment.DelegateAssignment();
   if ((delegate_assignment) && (message_clone->getMessageType() == ASSIGNMENT_ACK_MESSAGE))
   {
      dynamic_cast<assignmentACKMessage*>(message_clone)->setAssignmentReferenceTrackID(delegate_assignment->getReferenceTrackID());
   }

   // queue the assignment status
   m_OutgoingMessages.push_back(std::make_pair(action, std::shared_ptr<il::baseMessage>(message_clone)));
}

std::string AssetManagerInterface::MsgActionToStr(const eMessageAction action)
{
   std::string actionstr = "***ERRROR*** Unknown ACTION!!!!";
   switch (action)
   {
      case AssetManagerInterface::E_UPDATED_TRACK:
         actionstr = "Updated Track";
         break;
      case AssetManagerInterface::E_DROPPED_TRACK:
         actionstr = "Dropped Track";
         break;
      case AssetManagerInterface::E_STATUS_RCVD:
         actionstr = "Status Received";
         break;
      case AssetManagerInterface::E_REJECTED_ASSIG_RESPONSE:
         actionstr = "Rejected Assignment Response";
         break;
      case AssetManagerInterface::E_CANCEL_ASSIG_RECVD:
         actionstr = "Cancel Received Assignment";
         break;
      case AssetManagerInterface::E_ASSIG_ACK_RCVD:
         actionstr = "Assignment Status Received";
         break;
      case AssetManagerInterface::E_ASSIGN_OVERRIDE_NOTIF:
         actionstr = "Assignment Override Notification";
         break;
      case AssetManagerInterface::E_ASSIGN_MSG_OUT:
         actionstr = "Assignment Message Out";
         break;
      case AssetManagerInterface::E_SELF_DEFENSE_ASSIGNMENT_CREATED:
         actionstr = "Self Defense Assignment Created";
         break;
      case AssetManagerInterface::E_SENSOR_CUE:
         actionstr = "Sensor Cue";
         break;
      case AssetManagerInterface::E_ASSIGNMENT_TRACK_UPDATE:
         actionstr = "Assignment Track Update";
         break;
   }

   return actionstr;
}

void AssetManagerInterface::handleCreateAssignmentMsgIn(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> am)
{
#define SYSTEMIC_CANTCO true
#define GLOBAL_SCOPE true
#define LOCAL_SCOPE false
#define NO_ACTIVE_ASSIGNMENT false

   if (m_battle_manager && m_battle_manager->getCommitAuthority())
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "Warning - received create assignment on commit authority platform...unsupported";
      // generate a systemic cantco (global)
      internallyCANTCOAssignment(aMOELogger, am, "No authority - receiving platform has commit", E_REJECTED_ASSIG_RESPONSE, SYSTEMIC_CANTCO, GLOBAL_SCOPE);
   }
   else
   {
      const auto& master_track_record = getMasterTrack(am->getLocalTrackID());
      if (!master_track_record)
      {
         // no master track?
         internallyCANTCOAssignment(aMOELogger, am, "No master track", E_REJECTED_ASSIG_RESPONSE);
      } // no master track?
      else if (isExistingAssignment(am))
      {
         // existing assignment?
         // don't cancel it, just flag it as a warning
         HCL_ERROR_LOGGER(mGlobalLogger) << "Received assignment for " << am->getAssignedID() << " on track " << am->getLocalTrackID() << " but already prosecuting assignment. Ignoring.\n";

         auto existing_assignment = getAssignment(am->getLocalTrackID(), am->getAssignedID());
         if (existing_assignment->getAssigningID() != am->getAssigningID())
         {
            HCL_ERROR_LOGGER(mGlobalLogger) << "Assignment is conflicting, sending a CANTCO rejection back to the sender\n";
            internallyCANTCOAssignment(aMOELogger, am, "Already prosecuting assignment", E_REJECTED_ASSIG_RESPONSE);
         }

      } // existing assignment?
      else if (this->isTrackAssigned(am->getLocalTrackID()))
      {
         HCL_ERROR_LOGGER(mGlobalLogger) << "Received assignment for " << am->getAssignedID() << " on track " << am->getLocalTrackID() << " but track is already assigned. Conflicting assignment; sending a CANTCO rejection back to the sender\n";
         internallyCANTCOAssignment(aMOELogger, am, "Already prosecuting assignment", E_REJECTED_ASSIG_RESPONSE);
      }
      else if (m_battle_manager && !m_battle_manager->isTrackEngageable(master_track_record))
      {
         internallyCANTCOAssignment(aMOELogger, am, "Battle manager settings don't allow engagement of track", E_REJECTED_ASSIG_RESPONSE, SYSTEMIC_CANTCO, LOCAL_SCOPE);
      }
      else
      {
         if (!addDelegationAssignment(am))
         {
            // failed to add pending delegation
            internallyCANTCOAssignment(aMOELogger, am, "Delegation Assignment already pending", E_REJECTED_ASSIG_RESPONSE, SYSTEMIC_CANTCO, LOCAL_SCOPE);
         } // failed to add pending delegation
      }
   }
}

void AssetManagerInterface::handleCancelAssignmentMsgIn(std::shared_ptr<assignmentMessage> am)
{
   // update local assignment
   auto assignment = getAssignment(am->getLocalTrackID(), am->getAssignedID());
   // if it's not already complete, go ahead and process it.
   if (assignment && !assignment->isAssignmentComplete())
   {
      assignment->setAssignmentReason(am->getAssignmentReason());
      // increment assignment count
      m_ThisAsset->updateC2ChainAssignedUnitStatus(assignment->getAssignedID(), &m_Assets, assetRecord::INCREMENT, 1, GetIncrementMissileCount(assignment));

      if (assignment->getAssignedID() != getOwnID().getID())
      {
         queueOutgoingMessage(E_CANCEL_ASSIG_RECVD, am);
      }
      else
      {
         HCL_DEBUG_LOGGER(mGlobalLogger) << getCurrentTime() << ": " << getThisAsset()->getName() << " - processed assignment cancel for track " << assignment->getLocalTrackID() << " and weapon " << assignment->getAssignedID();
      }
   }
   else if (assignment)
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "Received assignment cancellation but assignment is already complete...";
   }
   else
   {
      HCL_WARN_LOGGER(mGlobalLogger) << "Received assignment cancellation but no matching assignment exists...";
   }
}


bool AssetManagerInterface::updateMasterTrack(const idRecord& TrackID, std::shared_ptr<airTargetMessage> atm)
{
   auto track = m_MasterTracks.find(TrackID);
   if (m_MasterTracks.end() == track)
   {
      // This is a new track
      std::shared_ptr<trackRecord> newTrack(new trackRecord);
      newTrack->processMessage(*atm);

      m_MasterTracks[newTrack->getID()] = newTrack;
   }
   else
   {
      // Existing track
      track->second->processMessage(*atm);
   }

   // invoke any processors that potentially need track updates (the SM for the case if a cue is ongoing on that track
   notifyMasterTrackUpdate(TrackID);

   // if an assignment exists on the track, send an assignment track update
   track = m_MasterTracks.find(TrackID);
   auto assignments = getAssignmentsOnTrack(TrackID);
   for (auto it = assignments.begin(); it != assignments.end(); ++it)
   {
      const auto& assignment = *it;
      // if we're not the assigned id, ensure we forward the track update
      if (assignment->getAssignedID().getID() != this->getOwnID().getID())
      {
         std::shared_ptr<il::assignmentTrackMessage> assignment_track(new il::assignmentTrackMessage(mGlobalLogger));
         assignment_track->setAssignedUnitID(assignment.Assignment()->getAssignedID());
         assignment_track->setTrack(*track->second);
         queueOutgoingMessage(E_ASSIGNMENT_TRACK_UPDATE, assignment_track);
      }
   }

   return true;
}

void AssetManagerInterface::notifyMasterTrackUpdate(const idRecord& TrackID)
{
   auto track = m_MasterTracks.find(TrackID);
   if (track != m_MasterTracks.end())
   {
      auto sm = this->GetSensorsManager();
      if (sm)
      {
         sm->MasterTrackUpdated(getCurrentTime(), track->second);
      }
   }
   else
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "AssetManagerInterface::notifyMasterTrackUpdate() Invalid Track: " << TrackID;
   }
}

bool AssetManagerInterface::deleteMasterTrack(const idRecord& TrackID, std::shared_ptr<airTargetMessage> atm)
{
   bool erased = false;

   auto track = m_MasterTracks.find(TrackID);
   if (m_MasterTracks.end() != track)
   {
      m_MasterTracks.erase(track);
      erased = true;
   }

   return erased;
}

bool AssetManagerInterface::isTrackAssigned(const idRecord& track_id) const
{
   return m_AssignmentArray.find(track_id) != m_AssignmentArray.end();
}

bool AssetManagerInterface::isExistingAssignment(std::shared_ptr<assignmentMessage> assignment_message)
{
   return getAssignment(assignment_message->getLocalTrackID(), assignment_message->getAssignedID()) ? true : false;
}

bool AssetManagerInterface::unitHasAssignments(const idRecord& asset_id, const bool compare_sub_id) const
{
   bool has_assignments = false;

   // if this is a weapon, do a direct lookup of the assignments array
   auto unit = m_Assets.GetAsset(asset_id);
   if (!unit)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "AssetManagerInterface::unitHasAssignments(): Invalid asset id: " << asset_id;
      return has_assignments = false;
   }

   if (unit->hasWeapon())
   {
      // this has an early yield component, manual loop here
      // for all assignment tracks
      for (auto assigned_track_itor = m_AssignmentArray.begin(), assign_itor = assigned_track_itor;
           assigned_track_itor != m_AssignmentArray.end() && !has_assignments;
           assigned_track_itor = assign_itor)
      {
         auto curr_track_id = assigned_track_itor->first; // save value in case of a destructive inner lambda
         for (; assign_itor != m_AssignmentArray.end() && assign_itor->first == curr_track_id;)
         {
            // foreach assignment
            if ((!compare_sub_id && assign_itor->second->getAssignedID().getID() == asset_id.getID()) ||
                (compare_sub_id && assign_itor->second->getAssignedID() == asset_id))
            {
               has_assignments = true;
               break;
            }

            assign_itor++;

         } // foreach assignment
      }
   }
   else if (unit->getC2Capable())
   {
      for (auto assigned_track_itor = m_AssignmentArray.begin(), assign_itor = assigned_track_itor;
           assigned_track_itor != m_AssignmentArray.end() && !has_assignments;
           assigned_track_itor = assign_itor)
      {
         auto curr_track_id = assigned_track_itor->first; // save value in case of a destructive inner lambda
         for (; assign_itor != m_AssignmentArray.end() && assign_itor->first == curr_track_id;)
         {
            // foreach assignment
            if ((!compare_sub_id && unit->isSubordinate(assign_itor->second->getAssignedID().getID(), m_Assets)) ||
                (compare_sub_id && unit->isSubordinate(assign_itor->second->getAssignedID(), m_Assets)))
            {
               has_assignments = true;
               break;
            }

            assign_itor++;

         } // foreach assignment
      }
   }
   else
   {
      has_assignments = false;
   }

   return has_assignments;
}

// This function iterates through the assignment array of the asset manager and returns true if the weaponID matches the subID of the assigned asset
// (indicating the weapon is assigned).
bool AssetManagerInterface::weaponHasAssignments(il::moeLoggingInterface& aMOELogger, const tWeaponID& weaponID)
{
   bool weaponIsAssigned = false;

   // loop through each active assignment and increment count if the assignment is not complete
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&weaponIsAssigned, &weaponID](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (!assignment_itor->second->isAssignmentComplete() &&
          assignment_itor->second->getAssignedID().getSubID() == weaponID.getID())
      {
         weaponIsAssigned = true;
      }
      ++assignment_itor;
   },
   this);

   return weaponIsAssigned;
}

std::shared_ptr<assignmentMessage> AssetManagerInterface::getAssignment(const idRecord& track_id, const idRecord& assigned_id)
{
   evaluationTrack null_track(mGlobalLogger);
   evaluationAssignment assignment(null_track, nullptr);

   if (getEvaluationAssignment(track_id, assigned_id, assignment))
   {
      return assignment;
   }

   return nullptr;
}


std::vector<evaluationAssignment> AssetManagerInterface::getAssignmentsOnReferenceTrack(il::moeLoggingInterface& aMOELogger, const idRecord& track_id, const idRecord& assigned_id)
{
   std::vector<evaluationAssignment> found_assignments;

   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&found_assignments, track_id, assigned_id](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (assignment_itor->second->getReferenceTrackID() == track_id &&
          assignment_itor->second->getAssignedID() == assigned_id)
      {
         found_assignments.push_back(assignment_itor->second);
      }
      ++assignment_itor;
   },
   this);


   return found_assignments;
}

std::vector<evaluationAssignment> AssetManagerInterface::getOurAssignments(il::moeLoggingInterface& aMOELogger)
{
   std::vector<evaluationAssignment> our_assignments;

   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&our_assignments](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (!assignment_itor->second->isAssignmentComplete() &&
          assignment_itor->second->getAssignedID().getID() == am_ptr->getThisAsset()->getID().getID())
      {
         our_assignments.push_back(assignment_itor->second);
      }
      ++assignment_itor;
   },
   this);

   return our_assignments;
}

bool AssetManagerInterface::getEvaluationAssignment(const idRecord& track_id, const idRecord& assigned_id, evaluationAssignment& eval_assign)
{
   bool found_assignment = false;
   // For assignment equality:
   //    track id
   //    assigned unit
   //    assigned system
   auto assignments = m_AssignmentArray.equal_range(track_id);

   // Loop through all assignments regarding this track ID.
   for (auto existing_assignment = assignments.first; existing_assignment != assignments.second && !found_assignment; ++existing_assignment)
   {
      if (existing_assignment->second.Assignment()->getAssignedID() == assigned_id)
      {
         eval_assign = existing_assignment->second;
         found_assignment = true;
      }
   }

   return found_assignment;
}

std::vector<evaluationAssignment> AssetManagerInterface::getAssignmentsOnTrack(const idRecord& track_id)
{
   std::vector<evaluationAssignment> ret_assignments;

   auto assignments = m_AssignmentArray.equal_range(track_id);

   // Loop through all assignments regarding this track ID.
   for (auto existing_assignment = assignments.first; existing_assignment != assignments.second; ++existing_assignment)
   {
      ret_assignments.push_back(existing_assignment->second);
   }

   return ret_assignments;
}


bool AssetManagerInterface::getEvaluationAssignment(const assignmentACKMessage& ack, evaluationAssignment& eval_assign)
{
   for (auto it = m_AssignmentArray.begin(); it != m_AssignmentArray.end(); ++it)
   {
      const auto& assignment_entry = *it;
      const auto& assignment = *assignment_entry.second.Assignment();
      const auto delegate_assignment = assignment_entry.second.DelegateAssignment();
      bool is_delegate = (delegate_assignment != nullptr);

      if ((assignment.getAssignedID() == ack.getAssignedID() || (is_delegate && delegate_assignment->getAssignedID() == ack.getAssignedID())) &&
          (assignment.getReferenceTrackID() == ack.getAssignmentReferenceTrackID() || (is_delegate && delegate_assignment->getReferenceTrackID() == ack.getAssignmentReferenceTrackID())))
      {
         eval_assign = assignment_entry.second;
         return true;
      }
   }

   return false;
}

bool AssetManagerInterface::addDelegationAssignment(std::shared_ptr<assignmentMessage> assignment_message)
{
   bool added = true;
   // don't check for redundancy since it will be easier to fail out once we have an assignment to catch the case
   // of merge assignments, etc.
   m_PendingDelegateAssignments.insert(std::make_pair(assignment_message->getLocalTrackID(), assignment_message));

   HCL_DEBUG_LOGGER(mGlobalLogger) << "added delegation assignment to pending delegation list for track: " << assignment_message->getLocalTrackID()
                      << ", assignment to: " << assignment_message->getAssignedID()
                      << ", from: " << assignment_message->getAssigningID()
                      << ", on: " << assignment_message->getAssignedID() << std::endl;

   return added;
}

void AssetManagerInterface::wilcoAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment)
{
   internallyUpdateRegularAssignmentStatus(aMOELogger, active_assignment, assignmentACKMessage::WILCO);

   aMOELogger.WILCOAssignment(getCurrentTime(), active_assignment.Assignment()->getStatus().get(), &active_assignment);
}

void AssetManagerInterface::havecoAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, bool was_success)
{
   internallyUpdateRegularAssignmentStatus(aMOELogger, active_assignment, was_success ? assignmentACKMessage::HAVCO_SUCCESS : assignmentACKMessage::HAVCO_FAILURE);

   aMOELogger.HAVCOAssignment(getCurrentTime(), active_assignment.Assignment()->getStatus().get(), &active_assignment);
}

void AssetManagerInterface::shotsFiredAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment)
{
   internallyUpdateRegularAssignmentStatus(aMOELogger, active_assignment, assignmentACKMessage::FIRING);

   if (active_assignment->getAssigningID() != m_ThisAssetID)
   {
      active_assignment->incrementShotsFired();
   }

   aMOELogger.AssignmentStatus(getCurrentTime(), active_assignment.Assignment()->getStatus().get(), &active_assignment);
}

void AssetManagerInterface::sensorTrackingAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const il::sensorRecord* sensor)
{
   // get updated track
   auto master_track = il::FindTrack(active_assignment.Assignment()->getLocalTrackID(), GetMasterTracksArray());

   switch (sensor->GetSensorType())
   {
      case il::SENSOR_TAR:
         internallyUpdateRegularAssignmentStatus(aMOELogger, active_assignment, assignmentACKMessage::TAR_TRACKING);
         aMOELogger.SensorTracking(getCurrentTime(), master_track.get(), sensor);
         break;
      case il::SENSOR_TTR:
         internallyUpdateRegularAssignmentStatus(aMOELogger, active_assignment, assignmentACKMessage::TTR_TRACKING);
         aMOELogger.SensorTracking(getCurrentTime(), master_track.get(), sensor);
         break;
      default:
         HCL_ERROR_LOGGER(mGlobalLogger) << "AssetManagerInterface::sensorTrackingAssignmentStatus(): Attempted to record sensor tracking but senor not TAR or TTR: "
                            << sensor->GetParentAsset()->getName() << ":" << sensor->GetSensorID();
         break;
   }

}

void AssetManagerInterface::internallyUpdateRegularAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const il::assignmentACKMessage::eAssignmentStatus status_enum)
{
   // already has a status?
   std::shared_ptr<assignmentACKMessage> status = active_assignment->getStatus();
   if (status)
   {
      status->setAckTime(getCurrentTime());
      status->setStatus(status_enum);
      queueOutgoingMessage(E_ASSIG_ACK_RCVD, status);
   }
   else
   {
      // fabricate an assignment ack since we don't have an existing ack
      std::shared_ptr<assignmentACKMessage> new_ack(new assignmentACKMessage(mGlobalLogger));
      new_ack->setAckTime(getCurrentTime());
      new_ack->setAssignedID(active_assignment->getAssignedID());
      new_ack->setInitiatingID(active_assignment->getInitiatingID());
      new_ack->setAssigningID(active_assignment->getAssigningID());
      new_ack->setAssignmentReferenceTrackID(active_assignment->getReferenceTrackID());
      new_ack->setStatus(status_enum);
      queueOutgoingMessage(E_ASSIG_ACK_RCVD, new_ack);

      active_assignment->setStatus(new_ack);
   }

   if (active_assignment->isAssignmentComplete())
   {
      updatePerceptions(aMOELogger);
   }
}

void AssetManagerInterface::internallyCancelAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const std::string& cancel_reason)
{
   // log the cancel before bookkeeping
   aMOELogger.AssignmentCancel(getCurrentTime(), &m_ThisAssetID, &active_assignment, cancel_reason);

   // don't check if the assignment is active, by definition, if we are being called, it better be against an active assignment
   if (active_assignment.Assignment()->getAssignedID().getID() == this->getOwnID().getID())
   {
      // already has a status?
      std::shared_ptr<assignmentACKMessage> status = active_assignment->getStatus();
      if (status)
      {
         status->setAckTime(getCurrentTime());
         status->setStatus(assignmentACKMessage::CANCELLED);
         status->setCantcoReason(cancel_reason);
         queueOutgoingMessage(E_ASSIG_ACK_RCVD, status);
      }
      else
      {

         // fabricate an assignment ack - cancelled
         std::shared_ptr<assignmentACKMessage> new_ack(new assignmentACKMessage(mGlobalLogger));
         new_ack->setAckTime(getCurrentTime());
         new_ack->setAssignedID(active_assignment->getAssignedID());
         new_ack->setInitiatingID(active_assignment->getInitiatingID());
         new_ack->setAssigningID(active_assignment->getAssigningID());
         new_ack->setAssignmentReferenceTrackID(active_assignment->getReferenceTrackID());
         new_ack->setStatus(assignmentACKMessage::CANCELLED);
         new_ack->setCantcoReason(cancel_reason);
         queueOutgoingMessage(E_ASSIG_ACK_RCVD, new_ack);

         active_assignment->setStatus(new_ack);
      }
   }
   else
   {
      // mark assignment as cancelled
      active_assignment->setAssignmentReason(assignmentMessage::CANCEL);
      queueOutgoingMessage(E_ASSIGN_MSG_OUT, active_assignment.Assignment());

      // release assignment
      m_ThisAsset->updateC2ChainAssignedUnitStatus(active_assignment->getAssignedID(), &m_Assets, assetRecord::INCREMENT, 1, GetIncrementMissileCount(active_assignment));
   }

}

void AssetManagerInterface::internallyCANTCOAssignment(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> assignment_message, const std::string& cantco_reason, const eMessageAction why_action, const bool is_systemic, const bool is_global)
{

   // log the event
   aMOELogger.CANTCOAssignment(getCurrentTime(), &m_ThisAssetID, assignment_message.get(), FindTrack(assignment_message->getLocalTrackID(), m_MasterTracks).get(), cantco_reason);


   // already has a status?
   std::shared_ptr<assignmentACKMessage> status = assignment_message->getStatus();
   if (status)
   {
      status->setAckTime(getCurrentTime());
      status->setStatus(assignmentACKMessage::CANTCO);
      status->setCantcoReason(cantco_reason);
   }
   else
   {
      // fabricate an assignment ack - cancelled
      status.reset(new assignmentACKMessage(mGlobalLogger));
      status->setAckTime(getCurrentTime());
      status->setAssignedID(assignment_message->getAssignedID());
      status->setInitiatingID(assignment_message->getInitiatingID());
      status->setAssigningID(assignment_message->getAssigningID());
      status->setAssignmentReferenceTrackID(assignment_message->getReferenceTrackID());
      status->setStatus(assignmentACKMessage::CANTCO);
      status->setCantcoReason(cantco_reason);

      assignment_message->setStatus(status);
   }

   // update systemic CANTCO information
   if (is_systemic)
   {
      if (is_global)
      {
         status->setSystemicCantcoScope(assignmentACKMessage::GLOBAL);
      }
      else
      {
         status->setSystemicCantcoScope(assignmentACKMessage::LOCAL);
      }

      HCL_DEBUG_LOGGER(mGlobalLogger) << "Got systemic CANTCO, adding reference track id " << status->getAssignmentReferenceTrackID() << " to exclusion array. Is Global? " << il::BoolToString(is_global);
      // add the the exclusion array
      m_ExclusionArray.addExclusion(status);
   }

   // queue the status
   queueOutgoingMessage(E_ASSIG_ACK_RCVD, status);

   // if this is an active assignment, we need to cancel it and update our local perception
   evaluationTrack null_track(mGlobalLogger);
   evaluationAssignment eval_assignment(null_track, nullptr);

   getEvaluationAssignment(assignment_message->getLocalTrackID(), assignment_message->getAssignedID(), eval_assignment);
   auto existing_assignment = eval_assignment.Assignment();
   // note: for rejected assignment responses, we don't necessarily want to kill the local assignment if we have one, that will be the directive of something like E_CANCEL_ASSIG_RECVD
   if (existing_assignment && why_action != E_REJECTED_ASSIG_RESPONSE)
   {
      HCL_DEBUG_LOGGER(mGlobalLogger) << "CANTCO was against an active assignment, cancelling. Reference Track id: " << assignment_message->getReferenceTrackID() << ", Local Track id: " << assignment_message->getLocalTrackID() << ", Assigned ID: " << assignment_message->getAssignedID();

      // update status
      existing_assignment->setStatus(status);


      // if we're the assigned unit, allow internally cancel assignemtn to release the assignment, otherwise, us
      if (existing_assignment->getAssignedID().getID() != getOwnID().getID())
      {
         internallyCancelAssignment(aMOELogger, eval_assignment, cantco_reason);
      }
      else
         // release assignment
      {
         m_ThisAsset->updateC2ChainAssignedUnitStatus(existing_assignment->getAssignedID(), &m_Assets, assetRecord::INCREMENT, 1, existing_assignment->getRequestedMissilesToCommit());
      }


   }
}

namespace
{
   std::shared_ptr<assignmentACKMessage> CreateCommonOverride(const evaluationAssignment& assignment, const double curr_time, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   {
      std::shared_ptr<assignmentACKMessage> new_ack(new assignmentACKMessage(aGlobalLogger));

      auto& original_assignment = assignment.Assignment();
      new_ack->setAckTime(curr_time);
      new_ack->setAssignedID(original_assignment->getAssignedID());
      new_ack->setInitiatingID(original_assignment->getInitiatingID());
      new_ack->setAssigningID(original_assignment->getAssigningID());
      new_ack->setAssignmentReferenceTrackID(original_assignment->getReferenceTrackID());

      return new_ack;
   }
}


std::shared_ptr<assignmentACKMessage> AssetManagerInterface::generateAssignmentOverrideNotification(const evaluationAssignment& assignment)
{
   auto new_ack = CreateCommonOverride(assignment, getCurrentTime(), mGlobalLogger);

   new_ack->setStatus(assignmentACKMessage::CHANGED_ASSIGNED_UNIT);

   auto& new_assignment = assignment.Assignment();

   new_ack->setAssignedID(assignment.DelegateAssignment()->getAssignedID());
   new_ack->setOverridingID(new_assignment->getInitiatingID());
   new_ack->setNewlyAssignedID(new_assignment->getAssignedID());


   return new_ack;
}

std::shared_ptr<assignmentACKMessage> AssetManagerInterface::generateDoctrineOverrideNotification(const evaluationAssignment& assignment)
{
   auto new_ack = CreateCommonOverride(assignment, getCurrentTime(), mGlobalLogger);

   new_ack->setStatus(assignmentACKMessage::UPDATED_SHOT_DOCTRINE);

   auto& new_assignment = assignment.Assignment();

   new_ack->setUpdatedShotDoctrine(new_assignment->getShotDoctrine());

   return new_ack;
}




////////////////////////
// Migrated Interface //
////////////////////////
void AssetManagerInterface::setOwnID(const idRecord& AssetID)
{
   m_ThisAssetID = AssetID;
   getThisAssetPtr();
}


idRecord AssetManagerInterface::getOwnID() const
{
   return m_ThisAssetID;
}

// this is really GTIQDAssetManagerSetStatus
bool AssetManagerInterface::updateOwnStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const double ecef_pos_m[3], const double ecef_vel_ms[3], const il::SYSTEM_STATUS status)
{

   bool time_to_send_position = SetPosition(sim_time, ecef_pos_m, ecef_vel_ms);

   bool time_to_send_status = SetStatus(aMOELogger, sim_time, status, time_to_send_position); // if we're sending position, force a status update as well

   // Update perceptions
   updatePerceptions(aMOELogger);

   // Update Own Status

   if (time_to_send_position || time_to_send_status)
   {
      // queue own position
      auto csm = m_ThisAsset->generateCombatStatusMessage();
      queueOutgoingMessage(E_STATUS_RCVD, csm);
   }

   return true;
}

void AssetManagerInterface::logMyStatus() const
{
   if (!m_ThisAsset)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "AssetManagerInterface: ThisAsset ptr is NULL!\n";
      throw std::runtime_error("ThisAsset ptr is NULL.");
   }

   m_ThisAsset->logMyStatus();
}

void AssetManagerInterface::logSubordinateStatus(bool direct_only) const
{
   if (!m_ThisAsset)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "AssetManagerInterface: ThisAsset ptr is NULL!\n";
      throw std::runtime_error("ThisAsset ptr is NULL.");
   }

   m_ThisAsset->logSubordinateStatus(m_Assets, direct_only);
}

void AssetManagerInterface::setAssetYellowAgeout(const double ageout_time_s)
{
   m_asset_yellow_ageout_time = ageout_time_s;
}

void AssetManagerInterface::setAssetRedAgeout(const double ageout_time_s)
{
   m_asset_red_ageout_time = ageout_time_s;
}

double AssetManagerInterface::getAssetYellowAgeout() const
{
   return m_asset_yellow_ageout_time;
}

void AssetManagerInterface::setReportPositionEveryMeters(const double distance_m)
{
   m_report_position_every_meters = distance_m;
}


void AssetManagerInterface::setReportPositionEverySeconds(const double time_s)
{
   m_report_position_every_seconds = time_s;
}

void AssetManagerInterface::setReportStatusEverySeconds(const double time_s)
{
   m_report_status_every_seconds = time_s;
}

void AssetManagerInterface::setAggregateUnitStatus(const bool is_set)
{
   m_aggregate_unit_status = is_set;
}

void AssetManagerInterface::setStationaryOpnsOnly(const bool is_set)
{
   m_stationary_opns_only = is_set;
}

void AssetManagerInterface::setWeaponRequired(const bool is_set)
{
   m_weapon_required = is_set;
}

void AssetManagerInterface::setRequireAllWeapons(const bool is_set)
{
   m_require_all_weapons = is_set;
}

void AssetManagerInterface::setEWRequired(const bool is_set)
{
   m_ew_required = is_set;
}

void AssetManagerInterface::setTARRequired(const bool is_set)
{
   m_tar_required = is_set;
}

void AssetManagerInterface::setTTRRequired(const bool is_set)
{
   m_ttr_required = is_set;
}

double AssetManagerInterface::getAssetRedAgeout() const
{
   return m_asset_red_ageout_time;
}

double AssetManagerInterface::getReportPositionEveryMeters() const
{
   return m_report_position_every_meters;
}

double AssetManagerInterface::getReportPositionEverySeconds() const
{
   return m_report_position_every_seconds;
}

double AssetManagerInterface::getReportStatusEverySeconds() const
{
   return m_report_status_every_seconds;
}

bool AssetManagerInterface::getAggregateUnitStatus() const
{
   return m_aggregate_unit_status;
}

bool AssetManagerInterface::getStationaryOpnsOnly() const
{
   return m_stationary_opns_only;
}

bool AssetManagerInterface::getWeaponRequired() const
{
   return m_weapon_required;
}

bool AssetManagerInterface::getRequireAllWeapons() const
{
   return m_require_all_weapons;
}

bool AssetManagerInterface::getEWRequired() const
{
   return m_ew_required;
}

bool AssetManagerInterface::getTARRequired() const
{
   return m_tar_required;
}

bool AssetManagerInterface::getTTRRequired() const
{
   return m_ttr_required;
}

idRecord AssetManagerInterface::getMessageSenderInfo(std::shared_ptr<baseMessage> Message, bool& fromCommander, bool& fromSubordinate, bool& fromPeer)
{
   idRecord SenderID(Message->getSenderID());

   fromCommander = m_ThisAsset->isDirectCommander(SenderID);
   fromSubordinate = m_ThisAsset->isDirectSubordinate(SenderID);
   fromPeer = m_ThisAsset->isDirectPeer(SenderID);

   return SenderID;
}


assetRecord& AssetManagerInterface::getMyAsset() const
{
   return *m_ThisAsset;
}



void AssetManagerInterface::getThisAssetPtr()
{
   m_ThisAsset = m_Assets.GetAsset(m_ThisAssetID);

   if (!m_ThisAsset)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "AssetManagerInterface::getThisAssetPtr(): This asset not found in assetMap! asset id = " << m_ThisAssetID << std::endl;
      throw std::runtime_error("This asset not found in assetMap.");
   }
}

bool AssetManagerInterface::isInitialized() const
{
   return (m_ThisAsset != nullptr);
}

void AssetManagerInterface::CalculateTrackStrengths(const double SimTime, const double max_grouping_distance)
{
   for (auto it = m_MasterTracks.begin(); it != m_MasterTracks.end(); ++it)
   {
      auto& CurrTrack = it->second;
      CurrTrack->calculateStrength(SimTime, m_MasterTracks, max_grouping_distance);
   }
}


////////////////////////////
// End Migrated Interface //
////////////////////////////

int AssetManagerInterface::GetIncrementMissileCount(std::shared_ptr<assignmentMessage> assignment) const
{
   return assignment->getRequestedMissilesToCommit() - assignment->getSalvosFired();
}

void AssetManagerInterface::handleIncomingCue(std::shared_ptr<cueMessage> cue)
{
   // resolve master track and assigned sensor
   auto sensor = getThisAsset()->getSensor(cue->getCuedSensorID());
   if (!sensor)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "AssetManagerInterface::handleIncomingCue(): Received cue for non-existent sensor: " << cue->getCuedSensorID();
      return;
   }

   // require at least a reference track to be in place if we're cueing a TTR
   auto master_track = FindTrack(cue->getLocalTrackID(), m_MasterTracks);
   if (sensor->IsCueable() && !master_track)
   {
      HCL_ERROR_LOGGER(mGlobalLogger) << "AssetManagerInterface::handleIncomingCue(): Received cue for reference track: " << cue->getReferenceTrackID() << " but could not resolve local track for sensor.";
      return;
   }

   // note: if a TAR, the master track could be null
   m_active_sensor_cues.HandleCue(getCurrentTime(), master_track, cue, sensor);
}

///////////////////////
// HELIOS Interfaces //
///////////////////////
bool AssetManagerInterface::SetPosition(const double sim_time, const double ecef_pos_m[3], const double ecef_vel_ms[3], bool force_send)
{
   bool time_to_send_position = false;

   if (!m_ThisAsset)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "invoked SetPosition but m_ThisAsset is unset!";
      return time_to_send_position;
   }


   // grab data we'll need for determining if we should send an update or not before we update internally
   auto last_position = m_ThisAsset->getPosition();
   auto last_update_time = m_ThisAsset->getStatusTime();

   // we unconditionally update internally regardless of whether we send a message or not
   m_ThisAsset->setPositionXYZ(ecef_pos_m[0], ecef_pos_m[1], ecef_pos_m[2]);
   m_ThisAsset->setVelocityXYZ(ecef_vel_ms[0], ecef_vel_ms[1], ecef_vel_ms[2]);
   m_ThisAsset->setPositionTime(sim_time);

   // determine if we need to trigger a self position message update
   if (!m_status_initialized ||
       m_ThisAsset->getPosition().calculateDistanceMeters(last_position) > this->m_report_position_every_meters ||
       m_ThisAsset->getPositionTime() - last_update_time >= this->m_report_position_every_seconds ||
       NumericUtils::NearlyEqual(m_ThisAsset->getPositionTime() - last_update_time, this->m_report_position_every_seconds) ||
       force_send)
   {
      time_to_send_position = true;
   }

   return time_to_send_position;
}

bool AssetManagerInterface::SetStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const il::SYSTEM_STATUS external_own_status, bool force_send)
{
   bool time_to_send_status = false;

   if (!m_status_initialized ||
       sim_time - m_ThisAsset->getStatusTime() >= this->m_report_status_every_seconds ||
       NumericUtils::NearlyEqual(sim_time - m_ThisAsset->getStatusTime(), this->m_report_status_every_seconds) ||
       force_send)
   {
      SetOwnStatus(aMOELogger, sim_time, external_own_status);

      CheckSubordinateTimeout(sim_time);

      time_to_send_status = true;
   }

   return time_to_send_status;
}

void AssetManagerInterface::SetOwnStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const il::SYSTEM_STATUS external_own_status)
{
   if (!m_ThisAsset)
   {
      HCL_FATAL_LOGGER(mGlobalLogger) << "invoked SetStatus but m_ThisAsset is unset!";
   }

   // InitializeOwnStatus
   bool is_initialization = false;
   if (!m_status_initialized)
   {
      m_ThisAsset->setMaxAssignments(this->m_max_assignments);
      m_ThisAsset->setNumAssignments(0);
      m_ThisAsset->setAssignmentDelay(this->m_assignment_delay_s);
      m_ThisAsset->setAggregateUnit(this->m_aggregate_unit_status);

      is_initialization = m_status_initialized = true;
   }

   // set status time to now
   m_ThisAsset->setStatusTime(sim_time);

   // perform status checks

   // Set up a failed status string to log which portions of the status check failed.
   std::string failed_status_string;
   auto set_c2_failure_string = [&failed_status_string](const std::string & failure) -> bool
   {
      if (failed_status_string.empty())
      {
         failed_status_string = failure;
      }
      else
      {
         failed_status_string += ", " + failure;
      }
      return false;
   };

   // Pre-calculate weapon status, and store it for later use.
   SYSTEM_STATUS weaponStatus = STATUS_RED;
   if (m_weapon_required)
   {
      weaponStatus = m_aggregate_unit_status ? m_ThisAsset->getWeaponStatus(m_Assets, m_require_all_weapons) : m_ThisAsset->getWeaponStatus(m_require_all_weapons);
   }

   // If C2 Status is GREEN, and all status requirements are met (either by this asset, or its subordinates, if aggregate unit status is on, set system status GREEN
   // Whatever requirements are failed should be reflected in the failed_status_string
   if ((m_ThisAsset->getC2Status() == STATUS_GREEN || set_c2_failure_string("C2 Failure")) &
       (!m_weapon_required || (weaponStatus == STATUS_GREEN || set_c2_failure_string("Weapon Failure"))) &
       (!m_ew_required || ((m_aggregate_unit_status ? m_ThisAsset->getEWStatus(m_Assets) : m_ThisAsset->getEWStatus()) == STATUS_GREEN || set_c2_failure_string("EW Failure"))) &
       (!m_tar_required || ((m_aggregate_unit_status ? m_ThisAsset->getTARStatus(m_Assets) : m_ThisAsset->getTARStatus()) == STATUS_GREEN || set_c2_failure_string("TAR Failure"))) &
       (!m_ttr_required || ((m_aggregate_unit_status ? m_ThisAsset->getTTRStatus(m_Assets) : m_ThisAsset->getTTRStatus()) == STATUS_GREEN || set_c2_failure_string("TTR Failure"))) &
       (!m_stationary_opns_only || (is_initialization) || (m_ThisAsset->getStationaryStatus() == STATUS_GREEN || set_c2_failure_string("Non-Stationary Failure"))) &
       (external_own_status == STATUS_GREEN || set_c2_failure_string("Externally-Defined Failure (" + systemStatusToString(external_own_status) + ")")))
   {
      m_ThisAsset->setSystemStatus(STATUS_GREEN);
   }
   // If the only entry in the failed status string is "Weapon Failure", the correct status might be WHITE
   else if (failed_status_string == "Weapon Failure")
   {
      m_ThisAsset->setSystemStatus(weaponStatus);
   }
   else
   {
      m_ThisAsset->setSystemStatus(STATUS_RED);
   }

   // if we triggered a failure, kill all assignments regardless of aggregate status
   if (!m_ThisAsset->isGreen())
   {
      // log our red status
      HCL_WARN_LOGGER(mGlobalLogger) << "Asset Manager Status WARNING: " << m_ThisAsset->getName() << " reporting " << systemStatusToString(m_ThisAsset->getSystemStatus()) << " status. Reason: " << failed_status_string;

      // kill all assignments
      killAllAssignments(aMOELogger, "Internal " + systemStatusToString(m_ThisAsset->getSystemStatus()) + " status triggered. Reason: " + failed_status_string);
   }
}


void AssetManagerInterface::CheckSubordinateTimeout(const double sim_time)
{
   m_ThisAsset->checkSubordinatesTimeout(m_Assets, sim_time);
}

void AssetManagerInterface::killAllAssignments(il::moeLoggingInterface& aMOELogger, const std::string& reason)
{
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&reason](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (!assignment_itor->second->isAssignmentComplete())
      {
         // CANTCO ongoing assignments then cancel them
         am_ptr->cantcoAssignment(aMOELogger, assignment_itor->second, reason);

         am_ptr->cancelAssignment(aMOELogger, assignment_itor->second, reason);
      }
      ++assignment_itor;
   },
   this);
}

// This function sets the number of assignments member variable of this asset manager's asset to the current number of active assignments
// by counting the number of incomplete assignments in this asset manager's assignment array. Then, the function loops through all the
// weapons on this asset manager's asset and updates the asset's perception of total munitions, prepped munitions and available fire channels
// to truth data for all weapons on platforms without an on-platform battle manager and all weapons without active assignments for platforms
// with an on-platform battle manager.
void AssetManagerInterface::updatePerceptions(il::moeLoggingInterface& aMOELogger)
{
   m_ThisAsset->setNumAssignments(GetNumIncompleteAssignments(aMOELogger));
   auto weapons = m_ThisAsset->getWeapons();
   for (auto it = weapons.begin(); it != weapons.end(); ++it)
   {
      const auto& weapon = *it;
      if (!(GetBattleManager() && weaponHasAssignments(aMOELogger, weapon->GetWeaponID())))
      {
         m_ThisAsset->updatePerceptions(weapon->GetWeaponID());
      }
   }
}

// This function iterates through the assignment array of this asset manager and returns the number of assignments that are not complete.
unsigned int AssetManagerInterface::GetNumIncompleteAssignments(il::moeLoggingInterface& aMOELogger)
{
   unsigned int count = 0;

   // loop through each active assignment and increment count if the assignment is not complete
   MapAssignmentArrayFunc
   (aMOELogger, m_AssignmentArray,
    [&count](il::moeLoggingInterface& aMOELogger, evalAssignmentMultimap & assignments, evalAssignmentMultimap::iterator & assigned_track_itor, evalAssignmentMultimap::iterator & assignment_itor, AssetManagerInterface * am_ptr) -> void
   {
      if (!assignment_itor->second->isAssignmentComplete())
         ++count;
      ++assignment_itor;
   },
   this);

   return count;
}

} // namespace il
