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
*   File: sensorsManagerInterface.hpp
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

#ifndef IADSLIB_SENSORS_MANAGER_INTERFACE_HPP
#define IADSLIB_SENSORS_MANAGER_INTERFACE_HPP

namespace il
{
class AssetManagerInterface;
}

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <memory>

#include <logger.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/referencesProcessor.hpp>
#include <iadsLib/enum.hpp>

#include <iadsLib/sensorRecordInterface.hpp>
#include <iadsLib/trackRecord.hpp>
#include <iadsLib/assignmentContainers.hpp>

namespace il
{
	class IADSLIB_EXPORT SensorsManagerInterface : public ReferencesAssetManager
	{
	public:
		SensorsManagerInterface();
		virtual SensorsManagerInterface *Clone ();
		virtual ~SensorsManagerInterface();

		virtual void Run (const double sim_time_s) {};

      // Scripting //
      virtual void SetMaxTARAcqTime (const double max_time_s);
      virtual double GetMaxTARAcqTime () const;

      virtual void SetTurnOffTARNoTracks (const bool turn_off);
      virtual bool GetTurnOffTARNoTracks () const;

      virtual void SetMaxTTRAcqTime (const double max_time_s);
      virtual double GetMaxTTRAcqTime () const;

      virtual void SetTurnOffTTRNoTracks (const bool turn_off);
      virtual bool GetTurnOffTTRNoTracks () const;

      // Runtime
      virtual void ProcessResponsibleAssignments (il::moeLoggingInterface& aMOELogger, const double sim_time);
      virtual void ProcessAssignment (const double sim_time, evaluationAssignment &eval_assignment);

      virtual void CreateSensorCues (il::moeLoggingInterface& aMOELogger, const double sim_time);
      virtual void DropCompletedAssignments (il::moeLoggingInterface& aMOELogger, double sim_time);

      virtual void MasterTrackUpdated (const double sim_time, std::shared_ptr<il::trackRecord> master_track);

      virtual ResponsibleAssignments& GetResponsibleAssignments () { return m_responsible_assignments; }
	protected:
		SensorsManagerInterface (const SensorsManagerInterface &from);

      // responsible assignment/cue message creation logic
      std::pair<bool, il::SensorEnum> DetermineNextCueType (const std::shared_ptr<il::assignmentMessage> &assignment, const std::shared_ptr<il::trackRecord> &master_track);
      bool AssignedRadarToAssignment (const double sim_time, const il::SensorEnum sensor_type, const std::shared_ptr<il::trackRecord> &master_track, const std::shared_ptr<assignmentMessage> &assignment, const bool skipped_previous_sensor = false);
      bool CueSensorForAssignment (const double sim_time, const std::shared_ptr<il::trackRecord> &master_track, const std::shared_ptr<assignmentMessage> &assignment, std::shared_ptr<sensorRecord> &sensor_to_assign, const bool skipped_tar = false);

      // responsible assignment maintenance
      bool RadarTargetFound (const idRecord &master_track_id, ResponsibleRadarAssignment &responsible_assignment) const;
      void ProcessAssignmentTrackingTime (il::moeLoggingInterface& aMOELogger, const double sim_time, ResponsibleRadarAssignment &responsible_assignment, evaluationAssignment &eval_assignment);
      bool TimeToCANTCOAssignment (const double sim_time, const ResponsibleRadarAssignment &responsible_assignment) const;
      std::string DetermineCANTCOReason (const ResponsibleRadarAssignment &responsible_assignment) const;

      // overridable for direct cueing potential
      virtual bool SendSensorCueMessage (il::moeLoggingInterface& aMOELogger, const double sim_time, const std::shared_ptr<const trackRecord> &master_track, const std::shared_ptr<const assignmentMessage> &assignment, const std::shared_ptr<sensorRecord> &sensor, const cueMessage::CueReason &reason);
	private:
		double m_max_tar_acq_time;
		bool m_turn_of_tar_if_no_tracks;

		double m_max_ttr_acq_time;
		bool m_turn_of_ttr_if_no_tracks;

      enum eRadarAssignmentAlgorithm { E_FIRST_FOUND };
      eRadarAssignmentAlgorithm m_assignment_algorithm;

      ResponsibleAssignments m_responsible_assignments;

		SensorsManagerInterface & operator=(const SensorsManagerInterface &rhs);
	};
}

#endif
