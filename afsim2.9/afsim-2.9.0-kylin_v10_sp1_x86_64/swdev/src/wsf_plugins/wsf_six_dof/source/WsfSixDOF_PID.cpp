// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_PID.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "WsfSixDOF_Utils.hpp"

wsf::six_dof::PID* wsf::six_dof::PID::Clone() const
{
   return new PID(*this);
}

void wsf::six_dof::PID::ProcessInput(UtInputBlock& aInputBlock)
{
   bool usingScalarGainData  = false;
   bool usingTabularGainData = false;
   bool validKpGain          = false;
   bool validKiGain          = false;
   bool validKdGain          = false;
   bool validMaxAccum        = false;
   bool validMaxErrorZero    = false;
   bool validMinErrorZero    = false;
   bool validLowpassAlpha    = false;
   bool validKtAntiWindup    = false;

   std::vector<PidGainData> gainTable;

   PidGainData singleGainTableElement;
   singleGainTableElement.ControllingValue = 0;

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();
      if (command == "update_interval")
      {
         double value;
         controllerInput.ReadValueOfType(value, UtInput::ValueType::cTIME);
         try
         {
            controllerInput.ValueGreater(value, 0.0);
            mUpdateInterval_sec = value;
         }
         catch (const UtInput::BadValue&)
         {
            auto log = ut::log::info() << "PID update_interval of " << value << " was ignored.";
            log.AddNote() << "Value must be greater than 0.0 to be used.";
         }
      }
      else if (command == "kp")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.KpGain = TruncatePidGain(value);
         usingScalarGainData           = true;
      }
      else if (command == "ki")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.KiGain = TruncatePidGain(value);
         usingScalarGainData           = true;
      }
      else if (command == "kd")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.KdGain = TruncatePidGain(value);
         usingScalarGainData           = true;
      }
      else if (command == "max_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.MaxAccum = value;
         usingScalarGainData             = true;
      }
      else if (command == "low_pass_alpha")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.LowpassAlpha = value;
         usingScalarGainData                 = true;
      }
      else if (command == "ignore_large_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.MaxErrorZero = value;
         usingScalarGainData                 = true;
      }
      else if (command == "ignore_small_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.MinErrorZero = value;
         usingScalarGainData                 = true;
      }
      else if (command == "kt_anti_windup_gain")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         singleGainTableElement.KtAntiWindup = TruncatePidGain(value);
         usingScalarGainData                 = true;
      }
      else if (command == "gain_table")
      {
         PidGainData gainTableElement;

         UtInputBlock subBlock(controllerInput, "end_gain_table");

         bool tempValidControllingValue = false;
         bool tempValidKpGain           = false;
         bool tempValidKiGain           = false;
         bool tempValidKdGain           = false;
         bool tempValidMaxAccum         = false;
         bool tempValidMaxErrorZero     = false;
         bool tempValidMinErrorZero     = false;
         bool tempValidLowpassAlpha     = false;
         bool tempValidKtAntiWindup     = false;

         ProcessPidGainTableBlock(subBlock,
                                  gainTableElement,
                                  tempValidControllingValue,
                                  tempValidKpGain,
                                  tempValidKiGain,
                                  tempValidKdGain,
                                  tempValidMaxAccum,
                                  tempValidMaxErrorZero,
                                  tempValidMinErrorZero,
                                  tempValidLowpassAlpha,
                                  tempValidKtAntiWindup);

         bool errorPresent = false;

         if (!tempValidControllingValue)
         {
            ut::log::error() << "Missing control_value in gain_table in WsfSixDOF_Pid::ProcessInput().";
         }

         if (validKpGain)
         {
            if (!tempValidKpGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing kp value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validKiGain)
         {
            if (!tempValidKiGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing ki value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validKdGain)
         {
            if (!tempValidKdGain)
            {
               errorPresent = true;
               ut::log::error() << "Missing kd value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validMaxAccum)
         {
            if (!tempValidMaxAccum)
            {
               errorPresent = true;
               ut::log::error() << "Missing max_error_accum value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validMaxErrorZero)
         {
            if (!tempValidMaxErrorZero)
            {
               errorPresent = true;
               ut::log::error()
                  << "Missing ignore_large_error_accum value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validMinErrorZero)
         {
            if (!tempValidMinErrorZero)
            {
               errorPresent = true;
               ut::log::error()
                  << "Missing ignore_small_error_accum value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validLowpassAlpha)
         {
            if (!tempValidLowpassAlpha)
            {
               errorPresent = true;
               ut::log::error() << "Missing low_pass_alpha value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         if (validKtAntiWindup)
         {
            if (!tempValidKtAntiWindup)
            {
               errorPresent = true;
               ut::log::error() << "Missing kt_anti_windup_gain value in gain_table in WsfSixDOF_Pid::ProcessInput().";
            }
         }

         // Set flags
         if (tempValidKpGain)
         {
            validKpGain = true;
         }
         if (tempValidKiGain)
         {
            validKiGain = true;
         }
         if (tempValidKdGain)
         {
            validKdGain = true;
         }
         if (tempValidMaxAccum)
         {
            validMaxAccum = true;
         }
         if (tempValidMaxErrorZero)
         {
            validMaxErrorZero = true;
         }
         if (tempValidMinErrorZero)
         {
            validMinErrorZero = true;
         }
         if (tempValidLowpassAlpha)
         {
            validLowpassAlpha = true;
         }
         if (tempValidKtAntiWindup)
         {
            validKtAntiWindup = true;
         }

         if (tempValidControllingValue && !errorPresent)
         {
            gainTable.push_back(gainTableElement);
         }
         usingTabularGainData = true;
      }
      else
      {
         auto out = ut::log::warning() << "Unrecognized data command within WsfSixDOF_Pid::ProcessInput().";
         out.AddNote() << "Command: " << command;
         out.AddNote() << "Location: " << controllerInput.GetLocation();
      }
   }

   if (usingTabularGainData && usingScalarGainData)
   {
      ut::log::error() << "Both tabular and scalar data are used in WsfSixDOF_Pid::ProcessInput().";
   }

   if (usingScalarGainData)
   {
      gainTable.push_back(singleGainTableElement);
   }

   SetPidGainTableData(gainTable);
}

void wsf::six_dof::PID::ProcessPidGainTableBlock(UtInputBlock& aInputBlock,
                                                 PidGainData&  aTableElement,
                                                 bool&         aValidControllingValue,
                                                 bool&         aValidKpGain,
                                                 bool&         aValidKiGain,
                                                 bool&         aValidKdGain,
                                                 bool&         aValidMaxAccum,
                                                 bool&         aValidMaxErrorZero,
                                                 bool&         aValidMinErrorZero,
                                                 bool&         aValidLowpassAlpha,
                                                 bool&         aValidKtAntiWindup)
{
   // Clear data
   aTableElement.clearData();

   // Clear flags
   aValidControllingValue = false;
   aValidKpGain           = false;
   aValidKiGain           = false;
   aValidKdGain           = false;
   aValidMaxAccum         = false;
   aValidMaxErrorZero     = false;
   aValidMinErrorZero     = false;
   aValidLowpassAlpha     = false;
   aValidKtAntiWindup     = false;

   std::string command;
   UtInput&    controllerInput = aInputBlock.GetInput();
   while (aInputBlock.ReadCommand())
   {
      command = controllerInput.GetCommand();

      if (command == "control_value")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.ControllingValue = value;
         aValidControllingValue         = true;
      }
      else if (command == "kp")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.KpGain = TruncatePidGain(value);
         aValidKpGain         = true;
      }
      else if (command == "ki")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.KiGain = TruncatePidGain(value);
         aValidKiGain         = true;
      }
      else if (command == "kd")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.KdGain = TruncatePidGain(value);
         aValidKdGain         = true;
      }
      else if (command == "max_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.MaxAccum = value;
         aValidMaxAccum         = true;
      }
      else if (command == "low_pass_alpha")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.LowpassAlpha = value;
         aValidLowpassAlpha         = true;
      }
      else if (command == "ignore_large_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.MaxErrorZero = value;
         aValidMaxErrorZero         = true;
      }
      else if (command == "ignore_small_error_accum")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.MinErrorZero = value;
         aValidMinErrorZero         = true;
      }
      else if (command == "kt_anti_windup_gain")
      {
         float value = 0.0f;
         controllerInput.ReadValue(value);
         aTableElement.KtAntiWindup = TruncatePidGain(value);
         aValidKtAntiWindup         = true;
      }
      else
      {
         auto out = ut::log::warning() << "Unrecognized data command within WsfSixDOF_Pid::ProcessPidGainTableBlock().";
         out.AddNote() << "Command: " << command;
      }
   }
}

double wsf::six_dof::PID::CalcOutputFromTargetWithLimits(double aSetPoint,
                                                         double aCurValue,
                                                         double aSimTime_sec,
                                                         double aMinOutput,
                                                         double aMaxOutput)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aSetPoint;
   mCurrentError = mSetPoint - mCurrentValue;

   return GetOutputWithLimits(aSimTime_sec, aMinOutput, aMaxOutput, true);
}

double wsf::six_dof::PID::CalcOutputFromErrorWithLimits(double aError,
                                                        double aCurValue,
                                                        double aSimTime_sec,
                                                        double aMinOutput,
                                                        double aMaxOutput)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aError;
   mCurrentError = aError;

   return GetOutputWithLimits(aSimTime_sec, aMinOutput, aMaxOutput, true);
}

double wsf::six_dof::PID::CalcOutputFromTarget(double aSetPoint, double aCurValue, double aSimTime_sec)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aSetPoint;
   mCurrentError = mSetPoint - mCurrentValue;

   return GetOutputWithLimits(aSimTime_sec, 0.0, 0.0, false);
}

double wsf::six_dof::PID::CalcOutputFromError(double aError, double aCurValue, double aSimTime_sec)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aError;
   mCurrentError = aError;

   return GetOutputWithLimits(aSimTime_sec, 0.0, 0.0, false);
}

double wsf::six_dof::PID::GetOutputWithLimits(double aSimTime_sec, double aMinOutput, double aMaxOutput, bool aUseLimits)
{
   // First, we calculate the derivative
   double dt_sec = aSimTime_sec - mLastSimTime_sec;

   if (dt_sec < GetUpdateInterval_sec())
   {
      return mOutput;
   }

   // Set gain values
   double tempKpGain       = 0;
   double tempKiGain       = 0;
   double tempKdGain       = 0;
   double tempLowpassAlpha = 0;
   double tempMaxAccum     = 0;
   double tempMaxErrorZero = 0;
   double tempMinErrorZero = 0;
   double tempKtAntiWindup = 0;

   CalcPidGainsData(mGainTables,
                    mControllingValue,
                    tempKpGain,
                    tempKiGain,
                    tempKdGain,
                    tempLowpassAlpha,
                    tempMaxAccum,
                    tempMaxErrorZero,
                    tempMinErrorZero,
                    tempKtAntiWindup);

   // Apply low-pass filter
   // Apply to process variable rather than the derivative

   // Derivative based on the process variable by itself
   if (mLastSimTime_sec > 0.0)
   {
      double sampledDerivative = -(mCurrentValue - mLastValue) / dt_sec;

      // We may not hit the update interval exactly, so modify alpha to help consistency
      double lowpassAlpha = 0.0;
      if (!UtMath::NearlyZero(tempLowpassAlpha))
      {
         double intendedTau =
            mUpdateInterval_sec.value_or(utils::cDT_RIGID_BODY_SEC) * ((1.0 - tempLowpassAlpha) / tempLowpassAlpha);
         lowpassAlpha = dt_sec / (intendedTau + dt_sec);
      }

      mCurrentDerivative = lowpassAlpha * sampledDerivative + (1.0 - lowpassAlpha) * mLastDerivative;
   }

   // Next, we calculate the accumulation. We begin by testing the error
   // for various anti-windup limits

   bool allowAccumulation = true;

   // We will not accumulate if the error is greater than the maximum value
   if (fabs(mCurrentError) > tempMaxErrorZero)
   {
      // Do not accumulate
      allowAccumulation = false;
   }

   // We will not accumulate if the error is less than the minimum value
   if (fabs(mCurrentError) < tempMinErrorZero)
   {
      // Do not accumulate
      allowAccumulation = false;
   }

   // Determine the "effective" Ki, which can be modified by the
   // Kt anti-windup technique

   // In this case, we are using the Kt technique that involves feeding back
   // the error between the unlimited pid output and a limited pid output
   // with a gain of Kt into the integral.

   // Calc the error
   double errorLimitedOutput = mOutput - mPrelimitedOutput;

   double ktE = tempKtAntiWindup * errorLimitedOutput;

   // Calc effective Ki
   double effectiveKi = tempKiGain + ktE;

   // Accumulate the error, if allowed
   if (allowAccumulation && mLastSimTime_sec > 0.0)
   {
      // Calculate the error accumulation
      mErrorAccum += (mCurrentError * dt_sec);
   }

   // Limit the error accumulation
   if (mErrorAccum < -tempMaxAccum)
   {
      mErrorAccum = -tempMaxAccum;
   }
   else if (mErrorAccum > tempMaxAccum)
   {
      mErrorAccum = tempMaxAccum;
   }

   // At this point, we have the error, the derivative, and the accumulation
   // and can perform pid output calculations

   // First, calculate the P contribution
   mKpOutputContrib = tempKpGain * mCurrentError;

   // Next, calculate the I contribution
   mKiOutputContrib = effectiveKi * mErrorAccum;

   // Finally, calculate the D contribution
   mKdOutputContrib = tempKdGain * mCurrentDerivative;

   // Calc the output without limiting
   mPrelimitedOutput = mKpOutputContrib + mKiOutputContrib + mKdOutputContrib;

   // Add the bias
   mPrelimitedOutput += mProportionalBiasValue;

   // Set the output (we may limit it below)
   mOutput = mPrelimitedOutput;

   // Now limit the output, if enabled
   if (aUseLimits)
   {
      if (mOutput < aMinOutput)
      {
         mOutput = aMinOutput;
      }
      if (mOutput > aMaxOutput)
      {
         mOutput = aMaxOutput;
      }
   }

   // Save error and derivative for next time
   mLastValue       = mCurrentValue;
   mLastError       = mCurrentError;
   mLastDerivative  = mCurrentDerivative;
   mLastSimTime_sec = aSimTime_sec;

   return mOutput;
}

void wsf::six_dof::PID::SetCurrentValue(double aCurrentValue)
{
   mCurrentValue = aCurrentValue;
}

void wsf::six_dof::PID::SetTargetValue(double aTargetValue)
{
   mSetPoint = aTargetValue;
}

double wsf::six_dof::PID::GetUpdateInterval_sec()
{
   return mUpdateInterval_sec.value_or(0.0);
}

bool wsf::six_dof::PID::TrySetUpdateInterval_sec(double aInterval_sec)
{
   if (mUpdateInterval_sec.has_value())
   {
      return false;
   }

   mUpdateInterval_sec = aInterval_sec;
   return true;
}

void wsf::six_dof::PID::AddElementsToGainTable(size_t aTotalElementsInTable)
{
   size_t currentNumElementsInTable = mGainTables.size();

   if (currentNumElementsInTable >= aTotalElementsInTable)
   {
      return;
   }

   size_t numToAdd = aTotalElementsInTable - currentNumElementsInTable;
   for (size_t i = 0; i < numToAdd; ++i)
   {
      PidGainData data;
      mGainTables.push_back(data);
   }
}

std::vector<wsf::six_dof::PidGainData>* wsf::six_dof::PID::GetPidGainTableData()
{
   return &mGainTables;
}

void wsf::six_dof::PID::SetPidGainTableData(const std::vector<PidGainData>& aDataTables)
{
   // Clear the tabular data
   mGainTables.clear();

   // Copy the tabular data
   mGainTables = aDataTables;
}

void wsf::six_dof::PID::GetPidValueData(SinglePidValueData& aData) const
{
   aData.SetPoint      = (float)mSetPoint;
   aData.CurrentValue  = (float)mCurrentValue;
   aData.KpValue       = (float)mKpOutputContrib;
   aData.KiValue       = (float)mKiOutputContrib;
   aData.KdValue       = (float)mKdOutputContrib;
   aData.FFValue       = (float)mProportionalBiasValue;
   aData.OutputBase    = (float)mPrelimitedOutput;
   aData.OutputLimited = (float)mOutput;
   aData.AccumError    = (float)mErrorAccum;
   aData.FFValueValid  = mProportionalBiasActive;
}

void wsf::six_dof::PID::SetBias(double aBias)
{
   mProportionalBiasActive = true;
   mProportionalBiasValue  = aBias;
}

void wsf::six_dof::PID::SetFeedForward(double aFeedForward)
{
   mProportionalBiasActive = true;
   mProportionalBiasValue  = aFeedForward;
}

bool wsf::six_dof::PID::GetFeedForward(double& aFeedForward)
{
   if (mProportionalBiasActive)
   {
      aFeedForward = mProportionalBiasValue;
      return true;
   }

   return false;
}

void wsf::six_dof::PID::SetControllingValue(double aControllingValue)
{
   mControllingValue = aControllingValue;
}

void wsf::six_dof::PID::ResetPidState()
{
   mSetPoint          = 0.0;
   mCurrentValue      = 0.0;
   mCurrentError      = 0.0;
   mCurrentDerivative = 0.0;

   mLastValue      = 0.0;
   mLastError      = 0.0;
   mLastDerivative = 0.0;

   mErrorAccum       = 0.0;
   mPrelimitedOutput = 0.0;
   mOutput           = 0.0;

   mKdOutputContrib = 0.0;
   mKiOutputContrib = 0.0;
   mKpOutputContrib = 0.0;

   ResetPidTiming();
}

void wsf::six_dof::PID::ResetPidTiming()
{
   mLastSimTime_sec = 0.0;
}

void wsf::six_dof::PID::CalcPidGainsData(std::vector<PidGainData>& aTables,
                                         double                    aControllingValue,
                                         double&                   aKpGain,
                                         double&                   aKiGain,
                                         double&                   aKdGain,
                                         double&                   aLowpassAlpha,
                                         double&                   aMaxAccum,
                                         double&                   aMaxErrorZero,
                                         double&                   aMinErrorZero,
                                         double&                   aKtAntiWindup)
{
   size_t tableSize = aTables.size();

   if (tableSize == 0)
   {
      aKpGain       = 0.0;
      aKiGain       = 0.0;
      aKdGain       = 0.0;
      aLowpassAlpha = 0.0;
      aMaxAccum     = 0.0;
      aMaxErrorZero = 0.0;
      aMinErrorZero = 0.0;
      aKtAntiWindup = 0.0;
      return;
   }

   if (tableSize == 1)
   {
      aKpGain       = aTables.at(0).KpGain;
      aKiGain       = aTables.at(0).KiGain;
      aKdGain       = aTables.at(0).KdGain;
      aLowpassAlpha = aTables.at(0).LowpassAlpha;
      aMaxAccum     = aTables.at(0).MaxAccum;
      aMaxErrorZero = aTables.at(0).MaxErrorZero;
      aMinErrorZero = aTables.at(0).MinErrorZero;
      aKtAntiWindup = aTables.at(0).KtAntiWindup;
      return;
   }

   if (aControllingValue <= aTables.at(0).ControllingValue)
   {
      aKpGain       = aTables.at(0).KpGain;
      aKiGain       = aTables.at(0).KiGain;
      aKdGain       = aTables.at(0).KdGain;
      aLowpassAlpha = aTables.at(0).LowpassAlpha;
      aMaxAccum     = aTables.at(0).MaxAccum;
      aMaxErrorZero = aTables.at(0).MaxErrorZero;
      aMinErrorZero = aTables.at(0).MinErrorZero;
      aKtAntiWindup = aTables.at(0).KtAntiWindup;
      return;
   }

   if (aControllingValue >= aTables.at(tableSize - 1).ControllingValue)
   {
      aKpGain       = aTables.at(tableSize - 1).KpGain;
      aKiGain       = aTables.at(tableSize - 1).KiGain;
      aKdGain       = aTables.at(tableSize - 1).KdGain;
      aLowpassAlpha = aTables.at(tableSize - 1).LowpassAlpha;
      aMaxAccum     = aTables.at(tableSize - 1).MaxAccum;
      aMaxErrorZero = aTables.at(tableSize - 1).MaxErrorZero;
      aMinErrorZero = aTables.at(tableSize - 1).MinErrorZero;
      aKtAntiWindup = aTables.at(tableSize - 1).KtAntiWindup;
      return;
   }

   std::vector<PidGainData>::iterator iter                 = aTables.begin();
   double                             lastControllingValue = (*iter).ControllingValue;
   double                             lastKpGain           = (*iter).KpGain;
   double                             lastKiGain           = (*iter).KiGain;
   double                             lastKdGain           = (*iter).KdGain;
   double                             lastLowpassAlpha     = (*iter).LowpassAlpha;
   double                             lastMaxAccum         = (*iter).MaxAccum;
   double                             lastMaxErrorZero     = (*iter).MaxErrorZero;
   double                             lastMinErrorZero     = (*iter).MinErrorZero;
   double                             lastKtAntiWindup     = (*iter).KtAntiWindup;

   while (iter != aTables.end())
   {
      double currentControllingValue = (*iter).ControllingValue;
      double currentKpGain           = (*iter).KpGain;
      double currentKiGain           = (*iter).KiGain;
      double currentKdGain           = (*iter).KdGain;
      double currentLowpassAlpha     = (*iter).LowpassAlpha;
      double currentMaxAccum         = (*iter).MaxAccum;
      double currentMaxErrorZero     = (*iter).MaxErrorZero;
      double currentMinErrorZero     = (*iter).MinErrorZero;
      double currentKtAntiWindup     = (*iter).KtAntiWindup;

      if (aControllingValue < currentControllingValue)
      {
         // We are bounded

         double deltaControllingValue = currentControllingValue - lastControllingValue;
         double deltaKpGain           = currentKpGain - lastKpGain;
         double deltaKiGain           = currentKiGain - lastKiGain;
         double deltaKdGain           = currentKdGain - lastKdGain;
         double deltaLowpassAlpha     = currentLowpassAlpha - lastLowpassAlpha;
         double deltaMaxAccum         = currentMaxAccum - lastMaxAccum;
         double deltaMaxErrorZero     = currentMaxErrorZero - lastMaxErrorZero;
         double deltaMinErrorZero     = currentMinErrorZero - lastMinErrorZero;
         double deltaKtAntiWindup     = currentKtAntiWindup - lastKtAntiWindup;

         double fraction = (aControllingValue - lastControllingValue) / deltaControllingValue;

         aKpGain       = fraction * deltaKpGain + lastKpGain;
         aKiGain       = fraction * deltaKiGain + lastKiGain;
         aKdGain       = fraction * deltaKdGain + lastKdGain;
         aLowpassAlpha = fraction * deltaLowpassAlpha + lastLowpassAlpha;
         aMaxAccum     = fraction * deltaMaxAccum + lastMaxAccum;
         aMaxErrorZero = fraction * deltaMaxErrorZero + lastMaxErrorZero;
         aMinErrorZero = fraction * deltaMinErrorZero + lastMinErrorZero;
         aKtAntiWindup = fraction * deltaKtAntiWindup + lastKtAntiWindup;
         return;
      }

      lastControllingValue = currentControllingValue;
      lastKpGain           = currentKpGain;
      lastKiGain           = currentKiGain;
      lastKdGain           = currentKdGain;
      lastLowpassAlpha     = currentLowpassAlpha;
      lastMaxAccum         = currentMaxAccum;
      lastMaxErrorZero     = currentMaxErrorZero;
      lastMinErrorZero     = currentMinErrorZero;
      lastKtAntiWindup     = currentKtAntiWindup;

      ++iter;
   }

   // If we are here, return the last point
   aKpGain       = aTables.at(tableSize - 1).KpGain;
   aKiGain       = aTables.at(tableSize - 1).KiGain;
   aKdGain       = aTables.at(tableSize - 1).KdGain;
   aLowpassAlpha = aTables.at(tableSize - 1).LowpassAlpha;
   aMaxAccum     = aTables.at(tableSize - 1).MaxAccum;
   aMaxErrorZero = aTables.at(tableSize - 1).MaxErrorZero;
   aMinErrorZero = aTables.at(tableSize - 1).MinErrorZero;
   aKtAntiWindup = aTables.at(tableSize - 1).KtAntiWindup;
}

float wsf::six_dof::PID::TruncatePidGain(float aGain)
{
   // Extremely small values can contribute to disparities between Linux and Windows.
   // This function truncates PID gains to 100x float epsilon to try to avoid some of this.
   float epsilon = std::numeric_limits<float>::epsilon() * 100.0f;

   if (UtMath::NearlyZero(aGain, epsilon))
   {
      aGain = 0.0;
   }

   return aGain;
}

void wsf::six_dof::PID::CopyPidGainsData(std::vector<PidGainData>&       aTargetTables,
                                         const std::vector<PidGainData>& aSourceTables)
{
   aTargetTables.clear();

   for (auto tempData : aSourceTables)
   {
      aTargetTables.push_back(tempData);
   }
}
