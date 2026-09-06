// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: zoneRecordInterface.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/

#ifndef ZONE_RECORD_INTERFACE_HPP
#define ZONE_RECORD_INTERFACE_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <iadsLib/enumString.hpp>
#include <iadsLib/enum.hpp>
#include <iadsLib/positionRecord.hpp>
#include "logger.hpp"

namespace il
{
   IADSLIB_EXPORT std::string zoneTypeToName (const eZoneType type_enum, std::weak_ptr<logger::GlobalLogger> aGlobalLogger);

   class IADSLIB_EXPORT zoneRecordInterface
   {
   public:
      zoneRecordInterface (const eZoneType type_enum, const std::string &type_str, const tZonePriority &priority, std::weak_ptr<logger::GlobalLogger> aGlobalLogger)
         : m_type (type_enum, type_str)
         , m_priority (priority)
         , mGlobalLogger(aGlobalLogger)
      { }

      virtual ~zoneRecordInterface() = default;

      inline virtual void setType (const eZoneType type_enum, const std::string &type_str) { m_type = EnumString<eZoneType>(type_enum, type_str); }
      inline virtual eZoneType getTypeEnum () const { return m_type.AsEnum(); }
      inline virtual std::string getTypeEnumString () const { return m_type.AsString(); }

      inline virtual void setPriority (const tZonePriority priority) { m_priority = priority; }
      inline virtual tZonePriority getPriority () const { return m_priority; }

      // abstract interface
      virtual std::string getName () const = 0;
      virtual positionRecord getCenterPoint () const = 0;
      virtual double getDistanceToZone (const positionRecord &position) const = 0;
      virtual double getDistanceToZoneCenter(const positionRecord &position) const = 0;
      virtual bool getClosestPointInZone(const il::positionRecord &position, il::positionRecord &zonePoint) const = 0;

      virtual bool isInsideZone (const positionRecord &position) const = 0;
      // only tests within the extents of the segment
      virtual bool isProjectedInsideZone (const positionRecord &position1, const positionRecord &position2) const = 0;
      // projects the arc through the zone, then tests
      virtual bool isProjectedInsideZone (const positionRecord &position, const double heading_rads) const = 0;

   protected:
      EnumString<eZoneType> m_type;
      tZonePriority         m_priority;
      std::weak_ptr<logger::GlobalLogger> mGlobalLogger;
   };

   class zoneContainer
   {
   public:
      typedef std::string tZoneName;

      zoneContainer (bool overwrite_existing = true);
      bool Add (const std::shared_ptr<zoneRecordInterface> &zone);

      std::vector<std::shared_ptr<zoneRecordInterface>> GetZones() const;
      std::vector<std::shared_ptr<zoneRecordInterface>> GetZones(const eZoneType &type) const;
      std::vector<std::shared_ptr<zoneRecordInterface>> GetZones(const unsigned long &e_zone_type_mask) const;

      std::size_t GetZoneCount () const; // all zones attached to the platform regardless of type
   protected:
      bool m_overwrite_existing;
      std::map<tZoneName, std::shared_ptr<zoneRecordInterface>> m_zones;
   };

}

#endif

