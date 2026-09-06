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
*   File: assetManagerInterface.hpp
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
*   Abstract: Asset Management for interacting with the battle manager
*
*******************************************************************************/

#ifndef ASSET_MANAGER_IFACE_HPP
#define ASSET_MANAGER_IFACE_HPP

namespace il
{
   class BattleManagerInterface;
}

#include <vector>
#include <utility>
#include <memory>

#include <logger.hpp>
#include <iadsLib/idRecord.hpp>

#include <iadsLib/baseMessage.hpp>
#include <iadsLib/airTargetMessage.hpp>
#include <iadsLib/plotMessage.hpp>
#include <iadsLib/bearingReportMessage.hpp>
#include <iadsLib/combatStatusMessage.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/weaponsControlMessage.hpp>
#include <iadsLib/cueMessage.hpp>
#include <iadsLib/moeLoggingInterface.hpp>

#include <iadsLib/trackRecord.hpp>
#include <iadsLib/exclusionArray.hpp>
#include <iadsLib/referencesProcessor.hpp>


namespace il
{
   class IADSLIB_EXPORT AssetManagerInterface : public ReferencesBattleManager, public ReferencesSensorsManager, public ReferencesWeaponsManager
   {
      public:
         enum eMessageAction
         {
            E_UPDATED_TRACK,
            E_DROPPED_TRACK,
            E_STATUS_RCVD,
            E_REJECTED_ASSIG_RESPONSE,
            E_CANCEL_ASSIG_RECVD,
            E_ASSIG_ACK_RCVD,
            E_ASSIGN_OVERRIDE_NOTIF,
            E_ASSIGN_MSG_OUT,
            E_SELF_DEFENSE_ASSIGNMENT_CREATED,
            E_SENSOR_CUE,
            E_ASSIGNMENT_TRACK_UPDATE,
         };
         typedef std::vector<std::pair<eMessageAction, std::shared_ptr<baseMessage>>> tOutgoingMsgArray;

         AssetManagerInterface(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
         virtual ~AssetManagerInterface() = default;
         virtual AssetManagerInterface* Clone();

         virtual void processMessage(std::shared_ptr<airTargetMessage> atm);
         virtual void processMessage(std::shared_ptr<plotMessage> pm);
         virtual void processMessage(std::shared_ptr<bearingReportMessage> brm);
         virtual void processMessage(std::shared_ptr<combatStatusMessage> csm);
         virtual void processMessage(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> am);
         virtual void processMessage(std::shared_ptr<assignmentACKMessage> ack);
         virtual void processMessage(std::shared_ptr<weaponsControlMessage> wcm);
         virtual void processMessage(std::shared_ptr<cueMessage> cue);

         inline virtual const tOutgoingMsgArray& getOutgoingMessages() const { return m_OutgoingMessages; }
         inline virtual void clearOutgoingMessages() { m_OutgoingMessages.clear(); }

         inline virtual trackMap& GetMasterTracksArray() { return m_MasterTracks; }

         virtual void UpdateDefendedAssetsArray();

         virtual void PrepareTracksForEvaluation();

         virtual void CreateAssignments(il::moeLoggingInterface& aMOELogger);
         virtual void LogAssignmentCreation(il::moeLoggingInterface& aMOELogger, const evaluationAssignment& evaled_assignment);
         virtual void CreateSelfDefenseAssignment(const evaluationAssignment& assignment);
         virtual void MonitorAssignments(il::moeLoggingInterface& aMOELogger);
         virtual void CompleteAssignments(il::moeLoggingInterface& aMOELogger);
         virtual void AssessOngoingAssignments(il::moeLoggingInterface& aMOELogger);
         virtual bool SendSensorCueMessage(const double sim_time, const tTrackID& master_track_id, std::shared_ptr<sensorRecord> sensor, const cueMessage::CueReason& reason);

         virtual void CalculateTrackStrengths(const double SimTime, const double max_grouping_distance);

         ////////////////////////
         // Migrated Interface //
         ////////////////////////
         virtual void setOwnID(const idRecord& AssetID);
         virtual idRecord getOwnID() const;
         virtual bool updateOwnStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const double ecef_pos_m[3], const double ecef_vel_ms[3], const il::SYSTEM_STATUS status = STATUS_GREEN);
         inline virtual void setStartTime(const double StartTime) { m_StartTime = StartTime; }
         inline virtual void updateTime(const double currentTime) { m_CurrentTime = currentTime; }
         virtual void logMyStatus() const;
         virtual void logSubordinateStatus(bool direct_only = true) const;
         inline virtual void setLogMyStatus(const bool logStatus) { m_log_my_status = logStatus; }
         inline virtual bool getLogMyStatus() const { return m_log_my_status; }

         virtual void setAssetYellowAgeout(const double ageout_time_s);
         virtual void setAssetRedAgeout(const double ageout_time_s);
         virtual void setReportPositionEveryMeters(const double distance_m);
         virtual void setReportPositionEverySeconds(const double time_s);
         virtual void setReportStatusEverySeconds(const double time_s);
         virtual void setAggregateUnitStatus(const bool is_set);
         virtual void setStationaryOpnsOnly(const bool is_set);
         virtual void setWeaponRequired(const bool is_set);
         virtual void setRequireAllWeapons(const bool is_set);
         virtual void setEWRequired(const bool is_set);
         virtual void setTARRequired(const bool is_set);
         virtual void setTTRRequired(const bool is_set);

         virtual double getAssetYellowAgeout() const;
         virtual double getAssetRedAgeout() const;
         virtual double getReportPositionEveryMeters() const;
         virtual double getReportPositionEverySeconds() const;
         virtual double getReportStatusEverySeconds() const;
         virtual bool   getAggregateUnitStatus() const;
         virtual bool   getStationaryOpnsOnly() const;
         virtual bool   getWeaponRequired() const;
         virtual bool   getRequireAllWeapons() const;
         virtual bool   getEWRequired() const;
         virtual bool   getTARRequired() const;
         virtual bool   getTTRRequired() const;

         inline virtual void setMaxAssignments(const int max_assignments) { m_max_assignments = max_assignments; }
         inline virtual int  getMaxAssignments() const { return m_max_assignments; }
         inline virtual int getNumAssignments() const { return m_ThisAsset ? m_ThisAsset->getNumAssignments() : 0; }
         inline virtual void setAssignmentDelay(const double delay_seconds) { m_assignment_delay_s = delay_seconds; }
         inline virtual double getAssignmentDelay() const { return m_assignment_delay_s; }
         inline virtual void setDecisionUpdateDelay(const double delay_seconds) { m_decision_update_delay_s = delay_seconds; }
         inline virtual double getDecisionUpdateDelay() const { return m_decision_update_delay_s; }

         virtual idRecord getMessageSenderInfo(std::shared_ptr<baseMessage> Message, bool& fromCommander, bool& fromSubordinate, bool& fromPeer);
         virtual assetRecord& getMyAsset() const;

         virtual bool isInitialized() const;

         std::weak_ptr<logger::GlobalLogger> getGlobalLogger() const { return mGlobalLogger; }

      protected:
         AssetManagerInterface(const AssetManagerInterface& from);
         virtual void getThisAssetPtr();

         std::weak_ptr<logger::GlobalLogger> mGlobalLogger;

         ////////////////////////////
         // End Migrated Interface //
         ////////////////////////////

      public:
         ///////////////////////////////////////////////////
         // Interfaces needed various other C2 processors //
         ///////////////////////////////////////////////////
         inline virtual double getCurrentTime() const { return m_CurrentTime - m_StartTime; }
         inline virtual void setLastProcessTime(double time_s) { m_LastProcessTime = time_s; }
         inline virtual double getLastProcessTime() const { return m_LastProcessTime; }
         inline virtual const std::vector<std::shared_ptr<zoneRecordInterface>>& getDefendedAssetsArray() const { return m_DefendedAssets; }
         inline virtual std::shared_ptr<assetRecord> getThisAsset() const { return m_ThisAsset; }
         inline virtual assetMap& getAssets() { return m_Assets; }
         inline virtual const assetMap& getAssets() const { return m_Assets; }
         inline virtual evalAssignmentMultimap& getAssignmentArray() { return m_AssignmentArray; }
         virtual void wilcoAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment);
         virtual void havecoAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, bool was_success = true);
         virtual void shotsFiredAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment);
         virtual void sensorTrackingAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const il::sensorRecord* sensor);
         inline virtual void cancelAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const std::string& cancel_reason) { internallyCancelAssignment(aMOELogger, active_assignment, cancel_reason); }
         inline virtual void cantcoAssignment(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> assignment_message, const std::string& cantco_reason, const bool is_systemic = false, const bool is_global = false)
         {
            internallyCANTCOAssignment(aMOELogger, assignment_message, cantco_reason, E_ASSIG_ACK_RCVD, is_systemic, is_global);
         }
         inline virtual exclusionArray& getExclusionArray() { return m_ExclusionArray; }

         virtual bool isTrackAssigned(const idRecord& track_id) const;
         inline virtual std::shared_ptr<trackRecord> getMasterTrack(const idRecord& trackid) { return FindTrack(trackid, m_MasterTracks); }
         /////////////////////////////////////////////////
         // End Interfaces needed by the battle manager //
         /////////////////////////////////////////////////


         /////////////////////////////////////////////
         // Interfaces needed by the sensor manager //
         /////////////////////////////////////////////

         // Interfaces needed specifically for external input processing
         virtual std::shared_ptr<assignmentMessage> getAssignment(const idRecord& track_id, const idRecord& assigned_id);
         virtual std::vector<evaluationAssignment> getAssignmentsOnReferenceTrack(il::moeLoggingInterface& aMOELogger, const idRecord& track_id, const idRecord& assigned_id);  // same as above but makes use of reference track instead of master track (local) id
         virtual std::vector<evaluationAssignment> getOurAssignments(il::moeLoggingInterface& aMOELogger);
         // returns the whole assignment, not just the core assignment piece - be careful about lifetime
         virtual bool getEvaluationAssignment(const idRecord& track_id, const idRecord& assigned_id, evaluationAssignment& eval_assign);
         virtual bool getEvaluationAssignment(const assignmentACKMessage& ack, evaluationAssignment& eval_assign);
         virtual std::vector<evaluationAssignment> getAssignmentsOnTrack(const idRecord& track_id);

         // HELIOS Interfaces
         bool SetPosition(const double sim_time, const double ecef_pos_m[3], const double ecef_vel_ms[3], bool force_send = false);
         bool SetStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const il::SYSTEM_STATUS external_own_status, bool force_send = false);
         void SetOwnStatus(il::moeLoggingInterface& aMOELogger, const double sim_time, const il::SYSTEM_STATUS external_own_status);
         void CheckSubordinateTimeout(const double sim_time);

      protected:

         virtual void queueOutgoingMessage(const eMessageAction action, std::shared_ptr<baseMessage> message);
         virtual void queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::combatStatusMessage> message);
         virtual void queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::assignmentMessage> message);
         virtual void queueOutgoingMessage(const eMessageAction action, std::shared_ptr<il::assignmentACKMessage> message);
         static std::string MsgActionToStr(const eMessageAction action);

         virtual void handleCreateAssignmentMsgIn(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> am);
         virtual void handleCancelAssignmentMsgIn(std::shared_ptr<assignmentMessage> am);

         // Track Management
         virtual bool updateMasterTrack(const idRecord& TrackID, std::shared_ptr<airTargetMessage> atm);
         virtual void notifyMasterTrackUpdate(const idRecord& TrackID);
         virtual bool deleteMasterTrack(const idRecord& TrackID, std::shared_ptr<airTargetMessage> atm);

         // Assignment bookkeeping
         virtual bool isExistingAssignment(std::shared_ptr<assignmentMessage> assignment_message);
         virtual bool unitHasAssignments(const idRecord& asset_id, const bool compare_sub_id = false) const;
         virtual bool weaponHasAssignments(il::moeLoggingInterface& aMOELogger, const tWeaponID& weaponID);
         virtual bool addDelegationAssignment(std::shared_ptr<assignmentMessage> assignment_message);
         virtual void internallyCancelAssignment(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const std::string& cancel_reason);
         virtual void internallyUpdateRegularAssignmentStatus(il::moeLoggingInterface& aMOELogger, evaluationAssignment& active_assignment, const il::assignmentACKMessage::eAssignmentStatus status_enum);
         virtual void internallyCANTCOAssignment(il::moeLoggingInterface& aMOELogger, std::shared_ptr<assignmentMessage> assignment_message, const std::string& cantco_reason, const eMessageAction why_action = E_ASSIG_ACK_RCVD, const bool is_systemic = false, const bool is_global = false);
         virtual void killAllAssignments(il::moeLoggingInterface& aMOELogger, const std::string& reason);
         unsigned int GetNumIncompleteAssignments(il::moeLoggingInterface& aMOELogger);
         // Assignment override
         virtual std::shared_ptr<assignmentACKMessage> generateAssignmentOverrideNotification(const evaluationAssignment& assignment);
         virtual std::shared_ptr<assignmentACKMessage> generateDoctrineOverrideNotification(const evaluationAssignment& assignment);

         // Determine number of missiles to give back to an assets perception based off what has happened in the assignment
         virtual int GetIncrementMissileCount(std::shared_ptr<assignmentMessage> assignment) const;

         virtual void handleIncomingCue(std::shared_ptr<cueMessage> cue);

         virtual void updatePerceptions(il::moeLoggingInterface& aMOELogger);

         // Shared Objects //
         // Map of assets indexed by assetID.
         assetMap m_Assets;

         //////////////////////
         // Track Structures //
         //////////////////////
         /// Map of all known tracks after being correlated - resident between processing loops
         trackMap m_MasterTracks;

         /// Defended assets array
         std::vector<std::shared_ptr<zoneRecordInterface>> m_DefendedAssets;

         /// List of messages waiting to be sent out.
         tOutgoingMsgArray m_OutgoingMessages;

         // All assignments being processed through this C2 level - resident between processing loops
         evalAssignmentMultimap m_AssignmentArray;

         // Exclusion arrays
         exclusionArray m_ExclusionArray;

         // Local Asset-Management centric objects
         idRecord m_ThisAssetID;
         std::shared_ptr<assetRecord> m_ThisAsset;

         // simulation-related times
         double m_StartTime;
         double m_CurrentTime;

         double m_LastProcessTime; // intended to be set when a full process cycle completes (to help implement decision update delay)

         bool m_status_initialized;

         // asset management timeouts
         double m_asset_yellow_ageout_time;
         double m_asset_red_ageout_time;
         double m_report_position_every_meters;
         double m_report_position_every_seconds;
         double m_report_status_every_seconds;
         // asset status aggregation
         bool m_aggregate_unit_status;
         bool m_stationary_opns_only;
         bool m_weapon_required;
         bool m_ew_required;
         bool m_tar_required;
         bool m_ttr_required;

         // the following flags just state for multi-weapon platforms that require a weapon to require all weapons to have munitions
         bool m_require_all_weapons;

         // assignment-related settings
         int m_max_assignments;
         double m_assignment_delay_s;

         // decision delays
         double m_decision_update_delay_s;

         // incoming assignments from another C2 that we intercept for delegation if we don't have commit authority (temporary structure)
         assignmentMap m_PendingDelegateAssignments;

         ActiveCues m_active_sensor_cues;
         bool m_log_my_status;

      private:
         AssetManagerInterface& operator=(const AssetManagerInterface& rhs) = delete;
   };
}

#endif
