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

#ifndef WSFCOVERAGESCENARIOEXTENSION_HPP
#define WSFCOVERAGESCENARIOEXTENSION_HPP

#include "WsfScenarioExtension.hpp"

namespace wsf
{
namespace coverage
{

class ScenarioExtension : public WsfCloneableScenarioExtension
{
public:
   ScenarioExtension();
   ~ScenarioExtension() override = default;

   void AddedToScenario() override;
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGESCENARIOEXTENSION_HPP
