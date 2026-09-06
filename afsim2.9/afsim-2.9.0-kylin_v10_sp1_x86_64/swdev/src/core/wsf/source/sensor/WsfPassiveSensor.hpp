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

#ifndef WSFPASSIVESENSOR_HPP
#define WSFPASSIVESENSOR_HPP

#include "wsf_export.h"

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "TblLookup.hpp"
#include "UtInput.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
class WsfEM_Xmtr;
#include "WsfEmitterTypeReporting.hpp"
#include "WsfPassiveDetector.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"
#include "WsfStandardSensorErrorModel.hpp"
#include "WsfStringId.hpp"
#include "WsfTargetTypeReporting.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptSensorClass.hpp"

//! A specialization of WsfSensor that provides the baseline passive RF sensor.

class WSF_EXPORT WsfPassiveSensor : public WsfSensor
{
public:
   enum SignalType
   {
      cST_CONTINUOUS,
      cST_PULSED
   };

   // Define inner classes to be declared friends to access outer class members on some compilers
   class PassiveRcvr;
   class PassiveBeam;
   class PassiveMode;
   friend class PassiveRcvr;
   friend class PassiveBeam;
   friend class PassiveMode;

   //! An internal class used to keep track of interacting transmitters and receivers.
   class Interactor
   {
   public:
      Interactor() = delete;
      Interactor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr, bool aUseListener, bool aUseChangeListener);

      //! Detection status for the listener detection method.
      //! This is set to cDS_NO_ATTEMPT at the start of each frame and then updated asynchronously
      //! during the frame by the listener detection method.
      enum
      {
         cDS_NOT_ATTEMPTED   = 0, //!< No detection attempt was performed.
         cDS_DETECTED        = 1, //!< A detection attempt was successful.
         cDS_FAILED_GEOMETRY = 2, //!< Failed detection because of geometric constraints.
         cDS_FAILED_SIGNAL   = 3  //!< Failed detection because of insufficient received signal.
      };

      // NOTE: No destructor is defined even though there are pointers. This is just a place to store things.
      // It is left to others to delete things at the proper time.

      //! Pointer to the transmitter.
      WsfEM_Xmtr* mXmtrPtr = nullptr;
      //! Pointer to the receiver.
      PassiveRcvr* mRcvrPtr = nullptr;
      //! The pointer to the result from listening method of detection. (0 if a using polling or a
      //! listening method of detection is not being used.)
      WsfSensorResult* mResultPtr = nullptr;
      //! The time when the last listening detection was attempted (successful or unsuccessful).
      double mLastAttemptTime = 0.0;
      //! The cumulative Pd accumulated over multiple detection chances (for PSOS).
      double mPSOS_CumulativePd = 0.0;
      //! The detection status during the current frame. This is kept as an unsigned char rather than
      //! a enum because enums are 4 bytes on many machines, and this class can be instantiated A LOT.
      unsigned char mDetectStatus = cDS_NOT_ATTEMPTED;
      //! 'true' if the listening detection method should be used (vs. polling)
      bool mUseListener = false;
      //! 'true' if target signal updates are to be listened for and updated (vs. polling).
      bool mUseChangeListener = false;
   };

   using InteractorList = std::list<Interactor>;

   //! A map of targets and their interactors.
   //! The key is the platform index of the target and the value is the list of interactors.
   using TargetInteractorMap = std::map<size_t, InteractorList>;

   struct FrequencyBand
   {
   public:
      double mLowerFrequency = 0.0;
      double mUpperFrequency = 0.0;
      double mDwellTime      = 0.0;
      double mRevisitTime    = 0.0;
   };

   //! A specialized WsfEM_Rcvr that can receive on multiple frequency bands.
   class PassiveRcvr : public WsfEM_Rcvr
   {
   public:
      PassiveRcvr(WsfEM_Antenna* aAntennaPtr);
      PassiveRcvr(const PassiveRcvr& aSrc, WsfEM_Antenna* aAntennaPtr);

      bool Initialize(WsfSimulation& aSimulation) override;
      bool ProcessInput(UtInput& aInput) override;

      bool CanInteractWith(WsfEM_Xmtr* aXmtrPtr) override;
      bool AddInteractor(WsfEM_Xmtr* aXmtrPtr) override;
      bool RemoveInteractor(WsfEM_Xmtr* aXmtrPtr) override;

      void EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult) override;
      void SignalChangeCallback(double aSimTime, size_t aTargetIndex) override;

      bool TuneFrequencyBand(double       aSimTime,
                             bool         aReset,
                             unsigned int aFrequencyBandNumber,
                             double       aLowerFreq,
                             double       aUpperFreq,
                             double       aDwellTime,
                             double       aRevisitTime);

      double ComputeReceivedPower(double                    aSourceAz,
                                  double                    aSourceEl,
                                  double                    aEBS_Az,
                                  double                    aEBS_El,
                                  double                    aReceivedPowerDensity,
                                  WsfEM_Types::Polarization aPolarization,
                                  double                    aFrequency,
                                  double&                   aAntennaGain) override;

      std::vector<FrequencyBand> mFrequencyBands;
      PassiveMode*               mModePtr;
      bool                       mPSOS_Enabled;
      bool                       mHavePSOS_FrequencyData;
      bool                       mPSOS_DebugEnabled;
   };

   //! A 'beam' of a sensor 'mode'
   class PassiveBeam : public WsfSensorBeam
   {
   public:
      PassiveBeam();
      PassiveBeam(const PassiveBeam& aSrc);
      PassiveBeam& operator=(const PassiveBeam& aRhs);
      ~PassiveBeam() override;

      // WsfSensorBeam methods
      WsfEM_Rcvr* GetEM_Rcvr() override { return mRcvrPtr; }

      bool ProcessInput(UtInput& aInput) override;

      bool Initialize(WsfSensor* aSensorPtr, PassiveMode* aModePtr);

      bool AttemptToDetect(double             aSimTime,
                           WsfEM_Xmtr*        aXmtrPtr,
                           WsfSensorResult&   aResult,
                           WsfEM_Interaction* aAuxInteractionPtr,
                           bool               aPSOS_Enabled,
                           Interactor*        aInteractorPtr);

      bool ComputePSOS_Effects(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfSensorResult& aResult, Interactor* aInteractorPtr);

      void ProcessFailedPSOS_Detection(double aSimTime, Interactor* aInteractorPtr);

      //! @name External Services access.
      //! These methods support the External Services and are not part of the WSF public interface.
      //@{
      size_t GetDetectionThresholdCount(SignalType aSignalType) const;
      void GetDetectionThresholdEntry(SignalType aSignalType, size_t aIndex, double& aFrequency, double& aThreshold) const;

      size_t GetDetectionSensitivityCount(SignalType aSignalType) const;
      void GetDetectionSensitivityEntry(SignalType aSignalType, size_t aIndex, double& aFrequency, double& aSensitivity) const;

      bool TuneFrequencyBand(double       aSimTime,
                             bool         aReset,
                             unsigned int aFrequencyBandNumber,
                             double       aLowerFreq,
                             double       aUpperFreq,
                             double       aDwellTime,
                             double       aRevistTime);
      //@}

      WsfEM_Antenna*     mAntennaPtr;
      PassiveRcvr*       mRcvrPtr;
      WsfPassiveDetector mDetector;
   };

   //! A 'mode' of the sensor.
   class WSF_EXPORT PassiveMode : public WsfSensorMode
   {
   public:
      class PassiveErrorModel : public wsf::StandardSensorErrorModel
      {
      public:
         bool               ProcessInput(UtInput& aInput) override;
         PassiveErrorModel* Clone() const override { return new PassiveErrorModel(*this); }

         void ApplyMeasurementErrors(const wsf::SphericalMeasurementErrors& aErrors, WsfSensorResult& aResult) override;
         std::unique_ptr<wsf::SphericalMeasurementErrors> ComputeMeasurementErrors(WsfSensorResult& aResult) const override;

      private:
         struct ErrorTable
         {
            TblIndVarU<double> mFrequency;
            TblDepVar1<double> mErrorSigma;
         };

         static void SelectErrorSigma(const ErrorTable& aErrorTable, double aFrequency, double& aErrorSigma);
         static void ReadErrorTable(UtInput& aInput, ErrorTable& aErrorTable, UtInput::ValueType aValueType);

         bool       mUsingErrorTables;
         ErrorTable mAzErrorTable;
         ErrorTable mElErrorTable;
         ErrorTable mRangeErrorTable;
      };

      PassiveMode();
      PassiveMode(const PassiveMode& aSrc);
      PassiveMode& operator=(const PassiveMode& aRhs);
      ~PassiveMode() override;

      // WsfSensorMode methods
      size_t         GetBeamCount() const override { return mBeamList.size(); }
      WsfSensorBeam* GetBeamEntry(size_t aIndex) override { return &(mBeamList[aIndex]); }

      WsfMode* Clone() const override;

      bool Initialize(double aSimTime) override;

      bool ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      bool CanDetect(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfSensorResult& aResult, WsfEM_Interaction& aAuxInteraction);

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

      void UpdateTrackReportingFlags(double aSimTime, WsfTrack* aTrackPtr) override;

      bool AddInteractor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr);

      bool RemoveInteractor(WsfEM_Xmtr* aXmtrPtr, PassiveRcvr* aRcvrPtr);

      void EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult, PassiveRcvr* aRcvrPtr);

      bool TuneFrequencyBand(double       aSimTime,
                             bool         aReset,
                             size_t       aBeamNumber,
                             unsigned int aFrequencyBandNumber,
                             double       aLowerFreq,
                             double       aUpperFreq,
                             double       aDwellTime,
                             double       aRevisitTime);

      double mUnframedDetectionCoastTime;
      double mRangingTime;
      double mRangingTimeTrackQuality;
      bool   mImplicitBeamUsed;
      bool   mExplicitBeamUsed;
      bool   mUnframedDetectionOptimization;
      //! Is 'Probabilistic Scan-On-Scan' (PSOS) processing enabled?
      //! If this is 'true' (the default) it means that SOS processing can occur if the user
      //! has defined the proper input data. This will be 'false' if a direct call is made to
      //! AttemptToDetect (such as from sensor_plot). In such a case each detection attempt is
      //! independent.
      bool mPSOS_Enabled;

      std::vector<PassiveBeam> mBeamList;
      TargetInteractorMap      mTargetInteractorMap;

   protected:
      virtual void ComputeInterferenceEffect(double aSimTime, WsfEM_Xmtr* aXmtrPtr, WsfEM_Rcvr* aRcvrPtr) {}

      //! Returns true if the mode should utilize the 'listener' detection method for an interference transmitter
      virtual bool UseListenerForInterferer(WsfEM_Xmtr* aXmtrPtr) { return false; }

   private:
      mutable std::recursive_mutex mMutex;
   };

   WsfPassiveSensor(WsfScenario& aScenario);
   WsfPassiveSensor(const WsfPassiveSensor& aSrc);
   WsfPassiveSensor& operator=(const WsfPassiveSensor&) = delete;
   ~WsfPassiveSensor() override                         = default;

   //! @name Sensor framework methods
   //@{
   bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

   WsfSensor* Clone() const override;
   bool       Initialize(double aSimTime) override;
   bool       ProcessInput(UtInput& aInput) override;
   void       Update(double aSimTime) override;
   void       TurnOff(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   //@}

   bool TuneFrequencyBand(double       aSimTime,
                          bool         aReset,
                          WsfStringId  aModeNameId,
                          size_t       aBeamNumber,
                          unsigned int aFrequencyBandNumber,
                          double       aLowerFreq,
                          double       aUpperFreq,
                          double       aDwellTime,
                          double       aRevisitTime);

   // Returns the number of Frequency Bands for the given beam
   unsigned int GetNumberofFrequencyBands(WsfStringId aModeId, size_t aBeam);

   const char* GetScriptClassName() const override { return "WsfPassiveSensor"; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

private:
   void SignalChangeCallback(double aSimTime, size_t aTargetIndex);

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<PassiveMode*> mPassiveModeList;

   //! The object used for determining the type of the target.
   WsfTargetTypeReporting mTargetTypeReporting;

   //! The object used for determining the type of the emitter.
   WsfEmitterTypeReporting mEmitterTypeReporting;

   //! The signal reporting list for the current 'AttemptToDetect'.
   //! This is populated in PassiveMode::AttemptToDetect and then merged into the track in PassiveMode::UpdateTrack.
   //! This is not thread-safe when a given sensor is allowed to operate in more than one thread, but that
   //! should never be the case.
   std::vector<WsfTrack::Signal> mSignalList;

   //! True when 'Update()' is in control.
   bool mUpdateActive;
};

// Script Accessor Class
class WSF_EXPORT WsfScriptPassiveSensorClass : public WsfScriptSensorClass
{
public:
   WsfScriptPassiveSensorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(FrequencyBandCount_1);
   UT_DECLARE_SCRIPT_METHOD(FrequencyBandCount_2);
   UT_DECLARE_SCRIPT_METHOD(TuneFrequencyBand);
};

#endif
