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

#include "WsfCoverageSimulationExtension.hpp"

#include <sstream>
#include <stdexcept>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfCoverageAsset.hpp"
#include "WsfCoverageGridTypes.hpp"
#include "WsfCoverageScenarioExtension.hpp"
#include "WsfCoverageTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace coverage
{

bool SimulationExtension::ProcessInput(UtInput& aInput)
{
   bool        myCommand{false};
   std::string command;
   aInput.GetCommand(command);
   if (command == "grid")
   {
      UtInputBlock block{aInput};
      std::string  gridName;
      if (!block.ReadCommand(gridName))
      {
         throw UtInput::BadValue{aInput, "Expected coverage grid name."};
      }
      if (mGrids.find(gridName) != mGrids.end())
      {
         std::stringstream msg{};
         msg << "Grid with name '" << gridName << "' already defined.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
      std::string gridType;
      if (!block.ReadCommand(gridType))
      {
         throw UtInput::BadValue{aInput, "Expected coverage grid type."};
      }
      auto gridPrototypePtr = GridTypes::Get(mScenarioExtension.GetScenario()).Find(gridType);
      if (!gridPrototypePtr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized coverage grid type."};
      }
      auto gridPtr = ut::clone(gridPrototypePtr);
      gridPtr->SetName(gridName);
      block.ProcessInput(gridPtr.get());
      mGrids[gridName] = std::move(gridPtr);
      myCommand        = true;
   }
   else if (command == "coverage")
   {
      UtInputBlock block{aInput};
      std::string  covName;
      if (!block.ReadCommand(covName))
      {
         throw UtInput::BadValue{aInput, "Expected coverage name."};
      }
      std::string covType;
      if (!block.ReadCommand(covType))
      {
         throw UtInput::BadValue{aInput, "Expected coverage type."};
      }
      auto covPrototypePtr = CoverageTypes::Get(mScenarioExtension.GetScenario()).Find(covType);
      if (!covPrototypePtr)
      {
         throw UtInput::BadValue{aInput, "Unrecognized coverage type."};
      }
      auto covPtr = ut::clone(covPrototypePtr);
      covPtr->SetName(covName);
      block.ProcessInput(covPtr.get());
      mCoverages[covName] = std::move(covPtr);
      myCommand           = true;
   }
   return myCommand;
}

bool SimulationExtension::Initialize()
{
   bool retval{true};

   for (auto& coverage : mCoverages)
   {
      retval = retval && coverage.second->Initialize(GetSimulation());
   }

   for (auto& grid : mGrids)
   {
      retval = retval && grid.second->Initialize();
   }

   RemoveUnusedGrids();

   return retval;
}

void SimulationExtension::PendingStart()
{
   for (auto& coverage : mCoverages)
   {
      if (!coverage.second->ResolveAssets(GetSimulation()))
      {
         throw std::runtime_error{"Unable to resolve coverage assets."};
      }
   }
   for (auto& grid : mGrids)
   {
      grid.second->PreparePlatforms(GetSimulation());
   }
   for (auto& coverage : mCoverages)
   {
      coverage.second->CategorizePlatforms(GetSimulation());
   }
   CategorizeAllNonAssets(GetSimulation());
   for (auto& coverage : mCoverages)
   {
      coverage.second->PendingStart(GetSimulation());
   }
}

void SimulationExtension::Complete(double aSimTime)
{
   for (auto& grid : mGrids)
   {
      grid.second->WriteGridDataFile();
   }
   for (auto& coverage : mCoverages)
   {
      coverage.second->Finalize(aSimTime, GetSimulation().GetDateTime().GetStartDateAndTime());
   }
}

Grid* SimulationExtension::GetGrid(const std::string& aGridName) const
{
   Grid* retvalPtr{nullptr};
   auto  iter = mGrids.find(aGridName);
   if (iter != mGrids.end())
   {
      retvalPtr = iter->second.get();
   }
   return retvalPtr;
}

Coverage* SimulationExtension::GetCoverage(const std::string& aCoverageName) const
{
   Coverage* retvalPtr{nullptr};
   auto      iter = mCoverages.find(aCoverageName);
   if (iter != mCoverages.end())
   {
      retvalPtr = iter->second.get();
   }
   return retvalPtr;
}

const SimulationExtension& SimulationExtension::Get(const WsfSimulation& aSimulation)
{
   return static_cast<SimulationExtension&>(aSimulation.GetExtension("wsf_coverage"));
}

void SimulationExtension::CategorizeAllNonAssets(const WsfSimulation& aSimulation) const
{
   std::string ga{cUNIVERSAL_GRID_ASSET_CATEGORY};
   std::string fa{cUNIVERSAL_FREE_ASSET_CATEGORY};
   std::string na{cUNIVERSAL_NON_ASSET_CATEGORY};
   for (std::size_t i = 0; i < aSimulation.GetPlatformCount(); ++i)
   {
      WsfPlatform* platPtr = aSimulation.GetPlatformEntry(i);
      if (!platPtr->IsCategoryMember(ga) && !platPtr->IsCategoryMember(fa))
      {
         platPtr->AddCategory(na);
      }
   }
}

void SimulationExtension::RemoveUnusedGrids()
{
   auto iter = mGrids.begin();
   auto end  = mGrids.end();
   while (iter != end)
   {
      if (!iter->second->IsUsed())
      {
         iter->second = nullptr;
         iter         = mGrids.erase(iter);
      }
      else
      {
         ++iter;
      }
   }
}

} // namespace coverage
} // namespace wsf
