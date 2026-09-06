// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MTT_PARAMETERS_HPP
#define MTT_PARAMETERS_HPP

class UtInput;
class SupBlock;

class MTT_Parameters
{
public:
   MTT_Parameters();
   ~MTT_Parameters() {}

   bool ProcessInput(UtInput& aInput);

   // Track Drop Times                             Suppressor block 388 offset value
   double mEmbryonicTrackDropTime;  // 11
   double mCandidateTrackDropTime;  // 19
   double mActiveTrackDropTime;     // 00
   double mVerticalChannelDropTime; // 32
   double mActiveTrackReportDelay;  // 01

   // Candidate Track Process Model 1-sigma errors
   double mCandidateTrackX_Acceleration; // 14
   double mCandidateTrackY_Acceleration; // 15

   // Process Model 1-sigma errors and parameters (active track)
   double mStraightFlightX_Acceleration;      // 03
   double mStraightFlightY_Acceleration;      // 04
   double mTurningFlightX_Acceleration;       // 05
   double mTurningFlightY_Acceleration;       // 06
   double mVerticalVelocity;                  // 12
   double mVerticalVelocityDecorrelationTime; // 13

   // 1-sigma state error thresholds
   double mVelocityLimitPromoteEmbryonicTrack;     // 02
   double mVelocityVariancePromoteEmbryonicTrack;  // 17
   double mPositionVariancePromoteCandidateTrack;  // 21
   double mVelocityVariancePromoteCandidateTrack;  // 22
   double mPositionVariancePromoteVerticalChannel; // 23
   double mVelocityVariancePromoteVerticalChannel; // 20

   // track promotion
   long   mPromoteSingleSourceTrackHitThreshold; // 09
   double mPromoteTrackHorizontalThreshold;      // 16
   double mPromoteTrackVerticalThreshold;        // 18

   // Track state variances
   double mStraightFlightMaxVariance; // 24
   double mTurningFlightMaxVariance;  // 25
   double mStraightFlightMinVariance; // 26
   double mTurningFlightMinVariance;  // 27

   // Mode Transition Probability Matrix
   double mStraightToStraightFlight; // 28
   double mStraightToTurningFlight;  // 29
   double mTurningToTurningFlight;   // 30
   double mTurningToStraightFlight;  // 31

   // Prob. false corr. rejection
   double mM2T_FalseRejectProbability; // 07
   double mT2T_FalseRejectProbability; // 08

   // Track correlation cylinder parameters
   double mTrackCorrelationCylinderHeight;   // 33
   double mTrackCorrelationCylinderDiameter; // 34

   void ConvertFrom(const SupBlock& aBlock);

private:
   bool ProcessTrackDropTimes(UtInput& aInput);
   bool ProcessCandidateTrackErrors(UtInput& aInput);
   bool ProcessActiveTrackErrors(UtInput& aInput);
   bool ProcessStateErrorThresholds(UtInput& aInput);
   bool ProcessStateErrorCovarianceThresholds(UtInput& aInput);
   bool ProcessStateVarianceLimit(UtInput& aInput);
   bool ProcessModeTransitionProbabilityMatrix(UtInput& aInput);
   bool ProcessProbabilityOfFalselyRejectingCorrelation(UtInput& aInput);
   bool ProcessTrackCorrelationCylinder(UtInput& aInput);
};

#endif
