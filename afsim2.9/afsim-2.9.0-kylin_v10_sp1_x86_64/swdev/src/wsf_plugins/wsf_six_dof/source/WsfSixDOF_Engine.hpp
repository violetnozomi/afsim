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

#ifndef WSFSIXDOFENGINE_HPP
#define WSFSIXDOFENGINE_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "UtLog.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

namespace wsf
{
namespace six_dof
{
class FuelTank;
class MassProperties;
class ThrustProducerObject;
class TypeManager;

class WSF_SIX_DOF_EXPORT Engine
{
public:
   // This is used to create a type of a Engine
   explicit Engine(ThrustProducerObject* aParentObject);
   virtual ~Engine() = default;
   Engine& operator=(const Engine& other) = delete;

   virtual Engine* Clone() const = 0;

   virtual bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) = 0;
   virtual bool Initialize(int64_t aSimTime_nanosec)                     = 0;

   virtual void DeriveFrom(const Engine* aSrc);

   ThrustProducerObject* GetParentThrustProducer() { return mParentThrustProducerPtr; }
   void                  SetParentThrustProducer(ThrustProducerObject* aThrustProducerPtr)
   {
      mParentThrustProducerPtr = aThrustProducerPtr;
   }

   // This returns the last 'updated' thrust value magnitude
   double GetThrust_lbs() const { return mCurrentThrust_lbs; }

   virtual double GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                double aDynPress_lbsqft,
                                                double aStatPress_lbssqft,
                                                double aSpeed_fps,
                                                double aMach,
                                                double aAlpha_rad,
                                                double aBeta_rad) = 0;

   virtual double GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                                double aDynPress_lbsqft,
                                                double aStatPress_lbssqft,
                                                double aSpeed_fps,
                                                double aMach,
                                                double aAlpha_rad,
                                                double aBeta_rad) = 0;

   // The CalculateThrust function is called to calculate the engine's
   // forces and moments. It does not change the state of the engine.
   // To change the state, call UpdateThrust instead. This version
   // uses the thrust producer's internal throttle values.
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
                        double& aFuelBurned_lbs);

   // The UpdateThrust function is called to calculate the engine's
   // forces and moments and update the state of the engine.
   // To perform F&M calculations without changing the state, call the
   // CalculateThrust function instead.
   void UpdateThrust(double  aDeltaT_sec,
                     double  aAlt_ft,
                     double  aDynPress_lbsqft,
                     double  aStatPress_lbssqft,
                     double  aSpeed_fps,
                     double  aMach,
                     double  aAlpha_rad,
                     double  aBeta_rad,
                     double& aForce_lbs,
                     double& aFuelBurnRate_pps,
                     double& aFuelBurned_lbs);

   // CheckCurrentFuelTank ensures the engine has a valid fuel flow.
   // Returns false and sets the tank pointer to null if it does not.
   bool CheckCurrentFuelTank();

   // SetFuelTank sets the fuel source for the engine. Returns false with an error by default.
   virtual bool SetFuelTank(const std::string& aFuelTankName);

   // GetFuelTank returns a pointer to the fuel tank, if one is assigned to the engine
   const FuelTank* GetFuelTank() const { return mCurrentFuelTank; }

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   virtual double GetFuelBurnRate_pph() const = 0;

   virtual double GetThrottlePosition() const = 0;

   virtual void SetThrottlePosition(double aThrottleLeverPosition) = 0;

   // Start the engine.
   virtual void Ignite(int64_t aIgniteTimeInFrame_nanosec) = 0;

   virtual void Shutdown(int64_t aTerminateTime_nanosec = 0) = 0;

   // Returns true if the engine is operating and burning fuel
   bool EngineOperating() const { return mEngineOperating; }

   // Returns true if the engine will/may produce a engine operating appearance.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineOperating()
   virtual bool MayHaveEngineOperating() const { return true; }

   // Returns true if the engine will/may produce engine smoke.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineSmoking()
   virtual bool MayProduceEngineSmoke() const { return true; }

   // Returns true if the engine will/may produce a contrail.
   // If this can not be determined with the knowledge available, return true.
   virtual bool MayContrail() const { return true; }

   // Returns true if the engine is producing a smoke trail. Note that
   // this is a dense trail of smoke, typically from a solid propellant
   // rocket, as opposed to a limited smoke from an engine (see EngineSmoking).
   bool ProducingSmokeTrail() const { return mProducingSmokeTrail; }

   // Returns true if the engine will/may produce a smoke trail.
   // see also ProducingSmokeTrail()
   // If this can not be determined with the knowledge available, return true.
   virtual bool MayProduceSmokeTrail() const { return false; }

   // Returns true if a flame is emanating from the engine, such as that
   // produced by an afterburner or a rocket that is producing a flame.
   // see also AfterburnerIsPresent(), for if they function may turn on
   bool AfterburnerOn() const { return mAfterburnerOn; }

   // Returns true if the engine is producing a contrail
   bool Contrailing() const { return mContrailing; }

   // Returns true if the engine is producing some smoke. Note that
   // this is limited smoke from an engine and is different from a
   // smoke trail effect, which is a dense trail of smoke, typically
   // from a solid propellant rocket.
   bool EngineSmoking() const { return mEngineSmoking; }

   // If true, will cause an engine to smoke. When false, it stops smoking, unless the
   // engine's nominal behavior is to smoke.
   // Note that this is for engine smoke, not a smoke trail.
   void MakeEngineSmoke(bool aSmoking) { mEngineDamageSmokeActivated = aSmoking; }

   // Returns true if an afterburner is present on any engine
   virtual bool AfterburnerIsPresent() const { return mAfterburnerPresent; }

protected:
   explicit Engine(const Engine& aSrc);

   virtual void CalculateThrust(double  aDeltaT_sec,
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
                                bool    aUpdateData) = 0;

   ThrustProducerObject* mParentThrustProducerPtr = nullptr;

   // A unique name for each subclass of engine
   std::string mTypeOfEngine;

   // Fuel tank or propellant information. Some engine types may need to use a particular fuel tank,
   // so SetFuelTank may not be available
   FuelTank* mCurrentFuelTank = nullptr;

   double mCurrentThrust_lbs = 0.0;

   // This is true if a shutdown is in progress. As the shutdown
   // process is handled, this should be set to false.
   bool mShutdownInProgress = false;

   // This is used during a shut down to indicate how far into
   // the current calculation frame the shut down occurs.
   int64_t mShutdownFraction_nanosec = 0;

   // This is used during an ignition to indicate how far into
   // the current calculation frame the ignition occurs.
   int64_t mIgniteTimeInFrame_nanosec = 0;

   bool mEngineOperating            = false;
   bool mContrailing                = false;
   bool mAfterburnerPresent         = false;
   bool mAfterburnerOn              = false;
   bool mProducingSmokeTrail        = false;
   bool mEngineDamageSmokeActivated = false;
   bool mEngineSmoking              = false;
};
} // namespace six_dof
} // namespace wsf

#endif
