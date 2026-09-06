// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFPM_FUEL_HPP
#define WSFPM_FUEL_HPP

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"

class WsfPM_Fuel : public WsfPM_PlatformPart
{
public:
   WsfPM_Fuel() {}
   WsfPM_Fuel(const WsfPProxyNode& aNode)
      : WsfPM_PlatformPart(aNode)
   {
   }
};

using WsfPM_FuelMap = WsfPM_ObjectMapT<WsfPM_Comm>;

#endif
