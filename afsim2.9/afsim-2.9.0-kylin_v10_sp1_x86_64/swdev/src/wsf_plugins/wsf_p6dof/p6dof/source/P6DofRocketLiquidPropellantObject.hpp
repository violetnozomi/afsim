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

#ifndef P6DOFROCKETLIQUIDPROPELLANTOBJECT_H
#define P6DOFROCKETLIQUIDPROPELLANTOBJECT_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofThrustProducerObject.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofFuelTank;
class UtInput;

class P6DOF_EXPORT P6DofRocketLiquidPropellantObject : public P6DofThrustProducerObject
{
public:
   // This is used to create a type of a P6DofThrustProducerObject
   explicit P6DofRocketLiquidPropellantObject(P6DofScenario* aScenario);

   ~P6DofRocketLiquidPropellantObject() override;

   P6DofRocketLiquidPropellantObject& operator=(const P6DofRocketLiquidPropellantObject& other) = delete;

   // Each engine class returns its own unique class type
   std::string GetClassType() override { return "LiquidRocket"; }

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   P6DofRocketLiquidPropellantObject* Clone() const;

   void DeriveFrom(const P6DofRocketLiquidPropellantObject* aSrc);

   void SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls) override;
   void SetControlInputValues(const P6DofFlightControlSystem* aFlightControls) override;

   // The CalculateThrust function is called to calculate the engine's
   // forces and moments. It does not change the state of the engine.
   // To change the state, call UpdateThrust instead. This version
   // uses the thrust producer's internal throttle values.
   void CalculateThrust(double                      aDeltaT_sec,
                        double                      aAlt_ft,
                        double                      aDynPress_lbsqft,
                        double                      aStatPress_lbssqft,
                        double                      aSpeed_fps,
                        double                      aMach,
                        double                      aAlpha_rad,
                        double                      aBeta_rad,
                        P6DofForceAndMomentsObject& aForceAndMoment,
                        double&                     aFuelBurnRate_pps,
                        double&                     aFuelBurned_lbs) override;

   // The UpdateThrust function is called to calculate the engine's
   // forces and moments and update the state of the engine.
   // To perform F&M calculations without changing the state, call the
   // CalculateThrust function instead.
   void UpdateThrust(double                      aDeltaT_sec,
                     double                      aAlt_ft,
                     double                      aDynPress_lbsqft,
                     double                      aStatPress_lbssqft,
                     double                      aSpeed_fps,
                     double                      aMach,
                     double                      aAlpha_rad,
                     double                      aBeta_rad,
                     P6DofForceAndMomentsObject& aForceAndMoment,
                     double&                     aFuelBurnRate_pps,
                     double&                     aFuelBurned_lbs) override;

   // SetFuelTank sets the fuel source for the engine
   void SetFuelTank(const std::string& aFuelTankName);

   // GetFuelTank returns the current fuel source
   P6DofFuelTank* GetFuelTank() const;

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

   // Liquid-propellant rockets do not have mass properties (the engine mass
   // is included in the overall empty mass of the P6DofVehicle instead).
   void CalculateCurrentMassProperties() override {}

   // The SetThrottleLeverPosition function should be called prior to calling
   // UpdateThrust so that the throttle will be properly set.
   void SetThrottlePosition(double aThrottleLeverPosition) override;

   // This returns the current throttle position
   double GetThrottlePosition() const override;

   // Start the engine.
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) override;

   void Shutdown(int64_t aTerminateTime_nanosec = 0) override;

protected:
   P6DofRocketLiquidPropellantObject(const P6DofRocketLiquidPropellantObject& aSrc);

   // This CalculateThrust function is called internally by either
   // CalculateThrust or UpdateThrust to calculate the engine's
   // forces and moments. The aUpdateData flag determines whether
   // or not to change the state of the engine.
   void CalculateThrust(double                      aDeltaT_sec,
                        double                      aAlt_ft,
                        double                      aDynPress_lbsqft,
                        double                      aStatPress_lbssqft,
                        double                      aSpeed_fps,
                        double                      aMach,
                        double                      aAlpha_rad,
                        double                      aBeta_rad,
                        P6DofForceAndMomentsObject& aForceAndMoment,
                        double&                     aFuelBurnRate_pps,
                        double&                     aFuelBurned_lbs,
                        bool                        aUpdateData);

   bool MayProduceSmokeTrail() const override { return mGenerateSmokeTrail; }

   double mThrottleLeverPosition;
   double mLastThrottleLeverPosition;

   // Fuel tank information
   P6DofFuelTank* mCurrentFuelTank;

   // Normalized thrust as a function of altitude
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mNormalizedThrustAlt;

   // Specific impulse as a function of altitude
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mIspAlt;

   // Throttle versus time. This is an optional table to drive the
   // throttle. Otherwise, conventional throttle control is used.
   UtCloneablePtr<UtTable::Curve> mThrottleVersusTime;

   // Max rated thrust at sea level or vacuum.
   // One of these must be specified.
   double mMaxThrustSeaLevel;
   // double                      mMaxThrustVacuum;

   // Max and min throttle range. These must be specified, or a
   // default of 1.0 and 0.0 will be used.
   // Note: Max throttle may be greater than 1.0 (100%).
   double mMaxThrottle;
   double mMinThrottle;

   // Propellant mass flow at sea level or vacuum
   // One of these will be calculated
   double mMaxMassFlowSeaLevel;
   double mMaxMassFlowVacuum;

   double mEngineThrustPercent;

   bool   mIsBurning;
   double mBurnTime_Sec;

   double mNormalizedSpinUp_per_sec;
   double mNormalizedSpinDown_per_sec;

   double mCurrentFuelBurnRate_pph;

   ThrustControlInputValueData* mThrottleSettingYaw;
   ThrustControlInputValueData* mThrottleSettingPitch;

   bool mGenerateSmokeTrail;
};

#endif
