// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  airTargetMessage
 *  \file   airTargetMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/airTargetMessage.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <cstring>

#if DEBUG_IADS
#include <cassert>
#endif

namespace il
{

/********************************************************************/

airTargetMessage::airTargetMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger) :
   baseMessage(aGlobalLogger, AIR_TARGET_MESSAGE),
   _DataTime(0.0),
   _UpdateInterval(0.0),
   _LLA(6, 0.0),
   _Covariance(36, 0.0),
   _Psi_rads(0),
   _Theta_rads(0),
   _Phi_rads(0),
   _Heading_rads(0),
   _IFF(E_IFF_UNKNOWN),
   _Quantity(1),
   _DisID(3, 0),
   _TargetTruthName("Unknown"),
   _ReportingSensorType("Unknown")
{
}

baseMessage* airTargetMessage::clone() const
{
   return new airTargetMessage(*this);
}

/**
   void logSTD()
   \brief Prints class to info logger.
*/
void airTargetMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void airTargetMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

/**
   void logSTD()
   \brief Prints class to any ostream.
*/
void airTargetMessage::logSTD(std::ostream& os) const
{
   os << std::fixed
      << "Air Target Message" << "\n"
      << "------------------" << "\n";
   baseMessage::logSTD(os);
   os << "Data Time: " << _DataTime << "\n"
      << "Update Interval: " << _UpdateInterval << "\n"
      << "ID: " << _TrackID << "\n"
      << " Latitude: " << _LLA[0] << "\n"
      << "Longitude: " << _LLA[1] << "\n"
      << " Altitude: " << _LLA[2] << "\n"
      << "Dx: " << _LLA[3] << "\n"
      << "Dy: " << _LLA[4] << "\n"
      << "Dz: " << _LLA[5] << "\n"
      << "Covariance matrix:";
   for (unsigned int i = 0; i < _Covariance.size(); ++i)
   {
      if (0 == (i % 6))
      {
         os << "\n";
      }
      os << std::setw(10) << std::setprecision(15) << std::fixed << _Covariance[i] << "\t";
   }
   os << "\n"
      << "Psi: " << getPsiDegs() << "\n"
      << "Theta: " << getThetaDegs() << "\n"
      << "Phi: " << getPhiDegs() << "\n"
      << "Heading: " << getHeadingDegs() << "\n"
      << "Type: " << _TargetType << "\n"
      << "Status: " << static_cast<int>(_TrackingStatus) << "\n"
      << "DIS ID: " << _DisID[0] << "\t" << _DisID[1] << "\t"
      << _DisID[2] << "\n"
      << "Target Truth: " << _TargetTruthName << " , ID: " << _TargetTruthID << "\n"
      << "Reporting Sensor Type: " << _ReportingSensorType;
}

void airTargetMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Air Target Message" << ","
      << _DataTime << ","
      << _UpdateInterval << ","
      << _TrackID << ","
      << _LLA[0] << ","
      << _LLA[1] << ","
      << _LLA[2] << ","
      << _LLA[3] << ","
      << _LLA[4] << ","
      << _LLA[5] << ","
      << getPsiDegs() << ","
      << getThetaDegs() << ","
      << getPhiDegs() << ","
      << getHeadingDegs() << ","
      << _TargetType << ","
      << static_cast<int>(_TrackingStatus) << ","
      << _DisID[0] << ","
      << _DisID[1] << ","
      << _DisID[2] << ","
      << _TargetTruthName << ","
      << _TargetTruthID << ","
      << _ReportingSensorType;
}


void airTargetMessage::setCovarianceMatrix(const std::vector<double>& covar)
{
   if (36 == covar.size())
   {
      _Covariance = covar;
   }
}

const std::vector<double>& airTargetMessage::getCovarianceMatrix() const
{
   return _Covariance;
}

std::vector<double> airTargetMessage::getCovarianceMatrix()
{
   return _Covariance;
}

void airTargetMessage::setDisID(const std::vector<unsigned short>& id)
{
   if (3 == _DisID.size())
   {
      _DisID = id;
   }
}

const std::vector<unsigned short>& airTargetMessage::getDisID() const
{
   return _DisID;
}

std::vector<unsigned short>& airTargetMessage::getDisID()
{
   return _DisID;
}

const std::vector<double>& airTargetMessage::getLLAVel() const
{
   return _LLA;
}

std::vector<double>& airTargetMessage::getLLAVel()
{
   return _LLA;
}

} // namespace il
