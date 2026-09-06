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

#include "dis/WsfDisWeaponEvents.hpp"

#include <memory>
#include <sstream>

#include "Dis.hpp"
#include "DisDEDamageDescriptionRecord.hpp"
#include "DisDEPrecisionAimpointRecord.hpp"
#include "DisDetonation.hpp"
#include "DisDirectedEnergyFire.hpp"
#include "DisEntityDamageStatus.hpp"
#include "DisFire.hpp"
#include "DisIO.hpp"
#include "DisTime.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfComm.hpp"
#include "WsfCommandChain.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfFuel.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfUtil.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponTypes.hpp"
#include "dis/WsfDisDirectedEnergyFire.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisMover.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
namespace
{
using OutgoingTransferList = WsfMilDisInterface::OutgoingTransferList;
using OutgoingTransfer     = WsfMilDisInterface::OutgoingTransfer;
} // namespace
// ================================================================================================
WsfDisWeaponEvents::WsfDisWeaponEvents(WsfMilDisInterface* aInterfacePtr)
{
   mMilPtr       = aInterfacePtr;
   mInterfacePtr = mMilPtr->GetDis();
   mExtMilPtr    = WsfMilExtInterface::Find(*WsfDisUtil::GetExtInterface(mInterfacePtr));
}

// ================================================================================================
//! Get the warhead enum based on any input category associations.
//! Default is to return 0 == "Other".
int WsfDisWeaponEvents::GetWarheadEnum(const WsfWeapon* aWeaponPtr)
{
   return mExtMilPtr->GetWarheadEnum(aWeaponPtr);
}

// ================================================================================================
void WsfDisWeaponEvents::WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTrackPtr)
{
   // DIS rules state that the DE fire PDU must be sent before the standard fire PDU,
   // so perform a check and defer it if the weapon is a DE weapon
   WsfWeapon* firingWeaponPtr = const_cast<WsfWeapon*>(aEngagementPtr->GetWeaponSystem());
   if ((firingWeaponPtr != nullptr) && (!firingWeaponPtr->ComponentHasRole(cWSF_COMPONENT_DIRECTED_ENERGY_WEAPON)))
   {
      WeaponFiredP(aSimTime, aEngagementPtr);
   }
}

// ================================================================================================
void WsfDisWeaponEvents::WeaponFiredP(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   if (aEngagementPtr == nullptr)
   {
      return;
   }

   if ((aEngagementPtr->IsExternallyControlled()) || (aEngagementPtr->GetFiringPlatform() == nullptr) ||
       ((aEngagementPtr->GetFiringPlatform() != nullptr) && (aEngagementPtr->GetFiringPlatform()->IsExternallyMoved())))
   {
      return;
   }

   WsfPlatform* firingPlatformPtr = nullptr;
   // NOT-USED WsfPlatform*    targetPlatformPtr = 0;
   WsfPlatform*  weaponPlatformPtr = nullptr;
   DisEntityId   firingEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityId   weaponEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityId   targetEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityType weaponEntityType;
   DisEntityType munitionType;

   const WsfWeapon* weaponPtr = aEngagementPtr->GetWeaponSystem();

   WsfDisPlatform* firingDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetFiringPlatformIndex());
   if (firingDisPlatformPtr != nullptr)
   {
      firingPlatformPtr = firingDisPlatformPtr->GetPlatform();
      firingEntityId    = firingDisPlatformPtr->GetEntityId();
   }

   WsfDisPlatform* weaponDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetWeaponPlatformIndex());
   if (weaponDisPlatformPtr != nullptr)
   {
      weaponPlatformPtr = weaponDisPlatformPtr->GetPlatform();
      weaponEntityId    = weaponDisPlatformPtr->GetEntityId();
      weaponEntityType  = weaponDisPlatformPtr->GetEntityType();
   }
   else if (weaponPtr != nullptr)
   {
      // This is an implicit weapon.
      WsfExtEntityType extType;
      if (weaponPtr->GetLaunchedPlatformType() != 0)
      {
         WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEntityType(weaponPtr->GetLaunchedPlatformType(), extType);
      }
      else
      {
         // Use the weapon type directly (as with directed energy weapons)
         WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEntityType(weaponPtr, extType);
      }
      weaponEntityType = WsfDisExt::ToDis(extType);
   }

   WsfDisPlatform* targetDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetTargetPlatformIndex());
   if (targetDisPlatformPtr != nullptr)
   {
      // NOT-USED targetPlatformPtr = targetDisPlatformPtr->GetPlatform();
      targetEntityId = targetDisPlatformPtr->GetEntityId();
   }

   // Determine if the PDU should be sent.

   if (weaponPlatformPtr != nullptr)
   {
      if (weaponPlatformPtr->IsExternallyControlled())
      {
         // The weapon is explicitly modeled ... don't send if the weapon is externally controlled.
         return;
      }
      else if (weaponDisPlatformPtr->IsIncomingTransfer())
      {
         // The weapon was created in response to a received Fire PDU, thus it should not be sent again.
         return;
      }
   }
   else if (firingPlatformPtr != nullptr)
   {
      // The weapon seems to be implicitly modeled (no weapon platform)... don't send if the shooter is externally controlled.
      if (firingPlatformPtr->IsExternallyControlled())
      {
         return;
      }
   }

   bool outgoingWeaponTransfer = IsOutgoingWeaponTransfer(aSimTime, weaponPlatformPtr);

   // Ensure the firing platform, weapon and target positions are up-to-date and have had at least one entity state sent.
   if (firingDisPlatformPtr != nullptr)
   {
      // Ensure the firing platform's position is up-to-date and an initial state PDU was sent
      // before sending this fire PDU.
      firingDisPlatformPtr->ForceUpdate(aSimTime);
   }
   if (weaponDisPlatformPtr != nullptr)
   {
      // Ensure the weapon platform's position is up-to-date and an initial state PDU was sent
      // before sending this fire PDU.
      weaponDisPlatformPtr->ForceUpdate(aSimTime);
   }
   if (targetDisPlatformPtr != nullptr)
   {
      // Ensure the target platform's position is up-to-date and an initial state PDU was sent
      // before sending this fire PDU.
      targetDisPlatformPtr->ForceUpdate(aSimTime);
   }
   // Allow weapon servers to decide weapon status for launch and last chance to set preassigned weapon tail number.
   bool bCanBeLaunched = true;
   ProcessWeaponPrelaunchStatus(weaponPtr, weaponPlatformPtr, firingDisPlatformPtr, weaponEntityId, bCanBeLaunched);
   if (!bCanBeLaunched)
   {
      return;
   }

   auto        pduPtr = ut::make_unique<DisFire>(); // memory deleted by DIS interface or thread
   WeaponEvent weaponEvent;

   // Event Site and Id
   auto iter             = mWeaponEventList.find(aEngagementPtr->GetSerialNumber());
   bool foundWeaponEvent = (iter != mWeaponEventList.end());
   if (foundWeaponEvent)
   {
      weaponEvent = iter->second;
   }
   else
   {
      // Keep track of the weapon event for weapon termination
      weaponEvent.mFiringDisEntityId   = firingEntityId;
      weaponEvent.mWeaponDisEntityId   = weaponEntityId;
      weaponEvent.mWeaponDisEntityType = weaponEntityType;
      weaponEvent.mTargetDisEntityId   = targetEntityId;
      weaponEvent.mDisEventId.SetSite(Dis::GetSite());
      weaponEvent.mDisEventId.SetApplication(Dis::GetApplication());
      weaponEvent.mDisEventId.AssignNewNumber();
      mWeaponEventList[aEngagementPtr->GetSerialNumber()] = weaponEvent;
   }
   pduPtr->SetEvent(weaponEvent.mDisEventId);

   pduPtr->SetFiringEntity(firingEntityId);
   pduPtr->SetWeaponEntity(weaponEntityId);
   pduPtr->SetWeaponType(weaponEntityType);
   pduPtr->SetTargetEntity(targetEntityId);

   // Determine the weapon location and velocity
   double weaponLocationWCS[3] = {0.0, 0.0, 0.0};
   double weaponVelocityWCS[3] = {0.0, 0.0, 0.0};

   if (weaponPlatformPtr != nullptr)
   {
      // Explicit weapon; Get the initial location and velocity from the weapon platform object.
      weaponPlatformPtr->GetLocationWCS(weaponLocationWCS);
      weaponPlatformPtr->GetVelocityWCS(weaponVelocityWCS);
   }
   else
   {
      // Implicit weapon; Get the initial location and velocity from the engagement object.
      aEngagementPtr->GetWeaponLocationAtLaunchWCS(weaponLocationWCS);
      aEngagementPtr->GetWeaponVelocityAtLaunchWCS(weaponVelocityWCS);
   }

   pduPtr->SetLocation(weaponLocationWCS[0], weaponLocationWCS[1], weaponLocationWCS[2]);
   pduPtr->SetVelocity(static_cast<float>(weaponVelocityWCS[0]),
                       static_cast<float>(weaponVelocityWCS[1]),
                       static_cast<float>(weaponVelocityWCS[2]));

   pduPtr->SetFireMissionIndex(0);

   // If available, set the range to target based on the engagement's range to target at launch.
   double targetLocationWCS[3];
   aEngagementPtr->GetTargetLocationAtLaunchWCS(targetLocationWCS);
   if (UtVec3d::MagnitudeSquared(targetLocationWCS) > 0.0)
   {
      double targetVecWCS[3];
      UtVec3d::Subtract(targetVecWCS, targetLocationWCS, weaponLocationWCS);
      double range = UtVec3d::Magnitude(targetVecWCS);
      pduPtr->SetRange((float)range);
   }
   else
   {
      // This should only occur if there is no valid target at fire time.
      pduPtr->SetRange(0.0F);
   }

   pduPtr->SetQuantity(1);
   pduPtr->SetRate(0);
   if (weaponPtr != nullptr)
   {
      pduPtr->SetWarhead(GetWarheadEnum(weaponPtr));
   }

   // Give observers a chance to modify the data before the PDU is sent out:
   ProcessFirePDU(*pduPtr);

   WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));

   // If the weapon is being transfered to from local to external control then:
   // - Mark the weapon as externally controlled.
   // - Mark the engagement as externally controlled.
   // This should be done AFTER the Fire PDU has been sent.

   if (outgoingWeaponTransfer)
   {
      if ((mExtMilPtr->GetDebugWarfarePDU() & 2) != 0)
      {
         auto out = ut::log::debug() << "Transferring to external control.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "WeaponId: " << weaponEntityId;
         out.AddNote() << "Weapon type: " << weaponEntityType;
      }
      const_cast<WsfWeaponEngagement*>(aEngagementPtr)->SetIsExternallyControlled(true);
      if (weaponPlatformPtr != nullptr)
      {
         weaponPlatformPtr->SetIsExternallyControlled(true);
      }
   }
}

// ================================================================================================
// static
void WsfDisWeaponEvents::DirectedEnergyWeaponFired(double                         aSimTime,
                                                   const WsfDirectedEnergyWeapon* aWeaponPtr,
                                                   const WsfWeaponEngagement*     aEngagementPtr)
{
   const WsfDirectedEnergyWeapon::Beam* beamPtr = aWeaponPtr->GetBeam();
   if ((beamPtr == nullptr) || (aEngagementPtr == nullptr))
   {
      return;
   }
   if ((aEngagementPtr->IsExternallyControlled()) || (aEngagementPtr->GetFiringPlatform() == nullptr) ||
       ((aEngagementPtr->GetFiringPlatform() != nullptr) && (aEngagementPtr->GetFiringPlatform()->IsExternallyMoved())))
   {
      return;
   }

   // Send a DE Fire PDU.
   auto pduPtr = ut::make_unique<WsfDisDirectedEnergyFire>(mMilPtr->GetDis());

   WsfPlatform*  firingPlatformPtr = nullptr;
   DisEntityId   firingEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityId   targetEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityType weaponEntityType;
   WeaponEvent   weaponEvent;

   WsfDisPlatform* firingDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetFiringPlatformIndex());
   if (firingDisPlatformPtr != nullptr)
   {
      firingPlatformPtr = firingDisPlatformPtr->GetPlatform();
      firingEntityId    = firingDisPlatformPtr->GetEntityId();
   }

   if (aWeaponPtr != nullptr)
   {
      // This is an implicit weapon.
      WsfExtEntityType extType;
      WsfDisUtil::GetExtInterface(mInterfacePtr)->GetEntityType(aWeaponPtr, extType);
      weaponEntityType = WsfDisExt::ToDis(extType);
   }

   WsfDisPlatform* targetDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetTargetPlatformIndex());
   if (targetDisPlatformPtr != nullptr)
   {
      targetEntityId = targetDisPlatformPtr->GetEntityId();
   }

   // Determine if the PDU should be sent.

   if (firingPlatformPtr != nullptr)
   {
      // The weapon seems to be implicitly modeled (no weapon platform)... don't send if the shooter is externally controlled.
      if (firingPlatformPtr->IsExternallyControlled())
      {
         return;
      }
   }

   // Ensure the firing platform, weapon and target positions are up-to-date and have had at least one entity state sent.
   if (firingDisPlatformPtr != nullptr)
   {
      // Ensure the firing platform's position is up-to-date and an initial state PDU was sent
      // before sending this fire PDU.
      firingDisPlatformPtr->ForceUpdate(aSimTime);
   }
   if (targetDisPlatformPtr != nullptr)
   {
      // Ensure the target platform's position is up-to-date and an initial state PDU was sent
      // before sending this fire PDU.
      targetDisPlatformPtr->ForceUpdate(aSimTime);
   }

   // Firing Entity
   pduPtr->SetFiringEntity(firingEntityId);

   // Event Site and Id
   auto iter             = mWeaponEventList.find(aEngagementPtr->GetSerialNumber());
   bool foundWeaponEvent = (iter != mWeaponEventList.end());
   if (foundWeaponEvent)
   {
      weaponEvent = iter->second;
   }
   else
   {
      weaponEvent.mFiringDisEntityId = firingEntityId;
      weaponEvent.mTargetDisEntityId = targetEntityId;
      weaponEvent.mDisEventId.SetSite(Dis::GetSite());
      weaponEvent.mDisEventId.SetApplication(Dis::GetApplication());
      weaponEvent.mDisEventId.AssignNewNumber();
      mWeaponEventList[aEngagementPtr->GetSerialNumber()] = weaponEvent;
   }
   pduPtr->SetEvent(weaponEvent.mDisEventId);

   // Munition Id
   pduPtr->SetMunitionType(weaponEntityType);

   // Shot start time
   DisClockTime timeStamp;
   timeStamp.SetHour(0); // Assume small time steps...
   timeStamp.SetTimePastHour(DisTime::GetTimeStamp(aEngagementPtr->GetStartTime()));
   pduPtr->SetShotStartTime(timeStamp);

   // Cumulative shot time
   double cumulativeShotTime = 0.0;
   if (aWeaponPtr->GetEngagement() != nullptr)
   {
      cumulativeShotTime = aSimTime - aWeaponPtr->GetEngagement()->GetStartTime();
   }
   pduPtr->SetCumulativeShotTime((DisFloat32)cumulativeShotTime);

   // Aperture/emitter location in firing entity coordinates
   double weaponLocECS[3];
   aWeaponPtr->GetLocation(weaponLocECS);
   pduPtr->SetApertureLocation((DisFloat32)weaponLocECS[0], (DisFloat32)weaponLocECS[1], (DisFloat32)weaponLocECS[2]);

   if (beamPtr != nullptr)
   {
      // Aperture Diameter
      pduPtr->SetApertureDiameter((DisFloat32)beamPtr->GetApertureDiameter());

      // Wavelength
      pduPtr->SetWavelength((DisFloat32)beamPtr->GetWavelength());

      // PRF
      pduPtr->SetPulseRepetitionFrequency((DisFloat32)beamPtr->GetPulseRepetitionFrequency());

      // Pulse width
      pduPtr->SetPulseWidth((DisFloat32)beamPtr->GetPulseWidth());

      // Flags
      // If not currently in the event list; set the "on" flag.
      if (!foundWeaponEvent)
      {
         pduPtr->SetFlags(3); // weapon "on"
      }
      else if (aWeaponPtr->IsFiring())
      {
         pduPtr->SetFlags(1); // state change
      }
      else
      {
         pduPtr->SetFlags(2); // weapon "off"
      }

      // Pulse shape
      pduPtr->SetPulseShape(1); // "Gaussian?"

      // Precision aimpoint.
      DisDEPrecisionAimpointRecord* aimpointPtr = new DisDEPrecisionAimpointRecord();
      aimpointPtr->SetTargetEntityId(targetEntityId);

      // Target Spot location is target centroid location
      double targetOffsetECS[3];
      beamPtr->GetOffsetLocationECS(targetOffsetECS);
      aimpointPtr->SetBeamSpotShape(1); // Gaussian
      aimpointPtr->SetPeakIrradiance((DisFloat32)beamPtr->GetPeakIrradiance());
      aimpointPtr->SetBeamSpotCrossSectionSemiMajorAxis((DisFloat32)beamPtr->GetSemiMajorAxis());
      aimpointPtr->SetBeamSpotCrossSectionSemiMinorAxis((DisFloat32)beamPtr->GetSemiMinorAxis());
      aimpointPtr->SetBeamSpotCrossSectionOrientationAngle((DisFloat32)beamPtr->GetIncidenceAngle());
      aimpointPtr->SetTargetCompoundIdentifier(0);
      aimpointPtr->SetTargetSpotEntityLocation((DisFloat32)targetOffsetECS[0],
                                               (DisFloat32)targetOffsetECS[1],
                                               (DisFloat32)targetOffsetECS[2]);
      WsfPlatform* targetPlatformPtr = GetSimulation()->GetPlatformByIndex(aEngagementPtr->GetTargetPlatformIndex());

      if (targetPlatformPtr != nullptr)
      {
         double targetLocWCS[3];
         targetPlatformPtr->GetLocationWCS(targetLocWCS);
         double targetVecWCS[3];
         targetPlatformPtr->ConvertECSVectorToWCS(targetVecWCS, targetOffsetECS);
         UtVec3d::Add(targetLocWCS, targetLocWCS, targetVecWCS);
         aimpointPtr->SetTargetSpotWorldLocation(targetLocWCS[0], targetLocWCS[1], targetLocWCS[2]);

         aimpointPtr->SetTargetSpotVelocity(0.0, 0.0, 0.0);
         aimpointPtr->SetTargetSpotAcceleration(0.0, 0.0, 0.0);
      }
      pduPtr->AddDERecord(aimpointPtr);
   }

   // Save the weapon event for use by fire, damage status, detonate pdus.
   WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));

   if (!foundWeaponEvent)
   {
      // Send the standard fire PDU; it must be sent after the DE PDU.
      WeaponFiredP(aSimTime, aEngagementPtr);
   }
}

// static
void WsfDisWeaponEvents::DirectedEnergyWeaponHit(double                         aSimTime,
                                                 const WsfDirectedEnergyWeapon* aWeaponPtr,
                                                 const WsfWeaponEngagement*     aEngagementPtr)
{
   // Send an entity damage status pdu:
   // The target must be controlled by the local simulation in order to issue a pdu:
   if (aEngagementPtr == nullptr)
   {
      return;
   }
   WsfPlatform* targetPtr = GetSimulation()->GetPlatformByIndex(aEngagementPtr->GetTargetPlatformIndex());
   if ((targetPtr == nullptr) || targetPtr->IsExternallyControlled())
   {
      return;
   }
   auto            pduPtr = ut::make_unique<DisEntityDamageStatus>();
   DisEntityId     targetEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   WsfDisPlatform* targetDisPlatformPtr =
      WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetTargetPlatformIndex());
   if (targetDisPlatformPtr != nullptr)
   {
      targetEntityId = targetDisPlatformPtr->GetEntityId();
   }
   pduPtr->SetDamagedEntity(targetEntityId);

   // Damage Description Resource count
   const WsfDirectedEnergyWeapon::Beam* beamPtr = aWeaponPtr->GetBeam();

   if (beamPtr != nullptr)
   {
      DisDEDamageDescriptionRecord* descPtr = new DisDEDamageDescriptionRecord();
      double                        damageLocECS[3];
      beamPtr->GetOffsetLocationECS(damageLocECS);
      descPtr->SetDamageLocation((DisFloat32)damageLocECS[0], (DisFloat32)damageLocECS[1], (DisFloat32)damageLocECS[2]);

      descPtr->SetDamageDiameter((DisFloat32)beamPtr->GetSemiMinorAxis()); // providing the small "diameter"
      descPtr->SetTemperature(-1000.0);                                    // Don't know / not modeled.
      descPtr->SetComponentIdentification(1);                              // Don't know / generic entity structure.
      WsfWeaponEffects::Result result             = aEngagementPtr->GetTargetResult();
      DisEnum8                 disResult          = 0;
      DisEnum8                 visualDamageStatus = 0;
      if (result == WsfWeaponEffects::cDAMAGED)
      {
         disResult          = 2;  // medium damage
         visualDamageStatus = 16; // heavy charring (16)
      }
      else if (result == WsfWeaponEffects::cKILLED)
      {
         disResult          = 4;  // destroyed
         visualDamageStatus = 24; // one or more holes burned (24)
      }
      descPtr->SetComponentDamageStatus(disResult);
      descPtr->SetComponentVisualDamageStatus(visualDamageStatus);
      descPtr->SetComponentVisualSmokeColor(0); // No smoke (Don't know / not modeled)
      auto       iter = mWeaponEventList.find(aEngagementPtr->GetSerialNumber());
      DisEventId disEventId;
      if (iter != mWeaponEventList.end())
      {
         disEventId = iter->second.mDisEventId;
      }
      descPtr->SetFireEvent(disEventId);
      pduPtr->AddDDRecord(descPtr);
   }

   WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));
}

// ================================================================================================
void WsfDisWeaponEvents::ImplicitWeaponEndEngagement(double                     aSimTime,
                                                     WsfImplicitWeapon*         aImplicitWeapon,
                                                     const WsfWeaponEngagement* aEngagementPtr)
{
   auto iter = mWeaponEventList.find(aEngagementPtr->GetSerialNumber());
   if (iter != mWeaponEventList.end())
   {
      mWeaponEventList.erase(iter);
   }
}

// ================================================================================================
// static
void WsfDisWeaponEvents::WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr)
{
   if (aEngagementPtr == nullptr)
   {
      return;
   }
   if (aEngagementPtr->IsExternallyControlled())
   {
      return;
   }
   if ((aEngagementPtr->GetWeaponPlatform() != nullptr) &&
       (WsfDisUtil::GetExtInterface(mInterfacePtr)->IsPrivate(aEngagementPtr->GetWeaponPlatform())))
   {
      return;
   }

   WsfDisPlatform* disPlatformPtr;
   WsfPlatform*    firingPlatformPtr = nullptr;
   WsfPlatform*    targetPlatformPtr = nullptr;
   WsfPlatform*    weaponPlatformPtr = nullptr;
   DisEventId      disEventId;
   DisEntityId     firingEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityId     weaponEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityId     targetEntityId(DisEntityId::NO_SPECIFIC_ENTITY);
   DisEntityType   weaponEntityType;

   // Attempt to find the corresponding fire event.

   auto iter = mWeaponEventList.find(aEngagementPtr->GetSerialNumber());
   if (iter != mWeaponEventList.end())
   {
      disEventId       = iter->second.mDisEventId;
      firingEntityId   = iter->second.mFiringDisEntityId;
      weaponEntityId   = iter->second.mWeaponDisEntityId;
      weaponEntityType = iter->second.mWeaponDisEntityType;
      targetEntityId   = iter->second.mTargetDisEntityId;

      // The engagement is over if the weapon is explicit; remove it from the map.
      if (aEngagementPtr->GetWeaponPlatform() != nullptr)
      {
         mWeaponEventList.erase(iter);
      }
   }

   disPlatformPtr = WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetFiringPlatformIndex());
   if (disPlatformPtr != nullptr)
   {
      firingPlatformPtr = disPlatformPtr->GetPlatform();
      firingEntityId    = disPlatformPtr->GetEntityId();
   }

   disPlatformPtr = WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetWeaponPlatformIndex());
   if (disPlatformPtr != nullptr)
   {
      weaponPlatformPtr = disPlatformPtr->GetPlatform();
      weaponEntityId    = disPlatformPtr->GetEntityId();
      weaponEntityType  = disPlatformPtr->GetEntityType();
      // other applications can process the entity id per requirements
      ProcessWeaponTerminated(disPlatformPtr, firingEntityId);
   }
   disPlatformPtr = WsfDisUtil::GetDisPlatform(mInterfacePtr, aEngagementPtr->GetTargetPlatformIndex());
   if (disPlatformPtr != nullptr)
   {
      targetPlatformPtr = disPlatformPtr->GetPlatform();
      targetEntityId    = disPlatformPtr->GetEntityId();
   }

   // Determine if the PDU should be sent.

   if (weaponPlatformPtr != nullptr)
   {
      // The weapon is explicitly modeled ... don't send if the weapon is externally controlled.
      if (weaponPlatformPtr->IsExternallyControlled())
      {
         return;
      }
   }
   else if (firingPlatformPtr != nullptr)
   {
      // The weapon seems to be implicitly modeled (no weapon platform)... don't send if the shooter is externally controlled.
      if (firingPlatformPtr->IsExternallyControlled())
      {
         return;
      }
      // The firing platform is a dis mover (it has no control over the weapon; all logic don via incoming_weapon_transfer logic)
      if (firingPlatformPtr->IsExternallyMoved())
      {
         return;
      }
   }
   if ((weaponPlatformPtr != nullptr) && (weaponPlatformPtr->GetAuxDataConst().AttributeExists("suppress_detonation_pdu")) &&
       (weaponPlatformPtr->GetAuxDataConst().GetBool("suppress_detonation_pdu")))
   {
      return;
   }
   auto pduPtr = ut::make_unique<DisDetonation>(); // memory deleted by DIS interface or thread
   if (disEventId.GetSite() == 0)
   {
      disEventId.SetSite(Dis::GetSite());
      disEventId.SetApplication(Dis::GetApplication());
      disEventId.AssignNewNumber();
   }
   pduPtr->SetEvent(disEventId);

   const WsfWeapon* weaponPtr = aEngagementPtr->GetWeaponSystem();
   if (weaponPtr != nullptr)
   {
      pduPtr->SetWarhead(GetWarheadEnum(aEngagementPtr->GetWeaponSystem()));
   }

   pduPtr->SetFiringEntity(firingEntityId);
   pduPtr->SetTargetEntity(targetEntityId);
   pduPtr->SetWeaponEntity(weaponEntityId);
   pduPtr->SetWeaponType(weaponEntityType);

   // Set the detonation location and offset from target.
   if (weaponPlatformPtr != nullptr)
   {
      double weaponLocationWCS[3];
      double weaponVelocityWCS[3];

      aEngagementPtr->GetWeaponLocationWCS(weaponPlatformPtr, weaponLocationWCS);
      weaponPlatformPtr->GetVelocityWCS(weaponVelocityWCS);
      pduPtr->SetLocation(weaponLocationWCS[0], weaponLocationWCS[1], weaponLocationWCS[2]);
      pduPtr->SetVelocity(static_cast<float>(weaponVelocityWCS[0]),
                          static_cast<float>(weaponVelocityWCS[1]),
                          static_cast<float>(weaponVelocityWCS[2]));

      // Compute the location of the impact relative to the target.
      if (targetPlatformPtr != nullptr)
      {
         UtEntity tempTargetPlatform(*targetPlatformPtr);
         double   targetLocationWCS[3];
         aEngagementPtr->GetTargetLocationWCS(targetPlatformPtr, targetLocationWCS);
         tempTargetPlatform.SetLocationWCS(targetLocationWCS);
         double weaponLocationECS[3];
         tempTargetPlatform.ConvertWCSToECS(weaponLocationWCS, weaponLocationECS);
         // The weapon may declare a miss distance while not providing the target and weapon location
         // which results in the declared miss distance. The impact location vector that is derived from
         // the positions will be rescaled so it's length matches the declared miss distance.
         //
         // The DIS standard (1278.1, Sec 5.3.4.2i) indicates that it is this field that should be used
         // for damage assessment, so we want the length of the vector to be correct.
         double missDistance = aEngagementPtr->GetMissDistance(targetPlatformPtr);
         UtVec3d::Normalize(weaponLocationECS);
         UtVec3d::Multiply(weaponLocationECS, weaponLocationECS, missDistance);
         pduPtr->SetImpactLocation(static_cast<float>(weaponLocationECS[0]),
                                   static_cast<float>(weaponLocationECS[1]),
                                   static_cast<float>(weaponLocationECS[2]));
      }
   }
   else if (targetPlatformPtr != nullptr) // This is an implicit weapon.
   {
      // Provide target location and velocity as set by in the engagement as impact point
      double locWCS[3];
      targetPlatformPtr->GetLocationWCS(locWCS);
      double velWCS[3];
      aEngagementPtr->GetWeaponVelocityWCS(velWCS);
      pduPtr->SetLocation(locWCS[0], locWCS[1], locWCS[2]);
      pduPtr->SetVelocity(static_cast<float>(velWCS[0]), static_cast<float>(velWCS[1]), static_cast<float>(velWCS[2]));
   }

   // From DisDetonation.hpp, DisDetonationResult is a DisEnum8 enumeration.
   // The specific enumeration values are not listed in the header file,
   // so I list them here for convenience:
   // 0=Other, 1=EntityImpact, 2=EntityProximateDetonation, 3=GroundImpact, 4=GroundProximateDetonation
   // 5=Detonation, 6=NoneOrNoDetonationDud, 7=HEHitSmall, 8=HEHitMedium, 9=HEHitLarge, 10=ArmourPiercingHit
   // 11=DirtBlastSmall, 12=DirtBlastMedium, 13=DirtBlastLarge, 14=WaterBlastSmall, 15=WaterBlastMedium,
   // 16=WaterBlastLarge, 17=AirHit, 18=BuildingHitSmall, 19=BuildingHitMedium, 20=BuildingHitLarge,
   // 21=MineClearingLineCharge, 22=EnvironmentObjectImpact, 23=EnvironmentObjectProximateDetonation,
   // 24=WaterImpact, 25=AirBurst, 26=KillWithFragmentType1, 27=KillWithFragmentType2, 28=KillWithFragmentType3,
   // 29=KillWithFragmentType1AfterFlyoutFailure, 30=KillWithFragmentType2AfterFlyoutFailure,
   // 31=MissDueToFlyoutFailure, 32=MissDueToEndGameFailure, 33=MissDueToFlyoutAndEndGameFailure

   // The only ones we will use are listed below:

   WsfWeaponEffects::Result            leth = aEngagementPtr->GetTargetResult();
   WsfWeaponEngagement::GeometryResult geom = aEngagementPtr->GetGeometryResult();

   if (leth == WsfWeaponEffects::cKILLED)
   {
      pduPtr->SetDetonationResult(1); // EntityImpact, (or at least close enough).
   }
   else if (geom == WsfWeaponEngagement::cTARGET_IMPACT)
   {
      pduPtr->SetDetonationResult(1); // EntityImpact
   }
   else if (leth == WsfWeaponEffects::cDAMAGED)
   {
      pduPtr->SetDetonationResult(1); // EntityImpact
   }
   else if ((geom == WsfWeaponEngagement::cTARGET_PROXIMITY_GROUND_IMPACT) ||
            (geom == WsfWeaponEngagement::cTARGET_PROXIMITY_AIR_BURST)) // if shooting a track and not an actual target
                                                                        // if it impacts against incidental damage
                                                                        // report kill for a/a engagments
   {
      pduPtr->SetDetonationResult(2); // EntityProximateDetonation.
   }
   else
   {
      if ((weaponPlatformPtr == nullptr) || (weaponPlatformPtr->GetDamageFactor() < 1.0))
      {
         pduPtr->SetDetonationResult(5); // Detonation.
      }
      else
      {
         // Weapon is fully damaged
         pduPtr->SetDetonationResult(6); // None.
      }
   }

   // If 'simple_miss_reporting' is in effect then anything other than a direct hit is reported as a 6.
   // Some simulations (VISE in particular) have very trivial processing. (CR 9577)
   if (mInterfacePtr->mSimpleMissReporting && ((pduPtr->GetDetonationResult() != 1) || (leth != WsfWeaponEffects::cKILLED)))
   {
      pduPtr->SetDetonationResult(6);
   }

   // (JAJ) The following are used with the wsf_*/tests/dis_warfare test case.
   //       You can uncomment the desired lines to check the behavior of WsfDisDetonation under different conditions
   //       that other simulations may provide.
   // pduPtr->SetDetonationResult(1);
   // pduPtr->SetFiringEntity(DisEntityId());
   // pduPtr->SetWeaponEntity(DisEntityId());
   // pduPtr->SetWeaponType(DisEntityType());
   // pduPtr->SetTargetEntity(DisEntityId());
   // pduPtr->SetLocation(0.0, 0.0, 0.0);
   // pduPtr->SetVelocity(0.0F, 0.0F, 0.0F);
   // pduPtr->SetImpactLocation(0.0F, 0.0F, 0.0F);

   // Give observers a chance to modify data before the PDU is sent
   ProcessDetonationPDU(*pduPtr);

   WsfDisUtil::PutPdu(mInterfacePtr, aSimTime, std::move(pduPtr));
}

// ================================================================================================
//! Determine if the incoming weapon entity should be transferred and controlled locally.
//!
//! This is invoked by WsfDisFire::Process to determine the DIS Fire event is specifying a weapon that
//! has been defined to be transferred to local control and modeled locally.
// static
bool WsfDisWeaponEvents::InitiateWeaponTransfer(double aSimTime, DisFire* aPduPtr)
{
   std::string     failMessage          = "";
   WsfDisPlatform* firingDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, aPduPtr->GetFiringEntity());
   WsfDisPlatform* targetDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, aPduPtr->GetTargetEntity());
   // Firing platform must exist.
   if (firingDisPlatformPtr == nullptr)
   {
      failMessage = "Firing DIS Platform, ";
      std::stringstream failStr("");
      failStr << (int)aPduPtr->GetFiringEntity().GetSite() << ":";
      failStr << (int)aPduPtr->GetFiringEntity().GetApplication() << ":";
      failStr << (int)aPduPtr->GetFiringEntity().GetEntity();
      failMessage += failStr.str();
      ReportErrorMessage(failMessage);

      return false;
   }

   // The firing platform must be externally controlled.
   if ((!firingDisPlatformPtr->GetPlatform()->IsExternallyControlled()) &&
       (!firingDisPlatformPtr->GetPlatform()->IsExternallyMoved()))
   {
      return false;
   }

   // The weapon type must be specified.
   if (aPduPtr->GetWeaponType() == DisEntityType())
   {
      failMessage = "Weapon Entity Type not specified";
      ReportErrorMessage(failMessage);

      ut::log::warning() << "Weapon Entity type not specified.";
      return false;
   }

   // If the weapon platform exists, it must be externally controlled (we don't take over something that we already own).
   WsfDisPlatform* weaponDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, aPduPtr->GetWeaponEntity());
   if (weaponDisPlatformPtr != nullptr)
   {
      if ((!weaponDisPlatformPtr->GetPlatform()->IsExternallyControlled()) &&
          (!weaponDisPlatformPtr->GetPlatform()->IsExternallyMoved()))
      {
         failMessage = "Weapon platform with id of ";
         std::stringstream failStr("");
         failStr << (int)aPduPtr->GetWeaponEntity().GetEntity();
         failMessage += failStr.str();
         failMessage += " is being used, weapon is in flight";
         ReportErrorMessage(failMessage);

         return false;
      }
   }

   // Determine if a transfer for the specified shooter/weapon should be initiated.

   const DisEntityId&                                       firingEntityId(aPduPtr->GetFiringEntity());
   const DisEntityType&                                     weaponEntityType(aPduPtr->GetWeaponType());
   const WsfMilDisInterface::IncomingTransferList&          incomingTransfers = mMilPtr->GetIncomingTransferList();
   WsfMilDisInterface::IncomingTransferList::const_iterator bestEntryIter     = incomingTransfers.end();
   for (auto wtli = incomingTransfers.begin(); wtli != incomingTransfers.end(); ++wtli)
   {
      const WsfMilDisInterface::IncomingTransfer& wt = *wtli;

      // Check if transfer is restricted based on firing entity ID.
      if (!wt.mFiringDisEntityId.IsDefined()) // At least one field is non-zero
      {
         if ((wt.mFiringDisEntityId.GetSite() != 0) && (wt.mFiringDisEntityId.GetSite() != firingEntityId.GetSite()))
         {
            continue;
         }
         if ((wt.mFiringDisEntityId.GetApplication() != 0) &&
             (wt.mFiringDisEntityId.GetApplication() != firingEntityId.GetApplication()))
         {
            continue;
         }
         if ((wt.mFiringDisEntityId.GetEntity() != 0) && (wt.mFiringDisEntityId.GetEntity() != firingEntityId.GetEntity()))
         {
            continue;
         }
      }

      if (wt.mWeaponDisEntityType == weaponEntityType)
      {
         bestEntryIter = wtli;
      }
   }

   if (bestEntryIter == incomingTransfers.end())
   {
      return false;
   }

   // This fire event has been determined to be one that should initiate a transfer. If the firing platform already
   // has a weapon of the correct type then it will be used. Otherwise we will allocate a weapon of the proper type.

   bool debugTransfer = ((mExtMilPtr->GetDebugWarfarePDU() & 2) != 0);
   if (debugTransfer)
   {
      auto out = ut::log::debug() << "Begin incoming weapon transfer.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Weapon: " << aPduPtr->GetWeaponEntity();
      out.AddNote() << "Weapon Type: " << aPduPtr->GetWeaponType();
   }

   const WsfMilDisInterface::IncomingTransfer& transferData = *bestEntryIter;

   WsfPlatform* firingPlatformPtr = firingDisPlatformPtr->GetPlatform();

   WsfWeapon* weaponPtr = nullptr;
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*firingPlatformPtr); !iter.AtEnd(); ++iter)
   {
      if (iter->GetTypeId() == transferData.mWeaponSystemType)
      {
         weaponPtr = *iter;
         break;
      }
   }
   if (weaponPtr == nullptr)
   {
      // Weapon not found - add it to the firing platform.

      std::string weaponSystemType(transferData.mWeaponSystemType);
      weaponPtr = WsfWeaponTypes::Get(mMilPtr->GetScenario()).Clone(transferData.mWeaponSystemType);
      if (weaponPtr == nullptr)
      {
         failMessage = "Unable to clone weapon type requested type not present ";
         ReportErrorMessage(failMessage);
         auto out = ut::log::error() << "Incoming weapon transfer. Unable to clone weapon type.";
         out.AddNote() << "Weapon System Type: " << weaponSystemType;
         return false;
      }

      weaponPtr->SetName(weaponSystemType); // TODO-need to make this unique???
      if (!firingPlatformPtr->AddComponent(weaponPtr))
      {
         failMessage = "Unable to add weapon object";
         ReportErrorMessage(failMessage);
         auto out = ut::log::error() << "Incoming weapon transfer. Unable to add weapon object.";
         out.AddNote() << "Weapon System Type: " << weaponSystemType;
         delete weaponPtr;
         return false;
      }

      if (!weaponPtr->Initialize(aSimTime))
      {
         failMessage = "Unable to initialize weapon object";
         ReportErrorMessage(failMessage);
         auto out = ut::log::error() << "Incoming weapon transfer. Unable to initialize weapon object.";
         out.AddNote() << "Weapon System Type: " << weaponSystemType;
         return false;
      }
   }
   /* ASCII OUT
   // Allow the external platform to preassigned id attributes to the weapon platform.
   int entity = 0;
   if (aPduPtr->GetWeaponEntity().GetEntity() > 0)
   {
      entity = (int)aPduPtr->GetWeaponEntity().GetEntity();
      SetWeaponId(weaponPtr, entity);
   }
   * END ASCII OUT */

   // Create a track to be associated with the firing. Technically one isn't needed if there isn't
   // a target, but we need one to get a track ID for the pending transfer.

   std::unique_ptr<WsfTrack> trackPtr(new WsfTrack());
   firingPlatformPtr->Update(aSimTime);
   trackPtr->SetTrackType(WsfTrack::cPROCESSED);
   trackPtr->SetFusible(true);
   trackPtr->Initialize(aSimTime, firingPlatformPtr->GetNextTrackId(), *GetSimulation());
   trackPtr->Update(aSimTime);
   double originWCS[3];
   firingPlatformPtr->GetLocationWCS(originWCS);
   trackPtr->SetPlatformOriginator(firingPlatformPtr);
   trackPtr->SetOriginatorLocationWCS(originWCS);

   // If a target was specified then populate the track with the information.

   if ((targetDisPlatformPtr != nullptr) && (targetDisPlatformPtr->GetPlatform() != nullptr))
   {
      WsfPlatform* targetPlatformPtr = targetDisPlatformPtr->GetPlatform();
      targetPlatformPtr->Update(aSimTime);
      trackPtr->SetSpatialDomain(targetPlatformPtr->GetSpatialDomain());

      // Get and set location
      double locWCS[3];
      targetPlatformPtr->GetLocationWCS(locWCS);
      trackPtr->SetLocationWCS(locWCS);
      trackPtr->SetWCS_LocationValid(true);
      trackPtr->Set3D(true);

      // Get and set velocity
      double velWCS[3] = {0.0};
      targetPlatformPtr->GetVelocityWCS(velWCS);
      if (UtVec3d::MagnitudeSquared(velWCS) == 0.0)
      {
         // Create a velocity vector that is small that will allow the user to recover any heading.
         // Pitch and roll are assumed to be zero.
         double heading;
         double pitch;
         double roll;
         targetPlatformPtr->GetOrientationNED(heading, pitch, roll);
         const double speed = 0.001;
         double       velNED[3];
         velNED[0] = speed * cos(heading);
         velNED[1] = speed * sin(heading);
         velNED[2] = 0.0;
         targetPlatformPtr->ConvertNEDVectorToWCS(velWCS, velNED);
      }
      trackPtr->SetVelocityWCS(velWCS);
      trackPtr->SetVelocityValid(true);

      // Set type and side
      trackPtr->SetTypeId(targetPlatformPtr->GetTypeId());
      trackPtr->SetTypeIdValid(true);
      trackPtr->SetSideId(targetPlatformPtr->GetSideId());
      trackPtr->SetSideIdValid(true);

      // Set truth information
      trackPtr->SetTargetIndex(targetPlatformPtr->GetIndex());
      trackPtr->SetTargetName(targetPlatformPtr->GetNameId());
      trackPtr->SetTargetType(targetPlatformPtr->GetTypeId());
   }
   else
   {
      // Allow a ground point location to get set for the track.
      WsfGeoPoint groundPt;
      SetTrackLocationData(groundPt, (int)aPduPtr->GetWeaponEntity().GetEntity());
      double lat;
      double lon;
      double alt;
      groundPt.GetLocationLLA(lat, lon, alt);
      double ptWCS[3];
      UtEntity::ConvertLLAToWCS(lat, lon, alt, ptWCS);
      trackPtr->SetLocationLLA(lat, lon, alt);
      trackPtr->SetLocationWCS(ptWCS);
      trackPtr->SetVelocityValid(true);
      trackPtr->SetLLA_LocationValid(true);
      trackPtr->SetWCS_LocationValid(true);
      double velWCS[3] = {0.0};
      trackPtr->SetVelocityWCS(velWCS);
      trackPtr->SetSpatialDomain(WSF_SPATIAL_DOMAIN_LAND);
      trackPtr->Set3D(true);
      trackPtr->SetTrackType(WsfTrack::cPREDEFINED);
      trackPtr->SetFusible(true);
   }

   // Force the quantity to ensure that doesn't limit the firing.
   weaponPtr->SetQuantityRemaining(100.0);

   // When Fire() is called, a platform representing the weapon should be created and added to the simulation.
   // This causes WsfDisInterface::AddPlatform() to be called (via the simulation observer PlatformAdded).
   // In order to assign the correct entity type and entity Id to the platform, we need to store this data so
   // it can be retrieved.

   PendingTransfer pt;
   pt.mFiringStartTime     = aSimTime;
   pt.mFiringPlatformIndex = firingDisPlatformPtr->GetPlatform()->GetIndex();
   pt.mTargetTrackId       = trackPtr->GetTrackId();
   pt.mWeaponSystemName    = weaponPtr->GetNameId();
   pt.mWeaponDisEntityId   = aPduPtr->GetWeaponEntity();
   pt.mWeaponDisEntityType = aPduPtr->GetWeaponType();
   mPendingTransferList.push_back(pt);

   WsfWeapon::FireOptions fireSettings;
   fireSettings.mWeaponId       = (int)aPduPtr->GetWeaponEntity().GetEntity();
   WsfWeapon::FireResult result = weaponPtr->Fire(aSimTime, trackPtr.get(), fireSettings);
   if (!(bool)result)
   {
      failMessage = "Local weapon firing failed";
      ReportErrorMessage(failMessage);
      ut::log::error() << "Incoming weapon transfer. Local weapon firing failed.";
      return false;
   }
   if (result.mEngagementPtr == nullptr)
   {
      failMessage = "Weapon engagement pointer is zero after weapon firing";
      ReportErrorMessage(failMessage);
      ut::log::error() << "Incoming weapon transfer. Weapon engagement pointer is zero after weapon firing.";
      return false;
   }

   // Save the fire mission index in the aux data on the weapon; needed for J11 processing
   // Will be used as the weapon track number if > 0 (NO_FIRE_MISSION)
   //
   // NOTE: The weapon platform may not exist after the fire (because of a mishandled SIMS deferred launch)
   //       and we don't want to segfault if that happens.
   WsfPlatform* weaponPlatformPtr = result.mEngagementPtr->GetWeaponPlatform();
   if (weaponPlatformPtr != nullptr)
   {
      (weaponPlatformPtr->GetAuxData()).Assign("fire_mission_index", (int)aPduPtr->GetFireMissionIndex());
   }

   // Create the 'weapon event' object needed for the detonation PDU.

   WeaponEvent weaponEvent;
   weaponEvent.mDisEventId = aPduPtr->GetEvent();
   if (weaponEvent.mDisEventId.GetNumber() == 0)
   {
      // TODO do we want to do this???? (Should we just leave what was passed
      weaponEvent.mDisEventId.SetSite(Dis::GetSite());
      weaponEvent.mDisEventId.SetApplication(Dis::GetApplication());
      weaponEvent.mDisEventId.AssignNewNumber();
   }

   weaponEvent.mFiringDisEntityId   = firingDisPlatformPtr->GetEntityId();
   weaponEvent.mWeaponDisEntityId   = aPduPtr->GetWeaponEntity();
   weaponEvent.mWeaponDisEntityType = aPduPtr->GetWeaponType();
   if (targetDisPlatformPtr != nullptr) // target may be a location
   {
      weaponEvent.mTargetDisEntityId = targetDisPlatformPtr->GetEntityId();
   }
   mWeaponEventList[result.mEngagementPtr->GetSerialNumber()] = weaponEvent;

   return true;
}

// ================================================================================================
//! Transfer the platform being created from internal to external control.
//!
//! This is invoked by WsfDisInterface::AddPlatform to transfer control of the newly
//! created platform to some external application.
//!
//! @param aSimTime    The current simulation time.
//! @param aPlatformPtr The platform to be transferred from local to external control.
//!
//! @note This is indirectly called via the PlatformAdded observer interface. The platform
//! to be transferred must have not yet been initialized because platform subsystems are
//! being deleted.
//!
//! @note This does not mark the platform as externally controlled. This is not done until
//! after the Fire PDU is issued because the platform must remain under internal control
//! so the initial PDU gets sent out.
bool WsfDisWeaponEvents::TransferPlatformToExternalControl(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Don't transfer unless the weapon is selected by 'outbound_weapon_transfer'.
   if (!IsOutgoingWeaponTransfer(aSimTime, aPlatformPtr))
   {
      return false;
   }

   WsfExtInterface::StripExternalPlatform(aPlatformPtr);

   // Attach the DIS mover to this platform.  It will simply extrapolate from the last entity state.
   WsfDisMover* moverPtr = new WsfDisMover(mInterfacePtr, aPlatformPtr, nullptr);
   aPlatformPtr->SetMover(moverPtr);
   aPlatformPtr->DeleteComponent<WsfFuel>();

   // In concept this would seem to be a good place to deleted any associated weapon effects/weapon engagement
   // data. DON'T DO IT - IT WILL CAUSE A SEGFAULT ON AN INBOUND WEAPON TRANSFER. The engagement pointer needs
   // to stay intact because it needs to be passed to the WeaponFired observers.

   return true;
}

// ================================================================================================
//! Transfer the platform being created from external to internal control.
//!
//! This is invoked by WsfDisInterface::AddPlatform to transfer control of the platform being created
//! to internal control. This really isn't so much a transfer as it is making the new platform assume
//! the identity of a platform that may already exist as an externally control platform.
//!
//! @param aSimTime       [input]  The current simulation time.
//! @param aPlatformPtr   [input]  The platform to be transferred from external to local control.
//! @param aEntityId      [output] The DIS Entity Id to be assigned to the platform IF the method return value is true.
//! @param aEntityType    [output] The DIS Entity Type to be assigned to the platform IF the method return value is true.
//!
//! @note This is indirectly called via the PlatformAdded observer interface.
// static
bool WsfDisWeaponEvents::TransferPlatformToInternalControl(double         aSimTime,
                                                           WsfPlatform*   aPlatformPtr,
                                                           DisEntityId&   aEntityId,
                                                           DisEntityType& aEntityType)
{
   if (aPlatformPtr == nullptr)
   {
      return false;
   }

   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*aPlatformPtr);
   if (engagementPtr == nullptr)
   {
      return false; // Must only be a weapon transfer
   }

   // Search for the pending transfer created by InitiateWeaponTransfer (via the Fire PDU).
   PendingTransferList::iterator ptli;
   for (ptli = mPendingTransferList.begin(); ptli != mPendingTransferList.end(); ++ptli)
   {
      PendingTransfer& pt = *ptli;
      if ((fabs(engagementPtr->GetStartTime() - pt.mFiringStartTime) < 0.01) &&
          (engagementPtr->GetFiringPlatformIndex() == pt.mFiringPlatformIndex) &&
          (engagementPtr->GetInitialTargetTrackId() == pt.mTargetTrackId) &&
          (engagementPtr->GetWeaponSystemName() == pt.mWeaponSystemName))
      {
         break;
      }
   }
   if (ptli == mPendingTransferList.end())
   {
      return false; // Not a pending transfer
   }

   // Capture the DIS entity ID and Entity Type to be assigned to the platform and remove
   // the pending transfer entry.

   aEntityId   = (*ptli).mWeaponDisEntityId;
   aEntityType = (*ptli).mWeaponDisEntityType;
   mPendingTransferList.erase(ptli);

   // If the desired entity ID is already in use then we must delete the existing platform that
   // is using the ID so we can create the local platform that has all the desired subsystems.

   WsfDisPlatform* oldDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, aEntityId);
   if (oldDisPlatformPtr != nullptr)
   {
      // Indicate the old associated DIS platform has been replaced. This prevents updating data that is dependent
      // on entity ID when the old platform is actually deleted (remember that this new platform is assuming the
      // entity ID of the old platform).
      oldDisPlatformPtr->SetHasBeenReplaced(true);
      WsfPlatform* oldPlatformPtr = oldDisPlatformPtr->GetPlatform();
      GetSimulation()->DeletePlatform(aSimTime, oldPlatformPtr);
   }

   return true;
}

// ================================================================================================
//! Is the specified platform selected by an 'outbound_weapon_transfer' command.
//!
//! @param aSimTime   The current simulation time.
//! @param aPlatformPtr The platform to be tested.
//! @returns 'true' if the platform was selected for transfer via an 'outbound_weapon_transfer' command.
bool WsfDisWeaponEvents::IsOutgoingWeaponTransfer(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr == nullptr)
   {
      return false;
   }

   // The platform must be part of a weapon engagement.
   WsfWeaponEngagement* engagementPtr = WsfWeaponEngagement::Find(*aPlatformPtr);
   if (engagementPtr == nullptr)
   {
      return false;
   }

   // ... and it must be internally controlled (at least for now).
   if (aPlatformPtr->IsExternallyControlled())
   {
      return false;
   }

   // Determine if the weapon system from which the platform was launched is mentioned in an
   // 'outgoing_weapon_transfer' command,

   const WsfWeapon* weaponPtr = engagementPtr->GetWeaponSystem();
   if (weaponPtr == nullptr)
   {
      return false;
   }
   bool                        isOutgoingTransfer = false;
   const OutgoingTransferList& outgoingTransfers  = mMilPtr->GetOutgoingTransferList();
   for (const OutgoingTransfer& transfer : outgoingTransfers)
   {
      if (transfer.mWeaponSystemType == weaponPtr->GetTypeId())
      {
         isOutgoingTransfer = true;
         break;
      }
   }
   return isOutgoingTransfer;
}

WsfSimulation* WsfDisWeaponEvents::GetSimulation() const
{
   return &mInterfacePtr->GetSimulation();
}
