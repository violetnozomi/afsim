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

#ifndef UTPID_CONTROLLER_HPP
#define UTPID_CONTROLLER_HPP

#include "ut_export.h"

#include <fstream>

#include "UtInput.hpp"

//! A Proportional Integral Differential Controller (PIDC) utility.
//! Implements a "standard" form PIDC, as opposed to the ideal parallel form.
//! The C source for this implementation was obtained from the link below,
//! and converted to C++.  Unfortunately, the link is unreliable, and does
//! not have a valid security certificate.  Consult the Ziegler-Nichols
//! method or other resources for advice on selecting PID gains to tune
//! the response to your particular system.  Implementation must assure
//! that Update() time interval is fixed.
//! Source:  https://www.hasustorm.com/books/English/CMP%20Books%20-%20Em
//! bedded%20Control%20Systems%20in%20C-C++%20-%20An%20Introduction%20for
//! %20Software%20Developers%20Using%20MATLAB.chm/6502/DDU0019.html

class UT_EXPORT UtPID_Controller
{
public:
   enum ControlType
   {
      P   = 0,
      PI  = 1,
      PID = 2
   };

   // ===== Constructors =====

   UtPID_Controller();
   UtPID_Controller(const UtPID_Controller& aSrc);
   virtual ~UtPID_Controller() = default;

   // ====== virtual class methods =========

   // This is intended as a one-time call.  Will open an output stream if specified.
   // During run-time, to Reset the PID, use Reset(), below.  Initialize calls Reset().
   virtual bool Initialize(double aSimTime, double aSampleInterval);

   virtual void Reset(double aSimTime);

   virtual bool ProcessInput(UtInput& aInput);
   virtual void ProcessInputBlock(UtInput& aInput);

   virtual double Update(double aSimTime, double aInputSample);

   // ===== Accessors =====

   // This function selects classical Ziegler-Nichols Tuning constants.  Will override and replace any current gain settings.
   // Not guaranteed to work for your particular application, but is a useful starting point:
   // aUpdateInterval = Sampling Interval
   // aUltimateGain   = Gain at which the system just begins to oscillate, but damps out or stays of constant amplitude.
   // aUltimatePeriod = Full-Wave oscillation period at the ultimate gain.
   // After this function is called, may query the object for the calculated gain values.
   void SelectZieglerNicholsTuning(ControlType aType, double aUltimateGain, double aUltimatePeriod, bool aNegateP_Gain = false);

   double GetProportionalGain() const { return mProportionalGain; }
   double GetIntegralGain() const { return mIntegralGain; }
   double GetDerivativeGain() const { return mDerivativeGain; }
   double GetIntegratedValue() const { return mIntegratedValue; }
   double GetThresholdValue() const { return mInputThreshold; }

   void SetProportionalGain(double aValue) { mProportionalGain = aValue; }
   void SetIntegralGain(double aValue) { mIntegralGain = aValue; }
   void SetDerivativeGain(double aValue) { mDerivativeGain = aValue; }
   void SetPrintModulus(unsigned int aValue) { mOutputModulus = aValue; }

   void SetStreamOutputEnable(bool aValue) { mOutputEnabled = aValue; }

   const std::string& GetStreamName() { return mStreamName; }

   UtPID_Controller& operator=(const UtPID_Controller& aSrc);

protected:
   void OpenFile();
   void SetStreamPtr(std::ostream* aStreamPtr);

   unsigned int mCounter;

   bool   mIsStarted;
   double mLastUpdate;

   double mProportionalGain;
   double mIntegralGain;
   double mDerivativeGain;

   double mIntegratedValue;

   double mPreviousInput;
   double mInputThreshold;

   double mLastOutput;

   // Following are output/debug/tuning attributes:
   bool          mOutputEnabled;
   std::ostream* mCurrentStreamPtr;
   std::ofstream mFileStream;
   std::string   mStreamName;
   unsigned int  mOutputModulus;
};

#endif
