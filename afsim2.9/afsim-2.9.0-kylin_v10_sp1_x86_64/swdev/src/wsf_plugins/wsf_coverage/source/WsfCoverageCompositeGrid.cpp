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

#include "WsfCoverageCompositeGrid.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfCoverageGridTypes.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

bool CompositeGrid::ProcessInput(UtInput& aInput)
{
   bool        myCommand{true};
   std::string command;
   aInput.GetCommand(command);
   if (command == "subgrid")
   {
      UtInputBlock block{aInput};
      std::string  gridName;
      if (!block.ReadCommand(gridName))
      {
         throw UtInput::BadValue{aInput, "Expected coverage sub-grid name."};
      }
      std::string fullName = GetName() + "_" + gridName;
      for (auto& grid : mSubGrids)
      {
         if (grid->GetName() == fullName)
         {
            std::stringstream msg{};
            msg << "Sub-grid with name '" << gridName << "' already defined.";
            throw UtInput::BadValue{aInput, msg.str()};
         }
      }
      std::string gridType;
      if (!block.ReadCommand(gridType))
      {
         throw UtInput::BadValue{aInput, "Expected coverage grid type."};
      }
      auto gridPrototypePtr = GridTypes::Get(GetScenario()).Find(gridType);
      if (!gridPrototypePtr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized coverage grid type."};
      }
      auto gridPtr = ut::clone(gridPrototypePtr);
      gridPtr->SetName(fullName);
      block.ProcessInput(gridPtr.get());
      mSubGrids.emplace_back(std::move(gridPtr));
   }
   else
   {
      myCommand = Grid::ProcessInput(aInput);
   }
   return myCommand;
}

void CompositeGrid::PreparePlatforms(WsfSimulation& aSimulation)
{
   for (auto& grid : mSubGrids)
   {
      grid->PreparePlatforms(aSimulation);
   }

   auto iter = mNameToId.begin();
   while (iter != mNameToId.end())
   {
      if (!aSimulation.GetPlatformByName(iter->first))
      {
         // The platform was not created by one of the sub-grids during its PreparePlatforms.
         // Remove that platform from this grid's maps.
         mPoints.erase(iter->second);
         mAssets.erase(iter->second);
         iter = mNameToId.erase(iter);
      }
      else
      {
         ++iter;
      }
   }
}

const Point* CompositeGrid::GetPoint(PointId aPointId) const
{
   const Point* retvalPtr{nullptr};
   auto         iter = mPoints.find(aPointId);
   if (iter != mPoints.end())
   {
      retvalPtr = &(iter->second);
   }
   return retvalPtr;
}

void CompositeGrid::Invoke(GridAction& aAction) const
{
   for (auto& point : mPoints)
   {
      aAction.Call(*this, point.second);
   }
}

GridAsset CompositeGrid::GetAsset(PointId aPointId) const
{
   GridAsset retval{"", cINVALID_POINT_ID, ""};
   auto      iter = mAssets.find(aPointId);
   if (iter != mAssets.end())
   {
      retval = iter->second;
   }
   return retval;
}

GridAsset CompositeGrid::GetAsset(const std::string& aPlatformName) const
{
   PointId id{cINVALID_POINT_ID};
   auto    iter = mNameToId.find(aPlatformName);
   if (iter != mNameToId.end())
   {
      id = iter->second;
   }
   return GetAsset(id);
}

void CompositeGrid::HasNullDevice(bool& aAnyNull, bool& aAllNull) const
{
   aAnyNull = false;
   aAllNull = true;
   for (auto& grid : mSubGrids)
   {
      bool any;
      bool all;
      grid->HasNullDevice(any, all);
      aAnyNull = aAnyNull || any;
      aAllNull = aAllNull && all;
   }
}

bool CompositeGrid::IsStructured() const
{
   // This grid will count as structured if the subgrids are structured, and each
   // only has one component.
   bool retval{true};
   for (auto& iter : mSubGrids)
   {
      retval = retval && iter->IsStructured() && iter->GetNumComponents() == 1;
   }
   return retval;
}

std::size_t CompositeGrid::GetNumComponents() const
{
   return mSubGrids.size();
}

std::string CompositeGrid::GetComponentName(std::size_t aComponentIndex) const
{
   std::string retval{};
   if (aComponentIndex < mSubGrids.size())
   {
      retval = mSubGrids[aComponentIndex]->GetName();
   }
   return retval;
}

std::pair<std::size_t, std::size_t> CompositeGrid::GetComponentSizes(std::size_t aComponentIndex) const
{
   std::pair<std::size_t, std::size_t> retval{0, 0};
   if (aComponentIndex < mSubGrids.size())
   {
      retval = mSubGrids[aComponentIndex]->GetComponentSizes(0);
   }
   return retval;
}

std::pair<std::size_t, std::size_t> CompositeGrid::GetPointIndices(PointId aPointId) const
{
   std::pair<std::size_t, std::size_t> retval{0, 0};
   auto                                idIter   = mParentToChild.find(aPointId);
   auto                                compIter = mIdToComponent.find(aPointId);
   if (idIter != mParentToChild.end() && compIter != mIdToComponent.end())
   {
      retval = mSubGrids[compIter->second]->GetPointIndices(idIter->second);
   }
   return retval;
}

void CompositeGrid::InvokeOnComponent(GridAction& aAction, std::size_t aComponentIndex) const
{
   // This action translates the child grid's points into points applicable to the parent grid.
   auto translateActionPtr = CreateGridAction(
      [this, &aAction, aComponentIndex](const Grid& aGrid, const Point& aPoint)
      {
         auto iter = mChildToParent[aComponentIndex].find(aPoint.GetID());
         if (iter != mChildToParent[aComponentIndex].end())
         {
            // Create the translated point.
            Point modPoint{iter->second, aPoint.GetLatitude(), aPoint.GetLongitude(), aPoint.GetAltitude()};
            // Call the provided action on the parent grid, with the translated point.
            aAction.Call(*this, modPoint);
         }
      });
   mSubGrids[aComponentIndex]->InvokeOnComponent(*translateActionPtr, 0);
}

bool CompositeGrid::InitializeP()
{
   // Check that the sub-grids all create platforms
   bool allCreate{true};
   for (auto& grid : mSubGrids)
   {
      if (!grid->CreatesNewPlatforms())
      {
         allCreate = false;
         break;
      }
   }
   if (!allCreate)
   {
      auto err = ut::log::error() << "Composite grid initialization error.";
      err.AddNote() << "All sub-grids must create their own platforms.";
      err.AddNote() << "Grid: " << GetName();
      return false;
   }

   mChildToParent.resize(mSubGrids.size());

   // Perform setup for each sub-grid
   for (std::size_t gridIdx = 0; gridIdx < mSubGrids.size(); ++gridIdx)
   {
      // Propagate options to sub-grids.
      mSubGrids[gridIdx]->SetSuppressGridPlatforms(GetSuppressGridPlatforms());
      mSubGrids[gridIdx]->SetCentralBody(GetCentralBody());
      // Grid::WriteGridData file would not be aware of mSubGrids, so there is no
      // need to propagate the filename to mSubGrids.

      // Must mark the sub-grid as used, or the subsequent Initialize will not complete.
      mSubGrids[gridIdx]->SetUsedFlag();
      if (!mSubGrids[gridIdx]->Initialize())
      {
         return false;
      }

      // Add points and assets to maps.
      auto actionPtr = CreateGridAction(
         [gridIdx, this](const Grid& aGrid, const Point& aPoint)
         {
            PointId id = GetNextAvailableId();

            auto ga = aGrid.GetAsset(aPoint.GetID());
            ga.SetPointId(id);
            mAssets.emplace(id, ga);

            mPoints.emplace(id, Point{id, aPoint.GetLatitude(), aPoint.GetLongitude(), aPoint.GetAltitude()});

            mNameToId.emplace(ga.GetPlatformName(), id);
            mIdToComponent.emplace(id, gridIdx);

            mParentToChild.emplace(id, aPoint.GetID());
            mChildToParent[gridIdx].emplace(aPoint.GetID(), id);
         });
      mSubGrids[gridIdx]->Invoke(*actionPtr);
   }
   return true;
}

} // namespace coverage
} // namespace wsf
