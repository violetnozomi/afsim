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

#ifndef WSFSIXDOFPID_HPP
#define WSFSIXDOFPID_HPP

#include "wsf_six_dof_export.h"

#include <stdio.h>
#include <vector>

class UtInputBlock;
#include "UtOptional.hpp"
#include "WsfSixDOF_VehicleData.hpp"

// PID defines a Proportional, Integral, Derivative (PID) controller
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

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PID
{
public:
   PID()  = default;
   ~PID() = default;

   PID& operator=(const PID& other) = delete;

   PID* Clone() const;

   PID(const PID& aSrc) = default;

   // This function will provide an error warning if a mixture of tabular and scalar gain data is used.
   void ProcessInput(UtInputBlock& aInputBlock);

   // This is the main method is used to update the PID. It uses
   // the set point and current value to determine the error.
   double CalcOutputFromTarget(double aSetPoint, double aCurValue, double aSimTime_sec);

   // This method includes output limits.
   double CalcOutputFromTargetWithLimits(double aSetPoint,
                                         double aCurValue,
                                         double aSimTime_sec,
                                         double aMinOutput,
                                         double aMaxOutput);

   // This method is used when the error needs to be calculated outside
   // of the method, such as when dealing with circular values such
   // as angles.
   double CalcOutputFromError(double aError, double aCurValue, double aSimTime_sec);

   // This method includes output limits.
   double CalcOutputFromErrorWithLimits(double aError, double aCurValue, double aSimTime_sec, double aMinOutput, double aMaxOutput);

   // This sets the current value of the plant
   void SetCurrentValue(double aCurrentValue);

   // This sets the target value (set point) for the plant
   void SetTargetValue(double aTargetValue);

   // This sets the controlling value (such as dynamic pressure). This only has
   // an effect if the controlling value is enabled
   void SetControllingValue(double aControllingValue);

   // This zeroes out calculated state data, useful when resetting parent vehicle state
   void ResetPidState();

   // This zeroes out sim-time data, useful when returning from wash-in
   void ResetPidTiming();

   // This gets the current gains of the PID
   std::vector<PidGainData>* GetPidGainTableData();

   // This sets the gains of the PID
   void SetPidGainTableData(const std::vector<PidGainData>& aDataTables);

   // This gets the current values of the PID
   void GetPidValueData(SinglePidValueData& aData) const;

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

   // This provides the sim time interval at which the PID operates
   double GetUpdateInterval_sec();

   // This attempts to set the update interval, but defers to a pre-existing value
   // Returns true if the interval was successfully set by this function
   bool TrySetUpdateInterval_sec(double aInterval_sec);

private:
   void ProcessPidGainTableBlock(UtInputBlock& aInputBlock,
                                 PidGainData&  aTableElement,
                                 bool&         aValidControllingValue,
                                 bool&         aValidKpGain,
                                 bool&         aValidKiGain,
                                 bool&         aValidKdGain,
                                 bool&         aValidMaxAccum,
                                 bool&         aValidMaxErrorZero,
                                 bool&         aValidMinErrorZero,
                                 bool&         aValidLowpassAlpha,
                                 bool&         aValidKtAntiWindup);

   double GetOutputWithLimits(double aSimTime_sec, double aMinOutput, double aMaxOutput, bool aUseLimits);

   void CalcPidGainsData(std::vector<PidGainData>& aTables,
                         double                    aControllingValue,
                         double&                   aKpGain,
                         double&                   aKiGain,
                         double&                   aKdGain,
                         double&                   aLowpassAlpha,
                         double&                   aMaxAccum,
                         double&                   aMaxErrorZero,
                         double&                   aMinErrorZero,
                         double&                   aKtAntiWindup);

   float TruncatePidGain(float aGain);

   static void CopyPidGainsData(std::vector<PidGainData>& aTargetTables, const std::vector<PidGainData>& aSourceTables);

   double               mLastSimTime_sec = 0.0;
   ut::optional<double> mUpdateInterval_sec;

   double mSetPoint          = 0.0; // PID set point (target setting)
   double mCurrentValue      = 0.0; // Current value of PID
   double mCurrentError      = 0.0; // Current error of PID
   double mCurrentDerivative = 0.0; // Current derivative of PID

   double mLastValue      = 0.0; // Previous value
   double mLastError      = 0.0; // Previous error
   double mLastDerivative = 0.0; // Previous derivative

   double mErrorAccum       = 0.0; // Accumulated error
   double mPrelimitedOutput = 0.0; // Output value before any limiting
   double mOutput           = 0.0; // Current output value

   double mKpOutputContrib = 0.0; // Most recent contribution to output by Kp
   double mKiOutputContrib = 0.0; // Most recent contribution to output by Ki
   double mKdOutputContrib = 0.0; // Most recent contribution to output by Kd

   // The flags are used by both single data (above) and tabular data (below)

   // These tables provide PID gain data when mEnableControllingValue is true
   std::vector<PidGainData> mGainTables;

   bool   mProportionalBiasActive = false;
   double mProportionalBiasValue  = 0.0; // This is a bias that is added to the proportional channel

   // Controlling Value -- This allows tabular PID gain data, often using
   // dynamic pressure as the "controlling value". For example, Kp can use
   // a table of values and interpolate based on the current controlling
   // value. This allows Kp to be a function of dynamic pressure, for instance.
   double mControllingValue = 0.0;
};
} // namespace six_dof
} // namespace wsf

#endif // WSFSIXDOFPID_H
