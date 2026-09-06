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

#include "WsfPM_RadarSensor.hpp"

WsfPM_RadarSensor::WsfPM_RadarSensor() {}

WsfPM_RadarSensor::WsfPM_RadarSensor(const WsfPProxyNode& aNode)
   : WsfPM_Sensor(aNode)
{
}

WsfPM_RadarSensorMode WsfPM_RadarSensor::modeTemplate() const
{
   return WsfPM_RadarSensorMode(*this + "template");
}

WsfPM_RadarSensorModeMap WsfPM_RadarSensor::modes() const
{
   return WsfPM_RadarSensorModeMap(*this + "mode");
}
