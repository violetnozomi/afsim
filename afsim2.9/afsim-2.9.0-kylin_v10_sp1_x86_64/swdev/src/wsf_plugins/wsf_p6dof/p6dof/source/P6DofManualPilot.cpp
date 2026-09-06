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

#include "P6DofManualPilot.hpp"

#include "P6DofCommonController.hpp"

P6DofManualPilot::P6DofManualPilot(P6DofScenario* aScenario)
   : P6DofPilotObject(aScenario)
{
}

P6DofManualPilot* P6DofManualPilot::Clone() const
{
   return new P6DofManualPilot(*this);
}

P6DofManualPilot::P6DofManualPilot(P6DofPilotObject* aPilotObject)
   : P6DofPilotObject(*aPilotObject)
{
}
