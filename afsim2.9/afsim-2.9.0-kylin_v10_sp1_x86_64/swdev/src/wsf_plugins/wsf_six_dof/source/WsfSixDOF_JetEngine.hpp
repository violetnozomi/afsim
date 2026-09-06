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

#ifndef WSFSIXDOFJETENGINEOBJECT_HPP
#define WSFSIXDOFJETENGINEOBJECT_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

class UtInput;
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Engine.hpp"
#include "WsfSixDOF_ForceAndMomentsObject.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT JetEngine : public Engine
{
public:
   // This is used to create a type of a ThrustProducerObject
   explicit JetEngine(ThrustProducerObject* aParentObject);

   ~JetEngine() = default;

   JetEngine& operator=(const JetEngine& other) = delete;

   bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   Engine* Clone() const override;

   void DeriveFrom(const Engine* aSrc) override;

   // By default, a jet engine will have "injectFuel" set to true.
   // Setting it to false, will "shutdown" the engine, which will spool down
   // its thrust. Setting it to true will "startup" the engine, which will
   // spin up to idle thrust. Any change to the InjectFuel function should be
   // called prior to calling UpdateThrust.
   void InjectFuel(bool aInjectFuel) { mInjectFuel = aInjectFuel; }

   // GetEnginePercentRPM returns the last 'updated' engine percent rpm
   double GetEnginePercentRPM() const;

   // GetNozzlePosition returns the last 'updated' normalized nozzle position
   double GetNozzlePosition() const;

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   double GetFuelBurnRate_pph() const override;

   // This provides the maximum potential thrust available, if full throttle, including
   // afterburner (if available), is applied
   double GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                        double aDynPress_lbsqft,
                                        double aStatPress_lbssqft,
                                        double aSpeed_fps,
                                        double aMach,
                                        double aAlpha_rad,
                                        double aBeta_rad) override;

   // This provides the minimum potential thrust available
   double GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                        double aDynPress_lbsqft,
                                        double aStatPress_lbssqft,
                                        double aSpeed_fps,
                                        double aMach,
                                        double aAlpha_rad,
                                        double aBeta_rad) override;

   // The SetThrottleLeverPosition function should be called prior to calling
   // UpdateThrust so that the throttle will be properly set.
   void SetThrottlePosition(double aThrottleLeverPosition) override;

   // This returns the current throttle position
   double GetThrottlePosition() const override;

   // Start the engine.
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) override;

   void Shutdown(int64_t aTerminateTime_nanosec = 0) override;

protected:
   JetEngine(const JetEngine& aSrc);

   // This CalculateThrust function is called internally by either
   // CalculateThrust or UpdateThrust to calculate the engine's
   // forces and moments. The aUpdateData flag determines whether
   // or not to change the state of the engine.
   void CalculateThrust(double  aDeltaT_sec,
                        double  aAlt_ft,
                        double  aDynPress_lbsqft,
                        double  aStatPress_lbssqft,
                        double  aSpeed_fps,
                        double  aMach,
                        double  aAlpha_rad,
                        double  aBeta_rad,
                        double& aForce_lbs,
                        double& aFuelBurnRate_pps,
                        double& aFuelBurned_lbs,
                        bool    aUpdateData) override;

   bool DetermineIfAfterburnerIsPresent();

   double mThrottleLeverPosition = 0.0;

   // Simple Engine tabular data
   UtCloneablePtr<UtTable::Curve> mIdleThrustTable{nullptr};
   UtCloneablePtr<UtTable::Curve> mMilThrustTable{nullptr};
   UtCloneablePtr<UtTable::Curve> mABThrustTable{nullptr};

   // Improved Engine tabular data (mach/alt)
   UtCloneablePtr<UtTable::Table> mIdleThrustMachAltTable{nullptr};
   UtCloneablePtr<UtTable::Table> mMilThrustMachAltTable{nullptr};
   UtCloneablePtr<UtTable::Table> mABThrustMachAltTable{nullptr};

   // Improved Engine tabular data (alt/mach)
   UtCloneablePtr<UtTable::Table> mIdleThrustAltMachTable{nullptr};
   UtCloneablePtr<UtTable::Table> mMilThrustAltMachTable{nullptr};
   UtCloneablePtr<UtTable::Table> mABThrustAltMachTable{nullptr};

   double mTSFC_Idle_pph = 0.0;
   double mTSFC_Mil_pph  = 0.0;
   double mTSFC_AB_pph   = 0.0;

   double mRatedThrustIdle_lbs = 0.0;
   double mRatedThrustMil_lbs  = 0.0;
   double mRatedThrustAB_lbs   = 0.0;

   double mEffectiveTSFC_Idle_pps = 0.0;
   double mEffectiveTSFC_Mil_pps  = 0.0;
   double mEffectiveTSFC_AB_pps   = 0.0;

   double mLastThrottleLeverPosition = 0.0;
   double mEnginePercentRPM          = 0.0;
   double mNozzlePosition            = 0.0;

   double mSpinUpMil_per_sec   = 0.0;
   double mSpinDownMil_per_sec = 0.0;
   double mSpinUpAB_per_sec    = 0.0;
   double mSpinDownAB_per_sec  = 0.0;

   UtCloneablePtr<UtTable::Curve> mSpinUpMilTable{nullptr};
   UtCloneablePtr<UtTable::Curve> mSpinDownMilTable{nullptr};
   UtCloneablePtr<UtTable::Curve> mSpinUpABTable{nullptr};
   UtCloneablePtr<UtTable::Curve> mSpinDownABTable{nullptr};

   double mCurrentFuelBurnRate_pph = 0.0;

   bool mThrottleLeverPositionSet = false;

   bool mInjectFuel = true;

   bool   mEngineMaySmoke         = false;
   double mEngineSmokesAboveLevel = 1.0;
};
} // namespace six_dof
} // namespace wsf

#endif
