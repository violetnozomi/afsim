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

#include "WsfAtmosphereScriptRegistration.hpp"

#include "UtMemory.hpp"
#include "UtScriptTypes.hpp"
#include "WsfScriptAtmosphere.hpp"
#include "WsfScriptJacchiaRobertsAtmosphere.hpp"
#include "WsfScriptPiecewiseExponentialAtmosphere.hpp"

namespace wsf
{
namespace space
{

void AtmosphereScriptRegistration(UtScriptTypes& aScriptTypes)
{
   aScriptTypes.Register(ut::make_unique<WsfScriptAtmosphere>("WsfAtmosphereModel", &aScriptTypes));
   aScriptTypes.Register(ut::make_unique<WsfScriptJacchiaRobertsAtmosphere>("WsfJacchiaRobertsAtmosphere", &aScriptTypes));
   aScriptTypes.Register(
      ut::make_unique<WsfScriptPiecewiseExponentialAtmosphere>("WsfPiecewiseExponentialAtmosphere", &aScriptTypes));
}

} // namespace space
} // namespace wsf
