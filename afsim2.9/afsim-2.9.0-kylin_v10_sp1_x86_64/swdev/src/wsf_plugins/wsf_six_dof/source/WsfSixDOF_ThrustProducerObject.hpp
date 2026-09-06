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

#ifndef WSFSIXDOFTHRUSTPRODUCEROBJECT_HPP
#define WSFSIXDOFTHRUSTPRODUCEROBJECT_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "UtLog.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"
#include "WsfSixDOF_Engine.hpp"
#include "WsfSixDOF_Object.hpp"

namespace wsf
{
namespace six_dof
{
class MassProperties;
class PropulsionSystem;

class WSF_SIX_DOF_EXPORT ThrustProducerObject : public Object
{
public:
   // This is used to create a type of a ThrustProducerObject
   ThrustProducerObject()        = default;
   ~ThrustProducerObject()       = default;
   ThrustProducerObject& operator=(const ThrustProducerObject& other) = delete;

   virtual ThrustProducerObject* Clone() const = 0;

   // This sets the propulsion system to which this thrust producer belongs
   void SetParentPropulsionSystem(PropulsionSystem* aParent) { mParentPropulsionSystemPtr = aParent; }

   // This retrieves the propulsion system to which this thrust producer belongs
   PropulsionSystem* GetParentPropulsionSystem() { return mParentPropulsionSystemPtr; }

   // This sets the engine to which this thrust producer belongs
   void SetEngine(std::unique_ptr<Engine> aEngine) { mEnginePtr = aEngine.release(); }

   // This retrieves the engine to which this thrust producer belongs
   const Engine* GetEngine() const { return mEnginePtr; }

   virtual bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager);

   bool Initialize(int64_t aSimTime_nanosec) { return mEnginePtr->Initialize(aSimTime_nanosec); }

   std::string GetBaseName() { return mBaseName; }

   // Start the engine.
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) { mEnginePtr->Ignite(aIgniteTimeInFrame_nanosec); }

   void Shutdown(int64_t aTerminateTime_nanosec = 0) { mEnginePtr->Shutdown(aTerminateTime_nanosec); };

   // .....................................................................................................

   // Set reference location and attitude (YPR) relative to parent
   virtual void SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft, const UtVec3dX& aThrustYPR_rad) = 0;

   // This provides the maximum potential thrust available, if full throttle(s) is applied.
   double GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                        double aDynPress_lbsqft,
                                        double aStatPress_lbssqft,
                                        double aSpeed_fps,
                                        double aMach,
                                        double aAlpha_rad,
                                        double aBeta_rad);

   // This provides the minimum potential thrust available
   double GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                        double aDynPress_lbsqft,
                                        double aStatPress_lbssqft,
                                        double aSpeed_fps,
                                        double aMach,
                                        double aAlpha_rad,
                                        double aBeta_rad);

   double GetInoperatingDragArea_ft2();

   virtual double GetThrottleMilSetting() = 0;
   virtual double GetThrottleAbSetting()  = 0;

   // Location of thrust relative to parent
   void GetThrustLocation(UtVec3dX& aThrustLocation_ft) const { aThrustLocation_ft = mThrustLocation_ft; };

   // Location of thrust producer relative to parent
   void GetReferenceLocation(UtVec3dX& aReferenceLocation_ft) const { aReferenceLocation_ft = mReferenceLocation_ft; };

   // Attitude of thrust relative to parent
   void GetThrustInstalledAnglesYPR(UtVec3dX& aThrustYPR_rad) const { aThrustYPR_rad = mThrustInstalledAnglesYPR_rad; };

   // .....................................................................................................

   // This returns the current throttle position
   double GetThrottlePosition() const { return mEnginePtr->GetThrottlePosition(); }

   // The SetThrottleLeverPosition function should be called prior to calling
   // UpdateThrust so that the throttle will be properly set.
   void SetThrottlePosition(double aThrottleLeverPosition)
   {
      return mEnginePtr->SetThrottlePosition(aThrottleLeverPosition);
   }

   // Enable (disable) thrust vectoring
   void SetThrustVectoringEnabled(bool aThrustVectoringEnabled) { mThrustVectoringEnabled = aThrustVectoringEnabled; }

   // Returns whether thrust vectoring is enabled
   bool ThrustVectoringEnabled() const { return mThrustVectoringEnabled; }

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   double GetFuelBurnRate_pph() const { return mEnginePtr->GetFuelBurnRate_pph(); }

   // This returns the last 'updated' thrust value magnitude
   double GetThrust_lbs() const { return mEnginePtr->GetThrust_lbs(); }

   // This returns the last 'updated' thrust in the forward direction
   double GetForwardThrust_lbs() const;

   // Direction of thrust relative to parent
   virtual void GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const = 0;

   // SetFuelTank sets the fuel source for the engine and returns true if successful
   virtual bool SetFuelTank(const std::string& aFuelTankName);

   // .....................................................................................................

   // Appearance Parameters
   // These functions use the mEngineOperating, mEngineSmoking, mAfterburnerOn, mContrailing,
   // and mProducingSmokeTrail parameters. For correct operation, derived engine classes
   // should set those paramaters, as appropriate, in their overridden CalculateThrust()
   // function, according to the specific characteristics of the type of engine.

   // Returns true if the engine is operating and burning fuel
   bool EngineOperating() const { return mEnginePtr->EngineOperating(); }

   // Returns true if the engine will/may produce a engine operating appearance.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineOperating()
   bool MayHaveEngineOperating() const { return mEnginePtr->MayHaveEngineOperating(); }

   // Returns true if the engine will/may produce engine smoke.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineSmoking()
   bool MayProduceEngineSmoke() const { return mEnginePtr->MayProduceEngineSmoke(); }

   // Returns true if the engine will/may produce a contrail.
   // If this can not be determined with the knowledge available, return true.
   bool MayContrail() const { return mEnginePtr->MayContrail(); }

   // Returns true if the engine is producing a smoke trail. Note that
   // this is a dense trail of smoke, typically from a solid propellant
   // rocket, as opposed to a limited smoke from an engine (see EngineSmoking).
   bool ProducingSmokeTrail() const { return mEnginePtr->ProducingSmokeTrail(); }

   // Returns true if the engine will/may produce a smoke trail.
   // see also ProducingSmokeTrail()
   // If this can not be determined with the knowledge available, return true.
   bool MayProduceSmokeTrail() const { return mEnginePtr->MayProduceSmokeTrail(); }

   // Returns true if a flame is emanating from the engine, such as that
   // produced by an afterburner or a rocket that is producing a flame.
   // see also AfterburnerIsPresent(), for if they function may turn on
   bool AfterburnerOn() const { return mEnginePtr->AfterburnerOn(); }

   // Returns true if the engine is producing a contrail
   bool Contrailing() const { return mEnginePtr->Contrailing(); }

   // Returns true if the engine is producing some smoke. Note that
   // this is limited smoke from an engine and is different from a
   // smoke trail effect, which is a dense trail of smoke, typically
   // from a solid propellant rocket.
   bool EngineSmoking() const { return mEnginePtr->EngineSmoking(); }

   // If true, will cause an engine to smoke. When false, it stops smoking, unless the
   // engine's nominal behavior is to smoke.
   // Note that this is for engine smoke, not a smoke trail.
   void MakeEngineSmoke(bool aSmoking) { mEnginePtr->MakeEngineSmoke(aSmoking); }

   // Returns true if an afterburner is present on any engine
   virtual bool AfterburnerIsPresent() const { return mEnginePtr->AfterburnerIsPresent(); }

protected:
   ThrustProducerObject(const ThrustProducerObject& aSrc);

   // For deep copies within the class
   virtual void DeriveFrom(const ThrustProducerObject* aSrc);

   // Common function for getting type name and base-type name, and deriving from base types as requested
   bool ProcessInputType(UtInput& aInput, TypeManager* aTypeManager);

   // Name of the type this thrust producer was derived from
   std::string mBaseName;

   // Parent object (propulsion system)
   PropulsionSystem* mParentPropulsionSystemPtr = nullptr;

   // Object that calculates the thrust force
   UtCloneablePtr<Engine> mEnginePtr{nullptr};

   // Nominal thrust direction vector (does not include vectoring/reversing)
   UtVec3dX mNominalThrustVectorDirection = UtVec3dX::cUNIT_FORWARD;

   // Rotation of thrust relative to parent
   UtVec3dX mThrustInstalledAnglesYPR_rad;

   // Location of the reference point (typically center of engine)
   UtVec3dX mReferenceLocation_ft;

   // Location of thrust relative to parent
   UtVec3dX mThrustLocation_ft;

   // Location of thrust relative to thrust producer reference point along the x-axis.
   // A positive value is forward of the engine, negative is aft.
   double mThrustOffset_ft = 0.0;

   // A reference area that can produce drag when the engine is not operating; 1.0 by default
   double mInOpRefArea_ft2 = 1.0;

   bool mThrustVectoringEnabled = false;
};
} // namespace six_dof
} // namespace wsf

#endif
