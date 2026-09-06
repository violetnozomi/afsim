// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  assignmentACKMessage
 *  \file   assignmentACKMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/colorText.hpp>
#include <sstream>

namespace il
{

assignmentACKMessage::assignmentACKMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger):
   baseMessage(aGlobalLogger, ASSIGNMENT_ACK_MESSAGE),
   _AckTime(0.0),
   _Status(UNACKNOWLEDGED),
   _SalvosFired(0),
   _SystemicCantco(false),
   _SystemicCantcoScope(LOCAL),    // default to something but not relevant unless systemic set is called
   _Updated_Shot_Doctrine(SHOOT_1)
{

}

baseMessage* assignmentACKMessage::clone() const
{
   return new assignmentACKMessage(*this);
}

void assignmentACKMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void assignmentACKMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void assignmentACKMessage::logSTD(std::ostream& os) const
{
   os << "Assignment ACK Message" << "\n"
      << "----------------------" << "\n";
   baseMessage::logSTD(os);
   os << "ACK Time: " << _AckTime << "\n"
      << "Assigned System ID: " << _AssignedID << "\n"
      << "Track ID: " <<  _TrackID << "\n"
      << "Comply Status: " << getComplyStatusString() << "\n"
      << "Salvos fired: " << _SalvosFired << "\n";
}

void assignmentACKMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Assignment ACK Message" << ","
      << _AckTime << ","
      << _AssignedID << ","
      <<  _TrackID   << ","
      << getComplyStatusString() << ","
      << _SalvosFired << std::endl;
}

std::string assignmentACKMessage::getComplyStatusString() const
{
   std::ostringstream status;
   switch (_Status)
   {
      case UNACKNOWLEDGED:
         status  << RED_TEXT_BLACK_BG << "UNACKNOWLEDGED";
         break;
      case WILCO:
         status  << GREEN_TEXT_BLACK_BG << "WILCO";
         break;
      case COMITTED:
         status  << GREEN_TEXT_BLACK_BG << "COMITTED";
         break;
      case CUE_TAR:
         status  << GREEN_TEXT_BLACK_BG << "CUE_TAR";
         break;
      case CUE_TTR:
         status  << GREEN_TEXT_BLACK_BG << "CUE_TTR";
         break;
      case TAR_SEARCHING:
         status  << GREEN_TEXT_BLACK_BG << "TAR_SEARCHING";
         break;
      case TAR_TRACKING:
         status  << GREEN_TEXT_BLACK_BG << "TAR_TRACKING";
         break;
      case TTR_TRACKING:
         status  << GREEN_TEXT_BLACK_BG << "TTR_TRACKING";
         break;
      case FIRING:
         status  << GREEN_TEXT_BLACK_BG << "FIRING";
         break;
      case MISS:
         status  << RED_TEXT_BLACK_BG << "MISS";
         break;
      case CHANGED_ASSIGNED_UNIT:
         status  << GREEN_TEXT_BLACK_BG << "CHANGED_ASSIGNED_UNIT";
         break;
      case UPDATED_SHOT_DOCTRINE:
         status  << GREEN_TEXT_BLACK_BG << "UPDATED_SHOT_DOCTRINE";
         break;
      case KILL:
         status  << GREEN_TEXT_BLACK_BG << "KILL";
         break;
      case CANCELLED:
         status  << RED_TEXT_BLACK_BG << "CANCELLED";
         break;
      case HAVCO_SUCCESS:
         status  << GREEN_TEXT_BLACK_BG << "HAVCO_SUCCESS";
         break;
      case HAVCO_FAILURE:
         status  << RED_TEXT_BLACK_BG << "HAVCO_FAILURE";
         break;
      case CANTCO:
         status  << GREEN_TEXT_BLACK_BG << "CANTCO (" << _CantcoReason << ")";
         break;
      default:
         std::stringstream strm_error;
         strm_error << "assignmentACKMessage::getComplyStatusString(): Unrecognized assignment status (" << _Status << ")\n";
         HCL_FATAL_LOGGER(mGlobalLogger) << strm_error.str();
         throw (strm_error.str());
   }

   return status.str();
}

void assignmentACKMessage::setAckTime(const double DataTime)
{
   _AckTime = DataTime;
}

double assignmentACKMessage::getAckTime() const
{
   return _AckTime;
}

bool assignmentACKMessage::statusTimeExists(const eAssignmentStatus status) const
{
   return _StatusTimesMap.find(status) != _StatusTimesMap.end();
}

double assignmentACKMessage::getStatusTime(const eAssignmentStatus status) const
{
   auto time = _StatusTimesMap.find(status);
   return time != _StatusTimesMap.end() ? time->second : 0.0;
}

void assignmentACKMessage::setStatusTime(const eAssignmentStatus status, const double time)
{
   _StatusTimesMap[status] = time;
}

void assignmentACKMessage::setAssignedID(const idRecord& value)
{
   _AssignedID = value;
}

idRecord assignmentACKMessage::getAssignedID() const
{
   return _AssignedID;
}

void assignmentACKMessage::setInitiatingID(const idRecord& value)
{
   _InitiatingUnitID = value;
}

idRecord assignmentACKMessage::getInitiatingID() const
{
   return _InitiatingUnitID;
}

void assignmentACKMessage::setAssigningID(const idRecord& value)
{
   _AssigningUnitID = value;
}

idRecord assignmentACKMessage::getAssigningID() const
{
   return _AssigningUnitID;
}

void assignmentACKMessage::setAssignmentReferenceTrackID(const idRecord& value)
{
   _TrackID = value;
}

idRecord assignmentACKMessage::getAssignmentReferenceTrackID() const
{
   return _TrackID;
}


void assignmentACKMessage::setStatus(const eAssignmentStatus status)
{
   setStatusTime(status, getAckTime());

   _Status = status;
}

assignmentACKMessage::eAssignmentStatus assignmentACKMessage::getStatus() const
{
   return _Status;
}

void assignmentACKMessage::setSalvosFired(const int salvos)
{
   _SalvosFired = salvos;
}

int assignmentACKMessage::getSalvosFired() const
{
   return _SalvosFired;
}


void assignmentACKMessage::setCantcoReason(const std::string& reason)
{
   _CantcoReason = reason;
}

std::string assignmentACKMessage::getCantoReason() const
{
   return _CantcoReason;
}

void assignmentACKMessage::setSystemicCantcoScope(const eSystemicCantcoScope scope)
{
   _SystemicCantco = true;
   _SystemicCantcoScope = scope;
}

bool assignmentACKMessage::getSystemicCantcoScope(eSystemicCantcoScope& scope) const
{
   if (_SystemicCantco)
   { scope = _SystemicCantcoScope; }

   return _SystemicCantco;
}

bool assignmentACKMessage::isSystemicCantco() const
{
   return _SystemicCantco;
}


void assignmentACKMessage::setOverridingID(const idRecord& value)
{
   _OverridingUnitID = value;
}

idRecord assignmentACKMessage::getOverridingID() const
{
   return _OverridingUnitID;
}

void assignmentACKMessage::setNewlyAssignedID(const idRecord& value)
{
   _NewlyAssignedID = value;
}

idRecord assignmentACKMessage::getNewlyAssignedID() const
{
   return _NewlyAssignedID;
}


void assignmentACKMessage::setUpdatedShotDoctrine(const eShotDoctrine& doctrine)
{
   _Updated_Shot_Doctrine = doctrine;
}

eShotDoctrine assignmentACKMessage::getUpdatedShotDoctrine() const
{
   return _Updated_Shot_Doctrine;
}

} // namespace il
