// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  assignmentTrackMessage
 *  \file   assignmentTrackMessage.cpp
 *
 **  Classification: UNCLASSIFIED
 */

#include <iostream>
#include <sstream>
#include <iadsLib/assignmentTrackMessage.hpp>
#include <logger.hpp>

namespace il
{

assignmentTrackMessage::assignmentTrackMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : baseMessage(aGlobalLogger, ASSIGNMENT_TRACK_MESSAGE)
{

}


baseMessage* assignmentTrackMessage::clone() const
{
   return new assignmentTrackMessage(*this);
}

void assignmentTrackMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void assignmentTrackMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void assignmentTrackMessage::logSTD(std::ostream& os) const
{
   os << std::fixed
      << "Assignment Track Message" << "\n"
      << "------------------" << "\n";
   baseMessage::logSTD(os);
   os << "Assigned Unit ID: " << _AssignedUnitID.getID() << " Specific System ID: " << _AssignedUnitID.getSubID() << std::endl
      << "Track Record: " << _TrackRecord.getID() << std::endl;
}

void assignmentTrackMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << ","
      << "Assignment Track Message" << ",";
   os << "Assigned Unit ID: " << _AssignedUnitID.getID() << "," << _AssignedUnitID.getSubID() << ","
      << _TrackRecord.getID() << std::endl;
}


void assignmentTrackMessage::setAssignedUnitID(const idRecord& assigned_unit)
{
   _AssignedUnitID = assigned_unit;

}

const idRecord& assignmentTrackMessage::getAssignedUnitID() const
{
   return _AssignedUnitID;
}

void assignmentTrackMessage::setTrack(const trackRecord& track)
{
   _TrackRecord = track;
}

const trackRecord& assignmentTrackMessage::getTrack() const
{
   return _TrackRecord;
}

} // namespace il
