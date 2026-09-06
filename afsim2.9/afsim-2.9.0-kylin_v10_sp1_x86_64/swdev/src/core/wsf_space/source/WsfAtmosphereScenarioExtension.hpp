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

#ifndef WSFATMOSPHERESCENARIOEXTENSION_HPP
#define WSFATMOSPHERESCENARIOEXTENSION_HPP

#include "WsfScenarioExtension.hpp"

namespace wsf
{
namespace space
{

class AtmosphereScenarioExtension : public WsfCloneableScenarioExtension
{
public:
   AtmosphereScenarioExtension();
   ~AtmosphereScenarioExtension() override = default;

   static constexpr const char* cEXTENSION_NAME = "wsf_atmosphere_model_manager";

   void AddedToScenario() override;
};

} // namespace space
} // namespace wsf

#endif // WSFATMOSPHEREMODELSCENARIOEXTENSION_HPP
