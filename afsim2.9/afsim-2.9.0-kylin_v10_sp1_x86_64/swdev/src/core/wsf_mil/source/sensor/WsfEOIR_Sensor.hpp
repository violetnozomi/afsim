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

#ifndef WSFEOIR_SENSOR_HPP
#define WSFEOIR_SENSOR_HPP

#include "wsf_mil_export.h"

#include "WsfEOIR_IRST_SensorMode.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorScheduler.hpp"
#include "WsfStandardSensorErrorModel.hpp"

//! A specialization of WsfSensor that provides a simple EO/IR sensor.
//!
//! The implementation is as follows:
//!
//! - Every 'frame_time', the sensor attempts to detect every object.
//! - Objects that are detected are considered to be 'in' the image.
class WSF_MIL_EXPORT WsfEOIR_Sensor : public WsfSensor
{
public:
   explicit WsfEOIR_Sensor(WsfScenario& aScenario);
   WsfEOIR_Sensor(const WsfEOIR_Sensor& aSrc);
   ~WsfEOIR_Sensor() override;
   WsfEOIR_Sensor& operator=(const WsfEOIR_Sensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   void       PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       TurnOff(double aSimTime) override;
   void       TurnOn(double aSimTime) override;
   void       Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;

   //! EOIR sensor Error model.
   class WSF_MIL_EXPORT EOIR_ErrorModel : public wsf::StandardSensorErrorModel
   {
   public:
      EOIR_ErrorModel* Clone() const { return new EOIR_ErrorModel(*this); }

      void ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;
   };

protected:
   //! A detection opportunity for a specific platform
   class EOIR_Chance
   {
   public:
      explicit EOIR_Chance(WsfPlatform* aTargetPtr);
      EOIR_Chance() = default;
      ~EOIR_Chance();
      EOIR_Chance& operator=(const EOIR_Chance&) = delete;

      WsfTrack* mTrackPtr{nullptr};
      size_t    mModeIndex{0};
      double    mLocationWCS[3]{0.0, 0.0, 0.0};
      size_t    mTargetIndex{0};
      bool      mVisibleInImage{true};
      bool      mNowDetecting{false}; //!< Used in NotifyObservers
   };

   //! A 'mode' of the sensor.
   class EOIR_Mode : public WsfEOIR_IRST_SensorMode
   {
   public:
      explicit EOIR_Mode(WsfScenario& aScenario);
      EOIR_Mode(const EOIR_Mode& aSrc) = default;
      EOIR_Mode& operator=(const EOIR_Mode&) = delete;
      ~EOIR_Mode() override                  = default;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, Result& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      int    mPixelCountWidth;
      int    mPixelCountHeight;
      double mAngularResolution; //! radians
      double mOmegaResolution;
   };

   void ClearSensingChances();

   class EOIR_SensorScheduler : public WsfSensorScheduler
   {
   public:
      EOIR_SensorScheduler();
      ~EOIR_SensorScheduler() override = default;

      bool SelectTarget(double               aSimTime,
                        double&              aNextSimTime,
                        size_t&              aTargetIndex,
                        WsfTrackId&          aRequestId,
                        WsfSensor::Settings& aSettings) override;

      WsfSensorScheduler* Clone() const override;

   private:
      size_t mTargetNumber;
   };

   void NotifyObservers(double aSimTime, EOIR_Mode* aModePtr, EOIR_Chance* aChancePtr, WsfSensorResult& aResult);

   void ProcessSensorDetectionChanged(double aSimTime, const EOIR_Chance& aChance, unsigned int aStatus);

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<EOIR_Mode*> mEOIR_ModeList;

   //! The list of platforms that are eligible for detection attempts
   using Chances = std::unordered_map<size_t, EOIR_Chance>;
   Chances mChances;

   //! The stream number is incremented each time the sensor is turned on.
   unsigned int mStreamNumber;

   //! The frame number is incremented each time a frame is produced and reset
   //! when the sensor is turned on.
   unsigned int mFrameNumber;

   //! true if SensorTrack<XXX> observers should be called
   bool mCallSensorTrackObservers;
};

#endif
