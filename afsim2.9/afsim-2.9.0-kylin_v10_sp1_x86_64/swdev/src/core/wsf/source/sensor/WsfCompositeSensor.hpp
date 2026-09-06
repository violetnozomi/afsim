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

#ifndef WSFCOMPOSITESENSOR_HPP
#define WSFCOMPOSITESENSOR_HPP

#include "wsf_export.h"

#include <mutex>
#include <vector>

#include "UtCallbackHolder.hpp"

class WsfFilter;
#include "WsfLocalTrack.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
class WsfTrackManager;

//! A specialization of WsfSensor that implements a sensor that is a collection of other sensors.
class WSF_EXPORT WsfCompositeSensor : public WsfSensor
{
public:
   enum OperatingMode
   {
      cOM_UNDEFINED,
      cOM_INDEPENDENT,
      cOM_SYNCHRONOUS
   };

   WsfCompositeSensor(WsfScenario& aScenario);
   WsfCompositeSensor(const WsfCompositeSensor& aSrc);
   WsfCompositeSensor& operator=(const WsfCompositeSensor& aSrc) = delete;
   ~WsfCompositeSensor() override;

   WsfSensor* Clone() const override;

   // Initialize before other sensors
   int GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_SENSOR - 10; }

   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;
   void Update(double aSimTime) override;

   size_t      GetEM_RcvrCount() const override;
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override;
   size_t      GetEM_XmtrCount() const override;
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override;

   bool WithinFieldOfView(double aSimTime, WsfPlatform* aPlatformPtr) override;
   bool WithinFieldOfView(double aSimTime, WsfTrack* aTrackPtr) override;

   //! A mode of the sensor.
   class CompositeMode : public WsfSensorMode
   {
   public:
      CompositeMode();
      CompositeMode(const CompositeMode& aSrc);
      CompositeMode& operator=(const CompositeMode& aSrc);
      CompositeMode(const WsfSensorMode& aSrc);
      CompositeMode& operator   =(const WsfSensorMode& aSrc);
      ~CompositeMode() override = default;

      WsfMode* Clone() const override;
      bool     Initialize(double aSimTime) override;
      bool     ProcessInput(UtInput& aInput) override;

      bool AttemptToDetect(double aSimTime, WsfPlatform* aTargetPtr, Settings& aSettings, WsfSensorResult& aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      void ApplyMeasurementErrors(WsfSensorResult& aResult) override;

      void InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr) override;

      void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

   private:
      WsfCompositeSensor* mMySensorPtr;

      //! A transient variable used between CompositeMode::AttemptToDetect and UpdateTrack.
      WsfSensorMode* mDetectModePtr;
   };

protected:
   bool DropContributorTrack(double aSimTime, const WsfTrackId& aRawTrackId);

   void UpdateCompositeTrack(double aSimTime, const WsfTrack& aRawTrack);

   unsigned int GetSensorIndex(WsfSensor* aSensorPtr) const;

   bool IsA_ConstituentSensor(WsfSensor* aSensorPtr);

   void SensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr);

   void SensorDetectionChanged(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetIndex, WsfSensorResult& aResult);

   void UpdateXmtrRcvrLists();

   OperatingMode mOperatingMode;

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<CompositeMode*> mCompositeModeList;

   //! The list of constituent sensors (not valid until after Initialize is called).
   std::vector<WsfSensor*> mSensorPtrs;

   //! The list of constituent sensor names.
   std::vector<WsfStringId> mSensorNames;

   //! @name Variables unique to independent mode.
   //@{
   //! The track quality to be assigned to composite tracks.
   //! If this value is 0 (the default), the track quality will be the quality from the last
   //! constituent track that updated the composite track.
   double mTrackQuality;

   //! The pointer to the prototype filter (if defined)
   WsfFilter* mFilterPtr;

   //! Pointer to the track manager that holds the track lists.
   WsfTrackManager* mTrackManagerPtr;

   //! Mutex for locking simulation observer callbacks when multi-threading.
   mutable std::recursive_mutex mMutex;

   //! Registered simulation observer callbacks.
   UtCallbackHolder mCallbacks;
   //@}

   //! @name Variables specific to synchronous mode.
   //@{
   //! The list of transmitters.
   std::vector<WsfEM_Xmtr*> mXmtrList;

   //! The list of receivers.
   std::vector<WsfEM_Rcvr*> mRcvrList;

   //@}
};

#endif
