// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file   postionRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <vclMath/Vector3.hpp>
#include "iadslib_export.h"

#ifndef IADSLIB_POSITIONRECORD_HPP
#define IADSLIB_POSITIONRECORD_HPP

/**
 *  @brief Encapsulate all IADSLIB functions and classes.
 */
namespace il
{
    /**
       @brief Class to represent position in geodetic and ecef.
    */
    class IADSLIB_EXPORT positionRecord
    {
    public:

      positionRecord();
      positionRecord( const positionRecord &record );
      ~positionRecord();

      positionRecord &operator=( const positionRecord &record );

      void setLLA( const vm::Vector3<double> &LLA );
      void setLLADegrees( const double &Lat, const double &Lon,
			  const double &Alt );
      void setLLA( const double &Lat, const double &Lon, const double &Alt );
      const vm::Vector3<double> &getLLA() const;
      vm::Vector3<double> &getLLA();

      void setXYZ( const vm::Vector3<double> &XYZ );
      void setXYZ( const double &X, const double &Y, const double &Z );
      const vm::Vector3<double> &getXYZ() const;
      vm::Vector3<double> &getXYZ();
      void getXYZ( double &X, double &Y, double &Z ) const;

      const double &getX() const;
      const double &getY() const;
      const double &getZ() const;

      const double &getLatRads() const;
      const double &getLonRads() const;

      const double getLatDegs() const;
      const double getLonDegs() const;

      const double getAltM() const;

      // some helper utilities
      const double calculateDistanceMeters(const positionRecord &other_position) const;
      const double calculateGroundDistanceMeters(const positionRecord &other_position) const;

    protected:
      vm::Vector3<double> m_LLA;
      vm::Vector3<double> m_XYZ;

    private:

    };
}
#endif
