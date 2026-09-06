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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtPID_Controller.hpp"

#include <cmath>
#include <iostream>

#include "UtInputBlock.hpp"

// Programmer's note:  There is opportunity to improve this algorithm, relating
// to how integral "wind-up" is handled.  The purpose of Integral Gain is to remove
// a steady-state bias or offset.  When the input is within threshold range of
// desired, mIntegratedValue is continuously summed, and the summed value allowed
// to push the output value closer to desired.  Logic may be desirable to "pre-set"
// a desired integral value that reduces the integral lag.time to converge.  Also,
// other logic may be desirable to slowly bleed down the mIntegratedValue toward
// zero when the output is not within mInputThreshold of desired.

// ============================================================================
UtPID_Controller::UtPID_Controller()
   : mCounter(0)
   , mIsStarted(false)
   , mLastUpdate(0.0)
   , mProportionalGain(1.0)
   , mIntegralGain(0.0)
   , mDerivativeGain(0.0)
   , mIntegratedValue(0.0)
   , mPreviousInput(0.0)
   , mInputThreshold(1.0)
   , mLastOutput(0.0)
   , mOutputEnabled(true)
   , mCurrentStreamPtr(nullptr)
   , mStreamName()
   , mOutputModulus(1)
{
}

// ============================================================================
UtPID_Controller::UtPID_Controller(const UtPID_Controller& aSrc)
   : mCounter(aSrc.mCounter)
   , mIsStarted(aSrc.mIsStarted)
   , mLastUpdate(aSrc.mLastUpdate)
   , mProportionalGain(aSrc.mProportionalGain)
   , mIntegralGain(aSrc.mIntegralGain)
   , mDerivativeGain(aSrc.mDerivativeGain)
   , mIntegratedValue(aSrc.mIntegratedValue)
   , mPreviousInput(aSrc.mPreviousInput)
   , mInputThreshold(aSrc.mInputThreshold)
   , mLastOutput(aSrc.mLastOutput)
   , mOutputEnabled(aSrc.mOutputEnabled)
   , mCurrentStreamPtr(nullptr)
   , mStreamName(aSrc.mStreamName)
   , mOutputModulus(aSrc.mOutputModulus)
{
}

// ============================================================================

UtPID_Controller& UtPID_Controller::operator=(const UtPID_Controller& aSrc)
{
   mCounter          = aSrc.mCounter;
   mIsStarted        = aSrc.mIsStarted;
   mLastUpdate       = aSrc.mLastUpdate;
   mProportionalGain = aSrc.mProportionalGain;
   mIntegralGain     = aSrc.mIntegralGain;
   mDerivativeGain   = aSrc.mDerivativeGain;
   mIntegratedValue  = aSrc.mIntegratedValue;
   mPreviousInput    = aSrc.mPreviousInput;
   mInputThreshold   = aSrc.mInputThreshold;
   mLastOutput       = aSrc.mLastOutput;
   mOutputEnabled    = aSrc.mOutputEnabled;
   mCurrentStreamPtr = nullptr;
   mStreamName       = aSrc.mStreamName;
   mOutputModulus    = aSrc.mOutputModulus;
   return *this;
}

// ============================================================================
void UtPID_Controller::SelectZieglerNicholsTuning(ControlType aType, double aUltimateGain, double aUltimatePeriod, bool aNegateP_Gain)
{
   // These gains apply to the standard PID form, as implemented here,
   // not the ideal parallel form of the PID controller.
   if (aType == P)
   {
      mProportionalGain = 0.5 * aUltimateGain;
      mIntegralGain     = 0.0;
      mDerivativeGain   = 0.0;
   }
   else if (aType == PI)
   {
      mProportionalGain = 0.45 * aUltimateGain;
      mIntegralGain     = 1.2 / aUltimatePeriod;
      mDerivativeGain   = 0.0;
   }
   else if (aType == PID)
   {
      mProportionalGain = 0.6 * aUltimateGain;
      mIntegralGain     = 2.0 / aUltimatePeriod;
      mDerivativeGain   = aUltimatePeriod / 8.0;
   }

   if (aNegateP_Gain)
   {
      mProportionalGain *= -1.0;
   }
}

// ============================================================================
// virtual
bool UtPID_Controller::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "output_stream")
   {
      aInput.ReadValue(mStreamName);
   }
   else if (command == "output_enable")
   {
      aInput.ReadValue(mOutputEnabled);
   }
   else if (command == "print_modulus")
   {
      int value;
      aInput.ReadValue(value);
      if (value > 0)
      {
         mOutputModulus = static_cast<unsigned int>(value);
      }
   }
   else if (command == "proportional_gain")
   {
      aInput.ReadValue(mProportionalGain);
   }
   else if (command == "input_threshold")
   {
      aInput.ReadValue(mInputThreshold);
      mInputThreshold = fabs(mInputThreshold);
   }
   else if (command == "integral_gain")
   {
      aInput.ReadValue(mIntegralGain);
   }
   else if (command == "derivative_gain")
   {
      aInput.ReadValue(mDerivativeGain);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// ============================================================================
// virtual
void UtPID_Controller::ProcessInputBlock(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      UtInput& input = inputBlock.GetInput();
      if (!ProcessInput(input))
      {
         throw UtInput::UnknownCommand(input);
      }
   }
}

// ============================================================================
// virtual
bool UtPID_Controller::Initialize(double aSimTime, double /*aSampleInterval*/)
{
   // Note that in this base class, aSampleInterval is not used.
   // Placeholder for derived classes as needed.
   OpenFile();

   if (mCurrentStreamPtr != nullptr)
   {
      // Print header labels for values:
      *mCurrentStreamPtr << " # ( PK = " << mProportionalGain << ", PI = " << mIntegralGain
                         << ", PD = " << mDerivativeGain << " ) " << std::endl;
      *mCurrentStreamPtr << "Counter, SimTime, Input, Started, Q, DeltaT, Deriv, Integral, Output, Contrib (PID)"
                         << std::endl;
   }

   Reset(aSimTime);

   return true;
}

// ============================================================================
// virtual
void UtPID_Controller::Reset(double aSimTime)
{
   mCounter         = 0;
   mLastUpdate      = aSimTime;
   mIntegratedValue = 0.0;
   mLastOutput      = 0.0;
   mIsStarted       = false;
}

// ============================================================================
// virtual
double UtPID_Controller::Update(double aSimTime, double aInputSample)
{
   if (aSimTime <= mLastUpdate)
      return mLastOutput;

   /* Set q to 1 if the input magnitude is below
      the threshold and 0 otherwise */
   double q     = (fabs(aInputSample) < mInputThreshold) ? 1.0 : 0.0;
   double deriv = 0;

   double dT = aSimTime - mLastUpdate;

   /* Update the input integral */
   mIntegratedValue += dT * q * aInputSample;

   /* Compute the input derivative */
   if (!mIsStarted)
   {
      mIsStarted = true;
   }
   else
   {
      deriv = (aInputSample - mPreviousInput) * (1.0 / dT);
   }

   mPreviousInput = aInputSample;
   mLastUpdate    = aSimTime;

   /* Return the PID controller actuator command */
   // return m_kp*(input + m_ki*m_integral + m_kd*deriv);
   mLastOutput = mProportionalGain * (aInputSample + mIntegralGain * mIntegratedValue + mDerivativeGain * deriv);

   if ((mCurrentStreamPtr != nullptr) && (mOutputEnabled) && ((mCounter % mOutputModulus) == 0))
   {
      // Output an indication of the current relative contributions of P, I, and D terms.
      double p   = mProportionalGain * aInputSample;
      double i   = mProportionalGain * mIntegralGain * mIntegratedValue;
      double d   = mProportionalGain * mDerivativeGain * deriv;
      double mag = sqrt(p * p + i * i + d * d);
      p /= mag;
      i /= mag;
      d /= mag;

      *mCurrentStreamPtr << mCounter << ", " << aSimTime << ", " << aInputSample << ", " << mIsStarted << ", " << q
                         << ", " << dT << ", " << deriv << ", " << mIntegratedValue << ", " << mLastOutput << ", (" << p
                         << ", " << i << ", " << d << ")" << std::endl;
   }

   ++mCounter;

   return mLastOutput;
}

// ============================================================================
void UtPID_Controller::SetStreamPtr(std::ostream* aStreamPtr)
{
   mCurrentStreamPtr = aStreamPtr;
}

// ============================================================================
//! Opens a file or stream for outputting data.
//! Note that only the stream header is written, until mOutputEnabled is set true and Update() is called.
void UtPID_Controller::OpenFile()
{
   if (mStreamName == "STDOUT")
   {
      SetStreamPtr(&std::cout);
   }
   else if (mStreamName == "NULL")
   {
      SetStreamPtr(nullptr);
   }
   else if (!mStreamName.empty())
   {
      mFileStream.open(mStreamName.c_str());
      if (mFileStream.is_open())
      {
         SetStreamPtr(&mFileStream);
      }
   }
}
