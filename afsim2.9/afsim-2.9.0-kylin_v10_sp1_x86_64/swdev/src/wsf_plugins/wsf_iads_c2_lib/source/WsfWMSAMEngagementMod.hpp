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
*   File: WsfWMSAMEngagementMod.hpp
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

#ifndef WSF_WM_SAM_ENGAGEMENT_MOD_HPP
#define WSF_WM_SAM_ENGAGEMENT_MOD_HPP

#include <map>

#include <memory>

#include <UtInput.hpp>
#include <WsfSimulation.hpp>
#include <WsfBehaviorTree.hpp>
#include <WsfTrackId.hpp>

#include <WsfBMUtils.hpp>
#include <WsfInterceptCalc.hpp>

#include <iadsLib/weaponsManagerEngageInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/assessmentRecord.hpp>

class WsfBMWeaponsManager;
class WsfSimulation;

class WsfWMSAMEngagementMod : public il::WeaponsManagerEngagementInterface
{
   public:
      WsfWMSAMEngagementMod(const WsfScenario& aScenario, WsfBMWeaponsManager* wm_component);
      WsfWMSAMEngagementMod(const WsfWMSAMEngagementMod& from);

      bool Initialize(const double sim_time);
      bool Initialize2(const double sim_time);
      bool ProcessInput(UtInput& aInput);

      virtual void Run(const double sim_time_s);

   protected:

      WsfBMWeaponsManager* m_wm_component;
      WsfBehaviorTree m_behavior_tree;
      bool m_has_behavior_tree;
      typedef std::map <il::idRecord, std::shared_ptr<WsfInterceptCalculator>> tScriptInterceptCache;
      tScriptInterceptCache m_script_calculators;
      std::shared_ptr<WsfInterceptCalculator> GetScriptCalculator(const il::evaluationAssignment& assignment);

      virtual void RunDefaultEngagement(const double sim_time_s);
      virtual bool AttemptAssignmentEngagement(const double sim_time_s, il::evaluationAssignment& assignment, WsfPlatform* owning_plat, WsfWeapon* weapon, WsfLocalTrack* track);
      virtual il::assessmentRecord AssessEngagement(const double sim_time_s, il::evaluationAssignment& assignment, WsfPlatform* owning_plat, WsfWeapon* weapon, WsfLocalTrack* track, const double assignment_delay_time_s = 0, const double expected_execution_delay_s = 0, const bool project = false);
      virtual void ExecuteEngagements(const double sim_time_s);
      virtual void MonitorEngagements(const double sim_time_s);
      virtual bool IsEngagedAgainstAssignment(const il::evaluationAssignment& assignment) const;
      virtual bool IsEngagementComplete(const il::evaluationAssignment& assignment, bool& was_successful) const;
      virtual int ShotsFiredForEngagement(const il::evaluationAssignment& assignment) const;
      virtual double GetLastEngagementTime(const il::evaluationAssignment& assignment) const;
      virtual double GetLastFireTimeForEngagement(const il::evaluationAssignment& assignment) const;
      virtual bool EngagementExpired(const double sim_time_s, const il::evaluationAssignment& assignment) const;
      virtual bool AssignmentHasValidTrack(const il::evaluationAssignment& eval_assignment) const;
      virtual bool TimeForSecondSalvo(const il::evaluationAssignment& assignment, WsfWeapon* weapon) const;

   private:

      class PendingEngagement
      {
         public:
            PendingEngagement(const double& sim_time, il::evaluationAssignment& assignment, WsfSimulation* sim, const il::assessmentRecord& assessment);

            int GetShotsFiredForAssignment() const;
            int GetWeaponsDetonatedForAssignment() const;
            double GetLastFireTimeForAssignment() const;
            double GetLastDetonationTimeForAssignment() const;

            int GetActiveWeaponsForAssignment() const;
            double GetCommandedTime() const;
            il::evaluationAssignment& GetAssignment();

            bool IsNextFireTimeSet() const;
            double GetNextFireTime() const;
            void SetNextFireTime(const double time_s);
            void ResetNextFireTime();

            WsfTrackId GetAssignedTrackID() const;
         private:
            WsfWeapon* GetAssignedWeapon() const;

            double m_command_time;

            bool   m_next_fire_time_set;
            double m_next_fire_time;

            il::evaluationAssignment m_assignment;

            WsfSimulation* m_sim_ptr;
            il::assessmentRecord m_assessment;
            // upon creation of the engagement, we record the existing counts for the
            // weapon against the track since we can't reset these within AFSIM and it
            // retains counts across the entire simulation run for each engagement, we
            // bookkeep these ourselves
            int m_wpn_initial_rounds_fired_at;
            int m_wpn_initial_rounds_completed_for;
            int m_wpn_initial_weapons_active_and_pending_for;
      };

      typedef il::idRecord tEngagementTrackId;
      std::map<tEngagementTrackId, PendingEngagement> m_engagements;
};

#endif
