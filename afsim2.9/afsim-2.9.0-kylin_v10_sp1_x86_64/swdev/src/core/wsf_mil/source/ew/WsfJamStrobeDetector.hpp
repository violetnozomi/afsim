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

#ifndef WSFJAMSTROBEDETECTOR_HPP
#define WSFJAMSTROBEDETECTOR_HPP

#include <map>

class UtInput;
class WsfEM_Interaction;
#include "wsf_mil_export.h"

#include "WsfSensor.hpp"
class WsfSensorMode;

class WSF_MIL_EXPORT WsfJamStrobeDetector
{
public:
   WsfJamStrobeDetector();
   WsfJamStrobeDetector(const WsfJamStrobeDetector& aSrc);
   WsfJamStrobeDetector& operator=(const WsfJamStrobeDetector&) = delete;
   virtual ~WsfJamStrobeDetector()                              = default;
   virtual bool Initialize(double aSimTime, WsfSensorMode* aSensorModePtr);
   virtual bool ProcessInput(UtInput& aInput);

   virtual void ApplyMeasurementErrors(WsfSensorResult& aResult);

   bool PerceiveJammingFirst() const { return mPerceiveJammingFirst; }

   double GetContJNR_PerceptionThreshold() const { return mContJNR_PerceptionThreshold; }
   double GetPulsedJNR_PerceptionThreshold() const { return mPulsedJNR_PerceptionThreshold; }
   double GetCoherentJNR_PerceptionThreshold() const { return mCoherentJNR_PerceptionThreshold; }

   double GetTrackQuality() const { return mTrackQuality; }

   void InitializeTrack(double aSimTime, WsfTrack*& aTrackPtr);

   void UpdateTrack(double aSimTime, WsfTrack* aTrackPtr, WsfPlatform* aTargetPtr, WsfSensorResult& aResult);

   WsfSimulation* GetSimulation() const;

protected:
   // virtual void ComputeMeasurementErrors(WsfSensorResult& aResult,
   //                                       double&          aAzError,
   //                                       double&          aElError,
   //                                       double&          aRangeError);

   // virtual void UpdateSensorCueingLimits();

   virtual void UpdateTrackReportingFlags(double aSimTime, WsfTrack*& aTrackPtr);

private:
   double ReadMeasurementError(UtInput& aInput);

   bool   mPerceiveJammingFirst;
   double mContJNR_PerceptionThreshold;     //! The required JNR for continuous type jamming perception by the operator.
   double mPulsedJNR_PerceptionThreshold;   //! The required JNR for pulse type jamming perception by the operator.
   double mCoherentJNR_PerceptionThreshold; //! The required JNR for coherent (FT) jamming perception by the operator.

   bool   mReportsBearing;
   bool   mReportsElevation;
   double mTrackQuality;

   //! The standard deviation of the error in azimuth and elevation measurements.
   //! If positive then it is in radians. If negative then it is the negative 'fraction_of_beamwidth' specification.
   double mAzErrorStdDev;
   double mElErrorStdDev;

   //! True if Gaussian error, false if uniform error
   bool mGaussianAzError;
   bool mGaussianElError;

   static const double cSDURN; // Standard deviation of Uniform distribution from -1 to +1

   WsfSensorMode* mSensorModePtr;
};

#endif
