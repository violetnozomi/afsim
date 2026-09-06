// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCoverageZoneBasedGrid.hpp"

#include <stdexcept>
#include <vector>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfZone.hpp"
#include "WsfZoneTypes.hpp"

namespace wsf
{
namespace coverage
{

bool ZoneBasedGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   // latitude_span and longitude_span, are defined on LatLonGrid, but
   // are not needed for ZoneBasedGrid. Intercept them.
   if (command == "latitude_span")
   {
      myCommand = false;
   }
   else if (command == "longitude_span")
   {
      myCommand = false;
   }
   else if (command == "zone")
   {
      aInput.ReadCommand(mZoneName);
   }
   else
   {
      myCommand = LatLonGrid::ProcessInput(aInput);
   }
   return myCommand;
}

void ZoneBasedGrid::PreparePlatforms(WsfSimulation& aSimulation)
{
   // Assure that the zone still exists.
   auto zonePtr = aSimulation.GetScenario().GetZoneTypes().Find(mZoneName);
   if (!zonePtr)
   {
      auto err = ut::log::error() << "Unable to find zone for coverage grid platform creation.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Zone: " << mZoneName;
      throw std::runtime_error{"Error during platform creation for WSF_ZONE_BASED_GRID"};
   }

   // Collect the ids of points that are not inside the zone.
   const ut::CentralBody& centralBody = GetCentralBody();
   auto                   actionPtr   = CreateGridAction(
      [&aSimulation, zonePtr, &centralBody, this](const Grid& aGrid, const Point& aPoint)
      {
         // get point location in WCS
         UtVec3d posWCS{};
         centralBody.ConvertLLA_ToWCS(aPoint.GetLatitude(), aPoint.GetLongitude(), aPoint.GetAltitude(), posWCS);
         if (!zonePtr->PointIsInside(&aSimulation, posWCS.GetData(), posWCS.GetData(), 0.0))
         {
            mCulledPoints.push_back(aPoint);
         }
      });
   Invoke(*actionPtr);

   // Remove those points from the grid
   for (auto point : mCulledPoints)
   {
      if (!RemovePoint(point.GetID()))
      {
         auto err = ut::log::error() << "Error culling points outside zone.";
         err.AddNote() << "Grid:  " << GetName();
         err.AddNote() << "Zone:  " << mZoneName;
         err.AddNote() << "Point: " << point.GetID();
         throw std::runtime_error{"Error during point culling for WSF_ZONE_BASED_GRID"};
      }
   }

   // Check that there are points left after culling.
   if (!GetSize())
   {
      auto err = ut::log::error() << "Grid has no points inside zone.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Zone: " << mZoneName;
      err.AddNote() << "Check grid altitude is inside zone's min and max altitude.";
      throw std::runtime_error{"Error during point culling for WSF_ZONE_BASED_GRID"};
   }

   // This call then handles the construction of the platforms that will be
   // at the grid locations.
   LatLonGrid::PreparePlatforms(aSimulation);
}

void ZoneBasedGrid::InvokeOnComponent(GridAction& aAction, std::size_t aComponentIndex) const
{
   LatLonGrid::InvokeOnComponent(aAction, aComponentIndex);
   for (auto& point : mCulledPoints)
   {
      aAction.Call(*this, point);
   }
}

bool ZoneBasedGrid::InitializeP()
{
   if (mZoneName.empty())
   {
      auto err = ut::log::error() << "No zone specified for WSF_ZONE_BASED_GRID";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }

   auto& zoneTypes = GetScenario().GetZoneTypes();
   auto  zonePtr   = zoneTypes.Find(mZoneName);
   if (!zonePtr)
   {
      auto err = ut::log::error() << "Unable to find zone during coverage grid initialization.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Zone: " << mZoneName;
      return false;
   }

   // Set the lat/lon extrema
   double latMin;
   double latMax;
   double lonMin;
   double lonMax;
   zonePtr->GetLatLonExtrema(latMin, lonMin, latMax, lonMax);
   SetMinimumLatitudeDeg(latMin);
   SetMaximumLatitudeDeg(latMax);
   SetMinimumLongitudeDeg(lonMin);
   SetMaximumLongitudeDeg(lonMax);

   // Set the origin of the grid to be the centroid of the zone, if not already set.
   if (!IsOriginSet())
   {
      auto centroid = zonePtr->Centroid();
      SetOrigin(centroid.GetLat(), centroid.GetLon());
   }

   // After this call, the set of points on this grid will fill the entire bounding
   // rectangle in latitude and longitude. There is no way to filter based on the
   // zone until the simulation is available. See PreparePlatforms, above.
   return LatLonGrid::InitializeP();
}

} // namespace coverage
} // namespace wsf
