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

#ifndef WSFRADARSENSOR_HPP
#define WSFRADARSENSOR_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "WsfDetectionProbabilityTable.hpp"
#include "WsfEM_Antenna.hpp"
class WsfEM_Clutter;
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMarcumSwerling.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"

//! A specialization of WsfSensor that implements a simple radar.
class WSF_EXPORT WsfRadarSensor : public WsfSensor
{
public:
   WsfRadarSensor(WsfScenario& aScenario);
   WsfRadarSensor(const WsfRadarSensor& aSrc);
   WsfRadarSensor& operator=(const WsfRadarSensor&) = delete;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   void       PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   void       PerformScheduledDetections(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   size_t      GetEM_RcvrCount(size_t aModeIndex) const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aModeIndex, size_t aIndex) const override;
   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;
   size_t      GetEM_XmtrCount(size_t aModeIndex) const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aModeIndex, size_t aIndex) const override;

   void UpdateXmtrRcvrLists();

   // Some compilers require inner classes to be declared friends if they want to access outer class members
   class RadarBeam;
   class RadarMode;
   friend class RadarBeam;
   friend class RadarMode;

   //! A RadarBeam represents a beam in a radar system.
   //! A single-beam radar is represents by one such object.
   //! A multi-beam radar is represented by a vector of these objects.
   class WSF_EXPORT RadarBeam : public WsfSensorBeam
   {
   public:
      RadarBeam();
      RadarBeam(const RadarBeam& aSrc);
      RadarBeam& operator=(const RadarBeam& aRhs);
      ~RadarBeam() override;

      // WsfSensorBeam methods
      WsfEM_Rcvr* GetEM_Rcvr() override { return mRcvrPtr.get(); }
      WsfEM_Xmtr* GetEM_Xmtr() override { return mXmtrPtr.get(); }

      void AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult);

      double               GetAdjustmentFactor() const { return mAdjustmentFactor; }
      double               GetIntegrationGain() const override { return mIntegrationGain; }
      void                 SetIntegrationGain(double aIntegrationGain) override { mIntegrationGain = aIntegrationGain; }
      int                  GetNumberOfPulsesIntegrated() const override { return mNumberOfPulsesIntegrated; }
      bool                 UsingDetector() const { return mUseDetector; }
      wsf::MarcumSwerling& GetDetector() { return mDetector; }

      //! Pointer to the detection probability table.
      //! If this is not a null pointer then it is used for determining the probability of detection.
      std::shared_ptr<wsf::DetectionProbabilityTable>& GetProbabilityTable() { return mProbabilityTablePtr; }

      double         GetDopplerResolution() const { return mDopplerResolution; }
      WsfEM_Clutter* GetClutter() const override { return mClutterPtr; }
      double         GetClutterAttenuationFactor() const { return mClutterAttenuationFactor; }

      bool Initialize(double       aSimTime,
                      unsigned int aBeamIndex,
                      WsfSensor*   aSensorPtr,
                      RadarMode*   aModePtr,
                      bool         aCanTransmit,
                      bool         aCanReceive,
                      bool         aShowCalibrationData,
                      bool         aIsMultiBeam);

      bool ProcessInput(UtInput& aInput) override;

      WsfStringId GetClutterType() const { return mClutterType; }

      std::unique_ptr<WsfEM_Antenna> mAntennaPtr;

      //! The transmitter for a radar beam.
      std::unique_ptr<WsfEM_Xmtr> mXmtrPtr;

      //! The receiver for a radar beam.
      std::unique_ptr<WsfEM_Rcvr> mRcvrPtr;

      //! The pointer to the clutter model (after initialization).
      //! This will be zero if no clutter model has been selected.
      WsfEM_Clutter* mClutterPtr;

      //! Additional parameters associated with measurement errors
      double mErrorModelAzBeamwidth;       // radians
      double mErrorModelElBeamwidth;       // radians
      double mErrorModelPulseWidth;        // seconds
      double mErrorModelDopplerResolution; // m/s

   private:
      WsfPlatform* GetPlatform();

      void AttemptToDetect(double           aSimTime,
                           WsfPlatform*     aTargetPtr,
                           Settings&        aSettings,
                           WsfEM_Xmtr*      aXmtrPtr,
                           WsfSensorResult& aResult);

      void Calibrate(bool aPrint);

      double ComputeIntegratedPulseCount(RadarMode& aMode);

      bool InitializeDetector(RadarMode& aMode);

      //! Can this beam transmit (copied from the sensor definition)
      bool mCanTransmit;

      //! If 'true' then used the detector (otherwise use fixed detection threshold)
      bool mUseDetector;

      //! Doppler resolution as velocity (m/s)
      double mDopplerResolution;

      //! If non-zero, this represents the 1 m^2 detection range.
      double mOneM2DetectRange;

      //! Applied when looking down at target.
      double mLookDownFactor;

      //! Applied if abs(closing speed) < Platform Speed.
      double mPRF_Factor;

      //! The adjustment to be made to the signal-to-noise required for detection once the sensor is locked on.
      double mPostLockonDetectionThresholdAdjustment;

      //! The amount of time to delay before applying the post-lockon detection threshold adjustment.
      double mPostLockonAdjustmentDelayTime;

      //! If non-zero, an alternative way of computing radar parameters.
      double mLoopGain;

      //! A term to represent a general adjustment (multiplier) to the radar range equation.
      double mAdjustmentFactor;

      //! Integration gain for non-Swerling detector.
      double mIntegrationGain;

      //! Number of pulses integrated for the Swerling detector.
      //! This is the actual input value. It will not match the value in mDetector (see reason below).
      int mNumberOfPulsesIntegrated;

      //! The detector model.
      //! @note Do not make this public for fear of it being misinterpreted. The integrated pulse
      //! count has been set to 1 after the call to InitializeDetector (see the code for the reason).
      //! Users who need the pulse count should call RadarBeam::GetNumberOfPulsesIntegrated to get the
      //! unmodified input value.
      wsf::MarcumSwerling mDetector;

      //! Pointer to the detection probability table.
      //! If this is not a null pointer then it is used for determining the probability of detection.
      std::shared_ptr<wsf::DetectionProbabilityTable> mProbabilityTablePtr;

      //! Clutter Attenuation Factor.
      //! A values between [0..1] which indicates the amount of clutter that gets through.
      //! (0 being totally attenuated, 1 being totally passed). This can represent the effects
      //! of clutter mapping, MTI, Doppler, or any other processing technique.
      double mClutterAttenuationFactor;

      //! The clutter_model to be used.
      WsfStringId mClutterType;
   };

   //! A 'mode' of the sensor.
   class WSF_EXPORT RadarMode : public WsfSensorMode
   {
   public:
      RadarMode();
      RadarMode(const RadarMode& aSrc);
      RadarMode& operator=(const RadarMode& aRhs);
      ~RadarMode() override;

      // WsfSensorMode methods.
      size_t         GetBeamCount() const override { return mBeamList.size(); }
      WsfSensorBeam* GetBeamEntry(size_t aIndex) override { return mBeamList[aIndex]; }

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      double GetAltFreqSelectDelay() const override { return mAltFreqSelectDelay; }
      void   ScheduleAltFreqChange(double aSimTime, int aAltFreqId = -1) override;
      bool   IsAltFreqChangeScheduled() const override { return mAltFreqChangeScheduled; }
      double GetLastAltFreqSelectTime() const override { return mLastAltFreqSelectTime; }
      bool   IsFrequencyAgile() const override { return mIsFrequencyAgile; }

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      bool                    mCanTransmit;
      bool                    mCanReceive;
      bool                    mImplicitBeamUsed;
      bool                    mExplicitBeamUsed;
      std::vector<RadarBeam*> mBeamList;

   protected:
      void SelectAlternateFrequency(double aSimTime, int aAltFreqId = -1) override;

      double mAltFreqSelectDelay;
      bool   mAltFreqChangeScheduled;
      double mLastAltFreqSelectTime;
      bool   mIsFrequencyAgile;
   };

private:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<RadarMode*> mRadarModeList;

   //! The list of active transmitters
   std::vector<WsfEM_Xmtr*> mXmtrList;

   //! The list of active receivers
   std::vector<WsfEM_Rcvr*> mRcvrList;

   //! true if any mode can transmit (not valid until Initialize is called).
   bool mAnyModeCanTransmit;

   //! true if any mode can receive (not valid until Initialize is called).
   bool mAnyModeCanReceive;

   //! Temporary geometry platform pointer to be created and used as required for false target interactions.
   WsfPlatform* mTempGeometryPtr;
};

#endif
