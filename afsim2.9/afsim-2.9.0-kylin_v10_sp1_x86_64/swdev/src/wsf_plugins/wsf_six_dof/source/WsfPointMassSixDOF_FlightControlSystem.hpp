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

#ifndef WSFPOINTMASSSIXDOFFLIGHTCONTROLSYSTEM_HPP
#define WSFPOINTMASSSIXDOFFLIGHTCONTROLSYSTEM_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "WsfPointMassSixDOF_ControlActuator.hpp"
#include "WsfPointMassSixDOF_VehicleData.hpp"
#include "WsfSixDOF_KinematicState.hpp"
#include "WsfSixDOF_Object.hpp"

namespace wsf
{
namespace six_dof
{
class PointMassPilotManager;
class PointMassPilotObject;

// The PointMassFlightControlSystem class provides a means to transform control inputs into
// control surface movement. PointMassFlightControlSystem connects to a PointMassSixDOF_PilotController
// to get control inputs. It offers functions for PointMassSixDOF_Objects to get control surface
// positions. Control surfaces include typical aerodynamic control surfaces but can
// also include unconventional control surfaces as well as other components (such as
// landing gear) that can produce forces and moments that are a function of extended
// position of the component.

class WSF_SIX_DOF_EXPORT PointMassFlightControlSystem : public Object
{
public:
   PointMassFlightControlSystem() = default;

   PointMassFlightControlSystem& operator=(const PointMassFlightControlSystem& other) = delete;

   PointMassFlightControlSystem* Clone() const;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);

   bool Initialize(int64_t aSimTime_nanosec);

   void Update(int64_t aSimTime_nanosec);

   void SetLastUpdateSimTime(int64_t aLastUpdateSimTime_nanosec)
   {
      mLastUpdateSimTime_nanosec = aLastUpdateSimTime_nanosec;
   }

   // This sets the pilot manager so the flight controls can get the active pilot object
   void SetPilotManager(PointMassPilotManager* aPointMassSixDOF_PilotManager);

   // This sets the pilot object, allowing the flight controls to get control inputs
   void SetPilotObject(PointMassPilotObject* aPointMassSixDOF_PilotObject);

   // .....................................................................................................

   const double* GetThrottleMilPtr() const { return &mThrottleMil; }
   const double* GetThrottleAbPtr() const { return &mThrottleAb; }

   double GetRollRateCommand_dps()
   {
      if (mStickRightCurvePtr != nullptr)
      {
         return mStickRightCurvePtr->Lookup(mStickRight);
      }

      return 0.0;
   }

   double GetPitchRateCommand_dps()
   {
      if (mStickBackCurvePtr != nullptr)
      {
         return mStickBackCurvePtr->Lookup(mStickBack);
      }

      return 0.0;
   }

   double GetYawRateCommand_dps()
   {
      if (mRudderRightCurvePtr != nullptr)
      {
         return mRudderRightCurvePtr->Lookup(mRudderRight);
      }

      return 0.0;
   }

   UtVec3dX GetBodyRateCommands_dps()
   {
      return UtVec3dX(GetRollRateCommand_dps(), GetPitchRateCommand_dps(), GetYawRateCommand_dps());
   }

   double GetFlapsPosition() { return mFlaps.GetCurrentSetting(); }

   double GetSpoilersPosition() { return mSpoilers.GetCurrentSetting(); }

   double GetSpeedbrakesPosition() { return mSpeedbrakes.GetCurrentSetting(); }

   // .....................................................................................................

   double FindStickBackForPitchCommand(double aPitchRate_rps)
   {
      return FindControlPositionForRotationCommand(aPitchRate_rps, *mStickBackCurvePtr);
   }

   double FindStickRightForRollCommand(double aRollRate_rps)
   {
      return FindControlPositionForRotationCommand(aRollRate_rps, *mStickRightCurvePtr);
   }

   double FindRudderRightForYawCommand(double aYawRate_rps)
   {
      return FindControlPositionForRotationCommand(aYawRate_rps, *mRudderRightCurvePtr);
   }

protected:
   PointMassFlightControlSystem(const PointMassFlightControlSystem& aSrc);

private:
   double LinearInterpolation(double aX, double aX0, double aX1, double aY0, double aY1)
   {
      return aY0 + (aX - aX0) * (aY1 - aY0) / (aX1 - aX0);
   }

   double FindControlPositionForRotationCommand(double aOmegaCommand, UtTable::Curve aControlCurve);

   PointMassPilotObject*  mPointMassSixDOF_PilotObjectPtr = nullptr;
   PointMassPilotManager* mPointMassSixDOF_PilotManager   = nullptr;

   int64_t mLastUpdateSimTime_nanosec = 0;

   UtCloneablePtr<UtTable::Curve> mStickBackCurvePtr{
      nullptr}; // Mapping between stick-back position and pitch rate command (deg/s)
   UtCloneablePtr<UtTable::Curve> mStickRightCurvePtr{
      nullptr}; // Mapping between stick-right position and roll rate command (deg/s)
   UtCloneablePtr<UtTable::Curve> mRudderRightCurvePtr{
      nullptr}; // Mapping between rudder-right position and yaw rate command (deg/s)

   double mStickBack   = 0.0;
   double mStickRight  = 0.0;
   double mRudderRight = 0.0;
   double mThrottleMil = 0.0;
   double mThrottleAb  = 0.0;

   PointMassControlActuator mFlaps       = PointMassControlActuator(this);
   PointMassControlActuator mSpeedbrakes = PointMassControlActuator(this);
   PointMassControlActuator mSpoilers    = PointMassControlActuator(this);
};
} // namespace six_dof
} // namespace wsf

#endif
