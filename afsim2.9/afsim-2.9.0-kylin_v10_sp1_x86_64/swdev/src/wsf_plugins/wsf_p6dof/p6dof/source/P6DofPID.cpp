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

#include "P6DofPID.hpp"

#include <math.h>

P6DofPID::P6DofPID()
   : mSetPoint(0.0)
   , mCurrentValue(0.0)
   , mCurrentError(0.0)
   , mCurrentDeriv(0.0)
   , mLastError(0.0)
   , mLastDerivative(0.0)
   , mErrorAccum(0.0)
   , mPrelimitedOutput(0.0)
   , mOutput(0.0)
   , mKpOutputContrib(0.0)
   , mKiOutputContrib(0.0)
   , mKdOutputContrib(0.0)
   , mUseAlpha(false)
   , mLimitMax(false)
   , mZeroGtMax(false)
   , mZeroLtMin(false)
   , mUseKt(false)
   , mProportionalBiasActive(false)
   , mProportionalBiasValue(0.0)
   , mControllingValue(0)
{
}

P6DofPID::~P6DofPID()
{
   // Delete the table data
   mGainTables.clear();
}

P6DofPID* P6DofPID::Clone() const
{
   return new P6DofPID(*this);
}

P6DofPID::P6DofPID(const P6DofPID& aSrc)
   : mSetPoint(aSrc.mSetPoint)
   , mCurrentValue(aSrc.mCurrentValue)
   , mCurrentError(aSrc.mCurrentError)
   , mCurrentDeriv(aSrc.mCurrentDeriv)
   , mLastError(aSrc.mLastError)
   , mLastDerivative(aSrc.mLastDerivative)
   , mErrorAccum(aSrc.mErrorAccum)
   , mPrelimitedOutput(aSrc.mPrelimitedOutput)
   , mOutput(aSrc.mOutput)
   , mKpOutputContrib(aSrc.mKpOutputContrib)
   , mKiOutputContrib(aSrc.mKiOutputContrib)
   , mKdOutputContrib(aSrc.mKdOutputContrib)
   , mUseAlpha(aSrc.mUseAlpha)
   , mLimitMax(aSrc.mLimitMax)
   , mZeroGtMax(aSrc.mZeroGtMax)
   , mZeroLtMin(aSrc.mZeroLtMin)
   , mUseKt(aSrc.mUseKt)
   , mProportionalBiasActive(aSrc.mProportionalBiasActive)
   , mProportionalBiasValue(aSrc.mProportionalBiasValue)
   , mControllingValue(aSrc.mControllingValue)
{
   // Copy the table data
   CopyPidGainsData(mGainTables, aSrc.mGainTables);
}

double P6DofPID::CalcOutputFromTargetAndCurrentWithLimits(double aSetPoint,
                                                          double aCurValue,
                                                          double aDt_sec,
                                                          double aMinOutput,
                                                          double aMaxOutput)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aSetPoint;
   mCurrentError = mSetPoint - mCurrentValue;

   return GetOutputWithLimits(aDt_sec, aMinOutput, aMaxOutput, true);
}

double P6DofPID::CalcOutputFromErrorWithLimits(double aError, double aDt_sec, double aMinOutput, double aMaxOutput)
{
   mSetPoint     = aError;
   mCurrentError = aError;

   return GetOutputWithLimits(aDt_sec, aMinOutput, aMaxOutput, true);
}

double P6DofPID::CalcOutputFromTargetAndCurrent(double aSetPoint, double aCurValue, double aDt_sec)
{
   mCurrentValue = aCurValue;
   mSetPoint     = aSetPoint;
   mCurrentError = mSetPoint - mCurrentValue;

   return GetOutputWithLimits(aDt_sec, 0.0, 0.0, false);
}

double P6DofPID::CalcOutputFromError(double aError, double aDt_sec)
{
   mSetPoint     = aError;
   mCurrentError = aError;

   return GetOutputWithLimits(aDt_sec, 0.0, 0.0, false);
}

double P6DofPID::GetOutputWithLimits(double aDt_sec, double aMinOutput, double aMaxOutput, bool aUseLimits)
{
   // First, we calculate the derivative
   double current_derivative = (mCurrentError - mLastError) / aDt_sec;
   double last_derivative    = mLastDerivative;

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

   // Apply low-pass filter if enabled
   if (mUseAlpha)
   {
      // Set current derivative, using the filter
      mCurrentDeriv = tempLowpassAlpha * current_derivative + (1.0 - tempLowpassAlpha) * last_derivative;
   }
   else
   {
      // Set current derivative, without using the filter
      mCurrentDeriv = current_derivative;
   }

   // Next, we calculate the accumulation. We begin by testing the error
   // for various anti-windup limits

   bool allowAccumulation = true;

   if (mZeroGtMax)
   {
      // We will not accumulate if the error is greater than the maximum value
      if (fabs(mCurrentError) > tempMaxErrorZero)
      {
         // Do not accumulate
         allowAccumulation = false;
      }
   }

   if (mZeroLtMin)
   {
      // We will not accumulate if the error is less than the minimum value
      if (fabs(mCurrentError) < tempMinErrorZero)
      {
         // Do not accumulate
         allowAccumulation = false;
      }
   }

   // Determine the "effective" Ki, which can be modified by the
   // Kt anti-windup technique
   double effectiveKi = tempKiGain;
   if (mUseKt)
   {
      // In this case, we are using the Kt technique that involves feeding back
      // the error between the unlimited pid output and a limited pid output
      // with a gain of Kt into the integral.

      // Calc the error
      double errorLimitedOutput = mOutput - mPrelimitedOutput;

      double ktE = tempKtAntiWindup * errorLimitedOutput;

      // Calc effective Ki
      effectiveKi = tempKiGain + ktE;
   }

   // Accumulate the error, if allowed
   if (allowAccumulation)
   {
      // Calculate the error accumulation
      mErrorAccum += (mCurrentError * aDt_sec);
   }

   // Limit the maximum accumulation, if enabled
   if (mLimitMax)
   {
      // Limit the error accumulation
      if (mErrorAccum < -tempMaxAccum)
      {
         mErrorAccum = -tempMaxAccum;
      }
      else if (mErrorAccum > tempMaxAccum)
      {
         mErrorAccum = tempMaxAccum;
      }
   }

   // At this point, we have the error, the derivative, and the accumulation
   // and can perform pid output calculations

   // First, calculate the P contribution
   mKpOutputContrib = tempKpGain * mCurrentError;

   // Next, calculate the I contribution
   mKiOutputContrib = effectiveKi * mErrorAccum;

   // Finally, calculate the D contribution
   mKdOutputContrib = tempKdGain * mCurrentDeriv;

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
   mLastError      = mCurrentError;
   mLastDerivative = current_derivative;

   return mOutput;
}

void P6DofPID::SetCurrentValue(double aCurrentValue)
{
   mCurrentValue = aCurrentValue;
}

void P6DofPID::SetTargetValue(double aTargetValue)
{
   mSetPoint = aTargetValue;
}

void P6DofPID::AddElementsToGainTable(size_t aTotalElementsInTable)
{
   size_t currentNumElementsInTable = mGainTables.size();

   if (currentNumElementsInTable >= aTotalElementsInTable)
   {
      return;
   }

   size_t numToAdd = aTotalElementsInTable - currentNumElementsInTable;
   for (size_t i = 0; i < numToAdd; ++i)
   {
      P6DOF::PidGainData data;
      mGainTables.push_back(data);
   }
}

std::vector<P6DOF::PidGainData>* P6DofPID::GetPidGainTableData(unsigned char& aPackedFlags)
{
   unsigned char flags = 0;
   if (mUseAlpha)
   {
      flags = flags | P6DOF::Pid::UseAlpha;
   }
   if (mLimitMax)
   {
      flags = flags | P6DOF::Pid::LimitMax;
   }
   if (mZeroGtMax)
   {
      flags = flags | P6DOF::Pid::ZeroGtMax;
   }
   if (mZeroLtMin)
   {
      flags = flags | P6DOF::Pid::ZeroLtMin;
   }
   if (mUseKt)
   {
      flags = flags | P6DOF::Pid::UseKt;
   }
   aPackedFlags = flags;

   return &mGainTables;
}

void P6DofPID::SetPidGainTableData(std::vector<P6DOF::PidGainData>& aDataTables, unsigned char& aPackedFlags)
{
   unsigned char flags = aPackedFlags;

   if (flags & P6DOF::Pid::UseAlpha)
   {
      mUseAlpha = true;
   }
   else
   {
      mUseAlpha = false;
   }

   if (flags & P6DOF::Pid::LimitMax)
   {
      mLimitMax = true;
   }
   else
   {
      mLimitMax = false;
   }

   if (flags & P6DOF::Pid::ZeroGtMax)
   {
      mZeroGtMax = true;
   }
   else
   {
      mZeroGtMax = false;
   }

   if (flags & P6DOF::Pid::ZeroLtMin)
   {
      mZeroLtMin = true;
   }
   else
   {
      mZeroLtMin = false;
   }

   if (flags & P6DOF::Pid::UseKt)
   {
      mUseKt = true;
   }
   else
   {
      mUseKt = false;
   }

   if (flags & P6DOF::Pid::ClearAccum)
   {
      // Clear the accumulation
      mErrorAccum = 0.0;
   }

   // Clear the tabular data
   mGainTables.clear();

   // Copy the tabular data
   mGainTables = aDataTables;
}

void P6DofPID::GetPidValueData(P6DOF::SinglePidValueData& aData) const
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

void P6DofPID::SetBias(double aBias)
{
   mProportionalBiasActive = true;
   mProportionalBiasValue  = aBias;
}

void P6DofPID::SetFeedForward(double aFeedForward)
{
   mProportionalBiasActive = true;
   mProportionalBiasValue  = aFeedForward;
}

bool P6DofPID::GetFeedForward(double& aFeedForward)
{
   if (mProportionalBiasActive)
   {
      aFeedForward = mProportionalBiasValue;
      return true;
   }

   return false;
}

void P6DofPID::SetControllingValue(double aControllingValue)
{
   mControllingValue = aControllingValue;
}

void P6DofPID::ResetPidState()
{
   mCurrentValue = 0.0;
   mCurrentError = 0.0;
   mCurrentDeriv = 0.0;

   mOutput           = 0.0;
   mPrelimitedOutput = 0.0;
   mLastError        = 0.0;
   mLastDerivative   = 0.0;
   mErrorAccum       = 0.0;

   mKdOutputContrib = 0.0;
   mKiOutputContrib = 0.0;
   mKpOutputContrib = 0.0;
}

void P6DofPID::CalcPidGainsData(std::vector<P6DOF::PidGainData>& aTables,
                                double                           aControllingValue,
                                double&                          aKpGain,
                                double&                          aKiGain,
                                double&                          aKdGain,
                                double&                          aLowpassAlpha,
                                double&                          aMaxAccum,
                                double&                          aMaxErrorZero,
                                double&                          aMinErrorZero,
                                double&                          aKtAntiWindup)
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

   std::vector<P6DOF::PidGainData>::iterator iter                 = aTables.begin();
   double                                    lastControllingValue = (*iter).ControllingValue;
   double                                    lastKpGain           = (*iter).KpGain;
   double                                    lastKiGain           = (*iter).KiGain;
   double                                    lastKdGain           = (*iter).KdGain;
   double                                    lastLowpassAlpha     = (*iter).LowpassAlpha;
   double                                    lastMaxAccum         = (*iter).MaxAccum;
   double                                    lastMaxErrorZero     = (*iter).MaxErrorZero;
   double                                    lastMinErrorZero     = (*iter).MinErrorZero;
   double                                    lastKtAntiWindup     = (*iter).KtAntiWindup;

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

void P6DofPID::CopyPidGainsData(std::vector<P6DOF::PidGainData>&       aTargetTables,
                                const std::vector<P6DOF::PidGainData>& aSourceTables)
{
   aTargetTables.clear();

   for (auto tempData : aSourceTables)
   {
      aTargetTables.push_back(tempData);
   }
}
