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
*   File: assignmentContainers.cpp
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

#include <iadsLib/assignmentContainers.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/threatRecordInterface.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <logger.hpp>

namespace il
{

evaluationAssignment::evaluationAssignment(const evaluationTrack& track,
                                           const std::shared_ptr<assignmentMessage>& evaluated_assignment)
   : m_evaluated_track(track)
   , m_evaluated_assignment(evaluated_assignment)
{
}

const std::shared_ptr<trackRecord>& evaluationAssignment::Track() const
{
   return m_evaluated_track.Track();
}

const std::shared_ptr<assignmentMessage>& evaluationAssignment::DelegateAssignment() const
{
   return m_evaluated_track.DelegateAssignment();
}

const std::shared_ptr<assignmentMessage>& evaluationAssignment::Assignment() const
{
   return m_evaluated_assignment;
}


std::shared_ptr<trackRecord>& evaluationAssignment::Track()
{
   return m_evaluated_track.Track();
}

std::shared_ptr<assignmentMessage>& evaluationAssignment::DelegateAssignment()
{
   return m_evaluated_track.DelegateAssignment();
}

std::shared_ptr<assignmentMessage>& evaluationAssignment::Assignment()
{
   return m_evaluated_assignment;
}

bool evaluationAssignment::IsOverridden() const
{
   bool is_overridden = false;
   // for an evaluation assignment to be overridden, it must:
   // 1) be created off of a delegation assignment: m_evaluated_track.DelegateAssignment() -> true
   // 2) be assigned to a different weapon than the original assignment
   auto delegate_assignment = m_evaluated_track.DelegateAssignment();
   if (delegate_assignment &&
       delegate_assignment->getAssignedID() != m_evaluated_assignment->getAssignedID())
   {
      is_overridden = true;
   }

   return is_overridden;
}

bool evaluationAssignment::IsShotDoctrineUpdated() const
{
   bool is_updated = false;
   // for an shot doctrine to be updated, it must:
   // 1) be created off of a delegation assignment: m_evaluated_track.DelegateAssignment() -> true
   // 2) be assigned a different doctrine than the original assignment
   auto delegate_assignment = m_evaluated_track.DelegateAssignment();
   if (delegate_assignment &&
       delegate_assignment->getShotDoctrine() != m_evaluated_assignment->getShotDoctrine())
   {
      is_updated = true;
   }

   return is_updated;
}

const std::shared_ptr<assignmentMessage>& evaluationAssignment::operator->() const
{
   return m_evaluated_assignment;
}

evaluationAssignment::operator const std::shared_ptr<assignmentMessage>& ()  const
{
   return m_evaluated_assignment;
}

evaluationAssignment::operator std::shared_ptr<assignmentMessage>& ()
{
   return m_evaluated_assignment;
}


// Sensor-manager related bookkeeping

/////////////////////////
// ResponsibleRadarCue //
/////////////////////////
ResponsibleRadarCue::ResponsibleRadarCue(std::shared_ptr<sensorRecord> sensor, const bool skipped_previous)
   : m_is_cued(false)
   , m_cue_time_s(-1)
   , m_is_tracking(false)
   , m_tracking_time(-1)
   , m_sensor(sensor)
   , m_skipped_previous(skipped_previous)
{ }

std::shared_ptr<sensorRecord> ResponsibleRadarCue::GetSensor() const
{
   return m_sensor;
}

void ResponsibleRadarCue::SetCued(const double cue_time_s)
{
   m_cue_time_s = cue_time_s;
   m_is_cued = true;
}

bool ResponsibleRadarCue::IsCued() const
{
   return m_is_cued;
}

double ResponsibleRadarCue::GetCuedTime() const
{
   return m_cue_time_s;
}

void ResponsibleRadarCue::SetTracking(const double tracking_time_s)
{
   m_tracking_time = tracking_time_s;
   m_is_tracking = true;
}

bool ResponsibleRadarCue::IsTracking() const
{
   return m_is_tracking;
}

double ResponsibleRadarCue::GetTrackingTime() const
{
   return m_tracking_time;
}


bool ResponsibleRadarCue::SkippedPreviousSensor() const
{
   return m_skipped_previous;
}


////////////////////////////////
// ResponsibleRadarAssignment //
////////////////////////////////
ResponsibleRadarAssignment::ResponsibleRadarAssignment(std::shared_ptr<trackRecord>       track,
                                                       std::shared_ptr<assignmentMessage> assignment,
                                                       std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : m_assignment_master_track(track),
     m_assignment(assignment),
     m_has_responsible_tar(false),
     m_responsible_tar(std::shared_ptr<sensorRecord>(nullptr)),
     m_has_responsible_ttr(false),
     m_responsible_ttr(std::shared_ptr<sensorRecord>(nullptr)),
     mGlobalLogger(aGlobalLogger)
{  }

void ResponsibleRadarAssignment::MasterTrackUpdated(const double sim_time,
                                                    const idRecord& invoking_asset,
                                                    const std::shared_ptr<const trackRecord>& track)
{
   m_assignment_master_track = track;

   if (m_has_responsible_ttr && m_responsible_ttr.IsCued())
   {
      m_responsible_ttr.GetSensor()->CueTrackUpdated(sim_time, invoking_asset, track->getID());
   }
   else if (m_has_responsible_tar && m_responsible_tar.IsCued())
   {
      m_responsible_tar.GetSensor()->CueTrackUpdated(sim_time, invoking_asset, track->getID());
   }
}

IADSLIB_EXPORT bool ResponsibleRadarAssignment::HasTARResponsibility() const
{
   return m_has_responsible_tar;
}

bool ResponsibleRadarAssignment::IsTARCued() const
{
   return m_has_responsible_tar && m_responsible_tar.IsCued();
}

void ResponsibleRadarAssignment::SetTARTracking(const double time_s)
{
   if (m_has_responsible_tar)
   {
      m_responsible_tar.SetTracking(time_s);
      return;
   }

   HCL_FATAL_LOGGER(mGlobalLogger) << "ResponsibleRadarAssignment::SetTARTracking. Attempt to set a tracking time when a TTR is not responsible";
   throw ("ResponsibleRadarAssignment::SetTARTracking. Attempt to set a tracking time when a TTR is not responsible");
}

bool ResponsibleRadarAssignment::IsTARTracking() const
{
   return m_has_responsible_tar && m_responsible_tar.IsTracking();
}

double ResponsibleRadarAssignment::GetTARTrackingTime() const
{
   return m_has_responsible_tar ? m_responsible_tar.GetTrackingTime() : -1;
}

IADSLIB_EXPORT std::shared_ptr<sensorRecord> ResponsibleRadarAssignment::GetResponsibleTAR() const
{
   return m_has_responsible_tar ? m_responsible_tar.GetSensor() : std::shared_ptr<sensorRecord>();
}

double ResponsibleRadarAssignment::GetCuedTARTime() const
{
   return m_has_responsible_tar ? m_responsible_tar.GetCuedTime() : -1;
}

void ResponsibleRadarAssignment::SetTARCued(const double time_s)
{
   if (m_has_responsible_tar)
   {
      m_responsible_tar.SetCued(time_s);
      return;
   }

   HCL_FATAL_LOGGER(mGlobalLogger) << "ResponsibleRadarAssignment::SetTARCued. Attempt to set a cued time when a TAR is not responsible";
   throw ("ResponsibleRadarAssignment::SetTARCued. Attempt to set a cued time when a TAR is not responsible");
}

bool ResponsibleRadarAssignment::AddTARResponsibility(std::shared_ptr<sensorRecord>& sensor)
{
   if (m_has_responsible_tar)
   {
      return false;
   }

   m_responsible_tar = ResponsibleRadarCue(sensor);
   m_has_responsible_tar = true;
   sensor->DecrementAvailableCues(1);

   return true;
}

void ResponsibleRadarAssignment::RemoveTARResponsibility()
{
   if (m_has_responsible_tar)
   {
      m_responsible_tar.GetSensor()->IncrementAvailableCues(1);
   }

   m_has_responsible_tar = false;
}

IADSLIB_EXPORT bool ResponsibleRadarAssignment::HasTTRResponsibility() const
{
   return m_has_responsible_ttr;
}

bool ResponsibleRadarAssignment::IsTTRCued() const
{
   return m_has_responsible_ttr && m_responsible_ttr.IsCued();
}

bool ResponsibleRadarAssignment::WasTARSkipped() const
{
   return m_has_responsible_ttr && m_responsible_ttr.SkippedPreviousSensor();
}

void ResponsibleRadarAssignment::SetTTRTracking(const double time_s)
{
   if (m_has_responsible_ttr)
   {
      m_responsible_ttr.SetTracking(time_s);
      return;
   }

   HCL_FATAL_LOGGER(mGlobalLogger) << "ResponsibleRadarAssignment::SetTTRTracking. Attempt to set a tracking time when a TTR is not responsible";
   throw ("ResponsibleRadarAssignment::SetTTRCued. Attempt to set a tracking time when a TTR is not responsible");
}

bool ResponsibleRadarAssignment::IsTTRTracking() const
{
   return m_has_responsible_ttr && m_responsible_ttr.IsTracking();
}

double ResponsibleRadarAssignment::GetTTRTrackingTime() const
{
   return m_has_responsible_ttr ? m_responsible_ttr.GetTrackingTime() : -1;
}

IADSLIB_EXPORT std::shared_ptr<sensorRecord> ResponsibleRadarAssignment::GetResponsibleTTR() const
{
   return m_has_responsible_ttr ? m_responsible_ttr.GetSensor() : std::shared_ptr<sensorRecord>();
}

double ResponsibleRadarAssignment::GetCuedTTRTime() const
{
   return m_has_responsible_ttr ? m_responsible_ttr.GetCuedTime() : -1;
}

void ResponsibleRadarAssignment::SetTTRCued(const double time_s)
{
   if (m_has_responsible_ttr)
   {
      m_responsible_ttr.SetCued(time_s);
      return;
   }

   HCL_FATAL_LOGGER(mGlobalLogger) << "ResponsibleRadarAssignment::SetTTRCued. Attempt to set a cued time when a TTR is not responsible";
   throw ("ResponsibleRadarAssignment::SetTTRCued. Attempt to set a cued time when a TTR is not responsible");
}

bool ResponsibleRadarAssignment::AddTTRResponsibility(std::shared_ptr<sensorRecord>& sensor, const bool skipping_tar)
{
   if (m_has_responsible_ttr)
   {
      return false;
   }

   m_responsible_ttr = ResponsibleRadarCue(sensor, skipping_tar);
   m_has_responsible_ttr = true;
   sensor->DecrementAvailableCues(1);

   return true;
}

void ResponsibleRadarAssignment::RemoveTTRResponsibility()
{
   if (m_has_responsible_ttr)
   {
      m_responsible_ttr.GetSensor()->IncrementAvailableCues(1);
   }

   m_has_responsible_ttr = false;
}


bool ResponsibleRadarAssignment::IsRadarCued() const
{
   return IsTARCued() || IsTTRCued();
}


////////////////////////////
// ResponsibleAssignments //
////////////////////////////
bool ResponsibleAssignments::HasResponsibility(const std::shared_ptr<const trackRecord>& assignment_master_track) const
{
   return m_responsible_assignments.find(assignment_master_track->getID()) != m_responsible_assignments.end();
}


ResponsibleRadarAssignment& ResponsibleAssignments::GetResponsibleAssignment(std::shared_ptr<trackRecord> assignment_master_track,
                                                                             std::shared_ptr<assignmentMessage> assignment, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   if (m_responsible_assignments.find(assignment_master_track->getID()) == m_responsible_assignments.end())
   {
      m_responsible_assignments.insert(std::make_pair(assignment_master_track->getID(),
                                                      ResponsibleRadarAssignment(assignment_master_track, assignment, aGlobalLogger)));
   }

   auto assignment_iter = m_responsible_assignments.find(assignment_master_track->getID());

   return assignment_iter->second;
}

ResponsibleRadarAssignment& ResponsibleAssignments::GetResponsibleAssignment(std::shared_ptr<trackRecord> assignment_master_track, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   auto assignment_iter = m_responsible_assignments.find(assignment_master_track->getID());
   if (assignment_iter != m_responsible_assignments.end())
   {
      return assignment_iter->second;
   }


   HCL_FATAL_LOGGER(aGlobalLogger) << "ResponsibleAssignments::GetResponsibleAssignment() unsafe call when no cue exists for master track: "
                      << assignment_master_track->getID();
   throw std::runtime_error("ResponsibleAssignments::GetResponsibleAssignment() unsafe call when no cue exists for master track");

   static ResponsibleRadarAssignment invalid_ra(nullptr, nullptr, aGlobalLogger);

   return invalid_ra;
}

IADSLIB_EXPORT void ResponsibleAssignments::ForeachResponsibleAssignment(il::moeLoggingInterface& aMOELogger,
                                                                         std::function<void(il::moeLoggingInterface& aMOELogger,
                                                                         const tTrackID& master_track_id,
                                                                         ResponsibleRadarAssignment& responsible_assignment)> to_invoke)
{
   for (auto it = m_responsible_assignments.begin(); it != m_responsible_assignments.end(); ++it)
   {
      to_invoke(aMOELogger, it->first, it->second);
   }
}

void ResponsibleAssignments::Delete(const tTrackID& master_track_id)
{
   auto resp_iter = m_responsible_assignments.find(master_track_id);
   if (resp_iter != m_responsible_assignments.end())
   {
      m_responsible_assignments.erase(resp_iter);
   }
}

///////////////
// ActiveCue //
///////////////
ActiveCue::ActiveCue(const std::shared_ptr<const trackRecord>& master_track,
                     const std::shared_ptr<il::cueMessage>& cue,
                     const std::shared_ptr<sensorRecord>& sensor)
   : m_assignment_master_track(master_track)
   , m_cue(cue)
   , m_sensor(sensor)
{

}

////////////////
// ActiveCues //
////////////////
bool ActiveCues::IsExistingCue(const tSensorID& sensor_id, const tTrackID& master_track_id) const
{
   bool is_existing_cue = false;

   auto sensor_cues_iter = m_active_cues.find(sensor_id);
   if (sensor_cues_iter != m_active_cues.end())
   {
      auto cue_iter = sensor_cues_iter->second.find(master_track_id);
      if (cue_iter != sensor_cues_iter->second.end())
      {
         is_existing_cue = true;
      }
   }

   return is_existing_cue;
}

void ActiveCues::HandleCue(const double curr_sim_time_s,
                           const std::shared_ptr<const trackRecord>& master_track,
                           const std::shared_ptr<il::cueMessage>& cue,
                           const std::shared_ptr<sensorRecord>& sensor)
{
   if (!IsExistingCue(cue->getCuedSensorID(), master_track->getID()))
   {
      if (cue->getCueReason() == cueMessage::NEW_CUE)
      {
         // create new entry

         // perform on processing
      }
      else if (cue->getCueReason() == cueMessage::CANCEL)
      {
         // throw warning, nothing else to do
      }
   }
   // existing cue, check for CANCEL vs NEW
   else if (cue->getCueReason() == cueMessage::CANCEL)
   {
      // remove entry

      // perform off processing
   }
   // existing cue and claimed new cue message
   else if (cue->getCueReason() == cueMessage::NEW_CUE)
   {
      // throw warning, nothing else to do
   }
}

} // namespace il
