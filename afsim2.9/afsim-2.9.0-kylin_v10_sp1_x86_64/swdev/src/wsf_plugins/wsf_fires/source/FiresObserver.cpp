// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "FiresObserver.hpp"

#include <string>

#include "BallisticPlatforms.hpp"
#include "FiresTables.hpp"
#include "UtMat3.hpp"
#include "WsfDraw.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfUtil.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"

namespace Fires
{

Observer::Observer(WsfSimulation* aSimPtr)
   : mSimPtr(aSimPtr)
   , mDraw(*aSimPtr)
{
   mCallbacks.Add(WsfObserver::WeaponFired(aSimPtr).Connect(&Observer::WeaponFired, this));
   mCallbacks.Add(WsfObserver::WeaponTerminated(aSimPtr).Connect(&Observer::WeaponTerminated, this));
}

Observer::~Observer() {}

void Observer::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr)
{
   // todo this will eventually be replaced by a weapons mode check
   const WsfWeapon* weaponSystemPtr = aEngagementPtr->GetWeaponSystem();

   if (weaponSystemPtr != nullptr)
   {
      WsfStringId weaponType = aEngagementPtr->GetWeaponSystem()->GetLaunchedPlatformType();

      // if (BallisticPlatforms::IsType(weaponType.GetString()))
      FiresTables* firesSetup = static_cast<FiresTables*>(mSimPtr->GetScenario().FindExtension("wsf_fires"));
      if (firesSetup->IsType(weaponType.GetString()))
      {
         WsfPlatform* weaponPtr = nullptr;

         // only need to process if this is an implicit weapon:
         const WsfImplicitWeapon* impWeaponPtr = dynamic_cast<const WsfImplicitWeapon*>(aEngagementPtr->GetWeaponSystem());
         if (impWeaponPtr != nullptr)
         {
            // Create an internal representation (for visualization purposes), and add it to the sim
            // Note: When using implicit weapons, it is up to the user to define these as DIS private types,
            // so they do not show up in another distributed simulation!
            weaponPtr = BallisticPlatforms::CreatePlatform(weaponType.GetString(), *aEngagementPtr, mSimPtr);

            if (weaponPtr != nullptr)
            {
               auto logInfo = ut::log::info() << "Adding Weapon platform: ";
               logInfo.AddNote() << "Weapon: " << weaponPtr->GetName();
               logInfo.AddNote() << "Weapon Type: " << weaponPtr->GetType();
               logInfo.Send();

               // Make it invincible so that is is not destroyed until after the end of the engagement.
               weaponPtr->SetIsIndestructible(true);

               // Make it concealed so that it is not detected.
               weaponPtr->SetConcealmentFactor(1.0);

               // Adding the platform with no name means the simulation will assign a name.
               mSimPtr->AddPlatform(mSimPtr->GetSimTime(), weaponPtr);
            }
         }
         else
         {
            weaponPtr = aEngagementPtr->GetWeaponPlatform();
         }

         if (weaponPtr != nullptr)
         {
            // Keep track of this engagement in order to provide visualization
            mEngagementMap[aEngagementPtr->GetSerialNumber()] = weaponPtr->GetIndex();

            mDraw.SetId(aEngagementPtr->GetSerialNumber());
            mDraw.SetDuration(1.0e10);
            mDraw.SetColor(1.0, 0.0, 0.0); // red
            if (aEngagementPtr->GetFiringPlatform() != nullptr)
            {
               mDraw.BeginLines();
               double locWCS[3];
               aEngagementPtr->GetFiringPlatform()->GetLocationWCS(locWCS);
               mDraw.VertexWCS(locWCS);
               double targetLocWCS[3];
               aEngagementPtr->GetTargetLocationAtLaunchWCS(targetLocWCS);
               mDraw.VertexWCS(targetLocWCS);
               mDraw.BeginIcons(0.0, "Bullseye");
               mDraw.VertexWCS(targetLocWCS);
               mDraw.End();
            }
         }
      }
   }
}

void Observer::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   std::map<unsigned int, size_t>::iterator iter = mEngagementMap.find(aEngagementPtr->GetSerialNumber());
   if (iter != mEngagementMap.end())
   {
      // Erase earlier line and bullseye.
      mDraw.Erase(aEngagementPtr->GetSerialNumber());

      // find the propagated object
      WsfPlatform* weaponPtr = mSimPtr->GetPlatformByIndex(iter->second);
      if (weaponPtr != nullptr)
      {
         // Set the detonation location to be the location of the (local) weapon.
         double targetLocWCS[3];
         weaponPtr->GetLocationWCS(targetLocWCS);

         mDraw.SetId(static_cast<unsigned int>(weaponPtr->GetIndex())); // Use a unique id.
         mDraw.SetDuration(3.0);
         mDraw.BeginIcons(0.0, "Explosion");
         mDraw.VertexWCS(targetLocWCS);

         WsfPlatform* firingPlatformPtr = aEngagementPtr->GetFiringPlatform();
         if (firingPlatformPtr != nullptr)
         {
            mDraw.SetColor(0.0, 0.0, 0.0); // black
            mDraw.SetLineSize(2);
            mDraw.BeginLines();
            double firingLocWCS[3];
            firingPlatformPtr->GetLocationWCS(firingLocWCS);
            mDraw.VertexWCS(firingLocWCS);
            mDraw.VertexWCS(targetLocWCS);
         }
         mDraw.End();

         mSimPtr->DeletePlatform(mSimPtr->GetSimTime(), weaponPtr);
      }
      mEngagementMap.erase(iter);
   }
}

} // namespace Fires
