// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \class  plotMessage
 *  \file   plotMessage.cpp
 *  \author Kenneth R. Sewell III \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004 \n
 *
 *  Classification: UNCLASSIFIED
 */

#include <logger.hpp>
#include <iadsLib/plotMessage.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <cstring>

#if DEBUG_IADS
#include <cassert>
#endif

namespace il
{

/********************************************************************/
singlePlot::singlePlot(const float& Range, const float& Az, const float& El):
   _Measurement(3)
{
   _Measurement[0] = Range;
   _Measurement[1] = Az;
   _Measurement[2] = El;
}

singlePlot::singlePlot(const singlePlot& Plot)
{
   operator=(Plot);
}

singlePlot::~singlePlot()
{
}


void singlePlot::log(std::ostream& os) const
{
   os << std::fixed
      << "Plot" << "\n"
      << "----" << "\n"
      << "    Range: " << _Measurement[0] << "\n"
      << "  Azimuth: " << _Measurement[1] << "\n"
      << "Elevation: " << _Measurement[2] << "\n";
}

void singlePlot::logCSV(std::ostream& os, const double& Time) const
{
   os << std::fixed << ","
      << "Plot" << ","
      << _Measurement[0] << ","
      << _Measurement[1] << ","
      << _Measurement[2];
}

singlePlot& singlePlot::operator=(const singlePlot& plot)
{
   if (this != &plot)
   {
      _Measurement = plot._Measurement;
   }

   return *this;
}

void singlePlot::setMeasurement(const std::vector<float>& measurement)
{
   std::copy(measurement.begin(), measurement.end(), _Measurement.begin());
}

void singlePlot::setMeasurement(const float& range,
                                const float& azimuth,
                                const float& elevation)
{
   _Measurement[0] = range;
   _Measurement[1] = azimuth;
   _Measurement[2] = elevation;
}

const std::vector<float>& singlePlot::getMeasurement() const
{
   return _Measurement;
}

void singlePlot::setRange(const float& range)
{
   _Measurement[0] = range;
}

void singlePlot::setAzimuth(const float& azimuth)
{
   _Measurement[1] = azimuth;
}

void singlePlot::setElevation(const float& elevation)
{
   _Measurement[2] = elevation;
}

const float& singlePlot::getRange() const
{
   return _Measurement[0];
}

const float& singlePlot::getAzimuth() const
{
   return _Measurement[1];
}

const float& singlePlot::getElevation() const
{
   return _Measurement[2];
}

/********************************************************************/

plotMessage::plotMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : baseMessage(aGlobalLogger, PLOT_MESSAGE),
   _DataTime(0.0),
   _platformLocation(3, 0.0f),
   _covariance(3, 0.0f),
   _searchVolume(6, 0.0f)
{
}

plotMessage::plotMessage(const plotMessage& message)
   : baseMessage(message)
{
   operator=(message);
}

baseMessage* plotMessage::clone() const
{
   return new plotMessage(*this);
}

void plotMessage::logSTD() const
{
   std::stringstream strm_out;
   logSTD(strm_out);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}

void plotMessage::logCSV(const double& Time) const
{
   std::stringstream strm_out;
   logCSV(strm_out, Time);
   HCL_INFO_LOGGER(mGlobalLogger) << strm_out.str();
}


/**
   void logSTD()
   \brief Prints class to any ostream.
*/
void plotMessage::logSTD(std::ostream& os) const
{
   os << std::fixed
      << "Plot Message" << "\n"
      << "------------" << "\n";
   baseMessage::logSTD(os);
   os << "    Data Time: " << _DataTime << "\n"
      << "    System ID: " << _SystemID << "\n"
      << " Platform Lat: " << _platformLocation[0] << "\n"
      << " Platform Lon: " << _platformLocation[1] << "\n"
      << " Platform Alt: " << _platformLocation[2] << "\n"
      << "   Covariance: " << _covariance[0] << "\n"
      << "              " << _covariance[1] << "\n"
      << "              " << _covariance[2] << "\n"
      << "Search Volume: Range: " << _searchVolume[0]
      << ".." << _searchVolume[1] << "\n"
      << "                  Az: " << _searchVolume[2]
      << ".." << _searchVolume[3] << "\n"
      << "                  El: " << _searchVolume[4]
      << ".." << _searchVolume[5] << "\n"
      << "   Num plots:         " << _plots.size() << "\n";
   for (unsigned int i = 0; i < _plots.size(); ++i)
   {
      _plots[i].log(os);
   }
}

void plotMessage::logCSV(std::ostream& os, const double& Time) const
{
   baseMessage::logCSV(os, Time);
   os << std::fixed << ","
      << "Plot Message" << ","
      << _DataTime << ","
      << _SystemID << ","
      << _platformLocation[0] << ","
      << _platformLocation[1] << ","
      << _platformLocation[2] << ","
      << _covariance[0] << ","
      << _covariance[1] << ","
      << _covariance[2] << ","
      << _searchVolume[0] << ","
      << _searchVolume[1] << ","
      << _searchVolume[2] << ","
      << _searchVolume[3] << ","
      << _searchVolume[4] << ","
      << _searchVolume[5] << ","
      << _plots.size();
   for (unsigned int i = 0; i < _plots.size(); ++i)
   {
      os << ",";
      _plots[i].log(os);
   }
}


void plotMessage::setDataTime(const double& DataTime)
{
   _DataTime = DataTime;
}

const double& plotMessage::getDataTime() const
{
   return _DataTime;
}

void plotMessage::setSystemID(const unsigned int& SystemID)
{
   _SystemID = SystemID;
}

const unsigned int& plotMessage::getSystemID() const
{
   return _SystemID;
}

void plotMessage::setPlatformLocation(const float& lat,
                                      const float& lon,
                                      const float& alt)
{
   _platformLocation[0] = lat;
   _platformLocation[1] = lon;
   _platformLocation[2] = alt;
}

void plotMessage::setPlatformLocation(const std::vector<float>& location)
{
   _platformLocation = location;
}

const std::vector<float>& plotMessage::getPlatformLocation() const
{
   return _platformLocation;
}

std::vector<float>& plotMessage::getPlatformLocation()
{
   return _platformLocation;
}

void plotMessage::setCovariance(const float& range,
                                const float& az,
                                const float& el)
{
   _covariance[0] = range;
   _covariance[1] = az;
   _covariance[2] = el;
}

void plotMessage::setCovariance(const std::vector<float>& covariance)
{
   _covariance = covariance;
}

const std::vector<float>& plotMessage::getCovariance() const
{
   return _covariance;
}

std::vector<float>& plotMessage::getCovariance()
{
   return _covariance;
}

void plotMessage::setSearchVolume(const float& minRange, const float& maxRange,
                                  const float& minAz, const float& maxAz,
                                  const float& minEl, const float& maxEl)
{
   _searchVolume[0] = minRange;
   _searchVolume[1] = maxRange;
   _searchVolume[2] = minAz;
   _searchVolume[3] = maxAz;
   _searchVolume[4] = minEl;
   _searchVolume[5] = maxEl;
}

void plotMessage::setSearchVolume(const std::vector<float>& SearchVolume)
{
   _searchVolume = SearchVolume;
}

const std::vector<float>& plotMessage::getSearchVolume() const
{
   return _searchVolume;
}

std::vector<float>& plotMessage::getSearchVolume()
{
   return _searchVolume;
}

size_t plotMessage::getNumPlots() const
{
   return _plots.size();
}

bool plotMessage::addPlot(const singlePlot& Plot)
{
   if (_plots.size() <= MAX_PLOTS)
   {
      _plots.push_back(Plot);
      return true;
   }
   else
   {
      return false;
   }
}

bool plotMessage::addPlot(const float& Range, const float& Az, const float& El)
{
   return addPlot(singlePlot(Range, Az, El));
}

const singlePlot& plotMessage::getPlot(const uint8_t& index) const
{
   return _plots.at(index);
}

} // namespace il
