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

#ifndef P6DOFPROPULSIONSYSTEM_H
#define P6DOFPROPULSIONSYSTEM_H

#include "p6dof_export.h"

#include <cinttypes>
#include <string>
#include <unordered_map>
#include <vector>

#include "P6DofMassProperties.hpp"
#include "P6DofObject.hpp"
#include "P6DofThrustProducerObject.hpp"
#include "UtVec3dX.hpp"

class P6DofFlightControlSystem;
class P6DofForceAndMomentsObject;
class P6DofFuelTank;
class P6DofScenario;
class P6DofVehicle;
class UtInput;

class P6DOF_EXPORT P6DofPropulsionSystem : public P6DofObject
{
public:
   explicit P6DofPropulsionSystem(P6DofScenario* aScenario);
   ~P6DofPropulsionSystem() override;

   P6DofPropulsionSystem& operator=(const P6DofPropulsionSystem& other) = delete;

   P6DofPropulsionSystem* Clone() const;

   void SetParentVehicle(P6DofVehicle* aParentVehicle) override;

   struct FuelTransfer
   {
      std::string    xferName;
      std::string    sourceTankName;
      std::string    targetTankName;
      P6DofFuelTank* sourceTank;
      P6DofFuelTank* targetTank;
   };

   // ProcessInput reads initialization data
   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   // Update should be called every frame. It clears the temp attributes and
   // sets the current attributes. The other 'update functions' (UpdateFuelBurn,
   // UpdateFuelFill, and UpdateFuelTransfer) should be called prior to calling
   // the Update function.
   void Update(int64_t aSimTime_nanosec);

   void SetLastSimTime(int64_t aLastSimTime_nanosec) { mLastSimTime_nanosec = aLastSimTime_nanosec; }

   // The CalculatePropulsionFM function is called to calculate the propulsion
   // forces and moments. It does not change the state of the propulsion object.
   // To change the state, call UpdatePropulsionFM  instead. This version
   // uses the thrust producer's internal throttle values.
   void CalculatePropulsionFM(double                      aDeltaT_sec,
                              double                      aAlt_ft,
                              double                      aDynPress_lbsqft,
                              double                      aStatPress_lbssqft,
                              double                      aSpeed_fps,
                              double                      aMach,
                              double                      aAlpha_rad,
                              double                      aBeta_rad,
                              P6DofForceAndMomentsObject& aForceAndMoment,
                              double&                     aFuelBurnRate_pps,
                              double&                     aFuelBurned_lbs);

   // The UpdatePropulsionFM function is called to calculate the propulsion
   // forces and moments and update the state of the propulsion object.
   // To perform F&M calculations without changing the state, call the
   // CalculatePropulsionFM function instead. This version
   // uses the thrust producer's internal throttle values.
   void UpdatePropulsionFM(double                      aDeltaT_sec,
                           double                      aAlt_ft,
                           double                      aDynPress_lbsqft,
                           double                      aStatPress_lbssqft,
                           double                      aSpeed_fps,
                           double                      aMach,
                           double                      aAlpha_rad,
                           double                      aBeta_rad,
                           P6DofForceAndMomentsObject& aForceAndMoment,
                           double&                     aFuelBurnRate_pps,
                           double&                     aFuelBurned_lbs);

   // GetFuelTankByName returns the fuel tank with the specified name
   P6DofFuelTank* GetFuelTank(const std::string& aName) const;

   // GetFuelTankList returns the list of fuel tanks in this propulsion object
   const std::unordered_map<std::string, P6DofFuelTank*>& GetFuelTanks() const { return mFuelTankMap; }

   // GetThrustProducerObjectByName returns the engine with the specified name
   P6DofThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName) const;

   // GetThrustProduceByIndex returns the engine with the specified index
   P6DofThrustProducerObject* GetThrustProducerByIndex(size_t aIndex) const;

   // GetThrustProduceList returns the list of engines in this propulsion object
   const std::vector<P6DofThrustProducerObject*>& GetThrustProducerList() const { return mEngineList; }

   // SetThrottleLeverPosition sets the throttles on all engines of this object
   void SetThrottleLeverPosition(double aThrottleLeverPosition);

   // SetThrustVectoringPitch_deg sets the thrust vectoring in yaw
   void SetThrustVectoringYaw_deg(double aThrustVectoringAngle_deg);

   // SetThrustVectoringPitch_deg sets the thrust vectoring in pitch
   void SetThrustVectoringPitch_deg(double aThrustVectoringAngle_deg);

   // EnableThrustVectoring enables/disable thrust vectoring
   void EnableThrustVectoring(bool aEnableThrustVectoring);

   // SetThrustVectoringPitch_deg sets the thrust vectoring in pitch
   void SetThrustReserver(double aThrustReserverSetting);

   // EnableThrustReverser enables/disable thrust vectoring
   void EnableThrustReverser(bool aEnableThrustReverser);

   // Returns true if reverser is enabled
   bool ThrustReverserEnabled() const { return mEnableThrustReverser; }

   // Returns true if vectoring is enabled
   bool ThrustVectoringEnabled() const { return mEnableThrustVectoring; }

   // Returns true if an afterburner is present on any engine
   bool AfterburnerIsPresent() const;

   // GetNumThrustProducers returns the number of engines in this propulsion object
   size_t GetNumThrustProducers() const;

   // GetNumFuelTanks returns the number of fuel tanks in this propulsion object
   size_t GetNumFuelTanks() const;

   // Ignite ignites/starts all engines of this object
   void Ignite(int64_t aIgniteTimeInFrame_nanosec);

   // Shutdown terminates thrust for all engines of this object
   // aTerminateTime_sec is the time termination occurs
   void Shutdown(int64_t aTerminateTime_nanosec = 0);

   // RemoveFuelTankByName returns true if the tank was removed
   bool RemoveFuelTankByName(std::string aName);

   // This requests adding a fuel transfer, but does not "connect it up".
   // It is typically used during setup (such as ProcessInput) and
   // returns true if the fuel transfer was added.
   bool AddFuelTransfer(const std::string& aTransferName,
                        const std::string& aSourceTankName,
                        const std::string& aTargetTankName);

   // This is used to support script-commanded fuel transfers and "connects
   // it up" within the fuel system. It returns true if the fuel transfer was added.
   bool AddFuelTransferAndConnect(const std::string& aTransferName,
                                  const std::string& aSourceTankName,
                                  const std::string& aTargetTankName);

   // RemoveFuelTransfer returns true if the transfer was removed
   bool RemoveFuelTransfer(const std::string& aTransferName);

   bool ModifyFuelQuantity(const std::string& aTankName, double aQty_lbs);

   bool FuelTransferNameUnique(const std::string& aName) const;

   // This returns true is at least one thrust producer is producing thrust
   bool IsProducingThrust() const;

   // This specifies the fuel feed (input tank name) for the specified engine.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string& aEngineName, std::string aFuelTankName);

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   bool SetFuelFeed(std::string aFuelTankName);

   // .....................................................................................................

   void SetControlInputHandles(const P6DofFlightControlSystem* aFlightControlSystem);
   void SetControlInputValues(const P6DofFlightControlSystem* aFlightControlSystem);

   // .....................................................................................................

   // This returns the maximum internal fuel for the object.
   // Note that this is quantity of fuel in fuel tanks, and does not include fuel in solid propellant rockets, for example.
   double GetInternalFuelTankCapacity_lbs() const;

   // This returns the current internal fuel for the object
   double GetCurrentInternalFuelTankQuantity_lbs() const;

   // This adds fuel in a simplistic manner. It does not use the "fill rate" limit of
   // tanks -- hence all of the fuel is put into the tank(s) at once. Later, we can
   // add a "proper" fuel fill that is more realistic and deals with a fill rate
   // that will require a finite amount of time to transfer. This function will try to
   // fill all tanks, but if insufficient fuel is being added, it will split the fuel
   // across tanks, making all tanks the same percent full. The function returns the
   // amount of fuel added.
   double AddFuelQuantity_lbs(double aFuelToAdd_lbs);

   // This adds fuel, as needed, to each tank in the system to the specified
   // percent full level for each tank. If the tank was already at a higher
   // percent full, fuel will be "drained" to bring it to the specified level.
   // The function returns the amount of fuel used in filling/draining all tanks.
   double FillAllTanks(double aPercentFull);

   // This drains all fuel from all of the tanks
   void EmptyAllTanks();

   // .....................................................................................................

   // This returns a pointer to the mass properties or null, if none exist
   const P6DofMassProperties& GetMassProperties() const { return mMassProperties; }

   // This calculates the current mass properties, including those
   // from fuel tanks as well as some thrust producers such as
   // solid-propellant rockets
   void CalculateCurrentMassProperties();

   // .....................................................................................................

   // Appearance Parameters. These will determine if any engine has the specified condition.
   // After using this function, individual engines can be consulted for their conditions.

   // Return true if at least one engine is operating.
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsOperating(bool aTestSubobjects = false) const;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsSmoking(bool aTestSubobjects = false) const;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineHasAfterburnerOn(bool aTestSubobjects = false) const;

   // Return true if at least one engine is contrailing
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsContrailing(bool aTestSubobjects = false) const;

   // Return true if at least one engine has a smoke trailing effect
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   bool AnEngineIsEmittingSmokeTrail(bool aTestSubobjects = false) const;

   // This will make an engine smoke. If an index of zero is used, all engines will smoke.
   void MakeAnEngineSmoke(int aEngineIndex);

   // .....................................................................................................

protected:
   P6DofPropulsionSystem(const P6DofPropulsionSystem& aSrc);

   void PropulsionCalculationsFM(double                      aDeltaT_sec,
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

   void RemoveInvalidFuelTransfers();

   struct fuelTankData
   {
      P6DofFuelTank* sourceTank;
      double         fuelActuallyProvided_lbs;
   };

   struct tankMatching
   {
      P6DofFuelTank*            targetAddTank;
      std::vector<fuelTankData> sourceTankList;
   };

   struct ThrustProducerTypeInfo
   {
      // baseTypeName string identifier for the class (such as "Jet" or "Ramjet"), See P6DofThrustProducerObject::GetClassType()
      std::string baseTypeName;
      std::string typeName;
      std::string instanceName;
      std::string fuelFeedName;
      UtVec3dX    relPos;
      double      offset_ft;
      UtVec3dX    relYPR;
   };

   std::vector<P6DofThrustProducerObject*>         mEngineList;
   std::vector<ThrustProducerTypeInfo*>            mEngineTypeList;
   std::unordered_map<std::string, P6DofFuelTank*> mFuelTankMap;
   std::vector<FuelTransfer*>                      mFuelTransferList;
   double  mPropulsionSystemThrottleLeverPosition; // Overall throttle lever position
   int64_t mLastSimTime_nanosec;

   std::vector<P6DofThrustProducerObject::ThrustControlInputValueData*> mThrustControlInputValueDataList;

   P6DofThrustProducerObject::ThrustControlInputValueData* mThrottleSettingMIL;
   P6DofThrustProducerObject::ThrustControlInputValueData* mThrottleSettingAB;
   P6DofThrustProducerObject::ThrustControlInputValueData* mThrottleSettingReverser;
   P6DofThrustProducerObject::ThrustControlInputValueData* mThrottleSettingYaw;
   P6DofThrustProducerObject::ThrustControlInputValueData* mThrottleSettingPitch;
   bool                                                    mThrottleLeverPositionSet;
   bool                                                    mThrustReverserSet;
   bool                                                    mThrustVectorYawSet;
   bool                                                    mThrustVectorPitchSet;

   double mThrustVectoringAngleYaw_deg;
   double mThrustVectoringAnglePitch_deg;
   bool   mEnableThrustVectoring;
   double mThrustReserverSetting;
   bool   mEnableThrustReverser;

   P6DofMassProperties mMassProperties;
};

#endif
