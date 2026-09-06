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

#ifndef WSFPOINTMASSSIXDOFCONTROLACTUATOR_HPP
#define WSFPOINTMASSSIXDOFCONTROLACTUATOR_HPP

#include <cinttypes>

class UtInputBlock;

namespace wsf
{
namespace six_dof
{
class PointMassFlightControlSystem;

class PointMassControlActuator
{
public:
   explicit PointMassControlActuator(PointMassFlightControlSystem* aFlightControlsParent);

   explicit PointMassControlActuator(const PointMassControlActuator& aSrc,
                                     PointMassFlightControlSystem*   aFlightControlsParent);

   ~PointMassControlActuator()       = default;
   PointMassControlActuator& operator=(const PointMassControlActuator& other) = delete;

   PointMassControlActuator* Clone(PointMassFlightControlSystem* aFlightControlsParent) const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInputBlock& aInputBlock);
   bool Initialize(int64_t aSimTime_nanosec);

   // This performs an update
   void Update(int64_t aSimTime_nanosec, double aCommandedAngle_deg);

   // This performs an update and returns the setting after the update
   double UpdateAndGetCurrentSetting(int64_t aSimTime_nanosec, double aCommandedSetting);

   // This returns the current angle
   double GetCurrentSetting() const;

   // This may be used to set the current angle. Note that this will instantaneously
   // set the angle without concern for lags and motion rates.
   void SetCurrentSetting(double aSetting);

protected:
   PointMassFlightControlSystem* mFlightControlsParent = nullptr;

   int64_t mLastSimTime_nanosec = 0;
   double  mCurrentSetting      = 0.0;
   double  mCommandedSetting    = 0.0;
   double  mLagTimeConstant_sec = 0.0;
};
} // namespace six_dof
} // namespace wsf


#endif
