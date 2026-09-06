// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOHERENTSENSORPROCESSOR_HPP
#define WSFCOHERENTSENSORPROCESSOR_HPP

#include "wsf_nx_export.h"

#include <list>
#include <map>
#include <vector>

class WsfFusionStrategy;
class WsfPlatform;
class WsfScenario;
#include "WsfSensorProcessor.hpp"
class WsfSensor;
#include "WsfSensorResult.hpp"
#include "WsfSingleSensorObserver.hpp"
#include "WsfStringId.hpp"
class WsfTrack;

//! A processor that coherently sums signals from multiple sensors to locate targets
//! that may be below detection thresholds of the individual sensors.

class WSF_NX_EXPORT WsfCoherentSensorProcessor : public WsfSensorProcessor, public WsfSingleSensorObserver
{
public:
   class ResultData
   {
   public:
      ResultData()
         : mSimTime()
         , mResult()
      {
      }

      ResultData(double aSimTime, WsfSensorResult& aResult)
         : mSimTime(aSimTime)
         , mResult(aResult)
      {
      }

      double          mSimTime;
      WsfSensorResult mResult;
   };

   //!  Detection Algorithm State.
   enum DetectionAlgorithmType
   {
      cDA_SNR,
      cDA_RSS_SNR
   };

   //! Target Sensing State.
   enum TargetSensingState
   {
      cSS_UNDETECTED,
      cSS_DETECTED
   };

   //! Type definitions for storing and processing sensor results
   using Results                   = std::vector<ResultData>;
   using SensorToResultsMap        = std::map<WsfStringId, Results>;
   using TargetToSensorToResultMap = std::map<WsfStringId, SensorToResultsMap>;

   WsfCoherentSensorProcessor(WsfScenario& aScenario);
   ~WsfCoherentSensorProcessor() override;

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;

   void Update(double aSimTime) override;

   // Overrides from WsfSingleSensorObserver
   void OnSensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr) override;
   void OnSensorDetectionAttempted(double           aSimTime,
                                   WsfSensor*       aSensorPtr,
                                   WsfPlatform*     aTargetPtr,
                                   WsfSensorResult& aResult) override;
   void OnSensorDeleting(WsfSensor* aSensorPtr) override;
   void OnSensorTargetUpdated(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult) override;

   class State
   {
   public:
      WsfTrack* mTrackPtr;
      // WsfFilter*       mFilterPtr;
      // unsigned int     mStreamNumber;
   };

   // class Status
   //{
   //    public:
   //    Status(WsfStringId aTruthName = WsfStringId())
   //       : mStateEntryTime(-1.0E+30),
   //         mMinimumTimeInState(0.0),
   //         mLastGoodUpdateTime(-1.0E+30),
   //         mCurrentState(cSS_UNDETECTED),
   //         mCoastingStartTime(-1.0E+30),
   //         mCoastingState(cSS_UNDETECTED),
   //         mTruthName(aTruthName),
   //         mIsStale(false)
   //    { }

   //   WsfStringId TruthName() const { return mTruthName; }

   //   void EnterState(TargetSensingState aNewState,
   //                   double             aStateEntryTime,
   //                   double             aMinimumTimeInState)
   //   {
   //      mCurrentState = aNewState;
   //      mStateEntryTime = aStateEntryTime;
   //      mMinimumTimeInState = aMinimumTimeInState;
   //   }

   //   void SetLastGoodUpdateTime(double aSimTime)
   //   {
   //      mLastGoodUpdateTime = aSimTime;
   //      mCoastingStartTime = -1.0E-30;
   //      mIsStale = false;
   //   }

   //   void SetStale() { mIsStale = true; }
   //   bool IsStale() const { return mIsStale; }

   //   //! Updates the coasting status.
   //   //! @returns true if the coast time limit has been exceeded, or false if coasting can continue.
   //   bool CoastTimeExceeded(double             aSimTime,
   //                          TargetSensingState aState,
   //                          double             aCoastTime)
   //   {
   //      if (mCoastingStartTime < 0.0)
   //      {
   //         mCoastingStartTime = aSimTime;
   //         mCoastingState = aState;
   //      }
   //      else if (aState > mCoastingState)
   //      {
   //         mCoastingState = aState;
   //      }
   //      mIsStale = false;
   //      return ((aSimTime - mCoastingStartTime) >= aCoastTime);
   //   }

   //   TargetSensingState CoastingState() const { return mCoastingState; }

   //   TargetSensingState CurrentState() const { return mCurrentState; }
   //   double StateEntryTime() const { return mStateEntryTime; }

   //   double EarliestStateExitTime() const { return mStateEntryTime + mMinimumTimeInState; }

   //   //! The time when the transition occurred into the current state.
   //   double              mStateEntryTime;
   //   //! The minimum time that must be spent in the current state before a transition to a higher state can occur.
   //   double              mMinimumTimeInState;
   //   //! The last time when a detection was registered that met the criteria for the current state.
   //   double              mLastGoodUpdateTime;
   //   //! The current state
   //   TargetSensingState  mCurrentState;
   //   //! The time when coasting started.
   //   double              mCoastingStartTime;
   //   //! The state (quality) of the update associated with mLastFailedUpdateTime;
   //   TargetSensingState  mCoastingState;
   //   //! The name id of the underlying platform represented by the object.
   //   WsfStringId         mTruthName;
   //   //! True if the object is stale (didn't exist in the last image).
   //   bool                mIsStale;
   //};

   //! Return the correlation strategy used by this processor.
   // WsfCorrelationStrategy& GetCorrelationStrategy() { return *mCorrelationStrategyPtr; }

   //! Return the fusion strategy used by this processor.
   WsfFusionStrategy& GetFusionStrategy() { return *mFusionStrategyPtr; }

   //! Sets the correlation strategy used by this processor.
   // void SetCorrelationStrategy(WsfStringId aTypeName);

   //! Sets the fusion strategy used by this processor.
   void SetFusionStrategy(WsfStringId aTypeName);

protected:
   WsfCoherentSensorProcessor(const WsfCoherentSensorProcessor& aSrc);
   WsfCoherentSensorProcessor& operator=(const WsfCoherentSensorProcessor&) = delete;

   //! Clear any data or lists maintained by inheriting classes
   bool SensorAdded(WsfSensor* aSensorPtr) override;
   bool SensorRemoved(WsfSensor* aSensorPtr) override;

   void ClearSensorResults(WsfStringId aSensorId);

   // virtual void TrackDropped(double          aSimTime,
   //                           const WsfTrack& aTrack) override;

   virtual void UpdateTrack(double           aSimTime,
                            WsfTrack*        aTrackPtr,
                            WsfSensor*       aSensorPtr,
                            WsfPlatform*     aTargetPtr,
                            WsfSensorResult& aResult);

private:
   void ClearStateList();

   void ProcessResults(double aSimTime, WsfSensor* aSensorPtr = nullptr, WsfPlatform* aTargetPtr = nullptr);

   void ResultsProcessingInitiated(double aSimTime);
   void ResultsProcessingCompleted(double aSimTime);

   void PurgeOldTracks(double aSimTime, WsfSensor* aSensorPtr);

   //! Attached single-sensor observer list
   std::list<WsfSensor*> mAttachedSensors;

   // Input Items
   bool mUseTargetResult;
   // bool                      mProcessOnTargetUpdate;
   DetectionAlgorithmType mDetectionAlgorithmType;
   double                 mDetectionThreshold;
   // bool                      mShowStateTransitions;

   // Results containers
   SensorToResultsMap        mIncomingResults;
   TargetToSensorToResultMap mProcessedResults;

   //! The maximum amount of time a track can coast before it is dropped.
   double mCoastTime;

   ////! @name Coast times before reverting to lower state.
   ////@{
   // WsfVariable<double>    mTransitionCoastTime;
   // WsfVariable<double>    mDetectionCoastTime;
   ////@}

   //! The state data for active tracks.
   //! The key value is the platform index of the target.
   using StateList = std::map<size_t, State*>;
   StateList mStateList;

   // typedef std::map<size_t, Status> StatusList;
   // StatusList                mStatusList;

   //! These are filtering and fusion strategies, and a prototype filter
   //! for all results/tracks. They are configured based on input values.
   // WsfCorrelationStrategy*   mCorrelationStrategyPtr;
   WsfFusionStrategy* mFusionStrategyPtr;
   // WsfFilter*                mPrototypeFilterPtr;  // could be a part of the state struct above
};

#endif
