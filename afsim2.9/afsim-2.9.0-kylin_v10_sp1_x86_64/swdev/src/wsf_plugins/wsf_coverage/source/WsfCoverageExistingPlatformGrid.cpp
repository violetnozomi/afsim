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

#include "WsfCoverageExistingPlatformGrid.hpp"

#include <stdexcept>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfCoverageTextOutputWriter.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

bool ExistingPlatformGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "platform")
   {
      std::string name;
      aInput.ReadCommand(name);
      std::string device;
      aInput.ReadCommand(device);
      mInputAssets[name] = device;
   }
   else
   {
      myCommand = Grid::ProcessInput(aInput);
   }
   return myCommand;
}

void ExistingPlatformGrid::PreparePlatforms(WsfSimulation& aSimulation)
{
   for (auto& iter : mPlatforms)
   {
      const WsfPlatform* platPtr = aSimulation.GetPlatformByName(iter.first);
      if (platPtr)
      {
         double lat;
         double lon;
         double alt;
         platPtr->GetLocationLLA(lat, lon, alt);
         mPoints.emplace(iter.second, Point{iter.second, lat, lon, alt});
      }
      else
      {
         // This would occur if a platform was defined in the input, but did not get
         // instantiated in the simulation. This is not supported by this grid.
         auto msg = ut::log::error() << "Coverage grid initialization error.";
         msg.AddNote() << "No such platform: " << iter.first;
         msg.AddNote() << "In grid: " << GetName();
         throw std::runtime_error{"Initialization error."};
      }
   }
}

const Point* ExistingPlatformGrid::GetPoint(PointId aPointId) const
{
   const Point* retvalPtr{nullptr};
   auto         iter = mPoints.find(aPointId);
   if (iter != mPoints.end())
   {
      retvalPtr = &(iter->second);
   }
   return retvalPtr;
}

void ExistingPlatformGrid::Invoke(GridAction& aAction) const
{
   for (auto& iter : mPoints)
   {
      aAction.Call(*this, iter.second);
   }
}

GridAsset ExistingPlatformGrid::GetAsset(PointId aPointId) const
{
   GridAsset retval{"", cINVALID_POINT_ID, ""};
   auto      iter = mAssets.find(aPointId);
   if (iter != mAssets.end())
   {
      retval = iter->second;
   }
   return retval;
}

GridAsset ExistingPlatformGrid::GetAsset(const std::string& aPlatformName) const
{
   GridAsset retval{"", cINVALID_POINT_ID, ""};
   auto      iter = mPlatforms.find(aPlatformName);
   if (iter != mPlatforms.end())
   {
      auto gaIter = mAssets.find(iter->second);
      if (gaIter != mAssets.end())
      {
         retval = gaIter->second;
      }
      else
      {
         throw std::runtime_error{"WSF_EXISTING_PLATFORM_GRID has indexing problem. PointId does not map into asset."};
      }
   }
   return retval;
}

void ExistingPlatformGrid::HasNullDevice(bool& aAnyNull, bool& aAllNull) const
{
   aAnyNull = false;
   aAllNull = true;
   for (auto& iter : mInputAssets)
   {
      if (iter.second == cNULL_COVERAGE_DEVICE_NAME)
      {
         aAnyNull = true;
      }
      else
      {
         aAllNull = false;
      }
   }
}

bool ExistingPlatformGrid::InitializeP()
{
   bool ok{true};

   for (auto& inputIter : mInputAssets)
   {
      if (GetScenario().GetInputPlatformByName(inputIter.first))
      {
         auto id                     = GetNextAvailableId();
         mPlatforms[inputIter.first] = id;
         mAssets.emplace(id, GridAsset{inputIter.first, id, inputIter.second});
      }
      else
      {
         auto msg = ut::log::error() << "Coverage grid initialization error.";
         msg.AddNote() << "No such platform: " << inputIter.first;
         msg.AddNote() << "In grid: " << GetName();
         ok = false;
         break;
      }
   }

   mInputAssets.clear();

   return ok;
}

void ExistingPlatformGrid::WriteHeaderExtension(TextOutputWriter& aWriter) const
{
   aWriter.WriteField("Platform");
}

void ExistingPlatformGrid::WriteLineExtension(TextOutputWriter& aWriter, PointId aPointId) const
{
   auto iter = mAssets.find(aPointId);
   if (iter != mAssets.end())
   {
      aWriter.WriteField(iter->second.GetPlatformName());
   }
   else
   {
      aWriter.WriteField("UNKNOWN");
   }
}

} // namespace coverage
} // namespace wsf
