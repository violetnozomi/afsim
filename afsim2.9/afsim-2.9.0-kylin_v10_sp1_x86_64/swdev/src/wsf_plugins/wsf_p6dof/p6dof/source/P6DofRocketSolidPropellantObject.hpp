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

#ifndef P6DOFROCKETSOLIDPROPELLANTOBJECT_H
#define P6DOFROCKETSOLIDPROPELLANTOBJECT_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofThrustProducerObject.hpp"
#include "UtCloneablePtr.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofScenario;
class UtInput;

class P6DOF_EXPORT P6DofRocketSolidPropellantObject : public P6DofThrustProducerObject
{
public:
   // This is used to create a type of a P6DofThrustProducerObject
   explicit P6DofRocketSolidPropellantObject(P6DofScenario* aScenario);

   ~P6DofRocketSolidPropellantObject() override;

   P6DofRocketSolidPropellantObject& operator=(const P6DofRocketSolidPropellantObject& other) = delete;

   // Each engine class returns its own unique class type
   std::string GetClassType() override { return "SolidRocket"; }

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(int64_t aSimTime_nanosec) override;

   P6DofRocketSolidPropellantObject* Clone() const;

   void DeriveFrom(const P6DofRocketSolidPropellantObject* aSrc);

   void SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls) override;
   void SetControlInputValues(const P6DofFlightControlSystem* aFlightControls) override;

   // The IgniteNow function is called to 'start' the rocket engine.
   void IgniteNow() { mIsBurning = true; }

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

   // GetPercentMaxThrust returns the last 'updated' engine thrust
   // in terms of percent of maximum thrust
   double GetPercentMaxThrust() const;

   // GetCurrentPropellant_lbs returns the amount of propellant remaining
   double GetCurrentPropellant_lbs() const;

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   double GetFuelBurnRate_pph() const override;

   // GetMaxPropellant_lbs returns the max propellant weight in lbs
   double GetMaxPropellant_lbs() const;

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

   // Solid-propellant rockets have mass properties. These
   // mass properties include the propellant mass.
   void CalculateCurrentMassProperties() override;

   // The SetThrottleLeverPosition function should be called prior to calling
   // UpdateThrust so that the throttle will be properly set.
   void SetThrottlePosition(double aThrottleLeverPosition) override;

   // This returns the current throttle position
   double GetThrottlePosition() const override;

   // The Ignite function is called to 'start' the rocket engine.
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) override;

   void Shutdown(int64_t aTerminateTime_nanosec = 0) override;

protected:
   P6DofRocketSolidPropellantObject(const P6DofRocketSolidPropellantObject& aSrc);

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

   double Thrust_lbs(double aBurnTime_Sec, double aAlt_ft, double& aMassFlow_pps);

   bool MayProduceSmokeTrail() const override { return mGenerateSmokeTrail; }

   // Thrust versus time at sea level or vacuum.
   // One of these must be specified.
   UtCloneablePtr<UtTable::Curve> mThrustSeaLevelVersusTime;
   UtCloneablePtr<UtTable::Curve> mThrustVacuumVersusTime;

   // Specific impulse as a function of altitude.
   // This must be specified or a default table will be used
   UtCloneablePtr<UtTable::Curve> mIspAlt;

   double mIspSeaLevel;
   double mIspVacuum;

   double mRatedThrust_lbs; // Used to determine percent thrust
   double mLastThrust_lbs;

   double mPropellantMass_lbs;
   double mMaxPropellantMass_lbs;

   bool   mIsBurning;
   double mBurnTime_sec;

   double mCurrentFuelBurnRate_pph;

   // This is the location of the center of mass of the propellant,
   // relative to the parent object
   UtVec3dX mPropellantCmLocation_ft;

   ThrustControlInputValueData* mThrottleSettingYaw;
   ThrustControlInputValueData* mThrottleSettingPitch;

   bool mGenerateSmokeTrail;
};

#endif
