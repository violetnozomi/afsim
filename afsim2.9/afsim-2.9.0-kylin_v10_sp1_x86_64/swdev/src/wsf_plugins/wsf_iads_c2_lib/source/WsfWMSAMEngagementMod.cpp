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
*   File: WsfWMSAMEngagementMod.cpp
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
*   Abstract:
*
*******************************************************************************/

#include <WsfWeapon.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfLaunchComputer.hpp>

#include <WsfWMSAMEngagementMod.hpp>
#include <WsfBMUtils.hpp>
#include <logger.hpp>

#include <WsfBMWeaponsManager.hpp>
#include <iadsLib/NumericUtils.hpp>

#include <WsfBMMOELogger.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

///////////////////////
// PendingEngagement //
///////////////////////

WsfWMSAMEngagementMod::PendingEngagement::PendingEngagement(const double& sim_time, il::evaluationAssignment& assignment, WsfSimulation* sim, const il::assessmentRecord& assessment)
   : m_command_time(sim_time)
   , m_next_fire_time_set(false)
   , m_next_fire_time(0)
   , m_assignment(assignment)
   , m_sim_ptr(sim)
   , m_assessment(assessment)
   , m_wpn_initial_rounds_fired_at(0)
   , m_wpn_initial_rounds_completed_for(0)
   , m_wpn_initial_weapons_active_and_pending_for(0)
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();
   if (wpn)
   {
      m_wpn_initial_rounds_fired_at = wpn->RoundsFiredAt(track_id);
      m_wpn_initial_rounds_completed_for = wpn->RoundsCompleteFor(track_id);
      m_wpn_initial_weapons_active_and_pending_for = wpn->WeaponsActiveAndPendingFor(track_id);
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(sim->GetScenario())) << "Invalid weapon in assignment: " << m_assignment->getAssignedID();
   }

   // set up next shot time
   if (m_assessment.canInterceptTrack())
   {
      SetNextFireTime(m_assessment.getAssignmentTime());
   }
}

int WsfWMSAMEngagementMod::PendingEngagement::GetShotsFiredForAssignment() const
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();

   int weapons_fired = 0;

   if (wpn)
   {
      weapons_fired = wpn->RoundsFiredAt(track_id) - m_wpn_initial_rounds_fired_at;
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "Could not resolve weapon from assigned id: " << track_id << std::endl;
   }

   // HCL_INFO_LOGGER() << m_sim_ptr->GetSimTime() << ": RoundsFiredAt(" << track_id << ") = " << weapons_fired;

   return weapons_fired;
}

int WsfWMSAMEngagementMod::PendingEngagement::GetWeaponsDetonatedForAssignment() const
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();

   int weapons_detonated = 0;
   if (wpn)
   {
      weapons_detonated = wpn->RoundsCompleteFor(track_id) - m_wpn_initial_rounds_completed_for;
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "Could not resolve weapon from assigned id: " << track_id << std::endl;
   }
   return weapons_detonated;
}

double WsfWMSAMEngagementMod::PendingEngagement::GetLastFireTimeForAssignment() const
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();

   double time_last_fired = -1;
   if (wpn)
   {
      if (GetShotsFiredForAssignment() > 0)
      {
         time_last_fired = wpn->TimeWeaponLastFiredFor(track_id);
      }
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "Could not resolve weapon from assigned id: " << track_id << std::endl;
   }
   return time_last_fired;
}

double WsfWMSAMEngagementMod::PendingEngagement::GetLastDetonationTimeForAssignment() const
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();

   double time_last_detonated = -1;
   if (wpn)
   {
      if (GetWeaponsDetonatedForAssignment() > 0)
      {
         time_last_detonated = wpn->TimeWeaponLastTerminatedFor(track_id);
      }
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "Could not resolve weapon from assigned id: " << track_id << std::endl;
   }
   return time_last_detonated;
}

int WsfWMSAMEngagementMod::PendingEngagement::GetActiveWeaponsForAssignment() const
{
   auto wpn = GetAssignedWeapon();
   auto track_id = GetAssignedTrackID();

   int weapons_active_and_pending = 0;
   if (wpn)
   {
      weapons_active_and_pending = wpn->WeaponsActiveAndPendingFor(track_id) - m_wpn_initial_weapons_active_and_pending_for;
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_sim_ptr->GetScenario())) << "Could not resolve weapon from assigned id: " << track_id << std::endl;
   }

   return weapons_active_and_pending;
}

double WsfWMSAMEngagementMod::PendingEngagement::GetCommandedTime() const
{
   return m_command_time;
}

bool WsfWMSAMEngagementMod::PendingEngagement::IsNextFireTimeSet() const
{
   return m_next_fire_time_set;
}

double WsfWMSAMEngagementMod::PendingEngagement::GetNextFireTime() const
{
   return m_next_fire_time;
}

void WsfWMSAMEngagementMod::PendingEngagement::SetNextFireTime(const double time_s)
{
   m_next_fire_time = time_s;
   m_next_fire_time_set = true;
}

void WsfWMSAMEngagementMod::PendingEngagement::ResetNextFireTime()
{
   m_next_fire_time = 0;
   m_next_fire_time_set = false;
}

il::evaluationAssignment& WsfWMSAMEngagementMod::PendingEngagement::GetAssignment()
{
   return m_assignment;
}

WsfWeapon* WsfWMSAMEngagementMod::PendingEngagement::GetAssignedWeapon() const
{
   return WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(m_sim_ptr, m_assignment.Assignment()->getAssignedID());
}

WsfTrackId WsfWMSAMEngagementMod::PendingEngagement::GetAssignedTrackID() const
{
   return WsfBMUtils::ConvertTrackId(m_assignment.Assignment()->getLocalTrackID());
}

///////////////////////////
// WsfWMSAMEngagementMod //
///////////////////////////
WsfWMSAMEngagementMod::WsfWMSAMEngagementMod(const WsfScenario& aScenario, WsfBMWeaponsManager* wm_component)
   : m_wm_component(wm_component)
   , m_behavior_tree(aScenario)
   , m_has_behavior_tree(false)
{
}
WsfWMSAMEngagementMod::WsfWMSAMEngagementMod(const WsfWMSAMEngagementMod& from)
   : m_wm_component(from.m_wm_component)
   , m_behavior_tree(from.m_behavior_tree)
   , m_has_behavior_tree(from.m_has_behavior_tree)
{
}

bool WsfWMSAMEngagementMod::Initialize(const double sim_time)
{
   bool success = true;

   success &= m_behavior_tree.Initialize(sim_time, m_wm_component, &m_wm_component->GetScriptContext());

   return success;
}

bool WsfWMSAMEngagementMod::Initialize2(const double sim_time)
{
   bool success = true;

   if (!m_has_behavior_tree)
   {
      HCL_INFO_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_wm_component->GetScenario())) << "Weapons Manager SAM initialized without behavior tree, using default HELIOS implementation.";
   }
   else
   {
      HCL_INFO_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(m_wm_component->GetScenario())) << "Weapons Manager SAM initialized with behavior tree.";
   }
   return success;
}

bool WsfWMSAMEngagementMod::ProcessInput(UtInput& aInput)
{
   bool myCommand = false;

   if (m_behavior_tree.ProcessInput(aInput))
   {
      m_has_behavior_tree = myCommand = true;
   }
   return myCommand;
}

void WsfWMSAMEngagementMod::Run(const double sim_time_s)
{
   if (m_has_behavior_tree)
   {
      m_behavior_tree.Execute(sim_time_s);
   }
   else
   {
      RunDefaultEngagement(sim_time_s); // execute default implementation
   }
}

void WsfWMSAMEngagementMod::RunDefaultEngagement(const double sim_time_s)
{
   auto am_proc = m_wm_component->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();
   auto wm_proc = m_wm_component->GetCoreWMRef();
   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

   // loop through our assignments that we are not engaging and kick them off
   const auto& our_assignments = am_proc->getOurAssignments(*logger);
   for (auto it = our_assignments.cbegin(); it != our_assignments.cend(); ++it)
   {
      auto eval_assignment = *it;
      if (eval_assignment->isAssignmentComplete() ||
          IsEngagedAgainstAssignment(eval_assignment))
      {
         continue;
      }

      auto owning_platform = WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(m_wm_component->GetSimulation(), eval_assignment.Assignment()->getAssignedID());
      auto weapon = WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(m_wm_component->GetSimulation(), eval_assignment.Assignment()->getAssignedID());
      std::shared_ptr<il::assignmentMessage> assignment = eval_assignment.Assignment();
      auto track = eval_assignment.Track();
      if (!weapon)
      {
         HCL_ERROR_LOGGER(am_proc->getGlobalLogger()) << "Error, could not locate assigned weapon for engagement: " << assignment->getAssignedID() << " against " << track->getTargetTruthName();
         am_proc->cantcoAssignment(*logger, assignment, "No weapon");
         continue;
      }
      else if (weapon->GetQuantityRemaining() <= 0)
      {
         HCL_ERROR_LOGGER(am_proc->getGlobalLogger()) << "Error, weapon has no munitions: " << assignment->getAssignedID() << " to engage target " << track->getTargetTruthName();
         am_proc->cantcoAssignment(*logger, assignment, "Weapon has no munitions");
         continue;
      }
      else if (sim_time_s - assignment->getAssignTime() > assignment->getMaxFiringTime() &&
               weapon->RoundsFiredAt(WsfBMUtils::ConvertTrackId(track->getID())) == 0)
      {
         HCL_ERROR_LOGGER(am_proc->getGlobalLogger()) << "Assignment has expired for: " << assignment->getAssignedID() << " against target " << track->getTargetTruthName();
         am_proc->cantcoAssignment(*logger, assignment, "Assignment expired");
         continue;
      }

      // ensure we have a master track and it's of the correct quality
      auto master_track = am_proc->getMasterTrack(track->getID());
      if (!master_track)
      {
         HCL_ERROR_LOGGER(am_proc->getGlobalLogger()) << "Error, no master track for engagement: " << assignment->getAssignedID() << " against " << track->getTargetTruthName();
         am_proc->cantcoAssignment(*logger, assignment, "No Master Track");
         continue;
      }

      // WILCO the assignment if we haven't already
      auto assignment_status = eval_assignment->getStatus();
      if ((!assignment_status) ||
          (!assignment_status->statusTimeExists(il::assignmentACKMessage::WILCO))) // assignment_status
      {
         am_proc->wilcoAssignment(*logger, eval_assignment);
      }

      auto reporting_sensor_type = master_track->getReportingSensorTypeEnum();
      auto reporting_sensor_type_str = master_track->getReportingSensorType();
      if ((reporting_sensor_type == il::SENSOR_RWR) ||
          ((reporting_sensor_type == il::SENSOR_EW) && (!wm_proc->getEngageEWTargets())) ||
          ((reporting_sensor_type == il::SENSOR_TAR) && (!wm_proc->getEngageTARTargets())) ||
          ((reporting_sensor_type == il::SENSOR_TTR) && (!wm_proc->getEngageTTRTargets())))
      {
         HCL_DEBUG_LOGGER(am_proc->getGlobalLogger()) << "Cannot engage target: " << master_track->getTargetTruthName() << ", track is of sensor type " << reporting_sensor_type_str << " waiting...";
         continue;
      }

      if (wm_proc->getEngageLocalTTRTargetsOnly() && !WsfBMUtils::IsTrackGeneratedByLocalSensor(m_wm_component->GetPlatform(), WsfBMUtils::ConvertTrackId(track->getID()), il::SENSOR_TTR))
      {
         HCL_DEBUG_LOGGER(am_proc->getGlobalLogger()) << "Cannot engage target: " << master_track->getTargetTruthName() << ", WM set to engage local TTR tracks only, and track was not generated by a TTR that is local to the platform (on the platform, a peer, or its direct commander) waiting...";
         continue;
      }

      if (reporting_sensor_type == il::SENSOR_UNKNOWN)
      {
         HCL_DEBUG_LOGGER(am_proc->getGlobalLogger()) << "Cannot engage target: " << master_track->getTargetTruthName() << ", no reporting sensor type" << " waiting...";
         continue;
      }

      auto localTrack = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID()));

      if (localTrack->GetQuality() < wm_proc->getEngageTrackQuality())
      {
         HCL_DEBUG_LOGGER(am_proc->getGlobalLogger()) << "Cannot engage target: " << master_track->getTargetTruthName() << ", track quality " << localTrack->GetQuality() << " less than track quality required to engage by WM (" << wm_proc->getEngageTrackQuality() << ")";
         continue;
      }

      AttemptAssignmentEngagement(sim_time_s, eval_assignment, owning_platform, weapon, m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID())));
   }

   // run monitor engagements before execute in case monitor dispatches new engagements
   MonitorEngagements(sim_time_s);

   ExecuteEngagements(sim_time_s);
}

bool WsfWMSAMEngagementMod::AttemptAssignmentEngagement(const double sim_time_s, il::evaluationAssignment& assignment, WsfPlatform* owning_plat, WsfWeapon* weapon, WsfLocalTrack* track)
{
   bool success = false, tmp_dontcare = false;

   auto am_proc = m_wm_component->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();
   auto wm_proc = m_wm_component->GetCoreWMRef();

   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

   if (IsEngagedAgainstAssignment(assignment) &&
       !IsEngagementComplete(assignment, tmp_dontcare) &&
       sim_time_s < GetLastFireTimeForEngagement(assignment) + m_wm_component->GetCoreWMRef()->getTimeBetweenEngagementsDelay())
      // can't engage yet?
   {
      return success = true;
   }

   if (!track)
   {
      am_proc->cantcoAssignment(*logger, assignment.Assignment(), "No Master Track");
      return success = false;
   }

   auto assessment = AssessEngagement(sim_time_s, assignment, owning_plat, weapon, track, 0, wm_proc->getExpectedExecutionDelay(), true);
   if (!assessment.canInterceptTrack())
   {
      am_proc->cantcoAssignment(*logger, assignment.Assignment(), "Cannot intercept");
      return success = false;
   }

   // add the engagement if we haven't already
   std::map<tEngagementTrackId, PendingEngagement>::iterator engagment_iter;
   if (!IsEngagedAgainstAssignment(assignment))
   {
      engagment_iter = m_engagements.insert(std::make_pair(assignment.Track()->getID(), PendingEngagement(sim_time_s, assignment, m_wm_component->GetSimulation(), assessment))).first;
   }
   else
   {
      engagment_iter = m_engagements.find(assignment.Track()->getID());
      engagment_iter->second.SetNextFireTime(assessment.getAssignmentTime());
   }

   return success;
}

il::assessmentRecord WsfWMSAMEngagementMod::AssessEngagement(const double sim_time_s, il::evaluationAssignment& assignment, WsfPlatform* owning_plat, WsfWeapon* weapon, WsfLocalTrack* track, const double assignment_delay_time_s, const double expected_execution_delay_s, const bool project)
{
   auto globalLogger = m_wm_component->GetCoreAMRef()->getGlobalLogger();
   auto script_calculator = GetScriptCalculator(assignment);
   if (!script_calculator)
   {
      HCL_ERROR_LOGGER(globalLogger) << "WsfWMSAMEngagementMod::AssessEngagement(): Could not find a can_intercept script on the weapon platform or weapon manager and the assigned weapon does not have a launch computer. Cannot do final assessment\n";
      return il::assessmentRecord(nullptr, globalLogger, nullptr);
   }

   // get the assigned weapon record
   const std::shared_ptr<il::weaponRecord> weapon_record = m_wm_component->GetCoreAMRef()->getThisAsset()->getWeapon(assignment.Assignment()->getAssignedID());
   if (!weapon_record)
   {
      HCL_ERROR_LOGGER(globalLogger) << "WsfWMSAMEngagementMod::AssessEngagement(): Invalid weapon record referenced in assignment: " << assignment.Assignment()->getAssignedID();
      return il::assessmentRecord(nullptr, globalLogger, nullptr);
   }

   // get the track record
   const auto track_record = assignment.Track();

   // the assessment records have an interface for projecting time to fire, we reuse that here.
   double time_to_project = 0, projection_incr = 10;

   if (project)
   {
      time_to_project = assignment->getMaxProjectionTime();
      projection_incr = assignment->getMaxProjectionTimeIncrement();
   }

   il::assessmentRecord assessment(script_calculator, globalLogger);
   assessment.AssessSAMEngagement(sim_time_s, weapon_record, *track_record, true, assignment_delay_time_s, expected_execution_delay_s, time_to_project, projection_incr);

   return assessment;
}

std::shared_ptr<WsfInterceptCalculator> WsfWMSAMEngagementMod::GetScriptCalculator(const il::evaluationAssignment& assignment)
{
   auto script_calculator = m_script_calculators.find(assignment.Assignment()->getAssignedID());
   // create one if we can
   if (script_calculator == m_script_calculators.end())
   {
      // if there isn't, see if there is a launch computer and invoke the interfaces directly
      std::shared_ptr<WsfInterceptCalculator>  calc_ptr(new WsfInterceptCalculator);

      if (calc_ptr->ScriptCalculatorExists(m_wm_component, WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment.Assignment()->getAssignedID())) &&
          calc_ptr->Initialize(m_wm_component))
      {
         m_script_calculators.insert(std::make_pair(assignment.Assignment()->getAssignedID(), calc_ptr));
         script_calculator = m_script_calculators.find(assignment.Assignment()->getAssignedID());
      }
   }

   return script_calculator != m_script_calculators.end() ? script_calculator->second : nullptr;
}

void WsfWMSAMEngagementMod::ExecuteEngagements(const double sim_time_s)
{
   auto am_proc = m_wm_component->GetCoreAMRef();

   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

   for (auto engagement_iter = m_engagements.begin(); engagement_iter != m_engagements.end(); ++engagement_iter)
   {
      auto& engagement = engagement_iter->second;
      auto& assignment = engagement.GetAssignment();
      auto weapon = WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment->getAssignedID());
      auto& track = assignment.Track();
      auto assignment_local_wsf_track = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID()));

      bool tmp_dontcare = false;
      if (!IsEngagementComplete(assignment, tmp_dontcare) &&
          !assignment->isAssignmentComplete() &&
          !EngagementExpired(sim_time_s, assignment) &&
          engagement_iter->second.IsNextFireTimeSet() &&
          NumericUtils::GreaterThanEqual(sim_time_s, engagement_iter->second.GetNextFireTime()))
      {
         auto owning_plat = WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment->getAssignedID());
         auto assessment = AssessEngagement(sim_time_s, assignment, owning_plat, weapon, assignment_local_wsf_track);

         if (assessment.canInterceptTrack())
         {
            // get weapons to commit
            auto shot_doctrine = assignment->getShotDoctrine();
            int wpns_to_commit = 1;
            if (shot_doctrine == il::SHOOT_2) // if SLS, we do one at a time
            {
               wpns_to_commit = 2;
            }

            if (weapon && assignment_local_wsf_track && weapon->FireSalvo(sim_time_s, assignment_local_wsf_track, wpns_to_commit))
            {
               am_proc->shotsFiredAssignmentStatus(*logger, assignment);
               engagement_iter->second.ResetNextFireTime();
            }
            else
            {
               HCL_ERROR_LOGGER(am_proc->getGlobalLogger()) << "Error firing salvo, CANTCO'ing assignment";
               am_proc->cantcoAssignment(*logger, assignment.Assignment(), "Weapon Failure", true, false);
            }
         }
      }
   }
}

void WsfWMSAMEngagementMod::MonitorEngagements(const double sim_time_s)
{
   auto am_proc = m_wm_component->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();
   auto wm_proc = m_wm_component->GetCoreWMRef();

   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

   auto engagement_iter = m_engagements.begin();
   while (engagement_iter != m_engagements.end())
   {
      auto& engagement = engagement_iter->second;
      auto& assignment = engagement.GetAssignment();
      auto assignment_platform = WsfBMUtils::GetAssignedPlatformFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment.Assignment()->getAssignedID());
      auto assignment_weapon = WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment.Assignment()->getAssignedID());
      auto track = assignment.Track();
      auto assignment_local_wsf_track = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID()));
      bool remove_engagement = false;
      bool haveco_success = false;

      // completed engagement?
      if (IsEngagedAgainstAssignment(assignment) && IsEngagementComplete(assignment, haveco_success))
      {
         am_proc->havecoAssignment(*logger, assignment, haveco_success);
         remove_engagement = true;
      }
      else if (assignment.Assignment()->isAssignmentComplete())
      {
         remove_engagement = true;
      }
      else if (EngagementExpired(sim_time_s, assignment))
      {
         am_proc->cantcoAssignment(*logger, assignment, "Shot Expired");
         remove_engagement = true;
      }
      else if (!AssignmentHasValidTrack(assignment))
      {
         am_proc->cantcoAssignment(*logger, assignment, "No Master Track");
         remove_engagement = true;
      }
      else if (assignment_weapon && TimeForSecondSalvo(assignment, assignment_weapon))
      {
         if (assignment_local_wsf_track && WsfBMUtils::BelievedAlive(m_wm_component->GetSimulation(), assignment_local_wsf_track))
         {
            AttemptAssignmentEngagement(sim_time_s, assignment, assignment_platform, assignment_weapon, m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID())));
         }
         else
         {
            auto confirmed_dead = WsfBMUtils::ConfirmedDead(m_wm_component->GetSimulation(), assignment_local_wsf_track);
            am_proc->havecoAssignment(*logger, assignment, confirmed_dead);
            remove_engagement = true;
         }
      }

      if (remove_engagement)
      {
         engagement_iter = m_engagements.erase(engagement_iter);
      }
      else
      {
         engagement_iter++;
      }
   }
}

bool WsfWMSAMEngagementMod::IsEngagedAgainstAssignment(const il::evaluationAssignment& assignment) const
{
   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   return eng_iter != m_engagements.end();
}

bool WsfWMSAMEngagementMod::IsEngagementComplete(const il::evaluationAssignment& assignment, bool& was_successful) const
{
   auto sim = m_wm_component->GetSimulation();

   bool engagement_complete = was_successful = false;

   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end())
   {
      int expected_qty = 1;
      if (assignment.Assignment()->getShotDoctrine() == il::SHOOT_2 || assignment.Assignment()->getShotDoctrine() == il::SHOOT_LOOK_SHOOT)
      {
         expected_qty = 2;
      }

      auto reporting_style = m_wm_component->GetCoreWMRef()->getHavecoReportingStyle();
      switch (reporting_style)
      {
         case il::WeaponsManagerInterface::E_ON_LAUNCH:
            engagement_complete = was_successful = eng_iter->second.GetShotsFiredForAssignment() >= expected_qty;
            break;
         case il::WeaponsManagerInterface::E_ON_DETONATION:
            engagement_complete = was_successful = eng_iter->second.GetShotsFiredForAssignment() >= expected_qty && eng_iter->second.GetWeaponsDetonatedForAssignment() >= expected_qty;
            break;
         case il::WeaponsManagerInterface::E_ON_KILL:
         {
            auto num_detonations = eng_iter->second.GetWeaponsDetonatedForAssignment();
            auto local_track = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(eng_iter->second.GetAssignedTrackID());
            auto confirmed_dead = WsfBMUtils::ConfirmedDead(sim, local_track);

            if ((num_detonations >= expected_qty) ||
                ((num_detonations > 0) && confirmed_dead))
            {
               engagement_complete = true;
               was_successful = confirmed_dead;
            }
         }
         break;
         default:
            HCL_FATAL_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMSAMEngagementMod::IsEngagementComplete(): invalid HAVECO reporting style: " << reporting_style << std::endl;
            throw UtException("WsfWMSAMEngagementMod::IsEngagementComplete(): invalid HAVECO reporting style");
      }
   }

   return engagement_complete;
}

int WsfWMSAMEngagementMod::ShotsFiredForEngagement(const il::evaluationAssignment& assignment) const
{
   int shots_fired = 0;
   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end())
   {
      shots_fired = eng_iter->second.GetShotsFiredForAssignment();
   }
   return shots_fired;
}

double WsfWMSAMEngagementMod::GetLastEngagementTime(const il::evaluationAssignment& assignment) const
{
   double last_engagement_time = -1000;

   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end())
   {
      auto reporting_style = m_wm_component->GetCoreWMRef()->getHavecoReportingStyle();
      switch (reporting_style)
      {
         case il::WeaponsManagerInterface::E_ON_LAUNCH:
            last_engagement_time = eng_iter->second.GetLastFireTimeForAssignment();
            break;
         case il::WeaponsManagerInterface::E_ON_DETONATION:
         case il::WeaponsManagerInterface::E_ON_KILL:
            last_engagement_time = eng_iter->second.GetLastDetonationTimeForAssignment();
            break;
         default:
            HCL_FATAL_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMSAMEngagementMod::GetLastEngagementTime(): invalid HAVECO reporting style: " << reporting_style << std::endl;
            throw UtException("WsfWMSAMEngagementMod::GetLastEngagementTime(): invalid HAVECO reporting style");
      }
   }

   return last_engagement_time;
}

double WsfWMSAMEngagementMod::GetLastFireTimeForEngagement(const il::evaluationAssignment& assignment) const
{
   double last_fire_time = -1000;

   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end())
   {
      last_fire_time = eng_iter->second.GetLastFireTimeForAssignment();
   }
   return last_fire_time;
}

bool WsfWMSAMEngagementMod::EngagementExpired(const double sim_time_s, const il::evaluationAssignment& assignment) const
{
   bool has_expired = false;

   // always look at active shots because if we have active missiles, don't cancel it mid-shot
   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end() && eng_iter->second.GetActiveWeaponsForAssignment() == 0 &&
       sim_time_s >= eng_iter->second.GetCommandedTime() + assignment.Assignment()->getMaxFiringTime())
   {
      has_expired = true;
   }

   return has_expired;
}

bool WsfWMSAMEngagementMod::AssignmentHasValidTrack(const il::evaluationAssignment& eval_assignment) const
{
   bool has_track = true;

   auto am_proc = m_wm_component->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();
   auto wm_proc = m_wm_component->GetCoreWMRef();

   std::shared_ptr<il::assignmentMessage> assignment = eval_assignment.Assignment();
   auto track = eval_assignment.Track();

   if (!am_proc->getMasterTrack(track->getID()) ||
       !m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID())))
   {
      has_track = false;
   }

   return has_track;
}

// This method is only used to determine if it is time to fire the second salvo when the shot doctrine is Shoot-Look-Shoot
bool WsfWMSAMEngagementMod::TimeForSecondSalvo(const il::evaluationAssignment& assignment, WsfWeapon* weapon) const
{
   bool time_for_second_shot = false;

   auto eng_iter = m_engagements.find(assignment.Track()->getID());
   if (eng_iter != m_engagements.end() &&
       eng_iter->second.GetShotsFiredForAssignment() == 1 &&
       assignment->getShotDoctrine() == il::SHOOT_LOOK_SHOOT &&
       weapon->WeaponsActiveAndPendingFor(WsfBMUtils::ConvertTrackId(assignment.Track()->getID())) == 0 &&
       !eng_iter->second.IsNextFireTimeSet()) // not currently awaiting for a specific shot time
   {
      time_for_second_shot = true;
   }

   return time_for_second_shot;
}
