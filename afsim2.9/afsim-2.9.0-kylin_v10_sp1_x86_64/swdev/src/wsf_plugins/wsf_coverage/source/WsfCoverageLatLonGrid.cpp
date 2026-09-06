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

#include "WsfCoverageLatLonGrid.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#include "UtEllipsoidalCentralBody.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "ext/WsfExtInterface.hpp"

namespace wsf
{
namespace coverage
{

bool LatLonGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "latitude_span")
   {
      aInput.ReadValueOfType(mLatMinimumDeg, UtInput::cLATITUDE);
      aInput.ValueGreater(mLatMinimumDeg, -90.0);
      aInput.ValueLess(mLatMinimumDeg, 90.0);
      aInput.ReadValueOfType(mLatMaximumDeg, UtInput::cLATITUDE);
      aInput.ValueGreater(mLonMinimumDeg, -90.0);
      aInput.ValueLess(mLonMinimumDeg, 90.0);
      if (mLatMaximumDeg < mLatMinimumDeg)
      {
         std::swap(mLatMaximumDeg, mLatMinimumDeg);
      }
   }
   else if (command == "longitude_span")
   {
      aInput.ReadValueOfType(mLonMinimumDeg, UtInput::cLONGITUDE);
      aInput.ReadValueOfType(mLonMaximumDeg, UtInput::cLONGITUDE);
      if (mLonMaximumDeg < mLonMinimumDeg)
      {
         mLonMaximumDeg = UtMath::NormalizeAngle0_360(mLonMaximumDeg);
      }
   }
   else if (command == "latitude_spacing")
   {
      aInput.ReadValueOfType(mLatSpacingDeg, UtInput::cANGLE);
      mLatSpacingDeg *= UtMath::cDEG_PER_RAD;
      aInput.ValueGreater(mLatSpacingDeg, 0.0);
   }
   else if (command == "longitude_spacing")
   {
      aInput.ReadValueOfType(mLonSpacingDeg, UtInput::cANGLE);
      mLonSpacingDeg *= UtMath::cDEG_PER_RAD;
      aInput.ValueGreater(mLonSpacingDeg, 0.0);
   }
   else if (command == "spacing")
   {
      double spacing;
      aInput.ReadValueOfType(spacing, UtInput::cANGLE);
      aInput.ValueGreater(spacing, 0.0);
      spacing *= UtMath::cDEG_PER_RAD;
      if (mLatSpacingDeg < 0.0)
      {
         mLatSpacingDeg = spacing;
      }
      if (mLonSpacingDeg < 0.0)
      {
         mLonSpacingDeg = spacing;
      }
   }
   else
   {
      myCommand = RectangularGrid::ProcessInput(aInput);
   }
   return myCommand;
}

bool LatLonGrid::InitializeP()
{
   if (mLatSpacingDeg <= 0.0)
   {
      auto err = ut::log::error() << "Latitude spacing undefined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (mLonSpacingDeg <= 0.0)
   {
      auto err = ut::log::error() << "Longitude spacing undefined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }

   // If the origin was not set, use the lower left corner.
   if (!mOriginSet)
   {
      mLatOriginDeg = mLatMinimumDeg;
      mLonOriginDeg = mLonMinimumDeg;
   }
   if (mLatOriginDeg < mLatMinimumDeg || mLatOriginDeg > mLatMaximumDeg)
   {
      auto err = ut::log::error() << "Latitude origin outside grid.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Lat. Origin: " << mLatOriginDeg;
      err.AddNote() << "Lat. Minimum: " << mLatMinimumDeg;
      err.AddNote() << "Lat. Maximum: " << mLatMaximumDeg;
      return false;
   }

   if (mLonOriginDeg < mLonMinimumDeg || mLonOriginDeg > mLonMaximumDeg)
   {
      double origin = UtMath::NormalizeAngle0_360(mLonOriginDeg);
      if (origin < mLonMinimumDeg || origin > mLonMaximumDeg)
      {
         auto err = ut::log::error() << "Longitude origin outside grid.";
         err.AddNote() << "Grid: " << GetName();
         err.AddNote() << "Lon. Origin: " << mLonOriginDeg;
         err.AddNote() << "Lon. Minimum: " << mLonMinimumDeg;
         err.AddNote() << "Lon. Maximum: " << mLonMaximumDeg;
         return false;
      }
      else
      {
         mLonOriginDeg = origin;
      }
   }

   return CreateRectangularGrid();
}

void LatLonGrid::SetOrigin(double aLat, double aLon)
{
   mOriginSet    = true;
   mLatOriginDeg = aLat;
   mLonOriginDeg = aLon;
}

bool LatLonGrid::RemovePoint(PointId aPointId)
{
   auto pointCount = mPoints.erase(aPointId);
   auto assetCount = mAssets.erase(GetPlatformName(aPointId));
   // NOTE: We don't remove from mIndices so that we can always recover what the
   //  full grid might have had for its indices.
   return pointCount == 1 && assetCount == 1;
}

double LatLonGrid::TakeLatitudeStep(double aLatitude, bool aForward) const
{
   double retval{aLatitude};

   // Take step
   if (aForward)
   {
      retval += mLatSpacingDeg;
   }
   else
   {
      retval -= mLatSpacingDeg;
   }

   return retval;
}

double LatLonGrid::TakeLongitudeStep(double aLongitude, double aLatitude, bool aForward) const
{
   double retval{aLongitude};

   // Take step
   if (aForward)
   {
      retval += mLonSpacingDeg;
   }
   else
   {
      retval -= mLonSpacingDeg;
   }

   return retval;
}

std::vector<double> LatLonGrid::GetLatitudeLoopValues() const
{
   std::vector<double> latLoop{};

   double lat = mLatOriginDeg;
   while (TakeLatitudeStep(lat, false) >= mLatMinimumDeg)
   {
      lat = TakeLatitudeStep(lat, false);
   }
   while (lat <= mLatMaximumDeg)
   {
      latLoop.push_back(lat);
      lat = TakeLatitudeStep(lat, true);
   }

   return latLoop;
}

std::vector<double> LatLonGrid::GetLongitudeLoopValues(double aLatitudeDeg) const
{
   double lon = mLonOriginDeg;
   while (TakeLongitudeStep(lon, aLatitudeDeg, false) >= mLonMinimumDeg)
   {
      lon = TakeLongitudeStep(lon, aLatitudeDeg, false);
   }
   std::vector<double> lonLoop{};
   while (lon <= mLonMaximumDeg)
   {
      lonLoop.push_back(lon);
      lon = TakeLongitudeStep(lon, aLatitudeDeg, true);
   }
   return lonLoop;
}

} // namespace coverage
} // namespace wsf
