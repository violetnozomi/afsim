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

#include "WsfWeaponPlatformExtension.hpp"

#include <cassert>
#include <iostream>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEngagement.hpp"

namespace
{
//! Component factory to process platform input.
class WeaponPlatformComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override;
};

// =================================================================================================
bool WeaponPlatformComponentFactory::ProcessInput(UtInput& aInput, WsfPlatform& aPlatform)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if ((command == "weapon_effects") || (command == "weapon_effects_type"))
   {
      WsfVariable<WsfStringId> effectType;
      effectType.ReadValue(aInput);
      WsfWeaponPlatformExtension::FindOrCreate(aPlatform).SetWeaponEffectsType(effectType);
   }
   else if (aInput.GetCommand() == "on_death")
   {
      std::string action;
      aInput.ReadValue(action);
      if (action == "remove")
      {
         aPlatform.SetOnBrokenAction(WsfPlatform::cON_BROKEN_REMOVE);
      }
      else if (action == "disable")
      {
         aPlatform.SetOnBrokenAction(WsfPlatform::cON_BROKEN_DISABLE);
      }
      else if (action == "disabled_but_movable")
      {
         aPlatform.SetOnBrokenAction(WsfPlatform::cON_BROKEN_MOVABLE_DISABLED);
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}
} // namespace

// =================================================================================================
void WsfWeaponPlatformExtension::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<WeaponPlatformComponentFactory>());
}

// =================================================================================================
// static
WsfStringId WsfWeaponPlatformExtension::GetWeaponEffectsType(WsfPlatform& aPlatform)
{
   WsfWeaponPlatformExtension* extensionPtr = Find(aPlatform);
   if (extensionPtr != nullptr)
   {
      return extensionPtr->GetWeaponEffectsType();
   }
   return WsfStringId();
}

// =================================================================================================
// static
WsfWeaponEngagement* WsfWeaponPlatformExtension::GetWeaponEngagement(const WsfPlatform& aPlatform)
{
   WsfWeaponPlatformExtension* extensionPtr = Find(aPlatform);
   if (extensionPtr != nullptr)
   {
      return extensionPtr->GetWeaponEngagement();
   }
   return nullptr;
}

// =================================================================================================
// static
bool WsfWeaponPlatformExtension::SetWeaponEngagement(WsfPlatform& aPlatform, WsfWeaponEngagement* aEngagementPtr)
{
   // If the request is to clear the engagement then don't create the extension for no reason.
   bool assigned = true;
   if (aEngagementPtr == nullptr)
   {
      WsfWeaponPlatformExtension* extensionPtr = Find(aPlatform);
      if (extensionPtr != nullptr)
      {
         extensionPtr->SetWeaponEngagement(nullptr);
      }
   }
   else
   {
      assigned = FindOrCreate(aPlatform).SetWeaponEngagement(aEngagementPtr);
   }
   return assigned;
}

// =================================================================================================
WsfWeaponPlatformExtension::WsfWeaponPlatformExtension()
   : WsfNamed("#weapon_platform_extension")
   , mPlatformPtr(nullptr)
   , mWeaponEffectsType()
   , mEngagementPtr(nullptr)
{
}

// =================================================================================================
WsfWeaponPlatformExtension::WsfWeaponPlatformExtension(const WsfWeaponPlatformExtension& aSrc)
   : WsfNamed(aSrc)
   , mPlatformPtr(nullptr)
   , mWeaponEffectsType(aSrc.mWeaponEffectsType)
   , mEngagementPtr(nullptr)
{
}

// =================================================================================================
WsfWeaponPlatformExtension::~WsfWeaponPlatformExtension()
{
   WsfWeaponEngagement::Delete(mEngagementPtr);
}

// =================================================================================================
const int* WsfWeaponPlatformExtension::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_WEAPON_PLATFORM_EXTENSION, 0};
   return roles;
}

// =================================================================================================
void* WsfWeaponPlatformExtension::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_WEAPON_PLATFORM_EXTENSION)
      return this;
   return nullptr;
}

// =================================================================================================
bool WsfWeaponPlatformExtension::Initialize(double aSimTime)
{
   assert(mPlatformPtr != nullptr);
   bool ok(true);

   // If this platform is the weapon in a weapon engagement then (re)tell the engagement
   // about the weapon.  See WsfWeaponEngagement::SetWeaponPlatform() for why this is needed.

   if (mEngagementPtr != nullptr)
   {
      ok &= mEngagementPtr->Initialize(aSimTime, mPlatformPtr);
   }

   // Weapon effects types uses global context for consistency with GetWeaponEffectsType.
   ok &= mWeaponEffectsType.Initialize("weapon_effects",
                                       mPlatformPtr,
                                       nullptr,
                                       mPlatformPtr->GetSimulation()->GetScriptContext());
   return ok;
}

// =================================================================================================
WsfStringId WsfWeaponPlatformExtension::GetWeaponEffectsType()
{
   assert(mPlatformPtr != nullptr);

   // NOTE: This is sometimes called before initialization of the platform (e.g.: by WsfExplicitWeapon).
   // Therefore, the object is initialized using the global context as the local context may not yet
   // be initialized!

   if (mWeaponEffectsType.IsReference())
   {
      // If it is a reference then an copy is initialized. This way if there is a problem then normal
      // initialization will detect the problem (we don't have a return that indicates a failure).

      WsfVariable<WsfStringId> weaponEffectsType(mWeaponEffectsType);
      if (weaponEffectsType.Initialize("weapon_effects", nullptr, nullptr, mPlatformPtr->GetSimulation()->GetScriptContext()))
      {
         return weaponEffectsType;
      }
      return WsfStringId(); // Reference could not be initialized
   }

   // The weapon effects type was a constant value OR a reference that has already been resolved.
   // Just return the value.
   return mWeaponEffectsType;
}

// =================================================================================================
//! Attach the weapon engagement object to the platform.
//! Any existing weapon engagement object is simply deleted.
//! Any required initialization of the weapon engagement object will be performed.
//!
//! @param aEngagementPtr Pointer to the engagement object. The platform assumes ownership of the object.
//! @returns true if successful or false if the weapon engagement could not be initialized. If false
//! is returned then the engagement object is deleted.
//! @note This is really only useful if the platform represents a weapon.
bool WsfWeaponPlatformExtension::SetWeaponEngagement(WsfWeaponEngagement* aEngagementPtr)
{
   assert(mPlatformPtr != nullptr);
   bool ok = true;
   if (aEngagementPtr != mEngagementPtr)
   {
      // We have to be a little careful to prevent infinite recursion between this method and
      // WsfWeaponEngagement::Delete. If the engagement being deleted has been assigned to me, then
      // I own the object and am responsible for deleting it (by calling Delete(0)). Delete sees
      // if the argument is actively owned by a platform, if so calls SetWeaponEngagement. This
      // causes the recursion. To break this we release our ownership prior to calling Delete.
      WsfWeaponEngagement* currentEngagementPtr = mEngagementPtr;
      mEngagementPtr                            = nullptr; // Release our ownership so Delete will delete.
      WsfWeaponEngagement::Delete(currentEngagementPtr);
      mEngagementPtr = aEngagementPtr;       // Take ownership of the new engagement
      if ((mPlatformPtr->GetIndex() != 0) && // Platform has been initialized
          (mEngagementPtr != nullptr) && (!mEngagementPtr->Initialize(mPlatformPtr)))
      {
         SetWeaponEngagement(nullptr); // Failed to initialize, so delete it.
         auto out = ut::log::error() << "Weapon Platform Extension: Unable to initialize weapon engagement.";
         out.AddNote() << "Extension: " << GetName();

         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
WsfWeaponPlatformExtension* WsfWeaponPlatformExtension::Find(const WsfPlatform& aPlatform)
{
   WsfWeaponPlatformExtension* extensionPtr(nullptr);
   aPlatform.FindByRole(extensionPtr);
   return extensionPtr;
}

// =================================================================================================
WsfWeaponPlatformExtension& WsfWeaponPlatformExtension::FindOrCreate(WsfPlatform& aPlatform)
{
   WsfWeaponPlatformExtension* extensionPtr = Find(aPlatform);
   if (extensionPtr == nullptr)
   {
      extensionPtr = new WsfWeaponPlatformExtension;
      aPlatform.AddComponent(extensionPtr);
   }
   return *extensionPtr;
}
