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

#ifndef WSFCYBERVISUALIZATIONMANAGER_HPP
#define WSFCYBERVISUALIZATIONMANAGER_HPP

#include <memory>

#include "WsfCyberVisualization.hpp"

namespace wsf
{
namespace cyber
{

class VisualizationManager : public Visualization
{
public:
   ~VisualizationManager() override = default;

   static VisualizationManager& Get(WsfSimulation& aSimulation);

   void AttackInitiated(const Engagement& aEngagement) final;
   void AttackSucceeded(const Engagement& aEngagement) final;
   void AttackFailed(const Engagement& aEngagement) final;

   void ScanInitiated(const Engagement& aEngagement) final;
   void ScanSucceeded(const Engagement& aEngagement) final;
   void ScanFailed(const Engagement& aEngagement) final;

   std::string GetName() const final;

   void AddVisualization(std::unique_ptr<Visualization> aVisualizationPtr);
   void RemoveVisualization(const std::string& aName);

private:
   std::vector<std::unique_ptr<Visualization>> mVisualizations;
};

} // namespace cyber
} // namespace wsf

#endif
