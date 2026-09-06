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

#ifndef WSFSIXDOFPILOTMANAGER_HPP
#define WSFSIXDOFPILOTMANAGER_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <list>
#include <string>
#include <unordered_map>

class UtInput;

namespace wsf
{
namespace six_dof
{
class CommonController;
class PilotObject;

// The PilotManager manages the PilotObjects on a vehicle.
// Note that it does *not* include an Update function.
class WSF_SIX_DOF_EXPORT PilotManager
{
public:
   PilotManager()          = default;
   virtual ~PilotManager() = default;

   PilotManager& operator=(const PilotManager& other) = delete;

   // This will initialize all pilot objects
   virtual bool Initialize(int64_t aSimTime_nanosec) = 0;

   virtual void SetLastSimTime(int64_t aLastSimTime_nanosec) = 0;

   // virtual void SetParentVehicle(SixDOF_Vehicle* aParentVehicle) = 0;

   // This returns the active pilot object
   virtual PilotObject* GetActivePilot() = 0;

   // If a manual pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   virtual PilotObject* GetActiveSimpleManualPilot() = 0;

   // If a manual pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   virtual PilotObject* GetActiveAugmentedManualPilot() = 0;

   // If a synthetic pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   virtual PilotObject* GetActiveSyntheticPilot() = 0;

   // This makes the manual pilot the active pilot. Returns true if succeeds.
   virtual bool MakeSimpleManualPilotActive() = 0;

   // This makes the manual pilot the active pilot. Returns true if succeeds.
   virtual bool MakeAugmentedManualPilotActive() = 0;

   // This makes the synthetic pilot the active pilot. Returns true if succeeds.
   virtual bool MakeSyntheticPilotActive() = 0;

   // This is called each time step by Mover. The PilotObjects pass these
   // values to their autopilot, where values can be integrated to determine
   // deltaRoll, for example.
   virtual void InputAngleDeltasToPilotObjects(const double& aYaw_rad, const double& aPitch_rad, const double& aRoll_rad) = 0;

   // This enables/disables controls for all pilot objects. When controls are
   // disabled, all control inputs are set to zero.
   virtual void EnableControls(bool aEnabled) = 0;

   // This returns the controls enabled/disabled state of the active pilot object.
   virtual bool AreControlsEnabled() const = 0;

   // This function is called by Mover when the vehicle is destroyed in
   // flight. The aPitchInputModifier and aRollInputModifier arguments vary between
   // +/-1 and control the pitch and roll control when destroyed.
   virtual void SetDestroyed(double aPitchInputModifier, double aRollInputModifier) = 0;

   // This provides a reference to the special common controller, which is used to access
   // the functions such as GetAlphaVsMachCL(), GetStickForZeroMomentVsMachAlpha(),
   // and GetEffectiveCLVsMachAlpha() which are used in testing.
   virtual const CommonController* GetSpecialCommonController() const = 0;
};
} // namespace six_dof
} // namespace wsf

#endif
