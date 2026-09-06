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

#ifndef WSFIMAGEPROCESSOR_HPP
#define WSFIMAGEPROCESSOR_HPP

#include "wsf_mil_export.h"

class WsfFilter;
#include "WsfImage.hpp"
#include "WsfMessageProcessor.hpp"
class WsfSensor;
class WsfTrack;

//! A specialization of WsfMessageProcessor that implements an example image processor.
class WSF_MIL_EXPORT WsfImageProcessor : public WsfMessageProcessor
{
public:
   //! @name Target recognition attributes.
   //@{
   enum TargetRecognitionState
   {
      cST_UNDETECTED,
      cST_WAITING_DETECTION,
      cST_DETECTED,
      cST_WAITING_CLASSIFICATION,
      cST_CLASSIFIED,
      cST_WAITING_IDENTIFICATION,
      cST_IDENTIFIED
   };

   WsfImageProcessor(WsfScenario& aScenario);
   ~WsfImageProcessor() override;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   bool ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;

   void TurnOff(double aSimTime) override;

   //! Return the value of 'coast_time'.
   double GetCoastTime() const { return mCoastTime; }

   class ObjectTest
   {
   public:
      ObjectTest() = default;

      void SetObject(const WsfImage::Object* aObjectPtr, const WsfImage* aImagePtr)
      {
         mObjectPtr     = aObjectPtr;
         mImagePtr      = aImagePtr;
         mReqDetected   = 0.0;
         mReqClassified = 0.0;
         mReqIdentified = 0.0;
         mDraw          = 1.0;

         mResult               = "UNDETECTED";
         mFinalResult          = "UNDETECTED";
         mPreviouslyClassified = false;
         mPreviouslyIdentified = false;
      }

      std::string GetResult() const { return mResult; }
      void        SetResult(const std::string& aResult) { mResult = aResult; }
      std::string GetFinalResult() const { return mFinalResult; }
      void        SetFinalResult(const std::string& aResult) { mFinalResult = aResult; }
      std::string GetPerceivedType() const { return mPerceivedType; }
      void        SetPerceivedType(const std::string& aPerceivedType) { mPerceivedType = aPerceivedType; }

      double GetDraw() const { return mDraw; }
      double GetRequiredDetected() const { return mReqDetected; }
      double GetRequiredClassified() const { return mReqClassified; }
      double GetRequiredIdentified() const { return mReqIdentified; }

      void SetDraw(double aDraw) { mDraw = aDraw; }
      void SetRequiredDetected(double aReqDetected) { mReqDetected = aReqDetected; }
      void SetRequiredClassified(double aReqClassified) { mReqClassified = aReqClassified; }
      void SetRequiredIdentified(double aReqIdentified) { mReqIdentified = aReqIdentified; }

      bool GetPreviouslyClassified() const { return mPreviouslyClassified; }
      bool GetPreviouslyIdentified() const { return mPreviouslyIdentified; }
      void SetPreviouslyClassified(bool aPreviouslyClassified) { mPreviouslyClassified = aPreviouslyClassified; }
      void SetPreviouslyIdentified(bool aPreviouslyIdentified) { mPreviouslyIdentified = aPreviouslyIdentified; }

      const WsfImage::Object* mObjectPtr{nullptr};
      const WsfImage*         mImagePtr{nullptr};

   private:
      std::string mResult;
      std::string mFinalResult;
      std::string mPerceivedType;
      double      mReqDetected{0.0};
      double      mReqClassified{0.0};
      double      mReqIdentified{0.0};
      double      mDraw{0.0};

      bool mPreviouslyClassified{false};
      bool mPreviouslyIdentified{false};
   };

   class State
   {
   public:
      WsfTrack*    mTrackPtr{nullptr};
      WsfFilter*   mFilterPtr{nullptr};
      unsigned int mStreamNumber{0};
   };

   class Status
   {
   public:
      Status(WsfStringId aTruthName = WsfStringId())
         : mStateEntryTime(-1.0E+30)
         , mMinimumTimeInState(0.0)
         , mLastGoodUpdateTime(-1.0E+30)
         , mCurrentState(cST_UNDETECTED)
         , mCoastingStartTime(-1.0E+30)
         , mCoastingState(cST_UNDETECTED)
         , mTruthName(aTruthName)
         , mIsStale(false)
      {
      }

      WsfStringId TruthName() const { return mTruthName; }

      void EnterState(TargetRecognitionState aNewState, double aStateEntryTime, double aMinimumTimeInState)
      {
         mCurrentState       = aNewState;
         mStateEntryTime     = aStateEntryTime;
         mMinimumTimeInState = aMinimumTimeInState;
      }

      void SetLastGoodUpdateTime(double aSimTime)
      {
         mLastGoodUpdateTime = aSimTime;
         mCoastingStartTime  = -1.0E-30;
         mIsStale            = false;
      }

      void SetStale() { mIsStale = true; }
      bool IsStale() const { return mIsStale; }

      //! Updates the coasting status.
      //! @returns true if the coast time limit has been exceeded, or false if coasting can continue.
      bool CoastTimeExceeded(double aSimTime, TargetRecognitionState aState, double aCoastTime);

      TargetRecognitionState CoastingState() const { return mCoastingState; }

      TargetRecognitionState CurrentState() const { return mCurrentState; }
      double                 StateEntryTime() const { return mStateEntryTime; }

      double EarliestStateExitTime() const { return mStateEntryTime + mMinimumTimeInState; }

      //! The time when the transition occurred into the current state.
      double mStateEntryTime{-1.0E+30};
      //! The minimum time that must be spent in the current state before a transition to a higher state can occur.
      double mMinimumTimeInState{0.0};
      //! The last time when a detection was registered that met the criteria for the current state.
      double mLastGoodUpdateTime{-1.0E+30};
      //! The current state
      TargetRecognitionState mCurrentState{cST_UNDETECTED};
      //! The time when coasting started.
      double mCoastingStartTime{-1.0E+30};
      //! The state (quality) of the update associated with mLastFailedUpdateTime;
      TargetRecognitionState mCoastingState{cST_UNDETECTED};
      //! The name id of the underlying platform represented by the object.
      WsfStringId mTruthName{nullptr};
      //! True if the object is stale (didn't exist in the last image).
      bool mIsStale{false};
   };

protected:
   WsfImageProcessor(const WsfImageProcessor& aSrc);
   WsfImageProcessor& operator=(const WsfImageProcessor&) = delete;

   virtual void ImageProcessingCompleted(double aSimTime, const WsfImage& aImage);

   virtual void ImageProcessingInitiated(double aSimTime, const WsfImage& aImage);

   virtual bool IsDetectable(double aSimTime, const WsfImage& aImage, const WsfImage::Object& aObject);

   virtual void TrackDropped(double aSimTime, const WsfTrack& aTrack);

   virtual void UpdateTrack(double aSimTime, WsfTrack& aTrack, const WsfImage& aImage, const WsfImage::Object& aObject);

private:
   void ClearStateList();

   void ProcessImage(double aSimTime, WsfImage& aImage);

   void PurgeOldTracks(double aSimTime, double aImageTime, WsfSensor* aSensorPtr);

   double ComputeProbabilityOfDetection(double aObjectSize) const;
   double ComputeProbabilityOfClassification(double aObjectSize) const;
   double ComputeProbabilityOfIdentification(double aObjectSize) const;

   void EvaluateObjectState(double aSimTime, const WsfImage& aImage, const WsfImage::Object& aObject);

   //! The optional prototype filter used for video streams.
   WsfFilter* mFilterPtr{nullptr};

   //! The maximum amount of time a track can coast before it is dropped.
   double mCoastTime{0.0};

   //! The logical length (in bits) of the track messages that originate from this processor.
   int mMessageLength{0};

   //! The message priority for messages that originate from this processor.
   int mMessagePriority{0};

   bool mReportsVelocity{true}; // TODO for now
   bool mReportsSide{false};
   bool mReportsType{false};
   bool mReportsBearingElevation{false};
   bool mTargetRecognitionEnabled{false};

   //! @name Unstable covariance reporting.
   //! The following control if unstable covariance matrices will be propagated from the filter to the track.
   //! The covariance is generally unstable for the first couple of updates.
   //@{
   bool mIncludeUnstableCovariance{false};
   bool mIncludeUnstableResidualCovariance{false};
   //@}

   //! The state data for active tracks.
   //! The key value is the platform index of the target.
   using StateList = std::map<size_t, State*>;
   StateList mStateList;

   //! @name minimum pixel count required to be considered for detection/classification/identification.
   //@{
   WsfVariable<double> mMinDetectPixelCount{0.0};
   WsfVariable<double> mMinClassPixelCount{0.0};
   WsfVariable<double> mMinIdentPixelCount{0.0};
   //@}

   //! The average aspect ratio of objects.
   //! Used to convert total number of pixels to number of line bars.
   WsfVariable<double> mAverageAspectRatio{4.0};

   //! @name Scene analysis factors for converting line bars/pixel counts to Pd.
   //@{
   WsfVariable<double> mDetectionSAF{1.0};
   WsfVariable<double> mClassificationSAF{4.0};
   WsfVariable<double> mIdentificationSAF{6.4};
   //@}

   //! @name Delay times before declaring detection/classification/identification
   //@{
   WsfVariable<double> mDetectionDelayTime{0.0};
   WsfVariable<double> mClassificationDelayTime{0.0};
   WsfVariable<double> mIdentificationDelayTime{0.0};
   //@}

   //! @name Coast times before reverting to lower state.
   //@{
   WsfVariable<double> mTransitionCoastTime{0.0};
   WsfVariable<double> mDetectionCoastTime{0.0};
   WsfVariable<double> mClassificationCoastTime{0.0};
   WsfVariable<double> mIdentificationCoastTime{0.0};
   //@}

   bool mShowStateTransitions{false};

   using StatusList = std::map<size_t, Status>;
   StatusList mStatusList;

   // The 'ObjectTest' instance must be a member variable because it must persist between the
   // calls to IsDetectable() and UpdateTrack() from the base class.
   ObjectTest mObjectTest;
};

#endif
