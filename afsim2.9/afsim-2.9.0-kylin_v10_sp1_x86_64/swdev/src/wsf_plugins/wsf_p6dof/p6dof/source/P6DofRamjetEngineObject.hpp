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

#ifndef P6DOFRAMJETENGINEOBJECT_H
#define P6DOFRAMJETENGINEOBJECT_H

#include "p6dof_export.h"

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

#include "P6DofThrustProducerObject.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofFuelTank;
class P6dofScenario;
class UtInput;

class P6DOF_EXPORT P6DofRamjetEngineObject : public P6DofThrustProducerObject
{
public:
   // This is used to create a type of a P6DofThrustProducerObject
   explicit P6DofRamjetEngineObject(P6DofScenario* aScenario);

   ~P6DofRamjetEngineObject() override;

   P6DofRamjetEngineObject& operator=(const P6DofRamjetEngineObject& other) = delete;

   // Each engine class returns its own unique class type
   std::string GetClassType() override { return "Ramjet"; }

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   P6DofRamjetEngineObject* Clone() const;

   void DeriveFrom(const P6DofRamjetEngineObject* aSrc);

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

   // The InjectFuel function should be called prior to calling
   // UpdateThrust so that the fueling will be properly set.
   void InjectFuel(bool aInjectFuel) { mInjectFuel = aInjectFuel; }

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

   // Ramjet engines do not have mass properties (the engine mass is
   // included in the overall empty mass of the P6DofVehicle instead).
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
   P6DofRamjetEngineObject(const P6DofRamjetEngineObject& aSrc);

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

   // This is called in CalculateThrust to determine the thrust multiplier, which
   // can range from 0 to 1 based on throttle lever position and the inject fuel
   // condition. It also handles fuel injection effects and may modify the fuel
   // burn request and the current thrust. The aDeadEngine argument denotes a
   // dead engine (unable to produce thrust). The function returns true if the
   // engine is dead (not producing thrust) or false if thrust is being produced.
   bool CalcThrustMultiplier(bool aDeadEngine, double& aFuelBurnRequest_lbs, double& aThrust_lbs);

   // Fuel tank information
   P6DofFuelTank*                  mCurrentFuelTank                    = nullptr;
   std::shared_ptr<UtTable::Table> mThrustAltMachTable_lbs             = nullptr;
   std::shared_ptr<UtTable::Table> mTsfcAltMachTable_pph               = nullptr;
   double                          mCurrentFuelBurnRate_pph            = 0.0;
   ThrustControlInputValueData*    mThrottleSettingYaw                 = nullptr;
   ThrustControlInputValueData*    mThrottleSettingPitch               = nullptr;
   bool                            mAfterburnerAppearanceWhenOperating = false;

   // This determines whether or not fuel will be injected (and thrust will be produced)
   bool mInjectFuel = false;

   // When true, this will always inject fuel, regardless of throttle command
   bool mLatchFuelInjection = false;

   // This is set to true once mInjectFuel is activated
   bool mInjectFuelTriggered = false;

   // When true, this will allow a proportional throttle control, versus the default on/off control
   bool mUseProportionalThrottle = false;

   // This is the minimum thrust level that may be used for proportional throttle control
   double mMinProportionalThrust = 0.0;

   // This is the current throttle level position. With default conditions, it will be either
   // 0 or 1, but with proportional control, it will be between mMinThrottleMultiplier and 1.
   double mThrottleLeverPosition = 0.0;

   // This is the overall thrust multiplier, which can range from 0 to 1 based on
   // throttle lever position and the inject fuel condition.
   double mOverallThrustMultiplier = 0.0;
};

#endif
