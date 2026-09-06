// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberVisualizationManager.hpp"

#include "WsfCyberSimulationExtension.hpp"

namespace wsf
{
namespace cyber
{

VisualizationManager& VisualizationManager::Get(WsfSimulation& aSimulation)
{
   return SimulationExtension::Get(aSimulation).GetCyberVisualizationManager();
}

void VisualizationManager::AttackInitiated(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->AttackInitiated(aEngagement);
   }
}

void VisualizationManager::AttackSucceeded(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->AttackSucceeded(aEngagement);
   }
}

void VisualizationManager::AttackFailed(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->AttackFailed(aEngagement);
   }
}

void VisualizationManager::ScanInitiated(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->ScanInitiated(aEngagement);
   }
}

void VisualizationManager::ScanSucceeded(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->ScanSucceeded(aEngagement);
   }
}

void VisualizationManager::ScanFailed(const Engagement& aEngagement)
{
   for (auto& v : mVisualizations)
   {
      v->ScanFailed(aEngagement);
   }
}

std::string VisualizationManager::GetName() const
{
   return "wsf::cyber::VisualizationManager";
}

void VisualizationManager::AddVisualization(std::unique_ptr<Visualization> aVisualizationPtr)
{
   mVisualizations.push_back(std::move(aVisualizationPtr));
}

void VisualizationManager::RemoveVisualization(const std::string& aName)
{
   for (auto it = mVisualizations.begin(); it != mVisualizations.end();)
   {
      if ((*it)->GetName() == aName)
      {
         it = mVisualizations.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

} // namespace cyber
} // namespace wsf
