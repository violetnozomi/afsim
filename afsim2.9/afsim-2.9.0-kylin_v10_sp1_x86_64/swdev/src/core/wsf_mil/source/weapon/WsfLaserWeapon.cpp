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

#include "WsfLaserWeapon.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfException.hpp"
#include "WsfHighEnergyLaser.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfThermalSystemTypes.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

namespace
{
const unsigned cUNLIMITED_SHOTS       = 1000000;
const double   cUNLIMITED_FIRING_TIME = 1.0e100;
} // namespace

WsfLaserWeapon::WsfLaserWeapon(WsfScenario& aScenario)
   : WsfDirectedEnergyWeapon(aScenario)
   , mHEL_Ptr(nullptr)
   , mState(cREADY_TO_FIRE)
   , mThermalSystemPtr(nullptr)
   , mFiringTime(0.0)
   , mTotalFiringTime(cUNLIMITED_FIRING_TIME)
   , mLastFiringStartTime(0.0)
   , mLastFiringEndTime(0.0)
   , mFiringUpdateInterval(0.0)
   , mCoolingUpdateInterval(0.0)
   , mPostCoolingCapacity(5.0)
   , mEfficiency(0.3)
   , mLowTempLimit(253.15)
   , mHighTempLimit(373.15)
   , mPowerDissipation(0.0)
   , mNumberOfShots(cUNLIMITED_SHOTS)
   , mCurrentShotNumber(0)
   , mShotId(0)
   , mCoolingTime(30.0)
   , mLastCoolingStartTime(0.0)
   , mLastUpdateTime(0.0)
   , mThermalSystemName(nullptr)
   , mAtThermalLimit(false)
   , mCommentsEnabled(false)
{
}

WsfLaserWeapon::WsfLaserWeapon(const WsfLaserWeapon& aSrc)
   : WsfDirectedEnergyWeapon(aSrc)
   , mHEL_Ptr(nullptr)
   , mState(aSrc.mState)
   , mThermalSystemPtr(nullptr)
   , mFiringTime(aSrc.mFiringTime)
   , mLastFiringStartTime(aSrc.mLastFiringStartTime)
   , mLastFiringEndTime(aSrc.mLastFiringEndTime)
   , mFiringUpdateInterval(aSrc.mFiringUpdateInterval)
   , mCoolingUpdateInterval(aSrc.mCoolingUpdateInterval)
   , mPostCoolingCapacity(aSrc.mPostCoolingCapacity)
   , mEfficiency(aSrc.mEfficiency)
   , mLowTempLimit(aSrc.mLowTempLimit)
   , mHighTempLimit(aSrc.mHighTempLimit)
   , mPowerDissipation(aSrc.mPowerDissipation)
   , mNumberOfShots(aSrc.mNumberOfShots)
   , mCurrentShotNumber(0)
   , mShotId(0)
   , mCoolingTime(aSrc.mCoolingTime)
   , mLastCoolingStartTime(aSrc.mLastCoolingStartTime)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mThermalSystemName(aSrc.mThermalSystemName)
   , mAtThermalLimit(aSrc.mAtThermalLimit)
   , mCommentsEnabled(aSrc.mCommentsEnabled)
{
   if (aSrc.mHEL_Ptr != nullptr)
   {
      SetHEL(static_cast<WsfHighEnergyLaser*>(aSrc.mHEL_Ptr->Clone()));
   }
   if (aSrc.mThermalSystemPtr != nullptr)
   {
      mThermalSystemPtr = new WsfThermalSystem(*(aSrc.mThermalSystemPtr));
   }
}

// virtual
WsfLaserWeapon::~WsfLaserWeapon()
{
   // Note: The base class destroys mHEL_Ptr as mBeamPtr;
   // Note: The platform destroys the thermal system as an owned component.
   if ((GetPlatform() == nullptr) || ((GetPlatform() != nullptr) && (!GetPlatform()->IsInitialized())))
   {
      delete mThermalSystemPtr;
   }
}

// virtual
WsfWeapon* WsfLaserWeapon::Clone() const
{
   return new WsfLaserWeapon(*this);
}

//! Set the HEL ptr, and establish it as the base class' "beam" pointer.
void WsfLaserWeapon::SetHEL(WsfHighEnergyLaser* aHEL_Ptr)
{
   mHEL_Ptr = aHEL_Ptr;
   SetBeam(mHEL_Ptr); // Base class assumes responsibility for mHEL_Ptr.
}

// virtual
bool WsfLaserWeapon::PreInitialize(double aSimTime)
{
   bool ok = true;
   if (mThermalSystemName != 0) // defined on the platform.
   {
      mThermalSystemPtr = static_cast<WsfThermalSystem*>(GetPlatform()->FindComponent(mThermalSystemName));
      if (mThermalSystemPtr == nullptr)
      {
         auto out = ut::log::info() << "Thermal system not defined.";
         out.AddNote() << "Name: " << mThermalSystemName;
         ok = false;
      }
   }
   else if (mThermalSystemPtr != nullptr)
   {
      std::string name = GetName() + "_thermal_system";
      mThermalSystemPtr->SetName(name);
      ok = GetPlatform()->AddComponent(mThermalSystemPtr);
   }

   if (ok && (mThermalSystemPtr != nullptr))
   {
      mPowerDissipation = mHEL_Ptr->GetInitialPower() * (1.0 / mEfficiency - 1);

      if (mThermalSystemPtr->IsSimple())
      {
         mThermalSystemPtr->SetPowerDissipated(mPowerDissipation);
         if (mLowTempLimit > 0.0)
         {
            mThermalSystemPtr->SetLowTemperature(mLowTempLimit);
         }
         if (mHighTempLimit > 0.0)
         {
            mThermalSystemPtr->SetHighTemperature(mHighTempLimit);
         }
      }
   }

   return ok;
}

// virtual
bool WsfLaserWeapon::Initialize(double aSimTime)
{
   bool ok = true;

   if (mFiringTime > 0.0) // Firing time was set.
   {
      if (mFiringUpdateInterval == 0.0)
      {
         mFiringUpdateInterval = mFiringTime / 10.0;
      }
   }
   else
   {
      mFiringTime = 1.0e+6; // high value; user must end all shots.
      if (mFiringUpdateInterval == 0.0)
      {
         mFiringUpdateInterval = 0.1;
      }
   }

   if (mCoolingUpdateInterval == 0.0)
   {
      mCoolingUpdateInterval = 10.0; // 10 s default to evaulate cooling
   }

   ok &= WsfWeapon::Initialize(aSimTime);
   ok &= mHEL_Ptr->Initialize(this);

   return ok;
}

// virtual
bool WsfLaserWeapon::Initialize2(double aSimTime)
{
   if (mThermalSystemPtr != nullptr)
   {
      if (DebugEnabled())
      {
         mThermalSystemPtr->PrintInfo(mLowTempLimit, mHighTempLimit);
      }

      // Ignore number of shots
      mNumberOfShots = cUNLIMITED_SHOTS;

      // Total firing time comes directly from the thermal system.
      if (mPowerDissipation > mThermalSystemPtr->GetHeatSink())
      {
         mTotalFiringTime = mThermalSystemPtr->GetEnergy(mLowTempLimit, mHighTempLimit) /
                            (mPowerDissipation - mThermalSystemPtr->GetHeatSink());
      }

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Total firing time:";
         out.AddNote() << "Time: " << mTotalFiringTime << " sec";
      }
   }

   return true;
}

// virtual
bool WsfLaserWeapon::ProcessInput(UtInput& aInput)
{
   bool        processed = true;
   std::string command   = aInput.GetCommand();

   if (command == "firing_time")
   {
      aInput.ReadValueOfType(mFiringTime, UtInput::cTIME);
   }
   else if (command == "firing_update_interval")
   {
      aInput.ReadValueOfType(mFiringUpdateInterval, UtInput::cTIME);
   }
   else if (command == "cooling_update_interval")
   {
      aInput.ReadValueOfType(mCoolingUpdateInterval, UtInput::cTIME);
   }
   else if (command == "number_of_shots")
   {
      aInput.ReadValue(mNumberOfShots);
   }
   else if (command == "cooling_time")
   {
      aInput.ReadValueOfType(mCoolingTime, UtInput::cTIME);
      aInput.ValueGreater(mCoolingTime, 0.0);
   }
   else if (command == "efficiency")
   {
      aInput.ReadValue(mEfficiency);
      aInput.ValueInClosedRange(mEfficiency, 0.0, 1.0);
   }
   else if (command == "high_temperature_limit")
   {
      aInput.ReadValueOfType(mHighTempLimit, UtInput::cTEMPERATURE);
   }
   else if (command == "low_temperature_limit")
   {
      aInput.ReadValueOfType(mLowTempLimit, UtInput::cTEMPERATURE);
   }
   else if (command == "thermal_system")
   {
      // inline definition
      if (mThermalSystemPtr == nullptr)
      {
         mThermalSystemPtr  = new WsfThermalSystem(GetScenario());
         mThermalSystemName = nullptr;
      }
      UtInputBlock block(aInput, "end_thermal_system");
      block.ProcessInput(mThermalSystemPtr);
   }
   else if (command == "use_thermal_system")
   {
      if (mThermalSystemPtr != nullptr)
      {
         delete mThermalSystemPtr;
         mThermalSystemPtr = nullptr;
      }
      std::string typeName;
      aInput.ReadCommand(typeName);
      mThermalSystemName = typeName;
   }
   else if (command == "minimum_total_firing_time") // Used to limit ability to shoot before thermal re-charge.
   {
      aInput.ReadValueOfType(mPostCoolingCapacity, UtInput::cTIME);
   }
   else if (command == "fluence_model")
   {
      std::string propType;
      aInput.ReadValue(propType);
      if (!WsfHighEnergyLaserTypes::Get(GetScenario()).IsType(propType))
      {
         throw UtInput::BadValue(aInput);
      }
      // mHEL_Type = WsfStringId(propType);
      SetHEL(WsfHighEnergyLaserTypes::Get(GetScenario()).Create(propType));

      static const std::string blockTerminator = "end_fluence_model";
      aInput.ReadCommand(command);
      while (command != blockTerminator)
      {
         if (!mHEL_Ptr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         aInput.ReadCommand(command);
      }
   }
   else if (command == "comments")
   {
      aInput.ReadValue(mCommentsEnabled);
   }
   // For disallowed commands, we want to "capture" them here, rather than let a
   // base class handle them. To avoid breaking backward compatibility in the middle
   // of a major release, a warning is issued rather than treating them as errors.
   else if ((command == "quantity") || (command == "maximum_quantity") || (command == "reload_increment") ||
            (command == "reload_inventory") || (command == "reload_threshold"))
   {
      ut::log::warning() << "The " << command << " command is incompatible with WSF_LASER_WEAPON (ignored).";
      double dummy;
      aInput.ReadValue(dummy);
   }
   else if (command == "reload_time")
   {
      ut::log::warning() << "The reload_time command is incompatible with WSF_LASER_WEAPON (ignored).";
      WsfRandomVariable dummy;
      dummy.ProcessInput(aInput, UtInput::cTIME);
   }
   else if (command == "inhibit_while_reloading")
   {
      ut::log::warning() << "The inhibit_while_reloading command is incompatible with WSF_LASER_WEAPON (ignored).";
      aInput.ReadBool();
   }
   else
   {
      processed = WsfWeapon::ProcessInput(aInput);
   }

   return processed;
}

// virtual
void WsfLaserWeapon::TurnOff(double aSimTime)
{
   CeaseFire(aSimTime);
   WsfWeapon::TurnOff(aSimTime);
}

// virtual
void WsfLaserWeapon::Update(double aSimTime)
{
   WsfDirectedEnergyWeapon::Update(aSimTime);
   EvaluateCooling(aSimTime);
   mLastUpdateTime = aSimTime;
}

// virtual
void WsfLaserWeapon::CeaseFire(double aSimTime)
{
   WsfImplicitWeapon::CeaseFire(aSimTime); // Removes the current engagement.
   if (mState == cFIRING)
   {
      EndShot(aSimTime);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "HEL: Turn-off.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         GetPlatform()->Comment(aSimTime, "HEL: turn-off");
      }
   }
}

// virtual
WsfWeapon::FireResult WsfLaserWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   FireResult result;

   FireTarget target = aTarget;

   if (aTarget.mTrackPtr != nullptr)
   {
      if (target.mTargetName.empty())
      {
         target.mTargetName = aTarget.mTrackPtr->GetTargetName();
      }
      if (target.mTargetOffset.empty())
      {
         if (aTarget.mTrackPtr->GetAuxDataConst().AttributeExists("TARGET_OFFSET"))
         {
            target.mTargetOffset = aTarget.mTrackPtr->GetAuxDataConst().GetString("TARGET_OFFSET");
         }
      }
   }

   if (!target.mTargetName.empty())
   {
      target.mTrackPtr = nullptr;
      bool canFire     = ((mState == cREADY_TO_FIRE) || (mState == cPAUSED));

      if (canFire && CanLase(aSimTime))
      {
         // update to the current time.
         Update(aSimTime);
         result = WsfDirectedEnergyWeapon::Fire(aSimTime, target, aSettings);
         if (result)
         {
            result.mSuccess = BeginShot(aSimTime);
         }
      }
   }
   return result;
}

bool WsfLaserWeapon::BeginShot(double aSimTime)
{
   bool begun = ((GetEngagement() != nullptr) && CanLase(aSimTime) && (mState != cFIRING));

   if (begun)
   {
      assert(!mHEL_Ptr->IsLasing());
      WsfWeaponEffects* weaponEffectsPtr = GetEngagement()->GetWeaponEffects();
      if (weaponEffectsPtr != nullptr)
      {
         // Set the damage radius.  The following std::max(...) call considers a default damage radius.
         // This is a legacy feature we still want to support.  The bucket size now ought to be associated with the weapon effects.
         mHEL_Ptr->SetDamageRadius(std::max(weaponEffectsPtr->GetDamageRadius(), mHEL_Ptr->GetDefaultDamageRadius()));
      }
      else
      {
         mHEL_Ptr->SetDamageRadius(mHEL_Ptr->GetDefaultDamageRadius()); // Support legacy operation for nonzero radius.
         // If zero, we use the beam size instead.
      }

      // Initiate firing the laser:
      mHEL_Ptr->BeginLasing(aSimTime, GetCurrentEngagement()->GetTargetPlatformIndex(), GetTargetOffset());

      if (mThermalSystemPtr != nullptr)
      {
         // Make sure thermal system has cooled for next shot.
         mThermalSystemPtr->TransferHeat(aSimTime, 0.0);
      }

      ++mCurrentShotNumber;
      mState = cFIRING;
      GetSimulation()->AddEvent(ut::make_unique<LaserEvent>(aSimTime + mFiringUpdateInterval,
                                                            GetPlatform()->GetIndex(),
                                                            this,
                                                            LaserEvent::cUPDATE_SHOT));

      GetSimulation()->AddEvent(
         ut::make_unique<LaserEvent>(aSimTime + mFiringTime, GetPlatform()->GetIndex(), this, LaserEvent::cEND_SHOT));
      mLastFiringStartTime = aSimTime;

      // Begin the shot and set up the engagement.
      WsfObserver::DirectedEnergyWeaponBeginShot(GetSimulation())(aSimTime, this, GetEngagement());

      // Update observer and send messages.
      WsfObserver::DirectedEnergyWeaponFired(GetSimulation())(aSimTime, this, GetEngagement());
   }
   else
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfLaserWeapon: Cannot fire.";

         if (mAtThermalLimit)
         {
            out.AddNote() << "Laser out of thermal constraints.";
         }
         else
         {
            out.AddNote() << "Laser already firing.";
         }
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
   }

   return begun;
}

// virtual
bool WsfLaserWeapon::UpdateShot(double aSimTime)
{
   bool success = (mState == cFIRING);
   if (success)
   {
      assert(mHEL_Ptr->IsLasing());
      mHEL_Ptr->UpdateLasing(aSimTime);
      assert(GetEngagement() != nullptr);
      Update(aSimTime);
      WsfObserver::DirectedEnergyWeaponUpdateShot(GetSimulation())(aSimTime, this, GetEngagement());
      WsfObserver::DirectedEnergyWeaponFired(GetSimulation())(aSimTime, this, GetEngagement());
   }
   mLastUpdateTime = aSimTime;
   return success;
}

void WsfLaserWeapon::EvaluateCooling(double aSimTime)
{
   if (mThermalSystemPtr != nullptr) // Using thermal system
   {
      double currentPowerDissipation = 0.0;
      if (mState == cFIRING)
      {
         currentPowerDissipation = mPowerDissipation;
      }

      // Only fire if the thermal system allows it.
      // If no thermal system is configured, one can always fire.
      double dt = aSimTime - mLastUpdateTime;
      mThermalSystemPtr->TransferHeat(aSimTime, currentPowerDissipation * dt);
      double temp = mThermalSystemPtr->GetTemperature();

      if ((!mAtThermalLimit) && ((temp < mLowTempLimit) || (temp > mHighTempLimit)))
      {
         mAtThermalLimit = true;
         CeaseFire(aSimTime);

         // Periodically re-evaluate the ability of the laser to fire.
         GetSimulation()->AddEvent(ut::make_unique<LaserEvent>(aSimTime + mCoolingUpdateInterval,
                                                               GetPlatform()->GetIndex(),
                                                               this,
                                                               LaserEvent::cEVALUATE_COOLING));

         // Make sure the thermal system reflects the cutoff.
         if (mThermalSystemPtr->GetTemperature() < mLowTempLimit)
         {
            mThermalSystemPtr->SetTemperature(mLowTempLimit);
         }
         else
         {
            mThermalSystemPtr->SetTemperature(mHighTempLimit);
         }

         // Print or comment.
         if (DebugEnabled() || mCommentsEnabled)
         {
            std::ostringstream oss;
            oss << aSimTime << " high energy laser thermal system cutoff; temp=" << mThermalSystemPtr->GetTemperature();
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "High energy laser thermal system cutoff.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Temperature: " << mThermalSystemPtr->GetTemperature();
            }
            if (mCommentsEnabled)
            {
               GetPlatform()->Comment(aSimTime, oss.str());
            }
         }
      }
   }
   else if ((!mAtThermalLimit) && (GetQuantityRemaining() <= 0.0)) // Using set total firing time (not using thermal system)
   {
      mAtThermalLimit = true;
      CeaseFire(aSimTime);
      mLastCoolingStartTime = aSimTime;

      // Periodically re-evaluate the ability of the laser to fire.
      GetSimulation()->AddEvent(
         ut::make_unique<LaserEvent>(aSimTime + mCoolingTime, GetPlatform()->GetIndex(), this, LaserEvent::cEVALUATE_COOLING));
   }
}

// If the laser is engaging a target, end the shot.
//@return true, if the laser is engaging; false otherwise.
bool WsfLaserWeapon::EndShot(double aSimTime)
{
   assert(mHEL_Ptr->IsLasing());

   bool success = (mState == cFIRING);
   if (success)
   {
      if (DebugEnabled() || mCommentsEnabled)
      {
         std::ostringstream oss;
         oss << "T: " << aSimTime << " " << GetPlatform()->GetName()
             << ":WsfLaserWeapon: End shot: " << mCurrentShotNumber << " of " << mNumberOfShots << ".";
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfLaserWeapon: End shot:";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Current shot number: " << mCurrentShotNumber << "/ " << mNumberOfShots;
         }
         if (mCommentsEnabled)
         {
            GetPlatform()->Comment(aSimTime, oss.str());
         }
      }

      mHEL_Ptr->UpdateLasing(aSimTime);
      mState = cREADY_TO_FIRE;
      ++mShotId; // Update the shot id here so that a shot won't be ended twice.
      mHEL_Ptr->EndLasing(aSimTime);
      WsfObserver::DirectedEnergyWeaponFired(GetSimulation())(aSimTime, this, GetEngagement());
      if ((GetEngagement() != nullptr) && (!GetEngagement()->IsComplete()))
      {
         // The following call to "Terminate" triggers WeaponTerminated callback.
         // GetEngagement()->Terminate(aSimTime, WsfWeaponEngagement::cTARGET_IMPACT);
         ClearEngagement(aSimTime);
      }
      WsfObserver::DirectedEnergyWeaponEndShot(GetSimulation())(aSimTime, this, GetEngagement());
   }

   return success;
}

// virtual
WsfEvent::EventDisposition WsfLaserWeapon::LaserEvent::Execute()
{
   WsfEvent::EventDisposition disposition = WsfEvent::cDELETE;

   if (GetSimulation()->PlatformExists(mParentPlatformIndex))
   {
      if (mEventId == cDONE_COOLING)
      {
         mWsfLaserWeaponPtr->mCurrentShotNumber = 0;
      }
      else if (mEventId == cUPDATE_SHOT)
      {
         if (mWsfLaserWeaponPtr->IsFiring())
         {
            if ((mWsfLaserWeaponPtr->mShotId == mShotId) && (mWsfLaserWeaponPtr->UpdateShot(GetTime())))
            {
               // reschedule this event.
               disposition = WsfEvent::cRESCHEDULE;
               SetTime(GetTime() + mWsfLaserWeaponPtr->mFiringUpdateInterval);
            }
         }
      }
      else if (mEventId == cEND_SHOT)
      {
         // Only end the current shot.
         if (mWsfLaserWeaponPtr->mShotId == mShotId)
         {
            mWsfLaserWeaponPtr->EndShot(GetTime());
         }
      }
      else if (mEventId == cEVALUATE_COOLING)
      {
         if (mWsfLaserWeaponPtr->mThermalSystemPtr != nullptr)
         {
            mWsfLaserWeaponPtr->Update(GetTime());

            if (mWsfLaserWeaponPtr->GetTotalFiringTimeRemaining(GetTime()) > mWsfLaserWeaponPtr->mPostCoolingCapacity)
            {
               mWsfLaserWeaponPtr->mAtThermalLimit = false;
               WsfObserver::DirectedEnergyWeaponCooldownComplete(GetSimulation())(GetTime(), mWsfLaserWeaponPtr);
            }
            else
            {
               // reschedule this event.
               disposition = WsfEvent::cRESCHEDULE;
               SetTime(GetTime() + mWsfLaserWeaponPtr->mCoolingUpdateInterval);
            }
         }
         else
         {
            // Cooling interval has elapsed; reset number of shots:
            mWsfLaserWeaponPtr->mCurrentShotNumber = 0;
            mWsfLaserWeaponPtr->mAtThermalLimit    = false;
            WsfObserver::DirectedEnergyWeaponCooldownComplete(GetSimulation())(GetTime(), mWsfLaserWeaponPtr);
         }
      }
   }
   return disposition;
}

//! Return whether the laser is currently firing.
bool WsfLaserWeapon::IsFiring() const
{
   return (mState == cFIRING);
}

double WsfLaserWeapon::GetCoolingTime() const
{
   return mCoolingTime;
}

double WsfLaserWeapon::GetCoolingTimeRemaining() const
{
   double coolingTime = 0.0;
   if (mCoolingTime > 0.0)
   {
      coolingTime = mCoolingTime - (GetSimulation()->GetSimTime() - mLastCoolingStartTime);
   }
   return coolingTime;
}

//! Return the total firing time per shot
double WsfLaserWeapon::GetFiringTime() const
{
   return mFiringTime;
}

//! Return the firing time remaining, if the laser is currently firing.
//! If the laser is not firing, return the firing time.
double WsfLaserWeapon::GetFiringTimeRemaining() const
{
   if (IsFiring())
   {
      return (mFiringTime - (GetSimulation()->GetSimTime() - mLastFiringStartTime));
   }
   else
   {
      return 0.0;
   }
}

//! Return the total lasing time remaining.  If the thermal system is being utilized, this is the
//! time until the high temperature limit is reached.  Otherwise it is the standard salvo time.
double WsfLaserWeapon::GetTotalFiringTimeRemaining(double aSimTime) const
{
   double timeRemaining = 10000.0;
   double dt            = aSimTime - mLastUpdateTime; // Time since last shot (could be 0).
   if (mThermalSystemPtr != nullptr && (mHighTempLimit > 0.0))
   {
      double heatCapacity = mThermalSystemPtr->GetRequiredHeatForTemperature(mHighTempLimit);
      if (mPowerDissipation > mThermalSystemPtr->GetHeatSink())
      {
         timeRemaining = (heatCapacity + mThermalSystemPtr->GetHeatSink() * dt) /
                         (mPowerDissipation - mThermalSystemPtr->GetHeatSink());
         if (timeRemaining > mTotalFiringTime)
         {
            timeRemaining = mTotalFiringTime;
         }
      }
   }
   else if (mFiringTime > 0.0)
   {
      timeRemaining = (mNumberOfShots - mCurrentShotNumber) * mFiringTime + GetFiringTimeRemaining();
   }
   return timeRemaining;
}

// virtual
//! Return whether the weapon is within physical constraints to lase.
//! This **does not** determine whether we have decided to lase
//! (see controller variables in header file).
//! (thermal and possibly, other constraints) to lase at the current time.
bool WsfLaserWeapon::CanLase(double aSimTime)
{
   EvaluateCooling(aSimTime);
   return (!mAtThermalLimit);
}

// virtual
//! Return the shot "quantity" remaining.
//! As the firing time is variable, we simply return the remaining firing time.
double WsfLaserWeapon::GetQuantityRemaining() const
{
   return GetTotalFiringTimeRemaining(mLastUpdateTime);
}

//! Set the firing time to the specified value
//!@note This method is used with the API to ensure long firing time,
//! so that the laser does not automatically end a shot.
void WsfLaserWeapon::SetFiringTime(double aFiringTime)
{
   mFiringTime = aFiringTime;
   if (mThermalSystemPtr != nullptr)
   {
      // Make the simple input consistent.
      mNumberOfShots     = static_cast<int>(mTotalFiringTime / mFiringTime);
      mCurrentShotNumber = static_cast<int>((mTotalFiringTime - GetFiringTimeRemaining()) / mTotalFiringTime) + 1;
   }
}

const char* WsfLaserWeapon::GetScriptClassName() const
{
   return "WsfLaserWeapon";
}
