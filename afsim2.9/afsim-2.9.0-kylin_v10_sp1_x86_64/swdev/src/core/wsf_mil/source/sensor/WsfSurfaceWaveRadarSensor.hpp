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

#ifndef WSFSURFACEWAVERADARSENSOR_HPP
#define WSFSURFACEWAVERADARSENSOR_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Noise.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMarcumSwerling.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
class WsfSensorResult;

//! A specialization of WsfSensor that implements a surface-wave radar sensor
class WSF_MIL_EXPORT WsfSurfaceWaveRadarSensor : public WsfSensor
{
public:
   WsfSurfaceWaveRadarSensor(WsfScenario& aScenario);
   WsfSurfaceWaveRadarSensor(const WsfSurfaceWaveRadarSensor& aSrc);
   WsfSurfaceWaveRadarSensor& operator=(const WsfSurfaceWaveRadarSensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;

   // some compilers require inner classes to be declared friends if they want to
   // access outer class members
   class RadarMode;
   friend class RadarMode;

   //! A 'mode' of the sensor.
   class RadarMode : public WsfSensorMode, public WsfSensorBeam
   {
   public:
      RadarMode();
      RadarMode(const RadarMode& aSrc);
      RadarMode& operator=(const RadarMode&) = delete;

      // WsfSensorMode methods.
      size_t         GetBeamCount() const override { return 1; }
      WsfSensorBeam* GetBeamEntry(size_t /* aBeamIndex */) override { return this; }

      // WsfSensorBeam methods.
      WsfEM_Rcvr* GetEM_Rcvr() override { return &mRcvr; }
      WsfEM_Xmtr* GetEM_Xmtr() override { return &mXmtr; }

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      void Calibrate();

      void   SetIntegrationGain(double aIntegrationGain) override { mIntegrationGain = aIntegrationGain; }
      double GetIntegrationGain() const override { return mIntegrationGain; }
      double GetDopplerResolution() const { return mDopplerResolution; }

   protected:
      void AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, WsfEM_Xmtr* aXmtrPtr, WsfSensorResult& aResult);

      //! Doppler resolution as velocity (m/s)
      double mDopplerResolution;

      //! A term to represent a general adjustment (multiplier) to the radar range equation.
      double mAdjustmentFactor;

      //! Integration gain for non-Swerling detector.
      double mIntegrationGain;

      wsf::MarcumSwerling mDetector;

      //! If 'true' then used the detector.
      bool mUseDetector;

      WsfEM_Antenna mAntenna;

      //! The transmitter for a radar beam.
      WsfEM_Xmtr mXmtr;

      //! The receiver for a radar beam.
      WsfEM_Rcvr mRcvr;

      //! the EM_noise for a radar beam
      WsfEM_Noise mNoise;
   };

private:
   //! the sensor-specific list of modes (not valid until Initialize is called)
   std::vector<RadarMode*> mRadarModeList;
};

#endif
