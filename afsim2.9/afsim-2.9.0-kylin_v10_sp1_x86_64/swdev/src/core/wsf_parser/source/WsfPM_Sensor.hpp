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

#ifndef WSFPM_SENSOR_HPP
#define WSFPM_SENSOR_HPP

#include "wsf_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"

class WSF_PARSER_EXPORT WsfPM_Sensor : public WsfPM_PlatformPart
{
public:
   WsfPM_Sensor();
   WsfPM_Sensor(const WsfPProxyNode& aNode);
};

using WsfPM_SensorMap = WsfPM_ObjectMapT<WsfPM_Sensor>;

#endif
