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

#include "dis/WsfDisDirectedEnergyFire.hpp"

#include <sstream>

#include "Dis.hpp"
#include "DisDEPrecisionAimpointRecord.hpp"
#include "DisDirectedEnergyAimpoint.hpp"
#include "DisIO.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfMilExtInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfWeapon.hpp"
#include "WsfWeaponEngagement.hpp"
#include "WsfWeaponTypes.hpp"
#include "dis/WsfDisExt.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "dis/WsfDisWeaponEvents.hpp"

WsfDisDirectedEnergyFire::WsfDisDirectedEnergyFire(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDirectedEnergyFire(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
   mMilInterfacePtr = static_cast<WsfMilDisInterface*>(
      mInterfacePtr->GetComponents().FindComponentByRole(cCOMPONENT_ROLE<WsfMilDisInterface>()));
   mExtMilPtr = WsfMilExtInterface::Find(*WsfDisUtil::GetExtInterface(aInterfacePtr));
}

WsfDisDirectedEnergyFire::WsfDisDirectedEnergyFire(WsfDisInterface* aInterfacePtr)
   : DisDirectedEnergyFire()
   , mInterfacePtr(aInterfacePtr)
{
   mMilInterfacePtr = static_cast<WsfMilDisInterface*>(
      mInterfacePtr->GetComponents().FindComponentByRole(cCOMPONENT_ROLE<WsfMilDisInterface>()));
   mExtMilPtr = WsfMilExtInterface::Find(*WsfDisUtil::GetExtInterface(aInterfacePtr));
}

//! Are we currently processing directed energy fire events?
bool WsfDisDirectedEnergyFire::Exists(const DisEventId& aEventId) const
{
   return (mMilInterfacePtr->FindImpicitWeaponEngagement(aEventId) != nullptr);
}

// virtual
int WsfDisDirectedEnergyFire::Process()
{
   double pduTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);
   double simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   WsfPlatform* firingPlatformPtr = nullptr;
   WsfPlatform* targetPlatformPtr = nullptr;

   WsfDisPlatform*          firingDisPlatformPtr    = WsfDisUtil::FindDisPlatform(mInterfacePtr, GetFiringEntity());
   WsfDirectedEnergyWeapon* directedEnergyWeaponPtr = mMilInterfacePtr->FindDirectedEnergyWeapon(GetEvent());

   if (firingDisPlatformPtr != nullptr)
   {
      firingPlatformPtr = firingDisPlatformPtr->GetPlatform();
      firingPlatformPtr->Update(simTime);
   }

   bool ignorePdu = (firingPlatformPtr == nullptr) || (!firingPlatformPtr->IsExternallyControlled());
   if (ignorePdu)
   {
      auto out = ut::log::info() << "Ignoring DE Fire PDU:";
      if (firingPlatformPtr != nullptr)
      {
         // Don't process this PDU if it was sent by ourself.
         out.AddNote() << "Sent by self.";
      }
      else
      {
         // We must have a firing platform in order to process the pdu.
         // This can happen in rare circumstances and would lead to a crash.
         out.AddNote() << "No valid firing platform.";
      }
      return 1; // delete the PDU
   }

   const DisEntityType& munitionType = GetMunitionType();
   WsfStringId          weaponType(
      mExtMilPtr->SelectWeaponType(WsfDisUtil::GetSimulation(mInterfacePtr), WsfDisExt::ToExt(munitionType)));

   // Retrieve the weapon effect type from the prototype weapon.
   WsfStringId weaponEffectsType;
   WsfWeapon*  weaponPtr = WsfWeaponTypes::Get(WsfDisUtil::GetScenario(mInterfacePtr)).Find(weaponType);

   if (weaponPtr != nullptr)
   {
      weaponEffectsType = weaponPtr->GetWeaponEffectsType();
   }

   // Attempt to clone the weapon if we don't already have it.
   if ((directedEnergyWeaponPtr == nullptr) && (firingPlatformPtr != nullptr))
   {
      auto dew                = ut::make_unique<WsfDirectedEnergyWeapon>(WsfDisUtil::GetScenario(mInterfacePtr));
      directedEnergyWeaponPtr = dew.get();
      WsfDirectedEnergyWeapon::Beam* beamPtr = new WsfDirectedEnergyWeapon::Beam();
      directedEnergyWeaponPtr->SetBeam(beamPtr);

      // Need a unique name for this temporary weapon object.
      std::ostringstream oss;
      oss << weaponType << "_" << GetEvent().GetSite() << ":" << GetEvent().GetApplication() << ":"
          << GetEvent().GetNumber();
      directedEnergyWeaponPtr->SetName(oss.str());
      directedEnergyWeaponPtr->SetPlatform(firingPlatformPtr);
      directedEnergyWeaponPtr->Initialize(simTime);
      mMilInterfacePtr->AddDirectedEnergyWeapon(std::move(dew), GetEvent());
   }

   { // RAII block
      auto out = ut::log::info() << "Weapon clone report:";
      out.AddNote() << "Platform: " << firingPlatformPtr->GetName();
      out.AddNote() << "Internal/External controlled: "
                    << (firingPlatformPtr->IsExternallyControlled() ? ", External" : ", Internal");
   }

   // Get the firing location. Use the PDU value if specified or the firing platform or weapon platform.

   double firingLocWCS[3];

   // Get the aperture/emitter location
   DisFloat32 apertureLocX;
   DisFloat32 apertureLocY;
   DisFloat32 apertureLocZ;
   GetApertureLocation(apertureLocX, apertureLocY, apertureLocZ);
   double apertureLocECS[3] = {apertureLocX, apertureLocY, apertureLocZ};
   if (!UtVec3d::Equals(apertureLocECS, 0.0))
   {
      firingPlatformPtr->ConvertECSToWCS(apertureLocECS, firingLocWCS);
   }
   else
   {
      firingPlatformPtr->GetLocationWCS(firingLocWCS);
   }

   double lat;
   double lon;
   double alt;
   WsfPlatform::ConvertWCSToLLA(firingLocWCS, lat, lon, alt);

   // Print PDU information to cout if requested.
   if ((mExtMilPtr->GetDebugWarfarePDU() & 1) != 0)
   {
      auto out = ut::log::debug() << "Directed Energy Fire PDU received.";
      out.AddNote() << "T = " << simTime;
      out.AddNote() << "Tpdu: " << pduTime;
      out.AddNote() << "Firing Entity: " << GetFiringEntity();
      out.AddNote() << "Munition Type: " << GetMunitionType();
      out.AddNote() << "Event ID: " << GetEvent();
      out.AddNote() << "Weapon Type: " << weaponType;
      out.AddNote() << "Platform Type: " << firingPlatformPtr->GetType();

      auto note = out.AddNote() << "Fire Loc LLA:";
      note.AddNote() << "LatDMS: " << UtLatPos(lat, UtAngle::FmtDMS + 2);
      note.AddNote() << "LonDMS: " << UtLonPos(lon, UtAngle::FmtDMS + 2);
      note.AddNote() << "Alt: " << alt << " m (" << alt * UtMath::cFT_PER_M << " ft)";
      note.AddNote() << "LatD: " << UtLatPos(lat, UtAngle::FmtD + 4);
      note.AddNote() << "LonD: " << UtLonPos(lon, UtAngle::FmtD + 4);
   }

   // Get the DE aimpoint record, if applicable
   auto                          deRecordIter(GetDEBegin());
   bool                          canProcess           = false;
   DisDEPrecisionAimpointRecord* precisionAimpointPtr = nullptr;

   while (deRecordIter != GetDEEEnd())
   {
      DisDERecord* deRecord = *deRecordIter;

      // TODO This is very much set up assuming we only receive one Precision Aimpoint record
      if (deRecord->GetRecordType() == DisDERecord::PRECISION_AIMPOINT_DE_RECORD_TYPE)
      {
         canProcess           = true;
         precisionAimpointPtr = dynamic_cast<DisDEPrecisionAimpointRecord*>(deRecord);
      }

      if (!canProcess)
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::warning() << "DisDirectedEnergyFire PDU Record Type not supported (ignoring).";
            out.AddNote() << "Record Type: " << deRecord->GetRecordType();
         }
         return 1;
      }
      ++deRecordIter;
   }

   // We *Should* have a precision aimpoint in order to complete calculations;
   // if not, give up and let the fire/detonate pdus do the job.
   if (precisionAimpointPtr == nullptr)
   {
      return 0;
   }

   const DisEntityId& targetId             = precisionAimpointPtr->GetTargetEntityId();
   WsfDisPlatform*    targetDisPlatformPtr = WsfDisUtil::FindDisPlatform(mInterfacePtr, targetId);
   if (targetDisPlatformPtr != nullptr)
   {
      targetPlatformPtr = targetDisPlatformPtr->GetPlatform();
   }
   if (targetPlatformPtr == nullptr) // Must have a target platform to initiate an engagement
   {
      return 0;
   }

   // Fill beam parameters
   WsfDirectedEnergyWeapon::Beam* beamPtr = directedEnergyWeaponPtr->GetBeam();
   if (beamPtr == nullptr) // Must have a beam to initiate an engagement
   {
      return 0;
   }

   beamPtr->SetApertureDiameter(GetApertureDiameter());
   beamPtr->SetWavelength(GetWavelength());
   beamPtr->SetPulseRepetitionFrequency(GetPulseRepetitionFrequency());
   beamPtr->SetPulseWidth(GetPulseWidth());
   beamPtr->SetPeakIrradiance(precisionAimpointPtr->GetPeakIrradiance());
   beamPtr->SetIncidenceAngle(precisionAimpointPtr->GetBeamSpotCrossSectionOrientationAngle());
   beamPtr->SetSpotRadius(precisionAimpointPtr->GetBeamSpotCrossSectionSemiMinorAxis());

   // Now, should call an update based on the cum. shot time (perhaps a method called "integrate")
   // beamPtr->Integrate(GetCumulativeDuration());
   // Engagement
   WsfWeaponEngagement* engagementPtr = nullptr;

   if ((GetFlags() & 1) == 1) // "on" state flag
   {
      // Start the engagement based on the current sim time and the sent cumulative duration
      // (with any luck, the cum. duration on first send will be zero).
      double engagementStartTime = std::max(0.0, (pduTime - (double)GetCumulativeShotTime()));
      engagementPtr              = new WsfWeaponEngagement(engagementStartTime,
                                              directedEnergyWeaponPtr->GetSimulation(),
                                              directedEnergyWeaponPtr,
                                              WsfTrackId(),
                                              targetPlatformPtr->GetIndex(),
                                              weaponEffectsType);
      beamPtr->SetEnergy(0.0);
      engagementPtr->SetIsExternallyControlled(true);
      WsfWeaponEngagement::Assign(*firingPlatformPtr, engagementPtr);
      engagementPtr->Initialize(simTime, firingPlatformPtr);
      mMilInterfacePtr->AddImplicitWeaponEngagement(engagementPtr, GetEvent());
   }
   else // Attempt to find existing engagement
   {
      engagementPtr = mMilInterfacePtr->FindImpicitWeaponEngagement(GetEvent());
   }

   if (engagementPtr != nullptr)
   {
      // What is time since beginning of shot.
      // double elapsedTime = engagementPtr->GetElapsedTime(simTime);
      double elapsedTimePdu = GetCumulativeShotTime();
      double dt             = elapsedTimePdu;
      if (dt > 0.0)
      {
         beamPtr->SetEnergy(beamPtr->GetEnergy() + precisionAimpointPtr->GetPeakIrradiance() * UtMath::cPI *
                                                      beamPtr->GetSemiMajorAxis() * beamPtr->GetSemiMinorAxis() * dt);
      }
      engagementPtr->Update(simTime); // Calls implicit weapon effects update.

      if ((GetFlags() % 2) == 0) // "off" state flag
      {
         ut::log::info() << "Received last directed energy fire for this engagement.";
         directedEnergyWeaponPtr->CeaseFire(simTime); // Ends the engagement
         firingPlatformPtr->DeleteComponent<WsfWeapon>(directedEnergyWeaponPtr->GetNameId());
         mMilInterfacePtr->DeleteDirectedEnergyWeapon(GetEvent());
      }
   }
   else
   {
      ut::log::warning() << "No engagement object.";
   }

   return 1;
}
