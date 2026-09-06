// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  assignmentTrackMessage
 *  @file   assignmentTrackMessage.hpp
 *
 **  Classification: UNCLASSIFIED
 */

#ifndef IADSLIB_ASSIGNMENT_TRACK_MESSAGE_HPP
#define IADSLIB_ASSIGNMENT_TRACK_MESSAGE_HPP

#include <iadsLib/idRecord.hpp>
#include <iadsLib/trackRecord.hpp>


namespace il
{
class IADSLIB_EXPORT assignmentTrackMessage : public baseMessage
{
   public:

      assignmentTrackMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~assignmentTrackMessage() = default;

      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;



      void setAssignedUnitID(const idRecord& assigned_unit);
      const idRecord& getAssignedUnitID() const;

      void setTrack(const trackRecord& track);
      const trackRecord& getTrack() const;

   protected:
      // Assigned ID //
      idRecord _AssignedUnitID;

      // Track
      trackRecord _TrackRecord;
};
}

#endif
