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

#ifndef P6DOFPID_H
#define P6DOFPID_H

#include <vector>

#include "P6DofVehicleData.hpp"

// P6DofPID defines a Proportional, Integral, Derivative (PID) controller
// class that is used by other autopilot classes. The PID includes the standard
// Kp, Ki, and Kd parameters as well as five additional attributes:
//
//   * MaxErrorAccum, limits accumulation to a max (absolute) value
//   * DerivativeLowpassAlpha, uses a low-pass filter on the derivative
//   * MaxErrorAntiWindup, halts accumulation if error is > value
//   * MinErrorAntiWindup, halts accumulation if error is < value
//   * Kt, which is an anti-windup to help address saturation
//
// It is also possible to use tabular gain data, if desired, based on a
// controlling parameter (such as dynamic pressure).

class P6DofPID
{
public:
   P6DofPID();
   ~P6DofPID();

   P6DofPID& operator=(const P6DofPID& other) = delete;

   P6DofPID* Clone() const;

   P6DofPID(const P6DofPID& aSrc);

   // This is the main method is used to update the PID. It uses
   // the set point and current value to determine the error.
   double CalcOutputFromTargetAndCurrent(double aSetPoint, double aCurValue, double aDt_sec);

   // This method includes output limits.
   double CalcOutputFromTargetAndCurrentWithLimits(double aSetPoint,
                                                   double aCurValue,
                                                   double aDt_sec,
                                                   double aMinOutput,
                                                   double aMaxOutput);

   // This method is used when the error needs to be calculated outside
   // of the method, such as when dealing with circular values such
   // as angles.
   double CalcOutputFromError(double aError, double aDt_sec);

   // This method includes output limits.
   double CalcOutputFromErrorWithLimits(double aError, double aDt_sec, double aMinOutput, double aMaxOutput);

   // This sets the current value of the plant
   void SetCurrentValue(double aCurrentValue);

   // This sets the target value (set point) for the plant
   void SetTargetValue(double aTargetValue);

   // This sets the controlling value (such as dynamic pressure). This only has
   // an effect if the controlling value is enabled
   void SetControllingValue(double aControllingValue);

   // This zeroes out calculated state data, useful when resetting parent vehicle state
   void ResetPidState();

   // This gets the current gains of the PID
   std::vector<P6DOF::PidGainData>* GetPidGainTableData(unsigned char& aPackedFlags);

   // This sets the gains of the PID
   void SetPidGainTableData(std::vector<P6DOF::PidGainData>& aDataTables, unsigned char& aPackedFlags);

   // This gets the current values of the PID
   void GetPidValueData(P6DOF::SinglePidValueData& aData) const;

   // This sets a bias (also know as "feed forward") for the proportional channel.
   // This bias will remain in effect until "cleared" by a value of zero.
   void SetBias(double aBias);
   void SetFeedForward(double aFeedForward);

   // This returns true if feed forward is active, and returns the
   // feed forward (bias) value by reference. Otherwise it returns
   // false and the reference value is undefined.
   bool GetFeedForward(double& aFeedForward);

   // This should be used to setup items in the gain table before calling the
   // various "set" functions such as SetKpTable, SetKtAntiWindupTable, etc. to
   // avoid an error in the set function due to insufficient elements in the table.
   void AddElementsToGainTable(size_t aTotalElementsInTable);

private:
   double GetOutputWithLimits(double aDt_sec, double aMinOutput, double aMaxOutput, bool aUseLimits);

   void CalcPidGainsData(std::vector<P6DOF::PidGainData>& aTables,
                         double                           aControllingValue,
                         double&                          aKpGain,
                         double&                          aKiGain,
                         double&                          aKdGain,
                         double&                          aLowpassAlpha,
                         double&                          aMaxAccum,
                         double&                          aMaxErrorZero,
                         double&                          aMinErrorZero,
                         double&                          aKtAntiWindup);

   static void CopyPidGainsData(std::vector<P6DOF::PidGainData>&       aTargetTables,
                                const std::vector<P6DOF::PidGainData>& aSourceTables);

   double mSetPoint;         // PID set point (target setting)
   double mCurrentValue;     // Current value of PID
   double mCurrentError;     // Current error of PID
   double mCurrentDeriv;     // Current derivative of PID
   double mLastError;        // Previous error
   double mLastDerivative;   // Previous derivative
   double mErrorAccum;       // Accumulated error
   double mPrelimitedOutput; // Output value before any limiting
   double mOutput;           // Output value
   double mKpOutputContrib;  // Most recent contribution to output by Kp
   double mKiOutputContrib;  // Most recent contribution to output by Ki
   double mKdOutputContrib;  // Most recent contribution to output by Kd

   // The flags are used by both single data (above) and tabular data (below)

   bool mUseAlpha;  // Use low-pass (alpha) filter on derivative
   bool mLimitMax;  // Limit accum to max accum value (mMaxErrorAccum)
   bool mZeroGtMax; // Do not accumulate when error > max value (mMaxErrorAntiWindup)
   bool mZeroLtMin; // Do not accumulate when error < min value (mMinErrorAntiWindup)
   bool mUseKt;     // Use Kt windup technique -- This is an anti-windup technique
                    // for the integral component that involves feeding back the
                    // error between the unlimited pid output and a limited pid
                    // output with a gain of Kt into the integral.

   // These tables provide PID gain data when mEnableControllingValue is true
   std::vector<P6DOF::PidGainData> mGainTables;

   bool   mProportionalBiasActive;
   double mProportionalBiasValue; // This is a bias that is added to the proportional channel

   // Controlling Value -- This allows tabular PID gain data, often using
   // dynamic pressure as the "controlling value". For example, Kp can use
   // a table of values and interpolate based on the current controlling
   // value. This allows Kp to be a function of dynamic pressure, for instance.
   double mControllingValue;
};

#endif // P6DOFPID_H
