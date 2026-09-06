// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_ZONE_RECORD_HPP
#define WSF_BM_ZONE_RECORD_HPP

#include <WsfBMCommon.hpp>
#include <string>
#include <WsfZoneDefinition.hpp>
#include <WsfPlatform.hpp>
#include <iadsLib/zoneRecordInterface.hpp>
#include "WsfBMGenericTypeWrapper.hpp"

class WsfSimulation;

class WSF_IADS_C2_LIB_EXPORT WsfBMZoneRecord : public WsfBMUnmanagedTypeWrapper<WsfZoneDefinition>, public il::zoneRecordInterface
{
   public:

      // WsfBMZoneRecord (const il::eZoneType type_enum, const std::string &type_str, const il::tZonePriority &priority);
      WsfBMZoneRecord(const il::eZoneType type_enum, const std::string& type_str, const il::tZonePriority& priority, WsfZoneDefinition* zone, WsfSimulation* sim_ptr);
      // abstract implementations
      virtual std::string getName() const;
      virtual il::positionRecord getCenterPoint() const;
      virtual double getDistanceToZone(const il::positionRecord& position) const;
      virtual double getDistanceToZoneCenter(const il::positionRecord& position) const;
      virtual bool getClosestPointInZone(const il::positionRecord& position, il::positionRecord& zonePoint) const;

      virtual bool isInsideZone(const il::positionRecord& position) const;
      virtual bool isProjectedInsideZone(const il::positionRecord& position1, const il::positionRecord& position2) const;
      virtual bool isProjectedInsideZone(const il::positionRecord& position, const double heading_rads) const;

   protected:
      WsfSimulation* m_sim_ptr;
};

#endif
