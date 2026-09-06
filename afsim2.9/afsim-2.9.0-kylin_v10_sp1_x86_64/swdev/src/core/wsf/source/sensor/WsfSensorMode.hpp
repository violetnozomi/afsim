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

#ifndef WSFSENSORMODE_HPP
#define WSFSENSORMODE_HPP

#include "wsf_export.h"

#include <memory>

#include "UtCloneablePtr.hpp"
#include "WsfMode.hpp"
#include "WsfSensor.hpp"
class WsfSensorBeam;
class WsfSensorErrorModelBase;
class WsfSensorModeList;
class WsfSensorResult;
#include "WsfStringId.hpp"
class WsfTrack;

//! A specialization of WsfMode for sensors.
//! A sensor mode represents a particular set of operating characteristics for
//! a sensor. For instance, a radar may have search, acquire and track modes.
class WSF_EXPORT WsfSensorMode : public WsfMode
{
   friend class WsfSensorModeList;

public:
   WsfSensorMode();
   WsfSensorMode(const WsfSensorMode& aSrc);
   WsfSensorMode& operator=(const WsfSensorMode& aRhs);
   ~WsfSensorMode() override;

   //! Return the pointer to the platform to which the associated sensor is attached.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Return the pointer to the sensor with which this mode is associated.
   WsfSensor* GetSensor() const { return mSensorPtr; }

   const WsfScenario& GetScenario() const;

   WsfSimulation* GetSimulation() const;

   //! Create an exact duplicate of this mode.
   WsfMode* Clone() const override = 0;

   virtual bool Initialize(double aSimTime);

   bool ProcessInput(UtInput& aInput) override;

   //! @name Beam access methods.
   //! These are optional methods that a sensor MAY provide to access beams.
   //! A given sensor model may or may not support beams. Some derived versions of
   //! WsfSensorComputer make use of these methods to gain access to details about
   //! the beam.
   //@{
   virtual size_t         GetBeamCount() const;
   virtual WsfSensorBeam* GetBeamEntry(size_t aIndex);
   //@}

   //! Attempt to detect the specified target.
   //! @param aSimTime   [input] The current simulation time.
   //! @param aTargetPtr [input] The platform that will be attempted to be detected.
   //! @param aSettings  [input] The sensor settings.
   //! @param aResult    [output] The results of the detection attempt.
   //! @returns 'true' if the target was detected or 'false' if it was not detected.
   virtual bool AttemptToDetect(double               aSimTime,
                                WsfPlatform*         aTargetPtr,
                                WsfSensor::Settings& aSettings,
                                WsfSensorResult&     aResult) = 0;

   virtual double GetAltFreqSelectDelay() const { return -1.0; }
   virtual void   ScheduleAltFreqChange(double aSimTime, int aAltFreqId = -1) {}
   virtual bool   IsAltFreqChangeScheduled() const { return false; }
   virtual double GetLastAltFreqSelectTime() const { return 0.0; }

   virtual bool IsFrequencyAgile() const { return false; }

   //! The mode is being deselected.
   //! The specific implementation should perform whatever actions are associated with deselecting the mode
   //! (e.g.: deactivating transmitters or receivers).
   //! @param aSimTime   [input] The current simulation time.
   void Deselect(double aSimTime) override = 0;

   //! The mode is being activated.
   //! The specific implementation should perform whatever actions are associated with selecting the mode
   //! (e.g.: activating transmitters or receivers).
   //! @param aSimTime   [input] The current simulation time.
   void Select(double aSimTime) override = 0;

   virtual void ApplyMeasurementErrors(WsfSensorResult& aResult);

   WSF_DEPRECATED virtual void ComputeMeasurementErrors(WsfSensorResult& aResult,
                                                        double&          aAzError,
                                                        double&          aElError,
                                                        double&          aRangeError,
                                                        double&          aRangeRateError) final;

   virtual void InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr);

   virtual void UpdateSensorCueingLimits();

   virtual void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   virtual void UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr);

   virtual bool WithinDetectionRange(double aSimTime, WsfPlatform* aTargetPtr);

   //! Return the optimistic maximum range of the sensor mode.
   double GetMaximumRange() const { return mMaximumRange; }

   //! Return the maximum number of cued-search or tracking requests the mode can support.
   int GetMaximumRequestCount() const { return mMaximumRequestCount; }

   //! Does using this mode prevent this or any other mode from searching?
   bool DisablesSearch() const { return mDisablesSearch; }

   //! Can this mode continue to search while it is also performing tracking requests.
   bool CanSearchWhileTrack() const { return mCanSearchWhileTrack; }

   double GetFrameTime() const { return mFrameTime; }
   void   SetFrameTime(double aFrameTime) { mFrameTime = aFrameTime; }
   double GetRevisitTime() const { return mRevisitTime; }
   double GetDwellTime() const { return mDwellTime; }

   //! Return the Pd required for detection.
   //! This is only meaningful for sensors that do not employ binary (0/1) detectors.
   double GetRequiredPd() const { return mRequiredPd; }

   //! Return the probability that a track will actually be established given than the M/N establish
   //! track criteria has been met.
   double EstablishTrackProbability() const { return mEstablishTrackProbability; }

   //! Return the probability that a track will actually be maintained given that the M/N maintain
   //! track criteria has been met.
   double MaintainTrackProbability() const { return mMaintainTrackProbability; }

   //! The number of hits (M) within the window (N) that are required to establish a track.
   int HitsToEstablishTrack() const { return mHitsToEstablishTrack; }

   //! The window size (N) for establishing a track.
   int EstablishTrackWindow() const { return mEstablishTrackWindow; }

   //! The number of hits (M) within the window (N) that are required to maintain a track.
   int HitsToMaintainTrack() const { return mHitsToMaintainTrack; }

   //! The window size (N) for maintaining a track.
   int MaintainTrackWindow() const { return mMaintainTrackWindow; }

   //! A bit mask that defines the track establish window.
   //! If the window is N detection opportunities, then the mask will have bits 2^0 through 2^(N-1) on.
   unsigned int EstablishTrackMask() const { return mEstablishTrackMask; }

   //! A bit mask that defines the track maintain window.
   //! If the window is N detection opportunities, then the mask will have bits 2^0 through 2^(N-1) on.
   unsigned int MaintainTrackMask() const { return mMaintainTrackMask; }

   double GetTrackQuality() const { return mTrackQuality; }

   //! Accessors for the reporting flags
   //@{
   unsigned int GetReportingFlags() const { return mReportingFlags.mFlags; }

   bool ReportsLocation() const { return mReportingFlags.mReportsLocation; }
   bool ReportsVelocity() const { return mReportingFlags.mReportsVelocity; }
   bool ReportsRange() const { return mReportingFlags.mReportsRange; }
   bool ReportsBearing() const { return mReportingFlags.mReportsBearing; }
   bool ReportsElevation() const { return mReportingFlags.mReportsElevation; }
   bool ReportsRangeRate() const { return mReportingFlags.mReportsRangeRate; }
   bool ReportsSide() const { return mReportingFlags.mReportsSide; }
   bool ReportsType() const { return mReportingFlags.mReportsType; }
   bool ReportsIFF_Status() const { return mReportingFlags.mReportsIFF_Status; }
   bool ReportsSignalToNoise() const { return mReportingFlags.mReportsSignalToNoise; }
   bool ReportsFrequency() const { return mReportingFlags.mReportsFrequency; }
   bool ReportsPulseWidth() const { return mReportingFlags.mReportsPulseWidth; }
   bool ReportsPulseRepititionInterval() const { return mReportingFlags.mReportsPulseRepititionInterval; }
   //@}

   //! @name Sensor error model methods.
   //@{
   const wsf::SensorErrorModelBase* GetErrorModel() const { return mErrorModelPtr.get(); }
   wsf::SensorErrorModelBase*       GetErrorModel() { return mErrorModelPtr.get(); }
   void                             SetErrorModel(std::unique_ptr<wsf::SensorErrorModelBase> aErrorModelPtr)
   {
      mErrorModelPtr = std::move(aErrorModelPtr);
   }
   //@}

   //! @name Sensor mode error access methods.
   //@{
   WSF_DEPRECATED virtual double GetAzErrorStdDev() final { return mAzErrorStdDev; }
   WSF_DEPRECATED virtual double GetElErrorStdDev() final { return mElErrorStdDev; }
   WSF_DEPRECATED virtual double GetRangeErrorStdDev() final { return mRangeErrorStdDev; }
   WSF_DEPRECATED virtual double GetRangeRateErrorStdDev() final { return mRangeRateErrorStdDev; }
   WSF_DEPRECATED virtual void   SetAzErrorStdDev(double aAzErrorStdDev) final;
   WSF_DEPRECATED virtual void   SetElErrorStdDev(double aElErrorStdDev) final;
   WSF_DEPRECATED virtual void   SetRangeErrorStdDev(double aRangeErrorStdDev) final;
   WSF_DEPRECATED virtual void   SetRangeRateErrorStdDev(double aRangeRateErrorStdDev) final;
   //@}

   //! @name Automatic mode switch control.
   //@{

   //! Return the mode list index of the mode to be used after declaring a track or a successful mode switch;
   //! @return The mode list index of the next mode to be used. This will either be the index for the new mode
   //! or the index for the current mode (the value of GetModeIndex()) if 'on_success' has not been defined.
   //! @note This is valid only after Initialize() has been called.
   size_t GetOnSuccessModeIndex() const { return mOnSuccessModeIndex; }

   //! Return the mode list index of the mode to be used after dropping a track.
   //! @return The mode list index of the next mode to be used. This will either be the index for the new mode
   //! or the index for the current mode (the value of GetModeIndex()) if 'on_failure' has not been defined.
   //! @note This is valid only after Initialize() has been called.
   size_t GetOnFailureModeIndex() const { return mOnFailureModeIndex; }
   //@}

   //! Return the logical length (int bits) of report messages that originate from this sensor mode.
   int GetMessageLength() const { return mMessageLength; }

   //! Return the priority of report messages that originate from this sensor mode.
   int GetMessagePriority() const { return mMessagePriority; }

   // This was formerly protected but needed to be made accessible to sensor components.
   void UpdateTrackPosition(WsfTrack* aTrackPtr, WsfSensorResult& aResult, const double aTgtLocationWCS[3]);

   WsfSensor::SlewMode GetCueMode() const { return mCueMode; }

protected:
   //! This nested class 'SelectAltFrequencyEvent' is necessary so that
   //! the data necessary for this event is separate from WsfSensorEvent.
   class WSF_EXPORT AltFrequencyChangeEvent : public WsfEvent
   {
   public:
      AltFrequencyChangeEvent(double aSimTime, WsfSensorMode* aSensorModePtr, int aAltFrequencyId = -1);

      EventDisposition Execute() override;

   private:
      size_t         mPlatformIndex;  //!< The platform index of the source platform.
      WsfSensorMode* mSensorModePtr;  //!< The sensor mode pointer.
      int            mAltFrequencyId; //!< The ID of the alternate frequency to select.
                                      // TODO-UNUSED unsigned int   mUpdateEventEpoch;
   };

   bool InitializeReporting();

   virtual void SelectAlternateFrequency(double aSimTime, int aAltFreqId = -1) {}

   //! Pointer to the sensor with which the mode is associated.
   WsfSensor* mSensorPtr;
   //! Pointer to the platform with which the sensor is associated.
   WsfPlatform* mPlatformPtr;

   //! A very optimistic estimate of the maximum range at which this mode can detect a target.
   //! This is ONLY used for performing initial detection culling.
   //! The default is std::numeric_limits<double>::max() and can be overridden by the sensor.
   double mMaximumRange;

   //! Mode-specific cueing rates and limits.
   //! These limits are in addition the slew rates and limits of the sensor. They are typically used
   //! to disable or otherwise limit cueing in a particular direction.
   //@{
   double              mAzCueRate;
   double              mElCueRate;
   double              mMinAzCue;
   double              mMaxAzCue;
   double              mMinElCue;
   double              mMaxElCue;
   WsfSensor::SlewMode mCueMode;
   //@}

   //! The maximum number of cued search/track requests.
   //! A pure search mode will have a maximum request count of 0.
   int mMaximumRequestCount;

   //! Can this mode continue to search while it is also tracking?
   //! This has applicability only if this mode has tracking capabilities (maximum_request_count > 0).
   //! If 'true' then this mode can perform search attempts while it is also performing tracking requests.
   //! If 'false' then submitting any tracking request to the mode will prevent it from performing search
   //! requests.
   //!
   //! This value is forced to 'true' if maximum_request_count = 0.
   bool mCanSearchWhileTrack;

   //! Does this mode prevent another mode from searching.
   //! If 'true' then using this mode will prevent this or any other active mode from performing searches.
   //! If 'false' then search attempts are interleaved to the extent possible.
   //!
   //! This value is forced to 'false' if maximum_request_count = 0.
   bool mDisablesSearch;

   double mFrameTime;
   double mRevisitTime;
   double mDwellTime;

   //! The required probability for detection.
   //! The default value is -1.0, which indicates the standard 1.0 - GetSimulation()->GetRandom().Uniform() is to be
   //! used. If greater than or equal to zero then this is the fixed required Pd required for detection.
   double mRequiredPd;

   //! @name Track establishment and maintain window definition.
   //@{

   //! When the M/N establish track criteria is met, this is the probability that a track will be established.
   double mEstablishTrackProbability;

   //! As long as the M/N maintain track criteria is met, this is the probability that the track will
   //! be maintained.
   double mMaintainTrackProbability;

   //! The number of hits (M) within the window (N) that are required to establish a track.
   int mHitsToEstablishTrack;
   //! The window size (N) for establishing a track.
   int mEstablishTrackWindow;
   //! A bit mask that defines the track establish window.
   //! If the window is N detection opportunities, then the mask will have bits 2^0 through 2^(N-1) on.
   unsigned int mEstablishTrackMask;

   //! The number of hits (M) within the window (N) that are required to maintain a track.
   int mHitsToMaintainTrack;
   //! The window size (N) for maintaining a track.
   int mMaintainTrackWindow;
   //! A bit mask that defines the track maintain window.
   //! If the window is N detection opportunities, then the mask will have bits 2^0 through 2^(N-1) on.
   unsigned int mMaintainTrackMask;

   //@}

   //! Base line track quality (0..1]
   double mTrackQuality;

   //! @name Sensor error parameters.
   //@{
   //! Azimuth error standard deviation.
   //! If positive then it is in radians. If negative then it is the negative 'percent_of_true_range' specification.
   double mAzErrorStdDev;
   //! Elevation error standard deviation.
   //! If positive then it is in radians. If negative then it is the negative 'percent_of_true_range' specification.
   double mElErrorStdDev;
   //! Range error standard deviation.
   //! If positive then it is in meters. If negative then it is the negative 'percent_of_true_range' specification.
   double mRangeErrorStdDev;
   //! Range rate error standard deviation.
   double mRangeRateErrorStdDev;
   //@}
   //! @note These have been moved into WsfStandardSensorErrorModel and will be removed in a future release.

   //! The model that calculates and applies errors to sensor detection results.
   ut::CloneablePtr<wsf::SensorErrorModelBase> mErrorModelPtr{nullptr};

   //! The name (ID) of the next mode on successful acquisition.
   //! The value is zero if no automatic switch is to be performed.
   WsfStringId mOnSuccessModeName;
   size_t      mOnSuccessModeIndex;

   //! The name (ID) of the next mode on lost of track.
   //! The value is zero if no automatic switch is to be performed.
   WsfStringId mOnFailureModeName;
   size_t      mOnFailureModeIndex;

   //! The length of messages originating from this sensor mode.
   int mMessageLength;
   //! The priority of messages originating from this sensor mode.
   int mMessagePriority;

   //! The spatial domain for tracks reported by this sensor mode.
   //! If this value is WSF_SPATIAL_DOMAIN_UNKNOWN then it will report the spatial domain of the platform.
   WsfSpatialDomain mReportsSpatialDomain;

   //! The following are flags/masks meant to be combined and used in the SetCapabilities method.
   //{@
   static const unsigned int cNONE                      = 0;
   static const unsigned int cLOCATION                  = 1;
   static const unsigned int cVELOCITY                  = 2;
   static const unsigned int cRANGE                     = 4;
   static const unsigned int cBEARING                   = 8;
   static const unsigned int cELEVATION                 = 16;
   static const unsigned int cRANGE_RATE                = 32;
   static const unsigned int cSIDE                      = 64;
   static const unsigned int cTYPE                      = 128;
   static const unsigned int cIFF_STATUS                = 256;
   static const unsigned int cSIGNAL_TO_NOISE           = 512;
   static const unsigned int cFREQUENCY                 = 1024;
   static const unsigned int cPULSEWIDTH                = 2048;
   static const unsigned int cPULSE_REPITITION_INTERVAL = 4096;
   static const unsigned int cSTATE_COVARIANCE          = 8192;
   static const unsigned int cOTHER                     = 16384; //! Catch-all for anything other than the above.
   static const unsigned int cALL                       = 0xFFFFFFFF;
   //@}

   struct ReportingFlags
   {
      ReportingFlags(unsigned int aReportingFlags)
         : mFlags(aReportingFlags)
      {
      }
      ReportingFlags()
         : mFlags(cALL)
      {
      }

      void Set(unsigned int aFlags) { mFlags = aFlags; }

      unsigned int Get() const { return mFlags; }

      union
      {
         struct
         {
            bool mReportsLocation : 1;
            bool mReportsVelocity : 1;
            bool mReportsRange : 1;
            bool mReportsBearing : 1;
            bool mReportsElevation : 1;
            bool mReportsRangeRate : 1;
            bool mReportsSide : 1;
            bool mReportsType : 1;
            bool mReportsIFF_Status : 1;
            bool mReportsSignalToNoise : 1;
            bool mReportsFrequency : 1;
            bool mReportsPulseWidth : 1;
            bool mReportsPulseRepititionInterval : 1;
            // Catch-all for anything other than the above.
            // Note:  A derived class must set this flag if none of the other flags apply.
            bool mReportsOther : 1;
         };
         unsigned int mFlags;
      };
   };

   void SetCapabilities(unsigned int aFlags) { mCapabilitiesFlags = aFlags; }
   void SetReportingFlags(unsigned int aFlags) { mReportingFlags = aFlags; }

   ReportingFlags mReportingFlags;
   ReportingFlags mCapabilitiesFlags;
   //@}
};

#endif
