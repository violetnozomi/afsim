// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @class  cueACKMessage
 *  @file   cueACKMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 */

#ifndef CUEACKMESSAGE_HPP
#define CUEACKMESSAGE_HPP

#include <string>
#include <iadsLib/enum.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/idRecord.hpp>

namespace il
{
class cueACKMessage : public baseMessage
{
   public:
      enum eCueStatus
      {
         UNACKNOWLEDGED    = 0,

         // Stages of engagement
         WILCO                 =  2,
         COMITTED              =  4,
         CUE_TAR               =  5,
         CUE_TTR               =  6,
         TAR_SEARCHING         =  7,
         TTR_SEARCHING         =  8,
         TAR_TRACKING          =  9,
         TTR_TRACKING          = 10,
         CHANGED_ASSIGNED_UNIT = 13,
         // Note: everything below this comment is considered a completed cue, if you
         // run out of enumspace above this, you need to recheck all completed cue check code
         KILL              = 20,
         CANCELLED         = 21,
         HAVCO_SUCCESS     = 23,
         HAVCO_FAILURE     = 24,
         CANTCO            = 25
      };

      enum eSystemicCantcoScope
      {
         LOCAL  = 0,  // remove from further consideration for this track
         GLOBAL = 1   // remove from further consideration for all tracks
      };

      cueACKMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~cueACKMessage() = default;
      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      std::string getComplyStatusString() const;

      void setAckTime(const double DataTime);
      double getAckTime() const;

      void setAssignedID(const idRecord& value);
      idRecord getAssignedID() const;

      void setInitiatingID(const idRecord& value);
      idRecord getInitiatingID() const;

      void setAssigningID(const idRecord& value);
      idRecord getAssigningID() const;

      void setTrackID(const idRecord& value);
      idRecord getTrackID() const;

      void setStatus(const eCueStatus status);
      eCueStatus getStatus() const;

      void setCantcoReason(const std::string& reason);
      std::string getCantoReason() const;

      void setSystemicCantcoScope(const eSystemicCantcoScope scope);  // only way to set systemic flag as well
      bool getSystemicCantcoScope(eSystemicCantcoScope& scope) const;
      bool isSystemicCantco() const;

      void setOverridingID(const idRecord& value);
      idRecord getOverridingID() const;

      void setNewlyAssignedID(const idRecord& value);
      idRecord getNewlyAssignedID() const;

   protected:
      // Timestamp of message.
      double _AckTime;

      // ID of unit assigned to track
      idRecord _AssignedID;

      // Bookkeeping Unit IDs //
      idRecord _InitiatingUnitID;
      idRecord _AssigningUnitID;

      // ID of track assigned
      idRecord _TrackID;

      // Cant comply, will comply, ...
      eCueStatus _Status;

      // The following is only valid if status == CANTCO
      std::string _CantcoReason;
      bool _SystemicCantco;
      eSystemicCantcoScope _SystemicCantcoScope;

      // Only set if status == CHANGED_ASSIGNED_UNIT
      idRecord _OverridingUnitID;
      idRecord _NewlyAssignedID;
};
}

#endif
