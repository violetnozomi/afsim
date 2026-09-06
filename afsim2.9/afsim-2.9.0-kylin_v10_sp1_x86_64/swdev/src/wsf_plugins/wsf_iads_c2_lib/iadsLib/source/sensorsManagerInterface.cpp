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
*   File: sensorsManagerInterface.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: Sensors manager superclass.
*
*******************************************************************************/

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/sensorsManagerInterface.hpp>
#include <iadsLib/cueMessage.hpp>

namespace il
{

SensorsManagerInterface::SensorsManagerInterface()
   : ReferencesAssetManager()
   , m_max_tar_acq_time(60)
   , m_turn_of_tar_if_no_tracks(false)
   , m_max_ttr_acq_time(60)
   , m_turn_of_ttr_if_no_tracks(true)
   , m_assignment_algorithm(E_FIRST_FOUND)
{
}


SensorsManagerInterface::SensorsManagerInterface(const SensorsManagerInterface& from)
   : ReferencesAssetManager(from)
   , m_max_tar_acq_time(from.m_max_tar_acq_time)
   , m_turn_of_tar_if_no_tracks(from.m_turn_of_tar_if_no_tracks)
   , m_max_ttr_acq_time(from.m_max_ttr_acq_time)
   , m_turn_of_ttr_if_no_tracks(from.m_turn_of_ttr_if_no_tracks)
   , m_assignment_algorithm(from.m_assignment_algorithm)
   // clear out responsible assignments
{
}

SensorsManagerInterface* SensorsManagerInterface::Clone()
{
   return new SensorsManagerInterface(*this);
}

SensorsManagerInterface::~SensorsManagerInterface()
{
}

void SensorsManagerInterface::SetMaxTARAcqTime(const double max_time_s)
{
   m_max_tar_acq_time = max_time_s;
}

double SensorsManagerInterface::GetMaxTARAcqTime() const
{
   return m_max_tar_acq_time;
}

void SensorsManagerInterface::SetTurnOffTARNoTracks(const bool turn_off)
{
   m_turn_of_tar_if_no_tracks = turn_off;
}

bool SensorsManagerInterface::GetTurnOffTARNoTracks() const
{
   return m_turn_of_tar_if_no_tracks;
}

void SensorsManagerInterface::SetMaxTTRAcqTime(const double max_time_s)
{
   m_max_ttr_acq_time = max_time_s;
}

double SensorsManagerInterface::GetMaxTTRAcqTime() const
{
   return m_max_ttr_acq_time;
}

void SensorsManagerInterface::SetTurnOffTTRNoTracks(const bool turn_off)
{
   m_turn_of_ttr_if_no_tracks = turn_off;
}

bool SensorsManagerInterface::GetTurnOffTTRNoTracks() const
{
   return m_turn_of_ttr_if_no_tracks;
}

void SensorsManagerInterface::ProcessResponsibleAssignments(il::moeLoggingInterface& aMOELogger, const double sim_time)
{
   auto am = GetAssetManager();

   auto proc_resp_assignment = [&](il::moeLoggingInterface& aMOELogger, const tTrackID &master_track_id, ResponsibleRadarAssignment &responsible_assignment) -> void
   {
      evaluationTrack null_track(am->getGlobalLogger());
      evaluationAssignment eval_assign(null_track, nullptr);
      auto got_assignment = am->getEvaluationAssignment(master_track_id, responsible_assignment.GetAssignment()->getAssignedID(), eval_assign);
      auto actual_assignment = eval_assign.Assignment();
      if (got_assignment && !actual_assignment->isAssignmentComplete() && responsible_assignment.IsRadarCued())
      {
         // have an active assignment and a radar has been cued, check if the radar has found the target
         if (RadarTargetFound(master_track_id, responsible_assignment))
         {
            ProcessAssignmentTrackingTime(aMOELogger, sim_time, responsible_assignment, eval_assign);
            // note: HELIOS spread out radar management code into various spots, we will now consolidate it into
            // the sensor manager drop completed assignments code
         }
         else
         {
            // didn't find target see if it's time to CANTCO the assignment
            if (TimeToCANTCOAssignment(sim_time, responsible_assignment))
            {
               am->cantcoAssignment(aMOELogger, actual_assignment, DetermineCANTCOReason(responsible_assignment));
            }
         }
      }
   };

   m_responsible_assignments.ForeachResponsibleAssignment(aMOELogger, proc_resp_assignment);
}

void SensorsManagerInterface::ProcessAssignment(const double sim_time, evaluationAssignment& eval_assignment)
{
   auto am = GetAssetManager();
   auto assignment = eval_assignment.Assignment();

   if (assignment->isAssignmentComplete())
   {
      return;
   }

   // get master track
   const auto& master_track_record = am->getMasterTrack(assignment->getLocalTrackID());
   if (!master_track_record)
   {
      HCL_ERROR_LOGGER(GetAssetManager()->getGlobalLogger()) << am->getThisAsset()->getName() << ": Error, could not find master track for cueing sensors. Assignment: " << assignment->getAssignedID() << " local track id: " << assignment->getLocalTrackID();
      return;
   }

   // determine the next cue we need to make
   bool time_to_transition = false;
   il::SensorEnum next_radar_type = il::SENSOR_UNKNOWN;

   std::tie(time_to_transition, next_radar_type) = DetermineNextCueType(assignment, master_track_record);
   if (time_to_transition)
   {
      auto radar_assigned = AssignedRadarToAssignment(sim_time, next_radar_type, master_track_record, assignment);
      // Note: this functionality replicates the if (!RadarAssigned logic of GTIQBSensorsManagerProcessAssignment
      // for the EW track source case
      if (!radar_assigned && next_radar_type == il::SENSOR_TAR)
      {
         radar_assigned = AssignedRadarToAssignment(sim_time, il::SENSOR_TTR, master_track_record, assignment, true);
      }
   }
}

void SensorsManagerInterface::DropCompletedAssignments(il::moeLoggingInterface& aMOELogger, double sim_time)
{

   // foreach (responsible assignment)
   //   get actual assignment
   //   if (TTR cued && TAR.responsible())
   //       cancel TAR cue
   //   if (assignment && assignment->IsComplete()
   //       cancel Responsible Cues
   //   if (!assignment)
   //       cancel Responsible Cues
   //
   //  note: ProcessResponsibleAssignments() handles the case where we haven't found the right quality of
   //     track within the search time and will cantco the assignment as a result

   auto am = GetAssetManager();

   // note: in general, we must do bookkeeping before SendSensorCueMessage() if we're doing cue cancellations
   auto cancel_responsible_cues = [&](il::moeLoggingInterface& aMOELogger, const idRecord &master_track_id, ResponsibleRadarAssignment &responsible_assignment) -> void
   {
      // TAR
      if (responsible_assignment.HasTARResponsibility())
      {
         auto responsible_sensor = responsible_assignment.GetResponsibleTAR();
         auto track = responsible_assignment.GetTrackRecord();
         auto assignment = responsible_assignment.GetAssignment();
         auto is_tar_queued = responsible_assignment.IsTARCued();
         responsible_assignment.RemoveTARResponsibility();
         if (is_tar_queued)
         {
            SendSensorCueMessage(aMOELogger, sim_time, track, assignment, responsible_sensor, cueMessage::CANCEL);
         }
      }

      // TTR
      if (responsible_assignment.HasTTRResponsibility())
      {
         auto responsible_sensor = responsible_assignment.GetResponsibleTTR();
         auto track = responsible_assignment.GetTrackRecord();
         auto assignment = responsible_assignment.GetAssignment();
         auto is_ttr_queued = responsible_assignment.IsTTRCued();
         responsible_assignment.RemoveTTRResponsibility();
         if (is_ttr_queued)
         {
            SendSensorCueMessage(aMOELogger, sim_time, track, assignment, responsible_sensor, cueMessage::CANCEL);
         }
      }
   };

   std::vector<il::idRecord> to_delete_resp_assignments;
   auto drop_completed_assignment = [&](il::moeLoggingInterface& aMOELogger, const tTrackID &master_track_id, ResponsibleRadarAssignment &responsible_assignment) -> void
   {
      evaluationTrack null_track(am->getGlobalLogger());
      evaluationAssignment eval_assign(null_track, nullptr);
      am->getEvaluationAssignment(master_track_id, responsible_assignment.GetAssignment()->getAssignedID(), eval_assign);
      auto actual_assignment = eval_assign.Assignment();
      const auto& master_track_record = am->getMasterTrack(master_track_id);
      bool delete_resp_assignment = false;

      if (responsible_assignment.HasTTRResponsibility() &&
          responsible_assignment.IsTTRCued() &&
          (master_track_record && master_track_record->getReportingSensorTypeEnum() == il::SENSOR_EW) &&
          (!responsible_assignment.WasTARSkipped()))
      {
         // the TTR could have been on a track and now it has degraded due to not aquiring it, remove TTR responsibility
         HCL_DEBUG_LOGGER(GetAssetManager()->getGlobalLogger()) << "Had TTR responsibility but track must have degraded, dropping TTR responsibility.";

         auto responsible_sensor = responsible_assignment.GetResponsibleTTR();
         auto track = responsible_assignment.GetTrackRecord();
         auto assignment = responsible_assignment.GetAssignment();
         responsible_assignment.RemoveTTRResponsibility();
         SendSensorCueMessage(aMOELogger, sim_time, track, assignment, responsible_sensor, cueMessage::CANCEL);
      }


      // see if we have moved onto the next sensor, turn off previous
      if (responsible_assignment.IsTTRCued() && responsible_assignment.IsTTRTracking() && responsible_assignment.HasTARResponsibility())
      {
         auto responsible_sensor = responsible_assignment.GetResponsibleTAR();
         auto track = responsible_assignment.GetTrackRecord();
         auto assignment = responsible_assignment.GetAssignment();
         auto is_tar_queued = responsible_assignment.IsTARCued();
         responsible_assignment.RemoveTARResponsibility();
         if (is_tar_queued)
         {
            SendSensorCueMessage(aMOELogger, sim_time, track, assignment, responsible_sensor, cueMessage::CANCEL);
         }
      }

      // assignment complete?
      if (actual_assignment && actual_assignment->isAssignmentComplete())
      {
         cancel_responsible_cues(aMOELogger, master_track_id, responsible_assignment);
         delete_resp_assignment = true;
      }

      if (!actual_assignment)
      {
         cancel_responsible_cues(aMOELogger, master_track_id, responsible_assignment);
         delete_resp_assignment = true;
      }

      if (delete_resp_assignment)
      {
         to_delete_resp_assignments.push_back(master_track_id);
      }
   };

   m_responsible_assignments.ForeachResponsibleAssignment(aMOELogger, drop_completed_assignment);

   // delete
   for (auto it = to_delete_resp_assignments.cbegin(); it != to_delete_resp_assignments.cend(); ++it)
   {
      const auto& master_track_id = *it;
      m_responsible_assignments.Delete(master_track_id);
   }
}

void SensorsManagerInterface::MasterTrackUpdated(const double sim_time, std::shared_ptr<il::trackRecord> master_track)
{
   auto am = GetAssetManager();

   if (m_responsible_assignments.HasResponsibility(master_track))
   {
      auto& ra = m_responsible_assignments.GetResponsibleAssignment(master_track, GetAssetManager()->getGlobalLogger());
      ra.MasterTrackUpdated(sim_time, am->getThisAsset()->getID(), master_track);
   }
}

std::pair<bool, il::SensorEnum> SensorsManagerInterface::DetermineNextCueType(const std::shared_ptr<il::assignmentMessage>& assignment,
                                                                              const std::shared_ptr<il::trackRecord>& master_track)
{
   if (m_responsible_assignments.HasResponsibility(master_track))
   {
      ResponsibleRadarAssignment resp_assignment =
         m_responsible_assignments.GetResponsibleAssignment(master_track, assignment, GetAssetManager()->getGlobalLogger());
      switch (master_track->getReportingSensorTypeEnum())
      {
         case il::SENSOR_UNKNOWN:
            break;
         case il::SENSOR_RWR:
            break;
         case il::SENSOR_EW:
            if (!resp_assignment.HasTARResponsibility() && !resp_assignment.HasTTRResponsibility())
            {
               return std::make_pair(true, il::SENSOR_TAR);
            }
            break;
         case il::SENSOR_TAR:
            if (!resp_assignment.HasTTRResponsibility())
            {
               return std::make_pair(true, il::SENSOR_TTR);
            }
            break;
         case il::SENSOR_TTR:
            break;
         default:
            break;
      }
   }
   else
   {
      switch (master_track->getReportingSensorTypeEnum())
      {
         case il::SENSOR_UNKNOWN:
            break;
         case il::SENSOR_RWR:
            break;
         case il::SENSOR_EW:
            return std::make_pair(true, il::SENSOR_TAR);
         case il::SENSOR_TAR:
         case il::SENSOR_TTR:
            return std::make_pair(true, il::SENSOR_TTR);
         default:
            break;
      }
   }

   return std::make_pair(false, il::SENSOR_UNKNOWN);
}

bool SensorsManagerInterface::AssignedRadarToAssignment(const double sim_time,
                                                        const il::SensorEnum sensor_type,
                                                        const std::shared_ptr<il::trackRecord>& master_track,
                                                        const std::shared_ptr<assignmentMessage>& assignment,
                                                        const bool skipped_previous_sensor)
{
   std::shared_ptr<il::assetRecord> delegated_sensor_manager;
   std::shared_ptr<il::sensorRecord> delegated_sensor;

   // search subordinates for a sensor of the desired type.
   // priority: 1) assigned unit
   //           2) first unit found from BFS
   auto am = GetAssetManager();
   auto& assets = am->getAssets();
   auto this_asset = am->getThisAsset();

   // check assigned unit for sensor type and a sensor manager, if so, delegate responsibility to that
   // platform as long as it's not us
   std::shared_ptr<sensorRecord> sensor_to_assign;
   auto assigned_asset = assets.GetAsset(assignment->getAssignedID().getID());
   if (assigned_asset &&
       assigned_asset->isGreen() &&
       (sensor_to_assign = assigned_asset->getNthSensorOfType(1, sensor_type)) &&
       // sensor_to_assign->HasEstimatedAvailableCues() && // Note: HELIOS doesn't have the issue since all TTRs have unlimited requests.
       // In this case of a stacked SM with an all-in-one SAM, it is a special case and conceivable that all SM responsiblity
       // should go to that SM regardless of CUE availability.
       assigned_asset->hasSensorManager() &&
       assigned_asset->getID() != this_asset->getID())
   {
      // note: we use the same interface for cueing regardless of the destination.
      // The dissemination proc has the ability to sort this even if it's destination is the same platform.
      delegated_sensor_manager = assigned_asset;
      delegated_sensor = sensor_to_assign;
      // technically, we are not assigning to a sensor, we are delegating, reset sensor here
      sensor_to_assign.reset();

   }
   // check assigned unit for sensor type - we already know it doesn't have a sensor manager or it is 'self' if we made it this far
   // if it has the correct sensor, take responsibility ownership of it.
   else if (assigned_asset &&
            assigned_asset->isGreen() &&
            (sensor_to_assign = assigned_asset->getNthSensorOfType(1, sensor_type)) &&
            sensor_to_assign->HasEstimatedAvailableCues())
   {
      // note: we use the same interface for cueing regardless of the destination.
      // The dissemination proc has the ability to sort this even if it's destination is the same platform.

   }
   else
   {
      // the assigned unit doesn't have a radar of the appropriate type, find a subordinate that does.
      // note: HELIOS uses a random ordering based on message send order (order entries added in SubordinateStatusArray).
      // we will use a BFS search starting from our platform to all subordinates
      std::vector<std::shared_ptr<sensorRecord>> sensors;
      auto find_all_matching_sensors = [&sensors, sensor_type](std::shared_ptr<assetRecord> subordinate) -> void
      {
         int sidx = 1;
         std::shared_ptr<il::sensorRecord> sensor;
         do
         {
            sensor = subordinate->getNthSensorOfType(sidx++, sensor_type);
            if (subordinate &&
                subordinate->isGreen() &&
                sensor && sensor->HasEstimatedAvailableCues())
            {
               sensors.push_back(sensor);
            }
         } while (sensor);
      };

      this_asset->foreachSubordinateBFS(am->getAssets(), find_all_matching_sensors);

      // if we found at least one matching sensor, take the first
      if (sensors.size())
      {
         sensor_to_assign = sensors[0];
      }
   }

   bool cued_sensor = false;
   // check if we are delegating off-platform first since nothing else needs to be done.
   if (delegated_sensor_manager && delegated_sensor)
   {
      HCL_DEBUG_LOGGER(am->getGlobalLogger()) << am->getThisAsset()->getName() << " delegating ownership of sensor manager responsibility for sensor "
         << delegated_sensor->GetParentAsset()->getName() << ":" << delegated_sensor->GetSensorID()
         << " against assignment track " << master_track->getID() << " and threat " << master_track->getTargetTruthName()
         << " to sensor manager on platform " << delegated_sensor_manager->getName() << " for sensor " << delegated_sensor->GetSensorID();
      cued_sensor = true;
   }
   else if (sensor_to_assign)
   {
      cued_sensor = CueSensorForAssignment(sim_time, master_track, assignment, sensor_to_assign, skipped_previous_sensor);
      if (cued_sensor)
         HCL_DEBUG_LOGGER(am->getGlobalLogger()) << am->getThisAsset()->getName() << " taking ownership of sensor manager responsibility for sensor "
         << sensor_to_assign->GetParentAsset()->getName() << ":" << sensor_to_assign->GetSensorID()
         << " against assignment track " << master_track->getID() << " and threat " << master_track->getTargetTruthName();
   }
   else
   {
      HCL_WARN_LOGGER(am->getGlobalLogger()) << sim_time << ": " << am->getThisAsset()->getName() << " could not find an available sensor to cue. Either all sensors are down or are at max cue capacity. Check that maximum_request_count matches max assignments for TTR sensors and superiors.";
   }

   return cued_sensor; // returns true if we cued or will delegate the sensor for assignment
}

bool SensorsManagerInterface::CueSensorForAssignment(const double sim_time,
                                                     const std::shared_ptr<il::trackRecord>& master_track,
                                                     const std::shared_ptr<assignmentMessage>& assignment,
                                                     std::shared_ptr<sensorRecord>& sensor_to_assign,
                                                     const bool skipped_tar)
{
   bool added_responsibility = false;
   // add responsible assignment
   auto& responsible_assignment = m_responsible_assignments.GetResponsibleAssignment(master_track, assignment, GetAssetManager()->getGlobalLogger());
   if (sensor_to_assign->IsTAR())
   {
      added_responsibility = responsible_assignment.AddTARResponsibility(sensor_to_assign);
   }
   else if (sensor_to_assign->IsTTR())
   {
      added_responsibility = responsible_assignment.AddTTRResponsibility(sensor_to_assign, skipped_tar);
   }
   else
   {
      HCL_WARN_LOGGER(GetAssetManager()->getGlobalLogger()) << "Attempt to add responsible assignment to non-tracking radar type?";
   }


   // note: we don't actually trigger a cue here, we let the asset manager handle dispatching of cues based on the responsible assignments
   return added_responsibility;
}

bool SensorsManagerInterface::RadarTargetFound(const idRecord& master_track_id, ResponsibleRadarAssignment& responsible_assignment) const
{
   bool found_track_of_quality = false;

   auto am = GetAssetManager();

   auto master_track = il::FindTrack(master_track_id, am->GetMasterTracksArray());
   if (master_track)
   {
      if (responsible_assignment.IsTTRCued())
      {
         if (master_track->getReportingSensorTypeEnum() == il::SENSOR_TTR)
         {
            found_track_of_quality = true;
         }
      }
      else if (responsible_assignment.IsTARCued())
      {
         if (master_track->getReportingSensorTypeEnum() == il::SENSOR_TAR)
         {
            found_track_of_quality = true;
         }
      }
   }

   return found_track_of_quality;
}

void SensorsManagerInterface::ProcessAssignmentTrackingTime(il::moeLoggingInterface& aMOELogger,
                                                            const double sim_time,
                                                            ResponsibleRadarAssignment& responsible_assignment,
                                                            evaluationAssignment& eval_assignment)
{
   auto am = GetAssetManager();

   auto master_track = il::FindTrack(responsible_assignment.GetTrackRecord()->getID(), am->GetMasterTracksArray());
   if (!master_track)
   {
      HCL_ERROR_LOGGER(am->getGlobalLogger()) << sim_time << ": Attempted to process assignment tracking time but coult not find master track.";
      return;
   }

   // Note: priority is TTR then TAR
   if (responsible_assignment.HasTTRResponsibility())
   {
      if (!responsible_assignment.IsTTRTracking() && responsible_assignment.IsTTRCued() && master_track->getReportingSensorTypeEnum() == il::SENSOR_TTR)
      {
         // set the senor as tracking for bookkeeping purposes
         responsible_assignment.SetTTRTracking(sim_time);

         // send an assignment status update for tracking (also write a HEFIR message)
         am->sensorTrackingAssignmentStatus(aMOELogger, eval_assignment, responsible_assignment.GetResponsibleTTR().get());
      }
   }
   else if (responsible_assignment.HasTARResponsibility())
   {
      if (!responsible_assignment.IsTARTracking() && responsible_assignment.IsTARCued() && master_track->getReportingSensorTypeEnum() == il::SENSOR_TAR)
      {
         // set the senor as tracking for bookkeeping purposes
         responsible_assignment.SetTARTracking(sim_time);

         // send an assignment status update for tracking (also write a HEFIR message)
         am->sensorTrackingAssignmentStatus(aMOELogger, eval_assignment, responsible_assignment.GetResponsibleTAR().get());
      }
   }
}

bool SensorsManagerInterface::TimeToCANTCOAssignment(const double sim_time, const ResponsibleRadarAssignment& responsible_assignment) const
{
   bool should_cantco = false;

   if (responsible_assignment.HasTTRResponsibility())
   {
      if (responsible_assignment.IsTTRCued() &&
         (sim_time > responsible_assignment.GetCuedTTRTime() + m_max_ttr_acq_time) &&
          !responsible_assignment.IsTTRTracking())
      {
         should_cantco = true;
      }
   }
   else if (responsible_assignment.HasTARResponsibility())
   {
      if (responsible_assignment.IsTARCued() &&
         (sim_time > responsible_assignment.GetCuedTARTime() + m_max_tar_acq_time) &&
          !responsible_assignment.IsTARTracking())
      {
         should_cantco = true;
      }
   }

   return should_cantco;
}

std::string SensorsManagerInterface::DetermineCANTCOReason(const ResponsibleRadarAssignment& responsible_assignment) const
{
   std::string cantco_reason = "**Error ** Unknown";

   // Note: priority is TTR then TAR
   if (responsible_assignment.HasTTRResponsibility())
   {
      cantco_reason = "No TTR acquire";
   }
   else if (responsible_assignment.HasTARResponsibility())
   {
      cantco_reason = "No TAR acquire";
   }

   return cantco_reason;
}

bool SensorsManagerInterface::SendSensorCueMessage(il::moeLoggingInterface& aMOELogger,
                                                   const double sim_time,
                                                   const std::shared_ptr<const trackRecord>& master_track,
                                                   const std::shared_ptr<const assignmentMessage>& assignment,
                                                   const std::shared_ptr<sensorRecord>& sensor,
                                                   const cueMessage::CueReason& reason)
{
   return GetAssetManager()->SendSensorCueMessage(sim_time, master_track->getID(), sensor, reason);
}

void SensorsManagerInterface::CreateSensorCues(il::moeLoggingInterface& aMOELogger, const double sim_time)
{
   auto am = GetAssetManager();
   auto sm = this;

   auto create_cues = [sim_time, &am, &sm](il::moeLoggingInterface& aMOELogger, const tTrackID &master_track_id, ResponsibleRadarAssignment &responsible_assignment) -> void
   {
      std::shared_ptr<il::sensorRecord> sensor_to_cue;
      auto track = responsible_assignment.GetTrackRecord();
      auto assignment = responsible_assignment.GetAssignment();

      if (responsible_assignment.HasTARResponsibility() && !responsible_assignment.IsTARCued())
      {
         sensor_to_cue = responsible_assignment.GetResponsibleTAR();
         if (sm->SendSensorCueMessage(aMOELogger, sim_time, track, assignment, sensor_to_cue, cueMessage::NEW_CUE))
         {
            responsible_assignment.SetTARCued(sim_time);
         }
      }
      else if (responsible_assignment.HasTTRResponsibility() && !responsible_assignment.IsTTRCued())
      {
         sensor_to_cue = responsible_assignment.GetResponsibleTTR();
         if (sm->SendSensorCueMessage(aMOELogger, sim_time, track, assignment, sensor_to_cue, cueMessage::NEW_CUE))
         {
            responsible_assignment.SetTTRCued(sim_time);
         }
      }

   };

   m_responsible_assignments.ForeachResponsibleAssignment(aMOELogger, create_cues);
}

} // namespace il
