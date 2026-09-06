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

#include "WsfCoverageDistanceSteppedGrid.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

#include "UtEllipsoidalCentralBody.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatformTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "ext/WsfExtInterface.hpp"

namespace wsf
{
namespace coverage
{

bool DistanceSteppedGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "step_distance")
   {
      aInput.ReadValueOfType(mLatStepDistance, UtInput::cLENGTH);
      aInput.ValueGreater(mLatStepDistance, 0.0);
      aInput.ReadValueOfType(mLonStepDistance, UtInput::cLENGTH);
      aInput.ValueGreater(mLonStepDistance, 0.0);
   }
   else if (command == "size")
   {
      aInput.ReadValue(mNumLatSteps);
      aInput.ReadValue(mNumLonSteps);
      aInput.ValueGreater(mNumLatSteps, 0);
      aInput.ValueGreater(mNumLonSteps, 0);
   }
   else if (command == "origin_index")
   {
      aInput.ReadValue(mLatOriginIndex);
      aInput.ReadValue(mLonOriginIndex);
   }
   else
   {
      myCommand = RectangularGrid::ProcessInput(aInput);
   }
   return myCommand;
}

bool DistanceSteppedGrid::InitializeP()
{
   if (mLatStepDistance <= 0.0)
   {
      auto err = ut::log::error() << "Latitude step distance must be defined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (mLonStepDistance <= 0.0)
   {
      auto err = ut::log::error() << "Longitude step distance must be defined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (mNumLatSteps == 0)
   {
      auto err = ut::log::error() << "Latitude step count must be defined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (mNumLonSteps == 0)
   {
      auto err = ut::log::error() << "Longitude step count must be defined.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (mLatOriginIndex < 0 || mLatOriginIndex >= mNumLatSteps)
   {
      auto err = ut::log::error() << "Latitude origin index out of bounds.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Allowed range: [0, " << mNumLatSteps << ")";
      err.AddNote() << "Provided value: " << mLatOriginIndex;
      return false;
   }
   if (mLonOriginIndex < 0 || mLonOriginIndex >= mNumLonSteps)
   {
      auto err = ut::log::error() << "Longitude origin index out of bounds.";
      err.AddNote() << "Grid: " << GetName();
      err.AddNote() << "Allowed range: [0, " << mNumLonSteps << ")";
      err.AddNote() << "Provided value: " << mLonOriginIndex;
      return false;
   }
   if (mPlatformType.empty() || mDeviceName.empty())
   {
      auto err = ut::log::error() << "Grid asset not specified.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }
   if (!mOriginSet)
   {
      auto err = ut::log::error() << "Origin of grid not specified.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }

   return CreateRectangularGrid();
}

double DistanceSteppedGrid::TakeLatitudeStep(double aLatitude, bool aForward) const
{
   double retval{aLatitude};

   // Get the step size in angle
   double mpd     = ut::EllipsoidalCentralBody::MetersPerDegreeLat(aLatitude, GetCentralBody().GetEllipsoid());
   double sma     = GetCentralBody().GetEllipsoid().GetSemiMajorAxis();
   double scaling = (sma + mAltitude) / sma;
   double delta   = mLatStepDistance / (scaling * mpd);

   if (aForward)
   {
      retval += delta;
   }
   else
   {
      retval -= delta;
   }

   // Avoid moving past the poles.
   if (retval <= -90.0 || retval >= 90.0)
   {
      retval = aLatitude;
   }

   return retval;
}

double DistanceSteppedGrid::TakeLongitudeStep(double aLongitude, double aLatitude, bool aForward) const
{
   double retval{aLongitude};

   // Get the step size in angle
   double mpd     = ut::EllipsoidalCentralBody::MetersPerDegreeLon(aLatitude, GetCentralBody().GetEllipsoid());
   double sma     = GetCentralBody().GetEllipsoid().GetSemiMajorAxis();
   double scaling = (sma + mAltitude) / sma;
   double delta   = mLonStepDistance / (scaling * mpd);

   if (aForward)
   {
      retval += delta;
   }
   else
   {
      retval -= delta;
   }

   return retval;
}

std::vector<double> DistanceSteppedGrid::GetLatitudeLoopValues() const
{
   std::vector<double> latLoop{};

   double lat = mLatOriginDeg;
   latLoop.push_back(mLatOriginDeg);
   for (int idx = mLatOriginIndex; idx > 0; --idx)
   {
      double next = TakeLatitudeStep(lat, false);
      if (next != lat)
      {
         latLoop.push_back(next);
      }
      lat = next;
   }
   lat = mLatOriginDeg;
   for (int idx = mLatOriginIndex; idx < mNumLatSteps - 1; ++idx)
   {
      double next = TakeLatitudeStep(lat, true);
      if (next != lat)
      {
         latLoop.push_back(next);
      }
      lat = next;
   }
   std::sort(latLoop.begin(), latLoop.end());

   return latLoop;
}

std::vector<double> DistanceSteppedGrid::GetLongitudeLoopValues(double aLatitudeDeg) const
{
   std::vector<double> lonLoop{};

   double lon = mLonOriginDeg;
   lonLoop.push_back(lon);
   for (int idx = mLonOriginIndex; idx > 0; --idx)
   {
      lon = TakeLongitudeStep(lon, aLatitudeDeg, false);
      lonLoop.push_back(lon);
   }
   lon = mLonOriginDeg;
   for (int idx = mLonOriginIndex; idx < mNumLonSteps - 1; ++idx)
   {
      lon = TakeLongitudeStep(lon, aLatitudeDeg, true);
      lonLoop.push_back(lon);
   }
   std::sort(lonLoop.begin(), lonLoop.end());

   return lonLoop;
}

} // namespace coverage
} // namespace wsf
