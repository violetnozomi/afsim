// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  bearingReportMessage
 *  \file   bearingReportMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 **  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/bearingReportMessage.hpp>

namespace il
{

bearingReportMessage::bearingReportMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger):
   baseMessage(aGlobalLogger, BEARING_REPORT_MESSAGE),
   _DataTime(0.0),
   _GlobalTrackNumber(0),
   _LocalTrackNumber(0),
   _X(0.0),
   _Y(0.0),
   _Z(0.0),
   _BearingAngle(0.0),
   _BearingAngleRateOfChange(0.0),
   _BearingAngleWidth(0.0),
   _ElevationReliable(0),
   _ElevationAngle(0.0),
   _ElevationAngleRateOfChange(0.0),
   _ElevationAngleWidth(0.0),
   _TrackingStatus(0)
{
}



baseMessage* bearingReportMessage::clone() const
{
   return new bearingReportMessage(*this);
}

void bearingReportMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void bearingReportMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void bearingReportMessage::logSTD(std::ostream& os) const
{
   os << "Bearing Report Message" << "\n"
      << "----------------------" << "\n";
   baseMessage::logSTD(os);
   os << "Data Time: " << _DataTime << "\n"
      << "Global Track Number: " << _GlobalTrackNumber << "\n"
      << "Local Track Number: " << _LocalTrackNumber << "\n"
      << "X: " << _X << "\n"
      << "Y: " << _Y << "\n"
      << "Z: " << _Z << "\n"
      << "Bearing Angle: " << _BearingAngle << "\n"
      << "Bearing Angle Rate Of Change: " << _BearingAngleRateOfChange << "\n"
      << "Bearing Angle Width: " << _BearingAngleWidth << "\n"
      << "Elevation Reliable: " << _ElevationReliable << "\n"
      << "Elevation Angle: " << _ElevationAngle << "\n"
      << "Elevation Angle Rate Of Change: " << _ElevationAngleRateOfChange << "\n"
      << "Elevation Angle Width: " << _ElevationAngleWidth << "\n"
      << "Tracking Status: " << _TrackingStatus << "\n";
}

void bearingReportMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Bearing Report Message" << ","
      << _DataTime << ","
      << _GlobalTrackNumber << ","
      << _LocalTrackNumber << ","
      << _X << ","
      << _Y << ","
      << _Z << ","
      << _BearingAngle << ","
      << _BearingAngleRateOfChange << ","
      << _BearingAngleWidth << ","
      << _ElevationReliable << ","
      << _ElevationAngle << ","
      << _ElevationAngleRateOfChange << ","
      << _ElevationAngleWidth << ","
      << _TrackingStatus << "\n";
}

} // namespace il
