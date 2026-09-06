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

#include "WsfMilSensorTypes.hpp"

#include "UtMemory.hpp"
#include "WsfScenario.hpp"
#include "WsfSensorTypes.hpp"

// Include files for built-in types
#include "WsfAcousticSensor.hpp"
#include "WsfBeamDirector.hpp"
#include "WsfEOIR_Sensor.hpp"
#include "WsfESM_Sensor.hpp"
#include "WsfIRST_Sensor.hpp"
#include "WsfLADAR_Sensor.hpp"
#include "WsfLaserDesignator.hpp"
#include "WsfLaserTracker.hpp"
#include "WsfOTH_RadarSensor.hpp"
#include "WsfOpticalSensor.hpp"
#include "WsfSAR_Sensor.hpp"
#include "WsfSurfaceWaveRadarSensor.hpp"

void WsfMilSensorTypes::Register(WsfScenario& aScenario)
{
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_ACOUSTIC_SENSOR", ut::make_unique<WsfAcousticSensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_BEAM_DIRECTOR", ut::make_unique<WsfBeamDirector>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_EOIR_SENSOR", ut::make_unique<WsfEOIR_Sensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_ESM_SENSOR", ut::make_unique<WsfESM_Sensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_IRST_SENSOR", ut::make_unique<WsfIRST_Sensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_LADAR_SENSOR", ut::make_unique<WsfLADAR_Sensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_LASER_TRACKER", ut::make_unique<WsfLaserTracker>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_LASER_DESIGNATOR", ut::make_unique<WsfLaserDesignator>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_OPTICAL_SENSOR", ut::make_unique<WsfOpticalSensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_OTH_RADAR_SENSOR", ut::make_unique<WsfOTH_RadarSensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_SAR_SENSOR", ut::make_unique<WsfSAR_Sensor>(aScenario));
   WsfSensorTypes::Get(aScenario).AddCoreType("WSF_SURFACE_WAVE_RADAR_SENSOR",
                                              ut::make_unique<WsfSurfaceWaveRadarSensor>(aScenario));
}
