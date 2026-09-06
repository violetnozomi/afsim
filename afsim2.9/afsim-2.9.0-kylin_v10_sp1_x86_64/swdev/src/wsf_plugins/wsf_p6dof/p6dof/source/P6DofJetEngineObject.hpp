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

#ifndef P6DOFJETENGINEOBJECT_H
#define P6DOFJETENGINEOBJECT_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofThrustProducerObject.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofFuelTank;
class P6DofScenario;
class UtInput;

class P6DOF_EXPORT P6DofJetEngineObject : public P6DofThrustProducerObject
{
public:
   // This is used to create a type of a P6DofThrustProducerObject
   explicit P6DofJetEngineObject(P6DofScenario* aScenario);

   ~P6DofJetEngineObject() override;

   P6DofJetEngineObject& operator=(const P6DofJetEngineObject& other) = delete;

   // Each engine class returns its own unique class type
   std::string GetClassType() override { return "Jet"; }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   P6DofJetEngineObject* Clone() const;

   void DeriveFrom(const P6DofJetEngineObject* aSrc);

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

   // By default, a jet engine will have "injectFuel" set to true.
   // Setting it to false, will "shutdown" the engine, which will spool down
   // its thrust. Setting it to true will "startup" the engine, which will
   // spin up to idle thrust. Any change to the InjectFuel function should be
   // called prior to calling UpdateThrust.
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

   // Jet engines do not have mass properties (the engine mass is
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
   P6DofJetEngineObject(const P6DofJetEngineObject& aSrc);

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

   bool DetermineIfAfterburnerIsPresent();

   double mThrottleLeverPosition;

   // Fuel tank information
   P6DofFuelTank* mCurrentFuelTank;

   // Simple Engine tabular data
   UtCloneablePtr<UtTable::Curve> mIdleThrustTable;
   UtCloneablePtr<UtTable::Curve> mMilThrustTable;
   UtCloneablePtr<UtTable::Curve> mABThrustTable;

   // Improved Engine tabular data (mach/alt)
   UtCloneablePtr<UtTable::Table> mIdleThrustMachAltTable;
   UtCloneablePtr<UtTable::Table> mMilThrustMachAltTable;
   UtCloneablePtr<UtTable::Table> mABThrustMachAltTable;

   // Improved Engine tabular data (alt/mach)
   UtCloneablePtr<UtTable::Table> mIdleThrustAltMachTable;
   UtCloneablePtr<UtTable::Table> mMilThrustAltMachTable;
   UtCloneablePtr<UtTable::Table> mABThrustAltMachTable;

   double mTSFC_Idle_pph;
   double mTSFC_Mil_pph;
   double mTSFC_AB_pph;

   double mRatedThrustIdle_lbs;
   double mRatedThrustMil_lbs;
   double mRatedThrustAB_lbs;

   double mEffectiveTSFC_Idle_pps;
   double mEffectiveTSFC_Mil_pps;
   double mEffectiveTSFC_AB_pps;

   double mLastThrottleLeverPosition;
   double mEnginePercentRPM;
   double mNozzlePosition;

   double mSpinUpMil_per_sec;
   double mSpinDownMil_per_sec;
   double mSpinUpAB_per_sec;
   double mSpinDownAB_per_sec;

   UtCloneablePtr<UtTable::Curve> mSpinUpMilTable;
   UtCloneablePtr<UtTable::Curve> mSpinDownMilTable;
   UtCloneablePtr<UtTable::Curve> mSpinUpABTable;
   UtCloneablePtr<UtTable::Curve> mSpinDownABTable;

   double mCurrentFuelBurnRate_pph;

   ThrustControlInputValueData* mThrottleSettingMIL;
   ThrustControlInputValueData* mThrottleSettingAB;
   ThrustControlInputValueData* mThrottleSettingReverser;
   ThrustControlInputValueData* mThrottleSettingYaw;
   ThrustControlInputValueData* mThrottleSettingPitch;

   bool mThrottleLeverPositionSet;

   bool mInjectFuel;

   bool   mEngineMaySmoke;
   double mEngineSmokesAboveLevel;
};

#endif
