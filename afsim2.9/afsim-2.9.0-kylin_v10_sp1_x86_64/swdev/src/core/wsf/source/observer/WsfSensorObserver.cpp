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

#include "WsfSensorObserver.hpp"

#include "WsfSimulation.hpp"

WSF_OBSERVER_CALLBACK_DEFINE(Sensor, ImageCreated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorDetectionAttempt)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorDetectionChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorFrequencyChanged)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorModeActivated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorModeDeactivated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorRequestCanceled)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorRequestInitiated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorRequestUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorScanUpdated)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorTurnedOff)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorTurnedOn)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorNonOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorOperational)
WSF_OBSERVER_CALLBACK_DEFINE(Sensor, SensorBroken)
