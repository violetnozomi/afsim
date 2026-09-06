// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @class  assignmentACKMessage
*  @file   assignmentACKMessage.hpp
*  @author Kenneth R. Sewell III
*          AFRL/RYWF
*          2241 Avionics Circle
*          WPAFB, Ohio 45433-7303
*          937-656-4899 x3004
*
**  Classification: UNCLASSIFIED
*/

#ifndef ASSIGNMENTACKMESSAGE_HPP
#define ASSIGNMENTACKMESSAGE_HPP

#include <string>
#include <iadsLib/enum.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>

namespace il
{
class IADSLIB_EXPORT assignmentACKMessage : public baseMessage
{
   public:
      enum eAssignmentStatus
      {
         UNACKNOWLEDGED        = 0,
         // Stages of engagement
         WILCO                 =  2,
         COMITTED              =  4,
         CUE_TAR               =  5,
         CUE_TTR               =  6,
         TAR_SEARCHING         =  7,
         TTR_SEARCHING         =  8,
         TAR_TRACKING          =  9,
         TTR_TRACKING          = 10,
         FIRING                = 11,
         MISS                  = 12,
         CHANGED_ASSIGNED_UNIT = 13,
         UPDATED_SHOT_DOCTRINE = 14,
         // Note: everything below this comment is considered a completed assignment, if you
         // run out of enumspace above this, you need to recheck all completed assignment check code
         KILL                  = 20,
         CANCELLED             = 21,
         HAVCO_SUCCESS         = 23,
         HAVCO_FAILURE         = 24,
         CANTCO                = 25
      };

      enum eSystemicCantcoScope
      {
         LOCAL  = 0,  // remove from further consideration for this track
         GLOBAL = 1   // remove from further consideration for all tracks
      };

      assignmentACKMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~assignmentACKMessage() = default;
      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      std::string getComplyStatusString() const;

      void setAckTime(const double DataTime);
      double getAckTime() const;

      bool statusTimeExists(const eAssignmentStatus status) const;
      double getStatusTime(const eAssignmentStatus status) const;

      void setAssignedID(const idRecord& value);
      idRecord getAssignedID() const;

      void setInitiatingID(const idRecord& value);
      idRecord getInitiatingID() const;

      void setAssigningID(const idRecord& value);
      idRecord getAssigningID() const;

      void setAssignmentReferenceTrackID(const idRecord& value);
      idRecord getAssignmentReferenceTrackID() const;

      void setStatus(const eAssignmentStatus status);
      eAssignmentStatus getStatus() const;

      void setSalvosFired(const int salvos);
      int getSalvosFired() const;

      void setCantcoReason(const std::string& reason);
      std::string getCantoReason() const;

      void setSystemicCantcoScope(const eSystemicCantcoScope scope);  // only way to set systemic flag as well
      bool getSystemicCantcoScope(eSystemicCantcoScope& scope) const;
      bool isSystemicCantco() const;

      void setOverridingID(const idRecord& value);
      idRecord getOverridingID() const;

      void setNewlyAssignedID(const idRecord& value);
      idRecord getNewlyAssignedID() const;

      void setUpdatedShotDoctrine(const eShotDoctrine& doctrine);
      eShotDoctrine getUpdatedShotDoctrine() const;

   protected:

      void setStatusTime(const eAssignmentStatus status, const double time);

      // Timestamp of message.
      double _AckTime;

      // Timestamps of statuses
      std::map<eAssignmentStatus, double> _StatusTimesMap;

      // ID of unit assigned to track
      idRecord _AssignedID;

      // Bookkeeping Unit IDs //
      idRecord _InitiatingUnitID;
      idRecord _AssigningUnitID;

      // ID of track assigned
      idRecord _TrackID;

      // Cant comply, will comply, ...
      eAssignmentStatus _Status;

      // Missiles fired at track
      int _SalvosFired;

      // The following is only valid if status == CANTCO
      std::string _CantcoReason;
      bool _SystemicCantco;
      eSystemicCantcoScope _SystemicCantcoScope;

      // Only set if status == CHANGED_ASSIGNED_UNIT
      idRecord _OverridingUnitID;
      idRecord _NewlyAssignedID;

      // Only set if status == UPDATED_SHOT_DOCTRINE
      eShotDoctrine _Updated_Shot_Doctrine;
};
}

#endif
