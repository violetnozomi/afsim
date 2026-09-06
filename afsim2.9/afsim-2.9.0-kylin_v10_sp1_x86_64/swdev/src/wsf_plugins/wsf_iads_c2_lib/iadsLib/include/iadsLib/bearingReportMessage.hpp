// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
*  @class  bearingReportMessage
*  @file   bearingReportMessage.hpp
*  @author Kenneth R. Sewell III
*          AFRL/RYWF
*          2241 Avionics Circle
*          WPAFB, Ohio 45433-7303
*          937-656-4899 x3004
*
**  Classification: UNCLASSIFIED
*/

#include <iadsLib/baseMessage.hpp>

#ifndef BEARINGREPORTMESSAGE_HPP
#define BEARINGREPORTMESSAGE_HPP

namespace il
{
class bearingReportMessage : public baseMessage
{
   public:

      bearingReportMessage(std::weak_ptr<logger::GlobalLogger> aGlobalLogger);
      virtual ~bearingReportMessage() = default;

      baseMessage* clone() const override;

      virtual void logSTD() const;
      virtual void logCSV(const double& Time) const;
      void logSTD(std::ostream& os) const override;
      void logCSV(std::ostream& os, const double& Time) const override;

   protected:
      // Timestamp of message data.
      double _DataTime;
      unsigned int _GlobalTrackNumber;
      unsigned int _LocalTrackNumber;

      // Position of asset reporting bearing in WGS84(m)
      double _X;
      double _Y;
      double _Z;

      // Bearing angle from North (radians)
      double _BearingAngle;

      // Bearing angle rate of change (radians/sec)
      double _BearingAngleRateOfChange;

      // Bearing angle width (radians)
      double _BearingAngleWidth;

      // Is Elevation value valid
      unsigned char _ElevationReliable;

      // Elevation angle (radians)
      double _ElevationAngle;

      // Elevation angle rate of change (radians/sec)
      double _ElevationAngleRateOfChange;

      // Elevation angle width (radians)
      double _ElevationAngleWidth;

      // Track is: New, Lost, Dropped, Extrapolated, Update...
      unsigned char _TrackingStatus;
};
}

#endif
