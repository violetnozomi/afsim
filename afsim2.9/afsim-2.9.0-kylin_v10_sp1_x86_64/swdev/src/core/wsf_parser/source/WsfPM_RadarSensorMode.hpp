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

#ifndef WSFPM_RADARSENSORMODE_HPP
#define WSFPM_RADARSENSORMODE_HPP

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_Transmitter.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_RadarSensorMode : public WsfPProxyNode
{
public:
   WsfPM_RadarSensorMode();
   WsfPM_RadarSensorMode(const WsfPProxyNode& aNode);

   //! @note The beam layer needs to be added.
   WsfPM_Transmitter transmitter() const;
};

using WsfPM_RadarSensorModeMap = WsfPM_ObjectMapT<WsfPM_RadarSensorMode>;

#endif // WSFPM_RADARSENSORMODE_HPP
