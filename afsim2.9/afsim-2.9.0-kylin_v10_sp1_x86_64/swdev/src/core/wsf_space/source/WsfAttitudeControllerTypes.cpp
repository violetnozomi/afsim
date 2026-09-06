// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfAttitudeControllerTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace // {anonymous}
{
class AttitudeControllerComponentFactory : public WsfComponentFactory<WsfSpaceMoverBase>
{
public:
};
} // namespace

WsfAttitudeControllerTypes& WsfAttitudeControllerTypes::Get(const WsfScenario& aScenario)
{
   return static_cast<WsfAttitudeControllerTypes&>(*aScenario.GetTypes("attitude_controller"));
}

WsfAttitudeControllerTypes::WsfAttitudeControllerTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfAttitudeController>(aScenario, cSINGULAR_BASE_TYPE, "attitude_controller")
{
   aScenario.RegisterComponentFactory(ut::make_unique<AttitudeControllerComponentFactory>());
}
