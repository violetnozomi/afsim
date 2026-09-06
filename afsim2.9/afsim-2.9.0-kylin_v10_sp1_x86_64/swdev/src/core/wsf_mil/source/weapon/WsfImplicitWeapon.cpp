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

#include "WsfImplicitWeapon.hpp"

#include <iostream>

#include "UtMemory.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLaunchComputer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

WsfImplicitWeapon::WsfImplicitWeapon(const WsfScenario& aScenario)
   : WsfWeapon(aScenario)
   , mEngagementDataMap()
   , mCurrentEngagementDataPtr(nullptr)
{
}

WsfImplicitWeapon::WsfImplicitWeapon(const WsfImplicitWeapon& aSrc)
   : WsfWeapon(aSrc)
   , mEngagementDataMap()
   , mCurrentEngagementDataPtr(nullptr)
{
}

// virtual
WsfImplicitWeapon::~WsfImplicitWeapon()
{
   std::map<int, EngagementData*>::iterator iter = mEngagementDataMap.begin();
   while (iter != mEngagementDataMap.end())
   {
      if (!iter->second->mTerminateSet)
      {
         delete iter->second;
      }
      ++iter;
   }
}

// virtual
WsfWeapon* WsfImplicitWeapon::Clone() const
{
   return new WsfImplicitWeapon(*this);
}

// virtual
bool WsfImplicitWeapon::Initialize(double aSimTime)
{
   // The weapon quantity setting defaults to zero in WsfWeapon.  Unless quantity is
   // specified for an implicit weapon, change the default value to the maximum_quantity
   // setting.
   if (!IsQuantitySpecified())
   {
      SetQuantityRemaining(GetMaximumQuantity());
   }

   return WsfWeapon::Initialize(aSimTime);
}

// virtual
WsfWeapon::FireResult WsfImplicitWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   FireResult result;

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Attempting implicit weapon fire:";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << GetName();
   }

   // The following are required to fire an implicit weapon:
   // 1.  Quantity > 0
   // 2.  Enough elapsed time from the last fire
   // 3.  Not inhibited while reloading
   // 4.  Weapon damage < 1.0

   if (IsReloading() && InhibitWhileReloading())
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "Unable to fire because a reload is in progress.";
      }
   }
   else if (GetQuantityRemaining() <= 0.0)
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "Unable to fire because quantity is zero.";
      }
   }
   else if (GetTimeLastFired() + GetFiringInterval() > aSimTime)
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "Unable to fire because the firing interval has not elapsed.";
      }
   }
   else if (GetDamageFactor() >= 1.0)
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "Unable to fire because the DamageFactor is 1.0.";
      }
   }
   else
   {
      EngagementData* engagementDataPtr = new EngagementData();
      if (aTarget.mTrackPtr != nullptr || !aTarget.mTargetName.empty())
      {
         if (aTarget.mTrackPtr != nullptr)
         {
            engagementDataPtr->mTargetTrackPtr.reset(aTarget.mTrackPtr->Clone());
            engagementDataPtr->mTargetName = aTarget.mTrackPtr->GetTargetName();
         }
         if (!aTarget.mTargetName.empty())
         {
            engagementDataPtr->mTargetName = aTarget.mTargetName;
         }
         engagementDataPtr->mTargetOffset = aTarget.mTargetOffset;

         result.mSuccess = BeginEngagement(aSimTime, engagementDataPtr);
      }
      else if (GetPlatform()->GetTrackManager().GetCurrentTarget() != nullptr)
      {
         engagementDataPtr->mTargetTrackPtr.reset(GetPlatform()->GetTrackManager().GetCurrentTarget()->Clone());
         result.mSuccess = BeginEngagement(aSimTime, engagementDataPtr);
      }

      if (!result.mSuccess)
      {
         delete engagementDataPtr;
      }
      else
      {
         result.mEngagementPtr = engagementDataPtr->mEngagementPtr;
      }
   }

   return result;
}

// Private
// Handle common fire logic.
bool WsfImplicitWeapon::BeginEngagement(double aSimTime, EngagementData* aEngagementDataPtr)
{
   bool success = ((aEngagementDataPtr->mTargetName != 0) || ((aEngagementDataPtr->mTargetTrackPtr != nullptr) &&
                                                              aEngagementDataPtr->mTargetTrackPtr->LocationValid()));
   if (success)
   {
      size_t       targetIndex = 0;
      WsfPlatform* targetPtr   = nullptr;

      double targetLocWCS[3];

      if (aEngagementDataPtr->mTargetTrackPtr != nullptr)
      {
         aEngagementDataPtr->mTargetName = aEngagementDataPtr->mTargetTrackPtr->GetTargetName();

         // TODO, get target offset as a new field in WsfTrack.
         // Until then we are using a piece of "standard" aux data
         if (aEngagementDataPtr->mTargetTrackPtr->GetAuxDataConst().AttributeExists("TARGET_OFFSET"))
         {
            aEngagementDataPtr->mTargetOffset =
               aEngagementDataPtr->mTargetTrackPtr->GetAuxDataConst().GetString("TARGET_OFFSET");
         }

         aEngagementDataPtr->mTargetTrackPtr->GetLocationWCS(targetLocWCS);
         if (!aEngagementDataPtr->mTargetName.IsNull())
         {
            targetPtr = GetSimulation()->GetPlatformByName(aEngagementDataPtr->mTargetName);
            if (targetPtr != nullptr)
            {
               targetIndex                      = targetPtr->GetIndex();
               aEngagementDataPtr->mTargetIndex = targetIndex;
            }
         }
      }
      else if (!aEngagementDataPtr->mTargetName.IsNull())
      {
         targetPtr = GetSimulation()->GetPlatformByName(aEngagementDataPtr->mTargetName);
         if (targetPtr != nullptr)
         {
            // Make a track with the truth location of the target.
            targetIndex                         = targetPtr->GetIndex();
            aEngagementDataPtr->mTargetTrackPtr = ut::make_unique<WsfTrack>();
            aEngagementDataPtr->mTargetTrackPtr->SetTargetName(aEngagementDataPtr->mTargetName);
            aEngagementDataPtr->mTargetTrackPtr->SetTargetIndex(targetIndex);
            aEngagementDataPtr->mTargetTrackPtr->SetTargetType(targetPtr->GetTypeId());
            targetPtr->GetLocationWCS(targetLocWCS);
            aEngagementDataPtr->mTargetTrackPtr->SetLocationWCS(targetLocWCS);
            aEngagementDataPtr->mTargetTrackPtr->Initialize(aSimTime, GetPlatform()->GetNextTrackId(), *GetSimulation());
         }
         else // this target no longer exists
         {
            success = false;
         }
      }

      if (success)
      {
         aEngagementDataPtr->mTargetTrackPtr->SetLocationWCS(targetLocWCS);
         WsfWeaponEngagement* weaponEngagementPtr =
            new WsfWeaponEngagement(aSimTime,
                                    GetSimulation(),
                                    this,
                                    aEngagementDataPtr->mTargetTrackPtr->GetTrackId(),
                                    targetIndex,
                                    GetWeaponEffectsType(),
                                    aEngagementDataPtr->mTargetOffset);

         WsfTrackId targetId(aEngagementDataPtr->mTargetTrackPtr->GetTrackId());
         auto       it = mWeaponStatusMap.find(targetId);
         if (it != mWeaponStatusMap.end())
         {
            if (it->second.mRoundsPending > 0)
            {
               it->second.mRoundsPending--;
            }
            it->second.mRoundsActive++;
            it->second.mTimeWeaponLastFired = aSimTime;
         }

         // If there is a launch computer, use it to set the completion time of the engagement
         if (GetLaunchComputer() != nullptr)
         {
            double tti =
               GetLaunchComputer()->EstimatedTimeToIntercept(aSimTime, *aEngagementDataPtr->mTargetTrackPtr, 0.0);
            weaponEngagementPtr->SetCompletionTime(aSimTime + tti);
            // velocity estimate from range / time.
            // This velocity can be used to provide an initial bearing of the weapon and / or an impact estimate.
            double velWCS[3];
            double myLocWCS[3];
            GetPlatform()->GetLocationWCS(myLocWCS);
            UtVec3d::Subtract(velWCS, targetLocWCS, myLocWCS);
            UtVec3d::Divide(velWCS, tti);
            weaponEngagementPtr->SetInitialWeaponVelocityWCS(velWCS);
         }

         // Set the "initial target location," which is used later for miss distance and lethality calculations.
         weaponEngagementPtr->SetInitialTargetLocationWCS(targetLocWCS);

         success = weaponEngagementPtr->Initialize(nullptr); // This is an implicit engagement
                                                             // so there is no weapon platform.

         if (success)
         {
            // If there is a launch computer, use it to set the completion time of the engagement
            if (GetLaunchComputer() != nullptr)
            {
               GetSimulation()->AddEvent(ut::make_unique<TerminateEngagementEvent>(weaponEngagementPtr->GetCompletionTime(),
                                                                                   GetPlatform()->GetIndex(),
                                                                                   this,
                                                                                   aEngagementDataPtr));
               aEngagementDataPtr->mTerminateSet = true;
            }

            // Transfer control of the engagement object
            aEngagementDataPtr->mEngagementPtr = weaponEngagementPtr;

            aEngagementDataPtr->mEngagementId = weaponEngagementPtr->GetSerialNumber();

            // Allow us to look up engagements by serial id.  This also stores the object pointer
            mEngagementDataMap[weaponEngagementPtr->GetSerialNumber()] = aEngagementDataPtr;

            // Manage quantity and time last fired
            ChangeQuantity(-1.0);
            SetTimeLastFired(aSimTime);
            ProcessReloading(aSimTime);

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Implicit weapon successfully fired:";
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Weapon: " << GetName();
            }

            // Update observers and send messages.
            WsfObserver::ImplicitWeaponBeginEngagement(GetSimulation())(aSimTime, this, aEngagementDataPtr->mEngagementPtr);

            // By rule, this becomes the current engagement data
            // SetCurrentEngagementData(aEngagementDataPtr);
            mCurrentEngagementDataPtr = aEngagementDataPtr;

            // Update observers and send messages.
            WsfObserver::WeaponFired(
               GetSimulation())(aSimTime, weaponEngagementPtr, aEngagementDataPtr->mTargetTrackPtr.get());
         }
         else
         {
            // Free memory allocated from the (rejected) engagement.
            WsfWeaponEngagement::Delete(weaponEngagementPtr);
         }
      }
   }
   return success;
}

//! Return the pointer to the requested engagement.
//! If the engagement does not exist, zero is returned.
//! If the passed argument is zero, or if there is no argument, the current engagement
//! is returned (if it exists).
//! @param aEngagementId The desired engagement id, if desired (if omitted the current engagement will be returned).
//! @return The engagement object corresponding with the given id, or zero if it does not exist.
WsfWeaponEngagement* WsfImplicitWeapon::GetEngagement(int aEngagementId) const // = 0
{
   WsfWeaponEngagement* engagementPtr = nullptr;
   if ((aEngagementId == 0) && (mCurrentEngagementDataPtr != nullptr))
   {
      engagementPtr = mCurrentEngagementDataPtr->mEngagementPtr;
   }
   else
   {
      EngagementDataMap::const_iterator iter = mEngagementDataMap.find(aEngagementId);
      if (iter != mEngagementDataMap.end())
      {
         engagementPtr = iter->second->mEngagementPtr;
      }
   }
   return engagementPtr;
}

//! Return the pointer to the current engagement, if any.
//! @return The current engagement object, if it exists.
WsfWeaponEngagement* WsfImplicitWeapon::GetCurrentEngagement() const
{
   return GetEngagement(0);
}

//! Clear the current engagement
void WsfImplicitWeapon::ClearCurrentEngagement(double aSimTime)
{
   ClearEngagement(aSimTime, 0);
}

//! Clear the current engagement.
void WsfImplicitWeapon::ClearEngagement(double aSimTime,
                                        int    aEngagementId) // = 0
{
   EngagementData* engDataPtr = nullptr;
   if ((aEngagementId == 0) && (mCurrentEngagementDataPtr != nullptr))
   {
      engDataPtr = mCurrentEngagementDataPtr;
   }
   else
   {
      EngagementDataMap::iterator iter = mEngagementDataMap.find(aEngagementId);
      if (iter != mEngagementDataMap.end())
      {
         engDataPtr = iter->second;
      }
   }
   if ((engDataPtr != nullptr) && (!engDataPtr->mTerminateSet))
   {
      // set up the event to terminate now
      GetSimulation()->AddEvent(
         ut::make_unique<TerminateEngagementEvent>(aSimTime, GetPlatform()->GetIndex(), this, engDataPtr));
   }
}

// virtual
//! Cease fire.  This method, for an implicit weapon, only has additional effect
//! if any of the associated engagements are not due to be terminated at a specific time.
void WsfImplicitWeapon::CeaseFire(double aSimTime)
{
   // See EndEngagement
   WsfWeapon::CeaseFire(aSimTime);

   // For any engagements not scheduled to be terminated, terminate and delete now:
   std::map<int, EngagementData*>::iterator iter = mEngagementDataMap.begin();
   while (iter != mEngagementDataMap.end())
   {
      if (!iter->second->mTerminateSet)
      {
         // set up the event to terminate now
         GetSimulation()->AddEvent(
            ut::make_unique<TerminateEngagementEvent>(aSimTime, GetPlatform()->GetIndex(), this, iter->second));
      }
      ++iter;
   }
}

//! override
//! Override of base class method; Apply terminal contitions to the effect.
void WsfImplicitWeapon::EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr)
{
   WsfPlatform* targetPlatformPtr = aEngagementPtr->GetTargetPlatform();

   if (targetPlatformPtr != nullptr)
   {
      // Provide target location and velocity as impact point
      double targetLocWCS[3];
      targetPlatformPtr->GetLocationWCS(targetLocWCS);
      aEngagementPtr->SetTargetLocationWCS(targetPlatformPtr, targetLocWCS);
      double weaponVelWCS[3];
      targetPlatformPtr->GetVelocityWCS(weaponVelWCS);
      aEngagementPtr->SetWeaponVelocityWCS(weaponVelWCS);
   }

   WsfWeapon::EngagementComplete(aSimTime, aEngagementPtr);
}

// virtual
void WsfImplicitWeapon::Update(double aSimTime)
{
   WsfWeapon::Update(aSimTime);
   std::map<int, EngagementData*>::iterator iter = mEngagementDataMap.begin();
   while (iter != mEngagementDataMap.end())
   {
      WsfWeaponEngagement* engagementPtr = iter->second->mEngagementPtr;
      // To avoid extra weapon hits / PK draws (see defect AFSIM-206), don't call
      // update at engagement start time or after TerminateEngagementEvent has been queued
      if (!iter->second->mTerminateSet && aSimTime > engagementPtr->GetStartTime())
      {
         engagementPtr->Update(aSimTime);
      }
      ++iter;
   }
}

//! Delete existing engagement data.  If this is the current engagement, set the pointer to the current
//! engagement to zero.
//!@param aEngagementDataPtr The engagement data to be deleted.
void WsfImplicitWeapon::DeleteEngagementData(EngagementData* aEngagementDataPtr)
{
   if (aEngagementDataPtr == mCurrentEngagementDataPtr)
   {
      mCurrentEngagementDataPtr = nullptr;
   }
   std::map<int, EngagementData*>::iterator iter = mEngagementDataMap.find(aEngagementDataPtr->mEngagementId);
   if (iter != mEngagementDataMap.end())
   {
      mEngagementDataMap.erase(iter);
   }
   delete aEngagementDataPtr;
}

// Protected
WsfEvent::EventDisposition WsfImplicitWeapon::TerminateEngagementEvent::Execute()
{
   WsfPlatform* firingPlatformPtr = GetSimulation()->GetPlatformByIndex(mWeaponPlatformIndex);
   if (firingPlatformPtr != nullptr) // Otherwise the platform has been killed, and the objects were already deleted.
   {
      WsfWeaponEngagement* engPtr = mEngagementDataPtr->mEngagementPtr;
      if (engPtr != nullptr) // should be true
      {
         WsfObserver::ImplicitWeaponEndEngagement(GetSimulation())(GetTime(), mWeaponPtr, engPtr);
         if (!engPtr->IsComplete())
         {
            // Terminate performs a final ProcessEffectImplicit call as in engPtr->Update
            engPtr->Terminate(GetTime());
         }
         mWeaponPtr->DeleteEngagementData(mEngagementDataPtr);
      }
   }
   else
   {
      // Take control and delete this ourselves
      delete mEngagementDataPtr;
   }
   return WsfEvent::cDELETE;
}

// Protected
WsfImplicitWeapon::TerminateEngagementEvent::TerminateEngagementEvent(double             aSimTime,
                                                                      size_t             aWeaponPlatformIndex,
                                                                      WsfImplicitWeapon* aImplicitWeaponPtr,
                                                                      EngagementData*&   aEngagementDataPtr)
   : WsfEvent(aSimTime)
   , mWeaponPlatformIndex(aWeaponPlatformIndex)
   , mWeaponPtr(aImplicitWeaponPtr)
   , mEngagementDataPtr(aEngagementDataPtr)
{
   aEngagementDataPtr->mTerminateSet = true;
}

//! Return the current target name.
WsfStringId WsfImplicitWeapon::GetTargetName() const
{
   WsfStringId targetName;
   if (mCurrentEngagementDataPtr != nullptr)
   {
      targetName = mCurrentEngagementDataPtr->mTargetName;
   }
   return targetName;
}

//! Return the name id of the target offset, if any.
WsfStringId WsfImplicitWeapon::GetTargetOffset() const
{
   WsfStringId targetIndex;
   if (mCurrentEngagementDataPtr != nullptr)
   {
      targetIndex = mCurrentEngagementDataPtr->mTargetOffset;
   }
   return targetIndex;
}

const char* WsfImplicitWeapon::GetScriptClassName() const
{
   return "WsfImplicitWeapon";
}

WsfImplicitWeapon::EngagementData::EngagementData(const WsfImplicitWeapon::EngagementData& aSrc)
   : mTargetName(aSrc.mTargetName)
   , mTargetIndex(aSrc.mTargetIndex)
   , mTargetOffset(aSrc.mTargetOffset)
   , mTargetTrackPtr(nullptr)
   , mEngagementPtr(nullptr)
   , mEngagementId(0)
   , mTerminateSet(aSrc.mTerminateSet)
{
   if (aSrc.mTargetTrackPtr != nullptr)
   {
      mTargetTrackPtr.reset(aSrc.mTargetTrackPtr->Clone());
   }
}

WsfImplicitWeapon::EngagementData::~EngagementData()
{
   WsfWeaponEngagement::Delete(mEngagementPtr);
}
