// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  assignmentMessage
 *  @file   assignmentMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 */

#ifndef ASSIGNMENTMESSAGE_HPP
#define ASSIGNMENTMESSAGE_HPP

#include <map>
#include <memory>

#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>
#include <iadsLib/assessmentRecord.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/assetRecord.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/assignmentContainers.hpp>


namespace il
{
class IADSLIB_EXPORT assignmentMessage : public baseMessage
{
   public:

      // New assignment, retransmit, update, cancel, reassignment
      enum AssignmentReason
      {
         NEW_ASSIGNMENT = 0,
         RETRANSMIT     = 1,
         UPDATE         = 2,
         REASSIGNMENT   = 3,
         CANCEL         = 4
      };

      assignmentMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      assignmentMessage(const assignmentMessage& fcm);
      virtual ~assignmentMessage() = default;
      assignmentMessage* clone() const override;

      assignmentMessage& operator=(const assignmentMessage& rec);

      bool operator<(const assignmentMessage& message) const;
      bool operator==(const assignmentMessage& message) const;
      bool operator!=(const assignmentMessage& message) const;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      void create(const double        simTime,
                  const eShotDoctrine shot_doctrine,
                  const idRecord&     initiating_unit_id,
                  const idRecord&     assigning_unit_id,
                  const idRecord&     reference_track_number,
                  const idRecord&     weapon_unit_id,
                  const idRecord&     weapon_id,
                  const double        max_shot_time,
                  const double        max_projection_time,
                  const double        max_projection_time_increment,
                  const bool          is_self_defense = false,
                  const bool          is_delegation = false);

      void setReferenceTrackID(const idRecord& track_num);
      const idRecord& getReferenceTrackID() const;

      void setLocalTrackID(const idRecord& track_num);
      const idRecord& getLocalTrackID() const;

      void setAssignedID(const idRecord& value);
      idRecord getAssignedID() const;

      void setCuedID(const idRecord& value);
      idRecord getCuedID() const;

      void setInitiatingID(const idRecord& value);
      idRecord getInitiatingID() const;

      void setAssigningID(const idRecord& value);
      idRecord getAssigningID() const;

      void setSelfDefenseFlag(const bool& value);
      bool getSelfDefenseFlag() const;

      void setAssignTime(const double& value);
      double getAssignTime() const;

      void setShotDoctrine(const eShotDoctrine& value);
      eShotDoctrine getShotDoctrine() const;

      void setMaxFiringTime(const double& value);
      double getMaxFiringTime() const;

      void setMaxProjectionTime(const double& value);
      double getMaxProjectionTime() const;

      void setMaxProjectionTimeIncrement(const double& value);
      double getMaxProjectionTimeIncrement() const;

      void setAssignmentReason(const AssignmentReason& value);
      AssignmentReason getAssignmentReason() const;
      std::string getAssignmentReasonString() const;

      // based off of shot-doctrine - ideal number we'd like to commit to the engagement
      int getRequestedMissilesToCommit() const;

      //////////////////////////////
      // status-related interface //
      //////////////////////////////
      assignmentACKMessage::eAssignmentStatus getAssignmentStatus() const;
      void setStatus(const std::shared_ptr<assignmentACKMessage>& status);
      inline std::shared_ptr<assignmentACKMessage>& getStatus() { return m_status; }
      bool isAssignmentComplete() const;
      bool wasSuccess() const;
      int getSalvosFired() const;
      void incrementShotsFired();

   protected:

      // Track ID //
      // ID of track to be engaged, cancelled, re-assigned...
      idRecord _ReferenceTrackID;
      idRecord _LocalTrackID;

      // Assigned ID //
      idRecord _AssignedID;

      idRecord _CuedID;

      // Bookkeeping Unit IDs //
      idRecord _InitiatingUnitID;
      idRecord _AssigningUnitID;

      // Note: How the above ids work is the following:
      // _AssigningUnitID => the unit making the assignment
      // _AssignedID => the unit being assigned


      // Flags
      bool _Delegation;
      bool _SelfDefenseAssignment;
      bool _Ready;

      double _AssignTime;

      // S1, S2, SLS, etc
      eShotDoctrine _ShotDoctrine;

      double  _ShotExpiry;  // maximum firing time in seconds
      double _MaxProjectionTime;
      double _MaxProjectionTimeIncrement;

      AssignmentReason _Reason;

      /////////////////////////////////////////
      // Internal Data - not sent in message //
      /////////////////////////////////////////
      // Statuses are kept internally and aren't sent out - it's more of a processing convenience
      std::shared_ptr<assignmentACKMessage> m_status;

      int m_shots_fired_during_assignment;
};
}

#endif
