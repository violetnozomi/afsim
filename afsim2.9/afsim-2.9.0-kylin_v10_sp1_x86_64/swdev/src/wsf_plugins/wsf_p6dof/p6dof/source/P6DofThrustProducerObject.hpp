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

#ifndef P6DOFTHRUSTPRODUCEROBJECT_H
#define P6DOFTHRUSTPRODUCEROBJECT_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <vector>

#include "P6DofFlightControlSystem.hpp"
#include "P6DofForceAndMomentsObject.hpp"
#include "P6DofMassProperties.hpp"
#include "P6DofObject.hpp"
#include "UtTable.hpp"
#include "UtVec3dX.hpp"

class P6DofFuelTank;
class P6DofPropulsionSystem;
class P6DofScenario;
class UtInput;

class P6DOF_EXPORT P6DofThrustProducerObject : public P6DofObject
{
public:
   // This is used to create a type of a P6DofThrustProducerObject
   explicit P6DofThrustProducerObject(P6DofScenario* aScenario);
   ~P6DofThrustProducerObject() override;
   P6DofThrustProducerObject& operator=(const P6DofThrustProducerObject& other) = delete;

   // ProcessInput is called to read input data from a UtInput stream, which
   // is typically an 'input file'.
   virtual bool ProcessInput(UtInput& aInput)        = 0;
   virtual bool Initialize(int64_t aSimTime_nanosec) = 0;

   // This sets the propulsion system to which this thrust producer belongs
   void SetParentPropulsionSystem(P6DofPropulsionSystem* aParent);

   // ThrustProducerObject-derived classes should include CalculateThrust and
   // UpdateThrust functions. CalculateThrust performs calculations but does
   // not update 'state' while UpdateThrust performs calculations and also
   // updates the state.

   // The CalculateThrust function is called to calculate the engine's
   // forces and moments. It does not change the state of the engine.
   // To change the state, call UpdateThrust instead. This version
   // uses the thrust producer's internal throttle values.
   virtual void CalculateThrust(double                      aDeltaT_sec,
                                double                      aAlt_ft,
                                double                      aDynPress_lbsqft,
                                double                      aStatPress_lbssqft,
                                double                      aSpeed_fps,
                                double                      aMach,
                                double                      aAlpha_rad,
                                double                      aBeta_rad,
                                P6DofForceAndMomentsObject& aForceAndMoment,
                                double&                     aFuelBurnRate_pps,
                                double&                     aFuelBurned_lbs) = 0;

   // The UpdateThrust function is called to calculate the engine's
   // forces and moments and update the state of the engine.
   // To perform F&M calculations without changing the state, call the
   // CalculateThrust function instead.
   virtual void UpdateThrust(double                      aDeltaT_sec,
                             double                      aAlt_ft,
                             double                      aDynPress_lbsqft,
                             double                      aStatPress_lbssqft,
                             double                      aSpeed_fps,
                             double                      aMach,
                             double                      aAlpha_rad,
                             double                      aBeta_rad,
                             P6DofForceAndMomentsObject& aForceAndMoment,
                             double&                     aFuelBurnRate_pps,
                             double&                     aFuelBurned_lbs) = 0;

   virtual std::string GetClassType() = 0;

   // This returns the last 'updated' thrust value magnitude
   double GetThrust_lbs() const;

   // This returns the last 'updated' thrust in the forward direction
   double GetForwardThrust_lbs() const;

   // Set reference location and attitude (YPR) relative to parent
   void SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft, UtVec3dX& aThrustYPR_rad);

   // Enable (disable) thrust vectoring
   void SetThrustVectoringEnabled(bool aThrustVectoringEnabled) { mThrustVectoringEnabled = aThrustVectoringEnabled; }

   // Returns whether thrust vectoring is enabled
   bool ThrustVectoringEnabled() const { return mThrustVectoringEnabled; }

   // Attitude of thrust vectoring relative to engine
   void SetThrustVectoringYaw_deg(double aThrustVectoringYaw_deg);

   // Attitude of thrust vectoring relative to engine
   void SetThrustVectoringPitch_deg(double aThrustVectoringPitch_deg);

   // Attitude of thrust vectoring relative to engine
   double GetThrustVectoringYaw_deg() const;

   // Attitude of thrust vectoring relative to engine
   double GetThrustVectoringPitch_deg() const;

   // Enable (disable) thrust reverser
   void SetThrustReverserEnabled(bool aThrustReverserEnabled) { mThrustReverserEnabled = aThrustReverserEnabled; }

   // Returns whether thrust reverser is enabled
   bool ThrustReverserEnabled() const { return mThrustReverserEnabled; }

   // Sets thrust reverser: 0=Off, 1=FullReverse
   void SetThrustReverserSetting(double aThrustReverserSetting);

   // Gets thrust reverser setting
   double GetThrustReverserSetting() const { return mThrustReverserSetting; }

   // Location of thrust relative to parent
   void GetThrustLocation(UtVec3dX& aThrustLocation_ft) const;

   // Attitude of thrust relative to parent
   void GetThrustInstalledAnglesYPR(UtVec3dX& aThrustYPR_rad) const;

   // Direction of thrust relative to parent
   void GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const;

   // CalcCurrentThrustVectors provides the current thrust unit vector and thrust location
   // in body coordinates including any thrust vectoring and reversing. The
   // aThrustFactor indicates the magnitude factor for the thrust (positive is forward thrust
   // and negative is reverse thrust, ranging from:
   // 1 = full forward thrust to -0.5 = full reverse thrust
   void CalcCurrentThrustVectors();

   // GetThrustForceAndMoment returns a P6DofForceAndMomentsObject
   // based on the last 'updated' thrust value
   const P6DofForceAndMomentsObject& GetThrustForceAndAndMoment() const;

   // GetFuelBurnRate_pph returns the current fuel/propellant burn rate in lbs/hr
   virtual double GetFuelBurnRate_pph() const = 0;

   // .....................................................................................................

   // This provides the maximum potential thrust available, if full throttle(s) is applied.
   virtual double GetMaximumPotentialThrust_lbs(double aAlt_ft,
                                                double aDynPress_lbsqft,
                                                double aStatPress_lbssqft,
                                                double aSpeed_fps,
                                                double aMach,
                                                double aAlpha_rad,
                                                double aBeta_rad) = 0;

   // This provides the minimum potential thrust available
   virtual double GetMinimumPotentialThrust_lbs(double aAlt_ft,
                                                double aDynPress_lbsqft,
                                                double aStatPress_lbssqft,
                                                double aSpeed_fps,
                                                double aMach,
                                                double aAlpha_rad,
                                                double aBeta_rad) = 0;

   // .....................................................................................................

   // This returns a pointer to the mass properties or null, if none exist
   P6DofMassProperties* GetMassProperties() const { return mMassPropertiesPtr; }

   // This abstract function calculates the current mass properties for the
   // thrust producer, including any integral fuel/propellant that may exist.
   virtual void CalculateCurrentMassProperties() = 0;

   // .....................................................................................................

   // The SetThrottleLeverPosition function should be called prior to calling
   // UpdateThrust so that the throttle will be properly set.
   virtual void SetThrottlePosition(double aThrottleLeverPosition) = 0;

   // This returns the current throttle position
   virtual double GetThrottlePosition() const = 0;

   // Start the engine.
   virtual void Ignite(int64_t aIgniteTimeInFrame_nanosec) = 0;

   virtual void Shutdown(int64_t aTerminateTime_nanosec = 0) = 0;

   virtual bool AfterburnerIsPresent() const { return mAfterburnerPresent; }

   // .....................................................................................................

   // Appearance Parameters
   // These functions use the mEngineOperating, mEngineSmoking, mAfterburnerOn, mContrailing,
   // and mProducingSmokeTrail parameters. For correct operation, derived engine classes
   // should set those paramaters, as appropriate, in their overridden CalculateThrust()
   // function, according to the specific characteristics of the type of engine.

   // Returns true if the engine is operating and burning fuel
   bool EngineOperating() const { return mEngineOperating; }

   // Returns true if the engine will/may produce a engine operating appearance.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineOperating()
   virtual bool MayHaveEngineOperating() const { return true; }

   // Returns true if the engine is producing some smoke. Note that
   // this is limited smoke from an engine and is different from a
   // smoke trail effect, which is a dense trail of smoke, typically
   // from a solid propellant rocket.
   bool EngineSmoking() const { return mEngineSmoking; }

   // Returns true if the engine will/may produce engine smoke.
   // If this can not be determined with the knowledge available, return true.
   // see also EngineSmoking()
   virtual bool MayProduceEngineSmoke() const { return true; }

   // Returns true if a flame is emanating from the engine, such as that
   // produced by an afterburner or a rocket that is producing a flame.
   // see also AfterburnerIsPresent(), for if they function may turn on
   bool AfterburnerOn() const { return mAfterburnerOn; }

   // Returns true if the engine is producing a contrail
   bool Contrailing() const { return mContrailing; }

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


   // If true, will cause an engine to smoke. When false, it stops smoking, unless the
   // engine's nominal behavior is to smoke.
   // Note that this is for engine smoke, not a smoke trail.
   void MakeEngineSmoke(bool aSmoking);

   // .....................................................................................................

   struct ThrustControlInputValueData
   {
      std::string name;
      size_t      handle;
      double      value;
   };

   struct ThrustBooleanControlInputData
   {
      std::string name;
      size_t      handle;
      bool        value;
   };

   virtual void SetControlInputHandles(const P6DofFlightControlSystem* aFlightControls) = 0;
   virtual void SetControlInputValues(const P6DofFlightControlSystem* aFlightControls)  = 0;

   static void SetControlInputHandle(const P6DofFlightControlSystem* aFlightControls,
                                     ThrustControlInputValueData*    aControlInput);
   static void SetControlInputValue(const P6DofFlightControlSystem* aFlightControls,
                                    ThrustControlInputValueData*    aControlInput);

protected:
   P6DofThrustProducerObject(const P6DofThrustProducerObject& aSrc);

   void DeriveFrom(const P6DofThrustProducerObject* aSrc);

   void SetMassProperties(P6DofMassProperties* aMassProperties) { mMassPropertiesPtr = aMassProperties; }

   // This will return the thrust vector using the thrust magnitude and current direction
   UtVec3dX CalcThrustVectorFromThrust(double aThrust_lbs);

   // This calculates various thrust F&M vectors.
   // It is used within the CalculateThrust function of thrust producers
   void CalcThrustProducerFM(double aThrustMagnitude_lbs, P6DofForceAndMomentsObject& aForceAndMoment);

   // Parent object (propulsion system)
   P6DofPropulsionSystem* mParentPropulsionSystemPtr;

   // A unique name for each subclass of thrust producer
   std::string mTypeOfThrustProducer;

   // Nominal thrust direction vector (does not include vectoring/reversing)
   UtVec3dX mNominalThrustVectorDirection;

   // Current thrust direction vector (includes vectoring/reversing)
   UtVec3dX mCurrentThrustVectorDirection;

   // Location of the reference point (typically center of engine)
   UtVec3dX mReferenceLocation_ft;

   // Location of thrust relative to parent
   UtVec3dX mThrustLocation_ft;

   // Current forces and moments of this thrust producer (lbs and ft-lbs respectively)
   P6DofForceAndMomentsObject mCurrentThrustFM_lbs_ftlbs;

   // Rotation of thrust relative to parent
   UtVec3dX mThrustInstalledAnglesYPR_rad;

   // Location of thrust relative to thrust producer reference point along the x-axis.
   // A positive value is forward of the engine, negative is aft.
   double mThrustOffset_ft;

   // Thrust vectoring states
   bool   mThrustVectoringEnabled;
   double mThrustVectorYaw_rad;
   double mThrustVectorPitch_rad;

   // Thrust reverser states
   bool   mThrustReverserEnabled;
   double mThrustReverserSetting;

   // This is true if a shutdown is in progress. As the shutdown
   // process is handled, this should be set to false.
   bool mShutdownInProgress;

   // This is used during a shut down to indicate how far into
   // the current calculation frame the shut down occurs.
   int64_t mShutdownFraction_nanosec;

   // This is used during an ignition to indicate how far into
   // the current calculation frame the ignition occurs.
   int64_t mIgniteTimeInFrame_nanosec;

   // These are for a reference area that can produce drag when
   // the engine is not operating
   bool   mInOpRefAreaValid;
   double mInOpRefArea_ft2;

   P6DofMassProperties* mMassPropertiesPtr;

   double mCurrentThrustFactor;

   bool mAfterburnerPresent;

   bool mEngineOperating;
   bool mEngineSmoking;
   bool mAfterburnerOn;
   bool mContrailing;
   bool mProducingSmokeTrail;
   bool mEngineDamageSmokeActivated;
};

#endif
