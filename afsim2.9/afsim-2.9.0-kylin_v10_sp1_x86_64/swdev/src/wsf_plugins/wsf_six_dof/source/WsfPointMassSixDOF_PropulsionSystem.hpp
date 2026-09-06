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

#ifndef WSFPOINTMASSSIXDOFPROPULSIONSYSTEM_HPP
#define WSFPOINTMASSSIXDOFPROPULSIONSYSTEM_HPP

#include "WsfPointMassSixDOF_ThrustProducerObject.hpp"
#include "WsfSixDOF_PropulsionSystem.hpp"

namespace wsf
{
namespace six_dof
{
class ForceAndRotationObject;
class PointMassFlightControlSystem;

class WSF_SIX_DOF_EXPORT PointMassPropulsionSystem : public PropulsionSystem
{
public:
   PointMassPropulsionSystem() = default;

   PointMassPropulsionSystem& operator=(const PointMassPropulsionSystem& other) = delete;

   PointMassPropulsionSystem* Clone() const;

   void SetParentVehicle(Mover* aParentVehicle) override;

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput, const TypeManager* aTypeManager);
   bool Initialize(int64_t aSimTime_nanosec) override;
   void LinkControls(PointMassFlightControlSystem* aFlightControls);

   // GetThrustProduceList returns the list of engines in this propulsion object
   const std::vector<std::unique_ptr<PointMassThrustProducerObject>>& GetThrustProducerList() const
   {
      return mEngineList;
   }

   // The CalculatePropulsionFM function is called to calculate the propulsion
   // forces and moments. It does not change the state of the propulsion object.
   // To change the state, call UpdatePropulsionFM  instead. This version
   // uses the thrust producer's internal throttle values.
   void CalculatePropulsionFM(double                  aDeltaT_sec,
                              double                  aAlt_ft,
                              double                  aDynPress_lbsqft,
                              double                  aStatPress_lbssqft,
                              double                  aSpeed_fps,
                              double                  aMach,
                              double                  aAlpha_rad,
                              double                  aBeta_rad,
                              ForceAndRotationObject& aForceAndMoment,
                              double&                 aFuelBurnRate_pps,
                              double&                 aFuelBurned_lbs);

   // The UpdatePropulsionFM function is called to calculate the propulsion
   // forces and moments and update the state of the propulsion object.
   // To perform F&M calculations without changing the state, call the
   // CalculatePropulsionFM function instead. This version
   // uses the thrust producer's internal throttle values.
   void UpdatePropulsionFM(double                  aDeltaT_sec,
                           double                  aAlt_ft,
                           double                  aDynPress_lbsqft,
                           double                  aStatPress_lbssqft,
                           double                  aSpeed_fps,
                           double                  aMach,
                           double                  aAlpha_rad,
                           double                  aBeta_rad,
                           ForceAndRotationObject& aForceAndMoment,
                           double&                 aFuelBurnRate_pps,
                           double&                 aFuelBurned_lbs);

   // .....................................................................................................

   // GetThrustProducerObjectByName returns the engine with the specified name
   ThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName) const override;

   // GetThrustProduceByIndex returns the engine with the specified index
   ThrustProducerObject* GetThrustProducerByIndex(size_t aIndex) const override;

   // SetThrottleLeverPosition sets the throttles on all engines of this object
   void SetThrottleLeverPosition(double aThrottleLeverPosition) override;

   // EnableThrustVectoring enables/disable thrust vectoring
   void EnableThrustVectoring(bool aEnableThrustVectoring) override;

   // Returns true if an afterburner is present on any engine
   bool AfterburnerIsPresent() const override;

   // GetNumThrustProducers returns the number of engines in this propulsion object
   size_t GetNumThrustProducers() const override;

   // Ignite ignites/starts all engines of this object
   void Ignite(int64_t aIgniteTimeInFrame_nanosec) override;

   // Shutdown terminates thrust for all engines of this object
   // aTerminateTime_sec is the time termination occurs
   void Shutdown(int64_t aTerminateTime_nanosec = 0) override;

   // This returns true is at least one thrust producer is producing thrust
   bool IsProducingThrust() const override;

   // This specifies the fuel feed (input tank name) for the specified engine.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string& aEngineName, std::string aFuelTankName) override;

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aFuelTankName) override;

   // .....................................................................................................

   // Appearance Parameters. These will determine if any engine has the specified condition.
   // After using this function, individual engines can be consulted for their conditions.

   // Return true if at least one engine is operating.
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsOperating(bool aTestSubobjects = false) const override;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsSmoking(bool aTestSubobjects = false) const override;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineHasAfterburnerOn(bool aTestSubobjects = false) const override;

   // Return true if at least one engine is contrailing
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsContrailing(bool aTestSubobjects = false) const override;

   // Return true if at least one engine has a smoke trailing effect
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsEmittingSmokeTrail(bool aTestSubobjects = false) const override;

   // This will make an engine smoke. If an index of zero is used, all engines will smoke.
   void MakeAnEngineSmoke(int aEngineIndex) override;

   // .....................................................................................................

protected:
   PointMassPropulsionSystem(const PointMassPropulsionSystem& aSrc);

   void PropulsionCalculationsFM(double                  aDeltaT_sec,
                                 double                  aAlt_ft,
                                 double                  aDynPress_lbsqft,
                                 double                  aStatPress_lbssqft,
                                 double                  aSpeed_fps,
                                 double                  aMach,
                                 double                  aAlpha_rad,
                                 double                  aBeta_rad,
                                 ForceAndRotationObject& aForceAndMoment,
                                 double&                 aFuelBurnRate_pps,
                                 double&                 aFuelBurned_lbs,
                                 bool                    aUpdateData);

   std::vector<std::unique_ptr<PointMassThrustProducerObject>> mEngineList;

   const double* mThrottleSettingMilPtr = nullptr;
   const double* mThrottleSettingAbPtr  = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif
