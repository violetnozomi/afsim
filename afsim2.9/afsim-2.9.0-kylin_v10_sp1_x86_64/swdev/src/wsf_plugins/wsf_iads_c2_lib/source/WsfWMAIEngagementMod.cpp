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
*   File: WsfWMAIEngagementMod.cpp
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

#include <cmath>
#include <WsfWeapon.hpp>
#include <WsfLocalTrack.hpp>
#include <WsfAirMover.hpp>
#include <WsfLaunchComputer.hpp>

#include <WsfWMAIEngagementMod.hpp>
#include <WsfBMUtils.hpp>
#include <WsfBMMOELogger.hpp>
#include <logger.hpp>

#include <WsfBMWeaponsManager.hpp>
#include <WsfWeaponsManagerAI.hpp>
#include <WsfBMSensorInfoRecord.hpp>
#include <ZoneUtils.hpp>
#include <iadsLib/NumericUtils.hpp>
#include <iadsLib/weaponsManagerAI.hpp>
#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/util.hpp>

//////////////
// Behavior //
//////////////

const std::string WsfWMAIEngagementMod::Behavior::BeamString = "Beam";
const std::string WsfWMAIEngagementMod::Behavior::DragString = "Drag";
const std::string WsfWMAIEngagementMod::Behavior::TakeActionString = "Take_Action";
const std::string WsfWMAIEngagementMod::Behavior::InterceptString = "Intercept";
const std::string WsfWMAIEngagementMod::Behavior::PursueString = "Pursue";
const std::string WsfWMAIEngagementMod::Behavior::UnknownString = "Unknown";

WsfWMAIEngagementMod::Behavior::Behavior(WsfWMAIEngagementMod* wm_em, il::idRecord local_track_id)
   : m_wm_em(wm_em)
   , m_local_track_id(local_track_id)
{
}

void WsfWMAIEngagementMod::Behavior::Perform(const double sim_time_s)
{
   HCL_FATAL_LOGGER(m_wm_em->m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMAIEngagementMod::Behavior::Perform() unimplemented in base class -- override for your specific Behavior";
}

bool WsfWMAIEngagementMod::Behavior::isFinished(const double sim_time_s)
{
   HCL_FATAL_LOGGER(m_wm_em->m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMAIEngagementMod::Behavior::Finished() unimplemented in base class -- override for your specific Behavior";
   return true;
}

std::string WsfWMAIEngagementMod::Behavior::getBehaviorTypeString(const BehaviorType type)
{
   switch (type)
   {
      case BEAM:
         return BeamString;
      case DRAG:
         return DragString;
      case TAKEACTION:
         return TakeActionString;
      case INTERCEPT:
         return InterceptString;
      case PURSUE:
         return PursueString;
      default:
         return UnknownString;
   }
}

WsfWMAIEngagementMod::Behavior::BehaviorType WsfWMAIEngagementMod::Behavior::getBehaviorTypeFromString(const std::string type)
{
   if (type == BeamString)
   {
      return BEAM;
   }
   else if (type == DragString)
   {
      return DRAG;
   }
   else if (type == TakeActionString)
   {
      return TAKEACTION;
   }
   else if (type == InterceptString)
   {
      return INTERCEPT;
   }
   else if (type == PursueString)
   {
      return PURSUE;
   }
   return UNKNOWN;
}

WsfWMAIEngagementMod::Intercept::Intercept(const il::evaluationAssignment& assignment, WsfWMAIEngagementMod* wm_em)
   : Behavior(wm_em, assignment->getLocalTrackID())
   , m_assigned_id(assignment->getAssignedID())
   , m_has_generated_action(false)
{
}

void WsfWMAIEngagementMod::Intercept::Perform(const double sim_time_s)
{
   auto am = m_wm_em->m_wm_component->GetCoreAMRef();
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get());

   auto masterTrack = il::FindTrack(m_local_track_id, am->GetMasterTracksArray());
   if (!masterTrack)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << am->getMyAsset().getName() << " attempted to perform AI Intercept, but could not find master track " << m_local_track_id;
      return;
   }

   // Attempt to calculate the intercept time & position
   double interceptTime;
   bool hasValidIntercept = il::CalculateInterceptTime(sim_time_s, am->getMyAsset().getPosition(), *masterTrack.get(), wm->getInterceptSpeed(), interceptTime);
   // If no valid intercept, fly pure pursuit
   // NH - Check the altitude after this call to getPropagatedPosition to make sure a WSF-side (as opposed to iadsLib) function is not required.
   il::positionRecord interceptPoint = masterTrack->getPropagatedPosition(hasValidIntercept ? interceptTime : sim_time_s);
   // NH - Currently using target altitude with no projection

   auto mover = dynamic_cast<WsfAirMover*>(m_wm_em->m_wm_component->GetPlatform()->GetMover());
   if (mover)
   {
      mover->GoToLocation(sim_time_s, interceptPoint.getLatDegs(), interceptPoint.getLonDegs(), masterTrack->getPosition().getAltM());
      mover->GoToSpeed(sim_time_s, std::min(wm->getInterceptSpeed(), mover->GetConstraints().mMaxSpeed), mover->GetConstraints().mMaxLinearAccel, true);
   }
   else
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << am->getMyAsset().getName() << " attempted to perform AI Intercept, but could not get WsfAirMover.";
   }
}

bool WsfWMAIEngagementMod::Intercept::isFinished(const double sim_time_s)
{
   // Return true if the track that the behavior was based on no longer appears in the master track list.
   if (!m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId()))
   {
      return true;
   }

   std::shared_ptr<il::AssetManagerInterface> am = m_wm_em->m_wm_component->GetCoreAMRef();
   il::evaluationTrack null_track(am->getGlobalLogger());
   il::evaluationAssignment eval_assign(null_track, nullptr);
   if (!am->getEvaluationAssignment(m_local_track_id, m_assigned_id, eval_assign))
   {
      return true;
   }

   // Calculate ground range between AI and target (propagated to current time)
   auto thisAssetPosition = am->getThisAsset()->getPosition();
   auto masterTrack = il::FindTrack(m_local_track_id, am->GetMasterTracksArray());
   if (!masterTrack)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << am->getMyAsset().getName() << " has assignment, but could not find master track " << m_local_track_id << " to determine if Intercept behavior is finished";
      return false;
   }
   auto propagatedPosition = masterTrack->getPropagatedPosition(sim_time_s);

   WsfGeoPoint assetPosition, targetPosition;
   assetPosition.SetLocationLLA(thisAssetPosition.getLatDegs(), thisAssetPosition.getLonDegs(), thisAssetPosition.getAltM());
   targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());

   return assetPosition.GroundRangeTo(targetPosition) < static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get())->getPursuitRange();
}

WsfWMAIEngagementMod::Pursue::Pursue(const il::evaluationAssignment& assignment, const bool generated_action, WsfWMAIEngagementMod* wm_em)
   : Behavior(wm_em, assignment->getLocalTrackID())
   , m_assigned_id(assignment->getAssignedID())
   , m_has_generated_action(generated_action)
{
}

void WsfWMAIEngagementMod::Pursue::Perform(const double sim_time_s)
{
   auto am = m_wm_em->m_wm_component->GetCoreAMRef();
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get());
   auto asset = am->getThisAsset();
   auto weapon = asset->getWeapon(m_assigned_id);

   // Move directly to where we think the target currently is
   auto track = il::FindTrack(m_local_track_id, am->GetMasterTracksArray());
   if (!track)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << asset->getName() << " attempted to perform AI Pursue, but could not find master track " << m_local_track_id;
      return;
   }
   auto position = track->getPropagatedPosition(sim_time_s);

   // Move at WMAI configured interceptSpeed, unless within configured minimum range of weapon, in which case, move at 90% of target speed
   double speed = wm->getInterceptSpeed();
   if (weapon->HasMinRangeAttribute() && position.calculateDistanceMeters(asset->getPosition()) < weapon->GetMinRange())
   {
      speed = 0.9 * track->getVelocity();
   }

   auto mover = dynamic_cast<WsfAirMover*>(m_wm_em->m_wm_component->GetPlatform()->GetMover());
   if (mover)
   {
      mover->GoToLocation(sim_time_s, position.getLatDegs(), position.getLonDegs(), track->getPosition().getAltM());
      mover->GoToSpeed(sim_time_s, std::min(speed, mover->GetConstraints().mMaxSpeed), mover->GetConstraints().mMaxLinearAccel, true);
   }
   else
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << asset->getName() << " attempted to perform AI Pursue, but could not get WsfAirMover.";
   }
}

bool WsfWMAIEngagementMod::Pursue::isFinished(const double sim_time_s)
{
   // Return true if the track that the behavior was based on no longer appears in the master track list.
   if (!m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId()))
   {
      return true;
   }

   auto am = m_wm_em->m_wm_component->GetCoreAMRef();
   il::evaluationTrack null_track(am->getGlobalLogger());
   il::evaluationAssignment eval_assign(null_track, nullptr);
   auto has_this_assignment = am->getEvaluationAssignment(m_local_track_id, m_assigned_id, eval_assign);
   return !has_this_assignment;
}

WsfWMAIEngagementMod::TakeAction::TakeAction(const il::idRecord& assignmentLocalTrackId, const il::idRecord& assignmentAssignedId, const il::eTakeAction& actionType, const double sim_time_s, const double actionDuration, WsfWMAIEngagementMod* wm_em)
   : Behavior(wm_em, assignmentLocalTrackId),
     m_assignment_local_track_id(assignmentLocalTrackId),
     m_assignment_assigned_id(assignmentAssignedId),
     m_action_type(actionType),
     m_action_duration_s(actionDuration),
     m_action_complete_time_s(sim_time_s)
{
   m_action_complete_time_s += actionDuration;
}

// Directs the platform to "take action" in relation to a target by continuing straight, breaking left, or breaking right at the highest speed allowed given
// the direction/destination of the action.
void WsfWMAIEngagementMod::TakeAction::Perform(const double sim_time_s)
{
   // Action to put target on my "beam"
   vm::Vector3<double> myLocation = m_wm_em->m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();
   auto targetLocation = m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(m_assignment_local_track_id)->getPosition().getXYZ();
   auto rel = myLocation - targetLocation;

   vm::Vector3<double> action_direction;

   // Action is straight ahead?
   if (m_action_type == il::eTakeAction::STRAIGHT)
   {
      return;
   }

   // Action is move right of threat's velocity (my LEFT in head-on)?
   else if (m_action_type == il::eTakeAction::BREAK_LEFT)
   {
      // Head 90 deg right of threat's velocity on a horizontal plane
      action_direction = rel.cross(myLocation);
   }

   // Action is move left of threat's velcoity (my RIGHT in head-on)?
   else if (m_action_type == il::eTakeAction::BREAK_RIGHT)
   {
      // Head 90 deg left of threat's velocity on a horizontal plane
      action_direction = myLocation.cross(rel);
   }

   // Calculate the velocity vector for moving in the determined direction at our maximum speed.
   action_direction.normalize();
   auto mover = dynamic_cast<WsfAirMover*>(m_wm_em->m_wm_component->GetPlatform()->GetMover());
   vm::Vector3<double> action_velocity = action_direction * mover->GetConstraints().mMaxSpeed;

   // Direct our platform to move in the chosen direction for the appropriate action duration.
   // (Store projected destination in a WsfGeoPoint for use in calculating maximum possible speed for this action).
   WsfGeoPoint destination = m_wm_em->setMoveInDirection(action_velocity, sim_time_s, m_action_duration_s);

   // Direct our platform to move at the highest speed allowed given our destination.
   double corner_velocity = mover->CornerVelocity(&destination);
   mover->GoToSpeed(sim_time_s, std::min(1.1 * corner_velocity, mover->GetConstraints().mMaxSpeed), mover->GetConstraints().mMaxLinearAccel, true);

   return;
}

// Returns true if (1) enough sim time has passed for TakeAction Behavior to complete or
// (2) the track that the behavior was based on no longer appears in the master track list.
bool WsfWMAIEngagementMod::TakeAction::isFinished(const double sim_time_s)
{
   return (sim_time_s >= m_action_complete_time_s) || !m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId());
}

WsfWMAIEngagementMod::Beam::Beam(const il::idRecord& local_track_id, WsfWMAIEngagementMod* wm_em, const double sim_time_s)
   : Behavior(wm_em, local_track_id),
     m_start_beam_s(sim_time_s),
     m_end_beam_s(sim_time_s)

{
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get());
   m_end_beam_s += wm->getEvasionRWRResponseBeamDuration();
}

void WsfWMAIEngagementMod::Beam::Perform(const double sim_time_s)
{
   // Update direction for beam response relative to this RespondBeam's attacker. Calculate end time of beam response and
   // store it in *m_wm_em as m_beam_end_time_s
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get());

   // Having been tracked by either a SAM or AI tracking radar, respond by a beam maneuver.
   auto myLocation = m_wm_em->m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();
   auto attackerLocation = m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId())->getPosition().getXYZ();

   // Try to put the attacker on my "beam"
   auto rel = myLocation - attackerLocation;

   // Calculate a vector perpendicular to the threat
   auto tz = m_wm_em->m_wm_component->GetCoreAMRef()->getThisAsset()->getVelocityXYZ().cross(rel);
   tz.normalize();
   auto uPos = m_wm_em->m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();
   uPos.normalize();

   auto tz_vec3 = UtVec3d(tz.getX(), tz.getY(), tz.getZ());
   auto uPos_vec3 = UtVec3d(uPos.getX(), uPos.getY(), uPos.getZ());

   auto cosAngle = tz_vec3.AngleWith(uPos_vec3);

   vm::Vector3<double> beam_response_direction;
   if (cosAngle < 0.0)
   {
      // Head 90 degrees left of threat's velocity on a horizontal plane.
      beam_response_direction = myLocation.cross(rel);
   }
   else
   {
      // Head 90 degrees right of threat's velocity on a horizontal plane.
      beam_response_direction = rel.cross(myLocation);
   }

   // Calculate the velocity vector for moving in the determined direction at our maximum speed.
   beam_response_direction.normalize();
   auto mover = dynamic_cast<WsfAirMover*>(m_wm_em->m_wm_component->GetPlatform()->GetMover());
   vm::Vector3<double> beam_response_velocity = beam_response_direction * mover->GetConstraints().mMaxSpeed;

   // Direct our platform to move in the chosen direction for the appropriate action duration.
   // (Store projected destination in a WsfGeoPoint for use in calculating maximum possible speed for this action).
   WsfGeoPoint destination = m_wm_em->setMoveInDirection(beam_response_velocity, sim_time_s, wm->getEvasionRWRResponseBeamDuration());

   // Direct our platform to move at the highest speed allowed given our destination.
   double corner_velocity = mover->CornerVelocity(&destination);
   mover->GoToSpeed(sim_time_s, std::min(1.1 * corner_velocity, mover->GetConstraints().mMaxSpeed), mover->GetConstraints().mMaxLinearAccel, true);
}

bool WsfWMAIEngagementMod::Beam::isFinished(const double sim_time_s)
{
   // Return true if either (1) beam time has elapsed or (2) the track that the behavior was based on no longer appears in the master track list.
   return BeamTimeElapsed(sim_time_s) || !m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId());
}

// Having been tracked by either a SAM or AI tracking radar AND having responded by a beam maneuver AND still being tracked,
// respond by running away from the attacker using a drag maneuver.
WsfWMAIEngagementMod::Drag::Drag(const il::idRecord& local_track_id, WsfWMAIEngagementMod* wm_em)
   : Behavior(wm_em, local_track_id)
{
}

void WsfWMAIEngagementMod::Drag::Perform(const double sim_time_s)
{
   auto myLocation = m_wm_em->m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();

   auto attacker = m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(m_local_track_id);
   auto attackerLocation = attacker->getPosition().getXYZ();
   auto attackerSpeed = attacker->getVelocity();

   auto mover = dynamic_cast<WsfAirMover*>(m_wm_em->m_wm_component->GetPlatform()->GetMover());

   // Attacker is moving?
   if (attackerSpeed > 0.1)
   {
      // Maneuver aircraft along the attackers's [Helios: missile's] velocity vector
      vm::Vector3<double> drag_response_direction = attacker->getVelocityVector();
      drag_response_direction.normalize();
      vm::Vector3<double> drag_response_velocity = drag_response_direction * mover->GetConstraints().mMaxSpeed;
      m_wm_em->setMoveInDirection(drag_response_velocity, sim_time_s, 10);
   }
   else
   {
      // Maneuver aircraft in opposite direction of vector between me and attacker
      vm::Vector3<double> drag_response_direction = myLocation - attacker->getPosition().getXYZ();
      drag_response_direction.normalize();
      vm::Vector3<double> drag_response_velocity = drag_response_direction * mover->GetConstraints().mMaxSpeed;
      m_wm_em->setMoveInDirection(drag_response_velocity, sim_time_s, 10);
   }

   // Command speed to be max speed
   mover->GoToSpeed(sim_time_s, mover->GetConstraints().mMaxSpeed, mover->GetConstraints().mMaxLinearAccel, true);

   m_wm_em->CANTCOAllAssignments("Performing drag response");
}

// Returns true if either: (1) the track that the behavior was based on no longer appears in the master track list,
// (2) the RWR's active track list contains no tracks on this RespondDrag's target, or (3) enough time has elapsed since the last
// update for each of the RWR's tracks on the target that we believe its lock on us has been broken.
bool WsfWMAIEngagementMod::Drag::isFinished(const double sim_time_s)
{
   // Return true if the track that the behavior was based on no longer appears in the master track list.
   if (!m_wm_em->m_wm_component->GetCoreAMRef()->getMasterTrack(getLocalTrackId()))
   {
      return true;
   }

   int recently_updated_tracks = 0;

   WsfTrackManager& track_manager = m_wm_em->m_wm_component->GetPlatform()->GetTrackManager();
   WsfLocalTrack* updated_local_track = track_manager.FindTrack(WsfBMUtils::ConvertTrackId(getLocalTrackId()));

   // Get all raw tracks making up local track
   auto fused_tracks = updated_local_track->GetRawTrackIds();

   // For each raw track, check if the RWR is its originating sensor
   for (int i = 0, count = fused_tracks.GetCount(); i != count; ++i)
   {
      WsfTrack* raw_track = track_manager.FindRawTrack(*(fused_tracks.GetEntry(i)));
      if (raw_track->GetSensorNameId() == m_wm_em->RWRNameId())
      {
         // If so, check if enough time has elapsed since the raw track's last update time that we should consider the lock to be broken
         auto weapons_manager = static_cast<il::weaponsManagerAI*>(m_wm_em->m_wm_component->GetCoreWMRef().get());
         if (sim_time_s <= raw_track->GetUpdateTime() + weapons_manager->getEvasionRWRResponseRestoreAfterBreakLock())
         {
            ++recently_updated_tracks;
         }
      }
   }
   return (recently_updated_tracks == 0);
}

///////////////////////////
// WsfWMAIEngagementMod //
///////////////////////////
WsfWMAIEngagementMod::WsfWMAIEngagementMod(const WsfScenario& aScenario, WsfBMWeaponsManager* wm_component)
   : m_wm_component(wm_component)
   , m_behavior_tree(aScenario)
   , m_has_behavior_tree(false)
{
}

WsfWMAIEngagementMod::WsfWMAIEngagementMod(const WsfWMAIEngagementMod& from)
   : m_wm_component(from.m_wm_component)
   , m_behavior_tree(from.m_behavior_tree)
   , m_has_behavior_tree(from.m_has_behavior_tree)
{
}

bool WsfWMAIEngagementMod::Initialize(const double sim_time)
{
   bool success = true;

   success &= m_behavior_tree.Initialize(sim_time, m_wm_component, &m_wm_component->GetScriptContext());
   return success;
}

bool WsfWMAIEngagementMod::Initialize2(const double sim_time)
{
   bool success = true;

   if (!m_has_behavior_tree)
   {
      HCL_INFO_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Weapons Manager AI initialized without behavior tree, using default HELIOS implementation.";
   }
   else
   {
      HCL_INFO_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Weapons Manager AI initialized with behavior tree.";
   }
   return success;
}

bool WsfWMAIEngagementMod::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (m_behavior_tree.ProcessInput(aInput))
   {
      m_has_behavior_tree = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void WsfWMAIEngagementMod::Push(std::shared_ptr<Behavior> behavior)
{
   m_behavior_stack.push(behavior);
}

std::shared_ptr<WsfWMAIEngagementMod::Behavior> WsfWMAIEngagementMod::Top()
{
   return m_behavior_stack.top();
}

void WsfWMAIEngagementMod::Pop()
{
   m_behavior_stack.pop();
}

bool WsfWMAIEngagementMod::Empty()
{
   return m_behavior_stack.empty();
}

void WsfWMAIEngagementMod::Run(const double sim_time_s)
{
   if (m_has_behavior_tree)
   {
      m_behavior_tree.Execute(sim_time_s);
   }
   else
   {
      RunDefaultEngagement(sim_time_s);
   } // execute default implementation
}

void WsfWMAIEngagementMod::RunDefaultEngagement(const double sim_time_s)
{
   AddBehaviorsToStack(sim_time_s);

   // Remove finished Behaviors
   while (!Empty() && Top()->isFinished(sim_time_s))
   {
      Pop();
      if (Empty())
      {
         Egress(sim_time_s);
      }
   }

   // Perform top unfinished Behavior
   if (!Empty())
   {
      Top()->Perform(sim_time_s);
   }
}

// Assess and add behaviors to the stack to be performed
void WsfWMAIEngagementMod::AddBehaviorsToStack(const double sim_time_s)
{
   il::weaponsManagerAI* wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());

   if (wm->getDebug())
   {
      LogDebugInfo(sim_time_s);
   }

   if (AddEvasionBehavior(sim_time_s))
   {
      return;
   }

   // (Helios appears to only allow TakeAction to occur once: the Helios equivalent at this point would check that TakeAction has NOT previously
   // occurred before calling AddTakeActionBehavior. We believe that this is a bug in Helios, so we're not keeping track
   // of whether a TakeAction has occurred or checking for a previous TakeAction here. Instead, TakeAction Behaviors are generated by Intercept [and Pursue?]
   // Behaviors: we limit the number of TakeAction Behaviors performed by allowing each Intercept-Pursue chain to generate only one TakeAction.
   // This happens within AddTakeActionBehavior(sim_time_s)).
   if (!Evading() && !TakingAction() && EmployingActionTechniques())
   {
      // Calls AssessAction to determine if a TakeAction Behavior is appropriate. Adds TakeAction Behavior to the behavior stack.
      if (AddTakeActionBehavior(sim_time_s))
      {
         return;
      }
   }

   // If we're not evading and we are taking action, and if it's time for the action to finish, the equivalent of
   // Helios's CompleteAction is the only thing that should happen on this loop. The equivalent of CompleteAction will
   // happen during PerformBehaviorOnStack when TakeAction::isFinished() is called on the TakeAction at the top of the stack.
   if (!Evading() && TakingAction())
   {
      auto topTakeAction = static_cast<TakeAction*>(Top().get());
      if (topTakeAction->isFinished(sim_time_s))
      {
         return;
      }
   }

   // Self defense check is performed in WsfDefaultWeaponsManagerImpl::on_update

   // If the platform is low on fuel, the assignment(s) needs to be marked as CANTCO and Egress
   if (wm->getIsLowOnFuel())
   {
      CANTCOAllAssignments("Low on fuel");
   }

   std::shared_ptr<il::AssetManagerInterface> am = m_wm_component->GetCoreAMRef();
   std::shared_ptr<il::assetRecord> thisAsset = am->getThisAsset();

   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

   // loop through our assignments that we are not engaging and kick them off
   const auto& our_assignments = am->getOurAssignments(*logger);
   for (auto iter = our_assignments.cbegin(); iter != our_assignments.cend(); ++iter)
   {
      il::evaluationAssignment assignment = *iter;
      WsfWeapon* weapon = WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(m_wm_component->GetSimulation(), assignment->getAssignedID());
      const std::shared_ptr<il::weaponRecord> weapon_record = m_wm_component->GetCoreAMRef()->getThisAsset()->getWeapon(assignment.Assignment()->getAssignedID());
      std::shared_ptr<il::trackRecord> track = assignment.Track();

      // HAVCO
      bool haveco_success;
      if (IsEngagementComplete(assignment, haveco_success))
      {
         am->havecoAssignment(*logger, assignment, haveco_success);
         continue;
      }

      // Own status is no longer GREEN? CANTCO mission.
      if (!thisAsset->isCombatReady())
      {
         am->cantcoAssignment(*logger, assignment, "No self status (not green)");
         continue;
      }

      if (!AssignmentHasValidTrack(assignment))
      {
         am->cantcoAssignment(*logger, assignment, "No Master Track");
         continue;
      }

      // Haven't fired and munitions no longer employed by unit?
      // Possibly should just compare SalvosFired to 0
      if (assignment->getSalvosFired() < assignment->getRequestedMissilesToCommit())
      {
         if (!weapon)
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "Error, could not locate assigned weapon for engagement: " << assignment->getAssignedID() << " against " << track->getTargetTruthName();
            am->cantcoAssignment(*logger, assignment, "No Weapon");
            continue;
         }
         else if (weapon->GetQuantityRemaining() + weapon->WeaponsActiveAndPendingFor(WsfBMUtils::ConvertTrackId(assignment->getLocalTrackID())) <= 0)
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "Error, weapon has insufficient munitions: " << assignment->getAssignedID() << " to engage target " << track->getTargetTruthName();
            am->cantcoAssignment(*logger, assignment, "Insufficient Munitions");
            continue;
         }
         if (!weapon_record)
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "WsfWMAIEngagementMod::AddBehaviorsToStack(): Invalid weapon record referenced in assignment: " << assignment.Assignment()->getAssignedID();
            am->cantcoAssignment(*logger, assignment, "No Weapon");
            continue;
         }
      }

      // If we haven't launched at the target yet or it is time to take our second shot, then we need to proceed.
      if (assignment->getSalvosFired() == 0 || TimeForSecondSalvo(assignment, weapon, sim_time_s))
      {
         // WILCO not reported on live target? WILCO the assignment if we haven't already
         auto assignment_status = assignment->getStatus();
         if ((!assignment_status) ||
             (!assignment_status->statusTimeExists(il::assignmentACKMessage::WILCO))) // assignment_status
         {
            am->wilcoAssignment(*logger, assignment);
         }

         // Not SCRAMBLED this assignment AND (NOT vectoring any target OR vectoring this target)? Fly the AI.
         if (Empty() && (!isParked() || sim_time_s > assignment->getAssignTime() + wm->getAlertTime()))
         {
            Engage(sim_time_s, assignment);
            continue;
         }

         // Taking Action?
         // If in the middle of taking an action, should this just prevent the AI from firing?
         if (TakingAction())
         {
            continue;
         }

         auto localTrack = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID()));

         // Engagement no longer viable considering kinematics and zones?
         // TODO This can potentially cause weird behavior when assigned against targets faster than the AI.
         // (Particularly once close enough to the target for pursue behavior to begin).
         auto assessment = AssessEngagement(sim_time_s, assignment, weapon_record);
         if (!assessment.canInterceptTrack())
         {
            am->cantcoAssignment(*logger, assignment, "No viable Intercept");
            continue;
         }

         // Calculate ground range between AI and target (propagated to current time)
         auto masterTrack = am->getMasterTrack(track->getID());
         auto propagatedPosition = masterTrack->getPropagatedPosition(sim_time_s);
         WsfGeoPoint assetPosition, targetPosition;
         assetPosition.SetLocationLLA(thisAsset->getPosition().getLatDegs(), thisAsset->getPosition().getLonDegs(), thisAsset->getPosition().getAltM());
         targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());

         auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());

         // If intercepting, and within Pursue range, a pursue behavior should begin.
         if (!Empty() && Top()->getType() == Behavior::INTERCEPT &&
             static_cast<Intercept*>(Top().get())->getLocalTrackId() == assignment->getLocalTrackID() &&
             assetPosition.GroundRangeTo(targetPosition) < wm->getPursuitRange())
         {
            // Add Pursue to behavior stack
            Push(std::make_shared<Pursue>(assignment, static_cast<Intercept*>(Top().get())->hasGeneratedAction(), this));

            // Calculate ground range between AI and target (propagated to current time)
            auto this_asset = m_wm_component->GetCoreAMRef()->getThisAsset();
            double ground_range = 0.0;
            double ground_range_DA = 0.0;
            getGroundRangeAndGroundRangeDA(assignment.Track().get(), sim_time_s, ground_range, ground_range_DA);

            // Log Pursue
            IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AIAddBehavior(sim_time_s, "PURSUE", assignment.Track().get(), this_asset.get(), ground_range, ground_range_DA);
         }

         // If not intercepting or pursuing the track of this assignment, continue iterating through assignments. Should only fire against the track the AI is engaging.
         if (Empty() || (Top()->getLocalTrackId() != assignment->getLocalTrackID() || !(Top()->getType() == Behavior::INTERCEPT || Top()->getType() == Behavior::PURSUE)))
         {
            continue;
         }

         // Check to ensure track is of sufficient quality
         // Master track should already be checked by this point by the call to AssignmentHasValidTrack
         auto reporting_sensor_type = masterTrack->getReportingSensorTypeEnum();
         auto reporting_sensor_type_str = masterTrack->getReportingSensorType();
         if (reporting_sensor_type == il::SENSOR_RWR ||
             (reporting_sensor_type == il::SENSOR_EW && !wm->getEngageEWTargets()) ||
             (reporting_sensor_type == il::SENSOR_TAR && !wm->getEngageTARTargets()) ||
             (reporting_sensor_type == il::SENSOR_TTR && !wm->getEngageTTRTargets()))
         {
            HCL_DEBUG_LOGGER(am->getGlobalLogger()) << "Cannot engage target: " << masterTrack->getTargetTruthName() << ", track is of sensor type " << reporting_sensor_type_str << " waiting...";
            continue;
         }

         if (wm->getEngageLocalTTRTargetsOnly() && !WsfBMUtils::IsTrackGeneratedByLocalSensor(m_wm_component->GetPlatform(), WsfBMUtils::ConvertTrackId(track->getID()), il::SENSOR_TTR))
         {
            HCL_DEBUG_LOGGER(am->getGlobalLogger()) << "Cannot engage target: " << masterTrack->getTargetTruthName() << ", WM set to engage local TTR tracks only, and track was not generated by a TTR that is local to the platform (on the platform, a peer, or its direct commander) waiting...";
            continue;
         }

         if (reporting_sensor_type == il::SENSOR_UNKNOWN)
         {
            HCL_DEBUG_LOGGER(am->getGlobalLogger()) << "Cannot engage target: " << masterTrack->getTargetTruthName() << ", no reporting sensor type" << " waiting...";
            continue;
         }

         if (localTrack->GetQuality() < wm->getEngageTrackQuality())
         {
            HCL_DEBUG_LOGGER(am->getGlobalLogger()) << "Cannot engage target: " << masterTrack->getTargetTruthName() << ", track quality " << localTrack->GetQuality() << " less than track quality required to engage by WM (" << wm->getEngageTrackQuality() << ")";
            continue;
         }

         // HELIOS comment
         // If we get here, we are ready to engage. However, we need to check against the
         //time we last engaged an assignment plus the delay between assignment engagements.
         //If there is a delay, we must wait for it to expire before we engage a new
         //assignment. This will not be hit for SALVO shots (as it should not be, since they
         //are delayed according to the SalvoDelay), as they are launched prior to this
         //function.

         double range = assetPosition.GetDistanceFrom(targetPosition);
         if (weapon_record->HasMaxRangeAttribute())
         {
            if (range > weapon_record->GetMaxRange())
            {
               continue;
            }
         }
         else
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "WsfWMAIEngagementMod::RunDefaultEngagement(): Weapon " << weapon_record->GetWeaponID() << " has no max range attribute.";
         }

         if (weapon_record->HasMinRangeAttribute())
         {
            if (range < weapon_record->GetMinRange())
            {
               continue;
            }
         }
         else
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "WsfWMAIEngagementMod::RunDefaultEngagement(): Weapon " << weapon_record->GetWeaponID() << " has no min range attribute.";
         }

         const auto& weapon_zones = weapon_record->GetAttachedZones();
         bool isInsideZone = il::IsInsideZone(weapon_zones, propagatedPosition, true) || wm->getAssessEngageVia() == il::MUNITION_INTERCEPT_PT_IGNORE_ZONE;
         if (!isInsideZone)
         {
            HCL_TRACE_LOGGER(am->getGlobalLogger()) << "WsfWMAIEngagementMod::RunDefaultEngagement(): " << sim_time_s << ": " << masterTrack->getTargetTruthName() << " not inside zones of weapon " << weapon_record->GetName();
         }

         // TODO Should we be willing to use an Intercept Calculator here?
         if (isInsideZone && assessment.canInterceptTrack() && weapon->FireSalvo(sim_time_s, localTrack, assignment->getShotDoctrine() == il::SHOOT_2 ? 2 : 1))
         {
            am->shotsFiredAssignmentStatus(*logger, assignment);
         }
         else
         {
            HCL_WARN_LOGGER(am->getGlobalLogger()) << "Error firing salvo, CANTCO'ing assignment";
            am->cantcoAssignment(*logger, assignment.Assignment(), "Weapon Failure", true, false);
         }
      }
   }
}

void WsfWMAIEngagementMod::LogDebugInfo(const double sim_time_s)
{
   auto sim = m_wm_component->GetSimulation();
   auto asset = m_wm_component->GetCoreAMRef()->getMyAsset();
   auto pos = asset.getPosition();
   std::string assignmentString = std::string();
   auto assignments = m_wm_component->GetCoreAMRef()->getOurAssignments(*IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger());
   for (auto iter = assignments.begin(); iter != assignments.end(); ++iter)
   {
      auto target = WsfBMUtils::GetPlatformFromUnitIDRecord(sim, iter->Track()->getTargetTruthID());
      double lat, lon, alt;
      if (target)
      {
         target->GetLocationLLA(lat, lon, alt);
      }
      else
      {
         lat = lon = alt = 0;
      }
      auto targetPos = il::positionRecord();
      targetPos.setLLA(lat, lon, alt);
      auto distance = pos.calculateDistanceMeters(targetPos);
      std::stringstream ss;
      ss << "\t" << iter->Assignment()->getLocalTrackID().getIDString() << ":" << iter->Assignment()->getLocalTrackID().getSubID() <<
         "\tRange: " << distance << "\tLat: " << lat << "\tLon: " << lon << "\tAlt: " << alt << "\n";
      assignmentString = ss.str();
   }
   std::string behaviorString = std::string();
   std::stack<std::shared_ptr<Behavior>> behaviorStack;
   while (!m_behavior_stack.empty())
   {
      behaviorString += "\t" + Behavior::getBehaviorTypeString(m_behavior_stack.top()->getType()) + "\n";
      behaviorStack.push(m_behavior_stack.top());
      m_behavior_stack.pop();
   }
   while (!behaviorStack.empty())
   {
      m_behavior_stack.push(behaviorStack.top());
      behaviorStack.pop();
   }
   std::string rwrString = std::string();
   WsfTrackList rwrTracks = RWRDetectedThreatList();
   for (unsigned int i = 0, count = rwrTracks.GetTrackCount(); i != count; ++i)
   {
      WsfTrack* rawTrack = rwrTracks.GetTrackEntry(i);
      rwrString += "\t" + rawTrack->GetTargetName() + "\n";
   }

   HCL_DEBUG_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMAIEngagementMod::AddBehaviorsToStack(" << sim_time_s << ")\n"
                                                                       << "Position:\tLat:" << pos.getLatDegs() << "\tLon" << pos.getLonDegs() << "\tAlt" << pos.getAltM() << "\n"
                                                                       << "Assignments:\n" << assignmentString
                                                                       << "Behaviors:\n" << behaviorString
                                                                       << "RWR:\n" << rwrString;
}

void WsfWMAIEngagementMod::Engage(const double sim_time_s, const il::evaluationAssignment& assignment)
{
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());

   // Calculate ground range between AI and target (propagated to current time)
   auto this_asset = m_wm_component->GetCoreAMRef()->getThisAsset();
   double ground_range = 0.0;
   double ground_range_DA = 0.0;
   getGroundRangeAndGroundRangeDA(assignment.Track().get(), sim_time_s, ground_range, ground_range_DA);

   if (!wm->getEscort())
   {
      if (ground_range > wm->getPursuitRange())
      {
         Push(std::make_shared<Intercept>(assignment, this));
         IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AIAddBehavior(sim_time_s, "INTERCEPT", assignment.Track().get(), this_asset.get(), ground_range, ground_range_DA);
      }
      else
      {
         // If this Pursue is immediately following an Intercept of the same assignment, ensure that info on whether the Intercept generated
         // a TakeAction Behavior gets passed on to the Pursue Behavior. (Each Intercept-Pursue chain should generate at most one TakeAction).
         if (!Empty() && Top()->getType() == Behavior::INTERCEPT && static_cast<Intercept*>(Top().get())->getAssignedId() == assignment->getAssignedID())
         {
            Push(std::make_shared<Pursue>(assignment, static_cast<Intercept*>(Top().get())->hasGeneratedAction(), this));
         }
         else
         {
            Push(std::make_shared<Pursue>(assignment, false, this));
         }
         IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AIAddBehavior(sim_time_s, "PURSUE", assignment.Track().get(), this_asset.get(), ground_range, ground_range_DA);
      }
   }
}

il::assessmentRecord WsfWMAIEngagementMod::AssessEngagement(const double sim_time_s, il::evaluationAssignment& assignment, const std::shared_ptr<il::weaponRecord> weapon, const double delay_time_s)
{
   auto am = m_wm_component->GetCoreAMRef();

   // TODO this script calculator is unused and needs to be scrapped
   auto script_calculator = GetScriptCalculator(assignment);
   //if (!script_calculator)
   //{
   //   HCL_WARN_LOGGER() << "WsfWMAIEngagementMod::AssessEngagement(): Could not find a can_intercept script on the weapon platform or weapon manager and the assigned weapon does not have a launch computer.\n";
   //}

   auto masterTrack = il::FindTrack(assignment->getLocalTrackID(), am->GetMasterTracksArray());
   if (!masterTrack)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << am->getMyAsset().getName() << " attempted to perform AI AssessEngagement, but could not find master track " << assignment->getLocalTrackID();
      return il::assessmentRecord(nullptr, am->getGlobalLogger(), nullptr);
   }

   // the assessment records have an interface for projecting time to fire, we reuse that here.
   const double time_to_project = DBL_MAX, projection_incr = DBL_MAX;

   auto assessment = il::assessmentRecord(script_calculator, am->getGlobalLogger());
   assessment.AssessAIEngagement(sim_time_s, weapon, *masterTrack.get(), true, delay_time_s, time_to_project, projection_incr);

   return assessment;
}

// Returns all threats detected by the RWR (i.e., all raw tracks in the RWR's active track list).
WsfTrackList WsfWMAIEngagementMod::RWRDetectedThreatList() const
{
   //return static_cast<WsfWeaponsManagerAI*>(m_wm_component)->RWRDetectedThreatList();
   WsfTrackList tracklist;
   std::shared_ptr<il::assetRecord> this_asset = m_wm_component->GetCoreAMRef()->getThisAsset();
   std::shared_ptr<il::sensorRecord> sensor_record = this_asset->getNthSensorOfType(1, il::SENSOR_RWR);
   auto rwr_record = static_cast<WsfBMRWRSensorInfoRecord*>(sensor_record.get());

   if (rwr_record)
   {
      rwr_record->GetRWRActiveTrackList(tracklist);
   }
   return tracklist;
}

// Returns the number of threats detected by the RWR.
int WsfWMAIEngagementMod::RWRDetectedThreatCount() const
{
   //return static_cast<WsfWeaponsManagerAI*>(m_wm_component)->RWRDetectedThreatCount();
   std::shared_ptr<il::assetRecord> this_asset = m_wm_component->GetCoreAMRef()->getThisAsset();
   std::shared_ptr<il::sensorRecord> sensor_record = this_asset->getNthSensorOfType(1, il::SENSOR_RWR);
   auto rwr_record = static_cast<WsfBMRWRSensorInfoRecord*>(sensor_record.get());

   if (rwr_record)
   {
      return rwr_record->GetRWRActiveTrackCount();
   }
   return 0;
}

// Returns a WsfTrackList containing raw tracks for only those threats detected by the RWR that are within either the SAM or AI response range.
// (Also ensures that all tracks in returned WsfTrackList are believed alive, since threats that are not believed alive may still remain in the
// RWR's active track list.)
WsfTrackList WsfWMAIEngagementMod::RWRDetectedThreatsInResponseRange(const double sim_time_s)
{
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());

   // Get all threats currently detected by the RWR (i.e., all raw tracks in the RWR's active track list)
   WsfTrackList detected_threats = RWRDetectedThreatList();
   std::vector<WsfTrackId> out_of_range_threats;

   // For each raw track in the RWR's detected threat list...
   for (int i = 0, count = detected_threats.GetTrackCount(); i != count; ++i)
   {
      //...look for the correlated local track.
      WsfTrack* raw_track = detected_threats.GetTrackEntry(i);
      WsfLocalTrack* local_track = FindCorrelatedTrack(raw_track->GetTrackId());
      if (local_track)
      {
         // If a local track was found, get the distance from our platform to that local track (propagated to the current sim time).
         il::idRecord local_track_id = WsfBMUtils::ConvertTrackId(local_track->GetTrackId());
         double distance = getDistanceToTrack(local_track_id, sim_time_s);

         // If the track is outside of the response range, add its track ID to the list of out-of-range threats.
         // Use AI response range for threats with a spatial domain of air, space, or unknown.
         // Use SAM response range for threats with a spatial domain of land, surface, or subsurface.
         double relevant_response_range = wm->getEvasionRWRAIResponseRange();
         switch (local_track->GetSpatialDomain())
         {
            case WSF_SPATIAL_DOMAIN_AIR:
               break;
            case WSF_SPATIAL_DOMAIN_SPACE:
               HCL_ERROR_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Threat has WSF_SPATIAL_DOMAIN_SPACE: default to AI response range, but evasion behavior is not specifically implemented for threats in this spatial domain.";
               break;
            case WSF_SPATIAL_DOMAIN_UNKNOWN:
               HCL_ERROR_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Threat has an unknown spatial domain: default to AI response range to determine evasion behavior.";
               break;
            case WSF_SPATIAL_DOMAIN_LAND:
            case WSF_SPATIAL_DOMAIN_SURFACE:
               relevant_response_range = wm->getEvasionRWRSAMResponseRange();
               break;
            case WSF_SPATIAL_DOMAIN_SUBSURFACE:
               relevant_response_range = wm->getEvasionRWRSAMResponseRange();
               HCL_ERROR_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Threat has WSF_SPATIAL_DOMAIN_SUBSURFACE: default to SAM response range, but evasion behavior is not specifically implemented for threats in this spatial domain.";
               break;
            default:
               HCL_ERROR_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "Threat has unhandled spatial domain: default to AI response range.";
               break;
         }

         if (distance < 0 || distance > relevant_response_range)
         {
            out_of_range_threats.push_back(raw_track->GetTrackId());
         }
      }
   }

   // Remove all out-of-range threats from the track list of detected threats.
   for (auto iter = out_of_range_threats.begin(); iter != out_of_range_threats.end(); ++iter)
   {
      const auto& track_id = *iter;
      detected_threats.RemoveTrack(track_id);
   }

   return detected_threats;
}

// Returns the number of threats detected by the RWR (i.e., tracks in its active track list) that are within the response range.
int WsfWMAIEngagementMod::RWRDetectedThreatCountInResponseRange(const double sim_time_s)
{
   WsfTrackList threats_in_range = RWRDetectedThreatsInResponseRange(sim_time_s);
   return threats_in_range.GetTrackCount();
}

// Returns a WsfLocalTrack* representing the highest-priority threat sensed by the RWR, using the user's selected priority.
// Default behavior: WsfLocalTrack* that is correlated with the first threat (raw tracK) in RWR's active tracklist.
// If unable to find correlated local track, returns nullptr.
WsfLocalTrack* WsfWMAIEngagementMod::RWRHighPriorityThreat(const double sim_time_s) /*const */
{
   // Get the list of threats detected by the RWR that are within our response range.
   WsfTrackList rwr_track_list = RWRDetectedThreatsInResponseRange(sim_time_s);

   // RWR isn't detecting any threats in response range, so return a nullptr.
   if (rwr_track_list.GetTrackCount() == 0)
   {
      return nullptr;
   }

   // RWR is only detecting one threat in response range, so find the correlated local track for this raw track and
   // return it as the highest priority threat.
   if (rwr_track_list.GetTrackCount() == 1)
   {
      WsfTrack* high_priority_raw_track = rwr_track_list.GetTrackEntry(0);
      return FindCorrelatedTrack(high_priority_raw_track->GetTrackId());
   }

   // RWR is detecting more than one threat in response range, so get user's preferred priority for choosing which threat to evade.
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());
   il::eRWRResponsePriority priority = wm->getEvasionRWRResponsePriority();

   // User has indicated that we should consider evasion behaviors relative to the closest in-range threat detected by the RWR.
   if (priority == il::eRWRResponsePriority::CHOOSE_CLOSEST_THREAT)
   {
      WsfLocalTrack* closest_threat_local_track = nullptr;
      double closest_distance = std::numeric_limits<double>::max();

      // For each raw track in the RWR's active track list...
      for (int i = 0, count = rwr_track_list.GetTrackCount(); i != count; ++i)
      {
         //...look for the correlated local track.
         WsfTrack* raw_track = rwr_track_list.GetTrackEntry(i);
         WsfLocalTrack* local_track = FindCorrelatedTrack(raw_track->GetTrackId());
         if (local_track)
         {
            // If a local track was found, get the distance from our platform to that local track (propagated to the current sim time).
            il::idRecord local_track_id = WsfBMUtils::ConvertTrackId(local_track->GetTrackId());
            double distance = getDistanceToTrack(local_track_id, sim_time_s);

            // If this threat is closer than our previously identified closest threat, update variables accordingly.
            if (distance > -1 && distance < closest_distance)
            {
               closest_threat_local_track = local_track;
               closest_distance = distance;
            }
         }
      }
      return closest_threat_local_track;
   }

   // User has either explicitly set the RWR response priority to "Default" or hasn't set a priority in the script. The default priority
   // for choosing which threat sensed by the RWR to evade is to choose the first raw track in the RWR's active track list. (Note that this
   // can lead to strange behavior, such as making decisions about evasion behavior relative to a distance threat while ignoring a much closer
   // threat, if the distant threat happens to be first in the RWR's active track list).
   else
   {
      // Get first raw track from the RWR's active track list.
      WsfTrackList rwr_track_list = RWRDetectedThreatsInResponseRange(sim_time_s);
      WsfTrack* high_priority_raw_track = rwr_track_list.GetTrackEntry(0);

      // Attempt to find and return the correlated local track in the master track list. (Otherwise, return nullptr).
      return FindCorrelatedTrack(high_priority_raw_track->GetTrackId());
   }
}

WsfStringId WsfWMAIEngagementMod::RWRNameId() const
{
   //return static_cast<WsfWeaponsManagerAI*>(m_wm_component)->RWRDetectedThreatCount();
   std::shared_ptr<il::assetRecord> this_asset = m_wm_component->GetCoreAMRef()->getThisAsset();
   std::shared_ptr<il::sensorRecord> sensor_record = this_asset->getNthSensorOfType(1, il::SENSOR_RWR);
   auto rwr_record = static_cast<WsfBMRWRSensorInfoRecord*>(sensor_record.get());

   if (rwr_record)
   {
      return rwr_record->GetNameId();
   }
   return WsfStringId();
}

// Returns true if user has turned on the RWR.
bool WsfWMAIEngagementMod::RWRResponse() const
{
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());
   return wm->getEvasionRWRResponse();
}

// Returns the update time of the threat most recently detected by the RWR.
double WsfWMAIEngagementMod::LastLockTime() const
{
   double lastLockTime = 0.0;
   auto rwrTrackList = RWRDetectedThreatList();
   for (int i = 0, count = rwrTrackList.GetTrackCount(); i != count; ++i)
   {
      auto trackEntry = rwrTrackList.GetTrackEntry(i);
      lastLockTime = std::max(lastLockTime, trackEntry->GetUpdateTime());
   }
   return lastLockTime;
}

// Returns true if a RespondBeam Behavior or RespondDrag Behavior is added to the behavior stack, or if we're currently evading.
bool WsfWMAIEngagementMod::AddEvasionBehavior(const double sim_time_s)
{
   bool performing_evasion = false;

   // We'll only evade (with either a beam response or a drag response) if we're not "taking action" and if user has turned on the RWR response.
   if (!TakingAction() && RWRResponse())
   {
      if (RWRDetectedThreatCountInResponseRange(sim_time_s) > 0)
      {
         // Find the highest priority threat sensed by the RWR. Look for the correlated local track associated with this threat
         // in the master track list, and test that the WsfLocalTrack* was found before proceeding.
         WsfLocalTrack* high_priority_threat = RWRHighPriorityThreat(sim_time_s);
         if (high_priority_threat != nullptr)
         {
            il::idRecord high_priority_threat_id = WsfBMUtils::ConvertTrackId(high_priority_threat->GetTrackId());
            // We're executing a beam response: should we switch to drag response, execute a beam response relative to a different threat, or continue our current beam response?
            if (!Empty() && Top()->getType() == Behavior::BEAM)
            {
               auto current_beam = static_cast<Beam*>(Top().get());

               // Has beam time elapsed for the top RespondBeam Behavior?
               if (current_beam->isFinished(sim_time_s))
               {
                  // If so, switch to a drag response by adding RespondDrag(relative to high_priority_threat) to behavior stack. This may or may not be the same threat that our beam response was associated with.
                  auto drag_response = std::make_shared<Drag>(high_priority_threat_id, this);
                  Push(drag_response);

                  // Get track record and range information about high priority threat needed for moe logging.
                  auto am = m_wm_component->GetCoreAMRef();
                  auto this_asset = am->getThisAsset();

                  WsfTrackId track_id = high_priority_threat->GetTrackId();
                  il::trackRecord* track = il::FindTrack(WsfBMUtils::ConvertTrackId(track_id), am->GetMasterTracksArray()).get();
                  double ground_range = 0.0;
                  double ground_range_DA = 0.0;
                  getGroundRangeAndGroundRangeDA(track, sim_time_s, ground_range, ground_range_DA);

                  IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AIAddBehavior(sim_time_s, "DRAG", track, this_asset.get(), ground_range, ground_range_DA);
               }

               // Otherwise, continue beam response.
               // If we've been responding to something other than the high priority threat, adapt current beam response to act relative to high priority threat.
               // (These WsfTrackIds should be reliable enough for this comparison but do not offer flexibility for multiple sensor/platform comparisons).
               else if (high_priority_threat_id != current_beam->getLocalTrackId())
               {
                  current_beam->setLocalTrackId(high_priority_threat_id);
               }

               performing_evasion = true;
            }

            // We're executing a drag response: should we perform a drag response relative to a different threat or continue our current drag response?
            else if (!Empty() && Top()->getType() == Behavior::DRAG)
            {
               auto current_drag = static_cast<Drag*>(Top().get());

               // If we've been responding to something other than the high priority threat, adapt current drag response to act relative to high priority threat.
               if (high_priority_threat_id != current_drag->getLocalTrackId())
               {
                  current_drag->setLocalTrackId(high_priority_threat_id);
               }

               performing_evasion = true;
            }

            // We're not currently evading a threat: should we begin evading a threat using a beam response?
            else
            {
               il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();

               il::idRecord attacker_local_track_id = WsfBMUtils::ConvertTrackId(high_priority_threat->GetTrackId());
               if (AssessBeamResponse(*logger, sim_time_s, attacker_local_track_id, high_priority_threat->GetSpatialDomain()))
               {
                  auto beam_response = std::make_shared<Beam>(high_priority_threat_id, this, sim_time_s);
                  Push(beam_response);
                  performing_evasion = true;

                  // Get track record and range information about high priority threat needed for moe logging.
                  auto am = m_wm_component->GetCoreAMRef();
                  auto this_asset = am->getThisAsset();

                  WsfTrackId track_id = high_priority_threat->GetTrackId();
                  il::trackRecord* track = il::FindTrack(WsfBMUtils::ConvertTrackId(track_id), am->GetMasterTracksArray()).get();
                  double ground_range = 0.0;
                  double ground_range_DA = 0.0;
                  getGroundRangeAndGroundRangeDA(track, sim_time_s, ground_range, ground_range_DA);

                  logger->AIAddBehavior(sim_time_s, "BEAM", track, this_asset.get(), ground_range, ground_range_DA);
               }
            }
         }
      }
   }

   return performing_evasion;
}

// If AssessResponse() determines that we should "take action," creates an appropriate TakeAction Behavior and adds it to the behavior stack.
bool WsfWMAIEngagementMod::AddTakeActionBehavior(const double sim_time_s)
{
   // Only add a TakeAction Behavior if an Intercept or Pursue is at the top of the behavior stack
   if (!Empty() && (Top()->getType() == Behavior::INTERCEPT || Top()->getType() == Behavior::PURSUE))
   {
      il::idRecord assignmentLocalTrackId;
      il::idRecord assignmentAssignedId;
      bool hasGeneratedAction = false;

      // If we're going to "take action," it will be relative to the same assignment that we're currently intercepting or pursuing:
      // Get assignment for TakeAction Behavior from the current Intercept or Pursue Behavior.
      if (Top()->getType() == Behavior::INTERCEPT)
      {
         auto topInterceptBehavior = static_cast<Intercept*>(Top().get());
         assignmentLocalTrackId = topInterceptBehavior->getLocalTrackId();
         assignmentAssignedId = topInterceptBehavior->getAssignedId();
         hasGeneratedAction = topInterceptBehavior->hasGeneratedAction();
      }
      else
      {
         auto topPursueBehavior = static_cast<Pursue*>(Top().get());
         assignmentLocalTrackId = topPursueBehavior->getLocalTrackId();
         assignmentAssignedId = topPursueBehavior->getAssignedId();
         hasGeneratedAction = topPursueBehavior->hasGeneratedAction();
      }

      il::eTakeAction actionType;

      // Any Intercept-Pursue chain can only generate one TakeAction Behavior: check that this Intercept or Pursue has not yet generated a TakeAction.
      if (!hasGeneratedAction)
      {
         double actionDuration = -1.0;
         // AssessAction has determined that we should add a TakeAction Behavior (and has specified type and duration of action).
         if (AssessAction(sim_time_s, assignmentLocalTrackId, actionType, actionDuration))
         {
            // Indicate that this Intercept or Pursue has generated a TakeAction Behavior.
            if (Top()->getType() == Behavior::INTERCEPT)
            {
               static_cast<Intercept*>(Top().get())->setHasGeneratedAction();
            }
            else
            {
               static_cast<Pursue*>(Top().get())->setHasGeneratedAction();
            }

            // Create a new TakeAction Behavior and add it to the behavior stack.
            auto takeAction = std::make_shared<TakeAction>(assignmentLocalTrackId, assignmentAssignedId, actionType, sim_time_s, actionDuration, this);
            Push(takeAction);

            // Get track record and range information about high priority threat needed for moe logging.
            auto am = m_wm_component->GetCoreAMRef();
            auto thisAsset = am->getThisAsset();

            il::trackRecord* track = il::FindTrack(assignmentLocalTrackId, am->GetMasterTracksArray()).get();
            double groundRange = 0.0;
            double groundRangeDA = 0.0;
            getGroundRangeAndGroundRangeDA(track, sim_time_s, groundRange, groundRangeDA);

            IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AITakeAction(sim_time_s, track, thisAsset.get(), TakeActionEnumToString(actionType), sim_time_s + actionDuration, groundRange, groundRangeDA);
            return true;
         }
      }
   }
   return false;
}

double WsfWMAIEngagementMod::getRWRResponseBeamDuration() const
{
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());
   return wm->getEvasionRWRResponseBeamDuration();
}

WsfLocalTrack* WsfWMAIEngagementMod::FindCorrelatedTrack(const WsfTrackId& raw_track_id) const
{
   WsfTrackManager& tm = m_wm_component->GetPlatform()->GetTrackManager();
   return tm.FindCorrelatedTrack(raw_track_id);
}

// Having been tracked by either a SAM or AI tracking radar (as reported by our RWR) that is in our response range, assess whether
// to respond by trying to put the threat on my beam. Returns true if a BeamResponse Behavior should be added to the behavior stack.
bool WsfWMAIEngagementMod::AssessBeamResponse(il::moeLoggingInterface& aMOELogger, const double sim_time_s, const il::idRecord& attacker_local_track_id, WsfSpatialDomain domain)
{
   auto am_proc = m_wm_component->GetCoreAMRef();

   // Am I in the process of engaging this attacker?
   // Loop through assignment array.
   bool respond_beam = true;
   il::MapAssignmentArrayFunc
   (aMOELogger, am_proc->getAssignmentArray(),
    [&respond_beam, attacker_local_track_id](il::moeLoggingInterface & aMOELogger, il::evalAssignmentMultimap & assignment, il::evalAssignmentMultimap::iterator & assigned_track_iter, il::evalAssignmentMultimap::iterator & assignment_iter, WsfWMAIEngagementMod * em) -> void
   {
      std::shared_ptr<il::assetRecord> this_asset = em->m_wm_component->GetCoreAMRef()->getThisAsset();

      // Am I the assigned unit?
      if (assignment_iter->second->getAssignedID() == this_asset->getID())
      {
         // Is attacker the unit that I have been assigned to kill?
         if (assignment_iter->second->getLocalTrackID() == attacker_local_track_id)
         {
            // If I'm firing, keep on attacking - no need to assess beam response. Also, no need to assess a response if we killed the target.
            if (em->AttackerEngagedOrBelievedDead(assignment_iter->second))
            {
               respond_beam = false;
            }
         }
      }
      ++assignment_iter;
   }, this);

   return respond_beam;
}

// Returns true if either: (1) we are currently engaging the assignment (i.e., our platform has any weapons active or pending
// for the assignment), or (2) the assignment is believed dead.
bool WsfWMAIEngagementMod::AttackerEngagedOrBelievedDead(const il::evaluationAssignment& assignment) const
{
   il::idRecord track_id = assignment.Assignment()->getLocalTrackID();
   WsfTrackId assignment_track_id = WsfBMUtils::ConvertTrackId(track_id);

   WsfSimulation* sim = m_wm_component->GetSimulation();
   WsfLocalTrack* local_track = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(assignment_track_id);
   bool believed_dead = !WsfBMUtils::BelievedAlive(sim, local_track);

   return believed_dead || AttackerEngaged(assignment);
}

// Returns true if our platform has any weapons active or pending for the assignment
bool WsfWMAIEngagementMod::AttackerEngaged(const il::evaluationAssignment& assignment) const
{
   il::idRecord track_id = assignment.Assignment()->getLocalTrackID();
   WsfTrackId assignment_track_id = WsfBMUtils::ConvertTrackId(track_id);

   int weapons_engaging_attacker = WsfWeapon::WeaponsActiveAndPendingFor(*(m_wm_component->GetPlatform()), assignment_track_id);
   return weapons_engaging_attacker > 0;
}

// Returns true if our platform has any weapons active or pending for the assignment
bool WsfWMAIEngagementMod::AttackerEngaged(const il::idRecord& assignment_local_track_id) const
{
   WsfTrackId assignment_track_id = WsfBMUtils::ConvertTrackId(assignment_local_track_id);

   int weapons_engaging_attacker = WsfWeapon::WeaponsActiveAndPendingFor(*(m_wm_component->GetPlatform()), assignment_track_id);
   return weapons_engaging_attacker > 0;
}

// If the local track referred to by local_track_id appears in the master track list, returns distance from this platform
// to the expected position of the track at the current sim time. Else, returns -1.0.
double WsfWMAIEngagementMod::getDistanceToTrack(const il::idRecord& local_track_id, const double sim_time_s)
{
   auto am = m_wm_component->GetCoreAMRef();
   auto thisAsset = am->getThisAsset();
   auto track = il::FindTrack(local_track_id, am->GetMasterTracksArray());

   if (track.get() != nullptr)
   {
      // Get the predicted position for this target
      il::positionRecord propagatedPosition = track->getPropagatedPosition(sim_time_s);

      // Get distance to target
      WsfGeoPoint assetPosition, targetPosition;
      assetPosition.SetLocationLLA(thisAsset->getPosition().getLatDegs(), thisAsset->getPosition().getLonDegs(), thisAsset->getPosition().getAltM());
      targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());
      return assetPosition.GetDistanceFrom(targetPosition);
   }
   return -1.0;
}

// If the local_track_id appears in the master track list, returns ground range from this platform to the position of the track
// (propagated to the current sim time). Else, returns -1.0.
double WsfWMAIEngagementMod::getGroundRangeToTrack(const il::idRecord& local_track_id, const double sim_time_s)
{
   auto am = m_wm_component->GetCoreAMRef();
   auto thisAsset = am->getThisAsset();
   auto track = il::FindTrack(local_track_id, am->GetMasterTracksArray());

   if (track.get() != nullptr)
   {
      // Get the predicted position for this target
      il::positionRecord propagatedPosition = track->getPropagatedPosition(sim_time_s);

      // Get distance to target
      WsfGeoPoint assetPosition, targetPosition;
      assetPosition.SetLocationLLA(thisAsset->getPosition().getLatDegs(), thisAsset->getPosition().getLonDegs(), thisAsset->getPosition().getAltM());
      targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());
      return assetPosition.GroundRangeTo(targetPosition);
   }
   return -1.0;
}

void WsfWMAIEngagementMod::getGroundRangeAndGroundRangeDA(const il::trackRecord* track, const double sim_time_s, double& ground_range, double& ground_range_DA)
{
   if (track)
   {
      auto am = m_wm_component->GetCoreAMRef();
      auto thisAsset = am->getThisAsset();

      // Get the predicted position for this track
      il::positionRecord propagatedPosition = track->getPropagatedPosition(sim_time_s);

      // Get ground range to track
      WsfGeoPoint assetPosition, targetPosition;
      assetPosition.SetLocationLLA(thisAsset->getPosition().getLatDegs(), thisAsset->getPosition().getLonDegs(), thisAsset->getPosition().getAltM());
      targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());
      ground_range = assetPosition.GroundRangeTo(targetPosition);

      // Calculate ground range between target and closest DA center
      auto zones = thisAsset->GetDefendedZones();
      double groundRangeDA = DBL_MAX;
      for (auto it = zones.cbegin(); it != zones.cend(); ++it)
      {
         auto zone = *it;
         WsfGeoPoint center;
         center.SetLocationLLA(zone->getCenterPoint().getLatDegs(), zone->getCenterPoint().getLonDegs(), zone->getCenterPoint().getAltM());

         double distance = targetPosition.GroundRangeTo(center);

         if (distance < groundRangeDA)
         {
            groundRangeDA = distance;
         }
      }
      if (groundRangeDA == DBL_MAX)
      {
         groundRangeDA = 0;
      }
      ground_range_DA = groundRangeDA;
   }
   return;
}

//***************************************************
//* Status functions
//***************************************************

// RWR frequency band must be tuned so that all enemy sensors with frequencies within the band may be considered TTRs.
// We assume that any TTR sensed by the RWR has a lock on us. Returns true if RWR reports ANY TTR locks.
bool WsfWMAIEngagementMod::TTRLock()
{
   return RWRDetectedThreatCount() > 0;
}

// Returns true if RWR's active track list contains a Track with a TrackId that matches attacker_unit_id.
bool WsfWMAIEngagementMod::TTRLock(const il::idRecord& attacker_unit_id)
{
   auto trackList = RWRDetectedThreatList();
   for (int i = 0, count = trackList.GetTrackCount(); i != count; ++i)
   {
      auto track = trackList.GetTrackEntry(i);
      if (WsfBMUtils::ConvertTrackId(track->GetTrackId()) == attacker_unit_id)
      {
         return true;
      }
   }

   return false;
}

// (In Helios, evading is set to "true" by AssessResponse before initiating beam response and set to "false" by Restore).
bool WsfWMAIEngagementMod::Evading()
{
   return !Empty() && (Top()->getType() == Behavior::BEAM || Top()->getType() == Behavior::DRAG);
}

// (In Helios, set to "true" by ExecuteAction and set to "false" by CompleteAction).
bool WsfWMAIEngagementMod::TakingAction()
{
   return !Empty() && Top()->getType() == Behavior::TAKEACTION;
}

// Adapted from il::trackRecord::getPropagatedPosition()
WsfGeoPoint WsfWMAIEngagementMod::setMoveInDirection(const vm::Vector3<double> velocity, const double sim_time_s, const double move_time_s/*, WsfGeoPoint& destination*/)
{
   WsfGeoPoint destination;
   auto mover = dynamic_cast<WsfAirMover*>(m_wm_component->GetPlatform()->GetMover());
   if (mover)
   {
      // Get my current position
      vm::Vector3<double> initial_position_XYZ = m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();
      il::positionRecord initial_position;
      initial_position.setXYZ(initial_position_XYZ);

      // Calculate the new position in XYZ from my current position, direction, and move time
      vm::Vector3<double> delta_vector(velocity * move_time_s);
      il::positionRecord new_position;
      new_position.setXYZ(initial_position_XYZ + delta_vector);

      // Ensure that destination altitude is the same as initial altitude
      vm::Vector3<double> lla(new_position.getLLA());
      lla.setAlt(initial_position.getAltM());
      new_position.setLLA(lla);

      mover->GoToLocation(sim_time_s, new_position.getLatDegs(), new_position.getLonDegs(), new_position.getAltM());

      // Store projected destination in a WsfGeoPoint, which may be used in functions called after WsfWMAIEngagementMod::setMoveInDirection.
      destination.SetLocationLLA(new_position.getLatDegs(), new_position.getLonDegs(), new_position.getAltM());
   }
   return destination;
}

// Helios: Assess whether to action (in form of presentation) vs assigned target
// If a TakeAction Behavior should be added to the behavior stack, returns true and stores values for appropriate action type and duration in actionType and actionDuration.
bool WsfWMAIEngagementMod::AssessAction(const double sim_time_s, const il::idRecord& assignment_local_track_id, il::eTakeAction& actionType, double& actionDuration)
{
   auto thisAsset = m_wm_component->GetCoreAMRef()->getThisAsset();
   // Helios: if (Assignment && Assignment->AssignedUnitID != MyUnitID) ... is this checking that the assignment isn't to self?
   if (assignment_local_track_id.isValid() && assignment_local_track_id != thisAsset->getID())
   {
      // If I'm firing at this attacker, keep firing ... no need to Action.
      if (AttackerEngaged(assignment_local_track_id))
      {
         return false;
      }
   }

   auto myLocation = m_wm_component->GetCoreAMRef()->getThisAsset()->getPositionXYZ();

   // Query for the associated track information
   auto am = m_wm_component->GetCoreAMRef();
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());
   auto track = il::FindTrack(assignment_local_track_id, am->GetMasterTracksArray());

   if (track.get() != nullptr)
   {
      // Get the predicted position for this target
      auto propagatedPosition = track->getPropagatedPosition(sim_time_s);

      // Get distance to target
      WsfGeoPoint assetPosition, targetPosition;
      assetPosition.SetLocationLLA(thisAsset->getPosition().getLatDegs(), thisAsset->getPosition().getLonDegs(), thisAsset->getPosition().getAltM());
      targetPosition.SetLocationLLA(propagatedPosition.getLatDegs(), propagatedPosition.getLonDegs(), propagatedPosition.getAltM());
      //double distanceToTarget = assetPosition.GetDistanceFrom(targetPosition);
      double distanceToTarget = assetPosition.GroundRangeTo(targetPosition);

      // Target location within action distance?
      if (distanceToTarget <= wm->getEvasionTakeActionDistance())
      {
         // Determine correct action type and duration based on my place in formation
         auto myPlaceInFormation = wm->getPlaceInFormation();
         switch (myPlaceInFormation)
         {
            case 1:
               actionType = wm->getEvasionAction1();
               actionDuration = wm->getEvasionAction1Duration();
               break;
            case 2:
               actionType = wm->getEvasionAction2();
               actionDuration = wm->getEvasionAction2Duration();
               break;
            case 3:
               actionType = wm->getEvasionAction3();
               actionDuration = wm->getEvasionAction3Duration();
               break;
            default:
               actionType = wm->getEvasionAction4();
               actionDuration = wm->getEvasionAction4Duration();
               break;
         }
         return true; // TakeAction Behavior should be added to behavior stack
      }
   }
   return false;
}

void WsfWMAIEngagementMod::Egress(const double sim_time_s)
{
   auto am = m_wm_component->GetCoreAMRef();
   auto wm = static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get());
   auto asset = am->getMyAsset();

   auto mover = dynamic_cast<WsfAirMover*>(m_wm_component->GetPlatform()->GetMover());
   if (!mover)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time_s << ": " << asset.getName() << " attempted to perform AI Egress, but could not get WsfAirMover.";
      return;
   }

   // Used for logging
   std::string egressMethod;
   std::string corridorName = "No corridor";

   auto corridors = asset.GetZones(il::E_ZONE_TYPE_COR);

   if (corridors.empty())
   {
      if (wm->getReturnToCAP() && asset.hasMunitions() && !wm->getIsLowOnFuel())
      {
         mover->ReturnToRoute(sim_time_s);
         egressMethod = "CAP";
      }
      else
      {
         il::positionRecord base = wm->getHomeBasePosition();
         double speed = m_wm_component->GetPlatform()->GetSpeed();

         float groundElevation;
         m_wm_component->GetPlatform()->GetTerrain().GetElevInterp(base.getLatDegs(), base.getLonDegs(), groundElevation);

         //WsfWaypoint highpoint = WsfWaypoint(base.getLatDegs(), base.getLonDegs(), asset.getPosition().getAltM(), speed);
         WsfWaypoint lowpoint = WsfWaypoint(base.getLatDegs(), base.getLonDegs(), groundElevation, speed);

         WsfRoute route = WsfRoute();
         //route.Append(highpoint);
         route.Append(lowpoint);

         mover->SetEndOfPath(WsfPath::cEP_STOP);
         mover->UpdateRoute(sim_time_s, route);

         egressMethod = "Land";
      }
   }
   else
   {
      auto closestCorridor = corridors.at(0);
      double shortestDistance = closestCorridor->getDistanceToZone(asset.getPosition());

      for (auto iter = ++(corridors.cbegin()); iter != corridors.cend(); ++iter)
      {
         auto corridor = *iter;
         double distance = corridor->getDistanceToZone(asset.getPosition());

         if (distance < shortestDistance)
         {
            shortestDistance = distance;
            closestCorridor = corridor;
         }
      }

      corridorName = closestCorridor->getName();

      il::positionRecord entrancePosition;
      if (!closestCorridor->getClosestPointInZone(asset.getPosition(), entrancePosition))
      {
         entrancePosition = closestCorridor->getCenterPoint();
      }
      auto entrancePoint = WsfWaypoint(entrancePosition.getLatDegs(), entrancePosition.getLonDegs(), asset.getPosition().getAltM(), m_wm_component->GetPlatform()->GetSpeed());

      il::positionRecord exitPosition;
      if (!closestCorridor->getClosestPointInZone(wm->getHomeBasePosition(), exitPosition))
      {
         exitPosition = closestCorridor->getCenterPoint();
      }
      auto exitPoint = WsfWaypoint(exitPosition.getLatDegs(), exitPosition.getLonDegs(), asset.getPosition().getAltM(), m_wm_component->GetPlatform()->GetSpeed());

      WsfRoute route = WsfRoute();
      route.Append(entrancePoint);
      route.Append(exitPoint);

      if (wm->getReturnToCAP() && asset.hasMunitions() && !wm->getIsLowOnFuel())
      {
         route.Append(*mover->GetDefaultRoute());
         egressMethod = "CAP";
      }
      else
      {
         il::positionRecord base = wm->getHomeBasePosition();
         double speed = m_wm_component->GetPlatform()->GetSpeed();

         float groundElevation;
         m_wm_component->GetPlatform()->GetTerrain().GetElevInterp(base.getLatDegs(), base.getLonDegs(), groundElevation);

         //WsfWaypoint highpoint = WsfWaypoint(base.getLatDegs(), base.getLonDegs(), asset.getPosition().getAltM(), speed);
         WsfWaypoint lowpoint = WsfWaypoint(base.getLatDegs(), base.getLonDegs(), groundElevation, speed);

         //route.Append(highpoint);
         route.Append(lowpoint);

         mover->SetEndOfPath(WsfPath::cEP_STOP);
         egressMethod = "Land";
      }

      mover->UpdateRoute(sim_time_s, route);
   }

   auto thisAsset = m_wm_component->GetCoreAMRef()->getThisAsset();
   IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger()->AIEgress(sim_time_s, thisAsset.get(), egressMethod, corridorName, m_wm_component->GetPlatform()->GetSpeed());

   return;
}

void WsfWMAIEngagementMod::PerformSelfDefense()
{
   // TODO
}

void WsfWMAIEngagementMod::CANTCOAllAssignments(const std::string& reason)
{
   auto am_proc = m_wm_component->GetCoreAMRef();
   il::moeLoggingInterface* logger = IADSC2SimulationExtension::Find(*m_wm_component->GetSimulation())->GetLogger();
   const auto& our_assignments = am_proc->getOurAssignments(*logger); // getOurAssignments() only returns assignments that are not complete and for which assignedID != our ID

   for (auto it = our_assignments.cbegin(), end = our_assignments.cend(); it != end; ++it)
   {
      auto eval_assignment = *it;
      am_proc->cantcoAssignment(*logger, eval_assignment.Assignment(), reason);
   }
   return;
}

bool WsfWMAIEngagementMod::EmployingActionTechniques()
{
   return static_cast<il::weaponsManagerAI*>(m_wm_component->GetCoreWMRef().get())->getEvasionTakeAction();
}

bool WsfWMAIEngagementMod::isParked()
{
   return m_wm_component->GetCoreAMRef()->getMyAsset().getSpeed() < 1.0;
}

std::shared_ptr<WsfInterceptCalculator> WsfWMAIEngagementMod::GetScriptCalculator(const il::evaluationAssignment& assignment)
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

bool WsfWMAIEngagementMod::IsEngagementComplete(const il::evaluationAssignment& assignment, bool& was_successful) const
{
   auto sim = m_wm_component->GetSimulation();
   auto trackId = WsfBMUtils::ConvertTrackId(assignment->getLocalTrackID());
   auto wpn = WsfBMUtils::GetAssignedWeaponFromAssignedIDRecord(sim, assignment->getAssignedID());
   auto local_track = m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(assignment->getLocalTrackID()));
   if (!wpn)
   {
      HCL_FATAL_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMAIEngagementMod::IsEngagementComplete(): cannot find weapon: " << assignment->getAssignedID() << std::endl;
      // Assignment will be cantco'd soon. Returning true here may result in a false havco.
      return false;
   }

   bool engagement_complete = was_successful = false;

   auto shotDoctrine = assignment->getShotDoctrine();

   int expected_qty = 1;
   if (shotDoctrine == il::SHOOT_2 ||
       (shotDoctrine == il::SHOOT_LOOK_SHOOT && WsfBMUtils::BelievedAlive(sim, local_track)))
   {
      expected_qty = 2;
   }

   auto reporting_style = m_wm_component->GetCoreWMRef()->getHavecoReportingStyle();
   switch (reporting_style)
   {
      case il::WeaponsManagerInterface::E_ON_LAUNCH:
         engagement_complete = was_successful = assignment->getSalvosFired() >= expected_qty;
         break;
      case il::WeaponsManagerInterface::E_ON_DETONATION:
         engagement_complete = was_successful = assignment->getSalvosFired() >= expected_qty && wpn->WeaponsActiveAndPendingFor(trackId) == 0;
         break;
      case il::WeaponsManagerInterface::E_ON_KILL:
      {
         auto confirmed_dead = WsfBMUtils::ConfirmedDead(sim, local_track);

         if ((assignment->getSalvosFired() >= expected_qty && (wpn->WeaponsActiveAndPendingFor(trackId) == 0)) ||
             ((assignment->getSalvosFired() >= 0) && (wpn->RoundsCompleteFor(trackId) > (wpn->RoundsFiredAt(trackId) - assignment->getSalvosFired())) && confirmed_dead))
         {
            engagement_complete = true;
            was_successful = confirmed_dead;
         }
      }
      break;
      default:
         HCL_FATAL_LOGGER(m_wm_component->GetCoreAMRef()->getGlobalLogger()) << "WsfWMAIEngagementMod::IsEngagementComplete(): invalid HAVECO reporting style: " << reporting_style << std::endl;
         throw UtException("WsfWMAIEngagementMod::IsEngagementComplete(): invalid HAVECO reporting style");
   }

   return engagement_complete;
}

bool WsfWMAIEngagementMod::AssignmentHasValidTrack(const il::evaluationAssignment& eval_assignment) const
{
   bool has_track = true;

   auto am_proc = m_wm_component->GetCoreAMRef();
   auto this_asset = am_proc->getThisAsset();
   auto wm_proc = m_wm_component->GetCoreWMRef();

   std::shared_ptr<il::assignmentMessage> assignment = eval_assignment.Assignment();
   auto track = eval_assignment.Track();

   if (!track ||
       !am_proc->getMasterTrack(track->getID()) ||
       !m_wm_component->GetPlatform()->GetTrackManager().FindTrack(WsfBMUtils::ConvertTrackId(track->getID())))
   {
      has_track = false;
   }

   return has_track;
}

bool WsfWMAIEngagementMod::TimeForSecondSalvo(const il::evaluationAssignment& assignment, WsfWeapon* weapon, const double sim_time_s) const
{
   WsfTrackId trackId = WsfBMUtils::ConvertTrackId(assignment->getLocalTrackID());

   return assignment->getShotDoctrine() == il::SHOOT_LOOK_SHOOT &&
          assignment->getSalvosFired() == 1 &&
          weapon->WeaponsActiveAndPendingFor(trackId) == 0;
}
