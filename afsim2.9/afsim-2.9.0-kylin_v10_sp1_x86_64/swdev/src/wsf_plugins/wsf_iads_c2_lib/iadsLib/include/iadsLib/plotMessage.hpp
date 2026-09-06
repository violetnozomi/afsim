// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @class  plotMessage
 *  @file   plotMessage.hpp
 *  @author Kenneth R. Sewell III
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 **  Classification: UNCLASSIFIED
 */

#include <iadsLib/baseMessage.hpp>
#include <vector>
#include <stdint.h>

#ifndef PLOTMESSAGE_HPP
#define PLOTMESSAGE_HPP

namespace il
{
class singlePlot
{
   public:
      singlePlot(const float& Range = 0.0f,
                  const float& Az = 0.0f,
                  const float& El = 0.0f);
      singlePlot(const singlePlot& Plot);
      ~singlePlot();

      virtual void log(std::ostream& os) const;
      virtual void logCSV(std::ostream& os, const double& Time) const;

      singlePlot& operator=(const singlePlot& plot);

      /**
         @param range Distance of measurement from platform in meters. (0, inf)
         @param azimuth Clockwise angle from North, to target in radians [0,2pi)
         @param elevation Elevation angle to target in radians [-pi/2, pi/2]
         */
      void setMeasurement(const float& range,
                           const float& azimuth,
                           const float& elevation);

      void setMeasurement(const std::vector<float>& measurement);
      const std::vector<float>& getMeasurement() const;

      void setRange(const float& range);
      void setAzimuth(const float& azimuth);
      void setElevation(const float& elevation);

      const float& getRange() const;
      const float& getAzimuth() const;
      const float& getElevation() const;

   protected:
      std::vector<float> _Measurement;
};

class plotMessage : public baseMessage
{
   public:
      enum MaxPlots
      {
         MAX_PLOTS = 107
      };

      plotMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      plotMessage(const plotMessage& message);
      virtual ~plotMessage() = default;

      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

      void setDataTime(const double& DataTime);
      const double& getDataTime() const;

      void setSystemID(const uint32_t& SystemID);
      const uint32_t& getSystemID() const;

      void setPlatformLocation(const float& lat,
                                 const float& lon,
                                 const float& alt);
      void setPlatformLocation(const std::vector<float>& location);
      const std::vector<float>& getPlatformLocation() const;
      std::vector<float>& getPlatformLocation();

      void setCovariance(const float& range, const float& az, const float& el);
      void setCovariance(const std::vector<float>& covar);
      const std::vector<float>& getCovariance() const;
      std::vector<float>& getCovariance();

      void setSearchVolume(const float& minRange, const float& maxRange,
                           const float& minAz, const float& maxAz,
                           const float& minEl, const float& maxEl);
      void setSearchVolume(const std::vector<float>& searchVol);
      const std::vector<float>& getSearchVolume() const;
      std::vector<float>& getSearchVolume();

      size_t getNumPlots() const;
      bool addPlot(const singlePlot& Plot);
      bool addPlot(const float& Range, const float& Az, const float& El);
      const singlePlot& getPlot(const uint8_t& index) const;
      void getPlot(const uint8_t& index, float& Range, float& Az, float& El) const;

   protected:
      double _DataTime;

      // System Unit ID
      uint32_t _SystemID;

      /// Sensing platform location lat/lon/alt (rad, rad, m)
      std::vector<float> _platformLocation;
      std::vector<float> _covariance;
      std::vector<float> _searchVolume;
      std::vector<singlePlot> _plots;
};
}

#endif
