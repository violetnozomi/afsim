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

#ifndef WSFTHERMALSYSTEM_HPP
#define WSFTHERMALSYSTEM_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
#include "UtLog.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatformPart.hpp"

// Nested class ThermalSystem::Component
class WsfThermalSystemComponent : public WsfUncloneableObject
{
public:
   WsfThermalSystemComponent();

   //! Constructor for adding a new type.
   WsfThermalSystemComponent(const std::string& aTypeString,
                             double             aSpecificHeatCapacity,
                             double             aSpecificHeatCapacityHighTemp  = 0.0,
                             double             aEnthalpy                      = 0.0,
                             double             aEnthalpyTransitionTemperature = 10000.0);

   bool   ProcessInput(UtInput& aInput) override;
   double TransferHeat(double aHeatTransfer);

   double GetTemperature() { return mTemperature; }
   void   SetTemperature(double aTemperature);
   void   SetMass(double aMass) { mMass = aMass; }

   double GetRequiredHeatForTemperature(double aTemperature) const;

   double GetHeatCapacity() const;

   double GetHeatCapacity(double aTemperature) const;

   double GetSpecificHeatCapacity() const { return mSpecificHeatCapacity; }

   double GetPhaseTransitionTemperature() const { return mEnthalpyTransitionTemperature; }

   double GetPhaseTransitionEnergy() const { return mMass * mEnthalpy; }

   double GetRemainingPhaseTransitionEnergy(bool aIsAddingHeat) const;

   bool InPhaseChange() const { return mInPhaseChange; }

   double GetEnergy(double aLowTemp, double aHighTemp) const;

   double PrintInfo(double aLowTemp, double aHighTemp, ut::log::MessageStream& aMessageStream) const;

   void PrintState(ut::log::MessageStream& aMessageStream) const;

private:
   double mMass;

   double mSpecificHeatCapacity;
   double mSpecificHeatCapacityHighTemp; // Above phase change.
   double mTemperature;

   //! Currently only model one enthalpy.
   //! typically solid-liquid-solid (as with ice).
   double mEnthalpy;
   double mEnthalpyTransitionTemperature;
   double mTotalTransitionEnergy;
   double mTransitionEnergy;
   bool   mInPhaseChange;
};

//! The thermal system is comprised of various thermal components.
//! Heat is transferred to these components, and a basic caliorimetric calculation
//! is performed to determine the change in temperature of the system.
//! This is a very simple representation, in that it is one dimensional and time-independent,
//! So we are not worried about time-dependent heat transfer, only the transfer itself
//! (we assume the transfer "works" on the timescales in which we are interested).
class WSF_EXPORT WsfThermalSystem : public WsfPlatformPart
{
public:
   class Component; // Full declaration is below.

   WsfThermalSystem(const WsfScenario& aScenario);
   WsfThermalSystem(const WsfThermalSystem& aSrc);
   WsfThermalSystem& operator=(const WsfThermalSystem&) = delete;
   ~WsfThermalSystem() override;

   WsfObject* Clone() const override { return new WsfThermalSystem(*this); }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;

   WsfComponent* CloneComponent() const override { return new WsfThermalSystem(*this); }

   const int* GetComponentRoles() const override;

   void* QueryInterface(int aRole) override
   {
      if (aRole == cWSF_COMPONENT_THERMAL_SYSTEM)
      {
         return this;
      }
      if (aRole == cWSF_COMPONENT_PLATFORM_PART)
      {
         return dynamic_cast<WsfPlatformPart*>(this);
      }
      return nullptr;
   }

   double GetUpdateInterval() const override { return mUpdateInterval; }
   void   SetUpdateInterval(double aUpdateInterval) override { mUpdateInterval = aUpdateInterval; }

   virtual double TransferHeat(double aSimTime, double aHeatTransfer);

   //! Return whether the system is "valid", i.e., is being utilized.
   bool IsValid() const { return ((mComponents.size() > 0) || mIsSimple); }

   //! Return whether we are configuring this system with "simple" inputs.
   bool IsSimple() const { return mIsSimple; }

   double GetTemperature() const { return mTemperature; }

   void SetTemperature(double aTemperature);

   double GetHeatSink() const { return mHeatSink; }

   void PrintInfo(double aLowTemp, double aHighTemp) const;

   double GetRequiredHeatForTemperature(double aTemperature) const;

   double GetEnergy(double aLowTemp, double aHighTemp) const;

   // Set the power to be dissipated in a recharge rate interval.
   void SetPowerDissipated(double aPowerDissipated) { mPowerDissipated = aPowerDissipated; }

   void   SetLowTemperature(double aLowTemperature) { mTargetTemperature = aLowTemperature; }
   double GetLowTemperature() const { return mTargetTemperature; }

   void   SetHighTemperature(double aHighTemperature) { mHighTemperature = aHighTemperature; }
   double GetHighTemperature() const { return mHighTemperature; }

protected:
private:
   void   CalculateHeatCapacity();
   double GetHeatTransfer(double aTemperature);
   double GetTemperature(double aHeatTransfer);
   bool   InitializeSimple();

   bool                                   mDebug;
   double                                 mTemperature;
   double                                 mLastTemperature;
   double                                 mTargetTemperature;
   double                                 mHighTemperature;
   double                                 mUpdateInterval;
   double                                 mLastUpdateTime;
   double                                 mHeatCapacity;
   double                                 mHeatSink;
   bool                                   mIsSimple;
   double                                 mHeatingTime;
   double                                 mSimpleRechargeRate;
   double                                 mPowerDissipated;
   std::vector<WsfThermalSystemComponent> mComponents;

}; // class WsfThermalSystem

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfThermalSystem, cWSF_COMPONENT_THERMAL_SYSTEM)

class WsfThermalSystemComponentTypes : public WsfObjectTypeList<WsfThermalSystemComponent>
{
public:
   WsfThermalSystemComponentTypes(WsfScenario& aScenario);
};

#endif
