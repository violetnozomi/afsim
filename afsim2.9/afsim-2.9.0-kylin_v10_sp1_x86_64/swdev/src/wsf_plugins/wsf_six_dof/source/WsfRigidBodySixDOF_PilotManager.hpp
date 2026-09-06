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

#ifndef WSFRIGIDBODYSIXDOFPILOTMANAGER_HPP
#define WSFRIGIDBODYSIXDOFPILOTMANAGER_HPP

#include "WsfRigidBodySixDOF_CommonController.hpp"
#include "WsfRigidBodySixDOF_ManualPilotAugmentedControls.hpp"
#include "WsfRigidBodySixDOF_ManualPilotSimpleControls.hpp"
#include "WsfRigidBodySixDOF_PilotObject.hpp"
#include "WsfRigidBodySixDOF_SyntheticPilot.hpp"
#include "WsfSixDOF_PilotManager.hpp"

namespace wsf
{
namespace six_dof
{
class RigidBodyMover;

// The RigidBodyPilotManager manages the RigidBodySixDOF_PilotObjects on a vehicle.
// Note that it does *not* include an Update function.
class WSF_SIX_DOF_EXPORT RigidBodyPilotManager : public PilotManager
{
public:
   RigidBodyPilotManager()  = default;
   ~RigidBodyPilotManager() = default;

   RigidBodyPilotManager& operator=(const RigidBodyPilotManager& other) = delete;

   RigidBodyPilotManager* Clone() const;

   bool ProcessInput(UtInput& aInput);

   // This will initialize all pilot objects
   bool Initialize(int64_t aSimTime_nanosec) override;

   void SetLastSimTime(int64_t aLastSimTime_nanosec) override;

   virtual void SetParentVehicle(RigidBodyMover* aParentVehicle);

   // This returns the active pilot object
   PilotObject* GetActivePilot() override;

   RigidBodyPilotObject* GetActiveRigidBodyPilot() { return mActivePilot; }

   // If a manual pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   PilotObject* GetActiveSimpleManualPilot() override;

   // If a manual pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   PilotObject* GetActiveAugmentedManualPilot() override;

   // If a synthetic pilot is the active pilot, it will be returned.
   // Otherwise, this function will return null.
   PilotObject* GetActiveSyntheticPilot() override;

   // This is called each time step by RigidBodySixDOF_Vehicle. The PilotObjects pass these
   // values to their autopilot, where values can be integrated to determine
   // deltaRoll, for example.
   void InputAngleDeltasToPilotObjects(const double& aYaw_rad, const double& aPitch_rad, const double& aRoll_rad) override;

   // This enables/disables controls for all pilot objects. When controls are
   // disabled, all control inputs are set to zero.
   void EnableControls(bool aEnabled) override;

   // This returns the controls enabled/disabled state of the active pilot object.
   bool AreControlsEnabled() const override;

   // This makes the manual pilot the active pilot. Returns true if succeeds.
   bool MakeSimpleManualPilotActive() override;

   // This makes the manual pilot the active pilot. Returns true if succeeds.
   bool MakeAugmentedManualPilotActive() override;

   // This makes the synthetic pilot the active pilot. Returns true if succeeds.
   bool MakeSyntheticPilotActive() override;

   // This function is called by Mover when the vehicle is destroyed in
   // flight. The aPitchInputModifier and aRollInputModifier arguments vary between
   // +/-1 and control the pitch and roll control when destroyed.
   void SetDestroyed(double aPitchInputModifier, double aRollInputModifier) override;

   // This provides a reference to the special common controller, which is used to access
   // the functions such as GetAlphaVsMachCL(), GetStickForZeroMomentVsMachAlpha(),
   // and GetEffectiveCLVsMachAlpha() which are used in testing.
   const CommonController* GetSpecialCommonController() const override { return mSpecialCommonController.get(); }

   const RigidBodyCommonController* GetSpecialRigidBodyCommonController() const
   {
      return mSpecialCommonController.get();
   }

protected:
   RigidBodyPilotManager(const RigidBodyPilotManager& aSrc);

   UtCloneablePtr<RigidBodyManualPilotSimpleControls>    mManualPilotSimpleControls{nullptr};
   UtCloneablePtr<RigidBodyManualPilotAugmentedControls> mManualPilotAugmentedControls{nullptr};
   UtCloneablePtr<RigidBodySyntheticPilot>               mSyntheticPilot{nullptr};

   RigidBodyPilotObject* mActivePilot = nullptr;

   UtCloneablePtr<RigidBodyCommonController> mSpecialCommonController{nullptr};

   bool mIsDestroyed = false;
};
} // namespace six_dof
} // namespace wsf

#endif
