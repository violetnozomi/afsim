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

#ifndef WSFOTHRADARSENSOR_HPP
#define WSFOTHRADARSENSOR_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "WsfDetectionProbabilityTable.hpp"
#include "WsfEM_Antenna.hpp"
class WsfEM_Clutter;
#include "WsfEM_Noise.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMTI_TableManager.hpp"
#include "WsfMarcumSwerling.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorSignalProcessor.hpp"

//! A specialization of WsfSensor that implements a simple radar.
class WSF_MIL_EXPORT WsfOTH_RadarSensor : public WsfSensor
{
public:
   WsfOTH_RadarSensor(WsfScenario& aScenario);
   WsfOTH_RadarSensor(const WsfOTH_RadarSensor& aSrc);
   WsfOTH_RadarSensor& operator=(const WsfOTH_RadarSensor&) = delete;

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
   class OTH_Beam;
   class OTH_Mode;
   friend class OTH_Beam;
   friend class OTH_Mode;

   //! A RadarBeam represents a beam in a radar system.
   //! A single-beam radar is represents by one such object.
   //! A multi-beam radar is represented by a vector of these objects.
   class OTH_Beam : public WsfSensorBeam
   {
   public:
      OTH_Beam();
      OTH_Beam(const OTH_Beam& aSrc);
      OTH_Beam& operator=(const OTH_Beam&) = delete;
      ~OTH_Beam() override;

      // WsfSensorBeam methods.
      WsfEM_Rcvr* GetEM_Rcvr() override { return mRcvrPtr.get(); }
      WsfEM_Xmtr* GetEM_Xmtr() override { return mXmtrPtr.get(); }

      bool ProcessInput(UtInput& aInput) override;

      void AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult);

      double GetAdjustmentFactor() const { return mAdjustmentFactor; }
      double GetIntegrationGain() const override { return mIntegrationGain; }
      void   SetIntegrationGain(double aIntegrationGain) override { mIntegrationGain = aIntegrationGain; }
      int    GetNumberOfPulsesIntegrated() const override { return mNumberOfPulsesIntegrated; }
      bool   UsingDetector() const { return mUseDetector; }

      double GetDopplerResolution() const { return mDopplerResolution; }

      //! For External Services.
      WsfMTI_TableManager::MTI_AdjustmentTable* GetMTI_AdjustmentTable() { return mMTI_AdjustmentTablePtr.get(); }

      bool AreIonosphericParametersValid();

      void ComputeIonosphericCharacteristics();

      void UpdateIonosphericCharacteristics();

      void GetReflectionLocationWCS(double aReflectionLocationWCS[]);

      bool GetIonosphericRangeConstrained() const { return mIonosphericCharacteristics.mRangeConstrained; }

      double GetMinUsableFrequency() const { return mIonosphericCharacteristics.mMinUsableFrequency; }

      void GetIonosphericRangeLimits(double& aMin, double& aMax);

      bool GetIonosphereChanged() const { return mIonosphereChanged; }

      void SetIonosphereChanged(bool aIonosphereChanged) { mIonosphereChanged = aIonosphereChanged; }

      void SetLoopGain(double aLoopGain) { mLoopGain = aLoopGain; }

      void SetClutterAttenuationFactor(double aFactor) { mClutterAttenuationFactor = aFactor; }

      // double ComputeClutterAttenuation();

      bool Initialize(double       aSimTime,
                      unsigned int aBeamIndex,
                      WsfSensor*   aSensorPtr,
                      OTH_Mode*    aModePtr,
                      bool         aCanTransmit,
                      bool         aCanReceive,
                      bool         aShowCalibrationData,
                      bool         aIsMultiBeam);

      void ReadTable(UtInput& aInput, WsfMTI_TableManager::MTI_AdjustmentTable& aMTI_AdjustmentTable);

      void ComputeMTI_Effects(WsfSensorResult& aResult, double& aClutterAttenuationFactor);

      WsfStringId    GetClutterType() const { return mClutterType; }
      WsfEM_Clutter* GetClutter() const override { return mClutterPtr; }

      std::unique_ptr<WsfEM_Antenna> mAntennaPtr;

      //! The transmitter for a radar beam.
      std::unique_ptr<WsfEM_Xmtr> mXmtrPtr;

      //! The receiver for a radar beam.
      std::unique_ptr<WsfEM_Rcvr> mRcvrPtr;

      //! The pointer to the clutter model (after initialization).
      //! This will be zero if no clutter model has been selected.
      WsfEM_Clutter* mClutterPtr;

      //! The galactic noise
      WsfEM_Noise mNoise;

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

      void ComputeClutterAttenuationFactor(OTH_Mode& aMode);

      double ComputeIntegratedPulseCount(OTH_Mode& aMode);

      bool InitializeDetector(OTH_Mode& aMode);
      bool InitializeMTI(OTH_Mode& aMode);

      void ComputeBouncePathRanges(WsfSensorResult& aResult, double& aXmtrToTgt, double& aRcvrToTgt);

      bool CanBounceToTarget(WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

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

      //! Pointer to the MTI adjustment table.
      //! If this is non-null then it will be used for adjusting the SNR based on closing speed.
      std::shared_ptr<WsfMTI_TableManager::MTI_AdjustmentTable> mMTI_AdjustmentTablePtr;

      //! The MTI table will not be applied (no attenuation) when the target is this far away.
      // ! Not to be confused with the MTI processing specific variable
      double mMTI_TableMaxRange;

      //! Clutter Attenuation Factor.
      //! A values between [0..1] which indicates the amount of clutter that gets through.
      //! (0 being totally attenuated, 1 being totally passed). This can represent the effects
      //! of clutter mapping, MTI, Doppler, or any other processing technique.
      double mClutterAttenuationFactor;

      //! The clutter_model to be used.
      WsfStringId mClutterType;

      //! @name MTI processing parameters
      //@{
      double mMTI_MaxRange; //!< MTI maximum range
      double mMTI_InterpulsePeriod_1;
      double mMTI_InterpulsePeriod_2;
      double mMTI_StabilityConstant;
      double mMTI_Weights[3];
      int    mMTI_NumberOfStages;
      bool   mPerformMTI_Processing; //!< 'true' if MTI processing should be performed
      bool   mMTI_ClutterLock;
      bool   mMTI_UpWind;
      //@}

      //! Data container to contain geometry data
      struct OTH_Geometry
      {
         double mAngularExtent;
         double mReflectionLocWCS[3];
         double mReflectionHeight;
         double mGreatCircleDistance;
      };

      struct SolarCharacteristics
      {
         int    mDayOfYear = 1;
         int    mHourOfDay = 12;
         double mSolarAngRads;
         double mSolarDeclinationAngRads;
         double mSolarZenithAngRads;
      };

      struct IonosphereCharacteristics
      {
         double mElectronDensityAtMax = 4.0e11;
         double mElectronHeightAtMax  = 250000.0;
         double mReflectionHeight     = 300000.0;
         double mTemperatureAtMax     = 1540.0;
         bool   mRangeConstrained     = false;

         double mComputedElectronicDensity;
         double mMaxIncidenceAngleDegrees;
         double mMinRange;
         double mMaxRange;
         double mCriticalFrequency;
         double mMaxUsableFrequency;
         double mMinUsableFrequency;
      };

      // ionospheric variables
      SolarCharacteristics      mSolarCharacteristics;
      IonosphereCharacteristics mIonosphericCharacteristics;
      OTH_Geometry              mOTH_Geometry;
      bool                      mIonosphereChanged;
      bool                      mComputeAtmosphericNoise;
      bool                      mDebug;
   };

   //! A 'mode' of the sensor.
   class OTH_Mode : public WsfSensorMode
   {
   public:
      OTH_Mode();
      OTH_Mode(const OTH_Mode& aSrc);
      OTH_Mode& operator=(const OTH_Mode&) = delete;
      ~OTH_Mode() override;

      // WsfSensorMode methods.
      size_t         GetBeamCount() const override { return mBeamList.size(); }
      WsfSensorBeam* GetBeamEntry(size_t aBeamIndex) override { return mBeamList[aBeamIndex]; }

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

      double GetAltFreqSelectDelay() const override { return mAltFreqSelectDelay; }
      void   ScheduleAltFreqChange(double aSimTime, int aAltFreqId = -1) override;
      bool   IsAltFreqChangeScheduled() const override { return mAltFreqChangeScheduled; }
      double GetLastAltFreqSelectTime() const override { return mLastAltFreqSelectTime; }
      bool   IsFrequencyAgile() const override { return mIsFrequencyAgile; }

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      bool                   mOverrideMeasurementWithTruth;
      bool                   mCanTransmit;
      bool                   mCanReceive;
      bool                   mImplicitBeamUsed;
      bool                   mExplicitBeamUsed;
      std::vector<OTH_Beam*> mBeamList;

   protected:
      void SelectAlternateFrequency(double aSimTime, int aAltFreqId = -1) override;

      double mAltFreqSelectDelay;
      bool   mAltFreqChangeScheduled;
      double mLastAltFreqSelectTime;
      bool   mIsFrequencyAgile;
   };

private:
   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<OTH_Mode*> mOTH_ModeList;

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
