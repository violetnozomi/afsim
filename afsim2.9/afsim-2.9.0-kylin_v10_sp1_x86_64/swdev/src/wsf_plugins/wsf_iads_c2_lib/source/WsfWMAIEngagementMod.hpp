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
*   File: WsfWMAIEngagementMod.hpp
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

#ifndef WSF_WM_AI_ENGAGEMENT_MOD_HPP
#define WSF_WM_AI_ENGAGEMENT_MOD_HPP

#include <map>

#include <memory>
#include <stack>

#include <WsfESM_Sensor.hpp>
#include <UtInput.hpp>
#include <WsfSimulation.hpp>
#include <WsfBehaviorTree.hpp>
#include <WsfTrackId.hpp>
#include <WsfStringId.hpp>

#include <WsfBMUtils.hpp>
#include <WsfInterceptCalc.hpp>

#include <iadsLib/weaponsManagerEngageInterface.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/assessmentRecord.hpp>

class WsfWeaponsManagerAI;
class WsfSimulation;

namespace il
{
class WeaponsManagerEngagementInterface;
}

class WsfWMAIEngagementMod : public il::WeaponsManagerEngagementInterface
{
   public:
      WsfWMAIEngagementMod(const WsfScenario& aScenario, WsfBMWeaponsManager* wm_component);
      WsfWMAIEngagementMod(const WsfWMAIEngagementMod& from);

      bool Initialize(const double sim_time);
      bool Initialize2(const double sim_time);
      bool ProcessInput(UtInput& aInput);

      void Run(const double sim_time_s) override;

   protected:

      class Behavior
      {
         public:
            enum BehaviorType
            {
               UNKNOWN,
               BEAM,
               DRAG,
               TAKEACTION,
               INTERCEPT,
               PURSUE
            };

            virtual void Perform(const double sim_time_s);
            virtual bool isFinished(const double sim_time_s);
            virtual BehaviorType getType() const = 0;
            const il::idRecord getLocalTrackId() const { return m_local_track_id; }

            static std::string getBehaviorTypeString(const BehaviorType type);
            static BehaviorType getBehaviorTypeFromString(const std::string type);
         protected:
            Behavior(WsfWMAIEngagementMod* wm_em, il::idRecord local_track_id);
            WsfWMAIEngagementMod* m_wm_em;
            il::idRecord m_local_track_id;
            static const std::string UnknownString, BeamString, DragString, TakeActionString, InterceptString, PursueString;

         private:
            Behavior(const Behavior& other); // = delete;
      };

      class Beam : public Behavior
      {
         public:
            Beam(const il::idRecord& local_track_id, WsfWMAIEngagementMod* wm_em, const double sim_time_s);
            void Perform(const double sim_time_s) override;
            bool isFinished(const double sim_time_s) override;
            BehaviorType getType() const override { return BEAM; }
            void setLocalTrackId(const il::idRecord& local_track_id) { m_local_track_id = local_track_id; }
            bool BeamTimeElapsed(const double sim_time_s) const { return sim_time_s >= m_end_beam_s; }
         private:
            double m_start_beam_s;
            double m_end_beam_s;
      };

      class Drag : public Behavior
      {
         public:
            Drag(const il::idRecord& local_track, WsfWMAIEngagementMod* wm_em);
            void Perform(const double sim_time_s) override;
            bool isFinished(const double sim_time_s) override;
            BehaviorType getType() const override { return DRAG; }
            void setLocalTrackId(const il::idRecord& local_track_id) { m_local_track_id = local_track_id; }
      };

      class Intercept : public Behavior
      {
         public:
            Intercept(const il::evaluationAssignment& assignment, WsfWMAIEngagementMod* wm_em);
            void Perform(const double sim_time_s) override;
            bool isFinished(const double sim_time_s) override;
            BehaviorType getType() const override { return INTERCEPT; }

            const il::idRecord getAssignedId() const { return m_assigned_id; }
            bool hasGeneratedAction() const { return m_has_generated_action; }
            void setHasGeneratedAction() { m_has_generated_action = true; }

         private:
            il::idRecord m_assigned_id;
            bool m_has_generated_action;
      };

      class Pursue : public Behavior
      {
         public:
            Pursue(const il::evaluationAssignment& assignment, const bool generated_action, WsfWMAIEngagementMod* wm_em);
            void Perform(const double sim_time_s) override;
            bool isFinished(const double sim_time_s) override;
            BehaviorType getType() const override { return PURSUE; }

            const il::idRecord getAssignedId() const { return m_assigned_id; }
            bool hasGeneratedAction() const { return m_has_generated_action; }
            void setHasGeneratedAction() { m_has_generated_action = true; }

         private:
            il::idRecord m_assigned_id;
            bool m_has_generated_action;
      };

      class TakeAction : public Behavior
      {
         public:
            TakeAction(const il::idRecord& assignmentLocalTrackId, const il::idRecord& assignmentAssignedId, const il::eTakeAction& actionType, const double simTime, const double actionDuration, WsfWMAIEngagementMod* wm_em);
            void Perform(const double sim_time_s) override;
            bool isFinished(const double sim_time_s) override;
            BehaviorType getType() const override { return TAKEACTION; }

         private:
            il::idRecord m_assignment_local_track_id;
            il::idRecord m_assignment_assigned_id;
            il::eTakeAction m_action_type;
            double m_action_duration_s;
            double m_action_complete_time_s;
            TakeAction(const TakeAction& other); // = delete;
      };

      WsfBMWeaponsManager* m_wm_component;
      WsfBehaviorTree m_behavior_tree;
      bool m_has_behavior_tree;
      using tScriptInterceptCache = std::map <il::idRecord, std::shared_ptr<WsfInterceptCalculator>>;
      tScriptInterceptCache m_script_calculators;
      std::shared_ptr<WsfInterceptCalculator> GetScriptCalculator(const il::evaluationAssignment& assignment);

      virtual void RunDefaultEngagement(const double sim_time_s);
      void AddBehaviorsToStack(const double sim_time_s);

      virtual il::assessmentRecord AssessEngagement(const double sim_time_s, il::evaluationAssignment& assignment, const std::shared_ptr<il::weaponRecord> weapon, const double delay_time_s = 0);

      virtual bool IsEngagementComplete(const il::evaluationAssignment& assignment, bool& was_successful) const;
      virtual bool AssignmentHasValidTrack(const il::evaluationAssignment& assignment) const;
      virtual bool TimeForSecondSalvo(const il::evaluationAssignment& assignment, WsfWeapon* weapon, const double sim_time_s) const;

   private:
      std::stack<std::shared_ptr<Behavior>> m_behavior_stack;
      void Push(std::shared_ptr<Behavior> behavior);
      std::shared_ptr<Behavior> Top();
      void Pop();
      bool Empty();

      WsfTrackList RWRDetectedThreatList() const;
      int RWRDetectedThreatCount() const;
      WsfTrackList RWRDetectedThreatsInResponseRange(const double sim_time_s);
      int RWRDetectedThreatCountInResponseRange(const double sim_time_s);
      WsfStringId RWRNameId() const;

      WsfLocalTrack* RWRHighPriorityThreat(const double sim_time_s);
      bool RWRResponse() const;
      double LastLockTime() const;
      WsfLocalTrack* FindCorrelatedTrack(const WsfTrackId& raw_track_id) const;

      void LogDebugInfo(const double sim_time_s);
      void AssessResponse(il::moeLoggingInterface& aMOELogger, const double sim_time_s);
      bool AssessBeamResponse(il::moeLoggingInterface& aMOELogger, const double sim_time_s, const il::idRecord& attacker_unit_id, WsfSpatialDomain domain);
      bool AttackerEngagedOrBelievedDead(const il::evaluationAssignment& attacker) const;
      bool AttackerEngaged(const il::evaluationAssignment& attacker) const;
      bool AttackerEngaged(const il::idRecord& attacker_local_track_id) const;
      bool AddEvadeBehavior(il::moeLoggingInterface& aMOELogger, const double sim_time_s);
      bool AddEvasionBehavior(const double sim_time_s);
      double getRWRResponseBeamDuration() const;
      void BeamResponse(const double sim_time_s, const il::idRecord& attacker_unit_id);
      bool AddTakeActionBehavior(const double sim_time_s);
      double getDistanceToTrack(const il::idRecord& local_track_id, const double sim_time_s);
      double getGroundRangeToTrack(const il::idRecord& local_track_id, const double sim_time_s);
      void getGroundRangeAndGroundRangeDA(const il::trackRecord* track, const double sim_time_s, double& ground_range, double& ground_range_DA);

      void Egress(const double sim_time_s);
      void Engage(const double sim_time_s, const il::evaluationAssignment& assignment);

      WsfGeoPoint setMoveInDirection(const vm::Vector3<double> velocity, const double sim_time_s, const double move_time_s);

      bool InsideWEZRange(double rAero, double rMax1, double rMax2, double rMin, double range);
      bool AssessAction(const double sim_time_s, const il::idRecord& assignment, il::eTakeAction& actionType, double& actionDuration);
      void CompleteAction();
      void PerformSelfDefense();
      void CANTCOAllAssignments(const std::string& reason);

      bool EmployingActionTechniques();
      bool isParked();
      bool TTRLock();
      bool TTRLock(const il::idRecord& attacker_unit_id);
      bool Evading();
      bool TakingAction();
      bool m_is_scrambled;

      il::idRecord m_current_assignment_local_track_id;
      il::idRecord m_current_assignment_assigned_id;
};

#endif
