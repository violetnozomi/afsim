// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  cueMessage
 *  \file   cueMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <stdint.h>
#include <logger.hpp>
#include <iadsLib/cueMessage.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/util.hpp>

namespace il
{

cueMessage::cueMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : baseMessage(aGlobalLogger, CUE_MESSAGE)
   , _Time(-1)
   , _Reason(NEW_CUE)
{

}

baseMessage* cueMessage::clone() const
{
   return new cueMessage(*this);
}

void cueMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void cueMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void cueMessage::logSTD(std::ostream& os) const
{
   os << "Sensor " << ReasonToStr(_Reason) << " Message"    << "\n"
      << "---------------------" << "\n";
   baseMessage::logSTD(os);
   os << "           Cue Time: " << _Time          << "\n"
      << " Reference Track ID: " << _ReferenceTrackID << "\n"
      << "     Local Track ID: " << _LocalTrackID << "\n"
      << "      Initiating ID: " << _InitiatingID << "\n"
      << "       Cued Unit ID: " << _CuedUnitID    << "\n"
      << "     Cued Sensor ID: " << _CuedSensorID  << "\n";

   os  << "\n";
}

void cueMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Sensor " << ReasonToStr(_Reason) << " Message"    << ","
      << _Time                   << ","
      << _ReferenceTrackID       << ","
      << _LocalTrackID           << ","
      << _InitiatingID          << ","
      << _CuedUnitID             << ","
      << _CuedSensorID;

   os  << "\n";
}

void cueMessage::create(const double simTime,
                        const CueReason reason,
                        const idRecord& reference_track_number,
                        const idRecord& initiating_unit_id,
                        const idRecord& cued_unit_id,
                        const idRecord& cued_sensor_id)
{
   _Time = simTime;
   _Reason = reason;
   _ReferenceTrackID = _LocalTrackID = reference_track_number;
   _InitiatingID = initiating_unit_id;
   _CuedUnitID = cued_unit_id;
   _CuedSensorID = cued_sensor_id;
}

void cueMessage::setCueTime(const double time_s)
{
   _Time = time_s;
}

double cueMessage::getCueTime() const
{
   return _Time;
}

void cueMessage::setReferenceTrackID(const idRecord& value)
{
   _ReferenceTrackID = value;
}

idRecord cueMessage::getReferenceTrackID() const
{
   return _ReferenceTrackID;
}

void cueMessage::setLocalTrackID(const idRecord& value)
{
   _LocalTrackID = value;
}

idRecord cueMessage::getLocalTrackID() const
{
   return _LocalTrackID;
}

void cueMessage::setInitiatingID(const idRecord& value)
{
   _InitiatingID = value;
}

idRecord cueMessage::getInitiatingID() const
{
   return _InitiatingID;
}

void cueMessage::setCuedUnitID(const idRecord& value)
{
   _CuedUnitID = value;
}

idRecord cueMessage::getCuedUnitID() const
{
   return _CuedUnitID;
}

void cueMessage::setCuedSensorID(const idRecord& value)
{
   _CuedSensorID = value;
}

idRecord cueMessage::getCuedSensorID() const
{
   return _CuedSensorID;
}


void cueMessage::setCueReason(const CueReason reason)
{
   _Reason = reason;
}

cueMessage::CueReason cueMessage::getCueReason() const
{
   return _Reason;
}

std::string cueMessage::ReasonToStr(const CueReason reason)
{
   std::string reasonstr;
   switch (reason)
   {
      case NEW_CUE:
         reasonstr = "Cue";
         break;
      case CANCEL:
         reasonstr = "Cancel Cue";
         break;
      default:
         throw ("cueMessage::ReasonToStr(): invalid enumeration");
   }

   return reasonstr;
}

} // namespace il
