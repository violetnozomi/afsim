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

#include "WsfImageProcessor.hpp"

#include <cassert>
#include <iostream>
#include <iterator>

#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfFilter.hpp"
#include "WsfFilterTypes.hpp"
#include "WsfImage.hpp"
#include "WsfImageMessage.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackObserver.hpp"
#include "WsfVideoMessage.hpp"
#include "script/WsfScriptMessageHandler.hpp"

namespace
{
static const char* sStateName[] = {"UNDETECTED",
                                   "WAITING_DETECTION",
                                   "DETECTED",
                                   "WAITING_CLASSIFICATION",
                                   "CLASSIFIED",
                                   "WAITING_IDENTIFICATION",
                                   "IDENTIFIED"};
}

// =================================================================================================
//! Constructor.
WsfImageProcessor::WsfImageProcessor(WsfScenario& aScenario)
   : WsfMessageProcessor(aScenario)
   , mFilterPtr(nullptr)
   , mCoastTime(0.0)
   , mMessageLength(0)
   , mMessagePriority(0)
   , mReportsVelocity(true)
   , // TODO for now
   mReportsSide(false)
   , mReportsType(false)
   , mReportsBearingElevation(false)
   , mTargetRecognitionEnabled(false)
   , mIncludeUnstableCovariance(false)
   , mIncludeUnstableResidualCovariance(false)
   , mStateList()
   , mMinDetectPixelCount(0.0)
   , mMinClassPixelCount(0.0)
   , mMinIdentPixelCount(0.0)
   , mAverageAspectRatio(4.0)
   , mDetectionSAF(1.0)
   , mClassificationSAF(4.0)
   , mIdentificationSAF(6.4)
   , mDetectionDelayTime(0.0)
   , mClassificationDelayTime(0.0)
   , mIdentificationDelayTime(0.0)
   , mTransitionCoastTime(0.0)
   , mDetectionCoastTime(0.0)
   , mClassificationCoastTime(0.0)
   , mIdentificationCoastTime(0.0)
   , mShowStateTransitions(false)
   , mStatusList()
   , mObjectTest()
{
}

// =================================================================================================
//! Copy constructor for Clone().
// virtual
WsfImageProcessor::WsfImageProcessor(const WsfImageProcessor& aSrc)
   : WsfMessageProcessor(aSrc)
   , mFilterPtr(nullptr)
   , mCoastTime(aSrc.mCoastTime)
   , mMessageLength(aSrc.mMessageLength)
   , mMessagePriority(aSrc.mMessagePriority)
   , mReportsVelocity(aSrc.mReportsVelocity)
   , mReportsSide(aSrc.mReportsSide)
   , mReportsType(aSrc.mReportsType)
   , mReportsBearingElevation(aSrc.mReportsBearingElevation)
   , mTargetRecognitionEnabled(aSrc.mTargetRecognitionEnabled)
   , mIncludeUnstableCovariance(aSrc.mIncludeUnstableCovariance)
   , mIncludeUnstableResidualCovariance(aSrc.mIncludeUnstableResidualCovariance)
   , mStateList()
   , mMinDetectPixelCount(aSrc.mMinDetectPixelCount)
   , mMinClassPixelCount(aSrc.mMinClassPixelCount)
   , mMinIdentPixelCount(aSrc.mMinIdentPixelCount)
   , mAverageAspectRatio(aSrc.mAverageAspectRatio)
   , mDetectionSAF(aSrc.mDetectionSAF)
   , mClassificationSAF(aSrc.mClassificationSAF)
   , mIdentificationSAF(aSrc.mIdentificationSAF)
   , mDetectionDelayTime(aSrc.mDetectionDelayTime)
   , mClassificationDelayTime(aSrc.mClassificationDelayTime)
   , mIdentificationDelayTime(aSrc.mIdentificationDelayTime)
   , mTransitionCoastTime(aSrc.mTransitionCoastTime)
   , mDetectionCoastTime(aSrc.mDetectionCoastTime)
   , mClassificationCoastTime(aSrc.mClassificationCoastTime)
   , mIdentificationCoastTime(aSrc.mIdentificationCoastTime)
   , mShowStateTransitions(aSrc.mShowStateTransitions)
   , mStatusList()
   , mObjectTest()
{
   if (aSrc.mFilterPtr != nullptr)
   {
      mFilterPtr = aSrc.mFilterPtr->Clone();
   }
}

// =================================================================================================
//! Destructor
// virtual
WsfImageProcessor::~WsfImageProcessor()
{
   ClearStateList();
   delete mFilterPtr;
}

// =================================================================================================
// virtual
WsfProcessor* WsfImageProcessor::Clone() const
{
   return new WsfImageProcessor(*this);
}

// =================================================================================================
// virtual
bool WsfImageProcessor::Initialize(double aSimTime)
{
   bool              ok            = WsfMessageProcessor::Initialize(aSimTime);
   WsfScriptContext& globalContext = GetSimulation()->GetScriptContext();

   // Resolve possible script variable references
   ok &= mAverageAspectRatio.Initialize("average_aspect_ratio", this, mContextPtr, globalContext);
   ok &= mMinDetectPixelCount.Initialize("minimum_detection_pixel_count", this, mContextPtr, globalContext);
   ok &= mMinClassPixelCount.Initialize("minimum_classification_pixel_count", this, mContextPtr, globalContext);
   ok &= mMinIdentPixelCount.Initialize("minimum_identification_pixel_count", this, mContextPtr, globalContext);
   ok &= mDetectionSAF.Initialize("detection_scene_analysis_factor", this, mContextPtr, globalContext);
   ok &= mClassificationSAF.Initialize("classification_scene_analysis_factor", this, mContextPtr, globalContext);
   ok &= mIdentificationSAF.Initialize("identification_scene_analysis_factor", this, mContextPtr, globalContext);
   ok &= mDetectionDelayTime.Initialize("detection_delay_time", this, mContextPtr, globalContext);
   ok &= mClassificationDelayTime.Initialize("classification_delay_time", this, mContextPtr, globalContext);
   ok &= mIdentificationDelayTime.Initialize("identification_delay_time", this, mContextPtr, globalContext);
   ok &= mTransitionCoastTime.Initialize("transition_coast_time", this, mContextPtr, globalContext);
   ok &= mDetectionCoastTime.Initialize("detection_coast_time", this, mContextPtr, globalContext);
   ok &= mClassificationCoastTime.Initialize("classification_coast_time", this, mContextPtr, globalContext);
   ok &= mIdentificationCoastTime.Initialize("identification_coast_time", this, mContextPtr, globalContext);
   return ok;
}

// =================================================================================================
// virtual
bool WsfImageProcessor::ProcessInput(UtInput& aInput)
{
   WsfFilter*  filterPtr = nullptr;
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "coast_time")
   {
      aInput.ReadValueOfType(mCoastTime, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mCoastTime, 0.0);
   }
   else if (command == "message_length")
   {
      aInput.ReadValueOfType(mMessageLength, UtInput::cDATA_SIZE);
      aInput.ValueGreaterOrEqual(mMessageLength, 0);
   }
   else if (command == "message_priority")
   {
      aInput.ReadValue(mMessagePriority);
      aInput.ValueGreaterOrEqual(mMessagePriority, 0);
   }
   else if (command == "reports_velocity")
   {
      mReportsVelocity = true;
   }
   else if (command == "reports_side")
   {
      mReportsSide = true;
   }
   else if (command == "reports_type")
   {
      mReportsType = true;
   }
   else if (command == "reports_bearing_elevation")
   {
      mReportsBearingElevation = true;
   }
   else if (command == "reports_nothing")
   {
      mReportsBearingElevation = false;
      mReportsVelocity         = false;
      mReportsSide             = false;
      mReportsType             = false;
   }
   else if (command == "include_unstable_covariance")
   {
      aInput.ReadValue(mIncludeUnstableCovariance);
   }
   else if (command == "include_unstable_residual_covariance")
   {
      aInput.ReadValue(mIncludeUnstableResidualCovariance);
   }
   else if (WsfFilterTypes::Get(GetScenario()).LoadInstance(aInput, filterPtr))
   {
      delete mFilterPtr;
      mFilterPtr = filterPtr;
   }
   else if (command == "target_recognition")
   {
      aInput.ReadValue(mTargetRecognitionEnabled);
   }
   else if (command == "average_aspect_ratio")
   {
      mAverageAspectRatio.ReadValue(aInput);
      mAverageAspectRatio.ValueGreaterOrEqual(aInput, 0.0); // A value of zero indicates to use pixel count
   }
   else if ((command == "minimum_detection_pixel_count") || (command == "min_detection_pixel_count"))
   {
      mMinDetectPixelCount.ReadValue(aInput);
      mMinDetectPixelCount.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if ((command == "minimum_classification_pixel_count") || (command == "min_classification_pixel_count"))
   {
      mMinClassPixelCount.ReadValue(aInput);
      mMinClassPixelCount.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if ((command == "minimum_identification_pixel_count") || (command == "min_identification_pixel_count"))
   {
      mMinIdentPixelCount.ReadValue(aInput);
      mMinIdentPixelCount.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "detection_scene_analysis_factor")
   {
      mDetectionSAF.ReadValue(aInput);
      mDetectionSAF.ValueGreater(aInput, 0.0);
   }
   else if (command == "classification_scene_analysis_factor")
   {
      mClassificationSAF.ReadValue(aInput);
      mClassificationSAF.ValueGreater(aInput, 0.0);
   }
   else if (command == "identification_scene_analysis_factor")
   {
      mIdentificationSAF.ReadValue(aInput);
      mIdentificationSAF.ValueGreater(aInput, 0.0);
   }
   else if (command == "detection_delay_time")
   {
      mDetectionDelayTime.ReadValueOfType(aInput, UtInput::cTIME);
      mDetectionDelayTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "classification_delay_time")
   {
      mClassificationDelayTime.ReadValueOfType(aInput, UtInput::cTIME);
      mClassificationDelayTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "identification_delay_time")
   {
      mIdentificationDelayTime.ReadValueOfType(aInput, UtInput::cTIME);
      mIdentificationDelayTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "transition_coast_time")
   {
      mTransitionCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
      mTransitionCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "detection_coast_time")
   {
      mDetectionCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
      mDetectionCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "classification_coast_time")
   {
      mClassificationCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
      mClassificationCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "identification_coast_time")
   {
      mIdentificationCoastTime.ReadValueOfType(aInput, UtInput::cTIME);
      mIdentificationCoastTime.ValueGreaterOrEqual(aInput, 0.0);
   }
   else if (command == "show_state_transitions")
   {
      mShowStateTransitions = true;
   }
   else
   {
      myCommand = WsfMessageProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfImageProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   mSuppressMessage = false;

   // Perform any local processing; this may change the value of mSuppressMessge.
   bool processed = mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);

   // Execute base class' functionality for scripting based on this message.
   ProcessAction(aSimTime, aMessage);

   // Image and video messages are specially processed.
   // They are not forwarded unless explicitly specified in script.
   std::string msgType = aMessage.GetType().GetString();
   if (msgType == WsfImageMessage::GetMessageType())
   {
      const WsfImageMessage& message  = static_cast<const WsfImageMessage&>(aMessage);
      WsfImage*              imagePtr = message.GetImage();
      if (imagePtr != nullptr)
      {
         ProcessImage(aSimTime, *imagePtr);
         processed = true;
      }
   }
   else if (msgType == WsfVideoMessage::GetMessageType())
   {
      const WsfVideoMessage& message  = static_cast<const WsfVideoMessage&>(aMessage);
      WsfImage*              imagePtr = message.GetImage();
      if (imagePtr != nullptr)
      {
         ProcessImage(aSimTime, *imagePtr);
         processed = true;
      }
   }
   else if (DebugEnabled())
   {
      auto out = ut::log::warning() << "The WSF_IMAGE_PROCESSOR will not process.";
      out.AddNote() << "Messages: " << aMessage.GetType();
   }

   return processed;
}

// =================================================================================================
//! This implementation rejects messages that are not a WsfImageMessage or WsfVideoMessage.
// virtual
bool WsfImageProcessor::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool messageReceived(false);
   if ((aMessage.GetType() == WsfImageMessage::GetTypeId()) || (aMessage.GetType() == WsfVideoMessage::GetTypeId()))
   {
      messageReceived = WsfMessageProcessor::ReceiveMessage(aSimTime, aMessage);
   }
   return messageReceived;
}

// =================================================================================================
// virtual
void WsfImageProcessor::TurnOff(double aSimTime)
{
   ClearStateList();
   WsfMessageProcessor::TurnOff(aSimTime);
}

// =================================================================================================
//! Hook to notify a derived class the processing of an image has been completed.
//!
//! @param aSimTime The current simulation time.
//! @param aImage   The image that was processed.
// protected virtual
void WsfImageProcessor::ImageProcessingCompleted(double aSimTime, const WsfImage& aImage)
{
   if (mTargetRecognitionEnabled)
   {
      // Any status block that was not updated directly is forced to have a 'no-detect' update.
      for (auto& sli : mStatusList)
      {
         Status& status = sli.second;
         if (status.IsStale())
         {
            double coastTime = mTransitionCoastTime;
            switch (status.CurrentState())
            {
            case cST_DETECTED:
               coastTime = mDetectionCoastTime;
               break;
            case cST_CLASSIFIED:
               coastTime = mClassificationCoastTime;
               break;
            case cST_IDENTIFIED:
               coastTime = mIdentificationCoastTime;
               break;
            default:
               break;
            }
            if (status.CoastTimeExceeded(aSimTime, cST_UNDETECTED, coastTime))
            {
               TargetRecognitionState currentState = status.CurrentState();
               status.EnterState(cST_UNDETECTED, aSimTime, 0.0);

               if (mShowStateTransitions)
               {
                  auto out = ut::log::info() << "State Transition:";
                  out.AddNote() << "T = " << aSimTime;
                  out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  out.AddNote() << "Processor: " << GetName();
                  out.AddNote() << "Transition: " << status.TruthName();
                  out.AddNote() << "Old State: " << sStateName[currentState];
                  out.AddNote() << "New State: " << sStateName[status.CurrentState()];
                  out.AddNote() << "Entry Time: " << status.StateEntryTime();
               }
            }
         }
      }
   }
}

// =================================================================================================
//! Hook to notify a derived class the processing of an image is beginning.
//!
//! @param aSimTime The current simulation time.
//! @param aImage   The image that will be processed.
// protected virtual
void WsfImageProcessor::ImageProcessingInitiated(double aSimTime, const WsfImage& aImage)
{
   if (mTargetRecognitionEnabled)
   {
      // Mark all status blocks as potentially stale.
      for (auto& sli : mStatusList)
      {
         sli.second.SetStale();
      }
   }
}

// =================================================================================================
//! Hook to allow derived classes to determine if an object is visible.
//!
//! This is called by ProcessImage to allow the base class to determine if the object is even
//! detectable. The base class always returns true, but the derived class may provide other
//! functionality.
//!
//! @param aSimTime The current simulation time.
//! @param aImage   The image being processed.
//! @param aObject  The object within the image.
//!
//! @returns true if the object is detectable.
// protected virtual
bool WsfImageProcessor::IsDetectable(double aSimTime, const WsfImage& aImage, const WsfImage::Object& aObject)
{
   bool detected = true;
   if (mTargetRecognitionEnabled)
   {
      EvaluateObjectState(aSimTime, aImage, aObject);
      if (mObjectTest.GetResult() == "UNDETECTED")
      {
         detected = false;
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "Undetected:";
            out.AddNote() << "Object: " << aObject.GetTruthName(*GetSimulation());
            out.AddNote() << "Pixels: " << aObject.GetPixelCount();
         }
      }
   }
   return detected;
}

// =================================================================================================
//! Hook to notify a derived class that a track representing an object is being dropped.
//!
//! @param aSimTime The current simulation time.
//! @param aTrack   The track being dropped.
// protected virtual
void WsfImageProcessor::TrackDropped(double aSimTime, const WsfTrack& aTrack)
{
   if (mTargetRecognitionEnabled)
   {
      // NOTE: At one time the entry was cleared from the status list, but that was overly aggressive.
      //       The base class may not have a coast time and thus drop a track at the first failed
      //       detection, but then start a new track on the next frame for the same target.
      mStatusList.erase(aTrack.GetTargetIndex());
   }
}

// =================================================================================================
//! Hook to allow derived classes to update the track prior to it being sent.
//!
//! This is called by ProcessImage just before sending a track that reflects the perceived state
//! of an object extracted from the image. The base class implementation does nothing except issue
//! some optional debug output. A derived class may provide an implementation to perform whatever
//! special processing they desire.
//!
//! @param aSimTime The current simulation time.
//! @param aTrack   The track about to be sent.
//! @param aImage   The image being processed.
//! @param aObject  The object within the image.
// protected virtual
void WsfImageProcessor::UpdateTrack(double aSimTime, WsfTrack& aTrack, const WsfImage& aImage, const WsfImage::Object& aObject)
{
   if (DebugEnabled())
   {
      double lat;
      double lon;
      double alt;
      aTrack.GetLocationLLA(lat, lon, alt);

      auto out = ut::log::debug() << "Update Track:";
      out.AddNote() << "Target: " << aTrack.GetTargetName();
      out.AddNote() << "Type: " << aTrack.GetTargetType();
      out.AddNote() << "Lat: " << lat;
      out.AddNote() << "Lon: " << lon;
      out.AddNote() << "Alt: " << alt;
      if (aTrack.VelocityValid())
      {
         double velocityWCS[3];
         aTrack.GetVelocityWCS(velocityWCS);
         out.AddNote() << "Velocity: " << velocityWCS[0] << ' ' << velocityWCS[1] << ' ' << velocityWCS[2];
      }
      out.AddNote() << "Pixels: " << aObject.GetPixelCount();
      out.AddNote() << "Intensity: " << aObject.GetPixelIntensity();
      if ((aObject.GetSignalLevel() > 0.0) && (aImage.GetNoiseLevel() > 0.0))
      {
         out.AddNote() << "S/N: " << UtMath::SafeLinearToDB(aObject.GetSignalLevel() / aImage.GetNoiseLevel()) << " db";
      }
   }

   if (mTargetRecognitionEnabled)
   {
      // IsDetectable has already primed mObjectTest with the detection results.

      // Shouldn't happen... but just in case.
      if (mObjectTest.GetResult() == "UNDETECTED")
      {
         return;
      }

      // Determine if the master track list already contains a local track for the truth entity represented
      // by this object in the image. (This assumes perfect correlation).
      //
      // 'imageTrackAttrPtr' will point to the aux_data attributes for the image track.
      //
      // 'existingTrackAttrPtr' will point to the aux_data attributes as follows:
      // 1) From the corresponding local track if it exists
      // 2) The image track if a corresponding local track does not exist.

      const WsfAttributeContainer* existingTrackAttrPtr = &aTrack.GetAuxDataConst();
      WsfAttributeContainer*       imageTrackAttrPtr    = &(aTrack.GetAuxData());
      WsfLocalTrackList&           localTrackList       = GetPlatform()->GetMasterTrackList();
      for (unsigned int entry = 0; entry < localTrackList.GetTrackCount(); ++entry)
      {
         WsfLocalTrack* localTrackPtr = localTrackList.GetTrackEntry(entry);
         if (localTrackPtr->GetTargetIndex() == aTrack.GetTargetIndex())
         {
            existingTrackAttrPtr = &localTrackPtr->GetAuxDataConst();
            break;
         }
      }

      // Add the CLASSIFIED and IDENTIFIED aux data state attributes to the 'image track'
      // if they don't already exist and initialize them to not-identified/not-classified.

      if (!imageTrackAttrPtr->AttributeExists("IDENTIFIED"))
      {
         imageTrackAttrPtr->Assign("IDENTIFIED", -1.0);
      }
      if (!imageTrackAttrPtr->AttributeExists("CLASSIFIED"))
      {
         imageTrackAttrPtr->Assign("CLASSIFIED", -1.0);
      }

      // Determine the new state of the track.
      //
      // We will never cause the state of a track to go 'backward' (i.e.: CLASSIFIED->DETECTED, etc.).
      // Any degradation of in the local track must be done externally.

      if (existingTrackAttrPtr->AttributeExists("IDENTIFIED") && existingTrackAttrPtr->AttributeExists("CLASSIFIED"))
      {
         // One of two cases:
         //
         // 1) There is no associated local track.
         // 2) There is an associated local track and it contains classification/identification data.

         if ((existingTrackAttrPtr->GetDouble("IDENTIFIED") >= 0.0) || (mObjectTest.GetResult() == "IDENTIFIED")) // IDENTIFIED
         {
            if (existingTrackAttrPtr->GetDouble("IDENTIFIED") >= 0.0)
            {
               mObjectTest.SetPreviouslyIdentified(true);
            }
            imageTrackAttrPtr->Assign("IDENTIFIED", aSimTime);
            imageTrackAttrPtr->Assign("CLASSIFIED", aSimTime);
            aTrack.SetTypeId(aObject.GetTruthType());
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("IDENTIFIED");
         }
         else if ((existingTrackAttrPtr->GetDouble("CLASSIFIED") >= 0.0) ||
                  (mObjectTest.GetResult() == "CLASSIFIED")) // CLASSIFIED
         {
            if (existingTrackAttrPtr->GetDouble("CLASSIFIED") >= 0.0)
            {
               mObjectTest.SetPreviouslyClassified(true);
            }
            imageTrackAttrPtr->Assign("IDENTIFIED", -1.0);
            imageTrackAttrPtr->Assign("CLASSIFIED", aSimTime);
            aTrack.SetTypeId(aObject.GetTruthType());
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("CLASSIFIED");
         }
         else // DETECTED
         {
            imageTrackAttrPtr->Assign("IDENTIFIED", -1.0);
            imageTrackAttrPtr->Assign("CLASSIFIED", -1.0);
            aTrack.SetTypeId(WsfStringId("UNKNOWN")); // unknown type
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("DETECTED");
         }
      }
      else
      {
         // A local track already exists for this target, but there isn't any classification/identification
         // aux data associated with the track.  Update the state in the 'object track', which will then get
         // fused into the local track by the track manager.

         if (mObjectTest.GetResult() == "IDENTIFIED") // Identified
         {
            imageTrackAttrPtr->Assign("IDENTIFIED", aSimTime);
            imageTrackAttrPtr->Assign("CLASSIFIED", aSimTime);
            aTrack.SetTypeId(aObject.GetTruthType());
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("IDENTIFIED");
         }
         else if (mObjectTest.GetResult() == "CLASSIFIED") // Classified
         {
            imageTrackAttrPtr->Assign("IDENTIFIED", -1.0);
            imageTrackAttrPtr->Assign("CLASSIFIED", aSimTime);
            aTrack.SetTypeId(aObject.GetTruthType());
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("CLASSIFIED");
         }
         else // Detected
         {
            imageTrackAttrPtr->Assign("IDENTIFIED", -1.0);
            imageTrackAttrPtr->Assign("CLASSIFIED", -1.0);
            aTrack.SetTypeId(WsfStringId("UNKNOWN"));
            aTrack.SetTypeIdValid(true);
            mObjectTest.SetFinalResult("DETECTED");
         }
      }

      mObjectTest.SetPerceivedType(aTrack.GetTypeId().GetString());

      if (DebugEnabled())
      {
         const WsfAttributeContainer* attPtr = &aTrack.GetAuxDataConst();
         auto                         out    = ut::log::debug() << "Detected target.";
         out.AddNote() << "Target: " << aTrack.GetTargetName();
         out.AddNote() << "Perceived Type: " << aTrack.GetTargetType();
         if (attPtr->AttributeExists("CLASSIFIED"))
         {
            out.AddNote() << "Classified: " << attPtr->GetDouble("CLASSIFIED");
         }
         else
         {
            out.AddNote() << "Classified: UNKNOWN";
         }
         if (attPtr->AttributeExists("IDENTIFIED"))
         {
            out.AddNote() << "Identified: " << attPtr->GetDouble("IDENTIFIED");
         }
         else
         {
            out.AddNote() << "Identified: UNKNOWN";
         }
         double lat;
         double lon;
         double alt;
         aTrack.GetLocationLLA(lat, lon, alt);
         out.AddNote() << "Lat: " << lat;
         out.AddNote() << "Lon: " << lon;
         out.AddNote() << "Alt: " << alt;

         if (aTrack.VelocityValid())
         {
            double velocityWCS[3];
            aTrack.GetVelocityWCS(velocityWCS);
            out.AddNote() << "Velocity: " << velocityWCS[0] << ' ' << velocityWCS[1] << ' ' << velocityWCS[2];
         }
         out.AddNote() << "Pixels: " << aObject.GetPixelCount();
         out.AddNote() << "Intensity: " << aObject.GetPixelIntensity();
      }
   }
}

// =================================================================================================
//! Compute the probability of detection using the Johnson criteria
// private
double WsfImageProcessor::ComputeProbabilityOfDetection(double aObjectSize) const
{
   double sizeRatio  = aObjectSize / mDetectionSAF;
   double commonTerm = pow(sizeRatio, (2.7 + 0.7 * sizeRatio));
   return (commonTerm / (1 + commonTerm));
}

// =================================================================================================
//! Compute the probability of classification using the Johnson criteria
// private
double WsfImageProcessor::ComputeProbabilityOfClassification(double aObjectSize) const
{
   double sizeRatio  = aObjectSize / mClassificationSAF;
   double commonTerm = pow(sizeRatio, (2.7 + 0.7 * sizeRatio));
   return (commonTerm / (1 + commonTerm));
}

// =================================================================================================
//! Compute the probability of identification using the Johnson criteria
// private
double WsfImageProcessor::ComputeProbabilityOfIdentification(double aObjectSize) const
{
   double sizeRatio  = aObjectSize / mIdentificationSAF;
   double commonTerm = pow(sizeRatio, (2.7 + 0.7 * sizeRatio));
   return (commonTerm / (1 + commonTerm));
}

// =================================================================================================
// private
void WsfImageProcessor::ClearStateList()
{
   for (auto& sli : mStateList)
   {
      State* statePtr = sli.second;
      delete statePtr->mTrackPtr;
      delete statePtr->mFilterPtr;
      delete statePtr;
   }
   mStateList.clear();
}

// =================================================================================================
// private
void WsfImageProcessor::ProcessImage(double aSimTime, WsfImage& aImage)
{
   // Try to locate the producing sensor (if it still exists).
   WsfSensor*   sensorPtr(nullptr);
   WsfPlatform* platformPtr(GetSimulation()->GetPlatformByIndex(aImage.GetPlatformIndex()));
   if (platformPtr != nullptr)
   {
      sensorPtr = platformPtr->GetComponent<WsfSensor>(aImage.GetSensorNameId());
   }

   // Get the location of the sensor at the time of the report.
   double sensorLocWCS[3];
   aImage.GetOriginatorLocationWCS(sensorLocWCS);

   // Get the objects list from the image
   const WsfImage::ObjectList& objectList = aImage.GetObjects();

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Image processed by processor.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Sensor: " << aImage.GetSensorNameId();
      out.AddNote() << "Type: " << aImage.GetSensorTypeId();
      out.AddNote() << "Mode: " << aImage.GetSensorModeId();
      out.AddNote() << "Size: " << aImage.GetWidth() << " x " << aImage.GetHeight() << " pixels";
      out.AddNote() << "Resolution: " << aImage.GetWidthResolution() << " x " << aImage.GetHeightResolution() << " m";

      if (aImage.GetNoiseLevel() > 0.0)
      {
         out.AddNote() << "Noise Level: " << UtMath::LinearToDB(aImage.GetNoiseLevel()) << " db";
      }
      if (aImage.GetBackgroundLevel() > 0.0)
      {
         out.AddNote() << "Background Level: " << UtMath::LinearToDB(aImage.GetBackgroundLevel()) << " db";
      }
   }

   ImageProcessingInitiated(aSimTime, aImage); // Notify derived classes

   bool isVideoStream = (aImage.GetImageNumber() > 0);

   // Process all the object in the image
   auto objectIter    = objectList.begin();
   auto objectIterEnd = objectList.end();
   for (; objectIter != objectIterEnd; ++objectIter)
   {
      const WsfImage::Object& object = *objectIter;

      // Invoke the virtual method that lets a derived class to let it determine if the object
      // is detectable. If not we just skip it.
      if (!IsDetectable(aSimTime, aImage, object))
      {
         continue;
      }

      // The object is detectable. Allocate a track if one does not already exist for this object.
      size_t     platformIndex = object.GetTruthIndex();
      WsfTrack*  trackPtr      = nullptr;
      WsfFilter* filterPtr     = nullptr;
      bool       trackUpdated  = false;
      if (isVideoStream)
      {
         auto sli = mStateList.find(platformIndex);
         if (sli != mStateList.end())
         {
            // We already have a track on this object.
            State* statePtr = (*sli).second;
            trackPtr        = statePtr->mTrackPtr;
            filterPtr       = statePtr->mFilterPtr;

            // If there is a filter and if the stream number has changed, reset the filter.
            if ((filterPtr != nullptr) && (statePtr->mStreamNumber != aImage.GetStreamNumber()))
            {
               statePtr->mStreamNumber = aImage.GetStreamNumber();
               filterPtr->Reset(aSimTime);
               trackPtr->SetStateCovariance(aSimTime, nullptr);
               trackPtr->SetResidualCovariance(nullptr);
            }

            trackPtr->Update(aImage.GetImageTime());
            trackUpdated = true;
         }
         else
         {
            // We don't have a track on this object, so start one.
            trackPtr = new WsfTrack(WsfTrack::cPROCESSED, this, this->GetPlatform());
            trackPtr->Initialize(aImage.GetImageTime(), GetPlatform()->GetNextTrackId(), *GetSimulation());
            State* statePtr = new State();
            if (mFilterPtr != nullptr)
            {
               filterPtr = mFilterPtr->Clone();
               filterPtr->Initialize(aSimTime, GetSimulation());
            }
            statePtr->mTrackPtr       = trackPtr;
            statePtr->mFilterPtr      = filterPtr;
            statePtr->mStreamNumber   = aImage.GetStreamNumber();
            mStateList[platformIndex] = statePtr;
         }
      }
      else
      {
         // This is a static image (e.g.: spot SAR or photograph).
         // The resulting track is a 'one-shot' (i.e.: no subsequent updates)
         trackPtr = new WsfTrack(WsfTrack::cPROCESSED, this, GetPlatform());
         trackPtr->Initialize(aImage.GetImageTime(), GetPlatform()->GetNextTrackId(), *GetSimulation());
      }
      trackPtr->SetMessageDataTag(GetSimulation()->CreateMessageDataTag());

      // The originator of the track is the processor, but one often needs to know information
      // about the sensor. This is kind of a hybrid track - it is a directly derived product
      // of the sensor, but it isn't a true 'sensor measurement' like a radar.
      trackPtr->SetSensorOriginator(aImage.GetSensorNameId(), aImage.GetSensorTypeId());
      trackPtr->SetSensorModeId(aImage.GetSensorModeId());

      // Set the location at the time of the report.
      trackPtr->SetOriginatorLocationWCS(sensorLocWCS);

      // Get the measured object location.
      double locationWCS[3];
      object.GetLocationWCS(locationWCS);

      // If this is a video stream with a filter, update the filter with the measured location.
      double velocityWCS[3] = {0.0, 0.0, 0.0};
      bool   velocityValid  = false;
      if (isVideoStream)
      {
         if (filterPtr != nullptr)
         {
            filterPtr->SetFrameTime(aImage.GetFrameTime());

            // Note: WsfImage assumes bearing error is azimuth error (i.e., the sensor is horizontal).
            // This is not necessarily the case and should be investigated further (todo).
            trackPtr->SetSensorAzimuthError(aImage.GetBearingError());
            trackPtr->SetSensorElevationError(aImage.GetElevationError());
            trackPtr->SetRangeError(aImage.GetRangeError());
            trackPtr->SetRangeRateError(aImage.GetRangeRateError());
            trackPtr->SetLocationWCS(locationWCS);

            UtCovariance measurementCovar(3, 3);
            measurementCovar.ComputeMeasurementCovarianceWCS(*trackPtr);
            trackPtr->SetMeasurementCovariance(aSimTime, measurementCovar);

            UtMeasurementData estimateOut;
            filterPtr->Update(aSimTime, *trackPtr, estimateOut);

            if (estimateOut.LocationValid())
            {
               UtVec3d::Set(locationWCS, estimateOut.GetLocationWCS().GetData());
            }
            if (estimateOut.VelocityValid())
            {
               UtVec3d::Set(velocityWCS, estimateOut.GetVelocityWCS().GetData());
            }

            // Update the track's state covariance matrix, if it's available from the filter.
            if (filterPtr->GetStateCovariance() != nullptr)
            {
               if (filterPtr->IsStable() || mIncludeUnstableCovariance)
               {
                  trackPtr->SetStateCovariance(aSimTime, *(filterPtr->GetStateCovariance()));
               }

               if (filterPtr->GetResidualCovariance() != nullptr)
               {
                  if (filterPtr->IsStable() || mIncludeUnstableResidualCovariance)
                  {
                     trackPtr->SetResidualCovariance(*(filterPtr->GetResidualCovariance()));
                  }
               }
            }

            // Don't report velocity until the filter is 'stable' (which is generally the 1st or 2nd update after
            // following the initial update.
            if (filterPtr->IsStable())
            {
               velocityValid = true;
            }

            // Report this is a filtered sensor report to prevent the track manager from performing more filtering
            // if it has an associated filter.
            trackPtr->SetTrackType(WsfTrack::cFILTERED_SENSOR);
         }
         else if (mReportsVelocity && trackUpdated) // Only report starting with first update following creation
         {
            // If a filter is not provided, truth velocity is reported if 'reports_velocity' is specified.
            WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(platformIndex);
            if (targetPtr != nullptr)
            {
               targetPtr->GetVelocityWCS(velocityWCS);
               velocityValid = true;
            }
         }
      }
      else
      {
         // Report static images as a 'filtered' sensor report to prevent the track manager from filtering the results.
         trackPtr->SetTrackType(WsfTrack::cSTATIC_IMAGE);
         velocityValid = true;
      }

      if (mReportsBearingElevation)
      {
         // Update the track with new bearing / elevation
         double locationWCS[3];
         object.GetLocationWCS(locationWCS);
         double targetVecWCS[3];
         UtVec3d::Subtract(targetVecWCS, locationWCS, sensorLocWCS);
         double range = UtVec3d::Magnitude(targetVecWCS);
         if ((range > 0.0) && // should be true
             (sensorPtr != nullptr))
         {
            double targetVecPCS[3];
            double targetVecECS[3];
            sensorPtr->ConvertWCSVectorToPCS(targetVecPCS, targetVecWCS);
            sensorPtr->ConvertPCSVectorToECS(targetVecECS, targetVecPCS);
            double bearing = atan2(targetVecECS[1], targetVecECS[0]);
            trackPtr->SetBearing(bearing);
            trackPtr->SetBearingValid(true);
            double elevation = -asin(targetVecECS[2] / range);
            trackPtr->SetElevation(elevation);
            trackPtr->SetElevationValid(true);

            // todo introduce bearing / elevation error from sensor characteristic
         }
         else
         {
            // TODO Upon further review in WSF 2.0, I'm not sure this is a good idea as it will have
            // TODO a track that does not have any valid location data. The point may be moot, however,
            // TODO in that the range should always be non-zero and most likely the producing sensor
            // TODO still exists. (JAJ)
            trackPtr->SetBearingValid(false);
            trackPtr->SetElevationValid(false);
         }
      }
      else // reports location
      {
         // Update the track with new (possibly filtered) location.
         trackPtr->SetLocationWCS(locationWCS);
         trackPtr->SetWCS_LocationValid(true);
         trackPtr->Set3D(true);
      }

      // Update the track with the velocity.
      // (For static images, or streams with no corresponding filter, the reported velocity will be zero).
      trackPtr->SetVelocityWCS(velocityWCS);
      trackPtr->SetVelocityValid(velocityValid);

      // Propagate the sensor measurement errors to the resulting track.
      trackPtr->SetRangeError(aImage.GetRangeError());
      trackPtr->SetBearingError(aImage.GetBearingError());
      trackPtr->SetElevationError(aImage.GetElevationError());
      trackPtr->SetRangeRateError(aImage.GetRangeRateError());

      // Propagate the sensor mode track quality to the resulting track
      trackPtr->SetTrackQuality(aImage.GetTrackQuality());

      // Update the track with the signal-to-noise.
      if ((object.GetSignalLevel() > 0.0) && (aImage.GetNoiseLevel() > 0.0))
      {
         trackPtr->SetSignalToNoise(object.GetSignalLevel() / aImage.GetNoiseLevel());
         trackPtr->SetSignalToNoiseValid(true);
      }

      // Update target pixel count.
      trackPtr->SetPixelCount(object.GetPixelCount());

      // Report the side if requested.
      if (mReportsSide)
      {
         trackPtr->SetSideId(object.GetTruthSide());
         trackPtr->SetSideIdValid(true);
      }

      // Report the type if requested.
      if (mReportsType)
      {
         trackPtr->SetTypeId(object.GetTruthType());
         trackPtr->SetTypeIdValid(true);
      }

      // Update the target truth data.
      trackPtr->SetTargetIndex(platformIndex);
      trackPtr->SetTargetName(GetSimulation()->GetPlatformNameId(platformIndex));
      trackPtr->SetTargetType(object.GetTruthType());

      // Invoke the virtual method that lets a derived classes update the track.
      UpdateTrack(aSimTime, *trackPtr, aImage, object);

      // Issue the sensor track initiate/update observer calls if the sensor still exists.
      if (sensorPtr != nullptr)
      {
         if (trackUpdated)
         {
            WsfObserver::SensorTrackUpdated(GetSimulation())(aSimTime, sensorPtr, trackPtr);
         }
         else
         {
            WsfObserver::SensorTrackInitiated(GetSimulation())(aSimTime, sensorPtr, trackPtr);
         }
      }

      // Send the track to all attached objects.
      // Note: Generally message suppression will not be enabled, but
      // it is allowed.
      if (!mSuppressMessage)
      {
         WsfTrackMessage message(GetPlatform(), *trackPtr);
         message.SetSizeBits(mMessageLength);
         message.SetPriority(mMessagePriority);
         SendMessage(aSimTime, message);
      }

      // The tracks related to static images are not persistent
      if (!isVideoStream)
      {
         delete trackPtr;
      }
   }

   PurgeOldTracks(aSimTime, aImage.GetImageTime(), sensorPtr);

   ImageProcessingCompleted(aSimTime, aImage); // Notify derived classes
}

// =================================================================================================
// private
void WsfImageProcessor::PurgeOldTracks(double aSimTime, double aImageTime, WsfSensor* aSensorPtr)
{
   auto sli = mStateList.begin();
   while (sli != mStateList.end())
   {
      State*    statePtr            = (*sli).second;
      WsfTrack* trackPtr            = statePtr->mTrackPtr;
      double    timeSinceLastUpdate = aImageTime - trackPtr->GetUpdateTime();
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
         delete statePtr->mFilterPtr;
         delete statePtr;
         mStateList.erase(sli++);
      }
      else
      {
         ++sli;
      }
   }
}

// =================================================================================================
// private
void WsfImageProcessor::EvaluateObjectState(double aSimTime, const WsfImage& aImage, const WsfImage::Object& aObject)
{
   mObjectTest.SetObject(&aObject, &aImage);
   mObjectTest.SetFinalResult("UNDETECTED");

   // Draw the random value used for determining detection/classification/identification
   double draw = GetSimulation()->GetRandom().Uniform<double>();
   mObjectTest.SetDraw(draw);

   // Use the Johnson criteria to determine the probability of detection/classification/identification.

   double pixelCount = aObject.GetPixelCount();
   double objectSize = pixelCount; // Assume using pixel count as object size
   if (mAverageAspectRatio > 0.0)
   {
      // Use the number of line bars/line pairs for the object size.
      // This is approximated using the shortest dimension of the object assuming an average aspect ratio
      //
      // Let: PC = pixel count, AR = average aspect ratio, SD = shortest dimension (in pixels)
      //      => LD = longest dimension (in pixels) = AR * SD
      //
      // PC = LD * SD = (AR * SD) * SD
      // SD = sqrt(PC / AR)
      // #Lines = SD
      // #LinePairs (line bars) = sqrt(PC / AR) / 2
      objectSize = 0.5 * sqrt(pixelCount / mAverageAspectRatio);
   }
   double reqDetected   = ComputeProbabilityOfDetection(objectSize);
   double reqClassified = ComputeProbabilityOfClassification(objectSize);
   double reqIdentified = ComputeProbabilityOfIdentification(objectSize);
   mObjectTest.SetRequiredDetected(reqDetected);
   mObjectTest.SetRequiredClassified(reqClassified);
   mObjectTest.SetRequiredIdentified(reqIdentified);

   Status status(aObject.GetTruthName(*GetSimulation()));
   size_t targetIndex = aObject.GetTruthIndex();
   auto   sli         = mStatusList.find(targetIndex);
   if (sli != mStatusList.end())
   {
      status = sli->second;
   }

   // Classify the detection status from the current frame.

   TargetRecognitionState frameState = cST_UNDETECTED;
   if ((draw <= reqIdentified) && (pixelCount >= mMinIdentPixelCount))
   {
      frameState = cST_IDENTIFIED;
   }
   else if ((draw <= reqClassified) && (pixelCount >= mMinClassPixelCount))
   {
      frameState = cST_CLASSIFIED;
   }
   else if ((draw <= reqDetected) && (pixelCount >= mMinDetectPixelCount))
   {
      frameState = cST_DETECTED;
   }

   if (mShowStateTransitions)
   {
      auto out = ut::log::info() << "Checking state.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Processor: " << GetName();
      out.AddNote() << "Frame State: " << sStateName[frameState];
      out.AddNote() << "Object State: " << sStateName[status.CurrentState()];
   }

   // Update the running detection status.
   //
   // NOTE: A timeout is declared when a 'detection of the required quality' is not encountered during the
   //       the timeout window. For instance, if in the CLASSIFIED state, the detections must be at least
   //       of CLASSIFED state. If, during the timeout window, such detections do not occur then a transition
   //       will be made to the highest state encountered during the timeout window.
   // encountered during the time a

   TargetRecognitionState currentState = cST_UNDETECTED;
   do
   {
      currentState = status.CurrentState();
      switch (status.CurrentState())
      {
      case cST_UNDETECTED:
         if (frameState >= cST_DETECTED)
         {
            status.EnterState(cST_WAITING_DETECTION, aSimTime, mDetectionDelayTime);
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else
         {
            status.SetLastGoodUpdateTime(-1.0E+30);
         }
         break;

      case cST_WAITING_DETECTION:
         if (frameState >= cST_DETECTED)
         {
            if (aSimTime >= status.EarliestStateExitTime())
            {
               status.EnterState(cST_DETECTED, aSimTime, 0.0);
            }
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mTransitionCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;

      case cST_DETECTED:
         if (frameState >= cST_DETECTED)
         {
            if (frameState >= cST_CLASSIFIED)
            {
               status.EnterState(cST_WAITING_CLASSIFICATION, aSimTime, mClassificationDelayTime);
            }
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mDetectionCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;

      case cST_WAITING_CLASSIFICATION:
         if (frameState >= cST_CLASSIFIED)
         {
            if (aSimTime >= status.EarliestStateExitTime())
            {
               status.EnterState(cST_CLASSIFIED, aSimTime, 0.0);
            }
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mTransitionCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;

      case cST_CLASSIFIED:
         if (frameState >= cST_CLASSIFIED)
         {
            if (frameState >= cST_IDENTIFIED)
            {
               status.EnterState(cST_WAITING_IDENTIFICATION, aSimTime, mIdentificationDelayTime);
            }
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mClassificationCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;

      case cST_WAITING_IDENTIFICATION:
         if (frameState >= cST_IDENTIFIED)
         {
            if (aSimTime >= status.EarliestStateExitTime())
            {
               status.EnterState(cST_IDENTIFIED, aSimTime, 0.0);
            }
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mTransitionCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;

      case cST_IDENTIFIED:
         if (frameState >= cST_IDENTIFIED)
         {
            status.SetLastGoodUpdateTime(aSimTime);
         }
         else if (status.CoastTimeExceeded(aSimTime, frameState, mIdentificationCoastTime))
         {
            status.EnterState(status.CoastingState(), aSimTime, 0.0);
         }
         break;
      }

      if (mShowStateTransitions && (currentState != status.CurrentState()))
      {
         auto out = ut::log::info() << "Transitioning states.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Processor: " << GetName();
         out.AddNote() << "Transition: " << status.TruthName();
         out.AddNote() << "Old State: " << status.TruthName();
         out.AddNote() << "New State: " << sStateName[currentState];
         out.AddNote() << "Entry Time: " << status.StateEntryTime();
      }
   } while (currentState != status.CurrentState());

   mStatusList[targetIndex] = status;

   // Set the result for the current detection.

   switch (status.CurrentState())
   {
   case cST_IDENTIFIED:
      mObjectTest.SetResult("IDENTIFIED");
      break;
   case cST_WAITING_IDENTIFICATION:
   case cST_CLASSIFIED:
      mObjectTest.SetResult("CLASSIFIED");
      break;
   case cST_WAITING_CLASSIFICATION:
   case cST_DETECTED:
      mObjectTest.SetResult("DETECTED");
      break;
   default:
      mObjectTest.SetResult("UNDETECTED");
      break;
   }
}

// =================================================================================================
// public
bool WsfImageProcessor::Status::CoastTimeExceeded(double aSimTime, TargetRecognitionState aState, double aCoastTime)
{
   if (mCoastingStartTime < 0.0)
   {
      mCoastingStartTime = aSimTime;
      mCoastingState     = aState;
   }
   else if (aState > mCoastingState)
   {
      mCoastingState = aState;
   }
   mIsStale = false;
   return ((aSimTime - mCoastingStartTime) >= aCoastTime);
}
