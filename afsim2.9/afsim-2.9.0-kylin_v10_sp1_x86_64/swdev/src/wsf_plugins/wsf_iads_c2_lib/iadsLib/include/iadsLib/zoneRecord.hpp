// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/** -*-c++-*-
 *  @file zoneRecord.hpp
 *  @author Kenneth R. Sewell III
 *          Kenneth.Sewell@wpafb.af.mil
 *          AFRL/RYWF
 *          2241 Avionics Circle
 *          WPAFB, Ohio 45433-7303
 *          937-656-4899 x3004
 *  @brief Class to represent zone dimensions and types.
 *
 *  Classification: UNCLASSIFIED
 *  Derived from gtiszon.prv - 13 May 05 - 5.0 baseline. DCFerguson
 */

#include <string>
#include <list>
#include <vclMath/Constants.hpp>
#include <vclMath/Vector3.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include <iadsLib/positionRecord.hpp>


#ifndef IADSLIB_ZONERECORD_HPP
#define IADSLIB_ZONERECORD_HPP

/**
 *  @brief Encapsulate all il functions and classes.
 */
namespace il
{

   zoneRecordInterface *CreatePointDefenseZone (const std::string &my_name, const positionRecord &position, const il::tZonePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

  /**
   *  @brief Class to represent zone dimensions and types.
   */
  class ZoneRecord : public zoneRecordInterface
  {
  public:
     virtual ~ZoneRecord() = default;
    /// Enumerated zone types.
    enum AREATYPE {
      /// Undefined zone type
      UNKNOWN = 0,
      /// Circular zone
      CIRCULAR = 1,
      /// Polygonal zone defined by a list of lat/lon points.
      COORDINATES = 2
    };

    /**
       @brief Constructor.
    */
    ZoneRecord(const eZoneType &type_enum, const std::string &type_str, const tZonePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

    /**
       @brief Set the name of the zone.
       @param Name of the zone.
    */
    inline void setName ( const std::string &name ) { m_Name = name; }

    /**
       @brief Get the name of the zone.
       @param Name of the zone.
    */
    inline virtual std::string getName () const { return m_Name; }

    /**
       @brief Set the center point of the zone.
       @param center Center point of the zone Lat/Lon/Alt in degrees and meters.
    */
    inline void setCenterInDeg( const vm::Vector3<double> &center )
    {
      m_CenterLLA.setLLADegrees (center[0], center[1], center[2]);
    }

    /**
       @brief Set the center point of the zone.
       @param center Center point of the zone Lat/Lon/Alt in radians and meters.
    */
    inline void setCenterInRad( const vm::Vector3<double> &center )
    {
      m_CenterLLA.setLLA (center[0], center[1], center[2]);
    }

    /**
       @brief Get the center point of the zone.
       @returns Center point of the zone Lat/Lon/Alt in rad and meters.
    */
    inline virtual positionRecord getCenterPoint() const
    {
      return m_CenterLLA;
    }

    /**
       @brief Set the type of zone.
       @param type Type of zone( See il::ZoneRecord::AREATYPE )
    */
    inline void setAreaType( const AREATYPE &type )
    {
      m_AreaType = type;
    }

    /**
       @brief Get the type of zone.
       @returns Type of zone( See AREATYPE )
    */
    inline AREATYPE getAreaType() const
    {
      return m_AreaType;
    }

    /**
       @brief Set whether the circular zone is a cylinder or a dome.
       @param isDomed If true, circular zone is a dome, else a cylinder.
    */
    inline void setDomed( const bool &isDomed )
    {
      m_Dome = isDomed;
    }

    /**
       @brief Get whether the circular zone is a cylinder or a dome.
       @returns isDomed True if circular zone is a dome, else false.
    */
    inline bool isDomed() const
    {
      return m_Dome;
    }

    /**
       @brief Set the range/radius for a circular zone in meters.
       @param range Radius of circular zone in meters.
    */
    inline void setRange( const double &range )
    {
      m_Range = range;
    }

    /**
       @brief Get the range/radius for a circular zone in meters.
       @returns The range/radius for a circular zone in meters.
    */
    inline double getRange() const
    {
      return m_Range;
    }

    /**
    @brief Calculate distance from point to zone center in meters.
    @param positionLLA Point
    @returns distance Distance from point to zone center in meters
    */
    virtual double getDistanceToZoneCenter(const positionRecord &position) const;

    /**
       @brief Calculate shortest distance from point to zone edge in meters.
       @param positionLLA Point
       @returns distance Shortest distance from point to zone edge in meters
    */
    virtual double getDistanceToZone(const positionRecord &position) const;


    /**
       @brief Find closest point in zone to given position record.
       @param position Point
       @param zonePoint Closest point in zone to given position record.
       @returns true if closest point was found and zonePoint was successfully set to found point.
    */
    virtual bool getClosestPointInZone(const il::positionRecord &position, il::positionRecord &zonePoint) const;

    /**
       @brief Determine if point is inside zone.
       @param positionLLA Point
       @returns true if point is inside zone, false otherwise.
    */
    virtual bool isInsideZone(const positionRecord &position) const;

    /**
       @brief Determine if two points create an intersecting line with a zone.
       @param positionLLA Point 1
       @param positionLLA Point 2
       @returns true if the points are projecting into the zone.
    */
    virtual bool isProjectedInsideZone(const positionRecord &position1, const positionRecord &position2) const;

    /**
       @brief Determine if an arc made by a point and a heading creates an intersecting line with a zone.
       @param positionLLA Point
       @param double heading in radians
       @returns true if the arc made projects into the zone.
    */
    virtual bool isProjectedInsideZone (const positionRecord &position, const double heading_rads) const;

    /**
       @brief Clear the list of all zone coordinates.
    */
    inline void clearPointList()
    {
      m_CoordLon.clear();
      m_CoordLat.clear();
    }

    /**
       @brief Append a new point onto the list of zone coordinates.
       @param newPoint New point (Lat/Lon/Alt) to append onto list.
    */
    inline void pushPointInDeg( const vm::Vector3<double> &newPoint )
    {
      m_CoordLon.push_back( newPoint.getLon() * vm::DEGREES_TO_RADIANS );
      m_CoordLat.push_back( newPoint.getLat() * vm::DEGREES_TO_RADIANS );
    }

    /**
       @brief Remove last point appended to list of zone coordinates.
    */
    inline void popPoint()
    {
      m_CoordLon.pop_back();
      m_CoordLat.pop_back();
    }

    inline void setCeiling( const double &ceiling )
    {
      m_Ceiling = ceiling;
      m_CeilingSet = true;
    }

    inline const double &getCeiling() const
    {
      return m_Ceiling;
    }

    inline void setFloor( const double &floor )
    {
      m_Floor = floor;
      m_FloorSet = true;
    }

    inline const double &getFloor() const
    {
      return m_Floor;
    }

  protected:
    /**
       @brief Determine if point is inside circular zone.
       @param positionLLA Point in Lat/Lon/Alt (deg/deg/m)
       @returns true if point is inside zone, false otherwise.
    */
    bool isInsideZoneCircular(
			      const vm::Vector3<double> &positionLLA
			      ) const;

    /**
       @brief Determine if point is inside polygonal zone.
       @param positionLLA Point in Lat/Lon/Alt (deg/deg/m)
       @returns true if point is inside zone, false otherwise.
    */
    bool isInsideZonePolygon(
			     const vm::Vector3<double> &positionLLA
			     ) const;

    bool isProjectedInsideCircularZone(
				       const vm::Vector3<double> &position1LLA,
				       const vm::Vector3<double> &position2LLA
				       ) const;

    bool isProjectedInsidePolygonZone(
				      const vm::Vector3<double> &position1LLA,
				      const vm::Vector3<double> &position2LLA
				      ) const;


    /// Name of zone
    std::string m_Name;

    /// Type of zone.
    AREATYPE m_AreaType;

    /// Is circular zone domed or cylindrical.
    bool m_Dome;
    /// Radius of circular zone in meters.
    double m_Range;

    /// Zone center in Lat/Lon/Alt
    positionRecord m_CenterLLA;

    //int m_Center;

    /// Flag indicating floor altitude has been set.
    bool m_FloorSet;
    /// Value of set floor altitude in meters.
    double m_Floor;

    /// Flag indicating ceiling altitude has been set.
    bool m_CeilingSet;
    /// Value of set ceiling altitude in meters.
    double m_Ceiling;

    /// Vector of Lat/Lon/Alt vectors for polygonal zone points.
    std::vector< double > m_CoordLon;
    std::vector< double > m_CoordLat;

  private:

  };
}

#endif
