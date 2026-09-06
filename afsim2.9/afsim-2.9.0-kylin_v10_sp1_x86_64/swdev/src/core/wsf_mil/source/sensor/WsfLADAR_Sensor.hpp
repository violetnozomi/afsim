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

#ifndef WSFLADAR_SENSOR_HPP
#define WSFLADAR_SENSOR_HPP

#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfLASER_RcvrComponent.hpp"
#include "WsfLASER_XmtrComponent.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

//! A specialization of WsfSensor that implements a simple Laser Detection and Range Finding (LADAR) Sensor.

class WsfLADAR_Sensor : public WsfSensor
{
public:
   WsfLADAR_Sensor(WsfScenario& aScenario);
   WsfLADAR_Sensor(const WsfLADAR_Sensor& aSrc);
   WsfLADAR_Sensor& operator=(const WsfLADAR_Sensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;

   static double SpectralRadiantEmittance(double aTemperature, double aWavelength);

   class LADAR_Result : public WsfSensorResult
   {
   public:
      void Print(ut::log::MessageStream& aMsgStream) override;

      WsfLASER_RcvrComponent::DetectionData mDetectionData;
   };

   //! A mode of the sensor.
   class LADAR_Mode : public WsfSensorMode
   {
   public:
      LADAR_Mode();
      LADAR_Mode(const LADAR_Mode& aSrc);
      LADAR_Mode& operator=(const LADAR_Mode&) = delete;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      virtual bool AttemptToDetect(double        aSimTime,
                                   WsfPlatform*  aTargetPtr,
                                   WsfEM_Xmtr&   aXmtr,
                                   Settings&     aSettings,
                                   LADAR_Result& aResult);

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      void ComputeBackgroundRadiance();

      double ComputeTargetSolarIrradiance(double aTargetArea, double aTargetReflectivity, double aRange, double aTransmittance);

      void ComputeProbabilityOfDetection(const WsfEM_Xmtr& aXmtr, LADAR_Result& aResult);

      double ComputeGaussianDetectionProbability(double aSignal, double aNoise, double aThreshold) const;

      WsfEM_Antenna mAntenna;

      //! The transmitter for a Mode.
      WsfEM_Xmtr mXmtr;

      //! The receiver for a Mode.
      WsfEM_Rcvr mRcvr;

      //! The xmtr component for laser transmission.
      WsfLASER_XmtrComponent* mLASER_XmtrComponentPtr;

      //! The rcvr component for laser reception.
      WsfLASER_RcvrComponent* mLASER_RcvrComponentPtr;

      //! Blackbody temperature for the background. (deg-K)
      double mBackgroundTemperature;

      //! Background irradiance (user specified or computed from the background temperature).
      double mBackgroundSpectralIrradiance; // W/m2/m

      //! Integration gain for the simple detector
      double mIntegrationGain;

      //! The detection threshold for the simple detector.
      double mDetectionThreshold;

      //! Probability of detection table (Pd vs. S/N).
      UtCloneablePtr<UtTable::Curve> mDetectionProbabilityPtr;
   };

protected:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<LADAR_Mode*> mLADAR_ModeList;
};

#endif
