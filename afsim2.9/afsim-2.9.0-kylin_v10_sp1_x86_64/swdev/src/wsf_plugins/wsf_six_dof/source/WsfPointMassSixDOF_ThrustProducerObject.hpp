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

#ifndef WSFPOINTMASSSIXDOFTHRUSTPRODUCEROBJECT_HPP
#define WSFPOINTMASSSIXDOFTHRUSTPRODUCEROBJECT_HPP

class UtInput;
#include "WsfPointMassSixDOF_FlightControlSystem.hpp"
#include "WsfSixDOF_ForceAndRotationObject.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"

namespace wsf
{
namespace six_dof
{
class PointMassPropulsionSystem;
class TypeManager;

class WSF_SIX_DOF_EXPORT PointMassThrustProducerObject : public ThrustProducerObject
{
public:
   // This is used to create a type of a PointMassThrustProducerObject
   PointMassThrustProducerObject()        = default;
   ~PointMassThrustProducerObject()       = default;
   PointMassThrustProducerObject& operator=(const PointMassThrustProducerObject& other) = delete;

   ThrustProducerObject* Clone() const override;

   // ProcessInput is called to read input data from a UtInput stream, which
   // is typically an 'input file'.
   bool ProcessInput(UtInput& aInput, TypeManager* aTypeManager) override;

   // ProcessInput is called to read input data from a UtInput stream, which
   // is typically an 'input file'.
   bool ProcessTableInput(UtInput& aInput);

   // ThrustProducerObject-derived classes should include CalculateThrust and
   // UpdateThrust functions. CalculateThrust performs calculations but does
   // not update 'state' while UpdateThrust performs calculations and also
   // updates the state.


   // The CalculateThrust function is called to calculate the engine's
   // forces and moments. It does not change the state of the engine.
   // To change the state, call UpdateThrust instead. This version
   // uses the thrust producer's internal throttle values.
   void CalculateThrust(double                  aDeltaT_sec,
                        double                  aAlt_ft,
                        double                  aDynPress_lbsqft,
                        double                  aStatPress_lbssqft,
                        double                  aSpeed_fps,
                        double                  aMach,
                        double                  aAlpha_rad,
                        double                  aBeta_rad,
                        ForceAndRotationObject& aForceAndRotation,
                        double&                 aFuelBurnRate_pps,
                        double&                 aFuelBurned_lbs)
   {
      double thrustForce_lbs = 0.0;

      mEnginePtr->CalculateThrust(aDeltaT_sec,
                                  aAlt_ft,
                                  aDynPress_lbsqft,
                                  aStatPress_lbssqft,
                                  aSpeed_fps,
                                  aMach,
                                  aAlpha_rad,
                                  aBeta_rad,
                                  thrustForce_lbs,
                                  aFuelBurnRate_pps,
                                  aFuelBurned_lbs);

      CalcThrustProducerFM(thrustForce_lbs, aForceAndRotation);
   }

   // The UpdateThrust function is called to calculate the engine's
   // forces and moments and update the state of the engine.
   // To perform F&M calculations without changing the state, call the
   // CalculateThrust function instead.
   void UpdateThrust(double                  aDeltaT_sec,
                     double                  aAlt_ft,
                     double                  aDynPress_lbsqft,
                     double                  aStatPress_lbssqft,
                     double                  aSpeed_fps,
                     double                  aMach,
                     double                  aAlpha_rad,
                     double                  aBeta_rad,
                     ForceAndRotationObject& aForceAndRotation,
                     double&                 aFuelBurnRate_pps,
                     double&                 aFuelBurned_lbs)
   {
      double thrustForce_lbs = 0.0;

      mEnginePtr->UpdateThrust(aDeltaT_sec,
                               aAlt_ft,
                               aDynPress_lbsqft,
                               aStatPress_lbssqft,
                               aSpeed_fps,
                               aMach,
                               aAlpha_rad,
                               aBeta_rad,
                               thrustForce_lbs,
                               aFuelBurnRate_pps,
                               aFuelBurned_lbs);

      CalcThrustProducerFM(thrustForce_lbs, aForceAndRotation);
   }

   double GetThrottleMilSetting() override { return mThrottleSettingMIL; }
   double GetThrottleAbSetting() override { return mThrottleSettingAB; }

   // Set reference location and attitude (YPR) relative to parent
   void SetThrustReferenceOffsetAttitude(const UtVec3dX& aReferenceLocation_ft, const UtVec3dX& aThrustYPR_rad) override;

   // Direction of thrust relative to parent
   void GetCurrentThrustVectorDirection(UtVec3dX& aDirection) const override;

   // CalcCurrentThrustVectors provides the current thrust unit vector and thrust location
   // in body coordinates including any thrust vectoring and reversing. The
   // aThrustFactor indicates the magnitude factor for the thrust (positive is forward thrust
   // and negative is reverse thrust, ranging from:
   // 1 = full forward thrust to -0.5 = full reverse thrust
   void CalcCurrentThrustVectors();

protected:
   PointMassThrustProducerObject(const PointMassThrustProducerObject& aSrc);

   // For deep copies within the class
   void DeriveFrom(const ThrustProducerObject* aSrc) override;

   // This will return the thrust vector using the thrust magnitude and current direction
   UtVec3dX CalcThrustVectorFromThrust(double aThrust_lbs);

   UtVec3dX CalcOmegaDotIncrementFromThrust(double aThrust_lbs);

   // This calculates various thrust F&M vectors.
   // It is used within the CalculateThrust function of thrust producers
   void CalcThrustProducerFM(double aThrustMagnitude_lbs, ForceAndRotationObject& aForceAndMoment);

   double mCurrentThrustFactor = 1.0;

   // These settings won't be used by all engine types
   double mThrottleSettingMIL = 0.0;
   double mThrottleSettingAB  = 0.0;

   UtVec3dX mOmegaDotIncrementSlope_rps2_lbs;
};
} // namespace six_dof
} // namespace wsf

#endif
