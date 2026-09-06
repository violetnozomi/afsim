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

#include "WsfThermalSystem.hpp"

#include <cassert>
#include <cmath>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
WsfThermalSystem::WsfThermalSystem(const WsfScenario& aScenario)
   : WsfPlatformPart(aScenario, cCOMPONENT_ROLE<WsfThermalSystem>())
   , mDebug(false)
   , mTemperature(0.0)
   , mLastTemperature(0.0)
   , mTargetTemperature(0.0)
   , mHighTemperature(0.0)
   , mUpdateInterval(0.0)
   , mLastUpdateTime(0.0)
   , mHeatCapacity(0.0)
   , mHeatSink(0.0)
   , mIsSimple(false)
   , mHeatingTime(0.0)
   , mSimpleRechargeRate(0.0)
   , mPowerDissipated(0.0)
{
}

// =================================================================================================
WsfThermalSystem::WsfThermalSystem(const WsfThermalSystem& aSrc)
   : WsfPlatformPart(aSrc)
   , mDebug(aSrc.mDebug)
   , mTemperature(aSrc.mTemperature)
   , mLastTemperature(aSrc.mLastTemperature)
   , mTargetTemperature(aSrc.mTargetTemperature)
   , mHighTemperature(aSrc.mHighTemperature)
   , mUpdateInterval(aSrc.mUpdateInterval)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mHeatCapacity(aSrc.mHeatCapacity)
   , mHeatSink(aSrc.mHeatSink)
   , mIsSimple(aSrc.mIsSimple)
   , mHeatingTime(aSrc.mHeatingTime)
   , mSimpleRechargeRate(aSrc.mSimpleRechargeRate)
   , mPowerDissipated(aSrc.mPowerDissipated)
   , mComponents(aSrc.mComponents)
{
}

// =================================================================================================
// virtual
WsfThermalSystem::~WsfThermalSystem() {}

// =================================================================================================
bool WsfThermalSystem::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "component")
   {
      WsfThermalSystemComponent component;
      myCommand = component.ProcessInput(aInput);
      if (myCommand)
      {
         mComponents.push_back(component);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   else if (command == "component_type")
   {
      // next should be the type name
      std::string type;
      aInput.ReadValue(type);
      WsfThermalSystemComponent* componentPtr = new WsfThermalSystemComponent();
      componentPtr->SetType(type);
      UtInputBlock block(aInput, "end_component_type");
      while (block.ReadCommand(command))
      {
         componentPtr->ProcessInput(block.GetInput());
      }

      // make sure we have enough data
      if (componentPtr->GetSpecificHeatCapacity() > 0.0)
      {
         WsfScenario::FromInput(aInput).GetThermalSystemComponentTypes().Add(componentPtr->GetType(),
                                                                             std::unique_ptr<WsfThermalSystemComponent>(
                                                                                componentPtr));
      }
      else
      {
         auto out = ut::log::error() << "WsfThermalSystemComponent: Not enough data for type.";
         out.AddNote() << "Type: " << type;
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "heat_sink")
   {
      aInput.ReadValueOfType(mHeatSink, UtInput::cPOWER);
   }
   else if ((command == "target_temperature") || (command == "low_temperature")) // for simple operation
   {
      aInput.ReadValueOfType(mTargetTemperature, UtInput::cTEMPERATURE);
   }
   else if (command == "high_temperature") // for simple operation
   {
      aInput.ReadValueOfType(mHighTemperature, UtInput::cTEMPERATURE);
   }
   else if (command == "initial_temperature")
   {
      aInput.ReadValueOfType(mTemperature, UtInput::cTEMPERATURE);
   }
   else if (command == "power_dissipated")
   {
      aInput.ReadValueOfType(mPowerDissipated, UtInput::cPOWER);
   }
   else if (command == "simple")
   {
      mIsSimple = true;
   }
   else if (command == "heating_time")
   {
      aInput.ReadValueOfType(mHeatingTime, UtInput::cTIME);
   }
   else if (command == "recharge_rate")
   {
      aInput.ReadValue(mSimpleRechargeRate);
      aInput.ValueInClosedRange(mSimpleRechargeRate, 0.0, 1.0);
   }
   else if (command == "debug")
   {
      aInput.ReadValue(mDebug);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Given heat transfer to the system, return the new temperature.
double WsfThermalSystem::TransferHeat(double aSimTime, double aHeatTransfer)
{
   double dt = aSimTime - mLastUpdateTime;
   if (dt > 0.0)
   {
      double heatTransfer = aHeatTransfer - mHeatSink * dt;
      if (mComponents.size() > 0)
      {
         mLastTemperature = mTemperature;
         CalculateHeatCapacity();
         double temp = GetTemperature(heatTransfer);

         unsigned i     = 0;
         double   ptt   = 0.0;
         bool     found = false;
         for (unsigned j = 0; j < mComponents.size(); ++j)
         {
            double pttTest = mComponents[j].GetPhaseTransitionTemperature();
            // The phase transition temperature must be between the calculated temperature and the last
            // temperature.  We also need to consider the highest or lowest phase change, depending on whether
            // the temp is increasing or decreasing.
            if (mComponents[j].InPhaseChange() ||
                ((temp < pttTest) && (mLastTemperature > pttTest) && ((!found) || (ptt < pttTest))) ||
                ((temp > pttTest) && (mLastTemperature < pttTest) && ((!found) || (ptt > pttTest))))
            {
               i     = j;
               ptt   = pttTest;
               found = true;
            }
         }
         if (found) // Need to go through phase transition
         {
            for (unsigned j = 0; j < mComponents.size(); ++j)
            {
               mComponents[j].SetTemperature(ptt);
            }
            mTemperature = ptt;
            heatTransfer -= GetHeatTransfer(ptt);

            // Now attempt to push through the phase transition.
            double remainingEnergy = mComponents[i].GetRemainingPhaseTransitionEnergy(heatTransfer > 0.0);
            if (mDebug)
            {
               auto out = ut::log::debug() << "Thermal System: Temp Update (phase transition).";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Temp: " << mTemperature;
            }

            if (fabs(heatTransfer) < fabs(remainingEnergy))
            {
               mTemperature = ptt;
               mComponents[i].TransferHeat(heatTransfer); // no more heat to transfer...
               if (mDebug)
               {
                  auto out  = ut::log::debug() << "Transition not complete.";
                  auto note = out.AddNote() << "Component: ";
                  mComponents[i].PrintState(note);
               }
            }
            else
            {
               mComponents[i].TransferHeat(remainingEnergy);
               heatTransfer -= remainingEnergy;

               if (mDebug)
               {
                  auto out  = ut::log::debug() << "Transition complete.";
                  auto note = out.AddNote() << "Component:";
                  mComponents[i].PrintState(note);
               }

               // We're not done; there is still heat to transfer.
               TransferHeat(aSimTime, heatTransfer);
            }
         }
         else // no phase transition...
         {
            // double heat = GetHeatTransfer(ptt);  // to phase transition.
            for (i = 0; i < mComponents.size(); ++i)
            {
               mComponents[i].SetTemperature(temp);
            }
            mTemperature = temp;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Thermal System: Temp Update (no phase transition).";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Temp: " << mTemperature;
               if (mTemperature <= mTargetTemperature)
               {
                  out.AddNote() << "Target temperature reached.";
                  out.AddNote() << "Target Temp: " << mTargetTemperature;
               }
            }
         }
      }

      mLastUpdateTime = aSimTime;
   } // dt > 0.0
   return mTemperature;
}

// =================================================================================================
//! Get the change only due to specific heat change only.
//! This may not give the correct answer as we may have to go
//! back and consider phase transitions.
// private
double WsfThermalSystem::GetTemperature(double aHeatTransfer)
{
   double temp = aHeatTransfer / mHeatCapacity + mLastTemperature;
   if (temp < mTargetTemperature)
   {
      temp = mTargetTemperature;
   }
   return temp;
}

// =================================================================================================
//! Given a goal temperature, return the amount of heat transfer
//! due to specific heat transfer only (no phase changes).
double WsfThermalSystem::GetHeatTransfer(double aTemperature)
{
   return mHeatCapacity * (aTemperature - mLastTemperature);
}

double WsfThermalSystem::GetRequiredHeatForTemperature(double aTemperature) const
{
   double heat = 0.0;
   for (unsigned i = 0; i < mComponents.size(); ++i)
   {
      heat += mComponents[i].GetRequiredHeatForTemperature(aTemperature);
   }
   return heat;
}

// =================================================================================================
const int* WsfThermalSystem::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_THERMAL_SYSTEM, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
bool WsfThermalSystem::Initialize(double aSimTime)
{
   bool ok = true;
   if (mIsSimple)
   {
      ok = InitializeSimple();
   }
   else
   {
      if (mComponents.size() == 0)
      {
         ut::log::error() << "Thermal System: Must specify at least one thermal_component.";
         return false;
      }
      if (mTargetTemperature == 0.0)
      {
         ut::log::error() << "Thermal System: Must specify a target_temperature.";
         return false;
      }
      if (mTemperature == 0.0)
      {
         mTemperature = mTargetTemperature; // start at full cooldown.
      }
      CalculateHeatCapacity();
      for (unsigned i = 0; i < mComponents.size(); ++i)
      {
         mComponents[i].SetTemperature(mTemperature);
      }
   }
   return ok & WsfPlatformPart::Initialize(aSimTime);
}

// =================================================================================================
// virtual
//! If running as a simple thermal sink, call this initialize method instead of Initialize()
bool WsfThermalSystem::InitializeSimple()
{
   bool ok = (mSimpleRechargeRate > 0.0) && (mHeatingTime > 0.0) && (mPowerDissipated > 0.0) &&
             (mTargetTemperature > 0.0) && (mHighTemperature > mTargetTemperature);
   if (ok)
   {
      assert(mComponents.size() == 0);
      // Insert a single block of aluminum to do the job.
      WsfThermalSystemComponent al = *(GetScenario().GetThermalSystemComponentTypes().Find("aluminum"));
      mHeatSink                    = mPowerDissipated * mSimpleRechargeRate;
      double energy                = mHeatingTime * (mPowerDissipated - mHeatSink);
      double mass                  = energy / (al.GetSpecificHeatCapacity() * (mHighTemperature - mTargetTemperature));
      al.SetMass(mass);
      mTemperature = mTargetTemperature;
      al.SetTemperature(mTargetTemperature);
      mComponents.push_back(al);
      CalculateHeatCapacity();
   }
   else
   {
      auto out = ut::log::error() << "Thermal System: Please specify the following for simple thermal operation:";
      if (mSimpleRechargeRate == 0.0)
      {
         out.AddNote() << "recharge_rate";
      }
      if (mHeatingTime == 0.0)
      {
         out.AddNote() << "heating_time";
      }
      if (mPowerDissipated == 0.0)
      {
         out.AddNote() << "power_dissipated";
      }
      if (mTargetTemperature == 0.0)
      {
         out.AddNote() << "low_temperature";
      }
      if (mHighTemperature == 0.0)
      {
         out.AddNote() << "high_temperature";
      }
      if (mTargetTemperature >= mHighTemperature)
      {
         out.AddNote() << "high_temperature value > low_temperature value";
      }
   }
   return ok;
}

// =================================================================================================
// Calculate the total heat capacity of the system.
void WsfThermalSystem::CalculateHeatCapacity()
{
   mHeatCapacity = 0.0;
   for (unsigned i = 0; i < mComponents.size(); ++i)
   {
      mHeatCapacity += mComponents[i].GetHeatCapacity(mTemperature);
   }
}

// =================================================================================================
void WsfThermalSystem::PrintInfo(double aLowTemp, double aHighTemp) const
{
   auto out = ut::log::info() << "Thermal System: Information:";
   out.AddNote() << "Low Temp: " << aLowTemp - 273.15 << " C";
   out.AddNote() << "High Temp: " << aHighTemp - 273.15 << " C";
   double energy = 0;
   for (unsigned i = 0; i < mComponents.size(); ++i)
   {
      const WsfThermalSystemComponent& tc   = mComponents[i];
      auto                             note = out.AddNote() << "Component:";
      energy += tc.PrintInfo(aLowTemp, aHighTemp, note);
   }
   out.AddNote() << "Total System Energy Capacity: " << energy << " J (" << energy / 1054 << " BTU).";
}

// =================================================================================================
//! Compute the total energy required to bring the system from one temperature to the other.
//! @return The heat capacity in joules of the system.
//! public
double WsfThermalSystem::GetEnergy(double aLowTemp, double aHighTemp) const
{
   double energy = 0;
   for (unsigned i = 0; i < mComponents.size(); ++i)
   {
      const WsfThermalSystemComponent& tc = mComponents[i];
      energy += tc.GetEnergy(aLowTemp, aHighTemp);
   }
   return energy;
}

// =================================================================================================
void WsfThermalSystem::SetTemperature(double aTemperature)
{
   for (unsigned i = 0; i < mComponents.size(); ++i)
   {
      mComponents[i].SetTemperature(aTemperature);
   }
   mTemperature = aTemperature;
}

// =================================================================================================
WsfThermalSystemComponent::WsfThermalSystemComponent()
{
   mMass                          = 0.0;
   mSpecificHeatCapacity          = 0.0;
   mSpecificHeatCapacityHighTemp  = 0.0;
   mTemperature                   = 0.0;
   mEnthalpy                      = 0.0;
   mEnthalpyTransitionTemperature = 100000.0; // so we won't use the high temp specific heat if not provided.
   mTotalTransitionEnergy         = 0.0;
   mTransitionEnergy              = 0.0;
   mInPhaseChange                 = false;
}

// =================================================================================================
//! Update based on heat transfer.
double WsfThermalSystemComponent::TransferHeat(double aHeatTransfer)
{
   double heatTransfer = aHeatTransfer;
   if (mInPhaseChange)
   {
      // must transfer energy equal to the enthalpy to change temperature again.
      double netEnergy = heatTransfer + mTransitionEnergy;
      if (netEnergy <= 0.0) // high-low temp change complete.
      {
         mInPhaseChange = false;
         heatTransfer   = netEnergy;
         SetTemperature(mEnthalpyTransitionTemperature - 1.0e-6);
      }
      else if (netEnergy >= mTotalTransitionEnergy) // low-high temp change complete
      {
         mInPhaseChange = false;
         heatTransfer -= (mTotalTransitionEnergy - mTransitionEnergy);
         SetTemperature(mEnthalpyTransitionTemperature + 1.0e-6);
      }
      else
      {
         mTransitionEnergy += heatTransfer;
      }
   }

   if (!mInPhaseChange)
   {
      double newTemp = heatTransfer / GetHeatCapacity() + mTemperature;

      if ((newTemp >= mEnthalpyTransitionTemperature) && (mTemperature < mEnthalpyTransitionTemperature))
      {
         mInPhaseChange = true;
         heatTransfer -= GetHeatCapacity() * (mEnthalpyTransitionTemperature - mTemperature);
         SetTemperature(mEnthalpyTransitionTemperature - 1.0e-6);
         // re-evaluate, as we may come back out of phase change.
         TransferHeat(heatTransfer);
      }
      else if ((newTemp <= mEnthalpyTransitionTemperature) && (mTemperature > mEnthalpyTransitionTemperature))
      {
         mInPhaseChange = true;
         // same heat transfer difference as above, but easier to understand.
         heatTransfer += GetHeatCapacity() * (mTemperature - mEnthalpyTransitionTemperature);
         SetTemperature(mEnthalpyTransitionTemperature + 1.0e-6);
         // re-evaluate, as we may come back out of phase change.
         TransferHeat(heatTransfer);
      }
      else // not in phase change
      {
         SetTemperature(newTemp);
      }
   }

   return mTemperature;
}

// =================================================================================================
bool WsfThermalSystemComponent::ProcessInput(UtInput& aInput)
{
   std::string command   = aInput.GetCommand();
   bool        myCommand = true;

   // TODO add inputs for new types from input files
   if (command == "component")
   {
      std::string type;
      aInput.ReadValue(type);
      *this = *WsfScenario::FromInput(aInput).GetThermalSystemComponentTypes().Find(type);

      aInput.ReadValueOfType(mMass, UtInput::cMASS);
      mTotalTransitionEnergy = mMass * mEnthalpy;
   }
   else if (command == "specific_heat_capacity")
   {
      aInput.ReadValue(mSpecificHeatCapacity);
   }
   else if (command == "specific_heat_capacity_high_temp")
   {
      aInput.ReadValue(mSpecificHeatCapacityHighTemp);
   }
   else if ((command == "enthalpy") || (command == "latent_heat"))
   {
      aInput.ReadValue(mEnthalpy);
   }
   else if ((command == "enthalpy_transition_temperature") || (command == "phase_change_temperature"))
   {
      aInput.ReadValueOfType(mEnthalpyTransitionTemperature, UtInput::cTEMPERATURE);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Constructor for adding a new type.
WsfThermalSystemComponent::WsfThermalSystemComponent(const std::string& aTypeString,
                                                     double             aSpecificHeatCapacity,
                                                     double             aSpecificHeatCapacityHighTemp,
                                                     double             aEnthalpy, //                      = 0.0;
                                                     double             aEnthalpyTransitionTemperature) // = 10000.0)
   : mMass(0.0)
   , mSpecificHeatCapacity(aSpecificHeatCapacity)
   , mSpecificHeatCapacityHighTemp(aSpecificHeatCapacityHighTemp)
   , mEnthalpy(aEnthalpy)
   , mEnthalpyTransitionTemperature(aEnthalpyTransitionTemperature)
   , mTotalTransitionEnergy(0.0)
   , mTransitionEnergy(0.0)
   , mInPhaseChange(false)
{
   SetType(aTypeString);
   if (mSpecificHeatCapacityHighTemp == 0.0)
   {
      mSpecificHeatCapacityHighTemp = mSpecificHeatCapacity;
   }
}

// =================================================================================================
double WsfThermalSystemComponent::GetHeatCapacity() const
{
   return GetHeatCapacity(mTemperature);
}

// =================================================================================================
//! Get the total available energy of the system, between the
//! two given temperatures.
double WsfThermalSystemComponent::GetEnergy(double aLowTemp, double aHighTemp) const
{
   double energy = 0;
   double ptt    = GetPhaseTransitionTemperature();
   if ((ptt >= aLowTemp) && (ptt <= aHighTemp))
   {
      energy = GetHeatCapacity(aLowTemp) * (ptt - aLowTemp) + GetHeatCapacity(aHighTemp) * (aHighTemp - ptt) +
               GetPhaseTransitionEnergy();
   }
   else
   {
      energy = GetHeatCapacity(aLowTemp) * (aHighTemp - aLowTemp);
   }
   return energy;
}

// =================================================================================================
double WsfThermalSystemComponent::GetHeatCapacity(double aTemperature) const
{
   if (aTemperature > mEnthalpyTransitionTemperature)
   {
      return mMass * mSpecificHeatCapacityHighTemp;
   }
   else
   {
      return mMass * mSpecificHeatCapacity;
   }
}

// =================================================================================================
//! Set the temperature, regardless of heat transfer.
void WsfThermalSystemComponent::SetTemperature(double aTemperature)
{
   mInPhaseChange = (aTemperature == mEnthalpyTransitionTemperature);
   if (mInPhaseChange)
   {
      if (mTemperature > mEnthalpyTransitionTemperature)
      {
         mTransitionEnergy = mTotalTransitionEnergy;
      }
      else if (mTemperature < mEnthalpyTransitionTemperature)
      {
         mTransitionEnergy = 0.0;
      }
   }
   if (aTemperature > mEnthalpyTransitionTemperature)
   {
      mTransitionEnergy = mTotalTransitionEnergy;
   }
   else if (aTemperature < mEnthalpyTransitionTemperature)
   {
      mTransitionEnergy = 0.0;
   }
   mTemperature = aTemperature;
}

// =================================================================================================
//! Given whether we intend to add or remove heat; return the energy necessary
//! to transition out of a phase change.  If the component is not in the phase
//! change state, this method returns zero.
double WsfThermalSystemComponent::GetRemainingPhaseTransitionEnergy(bool aIsAddingHeat) const
{
   double remainingEnergy = 0.0;
   if (mInPhaseChange)
   {
      if (aIsAddingHeat)
      {
         remainingEnergy = mTotalTransitionEnergy - mTransitionEnergy;
      }
      else
      {
         remainingEnergy = -mTransitionEnergy; // Negative heat transfer necessary.
      }
   }
   return remainingEnergy;
}

// =================================================================================================
double WsfThermalSystemComponent::GetRequiredHeatForTemperature(double aTemperature) const
{
   double heat  = 0.0;
   double dTemp = aTemperature - mTemperature;
   if (aTemperature > mTemperature)
   {
      if (mInPhaseChange)
      {
         heat += GetRemainingPhaseTransitionEnergy(dTemp > 0.0);
         heat += mSpecificHeatCapacityHighTemp * mMass * dTemp;
      }
      else if ((mEnthalpyTransitionTemperature > mTemperature) && (mEnthalpyTransitionTemperature < aTemperature))
      {
         heat += mSpecificHeatCapacity * mMass * (mEnthalpyTransitionTemperature - mTemperature);
         heat += mTotalTransitionEnergy;
         heat += mSpecificHeatCapacityHighTemp * mMass * (aTemperature - mEnthalpyTransitionTemperature);
      }
      else if (aTemperature < mEnthalpyTransitionTemperature)
      {
         heat += mSpecificHeatCapacity * mMass * dTemp;
      }
      else
      {
         heat += mSpecificHeatCapacityHighTemp * mMass * dTemp;
      }
   }
   else if (aTemperature < mTemperature)
   {
      if (mInPhaseChange)
      {
         heat += GetRemainingPhaseTransitionEnergy(false);
         heat += mSpecificHeatCapacity * mMass * dTemp;
      }
      else if ((mEnthalpyTransitionTemperature < mTemperature) && (mEnthalpyTransitionTemperature > aTemperature))
      {
         heat += mSpecificHeatCapacity * mMass * (mEnthalpyTransitionTemperature - mTemperature);
         heat += -mTotalTransitionEnergy;
         heat += mSpecificHeatCapacityHighTemp * mMass * (aTemperature - mEnthalpyTransitionTemperature);
      }
      else if (aTemperature < mEnthalpyTransitionTemperature)
      {
         heat += mSpecificHeatCapacity * mMass * dTemp;
      }
      else
      {
         heat += mSpecificHeatCapacityHighTemp * mMass * dTemp;
      }
   }
   return heat;
}

// =================================================================================================
double WsfThermalSystemComponent::PrintInfo(double aLowTemp, double aHighTemp, ut::log::MessageStream& aMessageStream) const
{
   auto&  out    = aMessageStream;
   double energy = 0.0;
   double heat   = 0.0;
   out.AddNote() << "Type: " << GetType();
   if ((mEnthalpyTransitionTemperature >= aLowTemp) && (mEnthalpyTransitionTemperature <= aHighTemp))
   {
      out.AddNote() << "Low Temp: " << aLowTemp - 273.15 << " C";
      out.AddNote() << "Transition Temp: " << mEnthalpyTransitionTemperature - 273.15 << " C";
      out.AddNote() << "High Temp: " << aHighTemp - 273.15 << " C";
      auto note = out.AddNote();

      heat = GetHeatCapacity(aLowTemp) * (mEnthalpyTransitionTemperature - aLowTemp);
      note.AddNote() << "Low-to-Transition: " << heat << " J (" << heat / 1054 << " BTU)";
      energy += heat;

      double pte = GetPhaseTransitionEnergy();
      note.AddNote() << "At Transition: " << heat << " J (" << heat / 1054 << " BTU)";
      energy += pte;

      heat = GetHeatCapacity(aHighTemp) * (aHighTemp - mEnthalpyTransitionTemperature);
      note.AddNote() << "Transition-to-High: " << heat << " J (" << heat / 1054 << " BTU)";
      energy += heat;

      note << "Heat: " << energy << " J (" << energy / 1054 << " BTU)";
   }
   else
   {
      energy = GetHeatCapacity(aLowTemp) * (aHighTemp - aLowTemp);
      out.AddNote() << "Low Temp: " << aLowTemp - 273.15 << " C";
      out.AddNote() << "High Temp: " << aHighTemp - 273.15 << " C";
      out.AddNote() << "Heat: " << heat << " J (" << heat / 1054 << " BTU)";
   }
   return energy;
}

// =================================================================================================
void WsfThermalSystemComponent::PrintState(ut::log::MessageStream& aMessageStream) const
{
   auto& out = aMessageStream;
   out.AddNote() << "Type: " << GetType();
   out.AddNote() << "Temp: " << mTemperature;
   if (mInPhaseChange)
   {
      out.AddNote() << "In phase change.";
      out.AddNote() << "BTU Transferred: " << mTransitionEnergy / 1054 << " / " << mTotalTransitionEnergy / 1054;
   }
}

// =================================================================================================
WsfThermalSystemComponentTypes::WsfThermalSystemComponentTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfThermalSystemComponent>(aScenario, "thermal_system_component")
{
   AddCoreType("aluminum", ut::make_unique<WsfThermalSystemComponent>("aluminum", 897));
   AddCoreType("water", ut::make_unique<WsfThermalSystemComponent>("water", 2050, 4181.3, 333550, 273.15));
}
