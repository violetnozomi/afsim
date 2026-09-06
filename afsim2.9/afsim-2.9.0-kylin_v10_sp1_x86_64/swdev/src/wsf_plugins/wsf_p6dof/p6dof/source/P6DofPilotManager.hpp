// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef P6DOFPILOTMANAGER_H
#define P6DOFPILOTMANAGER_H

#include "p6dof_export.h"

#include <cinttypes>
#include <list>
#include <string>
#include <unordered_map>

class P6DofCommonController;
class P6DofGuidanceAutopilotBankToTurn;
class P6DofGuidanceAutopilotSkidToTurn;
class P6DofHardwareAutopilotBankToTurn;
class P6DofHardwareAutopilotSkidToTurn;
class P6DofManualPilotAugmentedControls;
class P6DofManualPilotSimpleControls;
class P6DofPilotObject;
class P6DofScenario;
class P6DofSyntheticPilot;
class P6DofVehicle;
class UtInput;

// The P6DofPilotManager manages the P6DofPilotObjects on a vehicle.
// Note that it does *not* include an Update function.
class P6DOF_EXPORT P6DofPilotManager
{
public:
   explicit P6DofPilotManager(P6DofScenario* aScenario);
   virtual ~P6DofPilotManager();

   P6DofPilotManager& operator=(const P6DofPilotManager& other) = delete;

   P6DofPilotManager* Clone() const;

   bool ProcessInput(UtInput& aInput);

   // This will initialize all pilot objects
   bool Initialize(int64_t aSimTime_nanosec);

   void SetLastSimTime(int64_t aLastSimTime_nanosec);

   virtual void SetParentVehicle(P6DofVehicle* aParentVehicle);

   // This returns the active pilot object
   P6DofPilotObject* GetActivePilot();

   // If a manual pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   P6DofPilotObject* GetActiveManualPilot();

   // If a synthetic pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   P6DofPilotObject* GetActiveSyntheticPilot();

   // If a hardware autopilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   P6DofPilotObject* GetActiveHardwareAutopilot();

   // If a guidance autopilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   P6DofPilotObject* GetActiveGuidanceAutopilot();

   // This is called each time step by P6DofVehicle. The PilotObjects pass these
   // values to their autopilot, where values can be integrated to determine
   // deltaRoll, for example.
   void InputAngleDeltasToPilotObjects(const double& aYaw_rad, const double& aPitch_rad, const double& aRoll_rad);

   // This enables/disables controls for all pilot objects. When controls are
   // disabled, all control inputs are set to zero.
   virtual void EnableControls(bool aEnabled);

   // This returns the controls enabled/disabled state of the active pilot object.
   virtual bool ControlsEnabled() const;

   // This makes the manual pilot the active pilot. Returns true if succeeds.
   bool MakeManualPilotActive();

   // This makes the synthetic pilot the active pilot. Returns true if succeeds.
   bool MakeSyntheticPilotActive();

   // This makes the hardware autopilot the active pilot. Returns true if succeeds.
   bool MakeHardwareAutopilotActive();

   // This makes the guidance autopilot the active pilot. Returns true if succeeds.
   bool MakeGuidanceAutopilotActive();

   // This function is called by P6DofVehicle when the vehicle is destroyed in
   // flight. The aPitchInputModifier and aRollInputModifier arguments vary between
   // +/-1 and control the pitch and roll control when destroyed.
   void SetDestroyed(double aPitchInputModifier, double aRollInputModifier);

   // This provides a reference to the special common controller, which is used to access
   // the functions such as GetAlphaVsMachCL(), GetStickForZeroMomentVsMachAlpha(),
   // and GetEffectiveCLVsMachAlpha() which are used in testing.
   const P6DofCommonController* GetSpecialCommonController() const { return mSpecialCommonController; }

protected:
   P6DofPilotManager() = default;
   P6DofPilotManager(const P6DofPilotManager& aSrc);

   P6DofScenario*                     mScenario;
   P6DofManualPilotSimpleControls*    mManualPilotSimpleControls;
   P6DofManualPilotAugmentedControls* mManualPilotAugmentedControls;
   P6DofSyntheticPilot*               mSyntheticPilot;
   P6DofHardwareAutopilotBankToTurn*  mHardwareAutopilotBankToTurn;
   P6DofHardwareAutopilotSkidToTurn*  mHardwareAutopilotSkidToTurn;
   P6DofGuidanceAutopilotBankToTurn*  mGuidanceAutopilotBankToTurn;
   P6DofGuidanceAutopilotSkidToTurn*  mGuidanceAutopilotSkidToTurn;
   P6DofPilotObject*                  mActivePilot;
   bool                               mIsDestroyed;
   P6DofCommonController*             mSpecialCommonController;
};

#endif
