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

#include "dis/WsfDisFire.hpp"

#include "Dis.hpp"
#include "DisIO.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfMil.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponObserver.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "dis/WsfDisWeaponEvents.hpp"

UtCallbackListN<void(WsfDisFire&)> WsfDisFire::ProcessPDU;

WsfDisFire::WsfDisFire(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisFire(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
   mExtMilPtr = WsfMilExtInterface::Find(*WsfDisUtil::GetExtInterface(aInterfacePtr));
}

// virtual
int WsfDisFire::Process()
{
   double pduTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);
   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   WsfPlatform* firingPlatformPtr = nullptr;
   WsfPlatform* targetPlatformPtr = nullptr;
   WsfPlatform* weaponPlatformPtr = nullptr;

   WsfDisPlatform* firingDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetFiringEntity());
   if (firingDisPlatformPtr != nullptr)
   {
      firingPlatformPtr = firingDisPlatformPtr->GetPlatform();
      firingPlatformPtr->Update(simTime);
   }

   WsfDisPlatform* targetDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetTargetEntity());
   if (targetDisPlatformPtr != nullptr)
   {
      targetPlatformPtr = targetDisPlatformPtr->GetPlatform();
      targetPlatformPtr->Update(simTime);
   }

   WsfDisPlatform* weaponDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetWeaponEntity());
   if (weaponDisPlatformPtr != nullptr)
   {
      weaponPlatformPtr = weaponDisPlatformPtr->GetPlatform();
      weaponPlatformPtr->Update(simTime);
   }

   // Don't process this PDU if it was sent by ourself.
   bool ignorePdu = false;
   if (weaponPlatformPtr != nullptr)
   {
      // Explicitly modeled weapon. Ignore the PDU if the weapon it is internally controlled.
      ignorePdu = (!weaponPlatformPtr->IsExternallyControlled());
   }
   else if (firingPlatformPtr != nullptr)
   {
      // Implicitly modeled weapon. Ignore the PDU if the shooter is internally controlled.
      ignorePdu = (!firingPlatformPtr->IsExternallyControlled());
   }
   if (ignorePdu)
   {
      ut::log::info() << "Ignoring Fire PDU sent by self.";
      return 1; // delete the PDU
   }

   // Get the firing location. Use the PDU value if specified or the firing platform or weapon platform.

   double firingLocWCS[3];
   GetLocation(firingLocWCS[0], firingLocWCS[1], firingLocWCS[2]);
   if (UtVec3d::Equals(firingLocWCS, 0.0))
   {
      if (firingPlatformPtr != nullptr)
      {
         firingPlatformPtr->GetLocationWCS(firingLocWCS);
      }
      else if (weaponPlatformPtr != nullptr)
      {
         weaponPlatformPtr->GetLocationWCS(firingLocWCS);
      }
   }

   double lat;
   double lon;
   double alt;
   WsfPlatform::ConvertWCSToLLA(firingLocWCS, lat, lon, alt);


   if ((mExtMilPtr->GetDebugWarfarePDU() & 1) != 0)
   {
      auto out = ut::log::debug() << "Fire PDU received.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Tpdu: " << pduTime;
      auto firing = out.AddNote() << "Firing Entity: " << GetFiringEntity();
      if (firingPlatformPtr != nullptr)
      {
         firing.AddNote() << "Platform: " << firingPlatformPtr->GetName();
         firing.AddNote() << "External/Internal controlled: "
                          << (firingPlatformPtr->IsExternallyControlled() ? "External" : "Internal");
      }

      auto target = out.AddNote() << "Target Entity: " << GetTargetEntity();
      if (targetPlatformPtr != nullptr)
      {
         target.AddNote() << "Platform: " << targetPlatformPtr->GetName();
         target.AddNote() << "External/Internal controlled: "
                          << (targetPlatformPtr->IsExternallyControlled() ? "External" : "Internal");
      }

      auto weapon = out.AddNote() << "Weapon Entity: " << GetWeaponEntity();
      if (weaponPlatformPtr != nullptr)
      {
         weapon.AddNote() << "Platform: " << weaponPlatformPtr->GetName();
         weapon.AddNote() << "External/Internal controlled: "
                          << (weaponPlatformPtr->IsExternallyControlled() ? "External" : "Internal");
      }

      out.AddNote() << "Weapon Type: " << GetWeaponType();
      out.AddNote() << "Event ID: " << GetEvent();
      auto lla = out.AddNote() << "Fire Loc LLA:";
      lla.AddNote() << "LatDMS: " << UtLatPos(lat, UtAngle::FmtDMS + 2);
      lla.AddNote() << "LonDMS: " << UtLonPos(lon, UtAngle::FmtDMS + 2);
      lla.AddNote() << "Alt: " << alt << " m (" << alt * UtMath::cFT_PER_M << " ft)";
      lla.AddNote() << "LatD: " << UtLatPos(lat, UtAngle::FmtD + 4);
      lla.AddNote() << "LonD: " << UtLonPos(lon, UtAngle::FmtD + 4);
   }

   // Initiate a transfer to local control of the weapon if it was mentioned in an 'incoming_weapon_transfer' command.
   WsfMilDisInterface* milDisPtr = nullptr;
   mInterfacePtr->GetComponents().FindByRole(milDisPtr);
   if ((milDisPtr != nullptr) && milDisPtr->GetWeaponEvents()->InitiateWeaponTransfer(simTime, this))
   {
      if ((mExtMilPtr->GetDebugWarfarePDU() & 1) != 0)
      {
         ut::log::debug() << "WsfDisFire sending out notification of a received Fire PDU.";
      }
      ProcessPDU(*this);
      return 1; // transfer initiated, delete the PDU.
   }

   // Create an engagement structure and invoke the simulation observers.
   size_t targetIndex = 0;
   if (targetPlatformPtr != nullptr)
   {
      targetIndex = targetPlatformPtr->GetIndex();
   }
   WsfStringId weaponEffectsType;
   if (weaponPlatformPtr != nullptr)
   {
      weaponEffectsType = WsfWeaponEffects::GetWeaponEffectsType(*weaponPlatformPtr);
   }
   WsfWeaponEngagement* engagementPtr          = new WsfWeaponEngagement(simTime,
                                                                &WsfDisUtil::GetSimulation(mInterfacePtr),
                                                                nullptr,
                                                                WsfTrackId(),
                                                                targetIndex,
                                                                weaponEffectsType);
   bool                 deleteWeaponEngagement = true;
   engagementPtr->SetIsExternallyControlled(true);
   engagementPtr->SetFiringPlatform(firingPlatformPtr);
   engagementPtr->SetWeaponLocationWCS(weaponPlatformPtr, firingLocWCS); // Use reported location if available.
   if (engagementPtr->Initialize(weaponPlatformPtr))
   {
      if (weaponPlatformPtr != nullptr)
      {
         WsfWeaponEngagement::Assign(*weaponPlatformPtr, engagementPtr);
         deleteWeaponEngagement = false; // The weapon platform owns the engagement (or it failed and was deleted)
      }
      if ((mExtMilPtr->GetDebugWarfarePDU() & 1) != 0)
      {
         ut::log::debug() << "WsfDisFire sending out notification of a received Fire PDU.";
      }
      ProcessPDU(*this);
      if (engagementPtr != nullptr) // Make sure WsfWeaponEngagement::Assign() was successful
      {
         WsfObserver::WeaponFired (&WsfDisUtil::GetSimulation(mInterfacePtr))(simTime, engagementPtr, nullptr);
      }
   }
   else
   {
      ut::log::warning() << "WsfDisFire failed to initialize engagement object.";
   }
   if (deleteWeaponEngagement)
   {
      WsfWeaponEngagement::Delete(engagementPtr);
   }
   return 1;
}
