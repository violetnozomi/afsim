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

#ifndef WSFSIXDOFPROPULSIONSYSTEM_HPP
#define WSFSIXDOFPROPULSIONSYSTEM_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>
#include <unordered_map>
#include <vector>

class UtInput;
#include "UtVec3dX.hpp"
#include "WsfSixDOF_FuelTank.hpp"
#include "WsfSixDOF_MassProperties.hpp"
#include "WsfSixDOF_Object.hpp"
#include "WsfSixDOF_ThrustProducerObject.hpp"
#include "WsfSixDOF_Utils.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT PropulsionSystem : public Object
{
public:
   PropulsionSystem() = default;

   PropulsionSystem& operator=(const PropulsionSystem& other) = delete;

   struct FuelTransfer
   {
      std::string xferName;
      std::string sourceTankName;
      std::string targetTankName;
      FuelTank*   sourceTank = nullptr;
      FuelTank*   targetTank = nullptr;
   };

   void SetParentVehicle(Mover* aParentVehicle) override;

   // ProcessInput reads initialization data
   virtual bool Initialize(int64_t aSimTime_nanosec) = 0;

   // Update should be called every frame. It clears the temp attributes and
   // sets the current attributes. The other 'update functions' (UpdateFuelBurn,
   // UpdateFuelFill, and UpdateFuelTransfer) should be called prior to calling
   // the Update function.
   virtual void Update(int64_t aSimTime_nanosec);

   void SetLastSimTime(int64_t aLastSimTime_nanosec) { mLastSimTime_nanosec = aLastSimTime_nanosec; }

   // GetFuelTankList returns the list of fuel tanks in this propulsion object
   const std::unordered_map<std::string, UtCloneablePtr<FuelTank>>& GetFuelTanks() const { return mFuelTankMap; }

   // GetFuelTank returns the fuel tank with the specified name
   FuelTank* GetFuelTank(const std::string& aName) const;

   // GetThrustProducerObjectByName returns the engine with the specified name
   virtual ThrustProducerObject* GetThrustProducerObjectByName(const std::string& aName) const = 0;

   // GetThrustProduceByIndex returns the engine with the specified index
   virtual ThrustProducerObject* GetThrustProducerByIndex(size_t aIndex) const = 0;

   // SetThrottleLeverPosition sets the throttles on all engines of this object
   virtual void SetThrottleLeverPosition(double aThrottleLeverPosition) = 0;

   // EnableThrustVectoring enables/disable thrust vectoring
   virtual void EnableThrustVectoring(bool aEnableThrustVectoring) = 0;

   // Returns true if vectoring is enabled
   bool ThrustVectoringEnabled() const { return mEnableThrustVectoring; }

   // Returns true if an afterburner is present on any engine
   virtual bool AfterburnerIsPresent() const = 0;

   // GetNumThrustProducers returns the number of engines in this propulsion object
   virtual size_t GetNumThrustProducers() const = 0;

   // GetNumFuelTanks returns the number of fuel tanks in this propulsion object
   size_t GetNumFuelTanks() const;

   // Ignite ignites/starts all engines of this object
   virtual void Ignite(int64_t aIgniteTimeInFrame_nanosec) = 0;

   // Shutdown terminates thrust for all engines of this object
   // aTerminateTime_sec is the time termination occurs
   virtual void Shutdown(int64_t aTerminateTime_nanosec = 0) = 0;

   // RemoveFuelTankByName returns true if the tank was removed
   bool RemoveFuelTankByName(std::string aName);

   // This adds a tank to the propulsion system, and links the tank to the propulsion system and vehicle
   bool AddFuelTank(std::unique_ptr<FuelTank> aFuelTank);

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
   virtual bool IsProducingThrust() const = 0;

   // This specifies the fuel feed (input tank name) for the specified engine.
   // It returns true if the assignment is successful.
   virtual bool SetFuelFeed(std::string& aEngineName, std::string aFuelTankName) = 0;

   // This specifies the fuel feed (input tank name) for all engines.
   // It returns true if the assignment is successful.
   virtual bool SetFuelFeed(std::string aFuelTankName) = 0;

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

   // This returns the summed mass properties for the propulsion system
   const MassProperties GetMassProperties() const;

   // .....................................................................................................

   // Appearance Parameters. These will determine if any engine has the specified condition.
   // After using this function, individual engines can be consulted for their conditions.

   // Return true if at least one engine is operating.
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   virtual bool AnEngineIsOperating(bool aTestSubobjects = false) const = 0;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   virtual bool AnEngineIsSmoking(bool aTestSubobjects = false) const = 0;

   // Return true if at least one engine is operating
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   virtual bool AnEngineHasAfterburnerOn(bool aTestSubobjects = false) const = 0;

   // Return true if at least one engine is contrailing
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   virtual bool AnEngineIsContrailing(bool aTestSubobjects = false) const = 0;

   // Return true if at least one engine has a smoke trailing effect
   // If aTestSubobjects (optional) is true, if will test subobjects. Otherwise, it
   // will only test its own propulsion objects (the default).
   virtual bool AnEngineIsEmittingSmokeTrail(bool aTestSubobjects = false) const = 0;

   // This will make an engine smoke. If an index of zero is used, all engines will smoke.
   virtual void MakeAnEngineSmoke(int aEngineIndex) = 0;

   // .....................................................................................................

protected:
   PropulsionSystem(const PropulsionSystem& aSrc);

   void RemoveInvalidFuelTransfers();

   struct ThrustProducerInfo
   {
      std::string fuelFeedName;
      UtVec3dX    relativePosition;
      UtVec3dX    relativeYawPitchRoll;
   };

   struct FuelTankData
   {
      FuelTank* sourceTank               = nullptr;
      double    fuelActuallyProvided_lbs = 0.0;
   };

   struct TankMatching
   {
      FuelTank*                 targetAddTank = nullptr;
      std::vector<FuelTankData> sourceTankList;
   };

   std::vector<std::unique_ptr<FuelTransfer>>                mFuelTransferList;
   std::unordered_map<std::string, UtCloneablePtr<FuelTank>> mFuelTankMap;

   int64_t mLastSimTime_nanosec = 0;

   double mPropulsionSystemThrottleLeverPosition = 0.0; // Overall throttle lever position
   bool   mThrottleLeverPositionSet              = false;

   bool mEnableThrustVectoring = false;
};
} // namespace six_dof
} // namespace wsf

#endif
