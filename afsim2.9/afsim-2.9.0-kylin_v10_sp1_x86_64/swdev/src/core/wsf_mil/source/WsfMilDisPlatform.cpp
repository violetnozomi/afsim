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

#include "WsfMilDisPlatform.hpp"

#include "DisEmission.hpp"
#include "DisSystem.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"
#include "dis/WsfDisInterface.hpp"

WsfMilDisPlatform::WsfMilDisPlatform(WsfDisPlatform* aPlatformPtr)
   : mPlatformPtr(aPlatformPtr)
{
   mMilInterfacePtr = WsfMilDisInterface::Find(*aPlatformPtr->GetInterface());
}

WsfMilDisPlatform::~WsfMilDisPlatform()
{
   for (auto& jammer : mJammers)
   {
      delete jammer.mSystemPtr;
   }
}

WsfComponent* WsfMilDisPlatform::CloneComponent() const
{
   return nullptr;
}

WsfStringId WsfMilDisPlatform::GetComponentName() const
{
   return UtStringIdLiteral("#mil");
}

const int* WsfMilDisPlatform::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_MIL_DIS_PLATFORM, 0};
   return roles;
}

void* WsfMilDisPlatform::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_MIL_DIS_PLATFORM)
   {
      return this;
   }
   return nullptr;
}

void WsfMilDisPlatform::SendingEmission(double aSimTime, DisEmission* aEmissionPtr, DisEventId& aPreviousNumber)
{
   static unsigned int cMAX_SYSTEMS_PER_PDU = 8;

   WsfDisInterface*    disPtr    = mPlatformPtr->GetInterface();
   WsfMilDisInterface* milDisPtr = WsfMilDisInterface::Find(*disPtr);
   // Add systems for jammers.
   for (auto& jammer : mJammers)
   {
      JammerData& jammerData = jammer;
      DisSystem*  systemPtr  = jammerData.mSystemPtr;
      bool        addSystem  = true;
      if (systemPtr->GetNumberOfBeams() == 0)
      {
         // A beam count of zero indicates the system has been turned off. Include the system in the
         // emissions PDU only if the previous state was on (beam count != 0).
         addSystem = (jammerData.mLastBeamCountSent != 0);
      }
      jammerData.mLastBeamCountSent = systemPtr->GetNumberOfBeams();
      if (addSystem)
      {
         milDisPtr->UpdateSystemFromWeapon(jammer.mWeaponPtr, systemPtr); // TODO see note above
         if ((aEmissionPtr->GetNumberOfSystems() >= cMAX_SYSTEMS_PER_PDU) ||
             (aEmissionPtr->GetLength() + systemPtr->GetLengthOctets() > 1472))
         {
            aPreviousNumber.AssignNewNumber();
            aEmissionPtr->SetEventId(aPreviousNumber);
            disPtr->ImmediatePutPdu(aSimTime, *aEmissionPtr);
#ifdef TODO_REMOVE_THIS
            UpdatePduCounters();
#endif
            aEmissionPtr->RemoveAllSystemsWithNoDelete();
         }
         aEmissionPtr->AddSystem(systemPtr);
      }
   }
}

// ============================================================================
void WsfMilDisPlatform::AllocateEmissionSystems(DisUint8& emitterId)
{
   // Allocate system for each jammer...
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*(mPlatformPtr->GetPlatform())); !iter.AtEnd(); ++iter)
   {
      // GetEM_XmtrCount() will return 0 if none of the Xmtrs are active, such as in multi-beam
      // multi-spot jammers. So check to see if the weapon is of type WSF_RF_JAMMER also.
      if ((iter->GetEM_XmtrCount() != 0) || iter->IsA_TypeOf(UtStringIdLiteral("WSF_RF_JAMMER")))
      {
         if (!iter->IsPrivate())
         {
            DisSystem* systemPtr = new DisSystem();
            systemPtr->SetNumber(emitterId);
            mPlatformPtr->GetInterface()->SetEmitterTypeAndFunction(*iter, systemPtr);
            mJammers.emplace_back(*iter, systemPtr);
            ++emitterId;
         }
      }
   }
}

// ============================================================================
//! Given a pointer to a WSF weapon, return a pointer to our jammer data.
//! @note The return value may be zero.
WsfMilDisPlatform::JammerData* WsfMilDisPlatform::GetJammerData(WsfWeapon* aWeaponPtr)
{
   JammerData* dataPtr = nullptr;
   for (auto& jammer : mJammers)
   {
      if (jammer.mWeaponPtr == aWeaponPtr)
      {
         dataPtr = &jammer;
         break;
      }
   }
   return dataPtr;
}

DisSystem* WsfMilDisPlatform::GetJammerSystem(WsfWeapon* aWeaponPtr)
{
   DisSystem*  systemPtr = nullptr;
   JammerData* dataPtr   = GetJammerData(aWeaponPtr);
   if (dataPtr != nullptr)
   {
      systemPtr = dataPtr->mSystemPtr;
   }
   return systemPtr;
}

// ============================================================================
void WsfMilDisPlatform::JammingRequestCanceled(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex)
{
   JammerData* dataPtr = GetJammerData(aWeaponPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   DisSystem* systemPtr = dataPtr->mSystemPtr;

   // Now update the system; all beams get removed and then active ones added back
   UpdateJammerSystem(aSimTime, aWeaponPtr, systemPtr, aTargetIndex);

   mPlatformPtr->SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfMilDisPlatform::JammingRequestInitiated(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex)
{
   JammerData* dataPtr = GetJammerData(aWeaponPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   DisSystem* systemPtr = dataPtr->mSystemPtr;

   // Now update the system; all beams get removed and then active ones added back
   UpdateJammerSystem(aSimTime, aWeaponPtr, systemPtr, aTargetIndex);

   mPlatformPtr->SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfMilDisPlatform::JammingRequestUpdated(double aSimTime, WsfWeapon* aWeaponPtr, size_t aTargetIndex)
{
   JammerData* dataPtr = GetJammerData(aWeaponPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   DisSystem* systemPtr = dataPtr->mSystemPtr;

   // Now update the system; all beams get removed and then active ones added back
   UpdateJammerSystem(aSimTime, aWeaponPtr, systemPtr, aTargetIndex);

   mPlatformPtr->SendOrQueueEmissionsPDU(aSimTime);
}

void WsfMilDisPlatform::TurnOffEmission(DisEmission* aEmissionPtr)
{
   for (auto& jammer : mJammers)
   {
      jammer.mSystemPtr->RemoveAllBeams();
   }
}

// ============================================================================
void WsfMilDisPlatform::WeaponTurnedOff(double aSimTime, WsfWeapon* aWeaponPtr)
{
   JammerData* dataPtr = GetJammerData(aWeaponPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   DisSystem* systemPtr = dataPtr->mSystemPtr;

   // Send out the system with all the beams removed to indicate it is turned off.

   systemPtr->RemoveAllBeams();
   mMilInterfacePtr->UpdateSystemFromWeapon(aWeaponPtr, systemPtr);
   mPlatformPtr->SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
void WsfMilDisPlatform::WeaponTurnedOn(double aSimTime, WsfWeapon* aWeaponPtr)
{
   JammerData* dataPtr = GetJammerData(aWeaponPtr);
   if (dataPtr == nullptr)
   {
      return;
   }
   DisSystem* systemPtr = dataPtr->mSystemPtr;

   UpdateJammerSystem(aSimTime, aWeaponPtr, systemPtr, 0);
   mPlatformPtr->SendOrQueueEmissionsPDU(aSimTime);
}

// ============================================================================
// private
void WsfMilDisPlatform::UpdateJammerSystem(double aSimTime, WsfWeapon* aWeaponPtr, DisSystem* aSystemPtr, size_t aTargetIndex)
{
   WsfDisInterface* interfacePtr = mPlatformPtr->GetInterface();
   // Begin with a clean slate
   aSystemPtr->RemoveAllBeams();

   // Allocate a beam for each emitter.  If there are no emitters,
   // create a beam so VESPA et. al., can display it anyhow...
   size_t emitterCount = aWeaponPtr->GetEM_XmtrCount();
   if (emitterCount != 0)
   {
      for (size_t i = 0; i < emitterCount; ++i)
      {
         // Ensure the system doesn't exceed its maximum length of 255 * 4 = 1020 bytes.
         // A beam 52 bytes + 8 x number of T/J entries. T/J entries are added separately and
         // checks will be performed to ensure it is OK.

         if ((aSystemPtr->GetLengthOctets() + 52) > 1020)
         {
            break;
         }

         DisBeam*     beamPtr = new DisBeam();
         unsigned int xmtrId  = aWeaponPtr->GetEM_Xmtr(i).GetUniqueId();
         DisUint8     beamId  = static_cast<DisUint8>(mPlatformPtr->GetPartIdToBeamId(xmtrId));
         beamPtr->SetNumber(beamId);
         interfacePtr->SetBeamTypeAndFunction(aWeaponPtr,
                                              aWeaponPtr->GetCurrentModeName(),
                                              static_cast<int>(beamId),
                                              beamPtr,
                                              12);
         aSystemPtr->AddBeam(beamPtr);

         // Get the target entity ID; if it exists
         DisEntityId tgtEntityId;

         WsfPlatform* targetPtr = interfacePtr->GetSimulation().GetPlatformByIndex(aWeaponPtr->GetTargetIndex(xmtrId));
         if (targetPtr != nullptr)
         {
            interfacePtr->GetEntityId(targetPtr, tgtEntityId);
         }

         // Add the target to the Track/Jam list
         if (tgtEntityId.IsDefined())
         {
            if (beamPtr->CanAddTrackJamEntry(aSystemPtr))
            {
               beamPtr->AddTarget(DisTrackJam(tgtEntityId));
            }
         }
      }
   }

   // Update the system
   mMilInterfacePtr->UpdateSystemFromWeapon(aWeaponPtr, aSystemPtr);
}

WsfMilDisPlatform* WsfMilDisPlatform::Find(const WsfDisPlatform& aPlatform)
{
   WsfMilDisPlatform* componentPtr = nullptr;
   aPlatform.GetComponents().FindByRole(componentPtr);
   return componentPtr;
}
