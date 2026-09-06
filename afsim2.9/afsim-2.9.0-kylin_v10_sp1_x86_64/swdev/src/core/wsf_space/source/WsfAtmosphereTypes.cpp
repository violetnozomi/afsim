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

#include "WsfAtmosphereTypes.hpp"

#include "WsfJacchiaRobertsAtmosphere.hpp"
#include "WsfPiecewiseExponentialAtmosphere.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace space
{

AtmosphereTypes& AtmosphereTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<AtmosphereTypes&>(*aScenario.GetTypes("base_atmosphere_model"));
}

AtmosphereTypes::AtmosphereTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<wsf::space::Atmosphere>(aScenario, cSINGULAR_BASE_TYPE, "base_atmosphere_model")
{
   Add(JacchiaRobertsAtmosphere::cTYPE, ut::make_unique<JacchiaRobertsAtmosphere>());
   Add(PiecewiseExponentialAtmosphere::cTYPE, ut::make_unique<PiecewiseExponentialAtmosphere>());
}

} // namespace space
} // namespace wsf
