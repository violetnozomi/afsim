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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFSAR_SENSOR_HPP
#define WSFSAR_SENSOR_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtMatrix.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfStandardSensorErrorModel.hpp"
#include "script/WsfScriptSensorClass.hpp"

//! A specialization of WsfSensor that implements a synthetic aperture radar (SAR) sensor.
//!
//! This model provides a basic implementation of SAR 'spot' and 'strip' operating modes.
//!
//! The implementation for spot is as follows:
//!
//! - When the sensor is turned on, every platform is evaluated to determine if it is in the 'spot'
//!   (ignoring terrain).  If so, it is added to the sensing chance queue.  Note that this implies
//!   only those platforms that are in the spot when the SAR is turned on can be in the resulting image.
//! - At every 'frame_time' sample during the image formation time, each of the potential platforms
//!   will be sampled to see if it can be detected.
//! - When the sensor is turned off the resulting 'image' will be produced. Every platform that
//!   successfully passed every sensing chance will be put in to the 'image'.
class WSF_MIL_EXPORT WsfSAR_Sensor : public WsfSensor
{
public:
   WsfSAR_Sensor(WsfScenario& aScenario);
   ~WsfSAR_Sensor() override;

   bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;
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
   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;

   const char* GetScriptClassName() const override { return "WsfSAR_Sensor"; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   enum FailureReasons
   {
      cFR_FIELD_OF_VIEW_EXCEEDED        = 1,  //!< The area is outside the field of view
      cFR_BEYOND_HORIZON                = 2,  //!< The area is beyond the radar horizon
      cFR_DWELL_TIME_LIMIT_EXCEEDED     = 4,  //!< The dwell time limit was exceeded
      cFR_AMBIGUOUS_RANGE               = 8,  //!< The range would be ambiguous
      cFR_DOPPLER_FOLDOVER              = 16, //!< The Doppler frequency returns would be ambiguous
      cFR_INSUFFICIENT_CLUTTER_TO_NOISE = 32, //!< Clutter-to-Noise ratio is too low
      cFR_INSUFFICIENT_RESOLUTION       = 64  //!< The desired resolution cannot be achieved
   };

   bool CanImageNow(double  aSimTime,
                    double& aDwellTime,
                    double& aResolution,
                    double& aCNR,
                    double& aDopplerMargin,
                    int&    aFailureReasons);

   double GetDwellTime() const;
   void   SetDwellTime(double aDwellTime);

   double GetResolution() const;
   void   SetResolution(double aResolution);

   //! Return the clutter to noise from the last spot SAR request.
   //! @returns The achieved clutter-to-noise as a linear (not dB) value.
   double GetAchievedCNR() const { return mAchievedCNR; }

   //! Return the approximate achieved resolution from the last SAR request.
   //! @returns The length of one side of the resolution cell (meters).
   //! For a non-square cell this will be sqrt(x * y)
   double GetAchievedResolution() const { return mAchievedAzimuthResolution; }

   //! Return the current 'event number' for automatic turn off processing.
   //! This is an internal method.
   unsigned int GetTurnOffEventNumber() const { return mTurnOffEventNumber; }

   double GetBackscatterCoefficient() const { return mBackscatterCoefficient; }

   // Some compilers require inner classes to be declared friends if they want to access outer class members
   class SAR_Chance;
   friend class SAR_Chance;
   class SAR_Mode;
   friend class SAR_Mode;

   //! Data container to contain geometry data
   struct Geometry
   {
      //! The ground speed of the sensing platform.
      double mGroundSpeed;

      //! The altitude of the sensor.
      double mAltitude;

      //! The range from the sensing platform to the center-of-interest.
      double mSlantRange;

      //! The depression angle.
      double mDepressionAngle;

      //! The squint angle (horizontal angle between the sensing platforms' velocity vector
      //! and the line-of-sight vector to the center-of-interest.
      double mSquintAngle;

      //! The absolute value of the total angle between the sensing platforms' velocity vector
      //! and the line-of-sight vector to the center-of-interest.
      double mTotalAngle;

      //! The angle between the line of sight vector and the tangent plane at the center-of-interest.
      double mGrazingAngle;

      //! For electronically steered systems, this is the angle between the normal to the antenna face and
      //! the line-of-sight vector to the center of interest.
      double mScanAngle;
   };

   //! A detection opportunity for a specific platform
   class SAR_Chance
   {
   public:
      SAR_Chance(WsfPlatform* aTargetPtr);
      ~SAR_Chance();

      WsfTrack*    mTrackPtr;
      double       mSignalSum;
      double       mAreaSum;
      double       mLocationWCS[3];
      double       mComputedRangeError;
      double       mComputedDopplerError;
      size_t       mTargetIndex;
      unsigned int mNumberOfDetections;
      bool         mNowDetecting;
   };

   //! The 'mode' of the sensor.
   class SAR_Mode : public WsfSensorMode, public WsfSensorBeam
   {
   public:
      enum OperatingMode
      {
         cOM_SPOT,
         cOM_STRIP
      };

      SAR_Mode();
      SAR_Mode(const SAR_Mode& aSrc);
      ~SAR_Mode() override = default;
      SAR_Mode& operator=(const SAR_Mode&) = delete;

      // WsfSensorMode methods.
      size_t         GetBeamCount() const override { return 1; }
      WsfSensorBeam* GetBeamEntry(size_t /* aBeamIndex */) override { return this; }

      // WsfSensorBeam methods.
      WsfEM_Rcvr* GetEM_Rcvr() override { return mRcvrPtr.get(); }
      WsfEM_Xmtr* GetEM_Xmtr() override { return mXmtrPtr.get(); }

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      void Calibrate();

      class SAR_ErrorModel : public wsf::StandardSensorErrorModel
      {
      public:
         SAR_ErrorModel();
         SAR_ErrorModel(const SAR_ErrorModel& aSrc);
         ~SAR_ErrorModel() override = default;

         bool            ProcessInput(UtInput& aInput) override;
         bool            Initialize(WsfSensorMode* aSensorModePtr) override;
         SAR_ErrorModel* Clone() const override { return new SAR_ErrorModel(); }

         std::unique_ptr<wsf::SphericalMeasurementErrors> ComputeMeasurementErrors(WsfSensorResult& aResult) const override;
         void ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;
         void ApplySAR_MeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors, SAR_Chance& aChance);
         const UtMatrixd* GetErrorCovarianceMatrix() const { return &mCovarianceMatrix; }

      private:
         void ApplyRangeDopplerErrors(WsfEM_Antenna* aAntennaPtr, WsfPlatform* aTargetPtr, SAR_Chance* aChancePtr);

         void ComputeRangeDopplerErrors(double             aSimTime,
                                        const WsfPlatform* aTargetPtr,
                                        double&            aRangeError,
                                        double&            aDopplerError) const;

         //! Flag to trip error computation and application
         bool mComputeRangeDopplerErrors{false};
         //! Array containing user defined sensor errors
         double mSAR_Errors[15];

         //! Array of booleans used to alert user of uninitialized errors
         bool mSAR_ErrorsSet[15];

         mutable UtMatrixd                     mErrorsSqrd;
         mutable UtMatrixd                     mCovarianceMatrix;
         mutable ut::CloneablePtr<WsfPlatform> mTempPlatformPtr{nullptr};
         WsfEM_Antenna*                        mAntennaPtr{nullptr};
      };

      void ComputeGeometry(double aSimTime, Geometry& aGeometry);

      double ComputeCNR(const Geometry& aGeometry, double aPRF, double aDwellTime, double aResolution);

      double ComputeDwellTime(const Geometry& aGeometry, double aResolution);

      // double ComputePixelCount(double aRange,
      //                          double aOpticalCrossSection);

      double ComputePRF(const Geometry& aGeometry);

      double ComputeAzimuthResolution(const Geometry& aGeometry, double aDwellTime);

      double ComputeGroundRangeResolution(const Geometry& aGeometry);

      bool ComputeImageCenter(double aLocationWCS_1[3], double aLocationWCS_2[3], bool aFirstPass = false);

      void ComputeFOV(const Geometry& aGeometry);

      double ComputeSlantRange(double aAltitude, double aDepressionAngle, double& aGrazingAngle);

      double ComputeSquintAngle(WsfPlatform* aTargetPtr, double& aSlantRange);

      void ComputeUpdateInterval(double aSimTime);

      void PredictPerformance(const Geometry& aGeometry,
                              double&         aPRF,
                              double&         aDwellTime,
                              double&         aAzimuthResolution,
                              double&         aCNR,
                              double&         aMinimumPRF);

      bool WithinUnambiguousRange(WsfSensorResult& aResult);

      void SetIntegrationGain(double aIntegrationGain) override { mIntegrationGain = aIntegrationGain; }

      //! The pointer to the derived sensor class.
      WsfSAR_Sensor* mSAR_SensorPtr;

      //! The pointer to the derived error model class.
      SAR_ErrorModel* mSAR_ErrorModelPtr;

      //! The operating mode (spot, strip, etc.)
      OperatingMode mOperatingMode;

      //! The Antenna for the Mode
      std::unique_ptr<WsfEM_Antenna> mAntennaPtr;

      //! The transmitter for a Mode.
      std::unique_ptr<WsfEM_Xmtr> mXmtrPtr;

      //! The receiver for a Mode.
      std::unique_ptr<WsfEM_Rcvr> mRcvrPtr;

      //! Doppler filter broadening factor.
      double mKa;

      //! Doppler fold-over  margin factor (also called the over-collect ratio).
      double mKd;

      //! Desired azimuth resolution
      double mResolution;

      //! Desired or computed dwell/integration time.
      double mDwellTime;

      //! Maximum dwell/integration
      double mMaximumDwellTime;

      //! Minimum allowable clutter-to-noise limit
      double mMinimumCNR;

      //! The speed above which an object cannot be seen.
      double mMaxDetectableSpeed;

      //! The current azimuth resolution.
      double mCurrentAzimuthResolution;

      //! The current ground range resolution.
      double mCurrentGroundRangeResolution;

      //! Additional data to represent the sensor more simply
      //! (like in WsfRadarSensor).
      //@{

      //! The angular resolution of a pixel (radians).
      double mAngularResolution;

      //! Integration gain for non-Swerling detector.
      double mIntegrationGain;

      //! A term to represent a general adjustment (multiplier) to the radar range equation.
      double mAdjustmentFactor;

      //! If non-zero, this represents the 1 m^2 detection range.
      double mOneM2DetectRange;
      //@}

      //! For spot requests, compute dwell time necessary to achieve the required image resolution
      bool mComputeDwellTime;

      //! Compute the pulse repetition frequency.
      bool mComputePRF;

      //! For spot mode only, 'true' if the sensor is to be automatically turned off at the end
      //! of the image formation time.  If 'false' then the sensor must be turned off manually,
      //! and the sensor will continue to form the image.
      //!
      //! Note that the image is not delivered UNTIL the sensor is turned off!
      bool mAutomaticTurnOff;

      //! Inhibit collection if the computed dwell time is greater than the maximum allowable.
      bool mInhibitSubstandardCollection;

      //! Image height and width in meters; used for spot SAR
      double mImageHeight;
      double mImageWidth;

      //! The approximate image center.
      double mImageCenterWCS[3];
   };

private:
   WsfSAR_Sensor(const WsfSAR_Sensor& aSrc);
   WsfSAR_Sensor& operator=(const WsfSAR_Sensor&) = delete;

   void BuildAndSendImage(double aSimTime, SAR_Mode& aMode);

   void ClearSensingChances();

   void AddSensingChance(double aSimTime, const SAR_Mode& aMode, const Geometry& aGeometry, WsfPlatform* aTargetPtr);

   void ProcessModeChange(double aSimTime, size_t aOldModeIndex, size_t aNewModeIndex);

   void SpotModeBegin(double aSimTime, SAR_Mode& aMode, Geometry& aGeometry);

   void SpotModeEnd(double aSimTime, SAR_Mode& aMode);

   void SpotModeUpdate(double aSimTime, Settings& aSettings);

   void StripModeBegin(double aSimTime, SAR_Mode& aMode, Geometry& aGeometry);

   void StripModeEnd(double aSimTime, SAR_Mode& aMode);

   void StripModeUpdate(double aSimTime, Settings& aSettings);

   void NotifyObservers(double aSimTime, SAR_Mode* aModePtr, SAR_Chance* aChancePtr, WsfSensorResult& aResult, bool aDetected);

   //! The sensor-specific list of modes.
   std::vector<SAR_Mode*> mSAR_ModeList;

   //! The list of platforms that are eligible for detection attempts
   using Chances = std::vector<std::unique_ptr<SAR_Chance>>;
   Chances mChances;

   //! The stream number is incremented whenever the sensor is turned on or if there is
   //! a change in the type of image being produced (i.e. image -> spot)
   unsigned int mStreamNumber;

   //! The frame number is the number of an image within a stream of images.
   //! This will always be 0 for static (spot) SAR images.
   //! It will be 1 or greater for strip SAR images.
   unsigned int mFrameNumber;

   //! The surface backscatter coefficient (overrides the environment)
   double mBackscatterCoefficient;

   //! The minimum intensity represented in the image (watts)
   double mIntensityMin;

   //! The maximum intensity represented in the image (watts)
   double mIntensityMax;

   //! The range above the noise floor of the maximum intensity represented in the image.
   double mIntensityRange;

   //! The time when Update last did some real processing (not just enter/exit).
   double mLastUpdateTime;

   //! The simulation time when processing in the current mode was started.
   double mModeStartTime;

   //! The index of the mode that was in effect in the last call to Update.
   size_t mLastModeIndex;

   //! The event number for validation automatic turnoff events.
   unsigned int mTurnOffEventNumber;

   //! If 'true', and a pixel in a substandard image will be rescaled to maintain the aspect ratio of a pixel in
   //! a non-substandard image.
   bool mRescaleSubstandardImage;

   //! True if automatically turn off (and form image) at the end of the image formation time.
   //! This exists outside the 'mode' only for backward compatibility...
   bool mAutomaticTurnOff;

   //! true if 'SAR constraint plotting'.
   bool mSAR_ConstraintPlotting;

   //! If 'true', show resolution, dwell time and CNR data during turn-off/turn-on.
   bool mShowStatus;

   //! true if SensorTrack<XXX> observers should be called
   bool mCallSensorTrackObservers;

   //! Geometry at the start of the current spot or strip frame.
   Geometry mGeometry;

   //! The achieved clutter-to-noise ratio (absolute) from the last imaging request.
   double mAchievedCNR;

   //! The achieved azimuth resolution (m) from the last imaging request.
   double mAchievedAzimuthResolution;

   //! The achieved ground range resolution (m) from the last imaging request.
   double mAchievedGroundRangeResolution;

   //! A dummy platform used for computing the signal return from a resolution cell.
   WsfPlatform mTempPlatform;
};

// Script Accessor Class
class WSF_MIL_EXPORT WsfScriptSAR_SensorClass : public WsfScriptSensorClass
{
public:
   WsfScriptSAR_SensorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(GetDwellTime_1);
   UT_DECLARE_SCRIPT_METHOD(GetDwellTime_2);
   UT_DECLARE_SCRIPT_METHOD(SetDwellTime_1);
   UT_DECLARE_SCRIPT_METHOD(SetDwellTime_2);

   UT_DECLARE_SCRIPT_METHOD(GetResolution_1);
   UT_DECLARE_SCRIPT_METHOD(GetResolution_2);
   UT_DECLARE_SCRIPT_METHOD(SetResolution_1);
   UT_DECLARE_SCRIPT_METHOD(SetResolution_2);
};

#endif
