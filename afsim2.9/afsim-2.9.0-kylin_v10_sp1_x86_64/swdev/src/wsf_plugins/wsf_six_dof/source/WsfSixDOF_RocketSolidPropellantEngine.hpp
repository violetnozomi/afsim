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

#ifndef WSFSIXDOFROCKETSOLIDPROPELLANTENGINE_HPP
#define WSFSIXDOFROCKETSOLIDPROPELLANTENGINE_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Engine.hpp"
#include "WsfSixDOF_MassProperties.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RocketSolidPropellantEngine : public Engine
{
public:
   // This is used to create a type of a RocketSolidPropellantEngine
   explicit RocketSolidPropellantEngine(ThrustProducerObject* aParentObject);

   ~RocketSolidPropellantEngine() override;

   RocketSolidPropellantEngine& operator=(const RocketSolidPropellantEngine& other) = delete;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   Engine* Clone() const override;

   void DeriveFrom(const Engine* aSrc) override;

   // The IgniteNow function is called to 'start' the rocket engine.
   void IgniteNow() { mIsBurning = true; }

   // GetPercentMaxThrust returns the last 'updated' engine thrust
   // in terms of percent of maximum thrust
   double GetPercentMaxThrust() const;

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   double GetFuelBurnRate_pph() const override;

   // This provides the maximum potential thrust available
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

   // The Ignite function is called to 'start' the rocket engine.
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) override;

   void Shutdown(int64_t aTerminateTime_nanosec = 0) override;

protected:
   RocketSolidPropellantEngine(const RocketSolidPropellantEngine& aSrc);

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
                        double& aForceAndMoment,
                        double& aFuelBurnRate_pps,
                        double& aFuelBurned_lbs,
                        bool    aUpdateData) override;

   double Thrust_lbs(double aBurnTime_Sec, double aAlt_ft, double& aMassFlow_pps);

   void UpdatePropellantMass(double aRemainingPropellant_lbs);

   bool MayProduceSmokeTrail() const override { return mGenerateSmokeTrail; }

   // Thrust versus time at sea level or vacuum.
   // One of these must be specified.
   UtCloneablePtr<UtTable::Curve> mThrustSeaLevelVersusTime{nullptr};
   UtCloneablePtr<UtTable::Curve> mThrustVacuumVersusTime{nullptr};

   // Specific impulse as a function of altitude.
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mIspAlt{nullptr};

   double mIspSeaLevel = 0.0;
   double mIspVacuum   = 0.0;

   double mRatedThrust_lbs = 0.0; // Used to determine percent thrust
   double mLastThrust_lbs  = 0.0;

   bool   mIsBurning    = false;
   double mBurnTime_sec = 0.0;

   double mCurrentFuelBurnRate_pph = 0.0;

   bool mGenerateSmokeTrail = true;
};
} // namespace six_dof
} // namespace wsf

#endif
