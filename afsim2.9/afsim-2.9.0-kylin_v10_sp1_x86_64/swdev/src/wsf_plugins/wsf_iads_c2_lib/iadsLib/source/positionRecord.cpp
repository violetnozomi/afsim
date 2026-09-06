// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  \file   positionRecord.hpp
 *  \author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 */

#include <iadsLib/positionRecord.hpp>
#include <vclMath/GeodeticToECEF.hpp>
#include <vclMath/ECEFToGeodetic.hpp>
#include <vclMath/GreatCircle.hpp>

namespace il
{

positionRecord::positionRecord()
{}

positionRecord::positionRecord( const positionRecord &record ):
  m_LLA( record.m_LLA ),
  m_XYZ( record.m_XYZ )
{}

positionRecord::~positionRecord(){}

positionRecord &positionRecord::operator=( const positionRecord &record )
{
  if( &record != this )
    {
      m_LLA = record.m_LLA;
      m_XYZ = record.m_XYZ;
    }

  return *this;
}

/**
   @param LLA Lat/Lon/Alt in radians/meters.
 */
void positionRecord::setLLA( const vm::Vector3<double> &LLA )
{
  m_LLA = LLA;
  vm::geodeticToECEF( m_LLA.getLat(),
		      m_LLA.getLon(),
		      m_LLA.getAlt(),
		      m_XYZ.getX(),
		      m_XYZ.getY(),
		      m_XYZ.getZ()
		      );
}

/**
   @param Lat Latitude in radians.
   @param Lon Longitude in radians.
   @param Alt Altitude in meters.
 */
void positionRecord::setLLA( const double &Lat,
			     const double &Lon,
			     const double &Alt )
{
  m_LLA.set( Lat, Lon, Alt );
  vm::geodeticToECEF( m_LLA.getLat(),
		      m_LLA.getLon(),
		      m_LLA.getAlt(),
		      m_XYZ.getX(),
		      m_XYZ.getY(),
		      m_XYZ.getZ()
		      );
}

/**
   @param Lat Latitude in radians.
   @param Lon Longitude in radians.
   @param Alt Altitude in meters.
 */
void positionRecord::setLLADegrees( const double &Lat,
				    const double &Lon,
				    const double &Alt )
{

  m_LLA.set( Lat * vm::DEGREES_TO_RADIANS,
	     Lon * vm::DEGREES_TO_RADIANS, Alt );
  vm::geodeticToECEF( m_LLA.getLat(),
		      m_LLA.getLon(),
		      m_LLA.getAlt(),
		      m_XYZ.getX(),
		      m_XYZ.getY(),
		      m_XYZ.getZ()
		      );
}

/**
   @returns Lat/Lon/Alt in radians/meters.
 */
const vm::Vector3<double> &positionRecord::getLLA() const
{
  return m_LLA;
}

/**
   @returns Lat/Lon/Alt in radians/meters.
 */
vm::Vector3<double> &positionRecord::getLLA()
{
  return m_LLA;
}

/**
   @param XYZ ECEF XYZ coordinates in meters.
 */
void positionRecord::setXYZ( const vm::Vector3<double> &XYZ )
{
  m_XYZ = XYZ;
  vm::ecefToGeodetic( m_XYZ.getX(),
		      m_XYZ.getY(),
		      m_XYZ.getZ(),
		      m_LLA.getLat(),
		      m_LLA.getLon(),
		      m_LLA.getAlt()
		      );
}

/**
   @param X ECEF X coordinate in meters.
   @param Y ECEF Y coordinate in meters.
   @param Z ECEF Z coordinate in meters.
 */
void positionRecord::setXYZ( const double &X,
			     const double &Y,
			     const double &Z )
{
  m_XYZ.set(X, Y, Z);
  vm::ecefToGeodetic( m_XYZ.getX(),
		      m_XYZ.getY(),
		      m_XYZ.getZ(),
		      m_LLA.getLat(),
		      m_LLA.getLon(),
		      m_LLA.getAlt()
		      );
}

/**
   @returns ECEF XYZ position in meters.
*/
const vm::Vector3<double> &positionRecord::getXYZ() const
{
  return m_XYZ;
}

/**
   @returns ECEF XYZ position in meters.
*/
vm::Vector3<double> &positionRecord::getXYZ()
{
  return m_XYZ;
}

void positionRecord::getXYZ( double &X, double &Y, double &Z ) const
{
  m_XYZ.get( X, Y, Z );
}

const double &positionRecord::getX() const
{
  return m_XYZ.getX();
}

const double &positionRecord::getY() const
{
  return m_XYZ.getY();
}

const double &positionRecord::getZ() const
{
  return m_XYZ.getZ();
}


const double &positionRecord::getLatRads() const
{
  return m_LLA.getLat();
}

const double &positionRecord::getLonRads() const
{
  return m_LLA.getLon();
}


const double positionRecord::getLatDegs() const
{
   return m_LLA.getLat() * vm::RADIANS_TO_DEGREES;
}

const double positionRecord::getLonDegs() const
{
   return m_LLA.getLon() * vm::RADIANS_TO_DEGREES;
}

const double positionRecord::getAltM() const
{
   return m_LLA.getAlt();
}

const double positionRecord::calculateDistanceMeters(const positionRecord &other_position) const
{
   return (this->getXYZ() - other_position.getXYZ()).norm();
}

const double positionRecord::calculateGroundDistanceMeters(const positionRecord &other_position) const
{
   return vm::GreatCircle<double>::calculateDistanceInMeters(other_position.getLatDegs(),
      other_position.getLonDegs(), getLatDegs(), getLonDegs());
}

} // namespace il
