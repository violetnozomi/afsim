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

#include "FiresDisObserver.hpp"

#include <sstream>

#include "BallisticPlatforms.hpp"
#include "WsfDraw.hpp"
#include "WsfExtDetonation.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisDetonation.hpp"
#include "dis/WsfDisFire.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisWeaponEvents.hpp"
#include "ext/WsfExtInterface.hpp"

namespace Fires
{


DisObserver::DisObserver(WsfSimulation* aSimPtr)
   : mSimPtr(aSimPtr)
   , mDraw(*aSimPtr)
{
   mDisInterfacePtr = static_cast<WsfDisInterface*>(mSimPtr->FindExtension("dis_interface"));
   mExtInterfacePtr = static_cast<WsfExtInterface*>(mSimPtr->FindExtension("ext_interface"));

   // TODO - LBM - need to connect these callbacks, but they aren't accessible in a module yet
   // perhaps they can be moved to the WsfDisInterface module
   // mCallbacks.Add(WsfDisFire::ProcessPDU.Connect(&DisObserver::ProcessPDU, this));
   // mCallbacks.Add(WsfDisDetonation::ProcessPDU.Connect(&DisObserver::ProcessPDU, this));
   // mCallbacks.Add(WsfDisWeaponEvents::ProcessFirePDU.Connect(&DisObserver::ProcessFirePDU, this));
   // mCallbacks.Add(WsfDisWeaponEvents::ProcessDetonationPDU.Connect(&DisObserver::ProcessDetonationPDU, this));
}


void DisObserver::VisualizeDetonation(const DisEntityId& aFiringEntityId, const DisEntityId& aTargetEntityId)
{
   DisEventId eventId(aTargetEntityId.GetSite(), aTargetEntityId.GetApplication(), aTargetEntityId.GetEntity());
   std::map<DisEventId, WsfStringId>::iterator engagementIter = mEventToNameMap.find(eventId);

   WsfPlatform* targetPtr = nullptr;

   if (engagementIter != mEventToNameMap.end())
   {
      targetPtr = mSimPtr->GetPlatformByName(engagementIter->second);
   }
   else if (aTargetEntityId != DisEntityId::NO_SPECIFIC_ENTITY)
   {
      WsfDisPlatform* disTargetPtr = mDisInterfacePtr->FindDisPlatform(aTargetEntityId);
      if (disTargetPtr != nullptr)
      {
         targetPtr = disTargetPtr->GetPlatform();
      }
   }

   if (targetPtr != nullptr)
   {
      mDraw.Erase(static_cast<int>(targetPtr->GetIndex()));
      mDraw.SetId(static_cast<int>(targetPtr->GetIndex())); // Use a unique id.
      mDraw.SetDuration(3.0);
      mDraw.BeginIcons(0.0, "Explosion");
      double targetLocWCS[3];
      targetPtr->GetLocationWCS(targetLocWCS);
      mDraw.VertexWCS(targetLocWCS);

      WsfDisPlatform* firingDisEntityPtr = mDisInterfacePtr->FindDisPlatform(aFiringEntityId);
      if (firingDisEntityPtr != nullptr)
      {
         mDraw.SetColor(0.0, 0.0, 0.0); // black
         mDraw.SetLineSize(2);
         mDraw.BeginLines();
         double firingLocWCS[3];
         firingDisEntityPtr->GetPlatform()->GetLocationWCS(firingLocWCS);
         mDraw.VertexWCS(firingLocWCS);
         mDraw.VertexWCS(targetLocWCS);
      }
      mDraw.End();
   }
}

// void DisObserver::VisualizeFire(const DisEntityId& aFiringEntityId,
//                                 const DisEntityId& aTargetEntityId) const
//{
// }

// static
//! Perform custom processing on a detonation PDU received from an external source.
void DisObserver::ProcessPDU(WsfDisDetonation& aDet)
{
   // the target id should match the original event id in the fire pdu
   // Note that the target id is a DIS Entity Id, and the event id is a DIS Event id, but the structure is the same.
   DisEntityId                 targetId = aDet.GetTargetEntity();
   DisEventId                  eventId(targetId.GetSite(), targetId.GetApplication(), targetId.GetEntity());
   EngagementDataMap::iterator iter = mEngagementDataMap.find(eventId);
   if (iter != mEngagementDataMap.end())
   {
      EngagementData engData = iter->second;
      // found a match; if this is result code 17 (laser), clear the engagment with no effect.
      // Call "Defeat" on the saved engagement
      if ((aDet.GetDetonationResult() == 17) || (aDet.GetDetonationResult() == 1))
      {
         // Draw the explosion effect and a black line from shooter to target
         // the target entity is the original event id
         VisualizeDetonation(aDet.GetFiringEntity(), targetId);

         // Try to retrieve and end the engagement.
         WsfPlatform* firesFiringEntityPtr = mSimPtr->GetPlatformByIndex(engData.mPlatformIndex);
         if (firesFiringEntityPtr != nullptr)
         {
            WsfImplicitWeapon* weaponPtr =
               static_cast<WsfImplicitWeapon*>(firesFiringEntityPtr->GetComponent<WsfWeapon>(engData.mWeaponName));
            WsfWeaponEngagement* engPtr = weaponPtr->GetEngagement(engData.mEngagementId);
            if (engPtr != nullptr)
            {
               engPtr->Defeat();
               engPtr->Terminate(mSimPtr->GetSimTime());
            }
         }
      }
   }
}

// static
//! Perform custom processing on a fire PDU received from an external source.
// void DisObserver::ProcessPDU(WsfDisFire& aFire)
//{
//    VisualizeFire(aFire.GetFiringEntity(), aFire.GetTargetEntity());
// }

// static
//  Process an internal fire PDU before it is sent out.
void DisObserver::ProcessFirePDU(DisFire& aFire)
{
   // For an implicit weapon, store the firing platform, weapon, and engagement id (current engagement)
   WsfDisPlatform* firingDisPlatformPtr = mDisInterfacePtr->FindDisPlatform(aFire.GetFiringEntity());
   if (firingDisPlatformPtr != nullptr)
   {
      WsfPlatform* firingPlatformPtr = firingDisPlatformPtr->GetPlatform();

      // find weapon
      DisEntityType    disWeaponType = aFire.GetWeaponType();
      WsfExtEntityType weaponType(disWeaponType.GetEntityKind(),
                                  disWeaponType.GetDomain(),
                                  disWeaponType.GetCountry(),
                                  disWeaponType.GetCategory(),
                                  disWeaponType.GetSubcategory(),
                                  disWeaponType.GetSpecific(),
                                  disWeaponType.GetExtra());
      ut::log::info() << "DIS Weapon type:" << weaponType;
      WsfStringId id   = WsfStringId(mExtInterfacePtr->GetStringId(weaponType));
      std::string type = id.GetString();

      // find the weapon of the given type (this will probably be the only weapon for a fires launcher).
      WsfWeapon* weaponPtr = nullptr;

      for (WsfComponentList::RoleIterator<WsfWeapon> iter(*firingPlatformPtr); !iter.AtEnd(); ++iter)
      {
         WsfWeapon* candidateWeaponPtr = *iter;
         if (candidateWeaponPtr &&
             ((candidateWeaponPtr->GetTypeId() == id) || (candidateWeaponPtr->GetLaunchedPlatformType() == id)))
         {
            weaponPtr = candidateWeaponPtr;
         }
      }

      if (weaponPtr != nullptr)
      {
         // only need to process if this is an implicit weapon:
         WsfImplicitWeapon* impWeaponPtr = dynamic_cast<WsfImplicitWeapon*>(weaponPtr);
         if (impWeaponPtr != nullptr)
         {
            // This should be the current engagegment:
            WsfWeaponEngagement* engPtr = impWeaponPtr->GetCurrentEngagement();
            if (engPtr != nullptr)
            {
               EngagementData engData;
               engData.mEngagementId                = engPtr->GetSerialNumber();
               engData.mPlatformIndex               = firingPlatformPtr->GetIndex();
               engData.mWeaponName                  = impWeaponPtr->GetNameId();
               mEngagementDataMap[aFire.GetEvent()] = engData;
            }
         }
      }
   }
}

// static
//  Process an internal detonation PDU before it is sent out.
void DisObserver::ProcessDetonationPDU(DisDetonation& aDet)
{
   // remove the internally propagated entity if the implicit weapon has detonated
   std::map<DisEventId, WsfStringId>::iterator iter = mEventToNameMap.find(aDet.GetEvent());
   if (iter != mEventToNameMap.end())
   {
      WsfPlatform* weaponPtr = mSimPtr->GetPlatformByName(iter->second);
      if (weaponPtr != nullptr)
      {
         // Set the detonation location to be the location of the (local) weapon.
         double locationWCS[3];
         weaponPtr->GetLocationWCS(locationWCS);
         aDet.SetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);
      }
      mEventToNameMap.erase(iter);
   }
}

} // namespace Fires
