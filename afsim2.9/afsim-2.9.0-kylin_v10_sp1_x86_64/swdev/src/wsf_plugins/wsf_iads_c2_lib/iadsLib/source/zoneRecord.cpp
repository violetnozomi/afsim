// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file zoneRecord.cpp
 *  @author Kenneth R. Sewell III \n
 *          Kenneth.Sewell@wpafb.af.mil \n
 *          AFRL/RYWF \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3004
 *
 *  Classification: UNCLASSIFIED
 *  Derived from gtiszon.prv - 13 May 05 - 5.0 baseline. DCFerguson
 */

#include <cfloat>
#include <iostream>
#include <logger.hpp>
#include <iadsLib/zoneRecord.hpp>
#include <vclMath/GreatCircle.hpp>
#include <vclMath/Constants.hpp>
#include <vclMath/Geometry.hpp>

namespace il
{

zoneRecordInterface* CreatePointDefenseZone (const std::string &my_name, const positionRecord &position, const il::tZonePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
{
   // Intialize self as priority 1
   ZoneRecord *zone_ptr = new il::ZoneRecord (il::E_ZONE_TYPE_OTHER, "Point Defense DA", priority, aGlobalLogger);
   zone_ptr->setAreaType (il::ZoneRecord::UNKNOWN);
   zone_ptr->setCenterInRad (position.getLLA());
   zone_ptr->setName (my_name);

   return zone_ptr;
}

ZoneRecord::ZoneRecord(const eZoneType &type_enum, const std::string &type_str, const tZonePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
   : zoneRecordInterface (type_enum, type_str, priority, aGlobalLogger)
   , m_Name ("Unnamed Zone")
   , m_AreaType(UNKNOWN)
   , m_FloorSet(false)
   , m_CeilingSet(false)
{
}


bool ZoneRecord::isInsideZone(const positionRecord &position) const
{
  // Check if altitude is outside of floor/ceiling limits (if enabled)
  if( ( m_FloorSet && ( position.getAltM() < m_Floor ))
      || ( m_CeilingSet && ( position.getAltM() > m_Ceiling ))
      )
    {
      return false;
    }

  if( CIRCULAR == m_AreaType )
    {
       return isInsideZoneCircular( position.getLLA() );
    }
  else if( COORDINATES == m_AreaType )
    {
      return isInsideZonePolygon( position.getLLA() );
    }
  else
    {
      return false;
    }
}

bool ZoneRecord::isInsideZoneCircular( const vm::Vector3<double> &positionLLA
                   ) const
{
  double distance = vm::GreatCircle<double>::
    calculateDistanceInRad( m_CenterLLA.getLatRads(),
                m_CenterLLA.getLonRads(),
                positionLLA.getLat(),
                positionLLA.getLon()
             ) * vm::ARCDEGREES_TO_METERS;

  if( m_Range >= distance )
    {
      if( m_Dome )
   {
     // Convert point to ECEF xyz
     vm::Vector3<double> point;
     vm::geodeticToECEF( positionLLA.getLat(),
               positionLLA.getLon(),
               positionLLA.getAlt(),
               point.getX(),
               point.getY(),
               point.getZ()
               );

     // Convert zone center to ECEF xyz
     vm::Vector3<double> center;
     vm::geodeticToECEF( m_CenterLLA.getLatRads(),
               m_CenterLLA.getLonRads(),
               m_CenterLLA.getAltM(),
               center.getX(),
               center.getY(),
               center.getZ()
               );

     // Calculate straight line distance between the two ECEF points.
     vm::Vector3<double> deltaXYZ( point - center );

     // If distance is less than the dome range, then point is inside.
     if( deltaXYZ.norm() <= m_Range )
       {
         return true;
       }
     else
       {
         return false;
       }
   }
      else
   {
     // Zone is a cylinder so we are done.
     return true;
   }
    }
  else
    {
      // Outside of zone.
      return false;
    }
}

bool ZoneRecord::isInsideZonePolygon( const vm::Vector3<double> &positionLLA
                  ) const
{
  if( m_CoordLon.empty() )
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Zone coordinates are not defined!" << std::endl;
      return false;
    }

  return vm::isPointInPolygon( positionLLA.getLon(), positionLLA.getLat(), m_CoordLon, m_CoordLat );
}

double ZoneRecord::getDistanceToZoneCenter(const positionRecord &position) const
{
   return vm::GreatCircle<double>::calculateDistanceInMeters(getCenterPoint().getLatDegs(),
      getCenterPoint().getLonDegs(), position.getLatDegs(), position.getLonDegs());
}

double ZoneRecord::getDistanceToZone( const positionRecord &position) const
{
   double distance = DBL_MAX;
  if( CIRCULAR ==  m_AreaType )
    {
       // JLK - retest this - I think there was a radians/degrees issue here
      distance = (vm::GreatCircle<double>
        ::calculateDistanceInMeters( m_CenterLLA.getLatDegs(),
                      m_CenterLLA.getLonDegs(),
                      position.getLatDegs(),
                      position.getLonDegs()
                      ) * vm::ARCDEGREES_TO_METERS )
   - m_Range;

      return distance;
    }
  else if( COORDINATES == m_AreaType )
    {
      if( m_CoordLon.size() < 3 )
   {
     HCL_ERROR_LOGGER(mGlobalLogger) << "Zone requires more than 2 coordinates!" << std::endl;
     return distance;
   }

      distance = 99999999.0;

      // Loop through all the vertices, starting with the last and first point.
      double latLast = m_CoordLat.back();
      double lonLast = m_CoordLon.back();
      for( unsigned int i = 0; i != m_CoordLat.size(); ++i )
   {
     double lat = m_CoordLat[i];
     double lon = m_CoordLon[i];

     vm::Vector3<double>P( position.getLonRads(),
            position.getLatRads(),
            0.0 );
     vm::Vector3<double>A( lonLast, latLast, 0.0 );
     vm::Vector3<double>B( lon, lat, 0.0 );

     vm::Vector3<double> C = vm::projectPointToLineSegment( P, A, B );

     C -= P;
     double tempDistance = C.norm();

     distance = std::min( tempDistance, distance );

     // Save this vertex as the last vertex.
     latLast = lat;
     lonLast = lon;
   }

      return distance;
    }
  else
    {
      return distance;
    }
}

bool ZoneRecord::getClosestPointInZone(const il::positionRecord &position, il::positionRecord &zonePoint) const
{
   if (CIRCULAR == m_AreaType)
   {
      auto point = position.getXYZ() - m_CenterLLA.getXYZ();
      point.normalize();
      point *= m_Range;
      point += m_CenterLLA.getXYZ();
      zonePoint.setXYZ(point.getX(), point.getY(), point.getZ());

      return true;
   }
   else if (COORDINATES == m_AreaType)
   {
      if (m_CoordLon.size() < 3)
      {
         HCL_ERROR_LOGGER(mGlobalLogger) << "Zone requires more than 2 coordinates!" << std::endl;
         return false;
      }

      const double C_MAX_DISTANCE = 99999999.0;
      double distance = C_MAX_DISTANCE;

      // Loop through all the vertices, starting with the last and first point.
      double latLast = m_CoordLat.back();
      double lonLast = m_CoordLon.back();
      for (unsigned int i = 0; i != m_CoordLat.size(); ++i)
      {
         double lat = m_CoordLat[i];
         double lon = m_CoordLon[i];

         vm::Vector3<double>P(position.getLonRads(),
            position.getLatRads(),
            0.0);
         vm::Vector3<double>A(lonLast, latLast, 0.0);
         vm::Vector3<double>B(lon, lat, 0.0);

         vm::Vector3<double> C = vm::projectPointToLineSegment(P, A, B);

         vm::Vector3<double> temp = C - P;
         double tempDistance = temp.norm();

         distance = std::min(tempDistance, distance);

         if (distance == tempDistance){
            zonePoint.setXYZ(C.getX(), C.getY(), C.getZ());
         }

         // Save this vertex as the last vertex.
         latLast = lat;
         lonLast = lon;
      }

      return distance != C_MAX_DISTANCE;
   }
   else
   {
      return false;
   }
}

bool ZoneRecord::isProjectedInsideZone( const positionRecord &position1, const positionRecord &position2) const
{
  if( CIRCULAR == m_AreaType )
    {
       return isProjectedInsideCircularZone( position1.getLLA(), position2.getLLA() );
    }
  else if( COORDINATES == m_AreaType )
    {
      return isProjectedInsidePolygonZone( position1.getLLA(), position2.getLLA() );
    }

  return false;
}

bool ZoneRecord::isProjectedInsideZone (const positionRecord &position, const double heading_rads) const
{
   HCL_ERROR_LOGGER(mGlobalLogger) << "ZoneRecord::isProjectedInsideZoneisProjectedInsideZone() is not implemented...returning false";

   return false;
}

bool ZoneRecord
::isProjectedInsideCircularZone( const vm::Vector3<double> &position1LLA,
             const vm::Vector3<double> &position2LLA
             ) const
{
  // If one of the positions is in the zone then true...
  if( isInsideZoneCircular( position1LLA )
      || isInsideZoneCircular( position2LLA )
      )
    {
      return true;
    }

  // Otherwise we need to check the course defined by the two points.
  double distance = vm::GreatCircle<double>
    ::calculateDistanceFromPointToGreatCircleInRad( m_CenterLLA.getLatRads(),
                      m_CenterLLA.getLonRads(),
                      position1LLA.getLat(),
                      position1LLA.getLon(),
                      position2LLA.getLat(),
                      position2LLA.getLon()
                      )
    * vm::ARCDEGREES_TO_METERS;

  // If distance from center to line is less than radius, return true.
  return( distance < m_Range );
}

bool ZoneRecord
::isProjectedInsidePolygonZone( const vm::Vector3<double> &position1LLA,
            const vm::Vector3<double> &position2LLA
            ) const
{
  if( m_CoordLon.size() < 3 )
    {
      HCL_ERROR_LOGGER(mGlobalLogger) << "Zone requires more than 2 coordinates!" << std::endl;
      return false;
    }

  // Loop through all the vertices, starting with the last and first point.
  double xi, yi;
  double latLast = m_CoordLat.back();
  double lonLast = m_CoordLon.back();
  for( unsigned int i = 0; i != m_CoordLat.size(); ++i )
    {
      double lat = m_CoordLat[i];
      double lon = m_CoordLon[i];

      if( vm::intersectLineSegments( lonLast, latLast,
                 lon, lat,
                 position1LLA.getLon(),
                 position1LLA.getLat(),
                 position2LLA.getLon(),
                 position2LLA.getLat(),
                 xi, yi )
     )
   {
     return true;
   }

      // Save this vertex as the last vertex.
      latLast = lat;
      lonLast = lon;
    }

  return false;
}

} // namespace il
