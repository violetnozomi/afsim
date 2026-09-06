// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// UNCLASSIFIED

#include <logger.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/BattleManagerIface.hpp>
#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/util.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

namespace il
{

BattleManagerInterface* BattleManagerInterface::Clone()
{
   return new BattleManagerInterface(*this);
}

BattleManagerInterface::BattleManagerInterface(const std::shared_ptr<terrainInterface>& terrain_interface)
   : ReferencesAssetManager()
   , m_commit_authority(false)
   , m_project_trks_by_assign_delays(true)  // same as HELIOS
   , m_target_projection_time_s(cDEFAULT_TRACK_PROJECTION_TIME)  // default to a minute by 10 second intervals
   , m_target_projection_time_incr_s(cDEFAULT_TRACK_PROJECTION_INCR)
   , m_terrain_interface(terrain_interface)
{

}

BattleManagerInterface::BattleManagerInterface(const BattleManagerInterface& from)
   : ReferencesAssetManager(from)
   , m_EvaluationTracks()
   , m_EvaluatedAssignments()
   , m_Threats()
   , m_commit_authority(from.m_commit_authority)
   , m_trk_engage_rules(from.m_trk_engage_rules)
   , m_project_trks_by_assign_delays(from.m_project_trks_by_assign_delays)
   , m_target_projection_time_s(from.m_target_projection_time_s)
   , m_target_projection_time_incr_s(from.m_target_projection_time_incr_s)
   , m_xcept_iface(from.m_xcept_iface)
   , m_terrain_interface(from.m_terrain_interface)
{

}



void BattleManagerInterface::SetInterceptCalculator(const std::shared_ptr<InterceptCalculatorIface>& xcept_iface)
{
   m_xcept_iface = xcept_iface;
}



void BattleManagerInterface::setCommitAuthority(const bool value)
{
   m_commit_authority = value;
}

bool BattleManagerInterface::getCommitAuthority() const
{
   return m_commit_authority;
}

void BattleManagerInterface::setProjectTracksByDelays(const bool value)
{
   m_project_trks_by_assign_delays = value;
}

bool BattleManagerInterface::getProjectTracksByDelays() const
{
   return m_project_trks_by_assign_delays;
}

void BattleManagerInterface::setTargetProjectionTime(const double time_s)
{
   m_target_projection_time_s = time_s;
}

double BattleManagerInterface::getTargetProjectionTime() const
{
   return m_target_projection_time_s;
}

void BattleManagerInterface::setTargetProjectionTimeIncrement(const double time_s)
{
   m_target_projection_time_incr_s = time_s;
}

double BattleManagerInterface::getTargetProjectionTimeIncrement() const
{
   return m_target_projection_time_incr_s;
}

void BattleManagerInterface::setTerrainEngine(const std::shared_ptr<terrainInterface>& terrain_interface)
{
   m_terrain_interface = terrain_interface;
}

void BattleManagerInterface::addTrackIFFROE(const eIFFType iff, const bool engage)
{
   m_trk_engage_rules.SetEngageIffRule(iff, engage);
}


/////////////////// Interfaces needed by Asset Manager Interface ///////////////////

/////////////////// End Interfaces needed by Asset Manager Interface ///////////////////




size_t BattleManagerInterface::getNumThreats() const
{
   return m_Threats.size();
}


void BattleManagerInterface::makeAssignment(const idRecord& initiating_unit_id,
                                            const idRecord& assigning_unit_id,
                                            const idRecord& weapon_unit_id,
                                            const idRecord& weapon_id,
                                            const evaluationTrack& evaluation_track,
                                            const eShotDoctrine shot_doctrine,
                                            const double max_shot_time)
{
   std::shared_ptr<assignmentMessage> newAssignment(new assignmentMessage(GetAssetManager()->getGlobalLogger()));

   auto& track_id = evaluation_track.Track()->getID();

   // create assignment based on this assessment
   newAssignment->create(m_asset_manager->getCurrentTime(),
                         shot_doctrine,
                         initiating_unit_id,
                         assigning_unit_id,
                         track_id,
                         weapon_unit_id,
                         weapon_id,
                         max_shot_time,
                         m_target_projection_time_s,
                         m_target_projection_time_incr_s,
                         false);

   // push it to the pending assignment array
   evaluationAssignment eval_assignment(evaluation_track, newAssignment);
   m_EvaluatedAssignments.push_back(eval_assignment);

   HCL_DEBUG_LOGGER(GetAssetManager()->getGlobalLogger()) << "Time: " << m_asset_manager->getCurrentTime()
                      << ", track for target " << eval_assignment.Track()->getTargetTruthName()
                      << "(" << eval_assignment.Track()->getTargetTruthSide() << ")"
                      << " assigned to " << weapon_unit_id.getIDString()
                      << ". Weapon: " << weapon_id.getIDString()
                      << ", Assigning unit " << assigning_unit_id.getIDString()
                      << ", Shot doctrine: " << ShotDoctrineEnumToString(shot_doctrine)
                      << ", Max shot time: " << max_shot_time << std::endl;

   // update our C2 perception for assignments and estimated munitions
   m_asset_manager->getThisAsset()->updateC2ChainAssignedUnitStatus(newAssignment->getAssignedID(),
                                                                    &m_asset_manager->getAssets(),
                                                                    assetRecord::DECREMENT,
                                                                    1, // 1 assignment
                                                                    newAssignment->getRequestedMissilesToCommit());
}

int BattleManagerInterface::getNumAssignmentsPending(const evaluationTrack& eval_track) const
{
   int num_pending = 0;

   for (auto it = m_EvaluatedAssignments.begin(); it != m_EvaluatedAssignments.end(); ++it)
   {
      const auto& assignment = *it;
      if (assignment->getLocalTrackID() == eval_track.Track()->getID())
      {
         num_pending++;
      }
   }

   return num_pending;
}

void BattleManagerInterface::CANTCOUnallocatedDelegations(il::moeLoggingInterface& aMOELogger, const std::string& reason)
{
   // for any delegations that are not pending assignments now, and haven't
   // been explicitly excluded
   for (auto it = m_EvaluationTracks.begin(); it != m_EvaluationTracks.end(); ++it)
   {
      auto& track = *it;
      const auto& delegation = track.DelegateAssignment();

      // check if this track is a pending cancelled delegation, if not, there's
      // nothing else to check
      if (!delegation)
      {
         continue;
      }

      // if the incoming assignment has already been cancelled, skip it
      if (delegation->isAssignmentComplete())
      {
         continue;
      }

      // check that this track isn't already assigned - if not, it's a cancel
      if (getNumAssignmentsPending(track) == 0)
      {
         CANTCODelegatedAssignment(aMOELogger, track.DelegateAssignment(), "Assignment pending delegation unable to be processed:" + reason, false, false);
      }
   }

}

void BattleManagerInterface::CANTCODelegatedAssignment(il::moeLoggingInterface& aMOELogger,
                                                       std::shared_ptr<assignmentMessage>& assignment_message,
                                                       const std::string& cantco_reason,
                                                       const bool is_systemic,
                                                       const bool is_global)
{
   if (assignment_message)
   {
      m_asset_manager->cantcoAssignment(aMOELogger, assignment_message, cantco_reason, is_systemic, is_global);
   }
}

} // namespace il
