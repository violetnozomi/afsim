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

#ifndef WSFSIXDOFROCKETLIQUIDPROPELLANTENGINE_HPP
#define WSFSIXDOFROCKETLIQUIDPROPELLANTENGINE_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

class UtInput;
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Engine.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT RocketLiquidPropellantEngine : public Engine
{
public:
   // This is used to create a type of a RocketLiquidPropellantEngine
   explicit RocketLiquidPropellantEngine(ThrustProducerObject* aParentObject);

   ~RocketLiquidPropellantEngine();

   RocketLiquidPropellantEngine& operator=(const RocketLiquidPropellantEngine& other) = delete;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   Engine* Clone() const override;

   void DeriveFrom(const Engine* aSrc) override;

   // GetPercentMaxThrust returns the last 'updated' engine thrust
   // in terms of percent of maximum thrust
   double GetPercentMaxThrust() const;

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   double GetFuelBurnRate_pph() const override;

   // This provides the maximum potential thrust available, if full throttle is applied
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
   RocketLiquidPropellantEngine(const RocketLiquidPropellantEngine& aSrc);

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

   bool MayProduceSmokeTrail() const override { return mGenerateSmokeTrail; }

   double mThrottleLeverPosition     = 0.0;
   double mLastThrottleLeverPosition = 0.0;

   // Normalized thrust as a function of altitude
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mNormalizedThrustAlt{nullptr};

   // Specific impulse as a function of altitude
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mIspAlt{nullptr};

   // Throttle versus time. This is an optional table to drive the
   // throttle. Otherwise, conventional throttle control is used.
   UtCloneablePtr<UtTable::Curve> mThrottleVersusTime{nullptr};

   // Max rated thrust at sea level or vacuum.
   // One of these must be specified.
   double mMaxThrustSeaLevel = 0.0;
   // double mMaxThrustVacuum;

   // Max and min throttle range. These must be specified, or a
   // default of 1.0 and 0.0 will be used.
   // Note: Max throttle may be greater than 1.0 (100%).
   double mMaxThrottle = 1.0;
   double mMinThrottle = 0.0;

   // Propellant mass flow at sea level or vacuum
   // One of these will be calculated
   double mMaxMassFlowSeaLevel = 0.0;
   double mMaxMassFlowVacuum   = 0.0;

   double mEngineThrustPercent = 0.0;

   bool   mIsBurning    = false;
   double mBurnTime_Sec = 0.0;

   double mNormalizedSpinUp_per_sec   = 0.0;
   double mNormalizedSpinDown_per_sec = 0.0;

   double mCurrentFuelBurnRate_pph = 0.0;

   bool mGenerateSmokeTrail = false;
};
} // namespace six_dof
} // namespace wsf

#endif
