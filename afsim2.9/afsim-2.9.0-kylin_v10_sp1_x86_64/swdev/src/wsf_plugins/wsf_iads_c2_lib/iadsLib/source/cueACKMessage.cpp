// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  cueACKMessage
 *  \file   cueACKMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/cueACKMessage.hpp>
#include <iadsLib/colorText.hpp>
#include <sstream>

namespace il
{

cueACKMessage::cueACKMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger):
   baseMessage(aGlobalLogger, CUE_ACK_MESSAGE),
   _AckTime(0.0),
   _Status(UNACKNOWLEDGED),
   _SystemicCantco(false),
   _SystemicCantcoScope(LOCAL)    // default to something but not relevant unless systemic set is called
{

}

baseMessage* cueACKMessage::clone() const
{
   return new cueACKMessage(*this);
}

void cueACKMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void cueACKMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void cueACKMessage::logSTD(std::ostream& os) const
{
   os << "Cue ACK Message" << "\n"
      << "----------------------" << "\n";
   baseMessage::logSTD(os);
   os << "ACK Time: " << _AckTime << "\n"
      << "Assigned System ID: " << _AssignedID << "\n"
      << "Track ID: " <<  _TrackID << "\n"
      << "Comply Status: " << getComplyStatusString() << "\n";
}

void cueACKMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Cue ACK Message" << ","
      << _AckTime << ","
      << _AssignedID << ","
      <<  _TrackID   << ","
      << getComplyStatusString() << std::endl;
}


std::string cueACKMessage::getComplyStatusString() const
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
      case CHANGED_ASSIGNED_UNIT:
         status  << GREEN_TEXT_BLACK_BG << "CHANGED_ASSIGNED_UNIT";
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
         strm_error << "cueACKMessage::getComplyStatusString(): Unrecognized cue status (" << _Status << ")\n";
         HCL_FATAL_LOGGER(mGlobalLogger) << strm_error.str();
         throw (strm_error.str());
   }

   return status.str();
}



void cueACKMessage::setAckTime(const double DataTime)
{
   _AckTime = DataTime;
}

double cueACKMessage::getAckTime() const
{
   return _AckTime;
}

void cueACKMessage::setAssignedID(const idRecord& value)
{
   _AssignedID = value;
}

idRecord cueACKMessage::getAssignedID() const
{
   return _AssignedID;
}

void cueACKMessage::setInitiatingID(const idRecord& value)
{
   _InitiatingUnitID = value;
}

idRecord cueACKMessage::getInitiatingID() const
{
   return _InitiatingUnitID;
}

void cueACKMessage::setAssigningID(const idRecord& value)
{
   _AssigningUnitID = value;
}

idRecord cueACKMessage::getAssigningID() const
{
   return _AssigningUnitID;
}

void cueACKMessage::setTrackID(const idRecord& value)
{
   _TrackID = value;
}

idRecord cueACKMessage::getTrackID() const
{
   return _TrackID;
}


void cueACKMessage::setStatus(const eCueStatus status)
{
   _Status = status;
}

cueACKMessage::eCueStatus cueACKMessage::getStatus() const
{
   return _Status;
}

void cueACKMessage::setCantcoReason(const std::string& reason)
{
   _CantcoReason = reason;
}

std::string cueACKMessage::getCantoReason() const
{
   return _CantcoReason;
}

void cueACKMessage::setSystemicCantcoScope(const eSystemicCantcoScope scope)
{
   _SystemicCantco = true;
   _SystemicCantcoScope = scope;
}

bool cueACKMessage::getSystemicCantcoScope(eSystemicCantcoScope& scope) const
{
   if (_SystemicCantco)
   { scope = _SystemicCantcoScope; }

   return _SystemicCantco;
}

bool cueACKMessage::isSystemicCantco() const
{
   return _SystemicCantco;
}


void cueACKMessage::setOverridingID(const idRecord& value)
{
   _OverridingUnitID = value;
}

idRecord cueACKMessage::getOverridingID() const
{
   return _OverridingUnitID;
}

void cueACKMessage::setNewlyAssignedID(const idRecord& value)
{
   _NewlyAssignedID = value;
}

idRecord cueACKMessage::getNewlyAssignedID() const
{
   return _NewlyAssignedID;
}

} // namespace il
