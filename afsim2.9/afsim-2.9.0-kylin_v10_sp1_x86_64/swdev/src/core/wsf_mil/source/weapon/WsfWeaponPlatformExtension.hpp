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

#ifndef WSFWEAPONPLATFORMEXTENSION_HPP
#define WSFWEAPONPLATFORMEXTENSION_HPP

#include "wsf_mil_export.h"

class UtInput;
#include "WsfComponent.hpp"
#include "WsfMilComponentRoles.hpp"
#include "WsfNamed.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfStringId.hpp"
#include "WsfVariable.hpp"
class WsfWeaponEngagement;

//! A platform component used to maintain weapon-related data.
class WsfWeaponPlatformExtension : public WsfNamed, public WsfPlatformComponent
{
public:
   static void RegisterComponentFactory(WsfScenario& aScenario);

   static WsfStringId          GetWeaponEffectsType(WsfPlatform& aPlatform);
   static WsfWeaponEngagement* GetWeaponEngagement(const WsfPlatform& aPlatform);
   static bool                 SetWeaponEngagement(WsfPlatform& aPlatform, WsfWeaponEngagement* aEngagementPtr);

   static WsfWeaponPlatformExtension& FindOrCreate(WsfPlatform& aPlatform);
   static WsfWeaponPlatformExtension* Find(const WsfPlatform& aPlatform);

   WsfWeaponPlatformExtension();
   WsfWeaponPlatformExtension(const WsfWeaponPlatformExtension& aSrc);
   ~WsfWeaponPlatformExtension() override;
   WsfWeaponPlatformExtension& operator=(const WsfWeaponPlatformExtension&) = delete;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return new WsfWeaponPlatformExtension(*this); }
   WsfStringId   GetComponentName() const override { return GetNameId(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   bool Initialize(double aSimTime) override;

   //! @name Weapon effects methods.
   //@{
   //! Get the weapon effects type for the platform.
   WsfStringId GetWeaponEffectsType();

   //! Set the name of the weapon effects associated with this platform.
   void SetWeaponEffectsType(WsfStringId aWeaponEffectsType) { mWeaponEffectsType = aWeaponEffectsType; }
   void SetWeaponEffectsType(const WsfVariable<WsfStringId>& aWeaponEffectsType)
   {
      mWeaponEffectsType = aWeaponEffectsType;
   }
   //@}

   //! @name Weapon engagement data.
   //@{
   //! If this is a weapon platform, return the pointer to the associated engagement object.
   WsfWeaponEngagement* GetWeaponEngagement() const { return mEngagementPtr; }
   bool                 SetWeaponEngagement(WsfWeaponEngagement* aEngagementPtr);
   //@}

private:
   WsfPlatform*             mPlatformPtr;
   WsfVariable<WsfStringId> mWeaponEffectsType;
   WsfWeaponEngagement*     mEngagementPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfWeaponPlatformExtension, cWSF_COMPONENT_WEAPON_PLATFORM_EXTENSION)

#endif
