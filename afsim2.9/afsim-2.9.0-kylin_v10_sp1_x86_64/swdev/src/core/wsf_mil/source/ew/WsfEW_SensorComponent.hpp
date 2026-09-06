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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#ifndef WSFEW_SENSORCOMPONENT_HPP
#define WSFEW_SENSORCOMPONENT_HPP

#include <vector>

#include "WsfEW_Result.hpp"
#include "WsfFalseTarget.hpp"
class WsfFalseTargetBlip;
class WsfFalseTargetScreener;
#include "wsf_mil_export.h"

#include "WsfJamStrobeDetector.hpp"
#include "WsfMilComponentRoles.hpp"
class WsfScenario;
#include "WsfSensor.hpp"
#include "WsfSensorComponent.hpp"

//! The EW component that will be attached to all sensor systems.
class WSF_MIL_EXPORT WsfEW_SensorComponent : public WsfSensorComponent
{
public:
   static void                   RegisterComponentFactory(WsfScenario& aScenario);
   static WsfEW_SensorComponent* Find(const WsfSensor& aSensor);
   static WsfEW_SensorComponent* FindOrCreate(WsfSensor& aSensor);

   WsfEW_SensorComponent();
   WsfEW_SensorComponent(const WsfEW_SensorComponent& aSrc);
   WsfEW_SensorComponent& operator=(const WsfEW_SensorComponent&) = delete;
   ~WsfEW_SensorComponent() override                              = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfSensorComponent.
   //@{
   bool Initialize(double aSimTime) override;

   bool ProcessModeInput(UtInput& aInput, WsfSensorMode& aSensorMode) override;
   bool ProcessBeamInput(UtInput& aInput, WsfSensorBeam& aSensorBeam) override;

   void SelectMode(double aSimTime, WsfStringId aModeNameId) override;
   void DeselectMode(double aSimTime, WsfStringId aModeNameId) override;

   void AttemptToDetect(double aSimTime, WsfSensorResult& aResult) override;

   void ComputeSphericalMeasurementErrors(const WsfSensorResult&           aResult,
                                          wsf::SphericalMeasurementErrors& aErrors) const override;

   void PrePerformScheduledDetections(double aSimTime) override;

   void PostPerformScheduledDetections(double aSimTime) override;

   void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

   bool TrackerAllowTracking(double                 aSimTime,
                             const TrackerSettings& aSettings,
                             const WsfTrackId&      aRequestId,
                             size_t                 aObjectId,
                             WsfTrack*              aTrackPtr,
                             WsfSensorResult&       aResult) override;

   // Callback from WsfSensorTracker implementation to initialize a track.
   // Return a track of the desired type, or 0 to let the default be created.
   void TrackerInitializeTrack(double                 aSimTime,
                               const TrackerSettings& aSettings,
                               const WsfTrackId&      aRequestId,
                               size_t                 aObjectId,
                               WsfSensorMode*         aModePtr,
                               WsfTrack*&             aTrackPtr) override;

   // Callback by WsfSensorTrack implementation when updating a track.
   void TrackerUpdateTrack(double                 aSimTime,
                           const TrackerSettings& aSettings,
                           const WsfTrackId&      aRequestId,
                           size_t                 aObjectId,
                           WsfSensorMode*         aModePtr,
                           WsfTrack*              aTrackPtr,
                           WsfPlatform*           aTargetPtr,
                           WsfSensorResult&       aResult) override;

   // Callback from WsfSensorTracker implementation when dropping a track.
   void TrackerDropTrack(double                 aSimTime,
                         const TrackerSettings& aSettings,
                         const WsfTrackId&      aRequestId,
                         size_t                 aObjectId,
                         WsfSensorMode*         aModePtr,
                         WsfTrack*              aTrackPtr) override;
   //@}

   //! WsfEW_SensorComponent methods.
   //! These are called explicitly called by various components of the EW model
   //! (e.g. jammer, screener, techniques).
   //@{

   bool AttemptToDetectFalseTarget(double               aSimTime,
                                   WsfFalseTargetBlip*  aBlipPtr,
                                   WsfSensor::Settings& aSettings,
                                   WsfSensorResult&     aResult);

   WsfFalseTargetScreener* GetScreener(const WsfSensorMode& aSensorMode) const;

   bool IsEP_TechniqueActive(double aSimTime, WsfStringId aTechniqueId, WsfStringId aModeNameId = WsfStringId());
   bool SelectEP_Technique(double aSimTime, WsfStringId aTechniqueId, WsfStringId aModeNameId = WsfStringId());
   bool DeselectEP_Technique(double aSimTime, WsfStringId aTechniqueId, WsfStringId aModeNameId = WsfStringId());

   bool JammingPerceived(const WsfSensorMode& aSensorMode);
   bool JammingPerceived(WsfStringId aModeNameId);

   bool ContinuousJammingPerceived(const WsfSensorMode& aSensorMode);
   bool ContinuousJammingPerceived(WsfStringId aModeNameId);

   bool PulseJammingPerceived(const WsfSensorMode& aSensorMode);
   bool PulseJammingPerceived(WsfStringId aModeNameId);

   double GetJammingFirstPerceived(const WsfSensorMode& aSensorMode);
   double GetJammingLastPerceived(const WsfSensorMode& aSensorMode);
   //@}

   typedef std::map<WsfEM_Xmtr*, WsfSensorResult> JamStrobeResultsMap;

   //! The EW component extensions to a sensor beam.
   class EW_Beam
   {
   public:
      EW_Beam();

      double GetContJNR_PerceptionThreshold() const { return mContJNR_PerceptionThreshold; }
      double GetPulsedJNR_PerceptionThreshold() const { return mPulsedJNR_PerceptionThreshold; }
      double GetCoherentJNR_PerceptionThreshold() const { return mCoherentJNR_PerceptionThreshold; }
      bool   CanPerceiveJamming() const;

      bool DetectJamStrobes(WsfEW_SensorComponent& aEW_Sensor,
                            double                 aSimTime,
                            WsfSensor::Settings&   aSettings,
                            JamStrobeResultsMap&   aResults);

      size_t mBeamIndex;

      //! The required JNR for continuous type jamming perception by the operator.
      double mContJNR_PerceptionThreshold;

      //! The required JNR for pulse type jamming perception by the operator.
      double mPulsedJNR_PerceptionThreshold;

      //! The required JNR for coherent (FT) jamming perception by the operator.
      double mCoherentJNR_PerceptionThreshold;
   };

   //! The EW component extensions to a sensor mode.
   class EW_Mode
   {
   public:
      EW_Mode();
      EW_Mode(const EW_Mode& aSrc);
      EW_Mode& operator=(const EW_Mode&) = delete;
      ~EW_Mode();

      bool   JammingPerceived();
      bool   ContinuousJammingPerceived();
      bool   PulseJammingPerceived();
      double GetJammingFirstPerceived() const;
      double GetContJammingFirstPerceived() const { return mContJammingFirstPerceived; }
      double GetPulseJammingFirstPerceived() const { return mPulseJammingFirstPerceived; }
      double GetCoherentJammingFirstPerceived() const { return mCoherentJammingFirstPerceived; }
      double GetJammingLastPerceived() const;
      double GetContJammingLastPerceived() const { return mContJammingLastPerceived; }
      double GetPulseJammingLastPerceived() const { return mPulseJammingLastPerceived; }
      double GetCoherentJammingLastPerceived() const { return mCoherentJammingLastPerceived; }

      bool DetectJamStrobes(WsfEW_SensorComponent& aEW_Sensor,
                            double                 aSimTime,
                            WsfSensor::Settings&   aSettings,
                            JamStrobeResultsMap&   aResults);

      bool UpdateFalseTargetInteractors(double aSimTime, WsfEW_SensorComponent& aEW_Sensor, WsfSensor::Settings& aSettings);

      std::vector<EW_Beam> mBeamList;

      WsfFalseTargetScreener* mScreenerPtr;

      double mJammingPerceptionResetTime;
      double mContJammingFirstPerceived;
      double mContJammingLastPerceived;
      double mPulseJammingFirstPerceived;
      double mPulseJammingLastPerceived;
      double mCoherentJammingFirstPerceived;
      double mCoherentJammingLastPerceived;

      //! Jam strobe detector data which is associated with this mode
      WsfJamStrobeDetector mJamStrobeDetector;

      //! If 'true' then used the jam strobe detector
      bool mUseJamStrobeDetector;

      bool mAccumPerceivesContJamming;
      bool mAccumPerceivesPulseJamming;
      bool mAccumPerceivesCoherentJamming;

      bool mMaintainTrackMeasurementHistory;
   };

private:
   WsfSensorMode* GetMode(WsfStringId aModeNameId) const;
   EW_Mode&       GetOrCreateEW_Mode(const WsfSensorMode& aSensorMode);
   EW_Beam&       GetOrCreateEW_Beam(const WsfSensorBeam& aSensorBeam);

   void PerformFalseTargetDetections(double aSimTime, WsfSensor::Settings& aSettings, WsfTrackId& aRequestId);

   void PerformJamStrobeDetections(double aSimTime);

   bool DetectJamStrobes(double aSimTime, WsfSensor::Settings& aSettings, JamStrobeResultsMap& aResults);

   void AttemptToDetectFalseTarget(double                aSimTime,
                                   WsfFalseTarget::Blip* aBlipPtr,
                                   WsfPlatform*          aTempPlatformPtr,
                                   WsfSensor::Settings&  aSettings,
                                   WsfEM_Interaction&    aJammerInteraction,
                                   WsfSensorResult&      aResult);

   void UpdateFalseTargetTrack(double aSimTime, WsfTrack* aTrackPtr, WsfFalseTargetBlip* aBlipPtr, WsfSensorResult& aResult);

   static bool PerceivesJamming(const WsfSensorResult& aResult,
                                double                 aContJNR_Threshold,
                                double                 aPulsedJNR_Threshold,
                                double                 aCoherentJNR_Threshold,
                                bool&                  aPerceivesContJamming,
                                bool&                  aPerceivesPulseJamming,
                                bool&                  aPerceivesCoherentJamming);
   static bool PerceivesContinuousJamming(const WsfSensorResult& aResult, double aContJNR_Threshold);
   static bool PerceivesPulseJamming(const WsfSensorResult& aResult, double aPulsedJNR_Threshold);
   static bool PerceivesCoherentJamming(const WsfSensorResult& aResult, double aCoherentJNR_Threshold);

   std::vector<EW_Mode> mModeList;
   EW_Mode              mTemplate;

   //! @name Bistatic optimization.
   //! These are used for optimization of bistatic operations on behalf of the sensor.
   //! See AttemptToDetect for more information.
   //@{
   double       mLastSimTime;
   WsfEM_Rcvr*  mLastRcvrPtr;
   WsfEW_Result mLastEW_Result;
   //@}

   // See TrackerAllowTracking and TrackerDropTrack for this
   double mSuppressedTrackingSimTime;
   size_t mSuppressedTrackingObjectId;

   bool mIsWsfRadarSensor; //!< true if the sensor is a WsfRadarSensor

   //! @name Optimizations.
   //! The following allow quick exit from Pre/Post scheduled detections.
   //@{
   //! The following is true if at least one mode has a false target screener.
   bool mHasFalseTargetScreeners;
   //! The following is true if at least one mode has a jam strobe detector.
   bool mHasJamStrobeDetectors;
   //@}
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfEW_SensorComponent, cWSF_COMPONENT_EW_SENSOR)

#endif
