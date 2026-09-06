// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016-2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofObject.hpp"

#include <iostream>
#include <string>

P6DofObject::P6DofObject(P6DofScenario* aScenario)
   : mScenario(aScenario)
   , mParentVehicle(nullptr)
   , mObjName("")
{
}

P6DofObject* P6DofObject::Clone() const
{
   return new P6DofObject(*this);
}
