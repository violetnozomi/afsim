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

#ifndef WSFRIGIDBODYTHRUSTPRODUCEROBJECT_HPP
#define WSFRIGIDBODYTHRUSTPRODUCEROBJECT_HPP

class UtInput;
#include "WsfRigidBodySixDOF_FlightControlSystem.hpp"
#include "WsfSixDOF_ForceAndMomentsObject.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"

namespace wsf
{
namespace six_dof
{
class RigidBodyPropulsionSystem;

class WSF_SIX_DOF_EXPORT RigidBodyThrustProducerObject : public ThrustProducerObject
{
public:
   // This is used to create a type of a RigidBodyThrustProducerObject
   RigidBodyThrustProducerObject()        = default;
   ~RigidBodyThrustProducerObject()       = default;
   RigidBodyThrustProducerObject& operator=(const RigidBodyThrustProducerObject& other) = delete;

   ThrustProducerObject* Clone() const override;

   // ProcessInput is called to read input data from a UtInput stream, which
   // is typically an 'input file'.
   bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) override;

   // ThrustProducerObject-derived classes should include CalculateThrust and
   // UpdateThrust functions. CalculateThrust performs calculations but does
   // not update 'state' while UpdateThrust performs calculations and also
   // updates the state.

   // The CalculateThrust function is called to calculate the engine's
   // forces and moments. It does not change the state of the engine.
   // To change the state, call UpdateThrust instead. This version
   // uses the thrust producer's internal throttle values.
   void CalculateThrust(double                 aDeltaT_sec,
                        double                 aAlt_ft,
                        double                 aDynPress_lbsqft,
                        double                 aStatPress_lbssqft,
                        double                 aSpeed_fps,
                        double                 aMach,
                        double                 aAlpha_rad,
                        double                 aBeta_rad,
                        ForceAndMomentsObject& aForceAndMoment,
                        double&                aFuelBurnRate_pps,
                        double&                aFuelBurned_lbs);

   // The UpdateThrust function is called to calculate the engine's
   // forces and moments and update the state of the engine.
   // To perform F&M calculations without changing the state, call the
   // CalculateThrust function instead.
   void UpdateThrust(double                 aDeltaT_sec,
                     double                 aAlt_ft,
                     double                 aDynPress_lbsqft,
                     double                 aStatPress_lbssqft,
                     double                 aSpeed_fps,
                     double                 aMach,
                     double                 aAlpha_rad,
                     double                 aBeta_rad,
                     ForceAndMomentsObject& aForceAndMoment,
                     double&                aFuelBurnRate_pps,
                     double&                aFuelBurned_lbs);

   double GetThrottleMilSetting() override { return mThrottleSettingMIL->value; }
   double GetThrottleAbSetting() override { return mThrottleSettingAB->value; }

   // Set reference location and attitude (YPR) relative to parent
   void SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft, const UtVec3dX& aThrustYPR_rad) override;

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

   // Direction of thrust relative to parent
   void GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const override;

   // CalcCurrentThrustVectors provides the current thrust unit vector and thrust location
   // in body coordinates including any thrust vectoring and reversing. The
   // aThrustFactor indicates the magnitude factor for the thrust (positive is forward thrust
   // and negative is reverse thrust, ranging from:
   // 1 = full forward thrust to -0.5 = full reverse thrust
   void CalcCurrentThrustVectors();

   // .....................................................................................................

   struct ThrustControlInputValueData
   {
      std::string name;
      size_t      handle = 0;
      double      value  = 0.0;
   };

   struct ThrustBooleanControlInputData
   {
      std::string name;
      size_t      handle = 0;
      bool        value  = false;
   };

   void SetControlInputHandles(const RigidBodyFlightControlSystem* aFlightControls);
   void SetControlInputValues(const RigidBodyFlightControlSystem* aFlightControls);

   static void SetControlInputHandle(const RigidBodyFlightControlSystem* aFlightControls,
                                     ThrustControlInputValueData*        aControlInput);
   static void SetControlInputValue(const RigidBodyFlightControlSystem* aFlightControls,
                                    ThrustControlInputValueData*        aControlInput);

protected:
   RigidBodyThrustProducerObject(const RigidBodyThrustProducerObject& aSrc);

   // For deep copies within the class
   void DeriveFrom(const ThrustProducerObject* aSrc) override;

   // This will return the thrust vector using the thrust magnitude and current direction
   UtVec3dX CalcThrustVectorFromThrust(double aThrust_lbs);

   // This calculates various thrust F&M vectors.
   // It is used within the CalculateThrust function of thrust producers
   void CalcThrustProducerFM(double aThrustMagnitude_lbs, ForceAndMomentsObject& aForceAndMoment);

   // Current thrust direction vector (includes vectoring/reversing)
   UtVec3dX mCurrentThrustVectorDirection{1.0, 0.0, 0.0};

   // Thrust vectoring states
   double mThrustVectorYaw_rad   = 0.0;
   double mThrustVectorPitch_rad = 0.0;

   // Thrust reverser states
   bool   mThrustReverserEnabled = false;
   double mThrustReverserSetting = 0.0;

   // This is true if a shutdown is in progress. As the shutdown
   // process is handled, this should be set to false.
   bool mShutdownInProgress = false;

   // This is used during a shut down to indicate how far into
   // the current calculation frame the shut down occurs.
   int64_t mShutdownFraction_nanosec = 0;

   // This is used during an ignition to indicate how far into
   // the current calculation frame the ignition occurs.
   int64_t mIgniteTimeInFrame_nanosec = 0;

   double mCurrentThrustFactor = 1.0;

   // These settings won't be used by all engine types
   std::unique_ptr<ThrustControlInputValueData> mThrottleSettingMIL{nullptr};
   std::unique_ptr<ThrustControlInputValueData> mThrottleSettingAB{nullptr};
   std::unique_ptr<ThrustControlInputValueData> mThrottleSettingReverser{nullptr};
   std::unique_ptr<ThrustControlInputValueData> mThrottleSettingYaw{nullptr};
   std::unique_ptr<ThrustControlInputValueData> mThrottleSettingPitch{nullptr};
};
} // namespace six_dof
} // namespace wsf


#endif
