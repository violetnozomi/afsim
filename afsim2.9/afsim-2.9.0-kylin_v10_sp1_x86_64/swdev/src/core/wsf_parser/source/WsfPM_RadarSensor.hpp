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

#ifndef WSFPM_RADARSENSOR_HPP
#define WSFPM_RADARSENSOR_HPP

#include "wsf_parser_export.h"

#include "WsfPM_RadarSensorMode.hpp"
#include "WsfPM_Sensor.hpp"

class WSF_PARSER_EXPORT WsfPM_RadarSensor : public WsfPM_Sensor
{
public:
   WsfPM_RadarSensor();
   WsfPM_RadarSensor(const WsfPProxyNode& aNode);

   WsfPM_RadarSensorMode    modeTemplate() const;
   WsfPM_RadarSensorModeMap modes() const;
};

using WsfPM_RadarSensorMap = WsfPM_ObjectMapT<WsfPM_RadarSensor>;

#endif // WSFPM_RADARSENSOR_HPP
