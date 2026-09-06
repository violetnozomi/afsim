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
*   File: WsfSimpleSensorsManagerImpl.cpp
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
*   Abstract: Sensors manager Base class.
*
*******************************************************************************/

#include <WsfSimpleSensorsManagerImpl.hpp>

#include <iadsLib/assetManagerInterface.hpp>
#include <WsfBMMOELogger.hpp>
#include <WsfAssetManager.hpp>

WsfSimpleSensorsManagerImpl::WsfSimpleSensorsManagerImpl()
   : il::SensorsManagerInterface()
{
}

WsfSimpleSensorsManagerImpl::WsfSimpleSensorsManagerImpl(const WsfSimpleSensorsManagerImpl& from)
   : il::SensorsManagerInterface(from)
{
}

WsfSimpleSensorsManagerImpl* WsfSimpleSensorsManagerImpl::Clone()
{
   return new WsfSimpleSensorsManagerImpl(*this);
}

WsfSimpleSensorsManagerImpl::~WsfSimpleSensorsManagerImpl()
{
}

bool WsfSimpleSensorsManagerImpl::SendSensorCueMessage(il::moeLoggingInterface& aMOELogger, const double sim_time, const std::shared_ptr<const il::trackRecord>& master_track, const std::shared_ptr<const il::assignmentMessage>& assignment, const std::shared_ptr<il::sensorRecord>& sensor, const il::cueMessage::CueReason& reason)
{
   bool success = false;
   // don't actually send out the cue, just bookkeep it internally and treat it as if we are performing the local Asset Manager sensor manager cue handling code
   if (reason == il::cueMessage::NEW_CUE)
   {
      success = ProcessCurrentCueAssignment(aMOELogger, sim_time, master_track, assignment, sensor);
   }
   else if (reason == il::cueMessage::CANCEL)
   {
      success = ProcessDropCue(aMOELogger, sim_time, master_track, assignment, sensor);
   }
   else
   {
      HCL_FATAL_LOGGER(GetAssetManager()->getGlobalLogger()) << "WsfSimpleSensorsManagerImpl::SendSensorCueMessage(): Invalid cue type, not cancel or new";
   }

   return success;
}

// Returns true if sensor is turned on AND sensor is either sucessfully cued or is not cueable.
bool WsfSimpleSensorsManagerImpl::ProcessCurrentCueAssignment(il::moeLoggingInterface& aMOELogger, const double sim_time, const std::shared_ptr<const il::trackRecord>& master_track, const std::shared_ptr<const il::assignmentMessage>& assignment, const std::shared_ptr<il::sensorRecord>& sensor)
{
   auto am = GetAssetManager();

   if (!sensor->IsTurnedOn())
   {
      sensor->TurnOn(sim_time);
      return false;
   }

   // If sensor is turned on, attempt to cue a cueable sensor.
   else
   {
      if (sensor->IsCueable())
      {
         if (!sensor->Cue(sim_time, am->getThisAsset()->getID(), master_track->getID()))
         {
            HCL_ERROR_LOGGER(am->getGlobalLogger()) << "WsfSimpleSensorManagerImpl::ProcessCurrentCueAssignment(): Failed to cue " << sensor->GetSensorID() << ".";
            return false;
         }
      }

      // Write HEFIR message and return true if either the sensor is cueable and was successfully cued, or the sensor is not cueable.
      aMOELogger.SensorCue(sim_time, am->getThisAsset().get(), master_track.get(), assignment.get(), sensor.get());
      return true;
   }
}

bool WsfSimpleSensorsManagerImpl::ProcessDropCue(il::moeLoggingInterface& aMOELogger, const double sim_time, const std::shared_ptr<const il::trackRecord>& master_track, const std::shared_ptr<const il::assignmentMessage>& assignment, const std::shared_ptr<il::sensorRecord>& sensor)
{
   bool was_success = false;

   if (sensor->IsCueable())
   {
      was_success = sensor->DropCue(sim_time, master_track->getID());
   }
   else
   {
      was_success = true;
   }

   if (was_success)
   {
      // write HEFIR message
      auto am = GetAssetManager();
      //if (dynamic_cast<WsfAssetManager*>(am.get()))
      //dynamic_cast<WsfAssetManager*>(am.get())->GetSimulation();
      aMOELogger.SensorDropCue(sim_time, am->getThisAsset().get(), master_track.get(), assignment.get(), sensor.get());
   }

   // perform on/off processing
   if ((sensor->IsTTR() && GetTurnOffTTRNoTracks()) ||
       (sensor->IsTAR() && GetTurnOffTARNoTracks()))
   {
      TurnOffSensorIfNoTracks(aMOELogger, sim_time, sensor);
   }

   return true;
}

void WsfSimpleSensorsManagerImpl::TurnOffSensorIfNoTracks(il::moeLoggingInterface& aMOELogger, const double sim_time, const std::shared_ptr<il::sensorRecord>& sensor)
{
   // a few conditions must be met:
   // 1) the sensor can not have any outstanding responsible assignments
   // 2) the sensor must not have any outstanding "truth-based" active requests
   // this covers the case of TARs that only have responsible assignments and TTRs that have responsible assignments and real sensor requests

   // note: we use truth here because we may interfere with other processors we don't own controlling the sensors as well, don't yank the rug from
   // under them
   if (sensor->GetTruthActiveCues(GetAssetManager()->getGlobalLogger()))
   {
      return;
   }

   // check responsible assignments
   bool has_assignments = false;
   auto has_resp_assignment = [&has_assignments, &sensor](il::moeLoggingInterface& aMOELogger, const il::tTrackID &master_track_id, il::ResponsibleRadarAssignment &responsible_assignment) -> void
   {
      if ((sensor->IsTAR() && responsible_assignment.HasTARResponsibility() && responsible_assignment.GetResponsibleTAR() == sensor) ||
          (sensor->IsTTR() && responsible_assignment.HasTTRResponsibility() && responsible_assignment.GetResponsibleTTR() == sensor))
      {
         has_assignments = true;
      }
   };

   GetResponsibleAssignments().ForeachResponsibleAssignment(aMOELogger, has_resp_assignment);

   if (!has_assignments && sensor->IsTurnedOn())
   {
      sensor->TurnOff(sim_time);
   }
}

// next TTD: truth max cue info on sensors for more complex modes when tracking, maybe we need to split up between mode specific and non-mode specific request counts? TAR and TTR request counts?
// next TTD: truth vs perception active cues (and max)
