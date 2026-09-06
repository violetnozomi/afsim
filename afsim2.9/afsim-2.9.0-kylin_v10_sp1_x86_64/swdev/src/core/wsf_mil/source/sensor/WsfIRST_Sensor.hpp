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

#ifndef WSFIRST_SENSOR_HPP
#define WSFIRST_SENSOR_HPP

#include "wsf_mil_export.h"

#include "WsfEOIR_IRST_SensorMode.hpp"
#include "WsfSensor.hpp"

//! A specialization of WsfSensor that implements a simple infrared search/track sensor.

class WSF_MIL_EXPORT WsfIRST_Sensor : public WsfSensor
{
public:
   WsfIRST_Sensor(WsfScenario& aScenario);
   WsfIRST_Sensor(const WsfIRST_Sensor& aSrc);
   WsfIRST_Sensor& operator=(const WsfIRST_Sensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   // Default copy constructor (for Clone())
   // WsfIRST_Sensor(const WsfIRST_Sensor& aSrc);

   //! A mode of the sensor.
   class IRST_Mode : public WsfEOIR_IRST_SensorMode
   {
   public:
      IRST_Mode(WsfScenario& aScenario);
      IRST_Mode(const IRST_Mode& aSrc) = default;
      ~IRST_Mode() override            = default;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult) override;
      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;
   };

protected:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<IRST_Mode*> mIRST_ModeList;
};

#endif
