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

#ifndef WSFESM_SENSOR_HPP
#define WSFESM_SENSOR_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "WsfPassiveSensor.hpp"

//! A specialization of WsfSensor that provides the baseline passive RF sensor.
//! This is useful for modeling RWR, ELINT and COMINT sensing functions.

class WSF_MIL_EXPORT WsfESM_Sensor : public WsfPassiveSensor
{
public:
   // Define inner classes to be declared friends to access outer class members on some compilers

   //! A 'mode' of the sensor.
   class WSF_MIL_EXPORT ESM_Mode : public WsfPassiveSensor::PassiveMode
   {
   public:
      ESM_Mode()                     = default;
      ESM_Mode(const ESM_Mode& aSrc) = default;
      ESM_Mode& operator=(const ESM_Mode& aSrc) = default;
      ~ESM_Mode() override                      = default;

      // WsfSensorMode methods
      WsfMode* Clone() const override;

   protected:
      void ComputeInterferenceEffect(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfEM_Rcvr* aRcvrPtr) override;

      bool UseListenerForInterferer(WsfEM_Xmtr* aXmtrPtr) override;
   };

   WsfESM_Sensor(WsfScenario& aScenario);
   WsfESM_Sensor(const WsfESM_Sensor& aSrc) = default;
   WsfESM_Sensor& operator=(const WsfESM_Sensor& aSrc) = delete;
   ~WsfESM_Sensor() override                           = default;

   //! @name Sensor framework methods
   //@{
   WsfSensor* Clone() const override;
   //@}

   const char* GetScriptClassName() const override { return "WsfESM_Sensor"; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
};

#endif
