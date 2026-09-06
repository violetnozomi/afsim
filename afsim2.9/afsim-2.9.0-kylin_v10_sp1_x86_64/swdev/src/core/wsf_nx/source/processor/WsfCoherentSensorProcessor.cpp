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

#include "WsfCoherentSensorProcessor.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfFusionStrategyTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorTracker.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"

// namespace
//{
//    static const char* sStateName[] =
//    {
//       "UNDETECTED",
//       "DETECTED",
//    };
// }

WsfCoherentSensorProcessor::WsfCoherentSensorProcessor(WsfScenario& aScenario)
   : WsfSensorProcessor(aScenario)
   , WsfSingleSensorObserver()
   , mAttachedSensors()
   , mUseTargetResult(false)
   ,
   // mProcessOnTargetUpdate(false),
   mDetectionAlgorithmType(cDA_SNR)
   , mDetectionThreshold(pow(10.0, 3.0 / 10.0))
   ,
   // mShowStateTransitions(false),
   mIncomingResults()
   , mProcessedResults()
   , mCoastTime(0.0)
   ,
   // mTransitionCoastTime(0.0),
   // mDetectionCoastTime(0.0),
   mStateList()
   ,
   // mCorrelationStrategyPtr(0),
   mFusionStrategyPtr(nullptr)
// mPrototypeFilterPtr(0),
{
   // WsfCorrelationStrategyTypes& correlationTypes(WsfCorrelationStrategyTypes::Get(aScenario));
   // mCorrelationStrategyPtr = correlationTypes.Create(correlationTypes.GetDefaultStrategyName());
   WsfFusionStrategyTypes& fusionTypes(WsfFusionStrategyTypes::Get(aScenario));
   mFusionStrategyPtr = fusionTypes.Create(fusionTypes.GetDefaultStrategyName());
}

WsfCoherentSensorProcessor::WsfCoherentSensorProcessor(const WsfCoherentSensorProcessor& aSrc)
   : WsfSensorProcessor(aSrc)
   , WsfSingleSensorObserver(aSrc)
   , mAttachedSensors()
   , mUseTargetResult(aSrc.mUseTargetResult)
   ,
   // mProcessOnTargetUpdate(aSrc.mProcessOnTargetUpdate),
   mDetectionAlgorithmType(aSrc.mDetectionAlgorithmType)
   , mDetectionThreshold(aSrc.mDetectionThreshold)
   ,
   // mShowStateTransitions(aSrc.mShowStateTransitions),
   mIncomingResults()
   , mProcessedResults()
   , mCoastTime(aSrc.mCoastTime)
   ,
   // mTransitionCoastTime(aSrc.mTransitionCoastTime),
   // mDetectionCoastTime(aSrc.mDetectionCoastTime),
   mStateList()
   ,
   // mCorrelationStrategyPtr(aSrc.mCorrelationStrategyPtr->Clone()),
   mFusionStrategyPtr(aSrc.mFusionStrategyPtr->Clone())
// mPrototypeFilterPtr(0),
{
   // if (aSrc.mPrototypeFilterPtr != 0)
   //{
   //    mPrototypeFilterPtr = aSrc.mPrototypeFilterPtr->Clone();
   // }
}

bool WsfCoherentSensorProcessor::SensorAdded(WsfSensor* aSensorPtr)
{
   aSensorPtr->AttachObserver(this);
   mAttachedSensors.push_back(aSensorPtr);
   return true;
}

bool WsfCoherentSensorProcessor::SensorRemoved(WsfSensor* aSensorPtr)
{
   aSensorPtr->DetachObserver(this);
   mAttachedSensors.remove(aSensorPtr);
   ClearSensorResults(aSensorPtr->GetNameId());
   mIncomingResults.erase(aSensorPtr->GetNameId());
   return true;
}

// virtual
WsfCoherentSensorProcessor::~WsfCoherentSensorProcessor()
{
   ClearStateList();
   // delete mCorrelationStrategyPtr;
   delete mFusionStrategyPtr;
   // delete mPrototypeFilterPtr;

   // Assumes that the SensorRemoved function is clearing the list.
   while (!mAttachedSensors.empty())
   {
      SensorRemoved(mAttachedSensors.front());
   }
}

WsfProcessor* WsfCoherentSensorProcessor::Clone() const
{
   return new WsfCoherentSensorProcessor(*this);
}

bool WsfCoherentSensorProcessor::Initialize(double aSimTime)
{
   bool ok = WsfSensorProcessor::Initialize(aSimTime);

   if (mCoastTime <= 0.0)
   {
      // Default to the update interval for now
      // TODO-AWK add in ability to take sensor frame times also
      mCoastTime = GetUpdateInterval();
   }

   // TODO-AWK // Set a standard name for this processor to facilitate lookup.
   // TODO-AWK // There should only be one instance of an WSF_COHERENT_SENSOR_PROCESSOR on a platform:
   // TODO-AWK GetPlatform()->GetAuxData().Assign("COHERENT_SENSOR_PROCESSOR_NAME", GetName());

   ok &= mFusionStrategyPtr->Initialize(this); // No Track Manager - TODO - Issues may arise in some strategies
   // ok &= mCorrelationStrategyPtr->Initialize(this);

   return ok;
}

bool WsfCoherentSensorProcessor::Initialize2(double aSimTime)
{
   return WsfProcessor::Initialize2(aSimTime);
}

// virtual
bool WsfCoherentSensorProcessor::ProcessInput(UtInput& aInput)
{
   // WsfFilter* filterPtr = 0;
   // std::string correlationType;
   std::string fusionType;

   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "detection_threshold")
   {
      aInput.ReadValueOfType(mDetectionThreshold, UtInput::cRATIO);
      aInput.ValueGreater(mDetectionThreshold, 0.0);
   }
   else if (command == "use_target_result")
   {
      aInput.ReadValue(mUseTargetResult);
   }
   // else if (command == "result_processing_timing")
   //{
   //    string processTypeStr;
   //    aInput.ReadValue(processTypeStr);
   //    if (processTypeStr == "on_processor_update")
   //    {
   //       mProcessOnTargetUpdate = false;
   //    }
   //    else if (processTypeStr == "on_target_update")
   //    {
   //       mProcessOnTargetUpdate = true;
   //    }
   //    else
   //    {
   //       throw UtInput::BadValue(aInput, "result_processing_type not recognized: " + processTypeStr);
   //    }
   // }
   else if (command == "coast_time")
   {
      aInput.ReadValueOfType(mCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mCoastTime, 0.0);
   }
   // else if (command == "transition_coast_time")
   //{
   //    mTransitionCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
   //    mTransitionCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   // }
   // else if (command == "detection_coast_time")
   //{
   //    mDetectionCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
   //    mDetectionCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   // }
   else if (command == "result_processing_type")
   {
      std::string detectTypeStr;
      aInput.ReadValue(detectTypeStr);
      if (detectTypeStr == "SNR_BASED")
      {
         mDetectionAlgorithmType = cDA_SNR;
      }
      else if (detectTypeStr == "RSS_BASED")
      {
         mDetectionAlgorithmType = cDA_RSS_SNR;
      }
      else
      {
         throw UtInput::BadValue(aInput, "detect_type not recognized: " + detectTypeStr);
      }
   }
   // else if (WsfFilterTypes::Get(*GetScenario()).LoadInstance(aInput, filterPtr))
   //{
   //    delete mPrototypeFilterPtr;
   //    mPrototypeFilterPtr = filterPtr;
   // }
   // else if (mCorrelationStrategyPtr->ProcessInput(aInput))
   //{
   // }
   // else if ((command == "correlation_method") ||
   //    (command == "correlation_type"))
   //{
   //    aInput.ReadCommand(correlationType);
   // }
   else if ((command == "fusion_method") || (command == "fusion_type"))
   {
      aInput.ReadCommand(fusionType);
   }
   else if (mFusionStrategyPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfSensorProcessor::ProcessInput(aInput);
   }

   // if (!correlationType.empty())
   //{
   //    delete mCorrelationStrategyPtr;
   //    mCorrelationStrategyPtr = 0;
   //    mCorrelationStrategyPtr = WsfCorrelationStrategyTypes::Get(*GetScenario()).Create(correlationType);
   // }
   if (!fusionType.empty())
   {
      delete mFusionStrategyPtr;
      mFusionStrategyPtr = nullptr;
      mFusionStrategyPtr = WsfFusionStrategyTypes::Get(GetScenario()).Create(fusionType);
   }

   return myCommand;
}

void WsfCoherentSensorProcessor::Update(double aSimTime)
{
   // if (!mProcessOnTargetUpdate)
   //{
   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "WsfCoherentSensorProcessor: Updating:";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Name: " << GetName();
      logger.AddNote() << "Type: " << GetType();
   }
   ProcessResults(aSimTime);
   //}
}

void WsfCoherentSensorProcessor::OnSensorTurnedOff(double aSimTime, WsfSensor* aSensorPtr)
{
   // Clear all processed results.
   ClearSensorResults(aSensorPtr->GetNameId());

   // Clear all incoming results for this sensor
   mIncomingResults.erase(aSensorPtr->GetNameId());
}

// virtual
void WsfCoherentSensorProcessor::OnSensorDetectionAttempted(double           aSimTime,
                                                            WsfSensor*       aSensorPtr,
                                                            WsfPlatform*     aTargetPtr,
                                                            WsfSensorResult& aResult)
{
   if (!mUseTargetResult)
   {
      std::lock_guard<std::recursive_mutex> lock(mIncomingMutex);

      // if (DebugEnabled())
      //{
      //    cout << "Processor " << GetName()
      //       << " received detection attempt for '" << aSensorPtr->GetName()
      //       << "' for target '" << aTargetPtr->GetName() << endl;
      // }

      // TODO-AWK Create a struct to also hold the sensor and target pointers?
      ResultData resultData(aSimTime, aResult);
      mIncomingResults[aSensorPtr->GetNameId()].push_back(resultData);
   }
}

// virtual
void WsfCoherentSensorProcessor::OnSensorDeleting(WsfSensor* aSensorPtr)
{
   // Clear all processed results.
   ClearSensorResults(aSensorPtr->GetNameId());

   // Clear all incoming results for this sensor
   mIncomingResults.erase(aSensorPtr->GetNameId());
}

// virtual
void WsfCoherentSensorProcessor::OnSensorTargetUpdated(double           aSimTime,
                                                       WsfSensor*       aSensorPtr,
                                                       WsfPlatform*     aTargetPtr,
                                                       WsfSensorResult& aResult)
{
   if (mUseTargetResult)
   {
      std::lock_guard<std::recursive_mutex> lock(mIncomingMutex);

      ResultData resultData(aSimTime, aResult);
      mIncomingResults[aSensorPtr->GetNameId()].push_back(resultData);
   }

   if (GetUpdateInterval() <= 0.0)
   {
      ProcessResults(aSimTime, aSensorPtr, aTargetPtr);
   }
}

void WsfCoherentSensorProcessor::ClearStateList()
{
   for (auto& sli : mStateList)
   {
      State* statePtr = sli.second;
      delete statePtr->mTrackPtr;
      // delete statePtr->mFilterPtr;
      delete statePtr;
   }
   mStateList.clear();
}

void WsfCoherentSensorProcessor::ProcessResults(double aSimTime, WsfSensor* aSensorPtr /*= 0*/, WsfPlatform* aTargetPtr /*= 0*/)
{
   // Loop over all incoming results for all sensors and process into target collections
   for (const auto& incomingResult : mIncomingResults)
   {
      if ((aSensorPtr != nullptr) && (aSensorPtr->GetNameId() != incomingResult.first))
      {
         continue;
      }

      WsfStringId sensorNameId(incomingResult.first);

      ClearSensorResults(sensorNameId);

      // Loop over all incoming results for this sensor and process into target collections
      Results results = incomingResult.second;
      for (const ResultData& resultData : results)
      {
         if (resultData.mResult.GetTarget() != nullptr)
         {
            WsfStringId        tgtNameId(resultData.mResult.GetTarget()->GetNameId());
            SensorToResultsMap sensorResults;
            if (mProcessedResults.find(tgtNameId) == mProcessedResults.end())
            {
               mProcessedResults[tgtNameId] = sensorResults;
            }
            mProcessedResults[tgtNameId][sensorNameId].push_back(resultData);
         }
      }
   }
   mIncomingResults.clear();

   ResultsProcessingInitiated(aSimTime); // Notify derived classes

   // Process each target with new data collected
   for (auto& processedResult : mProcessedResults)
   {
      WsfPlatform* targetPtr = aTargetPtr;
      if (targetPtr != nullptr)
      {
         if (targetPtr->GetNameId() != processedResult.first)
         {
            continue;
         }
      }
      else
      {
         targetPtr = GetSimulation()->GetPlatformByName(processedResult.first);
      }

      ResultData           bestResultData;
      WsfSensorResult&     bestResult(bestResultData.mResult);
      WsfLocalSensorResult snrResult;

      SensorToResultsMap sensorToResultMap = processedResult.second;
      for (auto& sri : sensorToResultMap)
      {
         WsfSensor* sensorPtr = aSensorPtr;
         if (sensorPtr != nullptr)
         {
            if (sensorPtr->GetNameId() != sri.first)
            {
               continue;
            }
         }
         else
         {
            sensorPtr = GetPlatform()->GetComponent<WsfSensor>(sri.first);
         }

         // Loop over all processed results for this sensor and process into tracks as required
         Results results = sri.second;
         for (auto& result : results)
         {
            ResultData&      currentResultData = result;
            WsfSensorResult& currentResult     = result.mResult;

            if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
            {
               auto logger = ut::log::debug() << "WsfCoherentSensorProcessor: New Result:";
               logger.AddNote() << "T = " << aSimTime;
               logger.AddNote() << "Target: " << targetPtr->GetName();
               logger.AddNote() << "sensor: " << sensorPtr->GetName();
               logger.AddNote() << "SNR: " << UtMath::SafeLinearToDB(currentResult.mSignalToNoise) << " dB";
               logger.AddNote() << "Signal: " << currentResult.mRcvdPower;
               logger.AddNote() << "Noise: " << currentResult.mRcvrNoisePower;
            }

            if (mDetectionAlgorithmType == cDA_SNR)
            {
               // Greedy and last with highest SNR
               if ((currentResult.mSignalToNoise > 0.0) &&
                   ((snrResult.mSignalToNoise <= 0.0) || (currentResult.mSignalToNoise >= bestResult.mSignalToNoise)))
               {
                  bestResultData = currentResultData;
                  snrResult      = currentResult;
                  ++snrResult.mMeasurementCount;
               }
            }
            else if (mDetectionAlgorithmType == cDA_RSS_SNR)
            {
               if (currentResult.mSignalToNoise > 0.0)
               {
                  ++snrResult.mMeasurementCount;
                  if (snrResult.mSignalToNoise <= 0.0)
                  {
                     // First one in - just copy result
                     snrResult = currentResult;
                  }
                  else
                  {
                     // Calculate SNR and save in snrResult
                     snrResult.mRcvdPower      = sqrt(snrResult.mRcvdPower * snrResult.mRcvdPower +
                                                 currentResult.mRcvdPower * currentResult.mRcvdPower);
                     snrResult.mRcvrNoisePower = sqrt((snrResult.mRcvrNoisePower * snrResult.mRcvrNoisePower +
                                                       currentResult.mRcvrNoisePower * currentResult.mRcvrNoisePower) /
                                                      2.0);
                     snrResult.mSignalToNoise  = sqrt(snrResult.mSignalToNoise * snrResult.mSignalToNoise +
                                                     currentResult.mSignalToNoise * currentResult.mSignalToNoise);
                     snrResult.mSignalToNoise  = snrResult.mRcvdPower / snrResult.mRcvrNoisePower;
                  }

                  // Greedy and last with highest SNR for data
                  if (currentResult.mSignalToNoise >= bestResult.mSignalToNoise)
                  {
                     bestResultData = currentResultData;
                  }
               }
            }
            else
            {
               auto logger = ut::log::error() << "WsfCoherentSensorProcessor: 'detection_type' not recognized.";
               logger.AddNote() << "Name: " << GetName();
               logger.AddNote() << "Type: " << mDetectionAlgorithmType;
            }

            // TODO Sensor Result Fusion
            //
            // User checks on fusion type/strategy
            //    Decide if result can/should be used Is3d(), (SNR > Threshold), data validity, etc...
            //      perform Fusion
            //      Set in result (probably snResult then copy to bestResult below)
            //    end logic check(s)
            // end check fusion type/strategy

            if (currentResult.mMeasurement.LocationValid() ||
                (currentResult.mMeasurement.RangeValid() && currentResult.mMeasurement.BearingValid() &&
                 currentResult.mMeasurement.ElevationValid()))
            {
               mFusionStrategyPtr->UpdateLocalResultFromNonLocalResult(aSimTime, snrResult, currentResult);
            }
         }

         bestResult.mRcvdPower      = snrResult.mRcvdPower;
         bestResult.mRcvrNoisePower = snrResult.mRcvrNoisePower;
         bestResult.mSignalToNoise  = snrResult.mSignalToNoise;
         // TODO Set fusion data also?
      }

      // Create/Update tracks
      if (bestResultData.mResult.mSignalToNoise >= mDetectionThreshold)
      {
         // The target is detectable. Allocate a track if one does not already exist for this target.
         WsfSensor* sensorPtr = dynamic_cast<WsfSensor*>(bestResult.GetReceiver()->GetArticulatedPart());

         double simTime = bestResult.mMeasurement.mUpdateTime.IsValid() ? bestResult.mMeasurement.GetUpdateTime() :
                                                                          bestResultData.mSimTime;

         if (DebugEnabled())
         {
            auto logger = ut::log::debug() << "WsfCoherentSensorProcessor: Best/Calculated Result:";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Target: " << targetPtr->GetName();
            logger.AddNote() << "Sensor: " << sensorPtr->GetName();
            logger.AddNote() << "SNR: " << UtMath::SafeLinearToDB(bestResult.mSignalToNoise) << " dB";
            logger.AddNote() << "Signal: " << bestResult.mRcvdPower;
            logger.AddNote() << "Noise: " << bestResult.mRcvrNoisePower;
         }

         WsfTrack* trackPtr = nullptr;
         // WsfFilter* filterPtr = 0;
         auto sli = mStateList.find(targetPtr->GetIndex());
         if (sli != mStateList.end())
         {
            // We already have a track on this object.
            State* statePtr = (*sli).second;
            trackPtr        = statePtr->mTrackPtr;
            // filterPtr = statePtr->mFilterPtr;

            //// If there is a filter and if the stream number has changed, reset the filter.
            // if ((filterPtr != 0) &&
            //    (statePtr->mStreamNumber != aImage.GetStreamNumber()))
            //{
            //    statePtr->mStreamNumber = aImage.GetStreamNumber();
            //    filterPtr->Reset();
            //    trackPtr->SetStateCovariance(aSimTime, 0);
            //    trackPtr->SetResidualCovariance(0);
            // }

            trackPtr->Update(simTime);
         }
         else
         {
            // We don't have a track on this object, so start one.
            trackPtr = new WsfTrack(WsfTrack::cPROCESSED, this, GetPlatform());
            trackPtr->Initialize(simTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
            State* statePtr = new State();
            // if (mFilterPtr != 0)
            //{
            //    filterPtr = mFilterPtr->Clone();
            // }
            statePtr->mTrackPtr = trackPtr;
            // statePtr->mFilterPtr = filterPtr;
            // statePtr->mStreamNumber = aImage.GetStreamNumber();
            mStateList[targetPtr->GetIndex()] = statePtr;
         }
         trackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

         // Set SNR data
         trackPtr->SetSignalToNoiseValid(bestResult.mSignalToNoise > 0);
         trackPtr->SetSignalToNoise(bestResult.mSignalToNoise);
         trackPtr->SetFusible(true);

         // Set the location at the time of the report.
         // TODO-AWK - should this be this platform and do we need to redo the errors and stuff??
         double sensorLocWCS[3];
         UtVec3d::Set(sensorLocWCS, bestResult.mRcvrLoc.mLocWCS);
         if (!bestResult.mRcvrLoc.mIsValid)
         {
            if ((bestResult.GetReceiver() != nullptr) && (bestResult.GetReceiver()->GetAntenna() != nullptr))
            {
               bestResult.GetReceiver()->GetAntenna()->GetLocationWCS(sensorLocWCS);
            }
            else if (sensorPtr != nullptr)
            {
               sensorPtr->GetLocationWCS(sensorLocWCS);
            }
            else
            {
               GetPlatform()->GetLocationErrorWCS(sensorLocWCS);
            }
         }
         double platformLocationErrorWCS[3];
         GetPlatform()->GetLocationErrorWCS(platformLocationErrorWCS);
         UtVec3d::Add(sensorLocWCS, sensorLocWCS, platformLocationErrorWCS);
         trackPtr->SetOriginatorLocationWCS(sensorLocWCS);

         //// Update Sensor Information
         // if (sensorPtr != 0)
         //{
         //    // The originator of the track is the processor, but one often needs to know information
         //    // about the sensor. This is kind of a hybrid track - it is a directly derived product
         //    // of the sensor, but it isn't a true 'sensor measurement' like a radar.
         //    trackPtr->SetSensorOriginator(sensorPtr, sensorPtr->GetPlatform());
         //    trackPtr->SetSensorModeId(sensorPtr->GetModeEntry(bestResult.mModeIndex)->GetNameId());
         // }

         // Update the target truth data.
         if (targetPtr != nullptr)
         {
            trackPtr->SetTargetIndex(targetPtr->GetIndex());
            trackPtr->SetTargetName(targetPtr->GetNameId());
            trackPtr->SetTargetType(targetPtr->GetType());
         }

         // Set the measured object location.
         double locationWCS[3] = {0.0, 0.0, 0.0};
         if (bestResult.mMeasurement.LocationValid())
         {
            // Get the measured object location.
            bestResult.mMeasurement.GetLocationWCS(locationWCS);
         }
         else if (bestResult.mTgtLoc.mIsValid)
         {
            UtVec3d::Set(locationWCS, bestResult.mTgtLoc.mLocWCS);
         }
         // Update the track with new (possibly filtered) location.
         trackPtr->SetLocationWCS(locationWCS);
         trackPtr->SetWCS_LocationValid(true);
         trackPtr->Set3D(true);

         // Invoke the virtual method that lets a derived classes update the track.
         UpdateTrack(aSimTime, trackPtr, sensorPtr, targetPtr, bestResult);

         TrackUpdated(aSimTime, sensorPtr, trackPtr);
      }
   }
   PurgeOldTracks(aSimTime, aSensorPtr);
   ResultsProcessingCompleted(aSimTime); // Notify derived classes
}


// =================================================================================================
//! Hook to notify a derived class the processing of an image has been completed.
//!
//! @param aSimTime The current simulation time.
//! @param aImage   The image that was processed.
// protected virtual
void WsfCoherentSensorProcessor::ResultsProcessingCompleted(double aSimTime)
{
   //// Any status block that was not updated directly is forced to have a 'no-detect' update.
   // for (StatusList::iterator sli = mStatusList.begin(); sli != mStatusList.end(); ++sli)
   //{
   //    Status& status = sli->second;
   //    if (status.IsStale())
   //    {
   //       double coastTime = mTransitionCoastTime;
   //       switch (status.CurrentState())
   //       {
   //          case cSS_DETECTED:
   //             coastTime = mDetectionCoastTime;
   //             break;
   //          default:
   //             break;
   //       }
   //       if (status.CoastTimeExceeded(aSimTime, cSS_UNDETECTED, coastTime))
   //       {
   //          TargetSensingState currentState = status.CurrentState();
   //          status.EnterState(cSS_UNDETECTED, aSimTime, 0.0);

   //         if (mShowStateTransitions)
   //         {
   //            cout << "T=" << aSimTime << ' ' << GetPlatform()->GetName() << '.' << GetName()
   //                 << "; stale Transition " << status.TruthName()
   //                 << " from state " << sStateName[currentState]
   //                 << " to state: " << sStateName[status.CurrentState()]
   //                 << ", entry time: " << status.StateEntryTime()
   //                 << endl;
   //         }
   //      }
   //   }
   //}
}

// =================================================================================================
//! Hook to notify a derived class the processing of an image is beginning.
//!
//! @param aSimTime The current simulation time.
//! @param aResult   The image that will be processed.
// protected virtual
void WsfCoherentSensorProcessor::ResultsProcessingInitiated(double aSimTime)
{
   //// Mark all status blocks as potentially stale.
   // for (StatusList::iterator sli = mStatusList.begin(); sli != mStatusList.end(); ++sli)
   //{
   //    sli->second.SetStale();
   // }
}


void WsfCoherentSensorProcessor::PurgeOldTracks(double aSimTime, WsfSensor* aSensorPtr)
{
   for (auto sli = mStateList.begin(); sli != mStateList.end();)
   {
      State*    statePtr            = (*sli).second;
      WsfTrack* trackPtr            = statePtr->mTrackPtr;
      double    timeSinceLastUpdate = aSimTime - trackPtr->GetUpdateTime();
      if (timeSinceLastUpdate > mCoastTime)
      {
         TrackDropped(aSimTime, *trackPtr); // Notify derived classes

         // Send the track drop message to all attached objects.
         WsfTrackDropMessage message(GetPlatform(), aSimTime, trackPtr->GetTrackId(), trackPtr->GetTargetIndex());
         SendMessage(aSimTime, message);

         // Call the observer if the sensor still exists.
         if (aSensorPtr != nullptr)
         {
            WsfObserver::SensorTrackDropped(GetSimulation())(aSimTime, aSensorPtr, trackPtr);
         }
         delete statePtr->mTrackPtr;
         // delete statePtr->mFilterPtr;
         delete statePtr;
         sli = mStateList.erase(sli);
      }
      else
      {
         ++sli;
      }
   }
}

void WsfCoherentSensorProcessor::ClearSensorResults(WsfStringId aSensorId)
{
   // Clear all results for this sensor in all targets
   for (auto pri = mProcessedResults.begin(); pri != mProcessedResults.end();)
   {
      if (pri->second.find(aSensorId) != pri->second.end())
      {
         pri->second.erase(aSensorId);
      }

      if (pri->second.empty())
      {
         pri = mProcessedResults.erase(pri);
      }
      else
      {
         ++pri;
      }
   }
}

//// =================================================================================================
////! @param aSimTime  [input] The current simulation time.
////! @param aSensorPtr [input] Pointer to the sensor.
////! @param aTrackPtr [input] A pointer to the track being updated.
////! Hook to notify a derived class that a track representing a target being dropped.
////!
////! @param aSimTime The current simulation time.
////! @param aTrack   The track being dropped.
////protected virtual
// void WsfCoherentSensorProcessor::TrackDropped(double          aSimTime,
//                                     const WsfTrack& aTrack)
//{
//    //// NOTE: At one time the entry was cleared from the status list, but that was overly aggressive.
//    ////       The base class may not have a coast time and thus drop a track at the first failed
//    ////       detection, but then start a new track on the next frame for the same target.
//    //mStatusList.erase(aTrack.GetTargetIndex());
// }

// =================================================================================================
//! Hook to allow derived classes to update the track prior to it being sent.
//!
//! This is called by ProcessResults just before sending a track that reflects the perceived state
//! of a sensor result's target. The base class implementation does nothing except issue
//! some optional debug output. A derived class may provide an implementation to perform whatever
//! special processing they desire.
//!
//! @param aSimTime   The current simulation time.
//! @param aTrackPtr  A pointer to the track to be updated.
//! @param aSensorPtr Pointer to the sensor.
//! @param aTargetPtr Pointer to the target.
//! @param aResult    The results of the current detection attempt
// protected virtual
void WsfCoherentSensorProcessor::UpdateTrack(double           aSimTime,
                                             WsfTrack*        aTrackPtr,
                                             WsfSensor*       aSensorPtr,
                                             WsfPlatform*     aTargetPtr,
                                             WsfSensorResult& aResult)
{
   if (DebugEnabled() && (GetDebugMask() & WsfPlatformPart::cLEVEL_TWO_DEBUG))
   {
      auto logger = ut::log::debug() << "Updating track.";
      logger.AddNote() << "Target: " << aTrackPtr->GetTargetName();
      logger.AddNote() << "Target-type: " << aTrackPtr->GetTargetType();

      double lat, lon, alt;
      aTrackPtr->GetLocationLLA(lat, lon, alt);
      logger.AddNote() << "Lat: " << lat;
      logger.AddNote() << "Lon: " << lon;
      logger.AddNote() << "Alt: " << alt << " m";
      logger.AddNote() << "Sensor: " << aSensorPtr->GetName();
      logger.AddNote() << "Sensor-type: " << aSensorPtr->GetType();
      // if (aTrackPtr->VelocityValid())
      //{
      //    double velocityWCS[3];
      //    aTrackPtr->GetVelocityWCS(velocityWCS);
      //    cout << "; vel=" << velocityWCS[0] << ' ' << velocityWCS[1] << ' ' << velocityWCS[2];
      // }
      if (aResult.mSignalToNoise > 0.0)
      {
         logger.AddNote() << "SNR: " << UtMath::SafeLinearToDB(aResult.mSignalToNoise) << " db";
      }
   }
}

// -------------------------------------------------------------------------------------------------
//! Sets the fusion strategy used by this track manager.
void WsfCoherentSensorProcessor::SetFusionStrategy(WsfStringId aTypeName)
{
   auto& fusionTypes = WsfFusionStrategyTypes::Get(GetScenario());
   // set the new strategy only if it is not equivalent to the current one,
   // and if the new strategy represents an existing strategy type.
   if ((mFusionStrategyPtr->GetTypeId() != aTypeName) && fusionTypes.Find(aTypeName))
   {
      delete mFusionStrategyPtr;
      mFusionStrategyPtr = nullptr;
      mFusionStrategyPtr = fusionTypes.Create(aTypeName.GetString());
      if (!mFusionStrategyPtr->Initialize(this))
      {
         assert(false);
      }
      // TODO WsfObserver::FusionStrategyChanged(GetSimulation())(GetSimulation()->GetSimTime(), GetPlatform());
   }
}
