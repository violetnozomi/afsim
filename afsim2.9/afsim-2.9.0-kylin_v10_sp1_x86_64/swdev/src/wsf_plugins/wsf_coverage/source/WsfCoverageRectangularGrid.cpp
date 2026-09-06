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

#include "WsfCoverageRectangularGrid.hpp"

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

RectangularGrid::RectangularGrid(const RectangularGrid& aOther)
   : Grid(aOther)
   , mPlatformType{aOther.mPlatformType}
   , mDeviceName{aOther.mDeviceName}
   , mAltitude{aOther.mAltitude}
   , mLatOriginDeg{aOther.mLatOriginDeg}
   , mLonOriginDeg{aOther.mLonOriginDeg}
   , mPoints{aOther.mPoints}
   , mAssets{aOther.mAssets}
   , mOriginSet{aOther.mOriginSet}
   , mAltitudeAGL{aOther.mAltitudeAGL}
{
   if (aOther.IsInitialized())
   {
      Initialize();
   }
}

bool RectangularGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "altitude")
   {
      aInput.ReadValueOfType(mAltitude, UtInput::cLENGTH);
      std::string aglmsl;
      aInput.ReadCommand(aglmsl);
      if (aglmsl == "agl")
      {
         mAltitudeAGL = true;
      }
      else if (aglmsl == "msl")
      {
         mAltitudeAGL = false;
      }
      else
      {
         throw UtInput::BadValue{aInput, "Unknown altitude specifier"};
      }
   }
   else if (command == "origin")
   {
      aInput.ReadValueOfType(mLatOriginDeg, UtInput::cLATITUDE);
      aInput.ValueGreater(mLatOriginDeg, -90.0);
      aInput.ValueLess(mLatOriginDeg, 90.0);
      aInput.ReadValueOfType(mLonOriginDeg, UtInput::cLONGITUDE);
      mOriginSet = true;
   }
   else if (command == "asset")
   {
      aInput.ReadCommand(mPlatformType);
      aInput.ReadCommand(mDeviceName);
   }
   else
   {
      myCommand = Grid::ProcessInput(aInput);
   }
   return myCommand;
}

void RectangularGrid::PreparePlatforms(WsfSimulation& aSimulation)
{
   WsfPlatform* typePtr = aSimulation.GetScenario().GetPlatformTypes().Find(mPlatformType);
   if (!typePtr)
   {
      auto err = ut::log::error() << "Unknown platform type in grid asset specification.";
      err.AddNote() << "Type: " << mPlatformType;
      err.AddNote() << "Grid: " << GetName();
      throw std::runtime_error("Initialization error.");
   }

   if (GetSuppressGridPlatforms())
   {
      auto extInterfacePtr = WsfExtInterface::Find(aSimulation);
      if (extInterfacePtr)
      {
         extInterfacePtr->AddPrivateType(mPlatformType);
      }
   }

   // Action to create the platforms
   std::string side = "coverage_grid_" + GetName();
   for (auto& iter : mPoints)
   {
      // clone the template
      auto gridPlatPtr = ut::clone(typePtr);

      // set the name
      gridPlatPtr->SetName(GetPlatformName(iter.first));

      // set the initial position of the platform
      gridPlatPtr->SetCentralPoint(GetCentralBody());
      gridPlatPtr->SetLocationLLA(iter.second.GetLatitude(), iter.second.GetLongitude(), iter.second.GetAltitude());

      // set the side
      gridPlatPtr->SetSide(side);

      // add it to the simulation
      aSimulation.AddPlatform(gridPlatPtr.release());
   }
}

const Point* RectangularGrid::GetPoint(PointId aPointId) const
{
   const Point* retvalPtr{nullptr};
   auto         iter = mPoints.find(aPointId);
   if (iter != mPoints.end())
   {
      retvalPtr = &(iter->second);
   }
   return retvalPtr;
}

void RectangularGrid::Invoke(GridAction& aAction) const
{
   for (auto& iter : mPoints)
   {
      aAction.Call(*this, iter.second);
   }
}

GridAsset RectangularGrid::GetAsset(PointId aPointId) const
{
   return GetAsset(GetPlatformName(aPointId));
}

GridAsset RectangularGrid::GetAsset(const std::string& aPlatformName) const
{
   GridAsset retval{"", cINVALID_POINT_ID, ""};
   auto      iter = mAssets.find(aPlatformName);
   if (iter != mAssets.end())
   {
      retval = iter->second;
   }
   return retval;
}

void RectangularGrid::HasNullDevice(bool& aAnyNull, bool& aAllNull) const
{
   aAnyNull = mDeviceName == cNULL_COVERAGE_DEVICE_NAME;
   aAllNull = aAnyNull;
}

std::pair<std::size_t, std::size_t> RectangularGrid::GetComponentSizes(std::size_t aComponentIndex) const
{
   return std::make_pair(mLatSize, mLonSize);
}

std::pair<std::size_t, std::size_t> RectangularGrid::GetPointIndices(PointId aPointId) const
{
   std::pair<std::size_t, std::size_t> retval{mLatSize, mLonSize};
   auto                                iter = mIndices.find(aPointId);
   if (iter != mIndices.end())
   {
      retval = iter->second;
   }
   return retval;
}

bool RectangularGrid::CreateRectangularGrid()
{
   wsf::Terrain        terrain{wsf::TerrainInterface::Get(GetScenario())};
   std::vector<double> latLoop = GetLatitudeLoopValues();
   mLatSize                    = latLoop.size();
   std::size_t latIndex{0};
   for (double lat : latLoop)
   {
      std::vector<double> lonLoop = GetLongitudeLoopValues(lat);
      if (!mLonSize)
      {
         mLonSize = lonLoop.size();
      }
      else if (mLonSize != lonLoop.size())
      {
         auto msg = ut::log::error() << "Inconsistent number of longitude points in structured grid.";
         msg.AddNote() << "Grid: " << GetName();
         msg.AddNote() << "Type: " << GetType();
         return false;
      }

      std::size_t lonIndex{0};
      for (double lon : lonLoop)
      {
         auto  id = GetNextAvailableId();
         float terrainHeight{0.0};
         if (mAltitudeAGL)
         {
            terrain.GetElevInterp(lat, lon, terrainHeight);
         }
         mPoints.emplace(std::make_pair(id, Point{id, lat, lon, terrainHeight + mAltitude}));
         mIndices.emplace(std::make_pair(id, std::make_pair(latIndex, lonIndex)));
         std::string platName = GetPlatformName(id);
         mAssets.emplace(platName, GridAsset{platName, id, mDeviceName});

         ++lonIndex;
      }

      ++latIndex;
   }

   return true;
}

std::string RectangularGrid::GetPlatformName(PointId aPointId) const
{
   return GetName() + "_" + std::to_string(aPointId);
}

} // namespace coverage
} // namespace wsf
