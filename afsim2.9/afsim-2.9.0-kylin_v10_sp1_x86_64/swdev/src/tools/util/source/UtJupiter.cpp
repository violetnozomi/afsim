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

#include "UtJupiter.hpp"

#include "UtCalendar.hpp"
#include "UtSolarSystem.hpp"

UtVec3d ut::Jupiter::GetLocationECI(const UtCalendar& aEpoch) const
{
   UtVec3d retval;
   mPlanetPtr->GetLocationECI(aEpoch, retval);
   return retval;
}
