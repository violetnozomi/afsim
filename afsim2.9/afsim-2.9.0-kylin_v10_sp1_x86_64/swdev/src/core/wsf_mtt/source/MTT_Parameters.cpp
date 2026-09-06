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

#include "MTT_Parameters.hpp"

#include <iostream>
#include <string>

#include "SupBlock.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"

MTT_Parameters::MTT_Parameters()
   : // Track drop times
   mEmbryonicTrackDropTime(30.0)
   , // 11
   mCandidateTrackDropTime(30.0)
   , // 19
   mActiveTrackDropTime(60.0)
   , // 00
   mVerticalChannelDropTime(60.0)
   , // 32
   mActiveTrackReportDelay(0.0)
   , // 01

   // Candidate Track Process Model 1-sigma errors
   mCandidateTrackX_Acceleration(9.0)
   , // 14
   mCandidateTrackY_Acceleration(9.0)
   , // 15

   // Process Model 1-sigma errors and parameters (active track)
   mStraightFlightX_Acceleration(0.09)
   , // 03
   mStraightFlightY_Acceleration(0.09)
   , // 04
   mTurningFlightX_Acceleration(8.0)
   , // 05
   mTurningFlightY_Acceleration(8.0)
   , // 06
   mVerticalVelocity(6.25)
   , // 12
   mVerticalVelocityDecorrelationTime(20.0)
   , // 13

   // 1-sigma state error thresholds
   mVelocityLimitPromoteEmbryonicTrack(700.0)
   , // 02
   mVelocityVariancePromoteEmbryonicTrack(22500.0)
   , // 17
   mPositionVariancePromoteCandidateTrack(250000.0)
   , // 21
   mVelocityVariancePromoteCandidateTrack(1600.0)
   , // 22
   mPositionVariancePromoteVerticalChannel(250000.0)
   , // 23
   mVelocityVariancePromoteVerticalChannel(1600.0)
   , // 20

   // track promotion
   mPromoteSingleSourceTrackHitThreshold(0)
   , // 09
   mPromoteTrackHorizontalThreshold(2.0e4)
   , // 16
   mPromoteTrackVerticalThreshold(1.0e10)
   , // 18

   // Track state variances
   mStraightFlightMaxVariance(64000.0)
   , // 24
   mTurningFlightMaxVariance(64000.0)
   , // 25
   mStraightFlightMinVariance(16000.0)
   , // 26
   mTurningFlightMinVariance(16000.0)
   , // 27

   // Mode Transition Probability Matrix
   mStraightToStraightFlight(0.70)
   , // 28
   mStraightToTurningFlight(0.30)
   , // 29
   mTurningToTurningFlight(0.30)
   , // 30
   mTurningToStraightFlight(0.70)
   , // 31

   // Prob. false corr. rejection
   mM2T_FalseRejectProbability(1.0e-20)
   , // 07
   mT2T_FalseRejectProbability(1.0e-20)
   , // 08

   // Track correlation cylinder parameters
   mTrackCorrelationCylinderHeight(0.0)
   ,                                      // 33
   mTrackCorrelationCylinderDiameter(0.0) // 34

{
}

bool MTT_Parameters::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        processed = false;

   if (command == "track_drop_times")
   {
      processed = ProcessTrackDropTimes(aInput);
   }
   else if (command == "process_model_one_sigma_errors_candidate_track")
   {
      processed = ProcessCandidateTrackErrors(aInput);
   }
   else if (command == "process_model_one_sigma_errors_and_parameters_active_track")
   {
      processed = ProcessActiveTrackErrors(aInput);
   }
   else if (command == "one_sigma_state_error_thresholds")
   {
      processed = ProcessStateErrorThresholds(aInput);
   }
   else if (command == "state_error_covariance_matrix_condition_number_thresholds")
   {
      processed = ProcessStateErrorCovarianceThresholds(aInput);
   }
   else if (command == "state_variance_limit_active_track")
   {
      processed = ProcessStateVarianceLimit(aInput);
   }
   else if (command == "mode_transition_probability_matrix")
   {
      processed = ProcessModeTransitionProbabilityMatrix(aInput);
   }
   else if (command == "probability_of_falsely_rejecting_correlation")
   {
      processed = ProcessProbabilityOfFalselyRejectingCorrelation(aInput);
   }
   else if (command == "track_correlation_cylinder")
   {
      processed = ProcessTrackCorrelationCylinder(aInput);
   }
   else if (command == "consecutive_single_source_hits_to_promote_track")
   {
      int threshold;
      aInput.ReadValue(threshold);
      mPromoteSingleSourceTrackHitThreshold = threshold;
      processed                             = true;
   }

   return processed;
}

bool MTT_Parameters::ProcessTrackDropTimes(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_track_drop_times");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "embryonic_track")
      {
         aInput.ReadValueOfType(mEmbryonicTrackDropTime, UtInput::cTIME);
      }
      else if (command == "candidate_track")
      {
         aInput.ReadValueOfType(mCandidateTrackDropTime, UtInput::cTIME);
      }
      else if (command == "active_track")
      {
         aInput.ReadValueOfType(mActiveTrackDropTime, UtInput::cTIME);
      }
      else if (command == "vertical_channel_active_track")
      {
         aInput.ReadValueOfType(mVerticalChannelDropTime, UtInput::cTIME);
      }
      else if (command == "report_delay_for_active_track")
      {
         aInput.ReadValueOfType(mActiveTrackReportDelay, UtInput::cTIME);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessCandidateTrackErrors(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_process_model_one_sigma_errors_candidate_track");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "x_dir_accel")
      {
         aInput.ReadValueOfType(mCandidateTrackX_Acceleration, UtInput::cACCELERATION);
      }
      else if (command == "y_dir_accel")
      {
         aInput.ReadValueOfType(mCandidateTrackY_Acceleration, UtInput::cACCELERATION);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessActiveTrackErrors(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_process_model_one_sigma_errors_and_parameters_active_track");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "x_dir_accel_straight_flight_model")
      {
         aInput.ReadValueOfType(mStraightFlightX_Acceleration, UtInput::cACCELERATION);
      }
      else if (command == "y_dir_accel_straight_flight_model")
      {
         aInput.ReadValueOfType(mStraightFlightY_Acceleration, UtInput::cACCELERATION);
      }
      else if (command == "x_dir_accel_turning_flight_model")
      {
         aInput.ReadValueOfType(mTurningFlightX_Acceleration, UtInput::cACCELERATION);
      }
      else if (command == "y_dir_accel_turning_flight_model")
      {
         aInput.ReadValueOfType(mTurningFlightY_Acceleration, UtInput::cACCELERATION);
      }
      else if (command == "vertical_velocity")
      {
         aInput.ReadValueOfType(mVerticalVelocity, UtInput::cSPEED);
      }
      else if (command == "decorrelation_time_vert_velocity")
      {
         aInput.ReadValueOfType(mVerticalVelocityDecorrelationTime, UtInput::cTIME);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessStateErrorThresholds(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_one_sigma_state_error_thresholds");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "velocity_limit_to_promote_embryonic_track")
      {
         aInput.ReadValueOfType(mVelocityLimitPromoteEmbryonicTrack, UtInput::cSPEED);
      }
      else if (command == "velocity_error_to_promote_embryonic_track")
      {
         double error;
         aInput.ReadValueOfType(error, aInput.cSPEED);
         mVelocityVariancePromoteEmbryonicTrack = error * error;
      }
      else if (command == "position_error_to_promote_candidate_track")
      {
         double error;
         aInput.ReadValueOfType(error, aInput.cLENGTH);
         mPositionVariancePromoteCandidateTrack = error * error;
      }
      else if (command == "velocity_error_to_promote_candidate_track")
      {
         double error;
         aInput.ReadValueOfType(error, UtInput::cSPEED);
         mVelocityVariancePromoteCandidateTrack = error * error;
      }
      else if (command == "position_error_to_promote_vertical_channel")
      {
         double error;
         aInput.ReadValueOfType(error, UtInput::cLENGTH);
         mPositionVariancePromoteVerticalChannel = error * error;
      }
      else if (command == "velocity_error_to_promote_vertical_channel")
      {
         double error;
         aInput.ReadValueOfType(error, UtInput::cSPEED);
         mVelocityVariancePromoteVerticalChannel = error * error;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessStateErrorCovarianceThresholds(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_state_error_covariance_matrix_condition_number_thresholds");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "promote_track_in_horizontal_channel")
      {
         aInput.ReadValue(mPromoteTrackHorizontalThreshold);
      }
      else if (command == "promote_track_in_vertical_channel")
      {
         aInput.ReadValue(mPromoteTrackVerticalThreshold);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessStateVarianceLimit(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_state_variance_limit_active_track");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "max_std_dev_straight_flight_model")
      {
         double stDev;
         aInput.ReadValueOfType(stDev, UtInput::cLENGTH);
         mStraightFlightMaxVariance = stDev * stDev;
      }
      else if (command == "max_std_dev_turning_flight_model")
      {
         double stDev;
         aInput.ReadValueOfType(stDev, UtInput::cLENGTH);
         mTurningFlightMaxVariance = stDev * stDev;
      }
      else if (command == "min_std_dev_straight_flight_model")
      {
         double stDev;
         aInput.ReadValueOfType(stDev, UtInput::cLENGTH);
         mStraightFlightMinVariance = stDev * stDev;
      }
      else if (command == "min_std_dev_turning_flight_model")
      {
         double stDev;
         aInput.ReadValueOfType(stDev, UtInput::cLENGTH);
         mTurningFlightMinVariance = stDev * stDev;
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessModeTransitionProbabilityMatrix(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_mode_transition_probability_matrix");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "straight_to_straight_flight")
      {
         aInput.ReadValue(mStraightToStraightFlight);
      }
      else if (command == "straight_to_turning_flight")
      {
         aInput.ReadValue(mStraightToTurningFlight);
      }
      else if (command == "turning_to_turning_flight")
      {
         aInput.ReadValue(mTurningToTurningFlight);
      }
      else if (command == "turning_to_straight_flight")
      {
         aInput.ReadValue(mTurningToStraightFlight);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessProbabilityOfFalselyRejectingCorrelation(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_probability_of_falsely_rejecting_correlation");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "measurement_to_track")
      {
         aInput.ReadValue(mM2T_FalseRejectProbability);
      }
      else if (command == "track_to_track")
      {
         aInput.ReadValue(mT2T_FalseRejectProbability);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

bool MTT_Parameters::ProcessTrackCorrelationCylinder(UtInput& aInput)
{
   bool         processed = true;
   UtInputBlock block(aInput, "end_track_correlation_cylinder");
   while (block.ReadCommand())
   {
      std::string command = block.GetCommand();
      if (command == "cylinder_height")
      {
         aInput.ReadValueOfType(mTrackCorrelationCylinderHeight, UtInput::cLENGTH);
      }
      else if (command == "cylinder_diameter")
      {
         aInput.ReadValueOfType(mTrackCorrelationCylinderDiameter, UtInput::cLENGTH);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return processed;
}

void MTT_Parameters::ConvertFrom(const SupBlock& aBlock)
{
   // Track Drop Times                             Suppressor block 388 offset value
   mEmbryonicTrackDropTime  = aBlock.mFloatBlock[11]; // 11
   mCandidateTrackDropTime  = aBlock.mFloatBlock[19]; // 19
   mActiveTrackDropTime     = aBlock.mFloatBlock[0];  // 00
   mVerticalChannelDropTime = aBlock.mFloatBlock[32]; // 32
   mActiveTrackReportDelay  = aBlock.mFloatBlock[01]; // 01

   // Candidate Track Process Model 1-sigma errors
   mCandidateTrackX_Acceleration = aBlock.mFloatBlock[14]; // 14
   mCandidateTrackY_Acceleration = aBlock.mFloatBlock[15]; // 15

   // Process Model 1-sigma errors and parameters (active track)
   mStraightFlightX_Acceleration      = aBlock.mFloatBlock[3];  // 03
   mStraightFlightY_Acceleration      = aBlock.mFloatBlock[4];  // 04
   mTurningFlightX_Acceleration       = aBlock.mFloatBlock[5];  // 05
   mTurningFlightY_Acceleration       = aBlock.mFloatBlock[6];  // 06
   mVerticalVelocity                  = aBlock.mFloatBlock[12]; // 12
   mVerticalVelocityDecorrelationTime = aBlock.mFloatBlock[13]; // 13

   // 1-sigma state error thresholds
   mVelocityLimitPromoteEmbryonicTrack     = aBlock.mFloatBlock[2];  // 02
   mVelocityVariancePromoteEmbryonicTrack  = aBlock.mFloatBlock[17]; // 17
   mPositionVariancePromoteCandidateTrack  = aBlock.mFloatBlock[21]; // 21
   mVelocityVariancePromoteCandidateTrack  = aBlock.mFloatBlock[22]; // 22
   mPositionVariancePromoteVerticalChannel = aBlock.mFloatBlock[23]; // 23
   mVelocityVariancePromoteVerticalChannel = aBlock.mFloatBlock[20]; // 20

   // track promotion
   mPromoteSingleSourceTrackHitThreshold = aBlock.mIntBlock[9];    // 09
   mPromoteTrackHorizontalThreshold      = aBlock.mFloatBlock[16]; // 16
   mPromoteTrackVerticalThreshold        = aBlock.mFloatBlock[18]; // 18

   // Track state variances
   mStraightFlightMaxVariance = aBlock.mFloatBlock[24]; // 24
   mTurningFlightMaxVariance  = aBlock.mFloatBlock[25]; // 25
   mStraightFlightMinVariance = aBlock.mFloatBlock[26]; // 26
   mTurningFlightMinVariance  = aBlock.mFloatBlock[27]; // 27

   // Mode Transition Probability Matrix
   mStraightToStraightFlight = aBlock.mFloatBlock[28]; // 28
   mStraightToTurningFlight  = aBlock.mFloatBlock[29]; // 29
   mTurningToTurningFlight   = aBlock.mFloatBlock[30]; // 30
   mTurningToStraightFlight  = aBlock.mFloatBlock[31]; // 31

   // Prob. false corr. rejection
   mM2T_FalseRejectProbability = aBlock.mFloatBlock[7]; // 07
   mT2T_FalseRejectProbability = aBlock.mFloatBlock[8]; // 08

   // Track correlation cylinder parameters
   mTrackCorrelationCylinderHeight   = aBlock.mFloatBlock[33]; // 33
   mTrackCorrelationCylinderDiameter = aBlock.mFloatBlock[34]; // 34
}

// Unit test... Attempt to read the suppressot fllter params block
#ifdef UNIT_TEST_PARAMETERS
#include <iostream>

#include "UtInputFile.hpp"
#include "cmtt.hpp"

void main()
{
   UtInputFile file("filter_params.dat");
   try
   {
      while (true)
      {
         try
         {
            bool processed = WsfMTT_Correlation::ProcessInput(file);
            if (processed)
            {
               ut::log::info() << "Passed.";
               exit(1);
            }
            else
            {
               UtInput::UnknownCommand unk(file);
               auto                    logger = ut::log::error() << "Unknown Command";
               logger.AddNote() << unk.GetMessage();
               throw unk;
            }
         }
         catch (UtInput::UnknownCommand)
         {
            exit(0);
         }
      }
   }
   catch (UtInput::EndOfData)
   {
      ut::log::info() << "Done...";
   }
}

#endif
