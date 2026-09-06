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

#ifndef WSFNULLSENSOR_HPP
#define WSFNULLSENSOR_HPP

#include "wsf_export.h"

#include "WsfSensor.hpp"

//! A sensor that does not detect anything.
//! This sensor is used for replacing a sensor on a platform with one that does
//! absolutely nothing. This allows the structure of the platform to be maintained,
//! possibly eliminate the need to update control logic or scenario files that
//! refer to the sensor.
class WSF_EXPORT WsfNullSensor : public WsfSensor
{
public:
   WsfNullSensor(WsfScenario& aScenario);
   WsfNullSensor(const WsfNullSensor& aSrc);

   bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

   WsfSensor* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;

   ~WsfNullSensor() override;
};

#endif
