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

#ifndef P6DOFCONTROLACTUATOR_H
#define P6DOFCONTROLACTUATOR_H

#include <cinttypes>

class P6DofFlightControlSystem;
class UtInput;

class P6DofControlActuator
{
public:
   explicit P6DofControlActuator(P6DofFlightControlSystem* aFlightControlsParent);
   ~P6DofControlActuator()       = default;
   P6DofControlActuator& operator=(const P6DofControlActuator& other) = delete;

   P6DofControlActuator* Clone(P6DofFlightControlSystem* aFlightControlsParent) const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   // This performs an update
   void Update(int64_t aSimTime_nanosec, double aCommandedAngle_deg);

   // This performs an update and returns the angle after the update
   double UpdateAndGetCurrentAngle_deg(int64_t aSimTime_nanosec, double aCommandedAngle_deg);

   // This returns the current angle
   double GetCurrentAngle_deg() const;

   // This may be used to set the current angle. Note that this will instantaneously
   // set the angle without concern for lags and motion rates.
   void SetCurrentAngle_deg(double aAngle_deg);

protected:
   P6DofControlActuator(const P6DofControlActuator& aSrc, P6DofFlightControlSystem* aFlightControlsParent);

   double                    mMaxPositiveRate_dps;
   double                    mMaxNegativeRate_dps;
   double                    mMaxAngle_deg;
   double                    mMinAngle_deg;
   double                    mCurrentAngle_deg;
   double                    mCommandedAngle_deg;
   int64_t                   mLastSimTime_nanosec;
   double                    mLagTimeConstant_sec;
   P6DofFlightControlSystem* mFlightControlsParent;
};

#endif
