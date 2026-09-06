// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include <cfloat>
#include <iostream>

#include <WsfSimulation.hpp>

#include "WsfBMZoneRecord.hpp"
#include <WsfGeoPoint.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"
#include "ZoneUtils.hpp"

WsfBMZoneRecord::WsfBMZoneRecord(const il::eZoneType type_enum, const std::string& type_str, const il::tZonePriority& priority, WsfZoneDefinition* zone, WsfSimulation* sim_ptr)
   : WsfBMUnmanagedTypeWrapper("WsfBMZoneRecord", zone)
   , il::zoneRecordInterface(type_enum, type_str, priority, IADSC2ScenarioExtension::GetGlobalLogger(sim_ptr->GetScenario()))
   , m_sim_ptr(sim_ptr)
{
}

// abstract interface
std::string WsfBMZoneRecord::getName() const
{
   std::string name = "** ERROR **";
   if (IsValidRef())
   {
      name = m_wrapped_ptr->GetName();
   }
   return name;
}

il::positionRecord WsfBMZoneRecord::getCenterPoint() const
{
   il::positionRecord position;
   if (IsValidRef())
   {
      auto geopoint = ZoneUtils::GetZoneCentroid(m_sim_ptr, *m_wrapped_ptr);
      position.setLLADegrees(geopoint.GetLat(), geopoint.GetLon(), geopoint.GetAlt());
   }
   return position;
}

double WsfBMZoneRecord::getDistanceToZone(const il::positionRecord& position) const
{
   double distance;

   double wcsPoint[] = { position.getX(), position.getY(), position.getZ() };
   if (!ZoneUtils::GetDistanceFromPointToZone(m_sim_ptr, wcsPoint, *m_wrapped_ptr, distance))
   {
      distance = DBL_MAX;
   }
   return distance;
}

double WsfBMZoneRecord::getDistanceToZoneCenter(const il::positionRecord& position) const
{
   double wcsPoint[] = { position.getX(), position.getY(), position.getZ() };
   return ZoneUtils::GetDistanceFromPointToZoneCenter(m_sim_ptr, wcsPoint, *m_wrapped_ptr);
}

bool WsfBMZoneRecord::getClosestPointInZone(const il::positionRecord& position, il::positionRecord& zonePoint) const
{
   double wcsZonePoint[] = { 0, 0, 0 };
   double wcsPoint[] = { position.getX(), position.getY(), position.getZ() };
   if (ZoneUtils::GetClosestPointInZone(m_sim_ptr, wcsPoint, wcsZonePoint, *m_wrapped_ptr))
   {
      zonePoint.setXYZ(wcsZonePoint[0], wcsZonePoint[1], wcsZonePoint[2]);
      return true;
   }
   return false;
}

bool WsfBMZoneRecord::isInsideZone(const il::positionRecord& position) const
{
   double point_wcs[3] = { position.getX(), position.getY(), position.getZ() };
   return m_wrapped_ptr->PointIsInside(m_sim_ptr, point_wcs, point_wcs, 0, 0);
}

bool WsfBMZoneRecord::isProjectedInsideZone(const il::positionRecord& position1, const il::positionRecord& position2) const
{
   double point1_wcs[3] = { position1.getX(), position1.getY(), position1.getZ() },
                          point2_wcs[3] = { position2.getX(), position2.getY(), position2.getZ() };
   return ZoneUtils::isProjectedInsideZone2Points(m_sim_ptr, *m_wrapped_ptr, point1_wcs, point2_wcs);
}

bool WsfBMZoneRecord::isProjectedInsideZone(const il::positionRecord& position, const double heading_rads) const
{
   double point_wcs[3] = { position.getX(), position.getY(), position.getZ() };
   return ZoneUtils::isProjectedInsideZonePointHeading(m_sim_ptr, *m_wrapped_ptr, point_wcs, heading_rads);
}
