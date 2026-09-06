// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberDetonateEffect.hpp"

#include <algorithm>
#include <sstream>

#include "UtMemory.hpp"
#include "WsfCyberAttack.hpp"
#include "WsfCyberAttackTypes.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberScenarioExtension.hpp"
#include "WsfCyberSimulationExtension.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEffectsTypes.hpp"
#include "WsfWeaponEngagement.hpp"

namespace wsf
{
namespace cyber
{

DetonateEffect* DetonateEffect::Clone() const
{
   return new DetonateEffect(*this);
}

bool DetonateEffect::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "weapon_name")
   {
      UtStringId weaponName;
      aInput.ReadValue(weaponName);
      mWeaponNames.emplace(weaponName);
   }
   else if (command == "weapon_type")
   {
      UtStringId weaponType;
      aInput.ReadValue(weaponType);
      mWeaponTypes.emplace(weaponType);
   }
   else
   {
      myCommand = Effect::ProcessInput(aInput);
   }
   return myCommand;
}

void DetonateEffect::Attack(double aSimTime, wsf::cyber::Engagement& aEngagement)
{
   // This Attack will attempt to detonate every explicit weapon on the platform until either the platform
   // is destroyed or there are no more explicit weapons on it.

   auto         sim        = &aEngagement.GetSimulation();
   WsfPlatform* victimPlat = sim->GetPlatformByName(aEngagement.GetVictim());
   if (!victimPlat)
   {
      return;
   }

   WsfTrack dummy;
   dummy.SetPlatformOriginator(victimPlat);
   dummy.SetTrackType(WsfTrack::cPROCESSED);
   dummy.SetFusible(true);
   dummy.Initialize(aSimTime, victimPlat->GetNextTrackId(), *sim);
   double locWCS[3];
   victimPlat->GetLocationWCS(locWCS);
   dummy.SetTarget(*victimPlat);
   dummy.SetOriginatorLocationWCS(locWCS);
   dummy.Update(aSimTime);
   dummy.SetLocationWCS(locWCS);
   dummy.SetWCS_LocationValid(true);
   dummy.Set3D(true);

   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*victimPlat); !iter.AtEnd(); ++iter)
   {
      auto wpnPtr = CheckForValidWeapon(*iter);
      if (!wpnPtr)
      {
         // if this weapon on the platform is not a valid weapon to detonate, continue to the next weapon
         continue;
      }

      if (wpnPtr->UseDeferredLaunching())
      {
         ut::log::warning()
            << "Attempted to launch a WSF_CYBER_DETONATE_EFFECT on an unsupported weapon type. The weapon "
            << wpnPtr->GetName() << "on platform " << victimPlat->GetName()
            << " uses deferred launching,which is not supported by this effect. The next available weapon, if any, "
               "will be used instead.";
         continue;
      }

      double interval = wpnPtr->GetFiringInterval();
      wpnPtr->SetFiringInterval(0);

      while (!(victimPlat->IsDeleted() || victimPlat->IsBroken() || wpnPtr->GetQuantityRemaining() == 0.0))
      {
         auto result = wpnPtr->Fire(aSimTime, &dummy, {});
         if (!result)
         {
            // If we fail then we do not know why. Try the next weapon.
            ut::log::info() << "Failed to detonate weapon " << wpnPtr->GetName() << "on platform "
                            << victimPlat->GetName();
            break;
         }

         result.mEngagementPtr->GetWeaponEffects()->SetParentDamageAllowed(true);
         result.mEngagementPtr->Terminate(aSimTime, WsfWeaponEngagement::GeometryResult::cTARGET_IMPACT);
      }

      wpnPtr->SetFiringInterval(interval);
   }
}

void DetonateEffect::Restore(double aSimTime, wsf::cyber::Engagement& aEngagement)
{
   // No meaningful way to restore functionality
}

WsfExplicitWeapon* DetonateEffect::CheckForValidWeapon(WsfWeapon* aWpnPtr)
{
   // Check if the base type is an explicit weapon. If not, we want to continue to the next weapon
   auto out = dynamic_cast<WsfExplicitWeapon*>(aWpnPtr);
   if (!out)
   {
      return nullptr;
   }

   for (auto weaponType : aWpnPtr->GetTypeList())
   {
      if (mWeaponTypes.count(weaponType))
      {
         return out;
      }
   }

   std::string name = aWpnPtr->GetName();
   if (mWeaponNames.count(aWpnPtr->GetName()))
   {
      return out;
   }

   return nullptr;
}

} // namespace cyber
} // namespace wsf
