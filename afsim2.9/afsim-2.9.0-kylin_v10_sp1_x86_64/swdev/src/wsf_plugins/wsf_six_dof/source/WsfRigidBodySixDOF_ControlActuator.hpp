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

#ifndef WSFRIGIDBODYSIXDOFCONTROLACTUATOR_HPP
#define WSFRIGIDBODYSIXDOFCONTROLACTUATOR_HPP

#include <cinttypes>

class UtInput;

namespace wsf
{
namespace six_dof
{
class RigidBodyFlightControlSystem;

class RigidBodyControlActuator
{
public:
   explicit RigidBodyControlActuator(RigidBodyFlightControlSystem* aFlightControlsParent);
   ~RigidBodyControlActuator()       = default;
   RigidBodyControlActuator& operator=(const RigidBodyControlActuator& other) = delete;

   RigidBodyControlActuator* Clone(RigidBodyFlightControlSystem* aFlightControlsParent) const;

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
   RigidBodyControlActuator(const RigidBodyControlActuator& aSrc, RigidBodyFlightControlSystem* aFlightControlsParent);

   RigidBodyFlightControlSystem* mFlightControlsParent = nullptr;

   double  mMaxPositiveRate_dps = 0.0;
   double  mMaxNegativeRate_dps = 0.0;
   double  mMaxAngle_deg        = 0.0;
   double  mMinAngle_deg        = 0.0;
   double  mCurrentAngle_deg    = 0.0;
   double  mCommandedAngle_deg  = 0.0;
   int64_t mLastSimTime_nanosec = 0;
   double  mLagTimeConstant_sec = 0.0;
};
} // namespace six_dof
} // namespace wsf


#endif
